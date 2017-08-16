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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <errno.h>

#ifndef WITH_ARDUINO
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include "catcpinterface.h"
#include "pdu.h"
#include "caadapterutils.h"
#include "camutex.h"
#include "oic_malloc.h"
#include "oic_string.h"

/**
 * Logging tag for module name.
 */
#define TAG "OIC_CA_TCP_SERVER"

/**
 * Maximum CoAP over TCP header length
 * to know the total data length.
 */
#define TCP_MAX_HEADER_LEN  6

/**
 * Mutex to synchronize device object list.
 */
static ca_mutex g_mutexObjectList = NULL;

/**
 * Conditional mutex to synchronize.
 */
static ca_cond g_condObjectList = NULL;

/**
 * Maintains the callback to be notified when data received from remote device.
 */
static CATCPPacketReceivedCallback g_packetReceivedCallback = NULL;

/**
 * Error callback to update error in TCP.
 */
static CATCPErrorHandleCallback g_tcpErrorHandler = NULL;

/**
 * Connected Callback to pass the connection information to RI.
 */
static CATCPConnectionHandleCallback g_connectionCallback = NULL;

static CAResult_t CATCPCreateMutex();
static void CATCPDestroyMutex();
static CAResult_t CATCPCreateCond();
static void CATCPDestroyCond();
static int CACreateAcceptSocket(int family, CASocket_t *sock);
static void CAAcceptConnection(CATransportFlags_t flag, CASocket_t *sock);
static void CAFindReadyMessage();
static void CASelectReturned(fd_set *readFds);
static void CAReceiveMessage(int fd);
static void CAReceiveHandler(void *data);
static int CATCPCreateSocket(int family, CATCPSessionInfo_t *tcpServerInfo);

#define CHECKFD(FD) \
    if (FD > caglobals.tcp.maxfd) \
        caglobals.tcp.maxfd = FD;

static void CATCPDestroyMutex()
{
    if (g_mutexObjectList)
    {
        ca_mutex_free(g_mutexObjectList);
        g_mutexObjectList = NULL;
    }
}

static CAResult_t CATCPCreateMutex()
{
    if (!g_mutexObjectList)
    {
        g_mutexObjectList = ca_mutex_new();
        if (!g_mutexObjectList)
        {
            OIC_LOG(ERROR, TAG, "Failed to created mutex!");
            return CA_STATUS_FAILED;
        }
    }

    return CA_STATUS_OK;
}

static void CATCPDestroyCond()
{
    if (g_condObjectList)
    {
        ca_cond_free(g_condObjectList);
        g_condObjectList = NULL;
    }
}

static CAResult_t CATCPCreateCond()
{
    if (!g_condObjectList)
    {
        g_condObjectList = ca_cond_new();
        if (!g_condObjectList)
        {
            OIC_LOG(ERROR, TAG, "Failed to created cond!");
            return CA_STATUS_FAILED;
        }
    }
    return CA_STATUS_OK;
}

static void CAReceiveHandler(void *data)
{
    (void)data;
    OIC_LOG(DEBUG, TAG, "IN - CAReceiveHandler");

    while (!caglobals.tcp.terminate)
    {
        CAFindReadyMessage();
    }

    ca_mutex_lock(g_mutexObjectList);
    ca_cond_signal(g_condObjectList);
    ca_mutex_unlock(g_mutexObjectList);

    OIC_LOG(DEBUG, TAG, "OUT - CAReceiveHandler");
}

static void CAFindReadyMessage()
{
    fd_set readFds;
    struct timeval timeout = { .tv_sec = caglobals.tcp.selectTimeout };

    FD_ZERO(&readFds);

    if (-1 != caglobals.tcp.ipv4.fd)
    {
        FD_SET(caglobals.tcp.ipv4.fd, &readFds);
    }
    if (-1 != caglobals.tcp.ipv6.fd)
    {
        FD_SET(caglobals.tcp.ipv6.fd, &readFds);
    }
    if (-1 != caglobals.tcp.shutdownFds[0])
    {
        FD_SET(caglobals.tcp.shutdownFds[0], &readFds);
    }
    if (-1 != caglobals.tcp.connectionFds[0])
    {
        FD_SET(caglobals.tcp.connectionFds[0], &readFds);
    }

    uint32_t length = u_arraylist_length(caglobals.tcp.svrlist);
    for (size_t i = 0; i < length; i++)
    {
        CATCPSessionInfo_t *svritem =
                (CATCPSessionInfo_t *) u_arraylist_get(caglobals.tcp.svrlist, i);
        if (svritem && 0 <= svritem->fd)
        {
            FD_SET(svritem->fd, &readFds);
        }
    }

    int ret = select(caglobals.tcp.maxfd + 1, &readFds, NULL, NULL, &timeout);

    if (caglobals.tcp.terminate)
    {
        OIC_LOG_V(DEBUG, TAG, "Packet receiver Stop request received.");
        return;
    }
    if (0 >= ret)
    {
        if (0 > ret)
        {
            OIC_LOG_V(FATAL, TAG, "select error %s", strerror(errno));
        }
        return;
    }

    CASelectReturned(&readFds);
}

