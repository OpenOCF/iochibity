/******************************************************************
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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <sys/select.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <errno.h>

#ifdef __linux__		/* FIXM: use HAVE_NETLINK_H etc */
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

#include "../caipserver.h"
#include "../caipnwmonitor.h"
#include "octhread.h"
#include "caadapterutils.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include <coap/utlist.h>

#define TAG "OIC_CA_IP_IMPL"

/**
 * Remove network interface from list.
 */
static void CARemoveNetworkMonitorList(int ifiindex);

static void CARemoveNetworkMonitorList(int ifiindex)
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

u_arraylist_t *CAFindInterfaceChange()
{
    u_arraylist_t *iflist = NULL;
#ifdef __linux__		/* GAR:  Darwin support */
    char buf[4096] = { 0 };
    struct nlmsghdr *nh = NULL;
    struct sockaddr_nl sa = { .nl_family = 0 };
    struct iovec iov = { .iov_base = buf,
                         .iov_len = sizeof (buf) };
    struct msghdr msg = { .msg_name = (void *)&sa,
                          .msg_namelen = sizeof (sa),
                          .msg_iov = &iov,
                          .msg_iovlen = 1 };

    ssize_t len = recvmsg(caglobals.ip.netlinkFd, &msg, 0);

    for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len))
    {
        if (nh != NULL && (nh->nlmsg_type != RTM_DELADDR && nh->nlmsg_type != RTM_NEWADDR))
        {
            continue;
        }

        if (RTM_DELADDR == nh->nlmsg_type)
        {
            struct ifaddrmsg *ifa = (struct ifaddrmsg *)NLMSG_DATA (nh);
            if (ifa)
            {
                int ifiIndex = ifa->ifa_index;
                bool isFound = CACmpNetworkList(ifiIndex);
                if (isFound)
                {
                    CARemoveNetworkMonitorList(ifiIndex);
                    CAIPPassNetworkChangesToAdapter(CA_INTERFACE_DOWN);
                }
            }
            continue;
        }

        // Netlink message type is RTM_NEWADDR.
        struct ifaddrmsg *ifa = (struct ifaddrmsg *)NLMSG_DATA (nh);
        if (ifa)
        {
            int ifiIndex = ifa->ifa_index;
            iflist = CAIPGetInterfaceInformation(ifiIndex);
            if (!iflist)
            {
                OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
                return NULL;
            }
        }
    }
#endif
    return iflist;
}

u_arraylist_t *CAIPGetInterfaceInformation(int desiredIndex)
{
#if NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG_V(DEBUG, TAG, "IN %s: desiredIndex = %d", __func__, desiredIndex);
#endif
    if (desiredIndex < 0)
    {
        OIC_LOG_V(ERROR, TAG, "invalid index : %d", desiredIndex);
        return NULL;
    }

    u_arraylist_t *iflist = u_arraylist_create();
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create iflist: %s", strerror(errno));
        return NULL;
    }

    struct ifaddrs *ifp = NULL;
    if (-1 == getifaddrs(&ifp))
    {
        OIC_LOG_V(ERROR, TAG, "Failed to get ifaddrs: %s", strerror(errno));
        u_arraylist_destroy(iflist);
        return NULL;
    }

    struct ifaddrs *ifa = NULL;
#if NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG(DEBUG, TAG, "Iterating over interface addresses.");
#endif
    for (ifa = ifp; ifa; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
        {
            continue;
        }
        int family = ifa->ifa_addr->sa_family;
        if ((ifa->ifa_flags & IFF_LOOPBACK) || (AF_INET != family && AF_INET6 != family))
        {
            continue;
        }

        int ifindex = if_nametoindex(ifa->ifa_name);
        if (desiredIndex && (ifindex != desiredIndex))
        {
            continue;
        }

	/* GAR: we can only reach this point once if (name, index) pairs are unique?? */
        size_t length = u_arraylist_length(iflist);
        int already = false;
#if NETWORK_INTERFACE_CHANGED_LOGGING
        OIC_LOG_V(DEBUG, TAG, "Iterating over %" PRIuPTR " interfaces.", length);
#endif
        for (size_t i = 0; i < length; i++)
        {
#if NETWORK_INTERFACE_CHANGED_LOGGING
            OIC_LOG_V(DEBUG, TAG, "Checking interface %" PRIuPTR ".", i);
#endif
            CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);

            if (ifitem
                && (int)ifitem->index == ifindex
                && ifitem->family == (uint16_t)family)
            {
                already = true;
                break;
            }
        }
        if (already)
        {
            continue;
        }

        CAInterface_t *ifitem = (CAInterface_t *)OICCalloc(1, sizeof(CAInterface_t));
        if (!ifitem)
        {
            OIC_LOG(ERROR, TAG, "Malloc failed");
            goto exit;
        }

        OICStrcpy(ifitem->name, INTERFACE_NAME_MAX, ifa->ifa_name);
        ifitem->index = ifindex;
        ifitem->family = family;
        ifitem->flags = ifa->ifa_flags;

        if (ifitem->family == AF_INET6)
        {
            struct sockaddr_in6 *in6 = (struct sockaddr_in6*) ifa->ifa_addr;
            inet_ntop(ifitem->family, (void *)&(in6->sin6_addr), ifitem->addr,
                      sizeof(ifitem->addr));
        }
        else if (ifitem->family == AF_INET)
        {
            struct sockaddr_in *in = (struct sockaddr_in*) ifa->ifa_addr;
            inet_ntop(ifitem->family, (void *)&(in->sin_addr), ifitem->addr,
                      sizeof(ifitem->addr));
        }

        bool result = u_arraylist_add(iflist, ifitem);
        if (!result)
        {
            OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
            goto exit;
        }

        bool isFound = CACmpNetworkList(ifitem->index);
        if (!isFound)
        {
            CAInterface_t *newifitem = CANewInterfaceItem(ifitem->index, ifitem->name, ifitem->family,
                                                          ifitem->addr, ifitem->flags);
            CAResult_t ret = CAAddNetworkMonitorList(newifitem);
            if (CA_STATUS_OK != ret)
            {
                OICFree(newifitem);
                goto exit;
            }
            CAIPPassNetworkChangesToAdapter(CA_INTERFACE_UP);
            OIC_LOG_V(DEBUG, TAG, "Added interface: %s (%d)", ifitem->name, ifitem->family);
        }
    }
    freeifaddrs(ifp);
#if NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
#endif
    return iflist;

exit:
    freeifaddrs(ifp);
    u_arraylist_destroy(iflist);
#if NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
#endif
    return NULL;
}
