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

#include "cainterfacecontroller.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "utlist.h"

#include <assert.h>

#define TAG "OIC_CA_INF_CTR"

/**
 * Information about the network status.
 */
#if EXPORT_INTERFACE
typedef enum
{
    CA_INTERFACE_DOWN,   /**< Connection is not available */
    CA_INTERFACE_UP    /**< Connection is Available */
} CANetworkStatus_t;
#endif	/* INTERFACE */

#if EXPORT_INTERFACE
/**
 * Callback function type for connection status changes delivery.
 * @param[out]   info           Remote endpoint information.
 * @param[out]   isConnected    Current connection status info.
 */
typedef void (*CAConnectionStateChangedCB)(const CAEndpoint_t *info, bool isConnected);

/**
 * Callback function type for adapter status changes delivery.
 * @param[out]   adapter    Transport type information.
 * @param[out]   enabled    Current adapter status info.
 */
typedef void (*CAAdapterStateChangedCB)(CATransportAdapter_t adapter, bool enabled);

#if defined(TCP_ADAPTER) || defined(EDR_ADAPTER) || defined(LE_ADAPTER)
#define STATEFUL_PROTOCOL_SUPPORTED
#endif
#endif	/* INTERFACE */

#define CA_MEMORY_ALLOC_CHECK(arg) {if (arg == NULL) \
    {OIC_LOG(ERROR, TAG, "memory error");goto memory_error_exit;} }

// GAR: array of controller structs containing method pointers
static CAConnectivityHandler_t *g_adapterHandler = NULL;

static size_t g_numberOfAdapters =
#ifdef IP_ADAPTER
1
#endif
#ifdef ENABLE_TCP
+ 1
#endif
    ;

// @rewrite g_networkPacketReceivedCallback removed
/* static CANetworkPacketReceivedCallback g_networkPacketReceivedCallback = NULL; */
/* static void (*g_networkPacketReceivedCallback)(const CASecureEndpoint_t *sep, */
/* 					       const void *data, size_t dataLen); */

/**
 * This will be used to notify error result to the connectivity common logic layer.
 */
#if INTERFACE
typedef void (*CAErrorHandleCallback)(const CAEndpoint_t *endpoint,
                                      const void *data, size_t dataLen,
                                      CAResult_t result);
#endif
CAQueueingThread_t g_networkChangeCallbackThread;

static CAErrorHandleCallback g_errorHandleCallback = NULL;

/**
 * network callback structure is handling
 * for adapter state changed and connection state changed event.
 */
#if INTERFACE
struct CANetworkCallback
{

    /** Linked list; for multiple callback list.*/
    struct CANetworkCallback *next;

    /** Adapter state changed event callback. */
    // GAR FIXME: change name to indicate its an event handler
    void (*adapter)(CATransportAdapter_t adapter, bool enabled);
    // CAAdapterStateChangedCB adapter;

    /** Connection state changed event callback. */
    // @rewrite CAConnectionStateChangedCB conn;
    void (*conn)(const CAEndpoint_t *info, bool isConnected);

};
#endif

/**
 * struct to wrap the network change callback info.
 */
#if INTERFACE
typedef struct CANetworkCallbackThreadInfo_t
{
    void (*adapterCB)(CATransportAdapter_t adapter, bool enabled);
    //CAAdapterStateChangedCB adapterCB;
    CATransportAdapter_t adapter;
    bool isInterfaceUp;

#ifdef STATEFUL_PROTOCOL_SUPPORTED
    CAConnectionStateChangedCB connectionCB;
    CAEndpoint_t *endpoint;
    bool isConnected;
#endif
} CANetworkCallbackThreadInfo_t;
#endif

static void CANetworkChangeCallbackThreadProcess(void *threadData)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY (g_networkChangeCallbackThread)", __func__);
    assert(threadData);

    CANetworkCallbackThreadInfo_t *info = (CANetworkCallbackThreadInfo_t *) threadData;
    if (info->adapterCB)
    {
        info->adapterCB(info->adapter, info->isInterfaceUp);
    }
#ifdef STATEFUL_PROTOCOL_SUPPORTED
    else if(info->connectionCB)
    {
        info->connectionCB(info->endpoint, info->isConnected);
    }
#endif
}

static void CADestroyNetworkChangeCallbackData(void *data, uint32_t size)
{
    assert(data);
    OC_UNUSED(size);

    CANetworkCallbackThreadInfo_t *info = (CANetworkCallbackThreadInfo_t *) data;
#ifdef STATEFUL_PROTOCOL_SUPPORTED
    if (info->endpoint)
    {
        CAFreeEndpoint(info->endpoint);
        info->endpoint = NULL;
    }
#endif
    OICFree(info);
    info = NULL;
}