static void CASelectReturned(fd_set *readFds)
{
    VERIFY_NON_NULL_VOID(readFds, TAG, "readFds is NULL");

    if (caglobals.tcp.ipv4.fd != -1 && FD_ISSET(caglobals.tcp.ipv4.fd, readFds))
    {
        CAAcceptConnection(CA_IPV4, &caglobals.tcp.ipv4);
        return;
    }
    else if (caglobals.tcp.ipv6.fd != -1 && FD_ISSET(caglobals.tcp.ipv6.fd, readFds))
    {
        CAAcceptConnection(CA_IPV6, &caglobals.tcp.ipv6);
        return;
    }
    else if (-1 != caglobals.tcp.connectionFds[0] &&
            FD_ISSET(caglobals.tcp.connectionFds[0], readFds))
    {
        // new connection was created from remote device.
        // exit the function to update read file descriptor.
        char buf[MAX_ADDR_STR_SIZE_CA] = {0};
        ssize_t len = read(caglobals.tcp.connectionFds[0], buf, sizeof (buf));
        if (-1 == len)
        {
            return;
        }
        OIC_LOG_V(DEBUG, TAG, "Received new connection event with [%s]", buf);
        FD_CLR(caglobals.tcp.connectionFds[0], readFds);
        return;
    }
    else if (-1 != caglobals.tcp.connectionFds[0] &&
            FD_ISSET(caglobals.tcp.connectionFds[0], readFds))
    {
        // new connection was created from remote device.
        // exit the function to update read file descriptor.
        char buf[MAX_ADDR_STR_SIZE_CA] = {0};
        ssize_t len = read(caglobals.tcp.connectionFds[0], buf, sizeof (buf));
        if (-1 == len)
        {
            return;
        }
        OIC_LOG_V(DEBUG, TAG, "Received new connection event with [%s]", buf);
        FD_CLR(caglobals.tcp.connectionFds[0], readFds);
        return;
    }
    else
    {
        uint32_t length = u_arraylist_length(caglobals.tcp.svrlist);
        for (size_t i = 0; i < length; i++)
        {
            CATCPSessionInfo_t *svritem =
                    (CATCPSessionInfo_t *) u_arraylist_get(caglobals.tcp.svrlist, i);
            if (svritem && svritem->fd >= 0)
            {
                if (FD_ISSET(svritem->fd, readFds))
                {
                    CAReceiveMessage(svritem->fd);
                    FD_CLR(svritem->fd, readFds);
                }
            }
        }
    }
}

static void CAAcceptConnection(CATransportFlags_t flag, CASocket_t *sock)
{
    VERIFY_NON_NULL_VOID(sock, TAG, "sock is NULL");

    struct sockaddr_storage clientaddr;
    socklen_t clientlen = sizeof (struct sockaddr_in);
    if (flag & CA_IPV6)
    {
        clientlen = sizeof(struct sockaddr_in6);
    }

    int sockfd = accept(sock->fd, (struct sockaddr *)&clientaddr, &clientlen);
    if (-1 != sockfd)
    {
        CATCPSessionInfo_t *svritem =
                (CATCPSessionInfo_t *) OICCalloc(1, sizeof (*svritem));
        if (!svritem)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            close(sockfd);
            return;
        }

        svritem->fd = sockfd;
        svritem->sep.endpoint.flags = flag;
        CAConvertAddrToName((struct sockaddr_storage *)&clientaddr, clientlen,
                            svritem->sep.endpoint.addr, &svritem->sep.endpoint.port);

        ca_mutex_lock(g_mutexObjectList);
        bool result = u_arraylist_add(caglobals.tcp.svrlist, svritem);
        if (!result)
        {
            OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
            close(sockfd);
            OICFree(svritem);
            ca_mutex_unlock(g_mutexObjectList);
            return;
        }
        ca_mutex_unlock(g_mutexObjectList);

        CHECKFD(sockfd);
    }
}

