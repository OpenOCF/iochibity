/* ****************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542 // for PKTINFO
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for in6_pktinfo
#endif

#include "caipserver.h"
/* #include "iotivity_config.h" */
/* #include "iotivity_debug.h" */

#include <stdbool.h>

#if INTERFACE
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#if INTERFACE
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#include <errno.h>
#ifdef __linux__
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

#include <coap/pdu.h>
#include <inttypes.h>
/* #include "caipserver.h" */
/* #include "caipnwmonitor.h" */
/* #include "caadapterutils.h" */
/* #if defined(__WITH_DTLS__) || defined(__WITH_TLS__) */
/* #include "ca_adapter_net_ssl.h" */
/* #endif */
/* #include "octhread.h" */
/* #include "oic_malloc.h" */
/* #include "oic_string.h" */

#define USE_IP_MREQN
/* #if defined(_WIN32)
 * #undef USE_IP_MREQN
 * #endif */

/*
 * Logging tag for module name
 */
#define TAG "OIC_CA_IP_SERVER"

/**
 * Enum for defining different server types.
 */
typedef enum
{
    CA_UNICAST_SERVER = 0,      /**< Unicast Server */
    CA_MULTICAST_SERVER,        /**< Multicast Server */
    CA_SECURED_UNICAST_SERVER,  /**< Secured Unicast Server */
    CA_NFC_SERVER               /**< Listening Server */
} CAAdapterServerType_t;

#if EXPORT_INTERFACE
/**
 * Callback to be notified on reception of any data from remote OIC devices.
 *
 * @param[in]  sep         network endpoint description.
 * @param[in]  data          Data received from remote OIC device.
 * @param[in]  dataLength    Length of data in bytes.
 * @pre  Callback must be registered using CAIPSetPacketReceiveCallback().
 */
typedef void (*CAIPPacketReceivedCallback)(const CASecureEndpoint_t *sep,
                                           const void *data,
                                           size_t dataLength);

/**
  * Callback to notify error in the IP adapter.
  *
  * @param[in]  endpoint       network endpoint description.
  * @param[in]  data          Data sent/received.
  * @param[in]  dataLength    Length of data in bytes.
  * @param[in]  result        result of request from R.I.
  * @pre  Callback must be registered using CAIPSetPacketReceiveCallback().
 */
typedef void (*CAIPErrorHandleCallback)(const CAEndpoint_t *endpoint, const void *data,
                                        size_t dataLength, CAResult_t result);
#endif

/*
 * Enable or disable log for network changed event
 */
#if INTERFACE
#define NETWORK_INTERFACE_CHANGED_LOGGING 1

#define SELECT_TIMEOUT 1     // select() seconds (and termination latency)
#endif

#define IPv4_MULTICAST     "224.0.1.187"
static struct in_addr IPv4MulticastAddress = { 0 };

#define IPv6_DOMAINS       16
#define IPv6_MULTICAST_INT "ff01::158"
static struct in6_addr IPv6MulticastAddressInt;
#define IPv6_MULTICAST_LNK "ff02::158"
static struct in6_addr IPv6MulticastAddressLnk;
#define IPv6_MULTICAST_RLM "ff03::158"
static struct in6_addr IPv6MulticastAddressRlm;
#define IPv6_MULTICAST_ADM "ff04::158"
static struct in6_addr IPv6MulticastAddressAdm;
#define IPv6_MULTICAST_SIT "ff05::158"
static struct in6_addr IPv6MulticastAddressSit;
#define IPv6_MULTICAST_ORG "ff08::158"
static struct in6_addr IPv6MulticastAddressOrg;
#define IPv6_MULTICAST_GLB "ff0e::158"
static struct in6_addr IPv6MulticastAddressGlb;

/*
 * Buffer size for the receive message buffer
 */
#define RECV_MSG_BUF_LEN 16384