// remove unnecessary fn: CAGetAdapterIndex

// @rewrite: eliminate CARegisterCallback
//static
void CARegisterCallback(CAConnectivityHandler_t handler)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);

    CAConnectivityHandler_t *adapterHandler = OICRealloc(g_adapterHandler,
            (g_numberOfAdapters) * sizeof(*adapterHandler));
    if (NULL == adapterHandler)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed during registration");
        return;
    }
    g_adapterHandler = adapterHandler;
    g_adapterHandler[g_numberOfAdapters - 1] = handler;

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

#ifdef RA_ADAPTER
CAResult_t CASetAdapterRAInfo(const CARAInfo_t *caraInfo)
{
    return CASetRAInfo(caraInfo);
}
#endif

// @rewrite remove unnecessary fn: CAReceivedPacketCallback

// @rewrite CAAdapterChangedCallback called by each transport
// @rewrite We don't need this routine, since we call OCDefaultAdapterStateChangedHandler
// @rewrite  directly from udp_if_change_handler,
// this is for "adapter changes" (udp and tcp), for tcp connection changes: CAConnectionChangedCallback
// FIXME: move this to comm/L2_status_manager.c?
// FIXME: rename nw to linklayer, interfacelayer?
void oocf_enqueue_interface_chg_work_pkg( // @was CAAdapterChangedCallback
				  CATransportAdapter_t adapter, CANetworkStatus_t status)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OIC_LOG_V(DEBUG, TAG, "[%d] adapter state is changed to [%d]", adapter, status);

    // @rewrite: original code iterates over g_networkChangeCallbackList,
    // which contains adapterchange callbacks for nw level and tcp

    CANetworkCallbackThreadInfo_t *info = (CANetworkCallbackThreadInfo_t *)
	OICCalloc(1, sizeof(CANetworkCallbackThreadInfo_t));
    if (!info) {
	OIC_LOG(ERROR, TAG, "OICCalloc to info failed!");
	return;
    }
    // udp: OCDefaultAdapterStateChangedHandler
    info->adapterCB = OCDefaultAdapterStateChangedHandler;
    info->adapter = adapter;
    info->isInterfaceUp = (CA_INTERFACE_UP == status);
    CAQueueingThreadAddData(&g_networkChangeCallbackThread, info,
			    sizeof(CANetworkCallbackThreadInfo_t));

#ifdef ENABLE_TCP
    CANetworkCallbackThreadInfo_t *tcp_info = (CANetworkCallbackThreadInfo_t *)
	OICCalloc(1, sizeof(CANetworkCallbackThreadInfo_t));
    if (!tcp_info) {
	OIC_LOG(ERROR, TAG, "OICCalloc to tcp_info failed!");
	return;
    }
    tcp_info->adapterCB = OCAdapterStateChangedHandler;
    tcp_info->adapter = adapter;
    tcp_info->isInterfaceUp = (CA_INTERFACE_UP == status);
    CAQueueingThreadAddData(&g_networkChangeCallbackThread, tcp_info,
			    sizeof(CANetworkCallbackThreadInfo_t));
#endif
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

// static
// FIXME: this is only for outbound processing errors, rename it e.g. sender_error_handler
void CAAdapterErrorHandleCallback(const CAEndpoint_t *endpoint,
        const void *data, size_t dataLen,
        CAResult_t result)
{
    OIC_LOG(DEBUG, TAG, "received error from adapter in interfacecontroller");

    // Call the callback.
    if (g_errorHandleCallback != NULL)
    {
        g_errorHandleCallback(endpoint, data, dataLen, result);
    }
}

CAResult_t CAInitializeAdapters(ca_thread_pool_t thread_pool, CATransportAdapter_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY: %s (%d)", __func__,
	      CA_TRANSPORT_ADAPTER_STRING(transportType), transportType);

    // GAR: note that CA_DEFAULT_ADAPTER seems to mean all available adapters

    // Initialize adapters and register callback.
#ifndef DISABLE_UDP
    OIC_LOG_V(DEBUG, TAG, "UDP is enabled");
    if ((transportType & CA_ADAPTER_IP) || (CA_DEFAULT_ADAPTER == transportType)
        || (transportType == CA_ALL_ADAPTERS))
    {
        CAInitializeUDP(thread_pool);
    }
#endif /* DISABLE_UDP */

