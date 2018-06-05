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

#ifndef SINGLE_THREAD
#include <assert.h>
/* #include "caqueueingthread.h" */
#endif

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

#define STATEFUL_PROTOCOL_SUPPORTED ((TCP_ADAPTER) || (EDR_ADAPTER) || (LE_ADAPTER))
#endif	/* INTERFACE */

#define CA_MEMORY_ALLOC_CHECK(arg) {if (arg == NULL) \
    {OIC_LOG(ERROR, TAG, "memory error");goto memory_error_exit;} }

// GAR: array of controller structs containing method pointers
static CAConnectivityHandler_t *g_adapterHandler = NULL;

static size_t g_numberOfAdapters = 0;

// @rewrite g_networkPacketReceivedCallback removed
/* static CANetworkPacketReceivedCallback g_networkPacketReceivedCallback = NULL; */
/* static void (*g_networkPacketReceivedCallback)(const CASecureEndpoint_t *sep, */
/* 					       const void *data, size_t dataLen); */


static CAErrorHandleCallback g_errorHandleCallback = NULL;

static struct CANetworkCallback_t *g_networkChangeCallbackList = NULL;

#ifndef SINGLE_THREAD
CAQueueingThread_t g_networkChangeCallbackThread;
#endif

/**
 * network callback structure is handling
 * for adapter state changed and connection state changed event.
 */
typedef struct CANetworkCallback_t
{

    /** Linked list; for multiple callback list.*/
    struct CANetworkCallback_t *next;

    /** Adapter state changed event callback. */
    // GAR FIXME: change name to indicate its an event handler
    void (*adapter)(CATransportAdapter_t adapter, bool enabled);
    // CAAdapterStateChangedCB adapter;

    /** Connection state changed event callback. */
    // @rewrite CAConnectionStateChangedCB conn;
    void (*conn)(const CAEndpoint_t *info, bool isConnected);


} CANetworkCallback_t;

#ifndef SINGLE_THREAD
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
#endif // SINGLE_THREAD

static CAResult_t CAGetAdapterIndex(CATransportAdapter_t cType, size_t *adapterIndex)
{
    for (size_t index = 0 ; index < g_numberOfAdapters ; index++)
    {
        if (cType == g_adapterHandler[index].cType )
        {
            *adapterIndex = index;
            return CA_STATUS_OK;
        }
    }
    return CA_STATUS_FAILED;
}

// @rewrite: eliminate CARegisterCallback
//static
void CARegisterCallback(CAConnectivityHandler_t handler)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, transport adapter %s (%d)", __func__,
	      CA_TRANSPORT_ADAPTER_STRING(handler.cType), handler.cType);

    if (handler.startAdapter == NULL ||
        handler.startListenServer == NULL ||
        handler.stopListenServer == NULL ||
        handler.startDiscoveryServer == NULL ||
        handler.unicast == NULL ||
        handler.multicast == NULL ||
        handler.GetNetInfo == NULL ||
        handler.readData == NULL ||
        handler.stopAdapter == NULL ||
        handler.terminate == NULL)
    {
        OIC_LOG(ERROR, TAG, "connectivity handler is not enough to be used!");
        return;
    }
    size_t numberofAdapters = g_numberOfAdapters + 1;
    CAConnectivityHandler_t *adapterHandler = OICRealloc(g_adapterHandler,
            (numberofAdapters) * sizeof(*adapterHandler));
    if (NULL == adapterHandler)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed during registration");
        return;
    }
    g_adapterHandler = adapterHandler;
    g_numberOfAdapters = numberofAdapters;
    g_adapterHandler[g_numberOfAdapters - 1] = handler;

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

/**
 * Add a network callback from caller to the network callback list
 *
 * @param adapterCB  adapter state changed callback
 * @param connCB     connection state changed callback
 *
 * @return
 *     CAResult_t
 */
// only called once?
CAResult_t AddNetworkStateChangedCallback(void (*adapterCB)(CATransportAdapter_t adapter, bool enabled),
					  // CAAdapterStateChangedCB adapterCB,
					  CAConnectionStateChangedCB connCB)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    if (!adapterCB)
    {
        OIC_LOG(ERROR, TAG, "adapterCB is null");
        return CA_STATUS_INVALID_PARAM;
    }

