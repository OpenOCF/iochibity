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

#include "udp_data_sockets_posix.h"

#include <sys/types.h>
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
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IP_H
#include <netinet/ip.h>
#endif
#endif  /* INTERFACE */
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif

#include <errno.h>
#ifdef __linux__
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

/* #include <coap/pdu.h> */

#ifndef __APPLE__
int udp_netlinkFd;              /**< netlink */
#endif

int udp_shutdownFds[2]; // = { 80, 81 }; /**< pipe used to signal threads to stop */
CASocketFd_t udp_maxfd = 0;         /**< highest fd (for select) */

#if INTERFACE
#include <inttypes.h>
#define IFF_UP_RUNNING_FLAGS  (IFF_UP|IFF_RUNNING)
#endif

bool PORTABLE_check_setsockopt_err(void)
{
    return EADDRINUSE != errno;
}

bool PORTABLE_check_setsockopt_m4s_err(struct ip_mreqn *mreq, int ret)
{
    /* args not used in posix, used in windows */
    (void)mreq;
    (void)ret;
    return EADDRINUSE != errno;
}

bool PORTABLE_check_setsockopt_m6_err(CASocketFd_t fd, struct ipv6_mreq *mreq, int ret)
{
    /* args not used in posix, used in windows */
    (void)fd;
    (void)mreq;
    (void)ret;
    return EADDRINUSE != errno;
}
void CAUnregisterForAddressChanges(void)
{
}

