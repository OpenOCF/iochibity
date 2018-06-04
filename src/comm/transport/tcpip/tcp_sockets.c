/* ****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#include "tcp_sockets.h"

#include "utlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <errno.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

/**
 * Logging tag for module name.
 */
#define TAG "TCPSOCK"

/* tcp globals, from CAGlobals_t in _globals.h */
#ifndef TCP_V6
#define TCP_V6 0
#endif
CASocket_t tcp_socket_ipv6  = { .fd = OC_INVALID_SOCKET, .port = TCP_V6 }; /**< IPv6 accept socket */

#ifndef TCP_V6S
#define TCP_V6S 0
#endif
CASocket_t tcp_socket_ipv6s = { .fd = OC_INVALID_SOCKET, .port = TCP_V6S }; /**< IPv6 secure accept */

#ifndef TCP_V4
#define TCP_V4 0
#endif
CASocket_t tcp_socket_ipv4  = { .fd = OC_INVALID_SOCKET, .port = TCP_V4 };   /**< IPv4 accept */

#ifndef TCP_V4S
#define TCP_V4S 0
#endif
CASocket_t tcp_socket_ipv4s = { .fd = OC_INVALID_SOCKET, .port = TCP_V4S };  /**< IPv4 secure accept */

int tcp_shutdownFds[2];     /**< shutdown pipe */
int tcp_connectionFds[2];   /**< connection pipe */
CASocketFd_t tcp_maxfd;     /**< highest fd (for select) */

void *tcp_threadpool;       /**< threadpool between Initialize and Start */

bool tcp_client;
bool tcp_server;

#define CA_TCP_LISTEN_BACKLOG  3
#define CA_TCP_SELECT_TIMEOUT 10

int tcp_selectTimeout = CA_TCP_SELECT_TIMEOUT; /* in seconds */
int tcp_listenBacklog = CA_TCP_LISTEN_BACKLOG;


bool tcp_is_started;           /**< the TCP adapter has started */
volatile bool tcp_is_terminating;/**< the TCP adapter needs to stop */
bool tcp_is_ipv4_enabled = true; // ipv4tcpenabled;    /**< IPv4 TCP enabled by OCInit flags */
bool tcp_is_ipv6_enabled = true; //ipv6tcpenabled;    /**< IPv6 TCP enabled by OCInit flags */

/* static void CAInitializeTCPGlobals() */
/* { */
/*     caglobals.tcp.ipv4.fd = OC_INVALID_SOCKET; */
/*     caglobals.tcp.ipv4s.fd = OC_INVALID_SOCKET; */
/*     caglobals.tcp.ipv6.fd = OC_INVALID_SOCKET; */
/*     caglobals.tcp.ipv6s.fd = OC_INVALID_SOCKET; */

/*     // Set the port number received from application. */
/*     caglobals.tcp.ipv4.port = caglobals.ports.tcp.u4; */
/*     caglobals.tcp.ipv4s.port = caglobals.ports.tcp.u4s; */
/*     caglobals.tcp.ipv6.port = caglobals.ports.tcp.u6; */
/*     caglobals.tcp.ipv6s.port = caglobals.ports.tcp.u6s; */

/*     caglobals.tcp.selectTimeout = CA_TCP_SELECT_TIMEOUT; */
/*     caglobals.tcp.listenBacklog = CA_TCP_LISTEN_BACKLOG; */

/*     CATransportFlags_t flags = 0; */
/*     if (caglobals.client) */
/*     { */
/*         flags |= caglobals.clientFlags; */
/*     } */
/*     if (caglobals.server) */
/*     { */
/*         flags |= caglobals.serverFlags; */
/*     } */

/*     caglobals.tcp.ipv4tcpenabled = flags & CA_IPV4; */
/*     caglobals.tcp.ipv6tcpenabled = flags & CA_IPV6; */
/* } */