#ifdef STATEFUL_PROTOCOL_SUPPORTED
    if (!connCB)
    {
        OIC_LOG(ERROR, TAG, "connCB is null");
        return CA_STATUS_INVALID_PARAM;
    }
#endif

    CANetworkCallback_t *callback = NULL;
    LL_FOREACH(g_networkChangeCallbackList, callback)
    {
        if (callback && adapterCB == callback->adapter && connCB == callback->conn)
        {
            OIC_LOG(DEBUG, TAG, "this callback is already added");
            return CA_STATUS_OK;
        }
    }

    callback = (CANetworkCallback_t *) OICCalloc(1, sizeof(CANetworkCallback_t));
    if (NULL == callback)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed during registration");
        return CA_MEMORY_ALLOC_FAILED;
    }

    callback->adapter = adapterCB;
#ifdef STATEFUL_PROTOCOL_SUPPORTED
    // Since IP adapter(UDP) is the Connectionless Protocol, it doesn't need.
    callback->conn = connCB;
#endif
    LL_APPEND(g_networkChangeCallbackList, callback);
    OIC_LOG_V(DEBUG, TAG, "Appended CB to g_networkChangeCallbackList");

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

/**
 * Remove a network callback from the network callback list
 *
 * @param adapterCB  adapter state changed callback
 * @param connCB     connection state changed callback
 *
 * @return
 *     CAResult_t
 */
CAResult_t RemoveNetworkStateChangedCallback(void(*adapterCB)(CATransportAdapter_t adapter,
							      bool enabled),
					     // CAAdapterStateChangedCB adapterCB,
					     void (*connCB)(const CAEndpoint_t *info,
							    bool isConnected)
					     // CAConnectionStateChangedCB connCB
							    )
{
    OIC_LOG(DEBUG, TAG, "Remove NetworkStateChanged Callback");

    CANetworkCallback_t *callback = NULL;
    LL_FOREACH(g_networkChangeCallbackList, callback)
    {
        if (callback && adapterCB == callback->adapter && connCB == callback->conn)
        {
            OIC_LOG(DEBUG, TAG, "remove specific callback");
            LL_DELETE(g_networkChangeCallbackList, callback);
            OICFree(callback);
            callback = NULL;
            return CA_STATUS_OK;
        }
    }
    return CA_STATUS_OK;
}

/**
 * Remove all network callback from the network callback list
 */
static void RemoveAllNetworkStateChangedCallback()
{
    OIC_LOG(DEBUG, TAG, "Remove All NetworkStateChanged Callback");

    CANetworkCallback_t *callback = NULL;
    CANetworkCallback_t *tmp = NULL;
    LL_FOREACH_SAFE(g_networkChangeCallbackList, callback, tmp)
    {
        LL_DELETE(g_networkChangeCallbackList, callback);
        OICFree(callback);
        callback = NULL;
    }
    g_networkChangeCallbackList = NULL;
}

#ifdef RA_ADAPTER
CAResult_t CASetAdapterRAInfo(const CARAInfo_t *caraInfo)
{
    return CASetRAInfo(caraInfo);
}
#endif

/* signature: CANetworkPacketReceivedCallback */
// FIXME: for consistency, should be named CAAdapterReceivedPacketCallback
// @rewrite: instead of passing this to CAInitializeUDP, we just call
// @rewrite: it from there directly, so we remove static.
// @rewrite: ifc_CAReceivedPacketCallback @was CAReceivedPacketCallback
// static
// @rewrite remove void ifc_CAReceivedPacketCallback(const CASecureEndpoint_t *sep,
/* 			      const void *data, size_t dataLen) */
/* { */
/*     if (g_networkPacketReceivedCallback != NULL) */
/*     { */
/*         g_networkPacketReceivedCallback(sep, data, dataLen); */
/*     } */
/*     else */
/*     { */
/*         OIC_LOG(ERROR, TAG, "network packet received callback is NULL!"); */
/*     } */
/* } */