/* #ifdef EDR_ADAPTER */
/*     if ((transportType & CA_ADAPTER_RFCOMM_BTEDR) || (CA_DEFAULT_ADAPTER == transportType)) */
/*     { */
/*         CAInitializeEDR(/\* CARegisterCallback, *\/ */
/* 			ifc_CAReceivedPacketCallback, CAAdapterChangedCallback, */
/*                         CAConnectionChangedCallback, CAAdapterErrorHandleCallback, thread_pool); */
/*     } */
/* #endif /\* EDR_ADAPTER *\/ */

/* #ifdef LE_ADAPTER */
/*     if ((transportType & CA_ADAPTER_GATT_BTLE) || (CA_DEFAULT_ADAPTER == transportType)) */
/*     { */
/*         CAInitializeLE(CARegisterCallback, ifc_CAReceivedPacketCallback, CAAdapterChangedCallback, */
/*                        CAConnectionChangedCallback, CAAdapterErrorHandleCallback, thread_pool); */
/*     } */
/* #endif /\* LE_ADAPTER *\/ */

/* #ifdef RA_ADAPTER */
/*     if ((transportType & CA_ADAPTER_REMOTE_ACCESS) || (CA_DEFAULT_ADAPTER == transportType)) */
/*     { */
/*         CAInitializeRA(CARegisterCallback, ifc_CAReceivedPacketCallback, CAAdapterChangedCallback, */
/*                        thread_pool); */
/*     } */
/* #endif /\* RA_ADAPTER *\/ */

#ifdef ENABLE_TCP
    OIC_LOG_V(DEBUG, TAG, "TCP is enabled");
    if ((transportType & CA_ADAPTER_TCP) || (CA_DEFAULT_ADAPTER == transportType))
    {
        CAInitializeTCP(thread_pool);
    }
#endif /* ENABLE_TCP */

/* #ifdef NFC_ADAPTER */
/*     if ((transportType & CA_ADAPTER_NFC) || (CA_DEFAULT_ADAPTER == transportType)) */
/*     { */
/*         CAInitializeNFC(CARegisterCallback, ifc_CAReceivedPacketCallback, CAAdapterChangedCallback, */
/*                         CAAdapterErrorHandleCallback, thread_pool); */
/*     } */
/* #endif /\* NFC_ADAPTER *\/ */

    CAResult_t res = CA_STATUS_OK;

    // Initialize & Start network-change-callback-thread.
#ifdef DEBUG_THREADS
    g_networkChangeCallbackThread.name = "g_networkChangeCallbackThread";
#endif
    res = CAQueueingThreadInitialize(&g_networkChangeCallbackThread,
				     thread_pool,
                                     CANetworkChangeCallbackThreadProcess,
                                     CADestroyNetworkChangeCallbackData);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize callback queue thread");
        return res;
    }

    res = CAQueueingThreadStart(&g_networkChangeCallbackThread);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread start error(callback thread).");
        return res;
    }

    return CA_STATUS_OK;
}

// @rewrite remove CASetPacketReceivedCallback

CAResult_t CASetNetworkMonitorCallbacks(CAAdapterStateChangedCB adapterCB,
					       CAConnectionStateChangedCB connCB)
{
    OIC_LOG(DEBUG, TAG, "Set network monitoring callback");
    /* CAResult_t res = AddNetworkStateChangedCallback(adapterCB, connCB); */
    /* if (CA_STATUS_OK != res) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "AddNetworkStateChangedCallback has failed"); */
    /*     return CA_STATUS_FAILED; */
    /* } */
    return CA_STATUS_OK;
}

CAResult_t CAUnsetNetworkMonitorCallbacks(CAAdapterStateChangedCB adapterCB,
                                          CAConnectionStateChangedCB connCB)
{
    OIC_LOG(DEBUG, TAG, "Unset network monitoring callback");
    /* CAResult_t res = RemoveNetworkStateChangedCallback(adapterCB, connCB); */
    /* if (CA_STATUS_OK != res) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "RemoveNetworkStateChangedCallback has failed"); */
    /*     return CA_STATUS_FAILED; */
    /* } */
    return CA_STATUS_OK;
}

void CASetErrorHandleCallback(CAErrorHandleCallback errorCallback)
{
    OIC_LOG(DEBUG, TAG, "Set error handle callback");
    g_errorHandleCallback = errorCallback;
}

// called by CAAddNetworkType(CATransportAdapter_t transportType)
CAResult_t CAStartAdapter(CATransportAdapter_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY: transport adapter: %s (%d)", __func__,
	      CA_TRANSPORT_ADAPTER_STRING(transportType), transportType);