static char *ipv6mcnames[IPv6_DOMAINS] = {
    NULL,
    IPv6_MULTICAST_INT,
    IPv6_MULTICAST_LNK,
    IPv6_MULTICAST_RLM,
    IPv6_MULTICAST_ADM,
    IPv6_MULTICAST_SIT,
    NULL,
    NULL,
    IPv6_MULTICAST_ORG,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    IPv6_MULTICAST_GLB,
    NULL
};

/* #if defined (_WIN32)
 * #define IFF_UP_RUNNING_FLAGS  (IFF_UP)
 *
 *     char* caips_get_error(){
 *         static char buffer[32];
 *         snprintf(buffer, 32, "%i", WSAGetLastError());
 *         return buffer;
 *     }
 * #define CAIPS_GET_ERROR \
 *     caips_get_error()
 * #else */
/* #define IFF_UP_RUNNING_FLAGS  (IFF_UP|IFF_RUNNING) */

#if INTERFACE
#include <string.h>
#define CAIPS_GET_ERROR strerror(errno)
#endif

CAIPErrorHandleCallback g_ipErrorHandler = NULL;

CAIPPacketReceivedCallback g_packetReceivedCallback = NULL;

void CACloseFDs()
{
/* #if !defined(WSA_WAIT_EVENT_0)
 *     if (caglobals.ip.shutdownFds[0] != -1)
 *     {
 *         close(caglobals.ip.shutdownFds[0]);
 *         caglobals.ip.shutdownFds[0] = -1;
 *     }
 * #endif */
    CADeInitializeIPGlobals();
    CADeInitializeMonitorGlobals();
}

static void CAReceiveHandler(void *data)
{
    (void)data;

    while (!caglobals.ip.terminate)
    {
        CAFindReadyMessage();
    }
    CACloseFDs();
}

/* DELEGATE: void CAFindReadyMessage(); */

/* REMOVED: void CAUnregisterForAddressChanges() */

/* NEW DELEGATE: CADeInitializeMonitorGlobals(); */

void CADeInitializeIPGlobals()
{
    CLOSE_SOCKET(u6);
    CLOSE_SOCKET(u6s);
    CLOSE_SOCKET(u4);
    CLOSE_SOCKET(u4s);
    CLOSE_SOCKET(m6);
    CLOSE_SOCKET(m6s);
    CLOSE_SOCKET(m4);
    CLOSE_SOCKET(m4s);
    /* CAUnregisterForAddressChanges(); */
}

/* DELEGATE: static CAResult_t CAReceiveMessage(CASocketFd_t fd, CATransportFlags_t flags); */

void CAIPPullData()
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}

static CASocketFd_t CACreateSocket(int family, uint16_t *port, bool isMulticast)
{
    int socktype = SOCK_DGRAM;
#ifdef SOCK_CLOEXEC
    socktype |= SOCK_CLOEXEC;
#endif
    CASocketFd_t fd = socket(family, socktype, IPPROTO_UDP);
    if (OC_INVALID_SOCKET == fd)
    {
        OIC_LOG_V(ERROR, TAG, "create socket failed: %s", CAIPS_GET_ERROR);
        return OC_INVALID_SOCKET;
    }

#if !defined(SOCK_CLOEXEC) && defined(FD_CLOEXEC)
    int fl = fcntl(fd, F_GETFD);
    if (-1 == fl || -1 == fcntl(fd, F_SETFD, fl|FD_CLOEXEC))
    {
        OIC_LOG_V(ERROR, TAG, "set FD_CLOEXEC failed: %s", strerror(errno));
        close(fd);
        return OC_INVALID_SOCKET;
    }
#endif
    struct sockaddr_storage sa = { .ss_family = (short)family };
    socklen_t socklen = 0;

    if (family == AF_INET6)
    {
        int on = 1;

        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, OPTVAL_T(&on), sizeof (on)))
        {
            OIC_LOG_V(ERROR, TAG, "IPV6_V6ONLY failed: %s", CAIPS_GET_ERROR);
        }

