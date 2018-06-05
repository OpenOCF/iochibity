/** @file udp_controller.c
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
#include "udp_controller.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <coap/pdu.h>

#include <errno.h>

#define USE_IP_MREQN
#if defined(_WIN32)
#undef USE_IP_MREQN
#endif

/**
 * Logging tag for module name.
 */
#define TAG "UDPCTRL"

/**
 * Enum for defining different server types.
 */
typedef enum
{
    CA_UNICAST_SERVER = 0,      /**< Unicast Server */
    CA_MULTICAST_SERVER,        /**< Multicast Server */
    CA_SECURED_UNICAST_SERVER,  /**< Secured Unicast Server */
    CA_NFC_SERVER               /**< Listening Server */
} CAAdapterServerType_t;

#if EXPORT_INTERFACE
/**
 * Callback to be notified on reception of any data from remote OIC devices.
 *
 * @param[in]  sep         network endpoint description.
 * @param[in]  data          Data received from remote OIC device.
 * @param[in]  dataLength    Length of data in bytes.
 * @pre  Callback must be registered using CAIPSetPacketReceiveCallback().
 */
typedef void (*CAIPPacketReceivedCallback)(const CASecureEndpoint_t *sep,
                                           const void *data,
                                           size_t dataLength);

/**
  * Callback to notify error in the IP adapter.
  *
  * @param[in]  endpoint       network endpoint description.
  * @param[in]  data          Data sent/received.
  * @param[in]  dataLength    Length of data in bytes.
  * @param[in]  result        result of request from R.I.
  * @pre  Callback must be registered using CAIPSetPacketReceiveCallback().
 */
typedef void (*CAIPErrorHandleCallback)(const CAEndpoint_t *endpoint, const void *data,
                                        size_t dataLength, CAResult_t result);
#endif

#if EXPORT_INTERFACE
#define SELECT_TIMEOUT 1     // select() seconds (and termination latency)
#define RECV_MSG_BUF_LEN 16384
#include <string.h>
#define CAIPS_GET_ERROR strerror(errno)
#endif

#ifndef SINGLE_THREAD
#if EXPORT_INTERFACE
/**
 * Holds inter thread ip data information.
 */
typedef struct
{
    CAEndpoint_t *remoteEndpoint;
    void *data;
    uint32_t dataLen;
    bool isMulticast;
} CAIPData_t;
#endif	/* EXPORT_INTERFACE */

#endif

// to signal shutdown
oc_cond udp_data_receiver_runloop_cond;

/**
 * List of the CAEndpoint_t* that has a stack-owned IP address.
 */
u_arraylist_t *g_local_endpoint_cache = NULL;

/**
 * Network Packet Received Callback to CA.
 */
// @rewrite udp_networkPacketCallback removed
/* static CANetworkPacketReceivedCallback udp_networkPacketCallback = NULL; */
/* static void (*udp_networkPacketCallback)(const CASecureEndpoint_t *sep, */
/* 				       const void *data, */
/* 				       size_t dataLen) = NULL; */

/**
 * Network Changed Callback to CA.
 */
// static CAAdapterChangeCallback udp_networkChangeCallback = NULL;
// @rewrite not used: static void (*udp_networkChangeCallback)(CATransportAdapter_t adapter, CANetworkStatus_t status) = NULL;


/**
 * error Callback to CA adapter.
 */
static CAErrorHandleCallback udp_errorCB = NULL;

/* static void CAIPPacketReceivedCB(const CASecureEndpoint_t *endpoint, */
/*                                  const void *data, size_t dataLength); */

#ifdef SINGLE_THREAD
void CAIPPullData()
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}
#endif

/* #ifdef __WITH_DTLS__ */
/* ssize_t CAIPPacketSendCB(CAEndpoint_t *endpoint, */
/* 			 const void *data, size_t dataLength); */
/* #endif */

#ifndef SINGLE_THREAD