#ifndef DISABLE_UDP
    if (transportType == CA_ADAPTER_IP) {
	CAStartUDP();
	return CA_STATUS_OK;
    }
#endif

#ifdef ENABLE_TCP
    if (transportType == CA_ADAPTER_TCP) {
	CAStartTCP();
	return CA_STATUS_OK;
    }
#endif

    return CA_STATUS_FAILED;
}

void CAStopAdapter(CATransportAdapter_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, transport adapter %s (%d)", __func__,
	      CA_TRANSPORT_ADAPTER_STRING(transportType), transportType);

    if (transportType == CA_ADAPTER_IP) {
#ifndef DISABLE_UDP
	CAStopIP();
	OIC_LOG_V(INFO, TAG, "%s (UDP) EXIT", __func__);
	return;
#endif
    }

    if (transportType == CA_ADAPTER_TCP) {
#ifdef ENABLE_TCP
	CAStopTCP();		/* GAR: why not CATerminateTCP? */
	OIC_LOG_V(INFO, TAG, "%s (TCP) EXIT", __func__);
	return;
#endif
    }
}

void CAStopAdapters(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
#ifndef DISABLE_UDP
    CAStopAdapter(CA_ADAPTER_IP);
#endif
#ifdef ENABLE_TCP
    CAStopAdapter(CA_ADAPTER_TCP);
#endif
    CAQueueingThreadStop(&g_networkChangeCallbackThread);
}

// CAGetNetworkInfo, meaning get endpoints
CAResult_t CAGetNetworkInfo(CAEndpoint_t **ep_list_ptr, size_t *ep_count_ptr)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL_MSG(ep_list_ptr, TAG, "ep_list_ptr is null");
    VERIFY_NON_NULL_MSG(ep_count_ptr, TAG, "ep_count_ptr is null");

    OIC_LOG_V(DEBUG, TAG, "%s number of transports: %d", __func__, g_numberOfAdapters);

    CAResult_t res = CA_STATUS_FAILED;
    size_t ep_count = 0;
#ifndef DISABLE_UDP
    size_t udp_ep_count;
    CAEndpoint_t *udp_ep; /* array of eps */
    //udp_ep = null_ep;		/* reinitialize */
    //pudp_ep = NULL;
    res = udp_get_local_endpoints(&udp_ep, &udp_ep_count);
    if (res == CA_STATUS_OK) { ep_count += udp_ep_count; }
    OIC_LOG_V(INFO, TAG, "%s got %d local udp eps", __func__, udp_ep_count);
#endif

#ifdef ENABLE_TCP
    /* FIXME: nifs are transport independent? */
    size_t tcp_ep_count;
    CAEndpoint_t *tcp_ep; /* array */
    res = CA_STATUS_FAILED;
    res = tcp_get_local_endpoints(&tcp_ep, &tcp_ep_count); // @was CAGetTCPInterfaceInformation
    if (res == CA_STATUS_OK) {
	ep_count += tcp_ep_count;
	OIC_LOG_V(INFO, TAG, "%s got %d local tcp eps", __func__, tcp_ep_count);
	OIC_LOG_V(INFO, TAG, "%s 1st addr: %s", __func__, tcp_ep[0].addr);
	OIC_LOG_V(INFO, TAG, "%s 1st port: %d", __func__, tcp_ep[0].port);
	OIC_LOG_V(INFO, TAG, "%s 1st ifindex: %d", __func__, tcp_ep[0].ifindex);
    } else {
	OIC_LOG_V(ERROR, TAG, "%s tcp_get_local_endpoints error %d", __func__, res);
    }