#if defined(IPV6_RECVPKTINFO)
        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof (on)))
#else
        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IPV6, IPV6_PKTINFO, OPTVAL_T(&on), sizeof (on)))
#endif
        {
            OIC_LOG_V(ERROR, TAG, "IPV6_RECVPKTINFO failed: %s",CAIPS_GET_ERROR);
        }

        ((struct sockaddr_in6 *)&sa)->sin6_port = htons(*port);
        socklen = sizeof (struct sockaddr_in6);
    }
    else
    {
        int on = 1;
        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IP, IP_PKTINFO, OPTVAL_T(&on), sizeof (on)))
        {
            OIC_LOG_V(ERROR, TAG, "IP_PKTINFO failed: %s", CAIPS_GET_ERROR);
        }

        ((struct sockaddr_in *)&sa)->sin_port = htons(*port);
        socklen = sizeof (struct sockaddr_in);
    }

    if (isMulticast && *port) // use the given port
    {
        int on = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, OPTVAL_T(&on), sizeof (on)))
        {
            OIC_LOG_V(ERROR, TAG, "SO_REUSEADDR failed: %s", CAIPS_GET_ERROR);
            OC_CLOSE_SOCKET(fd);
            return OC_INVALID_SOCKET;
        }
    }

    if (OC_SOCKET_ERROR == bind(fd, (struct sockaddr *)&sa, socklen))
    {
	OIC_LOG_V(ERROR, TAG, "bind socket failed, port %d: %s", *port, CAIPS_GET_ERROR);
        OC_CLOSE_SOCKET(fd);
        return OC_INVALID_SOCKET;
    }

    if (!*port) // return the assigned port
    {
        if (OC_SOCKET_ERROR == getsockname(fd, (struct sockaddr *)&sa, &socklen))
        {
            OIC_LOG_V(ERROR, TAG, "getsockname failed: %s", CAIPS_GET_ERROR);
            OC_CLOSE_SOCKET(fd);
            return OC_INVALID_SOCKET;
        }
        *port = ntohs(family == AF_INET6 ?
                      ((struct sockaddr_in6 *)&sa)->sin6_port :
                      ((struct sockaddr_in *)&sa)->sin_port);
    }

    return fd;
}

/* DELEGATED: #define CHECKFD(FD) */

#define NEWSOCKET(FAMILY, NAME, MULTICAST) \
do \
{ \
    caglobals.ip.NAME.fd = CACreateSocket(FAMILY, &caglobals.ip.NAME.port, MULTICAST); \
    if (caglobals.ip.NAME.fd == OC_INVALID_SOCKET) \
    {   \
        caglobals.ip.NAME.port = 0; \
        caglobals.ip.NAME.fd = CACreateSocket(FAMILY, &caglobals.ip.NAME.port, MULTICAST); \
    }   \
    CHECKFD(caglobals.ip.NAME.fd); \
} while(0)

/* DELEGATE: static void CARegisterForAddressChanges(); */

/* DELEGATE: static void CAInitializeFastShutdownMechanism(); */

