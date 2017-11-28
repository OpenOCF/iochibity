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

#include "caipserver_darwin.h"

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

#include <inttypes.h>

#define USE_IP_MREQN

/*
 * Logging tag for module name
 */
#define TAG "OIC_DARWIN_CA_IP_SERVER"

/* FIXME: this works for data messages but not for network status
 change messages (netlinkFd).  use mac stuff for that instead of
 netlink. */
void CAFindReadyMessage()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
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
	OIC_LOG_V(DEBUG, TAG, "%s select expired", __func__);
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

/* FIXME: make it work for macOS - netlinkFd doesn't break but it doesn't work on os x */
void CASelectReturned(fd_set *readFds, int ret)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    (void)ret;
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
	/* not implemented - netlinkFd is always INVALID SOCKET on OS X */
        /* else if ((caglobals.ip.netlinkFd != OC_INVALID_SOCKET) && FD_ISSET(caglobals.ip.netlinkFd, readFds)) */
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
        (void)CAReceiveMessage(fd, flags);
        FD_CLR(fd, readFds);
    }
}

void CADeInitializeMonitorGlobals()
{
    /* Currently, same as for posix. that will change */
    if (caglobals.ip.shutdownFds[0] != -1)
    {
        close(caglobals.ip.shutdownFds[0]);
        caglobals.ip.shutdownFds[0] = -1;
    }
    /* FIXME: this is dummy code that does not work on os x (netlink) */
    if (caglobals.ip.netlinkFd != OC_INVALID_SOCKET)
    {
        close(caglobals.ip.netlinkFd);
        caglobals.ip.netlinkFd = OC_INVALID_SOCKET;
    }
}

void CARegisterForAddressChanges()
{
    OIC_LOG_V(DEBUG, TAG, "%s NOT YET SUPPORTED ON OS X", __func__);
    /* FIXME */
    caglobals.ip.netlinkFd = OC_INVALID_SOCKET;
}

void CAInitializeFastShutdownMechanism()
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    caglobals.ip.selectTimeout = -1; // don't poll for shutdown
    int ret = -1;
    ret = pipe(caglobals.ip.shutdownFds);
    if (-1 != ret)
    {
        ret = fcntl(caglobals.ip.shutdownFds[0], F_GETFD);
        if (-1 != ret)
        {
            ret = fcntl(caglobals.ip.shutdownFds[0], F_SETFD, ret|FD_CLOEXEC);
        }
        if (-1 != ret)
        {
            ret = fcntl(caglobals.ip.shutdownFds[1], F_GETFD);
        }
        if (-1 != ret)
        {
            ret = fcntl(caglobals.ip.shutdownFds[1], F_SETFD, ret|FD_CLOEXEC);
        }
        if (-1 == ret)
        {
            close(caglobals.ip.shutdownFds[1]);
            close(caglobals.ip.shutdownFds[0]);
            caglobals.ip.shutdownFds[0] = -1;
            caglobals.ip.shutdownFds[1] = -1;
        }
    }
    CHECKFD(caglobals.ip.shutdownFds[0]);
    CHECKFD(caglobals.ip.shutdownFds[1]);
/* #endif */
    if (-1 == ret)
    {
        OIC_LOG_V(ERROR, TAG, "fast shutdown mechanism init failed: %s", CAIPS_GET_ERROR);
        caglobals.ip.selectTimeout = SELECT_TIMEOUT; //poll needed for shutdown
    }
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}
