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
#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542 // for PKTINFO
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for in6_pktinfo
#endif

#include "udp_status_manager_posix.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <sys/select.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <errno.h>

/* #ifdef HAVE_IFADDRS_H */
#include <ifaddrs.h>
/* #endif */

/* FIXME: use HAVE_NETLINK_H etc */
#if defined(__linux__)
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

/* FIXME: refactor to caipnwmonitor_linux? use HAVE_NETLINK_H etc */
#if defined(__linux__)
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

#include "utlist.h"

#if EXPORT_INTERFACE
#include <stdint.h>
#endif


#define TAG "NWMONPOSIX"

/* #if EXPORT_INTERFACE
 * #include <stdint.h>
 * #endif	/\* INTERFACE *\/
 *
 * #if EXPORT_INTERFACE
 * #define INTERFACE_NAME_MAX 16
 * #include <stddef.h>
 * #endif	/\*  *\/
 *
 * /\**
 *  * Callback to be notified when IP adapter network state changes.
 *  *
 *  * @param[in]  adapter      Transport adapter.
 *  * @param[in]  status       Connection status either ::CA_INTERFACE_UP or ::CA_INTERFACE_DOWN.
 *  * @see CAIPSetConnectionStateChangeCallback() for registration.
 *  *\/
 * typedef void (*CAIPAdapterStateChangeCallback)(CATransportAdapter_t adapter,
 *                                                CANetworkStatus_t status); */

/* in nwmonitor0: CAResult_t CAIPInitializeNetworkAddressList() */

// @rewrite CAIPDestroyNetworkAddressList @was CAIPDestroyNetworkMonitorList
void CAIPDestroyNetworkAddressList()
{
    if (g_network_interfaces)
    {
        u_arraylist_destroy(g_network_interfaces);
        g_network_interfaces = NULL;
    }

    if (g_networkMonitorContextMutex)
    {
        oc_mutex_free(g_networkMonitorContextMutex);
        g_networkMonitorContextMutex = NULL;
    }
}

// g_network_interfaces is a list of nw INTERFACES! one entry per interface,
// regardless of address and address family
// @rewrite: CAAddToAddressList @was CAAddNetworkAddressList
static CAResult_t CAAddToNetworkAddressList(CAInterface_t *ifitem)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    VERIFY_NON_NULL_MSG(g_network_interfaces, TAG, "g_network_interfaces is NULL");
    VERIFY_NON_NULL_MSG(ifitem, TAG, "ifitem is NULL");

    oc_mutex_lock(g_networkMonitorContextMutex);
    bool result = u_arraylist_add(g_network_interfaces, (void *) ifitem);
    if (!result)
    {
        OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
        oc_mutex_unlock(g_networkMonitorContextMutex);
        return CA_STATUS_FAILED;
    }
    oc_mutex_unlock(g_networkMonitorContextMutex);
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
    return CA_STATUS_OK;
}

// @rewrite: CAIPStartNetworkMonitor is defunct
/* CAResult_t CAIPStartNetworkMonitor(void */
/* 				   (*ip_status_change_handler)(CATransportAdapter_t adapter, */
/* 							       CANetworkStatus_t status), */
/* 				   // CAIPAdapterStateChangeCallback callback, */
/*                                    CATransportAdapter_t adapter) */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); */
/*     CAResult_t res = CAIPInitializeNetworkAddressList(); */
/*     // @rewrite: CAIPSetConnectionStateChangeCallback not needed */
/*     /\* if (CA_STATUS_OK == res) *\/ */
/*     /\* { *\/ */
/*     /\*     return CAIPSetNetworkMonitorCallback(ip_status_change_handler, adapter); *\/ */
/*     /\* } *\/ */
/*     return res; */
/* } */

CAResult_t CAIPStopNetworkMonitor(CATransportAdapter_t adapter)
{
    CAIPDestroyNetworkAddressList();
    return CAIPUnSetNetworkMonitorCallback(adapter);
}