LOCAL void CAAcceptConnection(CATransportFlags_t flag, CASocket_t *sock)
{
    VERIFY_NON_NULL_VOID(sock, TAG, "sock is NULL");

    struct sockaddr_storage clientaddr;
    socklen_t clientlen = sizeof (struct sockaddr_in);
    if (flag & CA_IPV6)
    {
        clientlen = sizeof(struct sockaddr_in6);
    }

    CASocketFd_t sockfd = accept(sock->fd, (struct sockaddr *)&clientaddr, &clientlen);
    if (OC_INVALID_SOCKET != sockfd)
    {
        CATCPSessionInfo_t *svritem =
                (CATCPSessionInfo_t *) OICCalloc(1, sizeof (*svritem));
        if (!svritem)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            OC_CLOSE_SOCKET(sockfd);
            return;
        }

        svritem->fd = sockfd;
        svritem->sep.endpoint.flags = flag;
        svritem->sep.endpoint.adapter = CA_ADAPTER_TCP;
        svritem->state = CONNECTED;
        svritem->isClient = false;
        CAConvertAddrToName((struct sockaddr_storage *)&clientaddr, clientlen,
                            svritem->sep.endpoint.addr, &svritem->sep.endpoint.port);

        oc_mutex_lock(tcp_mutexObjectList);
        LL_APPEND(tcp_sessionList, svritem);
        oc_mutex_unlock(tcp_mutexObjectList);

        TCP_CHECKFD(sockfd);

        // pass the connection information to CA Common Layer.
        if (tcp_connectionCallback)
        {
            tcp_connectionCallback(&(svritem->sep.endpoint), true, svritem->isClient);
        }
    }
}

#if !defined(WSA_WAIT_EVENT_0)
static ssize_t CAWakeUpForReadFdsUpdate(const char *host)
{
    if (caglobals.tcp.connectionFds[1] != -1)
    {
        ssize_t len = 0;
        do
        {
            len = write(caglobals.tcp.connectionFds[1], host, strlen(host));
        } while ((len == -1) && (errno == EINTR));

        if ((len == -1) && (errno != EINTR) && (errno != EPIPE))
        {
            OIC_LOG_V(DEBUG, TAG, "write failed: %s", strerror(errno));
        }
        return len;
    }
    return -1;
}
#else
#endif

CAResult_t CATCPCreateSocket(int family, CATCPSessionInfo_t *svritem)
{
    VERIFY_NON_NULL_MSG(svritem, TAG, "svritem is NULL");

    OIC_LOG_V(DEBUG, TAG, "try to connect with [%s:%u]",
              svritem->sep.endpoint.addr, svritem->sep.endpoint.port);

    // #1. create tcp socket.
    CASocketFd_t fd = socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (OC_INVALID_SOCKET == fd)
    {
        OIC_LOG_V(ERROR, TAG, "create socket failed: %s", strerror(errno));
        return CA_SOCKET_OPERATION_FAILED;
    }
    svritem->fd = fd;

    // #2. convert address from string to binary.
    struct sockaddr_storage sa = { .ss_family = (short)family };
    CAResult_t res = CAConvertNameToAddr(svritem->sep.endpoint.addr,
                                         svritem->sep.endpoint.port, &sa);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "convert name to sockaddr failed");
        return CA_SOCKET_OPERATION_FAILED;
    }

    // #3. set socket length.
    socklen_t socklen = 0;
    if (sa.ss_family == AF_INET6)
    {
        socklen = sizeof(struct sockaddr_in6);
    }
    else
    {
        socklen = sizeof(struct sockaddr_in);
    }

    // #4. connect to remote server device.
    if (connect(fd, (struct sockaddr *)&sa, socklen) < 0)
    {
        OIC_LOG_V(ERROR, TAG, "failed to connect socket, %s", strerror(errno));
        CALogSendStateInfo(svritem->sep.endpoint.adapter, svritem->sep.endpoint.addr,
                           svritem->sep.endpoint.port, 0, false, strerror(errno));
        return CA_SOCKET_OPERATION_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "connect socket success");
    svritem->state = CONNECTED;
    TCP_CHECKFD(svritem->fd);
#if !defined(WSA_WAIT_EVENT_0)
    ssize_t len = CAWakeUpForReadFdsUpdate(svritem->sep.endpoint.addr);
    if (-1 == len)
    {
        OIC_LOG(ERROR, TAG, "wakeup receive thread failed");
        return CA_SOCKET_OPERATION_FAILED;
    }
#else
    CAWakeUpForReadFdsUpdate();
#endif
    return CA_STATUS_OK;
}

