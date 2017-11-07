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

#include "caipserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>
/* #ifdef HAVE_LINUX_IF_H */
#include <linux/if.h>
/* #endif */
#include <coap/utlist.h>
/* #ifdef HAVE_LINUX_NETLINK_H */
#include <linux/netlink.h>
/* #endif
 * #ifdef HAVE_LINUX_RNETLINK_H */
#include <linux/rtnetlink.h>
/* #endif */
/* #ifdef HAVE_NET_IF_H */
#include <net/if.h>
/* #endif */

#include "caadapterutils.h"
#include "caipnwmonitor.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "org_iotivity_ca_CaIpInterface.h"
#include "caifaddrs.h"

#define TAG "OIC_CA_IP_MONITOR"
#define NETLINK_MESSAGE_LENGTH  (4096)
#define IFC_LABEL_LOOP          "lo"
#define IFC_ADDR_LOOP_IPV4      "127.0.0.1"
#define IFC_ADDR_LOOP_IPV6      "::1"
/**
 * Used to storing adapter changes callback interface.
 */
static struct CAIPCBData_t *g_adapterCallbackList = NULL;

/**
 * Create new interface item to add in activated interface list.
 * @param[in]  index    Network interface index number.
 * @param[in]  name     Network interface name.
 * @param[in]  family   Network interface family type.
 * @param[in]  addr     New interface address.
 * @param[in]  flags    The active flag word of a device.
 * @return  CAInterface_t objects.
 */
static CAInterface_t *CANewInterfaceItem(int index, const char *name, int family,
                                         const char *addr, int flags);

/**
 * Add created new interface item activated interface list.
 * @param[in]  iflist   Network interface array list.
 * @param[in]  index    Network interface index number.
 * @param[in]  name     Network interface name.
 * @param[in]  family   Network interface family type.
 * @param[in]  addr     New interface address.
 * @param[in]  flags    The active flag word of a device.
 * @return  ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
static CAResult_t CAAddInterfaceItem(u_arraylist_t *iflist, int index,
                                     const char *name, int family, const char *addr, int flags);

// GAR: not used #define MAX_INTERFACE_INFO_LENGTH 1024 // allows 32 interfaces from SIOCGIFCONF

CAResult_t CAIPStartNetworkMonitor(CAIPAdapterStateChangeCallback callback,
                                   CATransportAdapter_t adapter)
{
    CAResult_t res = CAIPJniInit();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "failed to initialize ip jni interface");
        return res;
    }

    return CAIPSetNetworkMonitorCallback(callback, adapter);
}

CAResult_t CAIPStopNetworkMonitor(CATransportAdapter_t adapter)
{
    CAIPUnSetNetworkMonitorCallback(adapter);

    // if there is no callback to pass the changed status, stop monitoring.
    if (!g_adapterCallbackList)
    {
        return CAIPDestroyJniInterface();
    }

    return CA_STATUS_OK;
}

int CAGetPollingInterval(int interval)
{
    return interval;
}

static void CAIPPassNetworkChangesToAdapter(CANetworkStatus_t status)
{
    CAIPCBData_t *cbitem = NULL;
    LL_FOREACH(g_adapterCallbackList, cbitem)
    {
        if (cbitem && cbitem->adapter)
        {
            cbitem->callback(cbitem->adapter, status);
        }
    }
}

CAResult_t CAIPSetNetworkMonitorCallback(CAIPAdapterStateChangeCallback callback,
                                         CATransportAdapter_t adapter)
{
    if (!callback)
    {
        OIC_LOG(ERROR, TAG, "callback is null");
        return CA_STATUS_INVALID_PARAM;
    }

    CAIPCBData_t *cbitem = NULL;
    LL_FOREACH(g_adapterCallbackList, cbitem)
    {
        if (cbitem && adapter == cbitem->adapter && callback == cbitem->callback)
        {
            OIC_LOG(DEBUG, TAG, "this callback is already added");
            return CA_STATUS_OK;
        }
    }

    cbitem = (CAIPCBData_t *)OICCalloc(1, sizeof(*cbitem));
    if (!cbitem)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed");
        return CA_STATUS_FAILED;
    }

    cbitem->adapter = adapter;
    cbitem->callback = callback;
    LL_APPEND(g_adapterCallbackList, cbitem);

    return CA_STATUS_OK;
}

CAResult_t CAIPUnSetNetworkMonitorCallback(CATransportAdapter_t adapter)
{
    CAIPCBData_t *cbitem = NULL;
    CAIPCBData_t *tmpCbitem = NULL;
    LL_FOREACH_SAFE(g_adapterCallbackList, cbitem, tmpCbitem)
    {
        if (cbitem && adapter == cbitem->adapter)
        {
            OIC_LOG(DEBUG, TAG, "remove specific callback");
            LL_DELETE(g_adapterCallbackList, cbitem);
            OICFree(cbitem);
            return CA_STATUS_OK;
        }
    }
    return CA_STATUS_OK;
}

u_arraylist_t *CAFindInterfaceChange()
{
    char buf[NETLINK_MESSAGE_LENGTH] = { 0 };
    struct sockaddr_nl sa = { 0 };
    struct iovec iov = { .iov_base = buf,
                         .iov_len = sizeof (buf) };
    struct msghdr msg = { .msg_name = (void *)&sa,
                          .msg_namelen = sizeof (sa),
                          .msg_iov = &iov,
                          .msg_iovlen = 1 };

    // We do nothing with netlink event here.
    // Android BroadcastReceiver will work instead.
    ssize_t len = recvmsg(caglobals.ip.netlinkFd, &msg, 0);
    OC_UNUSED(len);

    return NULL;
}

/**
 * Used to send netlink query to kernel and recv response from kernel.
 *
 * @param[in]   idx       desired network interface index, 0 means all interfaces.
 * @param[out]  iflist    linked list.
 *
 */
