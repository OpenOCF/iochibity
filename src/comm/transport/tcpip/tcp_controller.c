/* ****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include "tcp_controller.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif


#if INTERFACE
typedef enum CAProtocol
{
    UNKNOWN = 0,
    TLS,
    COAP
} CAProtocol_t;
#endif

/**
 * Holds internal thread TCP data information.
 */
#if INTERFACE
/* src: catcpadapter.c */
typedef struct
{
    CAEndpoint_t *remoteEndpoint;
    void *data;
    size_t dataLen;
    bool isMulticast;
    bool encryptedData;
} CATCPData;
#endif

void *tcp_threadpool;           /* replaces caglobals.tcp.threadpool */

// NB: we use fn ptr prototypes rather than typedefs

/**
 * This will be used to notify network changes to the connectivity common logic layer.
 */
//typedef void (*CAAdapterChangeCallback)(CATransportAdapter_t adapter, CANetworkStatus_t status);

/**
 * Adapter Changed Callback to CA.
 */
// static CAAdapterChangeCallback g_networkChangeCallback = NULL;
/* void (*tcp_networkChangeCallback)(CATransportAdapter_t adapter, CANetworkStatus_t status) = NULL; */

/**
 * This will be used to notify connection changes to the connectivity common logic layer.
 */
/* src: caadapterinterface.h */
/* typedef void (*CAConnectionChangeCallback)(const CAEndpoint_t *info, bool isConnected); */
void (*tcp_connectionChangeCallback)(const CAEndpoint_t *info, bool isConnected);

/**
 * Connection Changed Callback to CA.
 */
/**
// static CAConnectionChangeCallback g_connectionChangeCallback = NULL;

/**
 * This will be used to notify error result to the connectivity common logic layer.
 */
/* typedef void (*CAErrorHandleCallback)(const CAEndpoint_t *endpoint, */
/*                                       const void *data, size_t dataLen, */
/*                                       CAResult_t result); */
void (*tcp_errorCallback)(const CAEndpoint_t *endpoint,
			  const void *data, size_t dataLen,
			  CAResult_t result);
/**
 * error Callback to CA adapter.
 */
//CAErrorHandleCallback tcp_errorCallback; // = NULL;

/* typedef void (*CAKeepAliveConnectionCallback)(const CAEndpoint_t *object, bool isConnected, */
/*                                               bool isClient); */
void (*tcp_connKeepAliveCallback)(const CAEndpoint_t *object,
				  bool isConnected,
				  bool isClient);

/**
 * KeepAlive Connected or Disconnected Callback to CA adapter.
 */
// CAKeepAliveConnectionCallback g_connKeepAliveCallback; // = NULL;

/**
 * Mutex to synchronize device object list.
 */
/* src: catcpserver.c */
oc_mutex tcp_mutexObjectList = NULL;

static CAResult_t CATCPCreateMutex()
{
    if (!tcp_mutexObjectList)
    {
        tcp_mutexObjectList =  oc_mutex_new_recursive();
        if (!tcp_mutexObjectList)
        {
            OIC_LOG(ERROR, TAG, "Failed to created mutex!");
            return CA_STATUS_FAILED;
        }
    }

    return CA_STATUS_OK;
}

static void CATCPDestroyMutex()
{
    if (tcp_mutexObjectList)
    {
        oc_mutex_free(tcp_mutexObjectList);
        tcp_mutexObjectList = NULL;
    }
}

/**
 * Conditional mutex to synchronize.
 */
oc_cond tcp_condObjectList = NULL;

static CAResult_t CATCPCreateCond()
{
    if (!tcp_condObjectList)
    {
        tcp_condObjectList = oc_cond_new();
        if (!tcp_condObjectList)
        {
            OIC_LOG(ERROR, TAG, "Failed to created cond!");
            return CA_STATUS_FAILED;
        }
    }
    return CA_STATUS_OK;
}

static void CATCPDestroyCond()
{
    if (tcp_condObjectList)
    {
        oc_cond_free(tcp_condObjectList);
        tcp_condObjectList = NULL;
    }
}

CAResult_t CAStopTCP()
{
    CAIPStopNetworkMonitor(CA_ADAPTER_TCP);

    /* Some times send queue thread fails to terminate as it's worker
       thread gets blocked at TCP session's socket connect operation.
       So closing sockets which are in connect operation at the time
       of termination of adapter would save send queue thread from
       getting blocked. */
    CATCPCloseInProgressConnections();

    if (tcp_sendQueueHandle && tcp_sendQueueHandle->threadMutex)
    {
        CAQueueingThreadStop(tcp_sendQueueHandle);
    }
    CATCPDeinitializeQueueHandles();

    CATCPStopServer();

    //Re-initializing the Globals to start them again
    // FIXME: CAInitializeTCPGlobals();

#ifdef __WITH_TLS__
    CAdeinitSslAdapter();
#endif

    return CA_STATUS_OK;
}

