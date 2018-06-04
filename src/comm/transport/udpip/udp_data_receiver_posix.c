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
#include <errno.h>

#if INTERFACE
#include <inttypes.h>
#endif

#define USE_IP_MREQN

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

void CAIPStopServer()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    udp_is_terminating = true;

    if (udp_shutdownFds[1] != -1)
    {
        close(udp_shutdownFds[1]);
        udp_shutdownFds[1] = -1;
        // receive thread will stop immediately
    }
    else
    {
        // receive thread will stop in SELECT_TIMEOUT seconds.
    }

    if (!udp_is_started)
    { // Close fd's since receive handler was not started
	udp_cleanup();  // @rewrite @was CACloseFDs();
    }
    udp_is_started = false;
}

void CAWakeUpForChange()
{
    if (udp_shutdownFds[1] != -1)
    {
        ssize_t len = 0;
        do
        {
            len = write(udp_shutdownFds[1], "w", 1);
        } while ((len == -1) && (errno == EINTR));
        if ((len == -1) && (errno != EINTR) && (errno != EPIPE))
        {
            OIC_LOG_V(DEBUG, TAG, "write failed: %s", strerror(errno));
        }
    }
}

CAResult_t CAGetLinkLocalZoneIdInternal(uint32_t ifindex, char **zoneId)
EXPORT
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