CAResult_t CAIPStartServer(const ca_thread_pool_t threadPool)
{
    CAResult_t res = CA_STATUS_OK;

    if (caglobals.ip.started)
    {
        return res;
    }
    if (!IPv4MulticastAddress.s_addr)
    {
        (void)inet_pton(AF_INET, IPv4_MULTICAST, &IPv4MulticastAddress);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_INT, &IPv6MulticastAddressInt);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_LNK, &IPv6MulticastAddressLnk);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_RLM, &IPv6MulticastAddressRlm);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_ADM, &IPv6MulticastAddressAdm);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_SIT, &IPv6MulticastAddressSit);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_ORG, &IPv6MulticastAddressOrg);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_GLB, &IPv6MulticastAddressGlb);
    }

    if (!caglobals.ip.ipv6enabled && !caglobals.ip.ipv4enabled)
    {
        caglobals.ip.ipv4enabled = true;  // only needed to run CA tests
    }

    if (caglobals.ip.ipv6enabled)
    {
        NEWSOCKET(AF_INET6, u6, false);
        NEWSOCKET(AF_INET6, u6s, false);
        NEWSOCKET(AF_INET6, m6, true);
        NEWSOCKET(AF_INET6, m6s, true);
        OIC_LOG_V(INFO, TAG, "IPv6 unicast port: %u", caglobals.ip.u6.port);
    }
    if (caglobals.ip.ipv4enabled)
    {
        NEWSOCKET(AF_INET, u4, false);
        NEWSOCKET(AF_INET, u4s, false);
        NEWSOCKET(AF_INET, m4, true);
        NEWSOCKET(AF_INET, m4s, true);
        OIC_LOG_V(INFO, TAG, "IPv4 unicast port: %u", caglobals.ip.u4.port);
    }

    OIC_LOG_V(DEBUG, TAG,
              "socket summary: u6=%d, u6s=%d, u4=%d, u4s=%d, m6=%d, m6s=%d, m4=%d, m4s=%d",
              caglobals.ip.u6.fd, caglobals.ip.u6s.fd, caglobals.ip.u4.fd, caglobals.ip.u4s.fd,
              caglobals.ip.m6.fd, caglobals.ip.m6s.fd, caglobals.ip.m4.fd, caglobals.ip.m4s.fd);

    OIC_LOG_V(DEBUG, TAG,
              "port summary: u6 port=%d, u6s port=%d, u4 port=%d, u4s port=%d, m6 port=%d,"
              "m6s port=%d, m4 port=%d, m4s port=%d",
              caglobals.ip.u6.port, caglobals.ip.u6s.port, caglobals.ip.u4.port,
              caglobals.ip.u4s.port, caglobals.ip.m6.port, caglobals.ip.m6s.port,
              caglobals.ip.m4.port, caglobals.ip.m4s.port);
 /* FIXME: windows: */
#if defined (SIO_GET_EXTENSION_FUNCTION_POINTER)
    caglobals.ip.wsaRecvMsg = NULL;
    GUID GuidWSARecvMsg = WSAID_WSARECVMSG;
    DWORD copied = 0;
    int err = WSAIoctl(caglobals.ip.u4.fd, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidWSARecvMsg, sizeof(GuidWSARecvMsg), &(caglobals.ip.wsaRecvMsg), sizeof(caglobals.ip.wsaRecvMsg), &copied, 0, 0);
    if (0 != err)
    {
        OIC_LOG_V(ERROR, TAG, "WSAIoctl failed %i", WSAGetLastError());
        return CA_STATUS_FAILED;
    }
#endif
    // set up appropriate FD mechanism for fast shutdown
    CAInitializeFastShutdownMechanism();

    // create source of network address change notifications
    CARegisterForAddressChanges();

    caglobals.ip.selectTimeout = CAGetPollingInterval(caglobals.ip.selectTimeout);

    res = CAIPStartListenServer();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to start listening server![%d]", res);
        return res;
    }

    caglobals.ip.terminate = false;
    res = ca_thread_pool_add_task(threadPool, CAReceiveHandler, NULL);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread_pool_add_task failed");
        return res;
    }
    OIC_LOG(DEBUG, TAG, "CAReceiveHandler thread started successfully.");

    caglobals.ip.started = true;
    return CA_STATUS_OK;
}

/* DELEGATE: void CAIPStopServer(); */

/* DELEGATE: void CAWakeUpForChange(); */