static CASocketFd_t CACreateAcceptSocket(int family, CASocket_t *sock)
{
    VERIFY_NON_NULL_RET(sock, TAG, "sock", OC_INVALID_SOCKET);

    if (OC_INVALID_SOCKET != sock->fd)
    {
        OIC_LOG(DEBUG, TAG, "accept socket created already");
        return sock->fd;
    }

    socklen_t socklen = 0;
    struct sockaddr_storage server = { .ss_family = (short)family };

    CASocketFd_t fd = socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (OC_INVALID_SOCKET == fd)
    {
        OIC_LOG(ERROR, TAG, "Failed to create socket");
        goto exit;
    }

    if (family == AF_INET6)
    {
        // the socket is restricted to sending and receiving IPv6 packets only.
        int on = 1;
        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, OPTVAL_T(&on), sizeof (on)))
        {
            OIC_LOG_V(ERROR, TAG, "IPV6_V6ONLY failed: %s", strerror(errno));
            goto exit;
        }
        ((struct sockaddr_in6 *)&server)->sin6_port = htons(sock->port);
        socklen = sizeof (struct sockaddr_in6);
    }
    else
    {
        ((struct sockaddr_in *)&server)->sin_port = htons(sock->port);
        socklen = sizeof (struct sockaddr_in);
    }

    int reuse = 1;
    if (OC_SOCKET_ERROR == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, OPTVAL_T(&reuse), sizeof(reuse)))
    {
        OIC_LOG(ERROR, TAG, "setsockopt SO_REUSEADDR");
        goto exit;
    }

    if (OC_SOCKET_ERROR == bind(fd, (struct sockaddr *)&server, socklen))
    {
        OIC_LOG_V(ERROR, TAG, "bind socket failed: %s", strerror(errno));
        goto exit;
    }

    if (listen(fd, caglobals.tcp.listenBacklog) != 0)
    {
        OIC_LOG(ERROR, TAG, "listen() error");
        goto exit;
    }

    if (!sock->port)  // return the assigned port
    {
        if (OC_SOCKET_ERROR == getsockname(fd, (struct sockaddr *)&server, &socklen))
        {
            OIC_LOG_V(ERROR, TAG, "getsockname failed: %s", strerror(errno));
            goto exit;
        }
        sock->port = ntohs(family == AF_INET6 ?
                      ((struct sockaddr_in6 *)&server)->sin6_port :
                      ((struct sockaddr_in *)&server)->sin_port);
    }

    return fd;

exit:
    if (fd != OC_INVALID_SOCKET)
    {
        OC_CLOSE_SOCKET(fd);
    }
    return OC_INVALID_SOCKET;
}

void CAInitializePipe(int *fds)
{
    int ret = pipe(fds);
    if (-1 != ret)
    {
        ret = fcntl(fds[0], F_GETFD);
        if (-1 != ret)
        {
            ret = fcntl(fds[0], F_SETFD, ret|FD_CLOEXEC);
        }
        if (-1 != ret)
        {
            ret = fcntl(fds[1], F_GETFD);
        }
        if (-1 != ret)
        {
            ret = fcntl(fds[1], F_SETFD, ret|FD_CLOEXEC);
        }
        if (-1 == ret)
        {
            close(fds[1]);
            close(fds[0]);

            fds[0] = -1;
            fds[1] = -1;

            OIC_LOG_V(ERROR, TAG, "pipe failed: %s", strerror(errno));
        }
    }
}

#define NEWSOCKET(FAMILY, NAME) \
    caglobals.tcp.NAME.fd = CACreateAcceptSocket(FAMILY, &caglobals.tcp.NAME); \
    if (caglobals.tcp.NAME.fd == OC_INVALID_SOCKET) \
    { \
        caglobals.tcp.NAME.port = 0; \
        caglobals.tcp.NAME.fd = CACreateAcceptSocket(FAMILY, &caglobals.tcp.NAME); \
    } \
    TCP_CHECKFD(caglobals.tcp.NAME.fd);


CASocketFd_t CAGetSocketFDFromEndpoint(const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint is NULL", OC_INVALID_SOCKET);

    OIC_LOG_V(DEBUG, TAG, "Looking for [%s:%d]", endpoint->addr, endpoint->port);

    // get connection info from list.
    oc_mutex_lock(tcp_mutexObjectList);
    CATCPSessionInfo_t *session = NULL;
    LL_FOREACH(tcp_sessionList, session)
    {
        if (!strncmp(session->sep.endpoint.addr, endpoint->addr,
                     sizeof(session->sep.endpoint.addr))
                && (session->sep.endpoint.port == endpoint->port)
                && (session->sep.endpoint.flags & endpoint->flags))
        {
            oc_mutex_unlock(tcp_mutexObjectList);
            OIC_LOG(DEBUG, TAG, "Found in session list");
            return session->fd;
        }
    }

    oc_mutex_unlock(tcp_mutexObjectList);
    OIC_LOG(DEBUG, TAG, "Session not found");
    return OC_INVALID_SOCKET;
}