static bool CAParsingNetorkInfo(int idx, u_arraylist_t *iflist)
{
    if ((idx < 0) || (iflist == NULL))
    {
        return false;
    }

    struct ifaddrs *ifp = NULL;
    CAResult_t ret = CAGetIfaddrsUsingNetlink(&ifp);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to get ifaddrs err code is: %d", ret);
        return false;
    }

    struct ifaddrs *ifa = NULL;
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
        if (idx && (ifindex != idx))
        {
            continue;
        }

        char ipaddr[MAX_ADDR_STR_SIZE_CA] = {0};
        if (family == AF_INET6)
        {
            struct sockaddr_in6 *in6 = (struct sockaddr_in6*) ifa->ifa_addr;
            inet_ntop(family, (void *)&(in6->sin6_addr), ipaddr, sizeof(ipaddr));
        }
        else if (family == AF_INET)
        {
            struct sockaddr_in *in = (struct sockaddr_in*) ifa->ifa_addr;
            inet_ntop(family, (void *)&(in->sin_addr), ipaddr, sizeof(ipaddr));
        }

        if ((strcmp(ipaddr, IFC_ADDR_LOOP_IPV4) == 0) ||
            (strcmp(ipaddr, IFC_ADDR_LOOP_IPV6) == 0) ||
            (strcmp(ifa->ifa_name, IFC_LABEL_LOOP) == 0))
        {
            OIC_LOG(DEBUG, TAG, "LOOPBACK continue!!!");
            continue;
        }

        CAResult_t result = CAAddInterfaceItem(iflist, ifindex,
                                               ifa->ifa_name, family,
                                               ipaddr, ifa->ifa_flags);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CAAddInterfaceItem fail");
            goto exit;
        }
    }
    CAFreeIfAddrs(ifp);
    return true;

exit:
    CAFreeIfAddrs(ifp);
    return false;
}

u_arraylist_t *CAIPGetInterfaceInformation(int desiredIndex)
{
    u_arraylist_t *iflist = u_arraylist_create();
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create iflist: %s", strerror(errno));
        return NULL;
    }

    if (!CAParsingNetorkInfo(desiredIndex, iflist))
    {
        goto exit;
    }

    return iflist;

exit:
    u_arraylist_destroy(iflist);
    return NULL;
}

static CAResult_t CAAddInterfaceItem(u_arraylist_t *iflist, int index,
                                     const char *name, int family, const char *addr, int flags)
{
    CAInterface_t *ifitem = CANewInterfaceItem(index, name, family, addr, flags);
    if (!ifitem)
    {
        return CA_STATUS_FAILED;
    }
    bool result = u_arraylist_add(iflist, ifitem);
    if (!result)
    {
        OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
        OICFree(ifitem);
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

static CAInterface_t *CANewInterfaceItem(int index, const char *name, int family,
                                         const char *addr, int flags)
{
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

    return ifitem;
}

CAResult_t CAGetLinkLocalZoneIdInternal(uint32_t ifindex, char **zoneId)
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