void applyMulticastToInterface4(uint32_t ifindex)
{
    if (!caglobals.ip.ipv4enabled)
    {
        return;
    }

#if defined(USE_IP_MREQN)
    struct ip_mreqn mreq = { .imr_multiaddr = IPv4MulticastAddress,
                             .imr_address.s_addr = htonl(INADDR_ANY),
                             .imr_ifindex = ifindex };
#else
    struct ip_mreq mreq  = { .imr_multiaddr.s_addr = IPv4MulticastAddress.s_addr,
                             .imr_interface.s_addr = htonl(ifindex) };
#endif

    int ret;
    ret = setsockopt(caglobals.ip.m4.fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, OPTVAL_T(&mreq), sizeof (mreq));
    if (OC_SOCKET_ERROR == ret)
    {
	if (PORTABLE_check_setsockopt_err())
        {
            OIC_LOG_V(ERROR, TAG, "       IPv4 IP_ADD_MEMBERSHIP failed: %s", CAIPS_GET_ERROR);
        }
    }
    ret = setsockopt(caglobals.ip.m4s.fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, OPTVAL_T(&mreq), sizeof (mreq));
    if (OC_SOCKET_ERROR == ret)
    {
	if (PORTABLE_check_setsockopt_m4s_err())
        {
            OIC_LOG_V(ERROR, TAG, "SECURE IPv4 IP_ADD_MEMBERSHIP failed: %s", CAIPS_GET_ERROR);
        }
    }
}

void applyMulticast6(CASocketFd_t fd, struct in6_addr *addr, uint32_t ifindex)
{
    struct ipv6_mreq mreq = { .ipv6mr_interface = ifindex };

    // VS2013 has problems with struct copies inside struct initializers, so copy separately.
    mreq.ipv6mr_multiaddr = *addr;

    int ret = setsockopt(fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, OPTVAL_T(&mreq), sizeof (mreq));
    if (OC_SOCKET_ERROR == ret)
    {
	if (PORTABLE_check_setsockopt_m6_err())
        {
            OIC_LOG_V(ERROR, TAG, "IPv6 IPV6_JOIN_GROUP failed: %s", CAIPS_GET_ERROR);
        }
    }
}

void applyMulticastToInterface6(uint32_t ifindex)
{
    if (!caglobals.ip.ipv6enabled)
    {
        return;
    }
    //applyMulticast6(caglobals.ip.m6.fd, &IPv6MulticastAddressInt, ifindex);
    applyMulticast6(caglobals.ip.m6.fd, &IPv6MulticastAddressLnk, ifindex);
    applyMulticast6(caglobals.ip.m6.fd, &IPv6MulticastAddressRlm, ifindex);
    //applyMulticast6(caglobals.ip.m6.fd, &IPv6MulticastAddressAdm, ifindex);
    applyMulticast6(caglobals.ip.m6.fd, &IPv6MulticastAddressSit, ifindex);
    //applyMulticast6(caglobals.ip.m6.fd, &IPv6MulticastAddressOrg, ifindex);
    //applyMulticast6(caglobals.ip.m6.fd, &IPv6MulticastAddressGlb, ifindex);

    //applyMulticast6(caglobals.ip.m6s.fd, &IPv6MulticastAddressInt, ifindex);
    applyMulticast6(caglobals.ip.m6s.fd, &IPv6MulticastAddressLnk, ifindex);
    applyMulticast6(caglobals.ip.m6s.fd, &IPv6MulticastAddressRlm, ifindex);
    //applyMulticast6(caglobals.ip.m6s.fd, &IPv6MulticastAddressAdm, ifindex);
    applyMulticast6(caglobals.ip.m6s.fd, &IPv6MulticastAddressSit, ifindex);
    //applyMulticast6(caglobals.ip.m6s.fd, &IPv6MulticastAddressOrg, ifindex);
    //applyMulticast6(caglobals.ip.m6s.fd, &IPv6MulticastAddressGlb, ifindex);
}