void CATerminateTCP()
{
#ifdef __WITH_TLS__
    CAsetSslAdapterCallbacks(// NULL,
			     NULL,
			     //NULL,
			     CA_ADAPTER_TCP);
#endif
    CAStopTCP();
    CATCPSetPacketReceiveCallback(NULL);

#ifdef WSA_WAIT_EVENT_0
    // Windows-specific clean-up.
    OC_VERIFY(0 == WSACleanup());
#endif
}

CAResult_t CAInitializeTCP(// CARegisterConnectivityCallback registerCallback,
                           //CANetworkPacketReceivedCallback networkPacketCallback,
                           //CAAdapterChangeCallback netCallback,
                           //CAConnectionChangeCallback connCallback,
                           //CAErrorHandleCallback errorCallback,
			   ca_thread_pool_t handle)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    /* VERIFY_NON_NULL_MSG(registerCallback, TAG, "registerCallback"); */
    /* VERIFY_NON_NULL_MSG(networkPacketCallback, TAG, "networkPacketCallback"); */
    /* VERIFY_NON_NULL_MSG(netCallback, TAG, "netCallback"); */

    VERIFY_NON_NULL_MSG(handle, TAG, "thread pool handle");

    // FIXME: call windows init code

    //tcp_networkChangeCallback = CAAdapterChangedCallback;
    // @was: g_networkChangeCallback = netCallback;

    // FIXME: eliminate tcp_connectionChangeCallback
    tcp_connectionChangeCallback = oocf_enqueue_tcp_conn_ch_work_pkg;
    // @was: g_connectionChangeCallback = connCallback;

    // tcp_networkPacketCallback = g_networkPacketReceivedCallback;
    // @was: g_networkPacketCallback = networkPacketCallback;

    // FIXME: eliminate tcp_errorCallback
    tcp_errorCallback = CAAdapterErrorHandleCallback;           // errorCallback;

    // g_errorCallback = errorCallback;

    // initialization is now static - CAInitializeTCPGlobals();
    tcp_threadpool = handle;

    // DELETED: fwding fn CATCPSetConnectionChangedCallback(CATCPConnectionHandler);
    CATCPSetPacketReceiveCallback(CATCPPacketReceivedCB);
    // @rewrite: replaced by static init: CATCPSetErrorHandler(CATCPErrorHandler);

#ifdef __WITH_TLS__
    if (CA_STATUS_OK != CAinitSslAdapter())
    {
        OIC_LOG(ERROR, TAG, "Failed to init SSL adapter");
    }
    else
    {
        CAsetSslAdapterCallbacks(//CATCPPacketReceivedCB,
				 CATCPPacketSendCB,
				 //CATCPErrorHandler,
				 CA_ADAPTER_TCP);
    }
#endif

    /* These routines are originallcalled from cainterfacecontroller,
or tcp_data_sender.c where they are accessed via table lookup. that
has been replaced by direct calls, so this can go away */

    //CAConnectivityHandler_t tcpHandler = {
        // .startAdapter = &CAStartTCP,  // tcp_controller.c
        // .stopAdapter = &CAStopTCP,    // tcp_controller.c
        // .startListenServer = &CAStartTCPListeningServer, // tcp_controller.c
        // .stopListenServer = &CAStopTCPListeningServer,    // tcp_controller.c
        // .startDiscoveryServer = &CAStartTCPDiscoveryServer, // tcp_controller.c
        // .unicast = &CASendTCPUnicastData,   // tcp_data_sender.c
        // .multicast = &CASendTCPMulticastData,  // tcp_data_sender.c
        // .GetNetInfo = &CAGetTCPInterfaceInformation, // tcp_sockets.c,  tcp_get_local_endpoints
        // .readData = &CAReadTCPData,   // NOP - not used
        // .terminate = &CATerminateTCP,  // tcp_controller.c
        // .cType = CA_ADAPTER_TCP
	//};

    //registerCallback(tcpHandler);
    //CARegisterCallback(tcpHandler);

    OIC_LOG(INFO, TAG, "OUT IntializeTCP is Success");
    return CA_STATUS_OK;
}