static void CAReceiveMessage(int fd)
{
    // #1. get remote device information from file descriptor.
    size_t index = 0;
    CATCPSessionInfo_t *svritem = CAGetSessionInfoFromFD(fd, &index);
    if (!svritem)
    {
        OIC_LOG(ERROR, TAG, "there is no connection information in list");
        return;
    }

    // #2. get already allocated memory size.
    size_t bufSize = (svritem->totalDataLen == 0) ? TCP_MAX_HEADER_LEN : svritem->totalDataLen;
    if (!svritem->recvData)
    {
        svritem->recvData = (unsigned char *) OICCalloc(1, bufSize);
        if (!svritem->recvData)
        {
            OIC_LOG(ERROR, TAG, "out of memory");
            CADisconnectTCPSession(svritem, index);
            return;
        }
    }

    // #3. receive data from remote device.
    ssize_t recvLen = recv(fd, svritem->recvData + svritem->recvDataLen,
                           bufSize - svritem->recvDataLen, 0);
    if (recvLen <= 0)
    {
        if(EWOULDBLOCK != errno)
        {
            OIC_LOG_V(ERROR, TAG, "Recvfrom failed %s", strerror(errno));
            CADisconnectTCPSession(svritem, index);
        }
        return;
    }
    svritem->recvDataLen += recvLen;

    // #4. get actual data length from coap over tcp header.
    if (!svritem->totalDataLen)
    {
        coap_transport_type transport = coap_get_tcp_header_type_from_initbyte(
                ((unsigned char *) svritem->recvData)[0] >> 4);

        size_t headerLen = coap_get_tcp_header_length_for_transport(transport);
        if (svritem->recvDataLen >= headerLen)
        {
            svritem->totalDataLen = CAGetTotalLengthFromHeader(
                    (unsigned char *) svritem->recvData);
            bufSize = svritem->totalDataLen;
            unsigned char *newBuf = OICRealloc(svritem->recvData, bufSize);
            if (!newBuf)
            {
                OIC_LOG(ERROR, TAG, "out of memory");
                CADisconnectTCPSession(svritem, index);
                return;
            }
            svritem->recvData = newBuf;
        }
    }

    // #5. pass the received data information to upper layer.
    if ((svritem->totalDataLen == svritem->recvDataLen) && g_packetReceivedCallback)
    {
        svritem->sep.endpoint.adapter = CA_ADAPTER_TCP;
        g_packetReceivedCallback(&svritem->sep, svritem->recvData, svritem->recvDataLen);
        OIC_LOG_V(DEBUG, TAG, "total received data len:%d", svritem->recvDataLen);

        // initialize data info to receive next message.
        OICFree(svritem->recvData);
        svritem->recvData = NULL;
        svritem->recvDataLen = 0;
        svritem->totalDataLen = 0;
    }

    return;
}

static void CAWakeUpForReadFdsUpdate(const char *host)
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
    }
}

static CAResult_t CATCPConvertNameToAddr(int family, const char *host, uint16_t port,
                                         struct sockaddr_storage *sockaddr)
{
    struct addrinfo *addrs = NULL;
    struct addrinfo hints = { .ai_family = family,
                              .ai_protocol   = IPPROTO_TCP,
                              .ai_socktype = SOCK_STREAM,
                              .ai_flags = AI_NUMERICHOST };

    int r = getaddrinfo(host, NULL, &hints, &addrs);
    if (r)
    {
        if (EAI_SYSTEM == r)
        {
            OIC_LOG_V(ERROR, TAG, "getaddrinfo failed: errno %s", strerror(errno));
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "getaddrinfo failed: %s", gai_strerror(r));
        }
        freeaddrinfo(addrs);
        return CA_STATUS_FAILED;
    }
    // assumption: in this case, getaddrinfo will only return one addrinfo
    // or first is the one we want.
    if (addrs[0].ai_family == AF_INET6)
    {
        memcpy(sockaddr, addrs[0].ai_addr, sizeof (struct sockaddr_in6));
        ((struct sockaddr_in6 *)sockaddr)->sin6_port = htons(port);
    }
    else
    {
        memcpy(sockaddr, addrs[0].ai_addr, sizeof (struct sockaddr_in));
        ((struct sockaddr_in *)sockaddr)->sin_port = htons(port);
    }
    freeaddrinfo(addrs);
    return CA_STATUS_OK;
}

