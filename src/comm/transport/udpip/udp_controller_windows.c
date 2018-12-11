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

#include "udp_controller_windows.h"

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

/* #define USE_IP_MREQN
 * #if defined(_WIN32)
 * #undef USE_IP_MREQN
 * #endif */

/*
 * Logging tag for module name
 */
#define TAG "OIC_CA_IP_SERVER"

/* WSAEVENT udp_addressChangeEvent;/\**< Event used to signal address changes *\/ */
/* WSAEVENT udp_shutdownEvent;     /\**< Event used to signal threads to stop *\/ */

#define USE_IP_MREQN
#if defined(_WIN32)
#undef USE_IP_MREQN
#endif

/* #if EXPORT_INTERFACE */
/* #define IFF_UP_RUNNING_FLAGS  (IFF_UP) */

/* typedef int socklen_t; */
/* #endif */

char* caips_get_error(){
    static char buffer[32];
    snprintf(buffer, 32, "%i", WSAGetLastError());
    return buffer;
}
#define CAIPS_GET_ERROR \
    caips_get_error()


