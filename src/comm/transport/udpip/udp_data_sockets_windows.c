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

#include "udp_data_sockets_windows.h"

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if EXPORT_INTERFACE
#include <winsock2.h>
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
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif

#include <errno.h>
/* #ifdef __linux__ */
/* #include <linux/netlink.h> */
/* #include <linux/rtnetlink.h> */
/* #endif */

/* #include <coap/pdu.h> */

#if EXPORT_INTERFACE
#include <inttypes.h>
#include <Ws2tcpip.h>
#endif

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
WSAEVENT addressChangeEvent;/**< Event used to signal address changes */
WSAEVENT shutdownEvent;     /**< Event used to signal threads to stop */

bool PORTABLE_check_setsockopt_err(void) EXPORT
{ return WSAEINVAL != WSAGetLastError(); }

bool PORTABLE_check_setsockopt_m4s_err(struct ip_mreq *mreq, int ret) EXPORT
{
    if (WSAEINVAL == WSAGetLastError())
        {
            // We're trying to join the multicast group again.
            // If the interface has gone down and come back up, the socket might be in
            // an inconsistent state where it still thinks we're joined when the interface
            // doesn't think we're joined.  So try to leave and rejoin the group just in case.
            setsockopt(udp_m4s.fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, OPTVAL_T(&mreq), sizeof(mreq));
            ret = setsockopt(udp_m4s.fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, OPTVAL_T(&mreq), sizeof(mreq));
        }
    return (OC_SOCKET_ERROR == ret);
}

bool PORTABLE_check_setsockopt_m6_err(CASocketFd_t fd, struct ipv6_mreq *mreq,  int ret) EXPORT
// bool PORTABLE_check_setsockopt_m6_err(fd, mreq, ret)
{
        if (WSAEINVAL == WSAGetLastError())
        {
            // We're trying to join the multicast group again.
            // If the interface has gone down and come back up, the socket might be in
            // an inconsistent state where it still thinks we're joined when the interface
            // doesn't think we're joined.  So try to leave and rejoin the group just in case.
            setsockopt(fd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, OPTVAL_T(&mreq), sizeof(mreq));
            ret = setsockopt(fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, OPTVAL_T(&mreq), sizeof(mreq));
        }
	return (OC_SOCKET_ERROR == ret);
}