static int CATCPCreateSocket(int family, CATCPSessionInfo_t *svritem)
{
    // #1. create tcp socket.
    int fd = socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == fd)
    {
        OIC_LOG_V(ERROR, TAG, "create socket failed: %s", strerror(errno));
        return -1;
    }

    // #2. convert address from string to binary.
    struct sockaddr_storage sa = { .ss_family = family };
    CAResult_t res = CATCPConvertNameToAddr(family, svritem->sep.endpoint.addr,
                                            svritem->sep.endpoint.port, &sa);
    if (CA_STATUS_OK != res)
    {
        close(fd);
        return -1;
    }

    // #3. set socket length.
    socklen_t socklen = 0;
    if (sa.ss_family == AF_INET6)
    {
        struct sockaddr_in6 *sock6 = (struct sockaddr_in6 *)&sa;
        if (!sock6->sin6_scope_id)
        {
            sock6->sin6_scope_id = svritem->sep.endpoint.ifindex;
        }
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
        close(fd);
        return -1;
    }

    OIC_LOG(DEBUG, TAG, "connect socket success");
    CAWakeUpForReadFdsUpdate(svritem->sep.endpoint.addr);
    return fd;
}

static int CACreateAcceptSocket(int family, CASocket_t *sock)
{
    VERIFY_NON_NULL_RET(sock, TAG, "sock", -1);

    if (sock->fd != -1)
    {
        OIC_LOG(DEBUG, TAG, "accept socket created already");
        return sock->fd;
    }

    socklen_t socklen = 0;
    struct sockaddr_storage server = { .ss_family = family };

    int fd = socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0)
    {
        OIC_LOG(ERROR, TAG, "Failed to create socket");
        goto exit;
    }

    if (family == AF_INET6)
    {
        // the socket is re‐stricted to sending and receiving IPv6 packets only.
        int on = 1;
        if (-1 == setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof (on)))
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
    if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)))
    {
        OIC_LOG(ERROR, TAG, "setsockopt SO_REUSEADDR");
        goto exit;
    }

    if (-1 == bind(fd, (struct sockaddr *)&server, socklen))
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
        if (-1 == getsockname(fd, (struct sockaddr *)&server, &socklen))
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
    if (fd >= 0)
    {
        close(fd);
    }
    return -1;
}

static void CAInitializePipe(int *fds)
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
    if (caglobals.tcp.NAME.fd == -1) \
    { \
        caglobals.tcp.NAME.port = 0; \
        caglobals.tcp.NAME.fd = CACreateAcceptSocket(FAMILY, &caglobals.tcp.NAME); \
    } \
    CHECKFD(caglobals.tcp.NAME.fd);

CAResult_t CATCPStartServer(const ca_thread_pool_t threadPool)
{
    if (caglobals.tcp.started)
    {
        return CA_STATUS_OK;
    }

    if (!caglobals.tcp.ipv4tcpenabled)
    {
        caglobals.tcp.ipv4tcpenabled = true;    // only needed to run CA tests
    }
    if (!caglobals.tcp.ipv6tcpenabled)
    {
        caglobals.tcp.ipv6tcpenabled = true;    // only needed to run CA tests
    }

    CAResult_t res = CATCPCreateMutex();
    if (CA_STATUS_OK == res)
    {
        res = CATCPCreateCond();
    }
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "failed to create mutex/cond");
        return res;
    }

    ca_mutex_lock(g_mutexObjectList);
    if (!caglobals.tcp.svrlist)
    {
        caglobals.tcp.svrlist = u_arraylist_create();
    }
    ca_mutex_unlock(g_mutexObjectList);

    if (caglobals.server)
    {
        NEWSOCKET(AF_INET, ipv4);
        NEWSOCKET(AF_INET6, ipv6);
        OIC_LOG_V(DEBUG, TAG, "IPv4 socket fd=%d, port=%d",
                  caglobals.tcp.ipv4.fd, caglobals.tcp.ipv4.port);
        OIC_LOG_V(DEBUG, TAG, "IPv6 socket fd=%d, port=%d",
                  caglobals.tcp.ipv6.fd, caglobals.tcp.ipv6.port);
    }

    // create pipe for fast shutdown
    CAInitializePipe(caglobals.tcp.shutdownFds);
    CHECKFD(caglobals.tcp.shutdownFds[0]);
    CHECKFD(caglobals.tcp.shutdownFds[1]);

    // create pipe for connection event
    CAInitializePipe(caglobals.tcp.connectionFds);
    CHECKFD(caglobals.tcp.connectionFds[0]);
    CHECKFD(caglobals.tcp.connectionFds[1]);

    caglobals.tcp.terminate = false;
    res = ca_thread_pool_add_task(threadPool, CAReceiveHandler, NULL);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread_pool_add_task failed");
        return res;
    }
    OIC_LOG(DEBUG, TAG, "CAReceiveHandler thread started successfully.");

    caglobals.tcp.started = true;
    return CA_STATUS_OK;
}