void tcp_create_accept_sockets()
{
    if (caglobals.tcp.ipv6tcpenabled)
    {
        NEWSOCKET(AF_INET6, ipv6);
        NEWSOCKET(AF_INET6, ipv6s);
        OIC_LOG_V(DEBUG, TAG, "IPv6 socket fd=%d, port=%d",
                  caglobals.tcp.ipv6.fd, caglobals.tcp.ipv6.port);
        OIC_LOG_V(DEBUG, TAG, "IPv6 secure socket fd=%d, port=%d",
                  caglobals.tcp.ipv6s.fd, caglobals.tcp.ipv6s.port);
    }

    if (caglobals.tcp.ipv4tcpenabled)
    {
        NEWSOCKET(AF_INET, ipv4);
        NEWSOCKET(AF_INET, ipv4s);
        OIC_LOG_V(DEBUG, TAG, "IPv4 socket fd=%d, port=%d",
                  caglobals.tcp.ipv4.fd, caglobals.tcp.ipv4.port);
        OIC_LOG_V(DEBUG, TAG, "IPv4 secure socket fd=%d, port=%d",
                  caglobals.tcp.ipv4s.fd, caglobals.tcp.ipv4s.port);
    }

}

#define CLOSE_TCP_SOCKET(TYPE) \
    if (tcp_socket_ ## TYPE.fd != OC_INVALID_SOCKET) \
    { \
        OC_CLOSE_SOCKET(tcp_socket_ ## TYPE.fd); \
        tcp_socket_ ## TYPE.fd = OC_INVALID_SOCKET; \
    }

void tcp_close_accept_sockets()
{
    CLOSE_TCP_SOCKET(ipv4);
    CLOSE_TCP_SOCKET(ipv4s);
    CLOSE_TCP_SOCKET(ipv6);
    CLOSE_TCP_SOCKET(ipv6s);
}

CAResult_t CAGetTCPInterfaceInformation(CAEndpoint_t **info, size_t *size)
{
    VERIFY_NON_NULL_MSG(info, TAG, "info is NULL");
    VERIFY_NON_NULL_MSG(size, TAG, "size is NULL");

    u_arraylist_t *iflist = CAIPGetInterfaceInformation(0);
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

#ifdef __WITH_TLS__
    const size_t endpointsPerInterface = 2;
#else
    const size_t endpointsPerInterface = 1;
#endif

    size_t interfaces = u_arraylist_length(iflist);
    for (size_t i = 0; i < u_arraylist_length(iflist); i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
        if (!ifitem)
        {
            continue;
        }

        if ((ifitem->family == AF_INET6 && !udp_ipv6enabled) ||
            (ifitem->family == AF_INET && !udp_ipv4enabled))
        {
            interfaces--;
        }
    }

    if (!interfaces)
    {
        OIC_LOG(DEBUG, TAG, "network interface size is zero");
        return CA_STATUS_OK;
    }

    size_t totalEndpoints = interfaces * endpointsPerInterface;
    CAEndpoint_t *ep = (CAEndpoint_t *)OICCalloc(totalEndpoints, sizeof (CAEndpoint_t));
    if (!ep)
    {
        OIC_LOG(ERROR, TAG, "Malloc Failed");
        u_arraylist_destroy(iflist);
        return CA_MEMORY_ALLOC_FAILED;
    }

    for (size_t i = 0, j = 0; i < u_arraylist_length(iflist); i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
        if (!ifitem)
        {
            continue;
        }

        if ((ifitem->family == AF_INET6 && !udp_ipv6enabled) ||
            (ifitem->family == AF_INET && !udp_ipv4enabled))
        {
            continue;
        }

        ep[j].adapter = CA_ADAPTER_TCP;
        ep[j].ifindex = ifitem->index;

        if (ifitem->family == AF_INET6)
        {
            ep[j].flags = CA_IPV6;
            ep[j].port = tcp_socket_ipv6.port;
        }
        else if (ifitem->family == AF_INET)
        {
            ep[j].flags = CA_IPV4;
            ep[j].port = tcp_socket_ipv4.port;
        }
        else
        {
            continue;
        }
        OICStrcpy(ep[j].addr, sizeof(ep[j].addr), ifitem->addr);

#ifdef __WITH_TLS__
        j++;

        ep[j].adapter = CA_ADAPTER_TCP;
        ep[j].ifindex = ifitem->index;

        if (ifitem->family == AF_INET6)
        {
            ep[j].flags = CA_IPV6 | CA_SECURE;
            ep[j].port = tcp_socket_ipv6s.port;
        }
        else
        {
            ep[j].flags = CA_IPV4 | CA_SECURE;
            ep[j].port = tcp_socket_ipv4s.port;
        }
        OICStrcpy(ep[j].addr, sizeof(ep[j].addr), ifitem->addr);
#endif
        j++;
    }

    *info = ep;
    *size = totalEndpoints;

    u_arraylist_destroy(iflist);

    return CA_STATUS_OK;
}