/* CAResult_t CAIPUnSetNetworkMonitorCallback(CATransportAdapter_t adapter)
 * {
 *     CAIPCBData_t *cbitem = NULL;
 *     CAIPCBData_t *tmpCbitem = NULL;
 *     LL_FOREACH_SAFE(g_adapterCallbackList, cbitem, tmpCbitem)
 *     {
 *         if (cbitem && adapter == cbitem->adapter)
 *         {
 *             OIC_LOG(DEBUG, TAG, "remove specific callback");
 *             LL_DELETE(g_adapterCallbackList, cbitem);
 *             OICFree(cbitem);
 *             return CA_STATUS_OK;
 *         }
 *     }
 *     return CA_STATUS_OK;
 * } */

static CAInterface_t *CANewInterfaceItem(int index, const char *name, int family,
                                         const char *addr, int flags)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OIC_LOG_V(DEBUG, TAG, "\tindex = %d, name = \"%s\", family = %d, addr = \"%s\", flags = %d",
	      index, name, family, addr, flags);

    CAInterface_t *ifitem = (CAInterface_t *)OICCalloc(1, sizeof (CAInterface_t));
    if (!ifitem)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed");
        return NULL;
    }

    OICStrcpy(ifitem->name, sizeof (ifitem->name), name);
    ifitem->index = index;
    ifitem->family = family;
    OICStrcpy(ifitem->addr, sizeof (ifitem->addr), addr);
    ifitem->flags = flags;

    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
    return ifitem;
}

/* DELEGATE: u_arraylist_t *CAFindInterfaceChange() */

bool CACmpNetworkList(uint32_t ifiindex)
{
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG_V(DEBUG, TAG, "IN %s: IF index: %d", __func__, ifiindex);
#endif
    if (!g_network_interfaces)
    {
        OIC_LOG(ERROR, TAG, "g_network_interfaces is NULL");
        return false;
    }

    oc_mutex_lock(g_networkMonitorContextMutex);

    size_t list_length = u_arraylist_length(g_network_interfaces);
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG_V(DEBUG, TAG, "g_network_interfaces list length: %d", list_length);
#endif
    for (size_t list_index = 0; list_index < list_length; list_index++)
    {
        CAInterface_t *currItem = (CAInterface_t *) u_arraylist_get(g_network_interfaces,
                                                                    list_index);
        if (currItem->index == ifiindex)
        {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "Found IF index %d in g_network_interfaces", ifiindex);
#endif
            oc_mutex_unlock(g_networkMonitorContextMutex);
            return true;
        }
    }
    oc_mutex_unlock(g_networkMonitorContextMutex);
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
    return false;
}

/* GAR FIXME: CAIPGetAllInterfaceInformation(), not CAIPGetInterfaceInformation(0) */
// GAR: called on RTM_NEWADDR
// @rewrite: this has side effects, so refactor it
// @rewrite: it adds addresses to g_network_interfaces and calls status chg handlers
// @rewrite: call it udp_get_ifaddrs?
u_arraylist_t			/**< @result list of CAInterface_t */
*CAIPGetInterfaceInformation(int desiredIndex /**< index of new if addr */)
{
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG_V(DEBUG, TAG, "IN %s: desiredIndex = %d", __func__, desiredIndex);
#endif
    if (desiredIndex < 0)
    {
        OIC_LOG_V(ERROR, TAG, "invalid index : %d", desiredIndex);
        return NULL;
    }

    // Caller must free
    u_arraylist_t *iflist = u_arraylist_create();
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create iflist: %s", strerror(errno));
        return NULL;
    }

    struct ifaddrs *ifp = NULL;
    errno = 0;
    int r = 0;

    r = getifaddrs(&ifp);
    if (-1 == r)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to get ifaddrs: %s", strerror(errno));
        u_arraylist_destroy(iflist);
        return NULL;
    }

    struct ifaddrs *ifa = NULL;
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG(DEBUG, TAG, "Iterating over interface addresses.");
#endif
    int i = 1;  // debugging
    char addr_str[256]; // debugging
    int ifindex = 0;
    for (ifa = ifp; ifa; ifa = ifa->ifa_next)
    {
	if (ifa->ifa_next == NULL) break;

	/* OIC_LOG_V(DEBUG, TAG, "item %d", i); */
	ifindex = 0;

	if (ifa->ifa_name)
	    ifindex = if_nametoindex(ifa->ifa_name);

#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	if (ifa->ifa_addr) {
	    inet_ntop(ifa->ifa_addr->sa_family,
		      &(((struct sockaddr_in*)ifa->ifa_addr)->sin_addr),
		      addr_str, sizeof(addr_str));
	    OIC_LOG_V(DEBUG, TAG, "item %d: %s (%d): %s", i++,
		      ifa->ifa_name, ifindex, addr_str);
	} else {
	    OIC_LOG_V(DEBUG, TAG, "item %d: %s (%d)", i++,
		      ifa->ifa_name, ifindex);
	}