void CATCPStopServer()
{
    // mutex lock
    ca_mutex_lock(g_mutexObjectList);

    // set terminate flag
    caglobals.tcp.terminate = true;

    if (caglobals.tcp.shutdownFds[1] != -1)
    {
        close(caglobals.tcp.shutdownFds[1]);
        // receive thread will stop immediately
    }

    if (caglobals.tcp.connectionFds[1] != -1)
    {
        close(caglobals.tcp.connectionFds[1]);
    }

    if (caglobals.tcp.started)
    {
        ca_cond_wait(g_condObjectList, g_mutexObjectList);
    }
    caglobals.tcp.started = false;

    // mutex unlock
    ca_mutex_unlock(g_mutexObjectList);

    if (-1 != caglobals.tcp.ipv4.fd)
    {
        close(caglobals.tcp.ipv4.fd);
        caglobals.tcp.ipv4.fd = -1;
    }

    if (-1 != caglobals.tcp.ipv6.fd)
    {
        close(caglobals.tcp.ipv6.fd);
        caglobals.tcp.ipv6.fd = -1;
    }

    CATCPDisconnectAll();
    CATCPDestroyMutex();
    CATCPDestroyCond();
}

void CATCPSetPacketReceiveCallback(CATCPPacketReceivedCallback callback)
{
    g_packetReceivedCallback = callback;
}

void CATCPSetConnectionChangedCallback(CATCPConnectionHandleCallback connHandler)
{
    g_connectionCallback = connHandler;
}

static size_t CACheckPayloadLength(const void *data, size_t dlen)
{
    VERIFY_NON_NULL_RET(data, TAG, "data", -1);

    coap_transport_type transport = coap_get_tcp_header_type_from_initbyte(
            ((unsigned char *)data)[0] >> 4);

    coap_pdu_t *pdu = coap_new_pdu(transport, dlen);
    if (!pdu)
    {
        OIC_LOG(ERROR, TAG, "outpdu is null");
        return 0;
    }

    int ret = coap_pdu_parse((unsigned char *) data, dlen, pdu, transport);
    if (0 >= ret)
    {
        OIC_LOG(ERROR, TAG, "pdu parse failed");
        coap_delete_pdu(pdu);
        return 0;
    }

    size_t payloadLen = 0;
    size_t headerSize = coap_get_tcp_header_length_for_transport(transport);
    OIC_LOG_V(DEBUG, TAG, "headerSize : %d, pdu length : %d",
              headerSize, pdu->length);
    if (pdu->length > headerSize)
    {
        payloadLen = (unsigned char *) pdu->hdr + pdu->length - pdu->data;
    }

    OICFree(pdu);

    return payloadLen;
}

