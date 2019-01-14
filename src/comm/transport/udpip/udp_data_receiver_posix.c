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

#include "udp_data_receiver_posix.h"

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

#if INTERFACE
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#endif

#ifdef HAVE_NETINET_IP_H
#include <netinet/ip.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif

#include <string.h>
#include <errno.h>

#if INTERFACE
#include <inttypes.h>
#endif

/*
 * Logging tag for module name
 */
#define TAG "OIC_CA_IP_SERVER"

#define SELECT_TIMEOUT 1     // select() seconds (and termination latency)

/* #if INTERFACE */
/* #define IFF_UP_RUNNING_FLAGS  (IFF_UP|IFF_RUNNING) */

/* #define SET(TYPE, FDS) \ */
/*     if (TYPE.fd != OC_INVALID_SOCKET) \ */
/*     { \ */
/*         FD_SET(TYPE.fd, FDS); \ */
/*     } */

/* #define ISSET(TYPE, FDS, FLAGS) \ */
/*     if (TYPE.fd != OC_INVALID_SOCKET && FD_ISSET(TYPE.fd, FDS)) \ */
/*     { \ */
/*         fd = TYPE.fd; \ */
/*         flags = FLAGS; \ */
/*     } */
/* #endif	/\* INTERFACE *\/ */

/* DELEGATE: void CAFindReadyMessage(); */

/* DELEGATE: void CADeInitializeMonitorGlobals(); */

#if EXPORT_INTERFACE
// @rewrite:: use <transport>_* instead of caglobals
#define UDP_CHECKFD(FD) \
do \
{ \
    if (FD > udp_maxfd) \
    { \
        udp_maxfd = FD; \
    } \
} while (0)
#endif

/* DELEGATE: void CAInitializeFastShutdownMechanism(); */