CAResult_t CAIPStartListenServer()
{
    if (caglobals.ip.started)
    {
        OIC_LOG(DEBUG, TAG, "Adapter is started already");
        return CA_STATUS_OK;
    }

    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    u_arraylist_t *iflist = CAIPGetInterfaceInformation(0);
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "CAIPGetInterfaceInformation() failed: %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

    size_t len = u_arraylist_length(iflist);
    OIC_LOG_V(DEBUG, TAG, "IP network interfaces found: %" PRIuPTR, len);

    for (size_t i = 0; i < len; i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);

        if (!ifitem)
        {
            continue;
        }
        if ((ifitem->flags & IFF_UP_RUNNING_FLAGS) != IFF_UP_RUNNING_FLAGS)
        {
            continue;
        }
        if (ifitem->family == AF_INET)
        {
#if NETWORK_INTERFACE_CHANGED_LOGGING
            OIC_LOG_V(DEBUG, TAG, "Adding IPv4 interface(%i) to multicast group", ifitem->index);
#endif
            applyMulticastToInterface4(ifitem->index);
        }
        if (ifitem->family == AF_INET6)
        {
#if NETWORK_INTERFACE_CHANGED_LOGGING
            OIC_LOG_V(DEBUG, TAG, "Adding IPv6 interface(%i) to multicast group", ifitem->index);
#endif
            applyMulticastToInterface6(ifitem->index);
        }
    }

    u_arraylist_destroy(iflist);
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAIPStopListenServer()
{
    u_arraylist_t *iflist = CAIPGetInterfaceInformation(0);
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "Get interface info failed: %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

    size_t len = u_arraylist_length(iflist);
    OIC_LOG_V(DEBUG, TAG, "IP network interfaces found: %" PRIuPTR, len);

    for (size_t i = 0; i < len; i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);

        if (!ifitem)
        {
            continue;
        }
        if ((ifitem->flags & IFF_UP_RUNNING_FLAGS) != IFF_UP_RUNNING_FLAGS)
        {
            continue;
        }
        if (ifitem->family == AF_INET)
        {
            CLOSE_SOCKET(m4);
            CLOSE_SOCKET(m4s);
            OIC_LOG_V(DEBUG, TAG, "IPv4 network interface: %s cloed", ifitem->name);
        }
        if (ifitem->family == AF_INET6)
        {
            CLOSE_SOCKET(m6);
            CLOSE_SOCKET(m6s);
            OIC_LOG_V(DEBUG, TAG, "IPv6 network interface: %s", ifitem->name);
        }
    }
    u_arraylist_destroy(iflist);
    return CA_STATUS_OK;
}

void CAProcessNewInterface(CAInterface_t *ifitem)
{
    if (!ifitem)
    {
        OIC_LOG(DEBUG, TAG, "ifitem is null");
        return;
    }

    if (ifitem->family == AF_INET6)
    {
        OIC_LOG_V(DEBUG, TAG, "Adding a new IPv6 interface(%i) to multicast group", ifitem->index);
        applyMulticastToInterface6(ifitem->index);
    }
    if (ifitem->family == AF_INET)
    {
        OIC_LOG_V(DEBUG, TAG, "Adding a new IPv4 interface(%i) to multicast group", ifitem->index);
        applyMulticastToInterface4(ifitem->index);
    }
}

void CAIPSetPacketReceiveCallback(CAIPPacketReceivedCallback callback)
{
    g_packetReceivedCallback = callback;
}

static void sendData(CASocketFd_t fd, const CAEndpoint_t *endpoint,
                     const void *data, size_t dlen,
                     const char *cast, const char *fam)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);

    if (!endpoint)
    {
        OIC_LOG(DEBUG, TAG, "endpoint is null");
        if (g_ipErrorHandler)
        {
            g_ipErrorHandler(endpoint, data, dlen, CA_STATUS_INVALID_PARAM);
        }
        return;
    }

    (void)cast;  // eliminates release warning
    (void)fam;

    struct sockaddr_storage sock = { .ss_family = 0 };
    CAConvertNameToAddr(endpoint->addr, endpoint->port, &sock);

    socklen_t socklen = 0;
    if (sock.ss_family == AF_INET6)
    {
        socklen = sizeof(struct sockaddr_in6);
    }
    else
    {
        socklen = sizeof(struct sockaddr_in);
    }
    PORTABLE_sendto(fd, data, dlen, 0, (struct sockaddr *)&sock, socklen, endpoint, cast, fam);
}