static void sendData(const CAEndpoint_t *endpoint, const void *data,
                     size_t dlen, const char *fam)
{
    // #1. get TCP Server object from list
    size_t index = 0;
    CATCPSessionInfo_t *svritem = CAGetTCPSessionInfoFromEndpoint(endpoint, &index);
    if (!svritem)
    {
        // if there is no connection info, connect to TCP Server
        svritem = CAConnectTCPSession(endpoint);
        if (!svritem)
        {
            OIC_LOG(ERROR, TAG, "Failed to create TCP server object");
            if (g_tcpErrorHandler)
            {
                g_tcpErrorHandler(endpoint, data, dlen, CA_SEND_FAILED);
            }
            return;
        }
    }

    // #2. check payload length
    size_t payloadLen = CACheckPayloadLength(data, dlen);
    // if payload length is zero, disconnect from TCP server
    if (!payloadLen)
    {
        OIC_LOG(DEBUG, TAG, "payload length is zero, disconnect from remote device");
        CADisconnectTCPSession(svritem, index);
        return;
    }

    // #3. check connection state
    if (svritem->fd < 0)
    {
        // if file descriptor value is wrong, remove TCP Server info from list
        OIC_LOG(ERROR, TAG, "Failed to connect to TCP server");
        CADisconnectTCPSession(svritem, index);
        if (g_tcpErrorHandler)
        {
            g_tcpErrorHandler(endpoint, data, dlen, CA_SEND_FAILED);
        }
        return;
    }

    // #4. send data to TCP Server
    ssize_t remainLen = dlen;
    do
    {
        ssize_t len = send(svritem->fd, data, remainLen, 0);
        if (-1 == len)
        {
            if (EWOULDBLOCK != errno)
            {
                OIC_LOG_V(ERROR, TAG, "unicast ipv4tcp sendTo failed: %s", strerror(errno));
                if (g_tcpErrorHandler)
                {
                    g_tcpErrorHandler(endpoint, data, dlen, CA_SEND_FAILED);
                }
                return;
            }
            continue;
        }
        data += len;
        remainLen -= len;
    } while (remainLen > 0);

    OIC_LOG_V(INFO, TAG, "unicast %stcp sendTo is successful: %zu bytes", fam, dlen);
}

void CATCPSendData(CAEndpoint_t *endpoint, const void *data, uint32_t datalen,
                   bool isMulticast)
{
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    if (!isMulticast)
    {
        if (caglobals.tcp.ipv6tcpenabled && (endpoint->flags & CA_IPV6))
        {
            sendData(endpoint, data, datalen, "ipv6");
        }
        if (caglobals.tcp.ipv4tcpenabled && (endpoint->flags & CA_IPV4))
        {
            sendData(endpoint, data, datalen, "ipv4");
        }
    }
}

CAResult_t CAGetTCPInterfaceInformation(CAEndpoint_t **info, uint32_t *size)
{
    VERIFY_NON_NULL(info, TAG, "info is NULL");
    VERIFY_NON_NULL(size, TAG, "size is NULL");

    return CA_NOT_SUPPORTED;
}

CATCPSessionInfo_t *CAConnectTCPSession(const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint is NULL", NULL);

    // #1. create TCP server object
    CATCPSessionInfo_t *svritem = (CATCPSessionInfo_t *) OICCalloc(1, sizeof (*svritem));
    if (!svritem)
    {
        OIC_LOG(ERROR, TAG, "Out of memory");
        return NULL;
    }
    memcpy(svritem->sep.endpoint.addr, endpoint->addr, sizeof(svritem->sep.endpoint.addr));
    svritem->sep.endpoint.adapter = endpoint->adapter;
    svritem->sep.endpoint.port = endpoint->port;
    svritem->sep.endpoint.flags = endpoint->flags;
    svritem->sep.endpoint.ifindex = endpoint->ifindex;

    // #2. create the socket and connect to TCP server
    int family = (svritem->sep.endpoint.flags & CA_IPV6) ? AF_INET6 : AF_INET;
    int fd = CATCPCreateSocket(family, svritem);
    if (-1 == fd)
    {
        OICFree(svritem);
        return NULL;
    }

    // #3. add TCP connection info to list
    svritem->fd = fd;
    ca_mutex_lock(g_mutexObjectList);
    if (caglobals.tcp.svrlist)
    {
        bool res = u_arraylist_add(caglobals.tcp.svrlist, svritem);
        if (!res)
        {
            OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
            close(svritem->fd);
            OICFree(svritem);
            ca_mutex_unlock(g_mutexObjectList);
            return NULL;
        }
    }
    ca_mutex_unlock(g_mutexObjectList);

    CHECKFD(fd);

    // pass the connection information to CA Common Layer.
    if (g_connectionCallback)
    {
        g_connectionCallback(&(svritem->sep.endpoint), true);
    }

    return svritem;
}