// @rewrite CAAdapterChangedCallback called only by UDP (CAIPAdapterHandler)
// @rewrite We don't need this routine, since we call OCDefaultAdapterStateChangedHandler
// @rewrite  directly from udp_if_change_handler,
// static
void CAAdapterChangedCallback(CATransportAdapter_t adapter, CANetworkStatus_t status)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OIC_LOG_V(DEBUG, TAG, "[%d] adapter state is changed to [%d]", adapter, status);

    // Call the callback.
    CANetworkCallback_t *callback  = NULL;
    LL_FOREACH(g_networkChangeCallbackList, callback)
    {
	// @rewrite callback->adapter is a chg event handler
        if (callback && callback->adapter)
        {
#ifndef SINGLE_THREAD
            CANetworkCallbackThreadInfo_t *info = (CANetworkCallbackThreadInfo_t *)
                                        OICCalloc(1, sizeof(CANetworkCallbackThreadInfo_t));
            if (!info)
            {
                OIC_LOG(ERROR, TAG, "OICCalloc to info failed!");
                return;
            }

	    // the CB is OCDefaultAdapterStateChangedHandler
            info->adapterCB = callback->adapter; /* change event handler */
            info->adapter = adapter;
            info->isInterfaceUp = (CA_INTERFACE_UP == status);

            CAQueueingThreadAddData(&g_networkChangeCallbackThread, info,
                                    sizeof(CANetworkCallbackThreadInfo_t));
#else
            if (CA_INTERFACE_UP == status)
            {
                callback->adapter(adapter, true); /* call chg event handler */
            }
            else if (CA_INTERFACE_DOWN == status)
            {
                callback->adapter(adapter, false);
            }
#endif //SINGLE_THREAD
        }
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

#ifdef STATEFUL_PROTOCOL_SUPPORTED
void CAConnectionChangedCallback(const CAEndpoint_t *endpoint, bool isConnected)
{
    OIC_LOG_V(DEBUG, TAG, "[%s] connection state is changed to [%d]", endpoint->addr, isConnected);

    // Call the callback.
    CANetworkCallback_t *callback = NULL;
    LL_FOREACH(g_networkChangeCallbackList, callback)
    {
        if (callback && callback->conn)
        {
#ifndef SINGLE_THREAD
            CANetworkCallbackThreadInfo_t *info = (CANetworkCallbackThreadInfo_t *)
                                        OICCalloc(1, sizeof(CANetworkCallbackThreadInfo_t));
            if (!info)
            {
                OIC_LOG(ERROR, TAG, "OICCalloc to info failed!");
                return;
            }

            CAEndpoint_t *cloneEp = CACloneEndpoint(endpoint);
            if (!cloneEp)
            {
                OIC_LOG(ERROR, TAG, "CACloneEndpoint failed!");
                OICFree(info);
                return;
            }

            info->connectionCB = callback->conn;
            info->endpoint = cloneEp;
            info->isConnected = isConnected;

            CAQueueingThreadAddData(&g_networkChangeCallbackThread, info,
                                    sizeof(CANetworkCallbackThreadInfo_t));
#else
            callback->conn(endpoint, isConnected);
#endif //SINGLE_THREAD
        }
    }
}
#endif //STATEFUL_PROTOCOL_SUPPORTED

// static
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
#ifdef IP_ADAPTER
    OIC_LOG_V(DEBUG, TAG, "UDP is enabled");
    if ((transportType & CA_ADAPTER_IP) || (CA_DEFAULT_ADAPTER == transportType)
        || (transportType == CA_ALL_ADAPTERS))
    {
        CAInitializeUDP(/* CARegisterCallback, */
		       /* ifc_CAReceivedPacketCallback, */
		       /* CAAdapterChangedCallback, */
                       /* CAAdapterErrorHandleCallback, */
		       thread_pool);
    }
#endif /* IP_ADAPTER */

#ifdef EDR_ADAPTER
    if ((transportType & CA_ADAPTER_RFCOMM_BTEDR) || (CA_DEFAULT_ADAPTER == transportType))
    {
        CAInitializeEDR(/* CARegisterCallback, */
			ifc_CAReceivedPacketCallback, CAAdapterChangedCallback,
                        CAConnectionChangedCallback, CAAdapterErrorHandleCallback, thread_pool);
    }
#endif /* EDR_ADAPTER */

#ifdef LE_ADAPTER
    if ((transportType & CA_ADAPTER_GATT_BTLE) || (CA_DEFAULT_ADAPTER == transportType))
    {
        CAInitializeLE(CARegisterCallback, ifc_CAReceivedPacketCallback, CAAdapterChangedCallback,
                       CAConnectionChangedCallback, CAAdapterErrorHandleCallback, thread_pool);
    }
#endif /* LE_ADAPTER */

#ifdef RA_ADAPTER
    if ((transportType & CA_ADAPTER_REMOTE_ACCESS) || (CA_DEFAULT_ADAPTER == transportType))
    {
        CAInitializeRA(CARegisterCallback, ifc_CAReceivedPacketCallback, CAAdapterChangedCallback,
                       thread_pool);
    }
#endif /* RA_ADAPTER */

#ifdef TCP_ADAPTER
    OIC_LOG_V(DEBUG, TAG, "TCP is enabled");
    if ((transportType & CA_ADAPTER_TCP) || (CA_DEFAULT_ADAPTER == transportType))
    {
        CAInitializeTCP(// CARegisterCallback,
			//ifc_CAReceivedPacketCallback,
			//CAAdapterChangedCallback,
                        // CAConnectionChangedCallback,
			//CAAdapterErrorHandleCallback,
			thread_pool);
    }
#endif /* TCP_ADAPTER */

#ifdef NFC_ADAPTER
    if ((transportType & CA_ADAPTER_NFC) || (CA_DEFAULT_ADAPTER == transportType))
    {
        CAInitializeNFC(CARegisterCallback, ifc_CAReceivedPacketCallback, CAAdapterChangedCallback,
                        CAAdapterErrorHandleCallback, thread_pool);
    }
#endif /* NFC_ADAPTER */

#ifndef SINGLE_THREAD
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
#endif //SINGLE_THREAD

    return CA_STATUS_OK;
}

