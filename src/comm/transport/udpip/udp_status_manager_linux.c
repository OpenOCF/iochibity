/** udp_status_manager_linux.c
 *
 */

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

#include "udp_status_manager_linux.h"

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>

#if EXPORT_INTERFACE
/* POSIX.1-2001, POSIX.1-2008 */
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
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
/* void CAFindReadyMessage() */
/* { */
/*     /\* OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); *\/ */
/*     fd_set readFds; */
/*     struct timeval timeout; */

/*     timeout.tv_sec = udp_selectTimeout; */
/*     timeout.tv_usec = 0; */
/*     struct timeval *tv = udp_selectTimeout == -1 ? NULL : &timeout; */

/*     FD_ZERO(&readFds); */
/*     SET(udp_u6,  &readFds) */
/*     SET(udp_u6s, &readFds) */
/*     SET(udp_u4,  &readFds) */
/*     SET(udp_u4s, &readFds) */
/*     SET(udp_m6,  &readFds) */
/*     SET(udp_m6s, &readFds) */
/*     SET(udp_m4,  &readFds) */
/*     SET(udp_m4s, &readFds) */

/*     if (udp_shutdownFds[0] != -1) */
/*     { */
/*         FD_SET(udp_shutdownFds[0], &readFds); */
/*     } */
/*     if (udp_netlinkFd != OC_INVALID_SOCKET) */
/*     { */
/*         FD_SET(udp_netlinkFd, &readFds); */
/*     } */

/*     int ret = select(udp_maxfd + 1, &readFds, NULL, NULL, tv); */

/*     if (udp_is_terminating) */
/*     { */
/*         OIC_LOG_V(DEBUG, TAG, "Packet receiver Stop request received."); */
/*         return; */
/*     } */

/*     if (0 == ret) */
/*     { */
/*         return; */
/*     } */
/*     else if (0 < ret) */
/*     { */
/*         CASelectReturned(&readFds, ret); */
/*     } */
/*     else // if (0 > ret) */
/*     { */
/*         OIC_LOG_V(FATAL, TAG, "select error %s", CAIPS_GET_ERROR); */
/*         return; */
/*     } */
/* } */

/* /\* process FDs that are ready for reading *\/ */
/* LOCAL void CASelectReturned(fd_set *readFds, int ret) */
/* { */
/*     /\* OIC_LOG_V(DEBUG, TAG, "CASelectReturned"); *\/ */
/*     (void)ret;			/\* ret = fd count *\/ */
/*     CASocketFd_t fd = OC_INVALID_SOCKET; */
/*     CATransportFlags_t flags = CA_DEFAULT_FLAGS; */

/*     while (!udp_is_terminating) */
/*     { */
/*         ISSET(udp_u6,  readFds, CA_IPV6) */
/*         else ISSET(udp_u6s, readFds, CA_IPV6 | CA_SECURE) */
/*         else ISSET(udp_u4,  readFds, CA_IPV4) */
/*         else ISSET(udp_u4s, readFds, CA_IPV4 | CA_SECURE) */
/*         else ISSET(udp_m6,  readFds, CA_MULTICAST | CA_IPV6) */
/*         else ISSET(udp_m6s, readFds, CA_MULTICAST | CA_IPV6 | CA_SECURE) */
/*         else ISSET(udp_m4,  readFds, CA_MULTICAST | CA_IPV4) */
/*         else ISSET(udp_m4s, readFds, CA_MULTICAST | CA_IPV4 | CA_SECURE) */
/*         else if ((udp_netlinkFd != OC_INVALID_SOCKET) && FD_ISSET(udp_netlinkFd, readFds)) */
/*         { */
/* #ifdef NETWORK_INTERFACE_CHANGED_LOGGING */
/*             OIC_LOG_V(DEBUG, TAG, "Rtnetlink event detected"); */
/* #endif */
/*             u_arraylist_t *iflist = CAFindInterfaceChange(); */
/*             if (iflist) */
/*             { */
/*                 size_t listLength = u_arraylist_length(iflist); */
/*                 for (size_t i = 0; i < listLength; i++) */
/*                 { */
/*                     CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i); */
/*                     if (ifitem) */
/*                     { */
/*                         udp_add_if_to_multicast_groups(ifitem); // @was CAProcessNewInterface */
/*                     } */
/*                 } */
/*                 u_arraylist_destroy(iflist); */
/*             } */
/*             break; */
/*         } */
/*         else if (FD_ISSET(udp_shutdownFds[0], readFds)) */
/*         { */
/*             char buf[10] = {0}; */
/*             ssize_t len = read(udp_shutdownFds[0], buf, sizeof (buf)); */
/*             if (-1 == len) */
/*             { */
/*                 continue; */
/*             } */
/*             break; */
/*         } */
/*         else */
/*         { */
/*             break; */
/*         } */
/* 	/\* at this point, any fd ready for reading, and flags, have been set by the ISSET macros above *\/ */
/*         (void)CAReceiveMessage(fd, flags); */
/*         FD_CLR(fd, readFds); */
/*     } */
/* } */