#endif

    /* OIC_LOG_V(DEBUG, TAG, */
    /* 	      "%" PRIuPTR " adapter network info size is %" PRIuPTR " res:%u", */
    /* 	      0, sz, udp_res); */

    OIC_LOG_V(DEBUG, TAG, "nif count total is %" PRIuPTR "!", ep_count);

    /* if (ep_count == 0) */
    /* { */
    /*     OICFree(tempInfo); */
    /*     OICFree(tempSize); */
    /*     return res; */
    /* } */

    // #3. add data into result
    // memory allocation
    CAEndpoint_t *ep_list = (CAEndpoint_t *) OICCalloc(ep_count, sizeof (*ep_list));
    /* CA_MEMORY_ALLOC_CHECK(ep_list); */
    if (ep_list == NULL)
    {
        OIC_LOG(ERROR, TAG, "ep_list calloc failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // #4. save data
    *ep_list_ptr = ep_list;
    *ep_count_ptr = ep_count;

#ifndef DISABLE_UDP
        memcpy(ep_list,
               udp_ep,  // tempInfo[0],
               sizeof(*ep_list) * udp_ep_count); // tempSize[index]);
        ep_list += udp_ep_count; //tempSize[index];
#endif
#ifdef ENABLE_TCP
        memcpy(ep_list,
               tcp_ep,  // tempInfo[0],
               sizeof(*ep_list) * tcp_ep_count);
        ep_list += udp_ep_count;
#endif

	OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
	return CA_STATUS_OK;
}

/* GAR
   find the controller methods struct of the selected transport
   then call its unicast method
   @rewrite: just make the method a member of nw_<transport>_controller.c
   and call it directly
 */
CAResult_t CASendUnicastData(const CAEndpoint_t *endpoint, const void *data, uint32_t length,
                             CADataType_t dataType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_LOG_V(DEBUG, TAG, "enpoint adapter: %x", endpoint->adapter);

    // @rewrite
    int32_t sentDataLen = 0;

    CATransportAdapter_t requestedAdapter = endpoint->adapter ? endpoint->adapter : CA_ALL_ADAPTERS;

    OIC_LOG(DEBUG, TAG, "unicast message to adapter");
#ifndef DISABLE_UDP
    if (requestedAdapter == CA_ADAPTER_IP)
	sentDataLen = CASendIPUnicastData(endpoint, data, length, dataType);
#endif
#ifdef ENABLE_TCP
    if (requestedAdapter == CA_ADAPTER_TCP)
	sentDataLen = CASendTCPUnicastData(endpoint, data, length, dataType);
#endif

    if ((0 > sentDataLen) || ((uint32_t)sentDataLen != length)) {
	OIC_LOG_V(ERROR, TAG, "Error sending data: data len: %d, sent len: %d", length, sentDataLen);
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

CAResult_t CASendMulticastData(const CAEndpoint_t *endpoint, const void *data, uint32_t length,
                               CADataType_t dataType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    // @rewrite
    uint32_t sentDataLen = 0;

    void *payload = (void *) OICMalloc(length);
    if (!payload)
	{
	    OIC_LOG(ERROR, TAG, "Out of memory!");
	    return CA_MEMORY_ALLOC_FAILED;
	}
    memcpy(payload, data, length);
#ifndef DISABLE_UDP
    sentDataLen = CASendIPMulticastData(endpoint, payload, length, dataType);
#endif

#ifdef ENABLE_TCP
    sentDataLen = CASendTCPMulticastData(endpoint, payload, length, dataType);
#endif

    OICFree(payload);

    if (sentDataLen != length) {
	OIC_LOG(ERROR, TAG, "multicast failed! Error will be reported from adapter");
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAStartListeningServerAdapters(void)
{
    CAResult_t result = CA_STATUS_FAILED;
#ifndef DISABLE_UDP
    result = udp_configure_eps(); //  @was CAIPStartListenServer();
#endif
#ifdef ENABLE_TCP
    result = CAStartTCPListeningServer();
#endif
    return result;
}

CAResult_t CAStopListeningServerAdapters(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
#ifndef DISABLE_UDP
    udp_close_sockets();
#endif

#ifdef ENABLE_TCP
    CAStopTCPListeningServer();	/* NB: doesn't actually do anything */
#endif
    return CA_STATUS_OK;
}

CAResult_t CAStartDiscoveryServerAdapters(void)
{
    /* NOTE: startdiscover etc. just calls CAIPStartListenServer,
       which adds ifs to multicast groups */
    CAResult_t result = CA_STATUS_FAILED;
#ifndef DISABLE_UDP
    result = udp_configure_eps(); // @was CAIPStartListenServer;
#endif
#ifdef ENABLE_TCP
    result = CAStartTCPDiscoveryServer();
#endif
    return result;
}

bool CAIsLocalEndpoint(const CAEndpoint_t *ep)
{
    VERIFY_NON_NULL_RET(ep, TAG, "ep is null", false);

#ifndef DISABLE_UDP
    if (ep->adapter & CA_ADAPTER_IP)
    {
        return CAIPIsLocalEndpoint(ep);
    }
#endif /* IP_ADAPTER */

    //TODO: implement for the other adapters(EDR/LE/NFC)
    return false;
}

void CATerminateAdapters(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
#ifndef DISABLE_UDP
    CATerminateIP();
#endif

    OICFree(g_adapterHandler);
    g_adapterHandler = NULL;
    g_numberOfAdapters = 0;

    CAQueueingThreadDestroy(&g_networkChangeCallbackThread);
    /* RemoveAllNetworkStateChangedCallback(); */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}