// @rewrite remove CASetPacketReceivedCallback
/* void CASetPacketReceivedCallback(void (*callback)(const CASecureEndpoint_t *sep, */
/* 						  const void *data, size_t dataLen)) */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); */
/*     g_networkPacketReceivedCallback = callback; */
/* } */

void CASetErrorHandleCallback(CAErrorHandleCallback errorCallback)
{
    OIC_LOG(DEBUG, TAG, "Set error handle callback");
    g_errorHandleCallback = errorCallback;
}

// called by CAAddNetworkType(CATransportAdapter_t transportType)
CAResult_t CAStartAdapter(CATransportAdapter_t transportType)
{
    /* size_t index = 0; */
    /* CAResult_t res = CA_STATUS_FAILED; */

    OIC_LOG_V(DEBUG, TAG, "%s ENTRY: transport adapter: %s (%d)", __func__,
	      CA_TRANSPORT_ADAPTER_STRING(transportType), transportType);

    // @rewrite: call start routines directly instead of looking them up
#ifdef IP_ADAPTER
    if (transportType == CA_ADAPTER_IP) {
	CAStartUDP();
	return CA_STATUS_OK;
    }
#endif

#ifdef TCP_ADAPTER
    if (transportType == CA_ADAPTER_TCP) {
	CAStartTCP();
	return CA_STATUS_OK;
    }
#endif

    return CA_STATUS_FAILED;


    /* res = CAGetAdapterIndex(transportType, &index); */
    /* if (CA_STATUS_OK != res) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "unknown connectivity type!"); */
    /*     return CA_STATUS_FAILED; */
    /* } */

    /* if (g_adapterHandler[index].startAdapter != NULL) */
    /* { */
    /*     res = g_adapterHandler[index].startAdapter(); */
    /* } */

    /* return res; */
}