/* called by CAStartUDP after udp_config_data_sockets has been called */
CAResult_t udp_start_services(const ca_thread_pool_t threadPool) // @was CAIPStartServer
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    CAResult_t res = CA_STATUS_OK;

    if (udp_is_started) return res;

    CAInitializeFastShutdownMechanism();

    CARegisterForAddressChanges();

    udp_selectTimeout = CAGetPollingInterval(udp_selectTimeout);

    res = udp_add_ifs_to_multicast_groups();  /* @was CAIPStartListenServer */
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "udp_add_ifs_to_multicast_groups failed with rc [%d]", res);
        return res;
    }

    udp_is_terminating = false;
    udp_data_receiver_runloop_cond = oc_cond_new();
    OIC_LOG_THREADS_V(DEBUG, TAG, "Adding udp_data_receiver_runloop to thread pool"); // for debugging
    res = ca_thread_pool_add_task(threadPool, udp_data_receiver_runloop, NULL);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread_pool_add_task failed for udp_data_receiver_runloop");
        return res;
    }
    OIC_LOG(DEBUG, TAG, "udp_data_receiver_runloop thread started successfully.");

    udp_is_started = true;
    return CA_STATUS_OK;
}

CAResult_t CAInitializeUDP(// CARegisterConnectivityCallback registerCallback,
			  // void (*registerCallback)(CAConnectivityHandler_t handler),
                          // CANetworkPacketReceivedCallback networkPacketCallback,
                          // CAAdapterChangeCallback netCallback,
                          /* CAErrorHandleCallback errorCallback, */
			  ca_thread_pool_t handle)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    //    VERIFY_NON_NULL_MSG(registerCallback, TAG, "registerCallback");
    // VERIFY_NON_NULL_MSG(networkPacketCallback, TAG, "networkPacketCallback");
    // VERIFY_NON_NULL_MSG(netCallback, TAG, "netCallback");
#ifndef SINGLE_THREAD
    VERIFY_NON_NULL_MSG(handle, TAG, "thread pool handle");
#endif

#ifdef WSA_WAIT_EVENT_0
    // Windows-specific initialization.
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData = {.wVersion = 0};
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (0 != err)
    {
        OIC_LOG_V(ERROR, TAG, "%s: WSAStartup failed: %i", __func__, err);
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, TAG, "WSAStartup Succeeded");
#endif

    // @rewrite: we do not need these "globals", we can just call the fns directly
    // udp_networkChangeCallback = CAAdapterChangedCallback;  // netCallback;
    // udp_networkPacketCallback = ifc_CAReceivedPacketCallback;  // networkPacketCallback;
    udp_errorCB            = CAAdapterErrorHandleCallback;  //errorCallback;

    // @rewrite: initialize statically: CAInitializeUDPGlobals();
    // @rewrite: statically initialize caglobals.ip.threadpool = handle;
    udp_threadpool = handle;

    // @rewrite g_ipErrorHandler removed, not need for CAIPSetErrorHandler(CAIPErrorHandler);
    // @rewrite remove CAIPSetPacketReceiveCallback(CAUDPPacketReceivedCB);

#ifdef __WITH_DTLS__
    if (CA_STATUS_OK != CAinitSslAdapter())
    {
        OIC_LOG(ERROR, TAG, "Failed to init SSL adapter");
    }
    else
    {
	CAsetSslAdapterCallbacks(// CAUDPPacketReceivedCB,
				 CAIPPacketSendCB, CAIPrrorHandler, CA_ADAPTER_IP);
    }
#endif

    // @NOTE: these are not needed since they are now called directly,
    // e.g. from CAStartListeningServerAdapters
    static const CAConnectivityHandler_t udpController =
        {
            .startAdapter         = &CAStartUDP, /* called directly in CAStartAdapter */
            .stopAdapter          = &CAStopIP,	 /* called directly in CAStopAdapter */
            /* .startListenServer    = &CAStartIPListeningServer, */
            .startListenServer    = &udp_add_ifs_to_multicast_groups, // @was CAIPStartListenServer,
            .stopListenServer     = &udp_close_sockets, // @was CAStopIPListeningServer -> CAIPStopListenServer,
            .startDiscoveryServer = &CAStartIPDiscoveryServer, /* CAStartDiscoveryServerAdapters */
	    // why no stopDiscoveryServer?
            .unicast              = &CASendIPUnicastData,
            .multicast            = &CASendIPMulticastData,
            .GetNetInfo           = &udp_get_local_endpoints, // @was CAGetIPInterfaceInformation
#ifdef SINGLE_THREAD
            .readData             = &CAReadIPData,	      /* only used ifdef SINGLE_THREAD? */
#endif
            .terminate            = &CATerminateIP,
            .cType                = CA_ADAPTER_IP
        };
    //registerCallback(udpController);
    CARegisterCallback(udpController);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

