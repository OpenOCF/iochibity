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
#include "caipnwmonitor_posix.h"

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


#define TAG "IPNWMP"

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

/* /\*
 *  * Structure for IP address information, to be used to construct a CAEndpoint_t.  The
 *  * structure name is a misnomer, as there is one entry per address not one per interface.
 *  * An interface with 4 addresses should result in 4 instances of CAInterface_t.
 *  *\/
 * typedef struct
 * {
 *     char name[INTERFACE_NAME_MAX];
 *     uint32_t index;
 *     uint32_t flags;
 *     uint16_t family;
 *     char addr[MAX_ADDR_STR_SIZE_CA];
 * } CAInterface_t;		/\* GAR: CAIfAddress_t *\/
 *
 * typedef struct CAIPCBData_t
 * {
 *     struct CAIPCBData_t *next;
 *     CATransportAdapter_t adapter;
 *     CAIPAdapterStateChangeCallback callback;
 * } CAIPCBData_t;
 *
 * /\**
 *  * Mutex for synchronizing access to cached interface and IP address information.
 *  *\/
 * static oc_mutex g_networkMonitorContextMutex = NULL;
 *
 * /\**
 *  * Used to storing network interface.
 *  *\/
 * static u_arraylist_t *g_netInterfaceList = NULL;
 *
 * /\**
 *  * Used to storing adapter changes callback interface.
 *  *\/
 * static struct CAIPCBData_t *g_adapterCallbackList = NULL;
 *
 * /\**
 *  * Destroy the network interface monitoring list.
 *  *\/
 * static void CAIPDestroyNetworkMonitorList();
 *
 * /\**
 *  * Pass the changed network status through the stored callback.
 *  *\/
 * static void CAIPPassNetworkChangesToAdapter(CANetworkStatus_t status); */

void CAIPDestroyNetworkMonitorList()
{
    if (g_netInterfaceList)
    {
        u_arraylist_destroy(g_netInterfaceList);
        g_netInterfaceList = NULL;
    }

    if (g_networkMonitorContextMutex)
    {
        oc_mutex_free(g_networkMonitorContextMutex);
        g_networkMonitorContextMutex = NULL;
    }
}

static CAResult_t CAAddNetworkMonitorList(CAInterface_t *ifitem)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    VERIFY_NON_NULL_MSG(g_netInterfaceList, TAG, "g_netInterfaceList is NULL");
    VERIFY_NON_NULL_MSG(ifitem, TAG, "ifitem is NULL");

    oc_mutex_lock(g_networkMonitorContextMutex);
    bool result = u_arraylist_add(g_netInterfaceList, (void *) ifitem);
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

CAResult_t CAIPStartNetworkMonitor(CAIPAdapterStateChangeCallback callback,
                                   CATransportAdapter_t adapter)
{
    CAResult_t res = CAIPInitializeNetworkMonitorList();
    if (CA_STATUS_OK == res)
    {
        return CAIPSetNetworkMonitorCallback(callback, adapter);
    }
    return res;
}

CAResult_t CAIPStopNetworkMonitor(CATransportAdapter_t adapter)
{
    CAIPDestroyNetworkMonitorList();
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
    OIC_LOG_V(DEBUG, TAG, "IN %s:"
              "index = %d, name = \"%s\", family = %d, addr = \"%s\", flags = %d",
              __func__, index, name, family, addr, flags);
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
    OIC_LOG_V(DEBUG, TAG, "IN %s: ifiindex = %ul", __func__, ifiindex);
#endif
    if (!g_netInterfaceList)
    {
        OIC_LOG(ERROR, TAG, "g_netInterfaceList is NULL");
        return false;
    }

    oc_mutex_lock(g_networkMonitorContextMutex);

    size_t list_length = u_arraylist_length(g_netInterfaceList);
    for (size_t list_index = 0; list_index < list_length; list_index++)
    {
        CAInterface_t *currItem = (CAInterface_t *) u_arraylist_get(g_netInterfaceList,
                                                                    list_index);
        if (currItem->index == ifiindex)
        {
            oc_mutex_unlock(g_networkMonitorContextMutex);
            return true;
        }
    }
    oc_mutex_unlock(g_networkMonitorContextMutex);
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
    return false;
}

/* GAR FIXME: CAIPGetAllInterfaceInformation(), not CAIPGetInterfaceInformation(0) */
u_arraylist_t *CAIPGetInterfaceInformation(int desiredIndex)
{
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
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

	/* GAR: hidden semantics: if desiredIndex == 0, then every if will be added to list */
        int ifindex = if_nametoindex(ifa->ifa_name);
        if (desiredIndex && (ifindex != desiredIndex))
        {
            continue;
        }

        size_t length = u_arraylist_length(iflist);
        int already = false;
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
        OIC_LOG_V(DEBUG, TAG, "Iterating over %d interfaces.", length); /*  " PRIuPTR " */
#endif
        for (size_t i = 0; i < length; i++)
        {
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
            OIC_LOG_V(DEBUG, TAG, "Checking interface %d.", i); /* " PRIuPTR " */
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
	    /* GAR: use DupIfItem(ifitem) instead of NewIfItem, to clarify sense */
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