void CAStopAdapter(CATransportAdapter_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, transport adapter %s (%d)", __func__,
	      CA_TRANSPORT_ADAPTER_STRING(transportType), transportType);

    if (transportType == CA_ADAPTER_IP) {
#ifdef IP_ADAPTER
	CAStopIP();
	return;
#endif
    }

    if (transportType == CA_ADAPTER_TCP) {
#ifdef TCP_ADAPTER
	CAStopTCP();
	return;
#endif
    }

    /* size_t index = 0; */
    /* CAResult_t res = CA_STATUS_FAILED; */

    /* res = CAGetAdapterIndex(transportType, &index); */
    /* if (CA_STATUS_OK != res) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "unknown transport type!"); */
    /*     return; */
    /* } */

    /* if (g_adapterHandler[index].stopAdapter != NULL) */
    /* { */
    /*     g_adapterHandler[index].stopAdapter(); */
    /* } */
}

#ifndef SINGLE_THREAD
void CAStopAdapters()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    // @rewrite
/* #ifdef ENABLE_UDP */
/*     CAStopAdapter(CA_ADAPTER_IP); */
/* #endif */
/* #ifdef ENABLE_TCP */
/*     CAStopAdapter(CA_ADAPTER_TCP); */
/* #endif */

    CATransportAdapter_t connType;
    u_arraylist_t *list = CAGetSelectedNetworkList();
    size_t length = u_arraylist_length(list);

    for (size_t i = 0; i < length; i++)
    {
        void* ptrType = u_arraylist_get(list, i);

        if (NULL == ptrType)
        {
            continue;
        }

        connType = *(CATransportAdapter_t *)ptrType;
        CAStopAdapter(connType);
    }

    CAQueueingThreadStop(&g_networkChangeCallbackThread);
}
#endif // not SINGLE_THREAD