/* CAAdapterStart */
// called by CAStartAdapter(CATransportAdapter_t transportType)
CAResult_t CAStartUDP()		// @rewrite @was CAStartIP (?)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    // @rewrite removed CAInitializeUDPGlobals(); in favor of static initialization

    // Specific the port number received from application.
    // ??? where are caglobals.ports set?
    // @rewrite: i can't see where ports are set, so this does no good:
    /* caglobals.ip.u6.port  = caglobals.ports.udp.u6; */
    /* caglobals.ip.u6s.port = caglobals.ports.udp.u6s; */
    /* caglobals.ip.u4.port  = caglobals.ports.udp.u4; */
    /* caglobals.ip.u4s.port = caglobals.ports.udp.u4s; */

    // @rewrite CAIPStartNetworkMonitor(CAIPAdapterHandler, CA_ADAPTER_IP);
    //  which calls CAIPInitializeNetworkMonitorList, then
    // CAIPSetNetworkMonitorCallback, which sets g_adapterCallbackList

    // we can skip this, since CAIPPassNetworkChangesToAdapters will
    // call the cb directly for each transport rather than go thru
    // g_adapterCallbackList

    /* addresses will be added by udp_get_ifs_for_rtm_newaddr, from CAFindInterfaceChange etc. */
    // @rewrite: the created list is never used, so why bother?
    CAIPCreateNetworkInterfaceList(); // @was CAIPStartNetworkMonitor

#ifdef SINGLE_THREAD
    uint16_t unicastPort = 55555;
    // Address is hardcoded as we are using Single Interface

    // udp_config_data_sockets();

    CAResult_t ret = udp_start_services();  // @was CAIPStartServer();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(DEBUG, TAG, "udp_start_services failed[%d]", ret);
        return ret;
    }
#else
    if (CA_STATUS_OK != udp_start_send_msg_queue())
    {
        OIC_LOG(ERROR, TAG, "Failed udp_start_send_msg_queue");
        CATerminateIP();
        return CA_STATUS_FAILED;
    }

    CAResult_t ret = udp_config_data_sockets((const ca_thread_pool_t)udp_threadpool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "udp_config_data_sockets failed, code %d", ret);
        return ret;
    }

    ret = udp_start_services((const ca_thread_pool_t)udp_threadpool); // @was CAIPStartServer
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to start server![%d]", ret);
        return ret;
    }
#endif

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAStopIPListeningServer()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    CAResult_t ret = udp_close_sockets(); // @was CAIPStopListenServer();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to stop listening server![%d]", ret);
    }

    return ret;
}

// GAR: clients run this, but not ListeningServer
CAResult_t CAStartIPDiscoveryServer()
{
    // @rewrite @was CAStartIPListeningServer -> CAIPStartListenServer
    return udp_add_ifs_to_multicast_groups();
}

#ifdef SINGLE_THREAD
CAResult_t CAReadIPData()
{
    CAIPPullData();
    return CA_STATUS_OK;
}
#endif

CAResult_t CAStopIP()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

#ifdef __WITH_DTLS__
    CAdeinitSslAdapter();
#endif

#ifndef SINGLE_THREAD
    // if (udp_sendQueueHandle && udp_sendQueueHandle->threadMutex)
    if (NULL != udp_sendQueueHandle.threadMutex)
    {
        CAQueueingThreadStop(&udp_sendQueueHandle);
    }
#endif

    CAIPStopNetworkMonitor(CA_ADAPTER_IP);
    CAIPStopServer();

    // CATerminateIP();	 why isn't this called on shutdown?

    return CA_STATUS_OK;
}

void CATerminateIP()
{
#ifdef __WITH_DTLS__
    CAsetSslAdapterCallbacks(//NULL,
			     NULL, NULL, CA_ADAPTER_IP);
#endif

    // @rewrite remove CAIPSetPacketReceiveCallback(NULL);

#ifndef SINGLE_THREAD
    udp_stop_send_msg_queue(); // @was CAIPDeinitializeQueueHandles
#endif

#ifdef WSA_WAIT_EVENT_0
    // Windows-specific clean-up.
    OC_VERIFY(0 == WSACleanup());
#endif
}

