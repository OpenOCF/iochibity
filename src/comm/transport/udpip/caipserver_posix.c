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

#include "caipserver_posix.h"

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
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
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#include <errno.h>
/* #ifdef __linux__
 * #include <linux/netlink.h>
 * #include <linux/rtnetlink.h>
 * #endif */

#if EXPORT_INTERFACE
#include <inttypes.h>
#endif

#define USE_IP_MREQN

/*
 * Logging tag for module name
 */
#define TAG "OIC_CA_IP_SERVER"

#define SELECT_TIMEOUT 1     // select() seconds (and termination latency)

#if EXPORT_INTERFACE
#define IFF_UP_RUNNING_FLAGS  (IFF_UP|IFF_RUNNING)

#define SET(TYPE, FDS) \
    if (caglobals.ip.TYPE.fd != OC_INVALID_SOCKET) \
    { \
        FD_SET(caglobals.ip.TYPE.fd, FDS); \
    }

#define ISSET(TYPE, FDS, FLAGS) \
    if (caglobals.ip.TYPE.fd != OC_INVALID_SOCKET && FD_ISSET(caglobals.ip.TYPE.fd, FDS)) \
    { \
        fd = caglobals.ip.TYPE.fd; \
        flags = FLAGS; \
    }
#endif	/* INTERFACE */

/* DELEGATE: void CAFindReadyMessage(); */

/* DELEGATE: void CADeInitializeMonitorGlobals(); */

