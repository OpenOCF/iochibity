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
#include "caipnwmonitor0.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_IFADDRS_H
#include <ifaddrs.h>
/* #else */
/* #include "ifaddrs.h" 		/\* netlink impl in portability layer *\/ */
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#include <errno.h>

/* #if defined(__linux__) || defined(__ANDROID__) /\* FIXM: use HAVE_NETLINK_H etc *\/
 * #include <linux/if.h>
 * #include <linux/netlink.h>
 * #include <linux/rtnetlink.h>
 * #endif */

#include "utlist.h"

#define TAG "IPNWM"

#if EXPORT_INTERFACE
#include <stdint.h>
#endif	/* INTERFACE */

#if EXPORT_INTERFACE
#define INTERFACE_NAME_MAX 16
#include <stddef.h>
#endif
/**
 * Callback to be notified when IP adapter network state changes.
 *
 * @param[in]  adapter      Transport adapter.
 * @param[in]  status       Connection status either ::CA_INTERFACE_UP or ::CA_INTERFACE_DOWN.
 * @see CAIPSetConnectionStateChangeCallback() for registration.
 */

/*
 * Structure for IP address information, to be used to construct a CAEndpoint_t.  The
 * structure name is a misnomer, as there is one entry per address not one per interface.
 * An interface with 4 addresses should result in 4 instances of CAInterface_t.
 */
#if EXPORT_INTERFACE
typedef struct
{
    char name[INTERFACE_NAME_MAX];
    uint32_t index;
    uint32_t flags;
    uint16_t family;
    char addr[MAX_ADDR_STR_SIZE_CA];
} CAInterface_t;		// @rewrite to CAIfAddress_t */
#endif

#if EXPORT_INTERFACE
typedef struct CAIPCBData_t
{
    struct CAIPCBData_t *next;
    CATransportAdapter_t adapter; /* will always be either CA_ADAPTER_IP (UDP) or CA_ADAPTER_TCP */
    void (*ip_status_change_event_handler)(CATransportAdapter_t transport_type,
					   CANetworkStatus_t status);
    // @rewrite CAIPAdapterStateChangeCallback callback;
} CAIPCBData_t;
#endif

/**
 * Mutex for synchronizing access to cached interface and IP address information.
 */
/* #if EXPORT_INTERFACE */
oc_mutex g_networkMonitorContextMutex = NULL;
/* #endif */

/**
 * Used to storing network interface.
 * list of CAInterface_t* (i.e., IF address structs)
 */
// @rewrite g_nw_addresses @was g_netInterfaceList
u_arraylist_t *g_nw_addresses = NULL;

/**
 * Used to storing adapter changes callback interface.
 */
struct CAIPCBData_t *g_adapterCallbackList = NULL;