void CADeInitializeMonitorGlobals()
{
    if (udp_netlinkFd != OC_INVALID_SOCKET)
    {
        close(udp_netlinkFd);
        udp_netlinkFd = OC_INVALID_SOCKET;
    }

    if (udp_shutdownFds[0] != -1)
    {
        close(udp_shutdownFds[0]);
        udp_shutdownFds[0] = -1;
    }
}

// GAR: rename: CARegisterForIfaceChanges
void CARegisterForAddressChanges()
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    udp_netlinkFd = OC_INVALID_SOCKET;
    // create NETLINK fd for interface change notifications
    struct sockaddr_nl sa = { AF_NETLINK, 0, 0,
                              RTMGRP_LINK /* nw interface create/delete/up/down events */
			      | RTMGRP_IPV4_IFADDR /* ipv4 address add/delete */
			      | RTMGRP_IPV6_IFADDR /* ipv6 address add/delete */
    };

    udp_netlinkFd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_ROUTE);
    if (udp_netlinkFd == OC_INVALID_SOCKET)
    {
        OIC_LOG_V(ERROR, TAG, "netlink socket failed: %s", strerror(errno));
    }
    else
    {
        int r = bind(udp_netlinkFd, (struct sockaddr *)&sa, sizeof (sa));
        if (r)
        {
            OIC_LOG_V(ERROR, TAG, "netlink bind failed: %s", strerror(errno));
            close(udp_netlinkFd);
            udp_netlinkFd = OC_INVALID_SOCKET;
        }
        else
        {
            UDP_CHECKFD(udp_netlinkFd);
        }
    }
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}

/* void CAInitializeFastShutdownMechanism() */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "IN %s", __func__); */
/*     udp_selectTimeout = -1; // don't poll for shutdown */
/*     int ret = -1; */
/*     ret = pipe2(udp_shutdownFds, O_CLOEXEC); */
/*     UDP_CHECKFD(udp_shutdownFds[0]); */
/*     UDP_CHECKFD(udp_shutdownFds[1]); */
/*     if (-1 == ret) */
/*     { */
/*         OIC_LOG_V(ERROR, TAG, "fast shutdown mechanism init failed: %s", CAIPS_GET_ERROR); */
/*         udp_selectTimeout = SELECT_TIMEOUT; //poll needed for shutdown */
/*     } */
/*     OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__); */
/* } */

void CAInitializeFastShutdownMechanism(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    udp_selectTimeout = -1; // don't poll for shutdown
    int ret = -1;
#if defined(WSA_WAIT_EVENT_0)
    udp_shutdownEvent = WSACreateEvent();
    if (WSA_INVALID_EVENT != udp_shutdownEvent)
    {
        ret = 0;
    }
#elif defined(HAVE_PIPE2)
    ret = pipe2(udp_shutdownFds, O_CLOEXEC);
    UDP_CHECKFD(udp_shutdownFds[0]);
    UDP_CHECKFD(udp_shutdownFds[1]);
#else
    ret = pipe(udp_shutdownFds);
    if (-1 != ret)
    {
        ret = fcntl(udp_shutdownFds[0], F_GETFD);
        if (-1 != ret)
        {
            ret = fcntl(udp_shutdownFds[0], F_SETFD, ret|FD_CLOEXEC);
        }
        if (-1 != ret)
        {
            ret = fcntl(udp_shutdownFds[1], F_GETFD);
        }
        if (-1 != ret)
        {
            ret = fcntl(udp_shutdownFds[1], F_SETFD, ret|FD_CLOEXEC);
        }
        if (-1 == ret)
        {
            close(udp_shutdownFds[1]);
            close(udp_shutdownFds[0]);
            udp_shutdownFds[0] = -1;
            udp_shutdownFds[1] = -1;
        }
    }
    UDP_CHECKFD(udp_shutdownFds[0]);
    UDP_CHECKFD(udp_shutdownFds[1]);
#endif
    if (-1 == ret)
    {
        OIC_LOG_V(ERROR, TAG, "fast shutdown mechanism init failed: %s", CAIPS_GET_ERROR);
        udp_selectTimeout = SELECT_TIMEOUT; //poll needed for shutdown
    }
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}