CAResult_t CAStartTCP()
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);

    // Start network monitoring to receive adapter status changes.

    // original code: CAIPStartNetworkMonitor, which calls
    // CAIPInitializeNetworkMonitorList, then initializes nw monitor
    // cbs. those are now statically initialized, so all we need do is
    // create the "nw monitor" (i.e. interface) list
    // FIXME: the if chg handler can be statically initialized, no need to pass it!
    ip_create_network_interface_list(tcp_interface_change_handler, CA_ADAPTER_TCP);

    if (CA_STATUS_OK != CATCPInitializeQueueHandles())
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize Queue Handle");
        CATerminateTCP();
        return CA_STATUS_FAILED;
    }

    // Start send queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(tcp_sendQueueHandle))
    {
        OIC_LOG(ERROR, TAG, "Failed to Start Send Data Thread");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CAStartTCPListeningServer()
{
    if (!ocf_server)
    {
        ocf_server = true;    // only needed to run CA tests
    }

    CAResult_t ret = CATCPStartServer((const ca_thread_pool_t)tcp_threadpool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to start listening server![%d]", ret);
        return ret;
    }

    return CA_STATUS_OK;
}

CAResult_t CAStopTCPListeningServer()
{
    return CA_STATUS_OK;
}

CAResult_t CAStartTCPDiscoveryServer()
{
    if (!tcp_client)
    {
        tcp_client = true;    // only needed to run CA tests
    }

    CAResult_t ret = CATCPStartServer((const ca_thread_pool_t)tcp_threadpool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to start discovery server![%d]", ret);
        return ret;
    }

    return CA_STATUS_OK;
}

CAResult_t CATCPStartServer(const ca_thread_pool_t threadPool) /* src: catcpserver.c */
{
    if (tcp_is_started)
    {
        OIC_LOG(DEBUG, TAG, "Adapter is started already");
        return CA_STATUS_OK;
    }

    tcp_is_ipv4_enabled = true; // only needed to run CA tests
    tcp_is_ipv6_enabled = true; // only needed to run CA tests

    CAResult_t res = CA_STATUS_OK;
    s_sessionList = u_arraylist_create();
    if (!s_sessionList)
    {
        res = CA_MEMORY_ALLOC_FAILED;
    }
    if (CA_STATUS_OK != res) {
        OIC_LOG(ERROR, TAG, "failed to alloc s_sessionList");
        return res;
    }
    res = CATCPCreateMutex();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "failed to create mutex");
        return res;
    }
    res = CATCPCreateCond();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "failed to create cond");
        return res;
    }

    tcp_create_accept_sockets();

    // create mechanism for fast shutdown
#ifdef WSA_WAIT_EVENT_0
    // FIXME: windows support
    caglobals.tcp.updateEvent = WSACreateEvent();
    if (WSA_INVALID_EVENT == caglobals.tcp.updateEvent)
    {
        OIC_LOG(ERROR, TAG, "failed to create shutdown event");
        return res;
    }
#else
    CAInitializePipe(tcp_shutdownFds);
    TCP_CHECKFD(tcp_shutdownFds[0]);
    TCP_CHECKFD(tcp_shutdownFds[1]);
#endif

#ifndef WSA_WAIT_EVENT_0
    CAInitializePipe(tcp_connectionFds);
    TCP_CHECKFD(tcp_connectionFds[0]);
    TCP_CHECKFD(tcp_connectionFds[1]);
#endif

    tcp_is_terminating = false;
    OIC_LOG_THREADS_V(DEBUG, TAG, "Adding tcp_data_receiver_runloop to thread pool");
    res = ca_thread_pool_add_task(threadPool,
				  tcp_data_receiver_runloop, // @was CAReceiveHandler,
				  NULL);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread_pool_add_task failed");
        return res;
    }
    OIC_LOG(DEBUG, TAG, "CAReceiveHandler thread started successfully.");

    tcp_is_started = true;
    return CA_STATUS_OK;
}

void CATCPStopServer()
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    if (tcp_is_terminating) // caglobals.tcp.terminate
    {
        OIC_LOG(DEBUG, TAG, "TCP is already terminating");
        return;
    }

    // mutex lock
    oc_mutex_lock(tcp_mutexObjectList);

    // set terminate flag.
    tcp_is_terminating = true;  // caglobals.tcp.terminate = true;

#if !defined(WSA_WAIT_EVENT_0)
    if (tcp_shutdownFds[1] != OC_INVALID_SOCKET)
    {
        close(tcp_shutdownFds[1]);
        tcp_shutdownFds[1] = OC_INVALID_SOCKET;
        // receive thread will stop immediately
    }
#else
    // Unit tests are calling CATCPStopServer even without CATCPStartServer being called.
    if (tcp_is_started) // (caglobals.tcp.started)
    {
        // Ask the receive thread to shut down.
        OC_STATIC_ASSERT((WSA_INVALID_EVENT == ((WSAEVENT)NULL)),
            "The assert() below relies on the default value of "
            "tcp_updateEvent being WSA_INVALID_EVENT");
        assert(tcp_updateEvent != WSA_INVALID_EVENT);
        OC_VERIFY(WSASetEvent(tcp_updateEvent));
    }
#endif

    tcp_close_accept_sockets();

    if (tcp_is_started)
    {
        oc_cond_wait(tcp_condObjectList, tcp_mutexObjectList);
        tcp_is_started = false;
    }