// @rewrite CAIPCreateNetworkAddressList @was CAIPInitializeNetworkMonitorList
CAResult_t CAIPCreateNetworkAddressList()
{
    if (!g_networkMonitorContextMutex)
    {
        g_networkMonitorContextMutex = oc_mutex_new();
        if (!g_networkMonitorContextMutex)
        {
            OIC_LOG(ERROR, TAG, "oc_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (!g_nw_addresses)
    {
        g_nw_addresses = u_arraylist_create();
        if (!g_nw_addresses)
        {
            OIC_LOG(ERROR, TAG, "u_arraylist_create has failed");
            CAIPDestroyNetworkAddressList();
            return CA_STATUS_FAILED;
        }
    }
    return CA_STATUS_OK;
}

/* DELEGATE: static void CAIPDestroyNetworkAddressList() */

/* DELEGATE: CAResult_t CAIPStartNetworkMonitor(CAIPAdapterStateChangeCallback callback,
 * 					     CATransportAdapter_t adapter) */

/* DELEGATE: CAResult_t CAIPStopNetworkMonitor(CATransportAdapter_t adapter) */

/**
 * Let the network monitor update the polling interval.
 * @param[in] interval Current polling interval, in seconds
 *
 * @return  desired polling interval
 */
int CAGetPollingInterval(int interval)
{
    return interval;
}

/* /\** */
/*  * Pass the changed network status through the stored callback. */
/*  *\/ */
/* // GAR: since IF changes are low in the stack they are passed up to both transports */
/* // NOTE: this is called recursively, from CAIPGetInterfaceInformation */
/* // @rewrite: CAIPPassNetworkChangesToTransports @was CAIPPassNetworkChangesToAdapter */
/* void CAIPPassNetworkChangesToTransports(CANetworkStatus_t status) */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "IN %s: status = %d", __func__, status); */

/*     // @rewrite: we only ever have two handlers at most, one for udp and one for tcp. */
/*     // what if we have multiple physical interfaces per transport? */
/* #ifdef ENABLE_IP */
/*     udp_status_change_handler(CA_ADAPTER_IP, status); */
/*     // log state of TRANSPORT adapter, not nw interface */
/*     CALogAdapterStateInfo(CA_ADAPTER_IP, status); */
/* #endif */
/* #ifdef ENABLE_TCP */
/*     tcp_status_change_handler(CA_ADAPTER_TCP, status); */
/*     CALogAdapterStateInfo(CA_ADAPTER_TCP, status); */
/* #endif */

/*     /\* CAIPCBData_t *cbitem = NULL; *\/ */
/*     /\* LL_FOREACH(g_adapterCallbackList, cbitem) *\/ */
/*     /\* { *\/ */
/*     /\*     if (cbitem && cbitem->adapter) *\/ */
/*     /\*     { *\/ */
/*     /\*         cbitem->ip_status_change_event_handler(cbitem->adapter, status); *\/ */
/*     /\*         CALogAdapterStateInfo(cbitem->adapter, status); *\/ */
/*     /\*     } *\/ */
/*     /\* } *\/ */
/*     OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__); */
/* }
 */

/**
 * Set callback for receiving local IP/TCP adapter connection status.
 *
 * @param[in]  callback     Callback to be notified when IP/TCP adapter connection state changes.
 * @param[in]  adapter      Transport adapter.
 * @return ::CA_STATUS_OK or an appropriate error code.
 */
// GAR: called by both CAStartUDP and CAStartTCP via CAIPStartNetworkMonitor, to install their
// status change event handlers
// @rewrite we can eliminate this by just calling the handlers
// @rewrite directly in CAIPPassNetworkChangesToAdapter
CAResult_t REMOVED_CAIPSetNetworkMonitorCallback(void
					 (*ip_status_change_handler)(CATransportAdapter_t adapter,
									    CANetworkStatus_t status),
					 // CAIPAdapterStateChangeCallback callback,
                                         CATransportAdapter_t adapter)
{
    if (!ip_status_change_handler)
    {
        OIC_LOG(ERROR, TAG, "ip_status_change_handler is null");
        return CA_STATUS_INVALID_PARAM;
    }

    CAIPCBData_t *cbitem = NULL;
    LL_FOREACH(g_adapterCallbackList, cbitem)
    {
        if (cbitem
	    && (cbitem->adapter == adapter)
	    && (cbitem->ip_status_change_event_handler == ip_status_change_handler))
        {
            OIC_LOG(DEBUG, TAG, "this ip_status_change_handler is already added");
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
    cbitem->ip_status_change_event_handler = ip_status_change_handler;
    LL_APPEND(g_adapterCallbackList, cbitem);

    return CA_STATUS_OK;
}

/**
 * Unset callback for receiving local IP/TCP adapter connection status.
 *
 * @param[in]  adapter      Transport adapter.
 * @return CA_STATUS_OK.
 */
// @rewrite CAIPUnSetNetworkMonitorCallback is defunct
CAResult_t CAIPUnSetNetworkMonitorCallback(CATransportAdapter_t adapter)
{
    CAIPCBData_t *cbitem = NULL;
    CAIPCBData_t *tmpCbitem = NULL;
    LL_FOREACH_SAFE(g_adapterCallbackList, cbitem, tmpCbitem)
    {
        if (cbitem && adapter == cbitem->adapter)
        {
            OIC_LOG(DEBUG, TAG, "remove specific ip_status_change_event_handler");
            LL_DELETE(g_adapterCallbackList, cbitem);
            OICFree(cbitem);
            return CA_STATUS_OK;
        }
    }
    return CA_STATUS_OK;
}

/* GAR: called by CASelectReturned */
/* DELEGATE: u_arraylist_t *CAFindInterfaceChange() */

/* GAR FIXME: CAIPGetAllInterfaceInformation(), not CAIPGetInterfaceInformation(0) */
/* DELEGATE: u_arraylist_t *CAIPGetInterfaceInformation(int desiredIndex) */