/*
 * Get realtime list of one address per unique (IF, family) pair
 */
CAResult_t udp_get_local_endpoints(CAEndpoint_t **info, size_t *size) // @was CAGetIPInterfaceInformation
{
    VERIFY_NON_NULL_MSG(info, TAG, "info is NULL");
    VERIFY_NON_NULL_MSG(size, TAG, "size is NULL");

    // GAR: get live list of CAInterface_t, each represents a unique (IF, family) pair
    u_arraylist_t *iflist = udp_get_ifs_for_rtm_newaddr(0);
    if (!iflist) {
        OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

#ifdef __WITH_DTLS__
    const size_t endpointsPerInterface = 2;
#else
    const size_t endpointsPerInterface = 1;
#endif

    /* get a count of enabled IPv4/IPv6 IFs */
    size_t interfaces = u_arraylist_length(iflist);
    for (size_t i = 0; i < u_arraylist_length(iflist); i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
        if (!ifitem)
        {
            continue;
        }

        if ((ifitem->family == AF_INET6 && !udp_ipv6_is_enabled) ||
            (ifitem->family == AF_INET && !udp_ipv4_is_enabled))
        {
            interfaces--;
        }
    }

    if (!interfaces)
    {
        OIC_LOG(DEBUG, TAG, "No enabled IPv4/IPv6 interfaces found");
        return CA_STATUS_OK;
    }

    size_t totalEndpoints = interfaces * endpointsPerInterface;
    CAEndpoint_t *eps = (CAEndpoint_t *)OICCalloc(totalEndpoints, sizeof (CAEndpoint_t));
    if (!eps)
    {
        OIC_LOG(ERROR, TAG, "Malloc Failed");
        u_arraylist_destroy(iflist);
        return CA_MEMORY_ALLOC_FAILED;
    }

    /* create one CAEndpoint_t per (IF, family) */
    for (size_t i = 0,		/* index into iflist to control iteration */
	     j = 0;		/* index into eps array */
	 i < u_arraylist_length(iflist); i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
        if (!ifitem)
        {
            continue;
        }

	/* skip disabled IFs */
        if ((ifitem->family == AF_INET6 && !udp_ipv6_is_enabled) ||
            (ifitem->family == AF_INET && !udp_ipv4_is_enabled))
        {
            continue;
        }

        eps[j].adapter = CA_ADAPTER_IP; /* meaning CA_ADAPTER_UDP */
        eps[j].ifindex = ifitem->index;

        if (ifitem->family == AF_INET6)
        {
            eps[j].flags = CA_IPV6;
            eps[j].port = udp_u6.port;
        }
        else
        {
            eps[j].flags = CA_IPV4;
            eps[j].port = udp_u4.port;
        }
        OICStrcpy(eps[j].addr, sizeof(eps[j].addr), ifitem->addr);

#ifdef __WITH_DTLS__
	/* add secured endpoint */
        j++;

        eps[j].adapter = CA_ADAPTER_IP;
        eps[j].ifindex = ifitem->index;

        if (ifitem->family == AF_INET6)
        {
            eps[j].flags = CA_IPV6 | CA_SECURE;
            eps[j].port = udp_u6s.port;
        }
        else
        {
            eps[j].flags = CA_IPV4 | CA_SECURE;
            eps[j].port = udp_u4s.port;
        }
        OICStrcpy(eps[j].addr, sizeof(eps[j].addr), ifitem->addr);
#endif
        j++;
    }

    *info = eps;
    *size = totalEndpoints;

    u_arraylist_destroy(iflist);

    return CA_STATUS_OK;
}

u_arraylist_t *local_endpoints = NULL;

u_arraylist_t *oocp_get_local_endpoints() EXPORT
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    /* size_t len = u_arraylist_length(g_local_endpoint_cache); */
    /* for (size_t i = 0; i < len; i++) */
    /* { */
    /*     CAEndpoint_t *ownIpEp = u_arraylist_get(g_local_endpoint_cache, i); */
    /* 	OIC_LOG_V(DEBUG, TAG, "local ptr: %p", ownIpEp); */
    /* } */

    local_endpoints = u_arraylist_create();

    if (!local_endpoints)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed! (local_endpoints)");
        return NULL;
    }

        CAEndpoint_t *eps = NULL;
        size_t numOfEps = 0;

        CAResult_t res = udp_get_local_endpoints(&eps, &numOfEps); /* @was CAGetIPInterfaceInformation */
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CAGetIPInterfaceInformation failed");
            return NULL;
        }
	OIC_LOG_V(DEBUG, TAG, "Found %d local endpoints", numOfEps);

        for (size_t i = 0; i < numOfEps; i++)
        {
	    OIC_LOG_V(DEBUG, TAG, "Caching %d: %s [%d]", i, eps[i].addr, eps[i].ifindex);
            u_arraylist_add(local_endpoints, (void *)&eps[i]);
        }
	return local_endpoints;
	//return g_local_endpoint_cache;
}