CAResult_t udp_recvmsg_on_socket(CASocketFd_t fd, CATransportFlags_t flags) // @was CAReceiveMessage
{
    /* pktinfo - see RFC 3542 */
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY flags: 0x%04x", __func__, flags);
    char recvBuffer[RECV_MSG_BUF_LEN] = {0};
    int level = 0;
    int type = 0;
    int namelen = 0;
    struct sockaddr_storage origin_addr = { .ss_family = 0 };
    unsigned char *pktinfo = NULL;
    size_t len = 0;
    struct cmsghdr *cmsg_hdr = NULL;
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
        len = sizeof (struct in_pktinfo);
    }

    struct msghdr msg = { .msg_name = &origin_addr,
                          .msg_namelen = namelen,
                          .msg_iov = &iov,
                          .msg_iovlen = 1,
                          .msg_control = &cmsg,
                          .msg_controllen = CMSG_SPACE(len) };

    ssize_t recvLen = recvmsg(fd, &msg, 0); // flags);
    if (OC_SOCKET_ERROR == recvLen)
    {
        OIC_LOG_V(ERROR, TAG, "Recvfrom failed %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

    for (cmsg_hdr = CMSG_FIRSTHDR(&msg); cmsg_hdr != NULL; cmsg_hdr = CMSG_NXTHDR(&msg, cmsg_hdr))
    {
        if (cmsg_hdr->cmsg_level == level && cmsg_hdr->cmsg_type == type)
        {
            pktinfo = CMSG_DATA(cmsg_hdr);
            break;
        }
    }
    if (!pktinfo)
    {
        OIC_LOG(ERROR, TAG, "pktinfo is null");
        return CA_STATUS_FAILED;
    }

    //#ifdef DEBUG_DGRAM
    // MSG_MCAST BSD only?
    /* OIC_LOG_V(DEBUG, TAG, "msghdr.msg_flags MSG_MCAST? %x", msg.msg_flags & MSG_MCAST); */

    char addr_str[256] = {0}; // debugging
    if (cmsg_hdr->cmsg_level == IPPROTO_IPV6) {
        OIC_LOG(DEBUG, TAG, "cmsg_level == IPPROTO_IPV6");
        OIC_LOG_V(DEBUG, TAG, "cmsg_type == IPV6_PKTINFO");
        inet_ntop(AF_INET6,
                  &(((struct in6_pktinfo*)pktinfo)->ipi6_addr),
                  addr_str, sizeof(addr_str));
        OIC_LOG_V(DEBUG, TAG, "dest addr: %s", addr_str);
        OIC_LOG_V(DEBUG, TAG, "dest ifindex: %u", ((struct in6_pktinfo*)pktinfo)->ipi6_ifindex);
    }
    //#endif
    CASecureEndpoint_t origin_sep = {.endpoint = {.adapter = CA_ADAPTER_IP, .flags = flags}};
    //LogSecureEndpoint(&origin_sep);

    // CA_IPV6 flag was set if socket was ipv6
    // we don't need the, the message tells us the protocol family
    /* if (flags & CA_IPV6)        /\* FIXME: use family from origin_addr? *\/ */
    if (cmsg_hdr->cmsg_level == IPPROTO_IPV6) {
        origin_sep.endpoint.flags |= CA_IPV6;
        origin_sep.endpoint.ifindex = ((struct in6_pktinfo *)pktinfo)->ipi6_ifindex;

        // if msg was received on mcast socket, flag was set CA_IPV6
        // we dont't qneed that, we can test the address
        /* if (flags & CA_MULTICAST) { /\* FIXME: use MSG_MCAST flag? *\/ */
        //struct in6_addr *addr = &(((struct in6_pktinfo *)pktinfo)->ipi6_addr);
        /* unsigned char topbits = ((unsigned char *)addr)[0]; */
        if (IN6_IS_ADDR_MULTICAST(&(((struct in6_pktinfo *)pktinfo)->ipi6_addr))) {
            OIC_LOG_V(DEBUG, TAG, "msg was multicast");
            /* NOTE: this flag indicates that the msg rcd from
               origin_ep was mcast, not that the origin_ep is an mcast
               ep */
            origin_sep.endpoint.flags |= CA_MULTICAST;
        } else {
            OIC_LOG_V(DEBUG, TAG, "msg was unicast");
            origin_sep.endpoint.flags &= ~CA_MULTICAST;
        }
        /* if (topbits == 0xff) { */
        /*     OIC_LOG_V(DEBUG, TAG, "msg was multicast"); */
        /*     origin_sep.endpoint.flags |= CA_MULTICAST; */
        /* } else { */
        /*     OIC_LOG_V(DEBUG, TAG, "msg was unicast"); */
        /*     origin_sep.endpoint.flags |= ~CA_MULTICAST; */
        /* } */

        /* origin_addr is origin_ep */
        struct sockaddr_in6 *origin_addr6 = (struct sockaddr_in6*)&origin_addr;

        if (IN6_IS_ADDR_LINKLOCAL(&origin_addr6->sin6_addr)) {
                                  //&(((struct in6_pktinfo *)pktinfo)->ipi6_addr.s6_addr))) {
            //OIC_LOG_V(DEBUG, TAG, "ipv6 scope linklocal");
            origin_sep.endpoint.flags |= CA_SCOPE_LINK;
        } else if (IN6_IS_ADDR_SITELOCAL(&origin_addr6->sin6_addr)) {
                    // &(((struct in6_pktinfo *)pktinfo)->ipi6_addr))) {
            //OIC_LOG_V(DEBUG, TAG, "ipv6 scope sitelocal");
            origin_sep.endpoint.flags |= CA_SCOPE_SITE;
        } else if (IN6_IS_ADDR_UNSPECIFIED(&origin_addr6->sin6_addr)) {
                    // &(((struct in6_pktinfo *)pktinfo)->ipi6_addr))) {
            //OIC_LOG_V(DEBUG, TAG, "ipv6 scope unspecified");
        }

        /* NOTE: multicast scope testing: IN6_IS_ADDR_MC_LINKLOCAL(a) */

        inet_ntop(AF_INET6, &(origin_addr6->sin6_addr),
                  origin_sep.endpoint.addr, sizeof(origin_sep.endpoint.addr));
        origin_sep.endpoint.port = ntohs(((struct sockaddr_in6 *)&origin_addr)->sin6_port);
        /* } */
    } else if (cmsg_hdr->cmsg_level == IPPROTO_IP) {
        origin_sep.endpoint.ifindex = ((struct in_pktinfo *)pktinfo)->ipi_ifindex;
        origin_sep.endpoint.flags |= CA_IPV4;

        struct in_addr *addr = &((struct in_pktinfo *)pktinfo)->ipi_addr;
        uint32_t host = ntohl(addr->s_addr);
        unsigned char topbits = ((unsigned char *)&host)[3];
        if (topbits < 224 || topbits > 239) {
                origin_sep.endpoint.flags |= ~CA_MULTICAST;
        } else {
                origin_sep.endpoint.flags |= CA_MULTICAST;
        }
        inet_ntop(AF_INET,
                  &(((struct sockaddr_in*)&origin_addr)->sin_addr),
                  origin_sep.endpoint.addr, sizeof(origin_sep.endpoint.addr));
        origin_sep.endpoint.port = ntohs(((struct sockaddr_in *)&origin_addr)->sin_port);
    }

    //CAConvertAddrToName(&origin_addr, namelen, origin_sep.endpoint.addr, &origin_sep.endpoint.port);

    LogSecureEndpoint(&origin_sep);

    if (flags & CA_SECURE) {
#ifdef __WITH_DTLS__
#ifdef DEBUG_TLS
        int decryptResult =
#endif
	    /*  */
        CAdecryptSsl(&origin_sep, (uint8_t *)recvBuffer, recvLen);
        OIC_LOG_TLS_V(DEBUG, TAG, "CAdecryptSsl returns [%d]", decryptResult);
#else
        OIC_LOG(ERROR, TAG, "Encrypted message but DTLS disabled");
#endif // __WITH_DTLS__
    }
    else
    {
        /* if (g_packetReceivedCallback) */
        /* { */
        /*     g_packetReceivedCallback(&origin_sep, recvBuffer, recvLen); */
        /* } */
	mh_CAReceivedPacketCallback(&origin_sep, recvBuffer, recvLen);
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

/* void CAWakeUpForChange(void) */
/* { */
/*     if (udp_shutdownFds[1] != -1) */
/*     { */
/*         ssize_t len = 0; */
/*         do */
/*         { */
/*             len = write(udp_shutdownFds[1], "w", 1); */
/*         } while ((len == -1) && (errno == EINTR)); */
/*         if ((len == -1) && (errno != EINTR) && (errno != EPIPE)) */
/*         { */
/*             OIC_LOG_V(DEBUG, TAG, "write failed: %s", strerror(errno)); */
/*         } */
/*     } */
/* } */