static void sendMulticastData6(const u_arraylist_t *iflist,
                               CAEndpoint_t *endpoint,
                               const void *data, size_t datalen)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    if (!endpoint)
    {
        OIC_LOG(DEBUG, TAG, "endpoint is null");
        return;
    }

    int scope = endpoint->flags & CA_SCOPE_MASK;
    char *ipv6mcname = ipv6mcnames[scope];
    if (!ipv6mcname)
    {
        OIC_LOG_V(INFO, TAG, "IPv6 multicast scope invalid: %d", scope);
        return;
    }
    OICStrcpy(endpoint->addr, sizeof(endpoint->addr), ipv6mcname);
    CASocketFd_t fd = caglobals.ip.u6.fd;

    size_t len = u_arraylist_length(iflist);
    for (size_t i = 0; i < len; i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
        if (!ifitem)
        {
            continue;
        }
        if ((ifitem->flags & IFF_UP_RUNNING_FLAGS) != IFF_UP_RUNNING_FLAGS)
        {
            continue;
        }
        if (ifitem->family != AF_INET6)
        {
            continue;
        }

        int index = ifitem->index;
        if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_IF, OPTVAL_T(&index), sizeof (index)))
        {
            OIC_LOG_V(ERROR, TAG, "setsockopt6 failed: %s", CAIPS_GET_ERROR);
            return;
        }
        sendData(fd, endpoint, data, datalen, "multicast", "ipv6");
    }
}

static void sendMulticastData4(const u_arraylist_t *iflist,
                               CAEndpoint_t *endpoint,
                               const void *data, size_t datalen)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");

#if defined(USE_IP_MREQN)
    struct ip_mreqn mreq = { .imr_multiaddr = IPv4MulticastAddress,
                             .imr_address.s_addr = htonl(INADDR_ANY),
                             .imr_ifindex = 0};
#else
    struct ip_mreq mreq  = { .imr_multiaddr.s_addr = IPv4MulticastAddress.s_addr,
                             .imr_interface = {0}};
#endif

    OICStrcpy(endpoint->addr, sizeof(endpoint->addr), IPv4_MULTICAST);
    CASocketFd_t fd = caglobals.ip.u4.fd;

    size_t len = u_arraylist_length(iflist);
    for (size_t i = 0; i < len; i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
        if (!ifitem)
        {
            continue;
        }
        if ((ifitem->flags & IFF_UP_RUNNING_FLAGS) != IFF_UP_RUNNING_FLAGS)
        {
            continue;
        }
        if (ifitem->family != AF_INET)
        {
            continue;
        }
#if defined(USE_IP_MREQN)
        mreq.imr_ifindex = ifitem->index;
#else
        mreq.imr_interface.s_addr = htonl(ifitem->index);
#endif
        if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, OPTVAL_T(&mreq), sizeof (mreq)))
        {
            OIC_LOG_V(ERROR, TAG, "send IP_MULTICAST_IF failed: %s (using defualt)",
                    CAIPS_GET_ERROR);
        }
        sendData(fd, endpoint, data, datalen, "multicast", "ipv4");
    }
}