#endif

        if (!ifa->ifa_addr)
        {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "\tSkipping IF %d - no address", ifindex);
#endif
            continue;
        }
        int family = ifa->ifa_addr->sa_family;
        if (ifa->ifa_flags & IFF_LOOPBACK) {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "\tSkipping loopback IF %d, family %d", ifindex, family);
#endif
	    continue;
	}
#ifdef __APPLE__
	if (AF_LINK == family) {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "\tSkipping AF_LINK IF %d", ifindex);
#endif
	    continue;
	}
#else
	if (AF_NETLINK == family) { /* linux */
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "\tSkipping AF_NETLINK IF %d", ifindex);
#endif
	    continue;
	}
#endif
	if (AF_INET != family && AF_INET6 != family) {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "\tSkipping non-IPv4/6 IF %d, family %d", ifindex, family);
#endif
	    continue;
	}

	/* GAR: hidden semantics: if desiredIndex == 0, then every if will be added to list */
        if ( (desiredIndex > 0) && (ifindex != desiredIndex))
        {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "Skipping IF %d (searching for %d)", ifindex, desiredIndex);
#endif
            continue;
        }

#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	OIC_LOG_V(DEBUG, TAG, "Found good IF/family: %d/%d (%s)", ifindex, family,
		  (family == AF_INET? "AF_INET" : family == AF_INET6? "AF_INET6" : "OTHER"));
#endif

	// we only need one item per (IF, family), in cases where
	// there may be multiple addresses per IF.
        size_t length = u_arraylist_length(iflist);
        int already = false;
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
        OIC_LOG_V(DEBUG, TAG, "Iterating over %d interface items seen so far.", length);
#endif
        for (size_t i = 0; i < length; i++)
        {
            CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
            if (ifitem
                && (int)ifitem->index == ifindex
                && ifitem->family == (uint16_t)family)
            {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "\tifitem %d: %d/%d: match", i, (int)ifitem->index, ifitem->family);
#endif
                already = true;
                break;
            }
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "\tifitem %d: %d/%d: no match", i, (int)ifitem->index, ifitem->family);
#endif
        }

        if (already) {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "We've already seen IF/family %d/%d; skipping", ifindex, family);
#endif
	    continue;
	}

#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	OIC_LOG_V(DEBUG, TAG, "Processing new item.");
#endif

	/* FIXME: CAInterface_t => CAIfAddress_t */
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
	    /* FIXME: free ifitem? */
            goto exit;
        }

        bool isFound = CACmpNetworkList(ifitem->index);

        if (!isFound)
        {
	    // GAR: why bother adding to g_network_interfaces? that list is never used for anything!
	    /* GAR: use DupIfItem(ifitem) instead of NewIfItem, to clarify sense */
            CAInterface_t *newifitem = CANewInterfaceItem(ifitem->index, ifitem->name, ifitem->family,
                                                          ifitem->addr, ifitem->flags);
            /* CAResult_t ret = CAAddNetworkMonitorList(newifitem); */
            CAResult_t ret = CAAddToNetworkAddressList(newifitem);
            if (CA_STATUS_OK != ret)
            {
                OICFree(newifitem);
                goto exit;
            }
            /* CAIPPassNetworkChangesToAdapter(CA_INTERFACE_UP); */
	    CAIPPassNetworkChangesToTransports(CA_INTERFACE_UP);
            OIC_LOG_V(DEBUG, TAG, "Processed IF/family %s/%d (%s)",
		      ifitem->name, ifitem->family,
		      (family == AF_INET? "AF_INET" : family == AF_INET6? "AF_INET6" : "OTHER"));
        }
    }
    freeifaddrs(ifp);
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
#endif
    return iflist;

exit:
    freeifaddrs(ifp);
    u_arraylist_destroy(iflist);
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
#endif
    return NULL;
}