static void CARemoveFromInterfaceList(int ifiindex) // @was  CARemoveNetworkMonitorList
{
    VERIFY_NON_NULL_VOID(g_netInterfaceList, TAG, "g_netInterfaceList is NULL");

    oc_mutex_lock(g_networkMonitorContextMutex);

    size_t list_length = u_arraylist_length(g_netInterfaceList);
    for (size_t list_index = 0; list_index < list_length; list_index++)
    {
        CAInterface_t *removedifitem = (CAInterface_t *) u_arraylist_get(
                g_netInterfaceList, list_index);
        if (removedifitem && ((int)removedifitem->index) == ifiindex)
        {
            if (u_arraylist_remove(g_netInterfaceList, list_index))
            {
                OICFree(removedifitem);
                oc_mutex_unlock(g_networkMonitorContextMutex);
                return;
            }
            continue;
        }
    }
    oc_mutex_unlock(g_networkMonitorContextMutex);
    return;
}

// FIXME: not called currently in udp_*
// @was: called by caipserver_linux::CASelectReturned when netlinkFd ready
// TODO: implement status monitoring for Linux
u_arraylist_t *udp_if_change_handler_linux() // @was CAFindInterfaceChange
{
    u_arraylist_t *iflist = NULL;
#if defined(__linux__) || defined (__ANDROID__) /* GAR:  Darwin support */
    char buf[4096] = { 0 };
    struct nlmsghdr *nh = NULL;
    struct sockaddr_nl sa = { .nl_family = 0 };
    struct iovec iov = { .iov_base = buf,
                         .iov_len = sizeof (buf) };
    struct msghdr msg = { .msg_name = (void *)&sa,
                          .msg_namelen = sizeof (sa),
                          .msg_iov = &iov,
                          .msg_iovlen = 1 };

    ssize_t len = recvmsg(udp_netlinkFd, &msg, 0);
    /* OIC_LOG_V(DEBUG, TAG, "Rtnetlink recvmsg len: %d", len); */

    for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len))
    {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	if (nh != NULL) {
	    if (nh->nlmsg_type == RTM_DELADDR) {
		OIC_LOG_V(DEBUG, TAG, "Rtnetlink event type RTM_DELADDR");
	    } else if (nh->nlmsg_type == RTM_NEWADDR) {
		OIC_LOG_V(DEBUG, TAG, "Rtnetlink event type RTM_NEWADDR");
	    } else if (nh->nlmsg_type == RTM_NEWLINK) {
		OIC_LOG_V(DEBUG, TAG, "Rtnetlink event type RTM_NEWLINK");
	    } else if (nh->nlmsg_type == RTM_DELLINK) {
		OIC_LOG_V(DEBUG, TAG, "Rtnetlink event type RTM_DELLINK");
	    } else {
		OIC_LOG_V(DEBUG, TAG, "Rtnetlink event type %d", nh->nlmsg_type);
	    }
	} else {
	    OIC_LOG_V(DEBUG, TAG, "Rtnetlink recvmsg fail?");
	}
#endif
        if (nh != NULL && (nh->nlmsg_type != RTM_DELADDR && nh->nlmsg_type != RTM_NEWADDR))
        {
	    /* GAR: what about RTM_NEWLINK, RTM_DELLINK? */
            continue;
        }

        if (RTM_DELADDR == nh->nlmsg_type)
        {
            struct ifaddrmsg *ifa = (struct ifaddrmsg *)NLMSG_DATA (nh);
            if (ifa)
            {
                int ifiIndex = ifa->ifa_index;
                bool isFound = InterfaceListContains(ifiIndex);
                if (isFound) {
		    CARemoveFromInterfaceList(ifiIndex);
                    //udp_if_change_handler(CA_INTERFACE_DOWN); // @was CAIPPassNetworkChangesToTransports
#ifdef IP_ADAPTER
		    udp_status_change_handler(CA_ADAPTER_IP, CA_INTERFACE_DOWN); // @was CAIPAdapterHandler
#endif
#ifdef TCP_ADAPTER
		    tcp_status_change_handler(CA_ADAPTER_IP, CA_INTERFACE_DOWN); //@was CATCPAdapterHandler
#endif
		    // @was CAIPPassNetworkChangesToAdapter(CA_INTERFACE_DOWN);
                }
            }
            continue;
        }

        // Netlink message type is RTM_NEWADDR.
        struct ifaddrmsg *ifa = (struct ifaddrmsg *)NLMSG_DATA (nh);
        if (ifa)
        {
            int ifiIndex = ifa->ifa_index;
	    /* FIXME: BUG. what if > 1 new addrs? only last will be in iflist */
	    // GAR: udp_get_ifs_for_rtm_newaddr will call CAIPPassNetworkChangesToAdapter
            iflist = udp_get_ifs_for_rtm_newaddr(ifiIndex);
            if (!iflist)
            {
                OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
                return NULL;
            }
	    /* GAR: CAProcessNewInterfaceItem? (android) */
        }
    }
#endif
    return iflist;
}