void CAIPSendData(CAEndpoint_t *endpoint, const void *data, size_t datalen,
                  bool isMulticast)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    bool isSecure = (endpoint->flags & CA_SECURE) != 0;

    if (isMulticast)
    {
        endpoint->port = isSecure ? CA_SECURE_COAP : CA_COAP;

        u_arraylist_t *iflist = CAIPGetInterfaceInformation(0);
        if (!iflist)
        {
            OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
            return;
        }

        if ((endpoint->flags & CA_IPV6) && caglobals.ip.ipv6enabled)
        {
            sendMulticastData6(iflist, endpoint, data, datalen);
        }
        if ((endpoint->flags & CA_IPV4) && caglobals.ip.ipv4enabled)
        {
            sendMulticastData4(iflist, endpoint, data, datalen);
        }

        u_arraylist_destroy(iflist);
    }
    else
    {
        if (!endpoint->port)    // unicast discovery
        {
            endpoint->port = isSecure ? CA_SECURE_COAP : CA_COAP;
        }

        CASocketFd_t fd;
        if (caglobals.ip.ipv6enabled && (endpoint->flags & CA_IPV6))
        {
            fd = isSecure ? caglobals.ip.u6s.fd : caglobals.ip.u6.fd;
#ifndef __WITH_DTLS__
            fd = caglobals.ip.u6.fd;
#endif
            sendData(fd, endpoint, data, datalen, "unicast", "ipv6");
        }
        if (caglobals.ip.ipv4enabled && (endpoint->flags & CA_IPV4))
        {
            fd = isSecure ? caglobals.ip.u4s.fd : caglobals.ip.u4.fd;
#ifndef __WITH_DTLS__
            fd = caglobals.ip.u4.fd;
#endif
            sendData(fd, endpoint, data, datalen, "unicast", "ipv4");
        }
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

CAResult_t CAGetIPInterfaceInformation(CAEndpoint_t **info, size_t *size)
{
    VERIFY_NON_NULL_MSG(info, TAG, "info is NULL");
    VERIFY_NON_NULL_MSG(size, TAG, "size is NULL");

    u_arraylist_t *iflist = CAIPGetInterfaceInformation(0);
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

#ifdef __WITH_DTLS__
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

        if ((ifitem->family == AF_INET6 && !caglobals.ip.ipv6enabled) ||
            (ifitem->family == AF_INET && !caglobals.ip.ipv4enabled))
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
    CAEndpoint_t *eps = (CAEndpoint_t *)OICCalloc(totalEndpoints, sizeof (CAEndpoint_t));
    if (!eps)
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

        if ((ifitem->family == AF_INET6 && !caglobals.ip.ipv6enabled) ||
            (ifitem->family == AF_INET && !caglobals.ip.ipv4enabled))
        {
            continue;
        }

        eps[j].adapter = CA_ADAPTER_IP;
        eps[j].ifindex = ifitem->index;

        if (ifitem->family == AF_INET6)
        {
            eps[j].flags = CA_IPV6;
            eps[j].port = caglobals.ip.u6.port;
        }
        else
        {
            eps[j].flags = CA_IPV4;
            eps[j].port = caglobals.ip.u4.port;
        }
        OICStrcpy(eps[j].addr, sizeof(eps[j].addr), ifitem->addr);

#ifdef __WITH_DTLS__
        j++;

        eps[j].adapter = CA_ADAPTER_IP;
        eps[j].ifindex = ifitem->index;

        if (ifitem->family == AF_INET6)
        {
            eps[j].flags = CA_IPV6 | CA_SECURE;
            eps[j].port = caglobals.ip.u6s.port;
        }
        else
        {
            eps[j].flags = CA_IPV4 | CA_SECURE;
            eps[j].port = caglobals.ip.u4s.port;
        }
        OICStrcpy(eps[j].addr, sizeof(eps[j].addr), ifitem->addr);
#endif
        j++;
    }

    *info = eps;
    *size = totalEndpoints;

    u_arraylist_destroy(iflist);

    return CA_STATUS_OK;
}

void CAIPSetErrorHandler(CAIPErrorHandleCallback errorHandleCallback)
{
    g_ipErrorHandler = errorHandleCallback;
}

CAResult_t CAGetLinkLocalZoneId(uint32_t ifindex, char **zoneId)
{
    return CAGetLinkLocalZoneIdInternal(ifindex, zoneId);
}
