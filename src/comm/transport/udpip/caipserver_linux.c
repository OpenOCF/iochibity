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

#include "caipserver_linux.h"

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
/* POSIX.1-2001, POSIX.1-2008 */
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
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
/* #endif */

#include <inttypes.h>

#define USE_IP_MREQN

/*
 * Logging tag for module name
 */
#define TAG "OIC_LINUX_CA_IP_SERVER"

/* This routine is misnamed. It monitors network FDs and processes
   (reads and dispatches) when ready */
void CAFindReadyMessage()
{
    fd_set readFds;
    struct timeval timeout;

    timeout.tv_sec = caglobals.ip.selectTimeout;
    timeout.tv_usec = 0;
    struct timeval *tv = caglobals.ip.selectTimeout == -1 ? NULL : &timeout;

    FD_ZERO(&readFds);
    SET(u6,  &readFds)
    SET(u6s, &readFds)
    SET(u4,  &readFds)
    SET(u4s, &readFds)
    SET(m6,  &readFds)
    SET(m6s, &readFds)
    SET(m4,  &readFds)
    SET(m4s, &readFds)

    if (caglobals.ip.shutdownFds[0] != -1)
    {
        FD_SET(caglobals.ip.shutdownFds[0], &readFds);
    }
    if (caglobals.ip.netlinkFd != OC_INVALID_SOCKET)
    {
        FD_SET(caglobals.ip.netlinkFd, &readFds);
    }

    int ret = select(caglobals.ip.maxfd + 1, &readFds, NULL, NULL, tv);

    if (caglobals.ip.terminate)
    {
        OIC_LOG_V(DEBUG, TAG, "Packet receiver Stop request received.");
        return;
    }

    if (0 == ret)
    {
        return;
    }
    else if (0 < ret)
    {
        CASelectReturned(&readFds, ret);
    }
    else // if (0 > ret)
    {
        OIC_LOG_V(FATAL, TAG, "select error %s", CAIPS_GET_ERROR);
        return;
    }
}

/* process FDs that are ready for reading */
LOCAL void CASelectReturned(fd_set *readFds, int ret)
{
    (void)ret;			/* ret = fd count */
    CASocketFd_t fd = OC_INVALID_SOCKET;
    CATransportFlags_t flags = CA_DEFAULT_FLAGS;

    while (!caglobals.ip.terminate)
    {
        ISSET(u6,  readFds, CA_IPV6)
        else ISSET(u6s, readFds, CA_IPV6 | CA_SECURE)
        else ISSET(u4,  readFds, CA_IPV4)
        else ISSET(u4s, readFds, CA_IPV4 | CA_SECURE)
        else ISSET(m6,  readFds, CA_MULTICAST | CA_IPV6)
        else ISSET(m6s, readFds, CA_MULTICAST | CA_IPV6 | CA_SECURE)
        else ISSET(m4,  readFds, CA_MULTICAST | CA_IPV4)
        else ISSET(m4s, readFds, CA_MULTICAST | CA_IPV4 | CA_SECURE)
        else if ((caglobals.ip.netlinkFd != OC_INVALID_SOCKET) && FD_ISSET(caglobals.ip.netlinkFd, readFds))
        {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
            OIC_LOG_V(DEBUG, TAG, "Rtnetlink event detected");
#endif
            u_arraylist_t *iflist = CAFindInterfaceChange();
            if (iflist)
            {
                size_t listLength = u_arraylist_length(iflist);
                for (size_t i = 0; i < listLength; i++)
                {
                    CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
                    if (ifitem)
                    {
                        CAProcessNewInterface(ifitem);
                    }
                }
                u_arraylist_destroy(iflist);
            }
            break;
        }
        else if (FD_ISSET(caglobals.ip.shutdownFds[0], readFds))
        {
            char buf[10] = {0};
            ssize_t len = read(caglobals.ip.shutdownFds[0], buf, sizeof (buf));
            if (-1 == len)
            {
                continue;
            }
            break;
        }
        else
        {
            break;
        }
	/* at this point, any fd ready for reading, and flags, have been set by the ISSET macros above */
        (void)CAReceiveMessage(fd, flags);
        FD_CLR(fd, readFds);
    }
}

void CADeInitializeMonitorGlobals()
{
    if (caglobals.ip.netlinkFd != OC_INVALID_SOCKET)
    {
        close(caglobals.ip.netlinkFd);
        caglobals.ip.netlinkFd = OC_INVALID_SOCKET;
    }

    if (caglobals.ip.shutdownFds[0] != -1)
    {
        close(caglobals.ip.shutdownFds[0]);
        caglobals.ip.shutdownFds[0] = -1;
    }
}

void CARegisterForAddressChanges()
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    caglobals.ip.netlinkFd = OC_INVALID_SOCKET;
    // create NETLINK fd for interface change notifications
    struct sockaddr_nl sa = { AF_NETLINK, 0, 0,
                              RTMGRP_LINK /* nw interface create/delete/up/down events */
			      | RTMGRP_IPV4_IFADDR /* ipv4 address add/delete */
			      | RTMGRP_IPV6_IFADDR /* ipv6 address add/delete */
    };

    caglobals.ip.netlinkFd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_ROUTE);
    if (caglobals.ip.netlinkFd == OC_INVALID_SOCKET)
    {
        OIC_LOG_V(ERROR, TAG, "netlink socket failed: %s", strerror(errno));
    }
    else
    {
        int r = bind(caglobals.ip.netlinkFd, (struct sockaddr *)&sa, sizeof (sa));
        if (r)
        {
            OIC_LOG_V(ERROR, TAG, "netlink bind failed: %s", strerror(errno));
            close(caglobals.ip.netlinkFd);
            caglobals.ip.netlinkFd = OC_INVALID_SOCKET;
        }
        else
        {
            CHECKFD(caglobals.ip.netlinkFd);
        }
    }
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}

void CAInitializeFastShutdownMechanism()
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    caglobals.ip.selectTimeout = -1; // don't poll for shutdown
    int ret = -1;
    ret = pipe2(caglobals.ip.shutdownFds, O_CLOEXEC);
    CHECKFD(caglobals.ip.shutdownFds[0]);
    CHECKFD(caglobals.ip.shutdownFds[1]);
    if (-1 == ret)
    {
        OIC_LOG_V(ERROR, TAG, "fast shutdown mechanism init failed: %s", CAIPS_GET_ERROR);
        caglobals.ip.selectTimeout = SELECT_TIMEOUT; //poll needed for shutdown
    }
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}