#if !defined(WSA_WAIT_EVENT_0)
    close(tcp_connectionFds[1]);
    close(tcp_connectionFds[0]);
    tcp_connectionFds[1] = OC_INVALID_SOCKET;
    tcp_connectionFds[0] = OC_INVALID_SOCKET;

    close(tcp_shutdownFds[0]);
    tcp_shutdownFds[0] = OC_INVALID_SOCKET;
#endif

    // mutex unlock
    oc_mutex_unlock(tcp_mutexObjectList);

    CATCPDisconnectAll();
    CATCPDestroyMutex();
    CATCPDestroyCond();

    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

/**
 * Callback to be notified on reception of any data from remote OIC devices.
 *
 * @param[in]  endpoint      network endpoint description.
 * @param[in]  data          Data received from remote OIC device.
 * @param[in]  dataLength    Length of data in bytes.
 * @pre  Callback must be registered using CAIPSetPacketReceiveCallback().
 */
#if INTERFACE
typedef void (*CATCPPacketReceivedCallback)(const CASecureEndpoint_t *endpoint,
                                            const void *data,
                                            size_t dataLength);
#endif
static CATCPPacketReceivedCallback tcp_packetReceivedCallback = NULL;
void CATCPSetPacketReceiveCallback(CATCPPacketReceivedCallback callback)
{
    tcp_packetReceivedCallback = callback;
}

void CATCPConnectionStateCB(const char *ipAddress, CANetworkStatus_t status)
{
    (void)ipAddress;
    (void)status;
}

/**
  * Callback to notify connection information in the TCP adapter.
  *
  * @param[in]  endpoint        network endpoint description.
  * @param[in]  isConnected     Whether keepalive message needs to be sent.
  * @param[in]  isClient        Host Mode of Operation.
  * @see  Callback must be registered using CATCPSetKeepAliveCallback().
 */
#if INTERFACE
typedef void (*CATCPConnectionHandleCallback)(const CAEndpoint_t *endpoint,
					      bool isConnected,
                                              bool isClient);
#endif
//CATCPConnectionHandleCallback tcp_connectionCallback = NULL;
void (*tcp_connectionCallback)(const CAEndpoint_t *endpoint, bool isConnected,
                                              bool isClient);
// DELETE: fwding fn CATCPSetConnectionChangedCallback(CATCPConnectionHandler);
/* void CATCPSetConnectionChangedCallback(CATCPConnectionHandleCallback connHandler) */
/* { */
/*     tcp_connectionCallback = connHandler; */ // @was g_connectionCallback
/* } */

/**
  * Callback to notify error in the TCP adapter.
  *
  * @param[in]  endpoint      network endpoint description.
  * @param[in]  data          Data sent/received.
  * @param[in]  dataLength    Length of data in bytes.
  * @param[in]  result        result of request from R.I.
  * @pre  Callback must be registered using CAIPSetPacketReceiveCallback().
 */
#if INTERFACE
typedef void (*CATCPErrorHandleCallback)(const CAEndpoint_t *endpoint, const void *data,
                                         size_t dataLength, CAResult_t result);
#endif
void CATCPErrorHandler(const CAEndpoint_t *endpoint, const void *data,
                              size_t dataLength, CAResult_t result)
{
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    if (tcp_errorCallback)
    {
        tcp_errorCallback(endpoint, data, dataLength, result);
    }
}
static CATCPErrorHandleCallback tcp_tcpErrorHandler = CATCPErrorHandler;
// CATCPSetErrorHandler(CATCPErrorHandler);
/* void CATCPSetErrorHandler(CATCPErrorHandleCallback errorHandleCallback) */
/* { */
/*     tcp_tcpErrorHandler = errorHandleCallback; */
/* } */

// FIXME: move to tcp_status_manager.c?
void CATCPSetKeepAliveCallbacks(CAKeepAliveConnectionCallback ConnHandler)
{
    tcp_connKeepAliveCallback = ConnHandler;
}

// from caconnectivitymanager.c:
/**
 * Callback function to pass the connection information from CA to RI.
 * @param[out]   object           remote device information.
 * @param[out]   isConnected      Whether keepalive message needs to be sent.
 * @param[out]   isClient         Host Mode of Operation.
 */
#if INTERFACE
typedef void (*CAKeepAliveConnectionCallback)(const CAEndpoint_t *object, bool isConnected,
                                              bool isClient);
#endif
/**
 * Register connection status changes callback to process KeepAlive.
 * connection informations are delivered these callbacks.
 * @param[in]   ConnHandler     Connection status changes callback.
 */
// src: caconnectivitymanager.c
void CARegisterKeepAliveHandler(CAKeepAliveConnectionCallback ConnHandler)
{
    CATCPSetKeepAliveCallbacks(ConnHandler);
}