#include <stdint.h>
CAResult_t CAGetLinkLocalZoneId(uint32_t ifindex, char **zoneId)
{
    return CAGetLinkLocalZoneIdInternal(ifindex, zoneId);
}

// FIXMME: move this to udp_status_manager?
// @rewrite udp_update_local_endpoint_cache @was CAUpdateStoredIPAddressInfo
// called by udp_if_change_handler (@was CAIPPassNetworkChangesToAdapter)
void udp_update_local_endpoint_cache(CANetworkStatus_t status) // @was CAUpdateStoredIPAddressInfo
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    if (CA_INTERFACE_UP == status)
    {
        OIC_LOG(DEBUG, TAG, "UDP status is UP. Caching local endpoints");

        CAEndpoint_t *eps = NULL;
        size_t numOfEps = 0;

        CAResult_t res = udp_get_local_endpoints(&eps, &numOfEps); /* @was CAGetIPInterfaceInformation */
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CAGetIPInterfaceInformation failed");
            return;
        }
	OIC_LOG_V(DEBUG, TAG, "Found %d local endpoints", numOfEps);

        for (size_t i = 0; i < numOfEps; i++)
        {
	    // FIXME: what about duplicates?
	    OIC_LOG_V(DEBUG, TAG, "Caching %d: %s [%d]", i, eps[i].addr, eps[i].ifindex);
            u_arraylist_add(g_local_endpoint_cache, (void *)&eps[i]);
        }
    }
    else // CA_INTERFACE_DOWN
    {
        OIC_LOG(DEBUG, TAG, "UDP status is off. Clearing the local endpoint cache");

        CAEndpoint_t *headEp = u_arraylist_get(g_local_endpoint_cache, 0);
        OICFree(headEp);
        headEp = NULL;

        size_t len = u_arraylist_length(g_local_endpoint_cache);
        for (size_t i = len; i > 0; i--)
        {
            u_arraylist_remove(g_local_endpoint_cache, i - 1);
        }
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

#endif // not SINGLE_THREAD

bool CAIPIsLocalEndpoint(const CAEndpoint_t *ep)
{
    char addr[MAX_ADDR_STR_SIZE_CA];
    OICStrcpy(addr, MAX_ADDR_STR_SIZE_CA, ep->addr);

    // drop the zone ID if the address of endpoint is IPv6. ifindex will be checked instead.
    if ((ep->flags & CA_IPV6) && strchr(addr, '%'))
    {
        strtok(addr, "%");
    }

    size_t len = u_arraylist_length(g_local_endpoint_cache);
    for (size_t i = 0; i < len; i++)
    {
        CAEndpoint_t *ownIpEp = u_arraylist_get(g_local_endpoint_cache, i);
        if (!strcmp(addr, ownIpEp->addr) && ep->port == ownIpEp->port
                                         && ep->ifindex == ownIpEp->ifindex)
        {
            return true;
        }
    }

    return false;
}

void CAIPErrorHandler(const CAEndpoint_t *endpoint, const void *data,
                      size_t dataLength, CAResult_t result)
{
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    if (udp_errorCB)
    {
        udp_errorCB(endpoint, data, dataLength, result);
    }
}