CAResult_t CAGetNetworkInfo(CAEndpoint_t **info, size_t *size)
{
    VERIFY_NON_NULL_MSG(info, TAG, "info is null");
    VERIFY_NON_NULL_MSG(size, TAG, "size is null");


    CAEndpoint_t **tempInfo = (CAEndpoint_t **) OICCalloc(g_numberOfAdapters, sizeof(*tempInfo));
    if (!tempInfo)
    {
        OIC_LOG(ERROR, TAG, "Out of memory!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    size_t *tempSize = (size_t *)OICCalloc(g_numberOfAdapters, sizeof(*tempSize));
    if (!tempSize)
    {
        OIC_LOG(ERROR, TAG, "Out of memory!");
        OICFree(tempInfo);
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAResult_t res = CA_STATUS_FAILED;
    size_t resSize = 0;
    for (size_t index = 0; index < g_numberOfAdapters; index++)
    {
        if (g_adapterHandler[index].GetNetInfo != NULL)
        {
            // #1. get information for each adapter
            res = g_adapterHandler[index].GetNetInfo(&tempInfo[index],
                    &tempSize[index]);

            // #2. total size
            if (res == CA_STATUS_OK)
            {
                resSize += tempSize[index];
            }

            OIC_LOG_V(DEBUG,
                      TAG,
                      "%" PRIuPTR " adapter network info size is %" PRIuPTR " res:%u",
                      index,
                      tempSize[index],
                      res);
        }
    }

    OIC_LOG_V(DEBUG, TAG, "network info total size is %" PRIuPTR "!", resSize);

    if (resSize == 0)
    {
        OICFree(tempInfo);
        OICFree(tempSize);
        return res;
    }

    // #3. add data into result
    // memory allocation
    CAEndpoint_t *resInfo = (CAEndpoint_t *) OICCalloc(resSize, sizeof (*resInfo));
    CA_MEMORY_ALLOC_CHECK(resInfo);

    // #4. save data
    *info = resInfo;
    *size = resSize;

    for (size_t index = 0; index < g_numberOfAdapters; index++)
    {
        // check information
        if (tempSize[index] == 0)
        {
            continue;
        }

        memcpy(resInfo,
               tempInfo[index],
               sizeof(*resInfo) * tempSize[index]);

        resInfo += tempSize[index];

        // free adapter data
        OICFree(tempInfo[index]);
        tempInfo[index] = NULL;
    }
    OICFree(tempInfo);
    OICFree(tempSize);

    OIC_LOG(DEBUG, TAG, "each network info save success!");
    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    for (size_t index = 0; index < g_numberOfAdapters; index++)
    {
        OICFree(tempInfo[index]);
        tempInfo[index] = NULL;
    }
    OICFree(tempInfo);
    OICFree(tempSize);

    return CA_MEMORY_ALLOC_FAILED;
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

    // @rewrite
    int32_t sentDataLen = 0;

    OIC_LOG(DEBUG, TAG, "unicast message to adapter");
#ifdef IP_ADAPTER
    sentDataLen = CASendIPUnicastData(endpoint, data, length, dataType);
#endif
#ifdef TCP_ADAPTER
    sentDataLen = CASendTCPUnicastData(endpoint, data, length, dataType);
#endif

    if ((0 > sentDataLen) || ((uint32_t)sentDataLen != length)) {
	OIC_LOG(ERROR, TAG, "Error sending data. The error will be reported in adapter.");
#ifdef SINGLE_THREAD
	//in case of single thread, no error handler. Report error immediately
	return CA_SEND_FAILED;
#endif
    }

/*     size_t index = 0; */
/*     CAResult_t res = CA_STATUS_FAILED; */

/*     VERIFY_NON_NULL_MSG(endpoint, TAG, "endpoint is null"); */

/*     // GAR search list of supported transports */
/*     u_arraylist_t *list = CAGetSelectedNetworkList(); */
/*     if (!list) */
/*     { */
/*         OIC_LOG(ERROR, TAG, "No selected network"); */
/*         return CA_SEND_FAILED; */
/*     } */
/*     CATransportAdapter_t requestedAdapter = endpoint->adapter ? endpoint->adapter : CA_ALL_ADAPTERS; */

/*     // GAR list contains uint32_t */
/*     for (size_t i = 0; i < u_arraylist_length(list); i++) */
/*     { */
/*         void *ptrType = u_arraylist_get(list, i); */

/*         if (NULL == ptrType) */
/*         { */
/*             continue; */
/*         } */

/*         CATransportAdapter_t connType = *(CATransportAdapter_t *)ptrType; */
/*         if (0 == (connType & requestedAdapter)) */
/*         { */
/*             continue; */
/*         } */

/* 	// GAR index into g_adapterHandler array */
/*         res = CAGetAdapterIndex(connType, &index); */
/*         if (CA_STATUS_OK != res) */
/*         { */
/*             OIC_LOG(ERROR, TAG, "unknown transport type!"); */
/*             return CA_STATUS_INVALID_PARAM; */
/*         } */

/*         int32_t sentDataLen = 0; */

/*         if (NULL != g_adapterHandler[index].unicast) */
/*         { */
/*             OIC_LOG(DEBUG, TAG, "unicast message to adapter"); */
/*             sentDataLen = g_adapterHandler[index].unicast(endpoint, data, length, dataType); */
/*         } */

/*         if ((0 > sentDataLen) || ((uint32_t)sentDataLen != length)) */
/*         { */
/*             OIC_LOG(ERROR, TAG, "Error sending data. The error will be reported in adapter."); */
/* #ifdef SINGLE_THREAD */
/*             //in case of single thread, no error handler. Report error immediately */
/*             return CA_SEND_FAILED; */
/* #endif */
/*         } */

    /* } */

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
#ifdef IP_ADAPTER
    sentDataLen = CASendIPMulticastData(endpoint, payload, length, dataType);
#endif

#ifdef TCP_ADAPTER
    sentDataLen = CASendTCPMulticastData(endpoint, payload, length, dataType);
#endif

    OICFree(payload);

    if (sentDataLen != length) {
	OIC_LOG(ERROR, TAG, "multicast failed! Error will be reported from adapter");
#ifdef SINGLE_THREAD
	//in case of single thread, no error handler. Report error immediately
	return CA_SEND_FAILED;
#endif
    }

/*     size_t index = 0; */
/*     CAResult_t res = CA_STATUS_FAILED; */

/*     VERIFY_NON_NULL_MSG(endpoint, TAG, "endpoint is null"); */

/*     u_arraylist_t *list = CAGetSelectedNetworkList(); */
/*     if (!list) */
/*     { */
/*         OIC_LOG(DEBUG, TAG, "No selected network"); */
/*         return CA_SEND_FAILED; */
/*     } */

/*     CATransportAdapter_t requestedAdapter = endpoint->adapter ? endpoint->adapter : CA_ALL_ADAPTERS; */
/*     size_t selectedLength = u_arraylist_length(list); */
/*     for (size_t i = 0; i < selectedLength; i++) */
/*     { */
/*         void *ptrType = u_arraylist_get(list, i); */

/*         if (NULL == ptrType) */
/*         { */
/*             continue; */
/*         } */

/*         CATransportAdapter_t connType = *(CATransportAdapter_t *)ptrType; */
/*         if (0 == (connType & requestedAdapter)) */
/*         { */
/*             continue; */
/*         } */

/*         res = CAGetAdapterIndex(connType, &index); */
/*         if (CA_STATUS_OK != res) */
/*         { */
/*             OIC_LOG(ERROR, TAG, "unknown connectivity type!"); */
/*             continue; */
/*         } */

/*         uint32_t sentDataLen = 0; */

/*         if (NULL != g_adapterHandler[index].multicast) */
/*         { */
/*             void *payload = (void *) OICMalloc(length); */
/*             if (!payload) */
/*             { */
/*                 OIC_LOG(ERROR, TAG, "Out of memory!"); */
/*                 return CA_MEMORY_ALLOC_FAILED; */
/*             } */
/*             memcpy(payload, data, length); */
/*             sentDataLen = g_adapterHandler[index].multicast(endpoint, payload, length, dataType); */
/*             OICFree(payload); */
/*         } */

/*         if (sentDataLen != length) */
/*         { */
/*             OIC_LOG(ERROR, TAG, "multicast failed! Error will be reported from adapter"); */
/* #ifdef SINGLE_THREAD */
/*             //in case of single thread, no error handler. Report error immediately */
/*             return CA_SEND_FAILED; */
/* #endif */
/*         } */
/*     } */

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAStartListeningServerAdapters()
{
    // @rewrite:
    CAResult_t result = CA_STATUS_FAILED;

#ifdef IP_ADAPTER
    result = udp_add_ifs_to_multicast_groups(); // @was CAIPStartListenServer();
#endif

#ifdef TCP_ADAPTER
    result = CAStartTCPListeningServer();
#endif

    return result;


    /* size_t index = 0; */
    /* CAResult_t result = CA_STATUS_FAILED; */

    /* u_arraylist_t *list = CAGetSelectedNetworkList(); */
    /* if (!list) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "No selected network"); */
    /*     return result; */
    /* } */

    /* size_t length = u_arraylist_length(list); */
    /* for (size_t i = 0; i < length; i++) */
    /* { */
    /*     void *ptrType = u_arraylist_get(list, i); */

    /*     if (ptrType == NULL) */
    /*     { */
    /*         continue; */
    /*     } */

    /*     CATransportAdapter_t connType = *(CATransportAdapter_t *)ptrType; */

    /*     if (CA_STATUS_OK != CAGetAdapterIndex(connType, &index)) */
    /*     { */
    /*         OIC_LOG(ERROR, TAG, "unknown connectivity type!"); */
    /*         continue; */
    /*     } */

    /*     if (g_adapterHandler[index].startListenServer != NULL) */
    /*     { */
    /*         const CAResult_t tmp = */
    /*             g_adapterHandler[index].startListenServer(); */

    /*         // Successful listen if at least one adapter started. */
    /*         if (CA_STATUS_OK == tmp) */
    /*         { */
    /*             result = tmp; */
    /*         } */
    /*     } */
    /* } */

    return result;
}

CAResult_t CAStopListeningServerAdapters()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    // @rewrite: call routines directly, without lookup table

#ifdef IP_ADAPTER
    udp_close_sockets();
#endif

#ifdef TCP_ADAPTER
    CAStopTCPListeningServer();	/* NB: doesn't actually do anything */
#endif

    /* size_t index = 0; */
    /* CAResult_t res = CA_STATUS_FAILED; */
    /* u_arraylist_t *list = CAGetSelectedNetworkList(); */
    /* if (!list) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "No selected network"); */
    /*     return CA_STATUS_FAILED; */
    /* } */

    /* size_t length = u_arraylist_length(list); */
    /* for (size_t i = 0; i < length; i++) */
    /* { */
    /*     void *ptrType = u_arraylist_get(list, i); */
    /*     if (ptrType == NULL) */
    /*     { */
    /*         continue; */
    /*     } */

    /*     CATransportAdapter_t connType = *(CATransportAdapter_t *)ptrType; */

    /*     res = CAGetAdapterIndex(connType, &index); */
    /*     if (CA_STATUS_OK != res) */
    /*     { */
    /*         OIC_LOG(ERROR, TAG, "unknown connectivity type!"); */
    /*         continue; */
    /*     } */

    /*     if (g_adapterHandler[index].stopListenServer != NULL) */
    /*     { */
    /*         g_adapterHandler[index].stopListenServer(); */
    /*     } */
    /* } */

    return CA_STATUS_OK;
}

CAResult_t CAStartDiscoveryServerAdapters()
{
    /* NOTE: startdiscover etc. just calls CAIPStartListenServer,
       which adds ifs to multicast groups */
    // @rewrite:
    CAResult_t result = CA_STATUS_FAILED;
#ifdef IP_ADAPTER
    result = udp_add_ifs_to_multicast_groups(); // @was CAIPStartListenServer;
#endif
#ifdef TCP_ADAPTER
    result = CAStartTCPDiscoveryServer();
#endif
    return result;

    /* size_t index = 0; */
    /* CAResult_t result = CA_STATUS_FAILED; */

    /* u_arraylist_t *list = CAGetSelectedNetworkList(); */

    /* if (!list) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "No selected network"); */
    /*     return result; */
    /* } */

    /* size_t length = u_arraylist_length(list); */
    /* for (size_t i = 0; i < length; i++) */
    /* { */
    /*     void *ptrType = u_arraylist_get(list, i); */

    /*     if (ptrType == NULL) */
    /*     { */
    /*         continue; */
    /*     } */

    /*     CATransportAdapter_t connType = *(CATransportAdapter_t *)ptrType; */

    /*     if (CA_STATUS_OK != CAGetAdapterIndex(connType, &index)) */
    /*     { */
    /*         OIC_LOG(DEBUG, TAG, "unknown connectivity type!"); */
    /*         continue; */
    /*     } */

    /*     if (g_adapterHandler[index].startDiscoveryServer != NULL) */
    /*     { */
    /* 	    // for udp this is always CAStartIPDiscoveryServer, which */
    /* 	    // runs CAStartIPListeningServer, which calls */
    /* 	    // CAIPStartListenServer, which just adds sockets to */
    /* 	    // multicast groups */
    /*         const CAResult_t tmp = */
    /*             g_adapterHandler[index].startDiscoveryServer(); */

    /*         // Successful discovery if at least one adapter started. */
    /*         if (CA_STATUS_OK == tmp) */
    /*         { */
    /*             result = tmp; */
    /*         } */
    /*     } */
    /* } */

    /* return result; */
}

bool CAIsLocalEndpoint(const CAEndpoint_t *ep)
{
    VERIFY_NON_NULL_RET(ep, TAG, "ep is null", false);

#ifdef IP_ADAPTER
    if (ep->adapter & CA_ADAPTER_IP)
    {
        return CAIPIsLocalEndpoint(ep);
    }
#endif /* IP_ADAPTER */

    //TODO: implement for the other adapters(EDR/LE/NFC)
    return false;
}

void CATerminateAdapters()
{
    for (size_t index = 0; index < g_numberOfAdapters; index++)
    {
        if (g_adapterHandler[index].terminate != NULL)
        {
            g_adapterHandler[index].terminate();
        }
    }

    OICFree(g_adapterHandler);
    g_adapterHandler = NULL;
    g_numberOfAdapters = 0;

#ifndef SINGLE_THREAD
    CAQueueingThreadDestroy(&g_networkChangeCallbackThread);
#endif //SINGLE_THREAD

    RemoveAllNetworkStateChangedCallback();
}