CAResult_t CADisconnectTCPSession(CATCPSessionInfo_t *svritem, size_t index)
{
    VERIFY_NON_NULL(svritem, TAG, "svritem is NULL");

    ca_mutex_lock(g_mutexObjectList);

    // close the socket and remove TCP connection info in list
    if (svritem->fd >= 0)
    {
        close(svritem->fd);
    }
    u_arraylist_remove(caglobals.tcp.svrlist, index);
    OICFree(svritem->recvData);

    // pass the connection information to CA Common Layer.
    if (g_connectionCallback)
    {
        g_connectionCallback(&(svritem->sep.endpoint), false);
    }

    OICFree(svritem);
    ca_mutex_unlock(g_mutexObjectList);

    return CA_STATUS_OK;
}

void CATCPDisconnectAll()
{
    ca_mutex_lock(g_mutexObjectList);
    uint32_t length = u_arraylist_length(caglobals.tcp.svrlist);

    CATCPSessionInfo_t *svritem = NULL;
    for (size_t i = 0; i < length; i++)
    {
        svritem = (CATCPSessionInfo_t *) u_arraylist_get(caglobals.tcp.svrlist, i);
        if (svritem && svritem->fd >= 0)
        {
            shutdown(svritem->fd, SHUT_RDWR);
            close(svritem->fd);
            OICFree(svritem->recvData);
        }
    }
    u_arraylist_destroy(caglobals.tcp.svrlist);
    ca_mutex_unlock(g_mutexObjectList);
}

CATCPSessionInfo_t *CAGetTCPSessionInfoFromEndpoint(const CAEndpoint_t *endpoint, size_t *index)
{
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint is NULL", NULL);
    VERIFY_NON_NULL_RET(index, TAG, "index is NULL", NULL);

    // get connection info from list
    uint32_t length = u_arraylist_length(caglobals.tcp.svrlist);
    for (size_t i = 0; i < length; i++)
    {
        CATCPSessionInfo_t *svritem = (CATCPSessionInfo_t *) u_arraylist_get(
                caglobals.tcp.svrlist, i);
        if (!svritem)
        {
            continue;
        }

        if (!strncmp(svritem->sep.endpoint.addr, endpoint->addr,
                     sizeof(svritem->sep.endpoint.addr))
                && (svritem->sep.endpoint.port == endpoint->port)
                && (svritem->sep.endpoint.flags & endpoint->flags))
        {
            *index = i;
            return svritem;
        }
    }

    return NULL;
}

CATCPSessionInfo_t *CAGetSessionInfoFromFD(int fd, size_t *index)
{
    ca_mutex_lock(g_mutexObjectList);

    // check from the last item.
    CATCPSessionInfo_t *svritem = NULL;
    uint32_t length = u_arraylist_length(caglobals.tcp.svrlist);
    for (size_t i = 0; i < length; i++)
    {
        svritem = (CATCPSessionInfo_t *) u_arraylist_get(caglobals.tcp.svrlist, i);

        if (svritem && svritem->fd == fd)
        {
            *index = i;
            ca_mutex_unlock(g_mutexObjectList);
            return svritem;
        }
    }

    ca_mutex_unlock(g_mutexObjectList);

    return NULL;
}

size_t CAGetTotalLengthFromHeader(const unsigned char *recvBuffer)
{
    OIC_LOG(DEBUG, TAG, "IN - CAGetTotalLengthFromHeader");

    coap_transport_type transport = coap_get_tcp_header_type_from_initbyte(
            ((unsigned char *)recvBuffer)[0] >> 4);
    size_t optPaylaodLen = coap_get_length_from_header((unsigned char *)recvBuffer,
                                                        transport);
    size_t headerLen = coap_get_tcp_header_length((unsigned char *)recvBuffer);

    OIC_LOG_V(DEBUG, TAG, "option/paylaod length [%d]", optPaylaodLen);
    OIC_LOG_V(DEBUG, TAG, "header length [%d]", headerLen);
    OIC_LOG_V(DEBUG, TAG, "total data length [%d]", headerLen + optPaylaodLen);

    OIC_LOG(DEBUG, TAG, "OUT - CAGetTotalLengthFromHeader");
    return headerLen + optPaylaodLen;
}

void CATCPSetErrorHandler(CATCPErrorHandleCallback errorHandleCallback)
{
    g_tcpErrorHandler = errorHandleCallback;
}