CAResult_t CAReceiveMessage(CASocketFd_t fd, CATransportFlags_t flags)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    char recvBuffer[RECV_MSG_BUF_LEN] = {0};
    int level = 0;
    int type = 0;
    int namelen = 0;
    struct sockaddr_storage srcAddr = { .ss_family = 0 };
    unsigned char *pktinfo = NULL;
    size_t len = 0;
    struct cmsghdr *cmp = NULL;
    struct iovec iov = { .iov_base = recvBuffer, .iov_len = sizeof (recvBuffer) };
    union control
    {
        struct cmsghdr cmsg;
        unsigned char data[CMSG_SPACE(sizeof (struct in6_pktinfo))];
    } cmsg;

    if (flags & CA_IPV6)
    {
        namelen = sizeof (struct sockaddr_in6);
        level = IPPROTO_IPV6;
        type = IPV6_PKTINFO;
        len = sizeof (struct in6_pktinfo);
    }
    else
    {
        namelen = sizeof (struct sockaddr_in);
        level = IPPROTO_IP;
        type = IP_PKTINFO;
        len = sizeof (struct in6_pktinfo);
    }

    struct msghdr msg = { .msg_name = &srcAddr,
                          .msg_namelen = namelen,
                          .msg_iov = &iov,
                          .msg_iovlen = 1,
                          .msg_control = &cmsg,
                          .msg_controllen = CMSG_SPACE(len) };

    ssize_t recvLen = recvmsg(fd, &msg, flags);
    if (OC_SOCKET_ERROR == recvLen)
    {
        OIC_LOG_V(ERROR, TAG, "Recvfrom failed %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

    for (cmp = CMSG_FIRSTHDR(&msg); cmp != NULL; cmp = CMSG_NXTHDR(&msg, cmp))
    {
        if (cmp->cmsg_level == level && cmp->cmsg_type == type)
        {
            pktinfo = CMSG_DATA(cmp);
        }
    }
    if (!pktinfo)
    {
        OIC_LOG(ERROR, TAG, "pktinfo is null");
        return CA_STATUS_FAILED;
    }

    CASecureEndpoint_t sep = {.endpoint = {.adapter = CA_ADAPTER_IP, .flags = flags}};

    if (flags & CA_IPV6)
    {
        sep.endpoint.ifindex = ((struct in6_pktinfo *)pktinfo)->ipi6_ifindex;

        if (flags & CA_MULTICAST)
        {
            struct in6_addr *addr = &(((struct in6_pktinfo *)pktinfo)->ipi6_addr);
            unsigned char topbits = ((unsigned char *)addr)[0];
            if (topbits != 0xff)
            {
                sep.endpoint.flags &= ~CA_MULTICAST;
            }
        }
    }
    else
    {
        sep.endpoint.ifindex = ((struct in_pktinfo *)pktinfo)->ipi_ifindex;

        if (flags & CA_MULTICAST)
        {
            struct in_addr *addr = &((struct in_pktinfo *)pktinfo)->ipi_addr;
            uint32_t host = ntohl(addr->s_addr);
            unsigned char topbits = ((unsigned char *)&host)[3];
            if (topbits < 224 || topbits > 239)
            {
                sep.endpoint.flags &= ~CA_MULTICAST;
            }
        }
    }

    CAConvertAddrToName(&srcAddr, namelen, sep.endpoint.addr, &sep.endpoint.port);

    if (flags & CA_SECURE)
    {
#ifdef __WITH_DTLS__
#ifdef TB_LOG
        int decryptResult =
#endif
        CAdecryptSsl(&sep, (uint8_t *)recvBuffer, recvLen);
        OIC_LOG_V(DEBUG, TAG, "CAdecryptSsl returns [%d]", decryptResult);
#else
        OIC_LOG(ERROR, TAG, "Encrypted message but no DTLS");
#endif // __WITH_DTLS__
    }
    else
    {
        if (g_packetReceivedCallback)
        {
            g_packetReceivedCallback(&sep, recvBuffer, recvLen);
        }
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

#if EXPORT_INTERFACE
#define CHECKFD(FD) \
do \
{ \
    if (FD > caglobals.ip.maxfd) \
    { \
        caglobals.ip.maxfd = FD; \
    } \
} while (0)
#endif

/* DELEGATE: void CAInitializeFastShutdownMechanism(); */

void CAIPStopServer()
{
    caglobals.ip.terminate = true;

    if (caglobals.ip.shutdownFds[1] != -1)
    {
        close(caglobals.ip.shutdownFds[1]);
        caglobals.ip.shutdownFds[1] = -1;
        // receive thread will stop immediately
    }
    else
    {
        // receive thread will stop in SELECT_TIMEOUT seconds.
    }

    if (!caglobals.ip.started)
    { // Close fd's since receive handler was not started
        CACloseFDs();
    }
    caglobals.ip.started = false;
}

void CAWakeUpForChange()
{
    if (caglobals.ip.shutdownFds[1] != -1)
    {
        ssize_t len = 0;
        do
        {
            len = write(caglobals.ip.shutdownFds[1], "w", 1);
        } while ((len == -1) && (errno == EINTR));
        if ((len == -1) && (errno != EINTR) && (errno != EPIPE))
        {
            OIC_LOG_V(DEBUG, TAG, "write failed: %s", strerror(errno));
        }
    }
}

bool PORTABLE_check_setsockopt_err()
{
    return EADDRINUSE != errno;
}

bool PORTABLE_check_setsockopt_m4s_err(struct ip_mreqn mreq, int ret)
{
    return EADDRINUSE != errno;
}

bool PORTABLE_check_setsockopt_m6_err(fd, mreq, ret)
{
    return EADDRINUSE != errno;
}

void PORTABLE_sendto(CASocketFd_t fd,
                     const void *data,
		     size_t dlen,
		     int flags,
		     struct sockaddr * sockaddrptr,
		     socklen_t socklen,
		     const CAEndpoint_t *endpoint,
		     const char *cast, const char *fam)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
#ifdef TB_LOG
    const char *secure = (endpoint->flags & CA_SECURE) ? "secure " : "insecure ";
#endif
    ssize_t len = sendto(fd, data, dlen, 0, sockaddrptr, socklen);
    if (OC_SOCKET_ERROR == len)
    {
         // If logging is not defined/enabled.
        if (g_ipErrorHandler)
        {
            g_ipErrorHandler(endpoint, data, dlen, CA_SEND_FAILED);
        }
        OIC_LOG_V(ERROR, TAG, "%s%s %s sendTo failed: %s", secure, cast, fam, strerror(errno));
        CALogSendStateInfo(endpoint->adapter, endpoint->addr, endpoint->port,
                           len, false, strerror(errno));
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "%s%s %s sendTo is successful: %zd bytes", secure, cast, fam, len);
        CALogSendStateInfo(endpoint->adapter, endpoint->addr, endpoint->port,
                           len, true, NULL);
    }
}

CAResult_t CAGetLinkLocalZoneIdInternal(uint32_t ifindex, char **zoneId)
{
    if (!zoneId || (*zoneId != NULL))
    {
        return CA_STATUS_INVALID_PARAM;
    }

    *zoneId = (char *)OICCalloc(IF_NAMESIZE, sizeof(char));
    if (!(*zoneId))
    {
        OIC_LOG(ERROR, TAG, "OICCalloc failed in CAGetLinkLocalZoneIdInternal");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (!if_indextoname(ifindex, *zoneId))
    {
        OIC_LOG(ERROR, TAG, "if_indextoname failed in CAGetLinkLocalZoneIdInternal");
        OICFree(*zoneId);
        *zoneId = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "Given ifindex is %d parsed zoneId is %s", ifindex, *zoneId);
    return CA_STATUS_OK;
}
