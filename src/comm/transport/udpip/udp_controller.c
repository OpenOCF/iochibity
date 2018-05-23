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

/* #include "caipnwmonitor.h" */
/* #include "caipserver.h" */
/* #include "caqueueingthread.h" */
/* #include "caadapterutils.h" */
/* #ifdef __WITH_DTLS__ */
/* #include "ca_adapter_net_ssl.h" */
/* #endif */
/* #include "octhread.h" */
/* #include "uarraylist.h" */
/* #include "caremotehandler.h" */
/* #include "logger.h" */
/* #include "oic_malloc.h" */
/* #include "oic_string.h" */
/* #include "iotivity_debug.h" */

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
// @rewrite g_networkPacketCallback removed
/* static CANetworkPacketReceivedCallback g_networkPacketCallback = NULL; */
/* static void (*g_networkPacketCallback)(const CASecureEndpoint_t *sep, */
/* 				       const void *data, */
/* 				       size_t dataLen) = NULL; */

/**
 * Network Changed Callback to CA.
 */
// static CAAdapterChangeCallback g_networkChangeCallback = NULL;
// @rewrite not used: static void (*g_networkChangeCallback)(CATransportAdapter_t adapter, CANetworkStatus_t status) = NULL;


/**
 * error Callback to CA adapter.
 */
static CAErrorHandleCallback g_udpErrorCB = NULL;

/* static void CAIPPacketReceivedCB(const CASecureEndpoint_t *endpoint, */
/*                                  const void *data, size_t dataLength); */

void CAIPPullData()
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}

#ifdef __WITH_DTLS__
static ssize_t CAIPPacketSendCB(CAEndpoint_t *endpoint,
                                const void *data, size_t dataLength);
#endif

#ifndef SINGLE_THREAD

/* static CAResult_t CAIPInitializeQueueHandles(); */

/* static void CAIPDeinitializeQueueHandles(); */

/* static void CAIPSendDataThread(void *threadData); */

/* static CAIPData_t *CACreateIPData(const CAEndpoint_t *remoteEndpoint, */
/*                                   const void *data, uint32_t dataLength, */
/*                                   bool isMulticast); */

/* void CAFreeIPData(CAIPData_t *ipData); */

/* static void CADataDestroyer(void *data, uint32_t size); */

// FIXMME: move this to udp_status_manager?
// @rewrite udp_update_local_endpoint_cache @was CAUpdateStoredIPAddressInfo
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

#ifdef __WITH_DTLS__
static ssize_t CAIPPacketSendCB(CAEndpoint_t *endpoint, const void *data, size_t dataLength)
{
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint is NULL", -1);
    VERIFY_NON_NULL_RET(data, TAG, "data is NULL", -1);

    CAIPSendData(endpoint, data, dataLength, false);
    return dataLength;
}
#endif

/* static void CAUDPPacketReceivedCB(const CASecureEndpoint_t *sep, */
/* 				  const void *data, */
/* 				  size_t dataLength) */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); */
/*     VERIFY_NON_NULL_VOID(sep, TAG, "sep is NULL"); */
/*     VERIFY_NON_NULL_VOID(data, TAG, "data is NULL"); */

/*     OIC_LOG_V(DEBUG, TAG, "Address: %s, port:%d", sep->endpoint.addr, sep->endpoint.port); */

/*     // @rewrite g_networkPacketCallback holds cainterfacecontroller::CAReceivedPacketCallback */
/*     // we can just call that directly */

/*     if (g_networkPacketCallback) */
/*     { */
/* 	OIC_LOG_V(DEBUG, TAG, "CALLING g_networkPacketCallback!!!"); */
/*         g_networkPacketCallback(sep, data, dataLength); */
/*     } else { */
/* 	OIC_LOG_V(DEBUG, TAG, "NO g_networkPacketCallback!!!"); */
/*     } */

/* } */

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

    if (g_udpErrorCB)
    {
        g_udpErrorCB(endpoint, data, dataLength, result);
    }
}

// @rewrite: we initialize statically, no need for CAInitializeUDPGlobals
/* static void CAInitializeUDPGlobals() */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); */
/*     caglobals.ip.u6.fd  = OC_INVALID_SOCKET; */
/*     caglobals.ip.u6s.fd = OC_INVALID_SOCKET; */
/*     caglobals.ip.u4.fd  = OC_INVALID_SOCKET; */
/*     caglobals.ip.u4s.fd = OC_INVALID_SOCKET; */
/*     caglobals.ip.m6.fd  = OC_INVALID_SOCKET; */
/*     caglobals.ip.m6s.fd = OC_INVALID_SOCKET; */
/*     caglobals.ip.m4.fd  = OC_INVALID_SOCKET; */
/*     caglobals.ip.m4s.fd = OC_INVALID_SOCKET; */
/*     caglobals.ip.u6.port  = 0; */
/*     caglobals.ip.u6s.port = 0; */
/*     caglobals.ip.u4.port  = 0; */
/*     caglobals.ip.u4s.port = 0; */
/*     caglobals.ip.m6.port  = CA_COAP; */
/*     caglobals.ip.m6s.port = CA_SECURE_COAP; */
/*     caglobals.ip.m4.port  = CA_COAP; */
/*     caglobals.ip.m4s.port = CA_SECURE_COAP; */

/*     // @rewrite: flags is only to initialize ipvXenabled, which we do statically */
/*     /\* CATransportFlags_t flags = 0; *\/ */
/*     /\* if (caglobals.client) *\/ */
/*     /\* { *\/ */
/*     /\*     flags |= caglobals.clientFlags; *\/ */
/*     /\* } *\/ */
/*     /\* if (caglobals.server) *\/ */
/*     /\* { *\/ */
/*     /\*     flags |= caglobals.serverFlags; *\/ */
/*     /\* } *\/ */
/*     // @rewrite : ipvXenabled flags are boolean! we initialize statically */
/*     /\* caglobals.ip.ipv6enabled = flags & CA_IPV6; *\/ */
/*     /\* caglobals.ip.ipv4enabled = flags & CA_IPV4; *\/ */
/*     // FIXME: we're already forcing dual stack, initialize it statically */
/*     /\* caglobals.ip.dualstack = caglobals.ip.ipv6enabled && caglobals.ip.ipv4enabled; *\/ */
/*     OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__); */
/* } */

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
    // g_networkChangeCallback = CAAdapterChangedCallback;  // netCallback;
    // g_networkPacketCallback = ifc_CAReceivedPacketCallback;  // networkPacketCallback;
    g_udpErrorCB            = CAAdapterErrorHandleCallback;  //errorCallback;

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
				 CAIPPacketSendCB, CAIPErrorHandler, CA_ADAPTER_IP);
    }
#endif

    // @rewrite: call these directly, no need for CAConnectivityHandler
    static const CAConnectivityHandler_t udpController =
        {
            .startAdapter         = &CAStartUDP,
            .stopAdapter          = &CAStopIP,
            /* .startListenServer    = &CAStartIPListeningServer, */
            .startListenServer    = &udp_add_ifs_to_multicast_groups, // @was CAIPStartListenServer,
            .stopListenServer     = &udp_close_sockets, // @was CAStopIPListeningServer -> CAIPStopListenServer,
            .startDiscoveryServer = &CAStartIPDiscoveryServer,
	    // why no stopDiscoveryServer?
            .unicast              = &CASendIPUnicastData,
            .multicast            = &CASendIPMulticastData,
            .GetNetInfo           = &udp_get_local_endpoints, // @was CAGetIPInterfaceInformation
            .readData             = &CAReadIPData,
            .terminate            = &CATerminateIP,
            .cType                = CA_ADAPTER_IP
        };
    //registerCallback(udpController);
    CARegisterCallback(udpController);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

/* called by CAStartUDP after udp_config_data_sockets has been called */
CAResult_t udp_start_services(const ca_thread_pool_t threadPool) // @was CAIPStartServer
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    CAResult_t res = CA_STATUS_OK;

    if (udp_started) return res;

    CAInitializeFastShutdownMechanism();

    CARegisterForAddressChanges();

    udp_selectTimeout = CAGetPollingInterval(udp_selectTimeout);

    res = udp_add_ifs_to_multicast_groups();  /* @was CAIPStartListenServer */
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "udp_add_ifs_to_multicast_groups failed with rc [%d]", res);
        return res;
    }

    udp_terminate = false;
    // @rewrite udp_data_receiver_runloop @was CAReceiveHandler
    udp_data_receiver_runloop_cond = oc_cond_new();
    res = ca_thread_pool_add_task(threadPool,
				  udp_data_receiver_runloop,
				  "udp_data_receiver_runloop",
				  NULL);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread_pool_add_task failed for udp_data_receiver_runloop");
        return res;
    }
    OIC_LOG(DEBUG, TAG, "udp_data_receiver_runloop thread started successfully.");

    udp_started = true;
    return CA_STATUS_OK;
}

/* CAAdapterStart */
CAResult_t CAStartUDP()
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
    /* addresses will be added by CAIPGetInterfaceInformation, from CAFindInterfaceChange etc. */
    // @rewrite: the created list is never used, so why bother?
    CAIPCreateNetworkAddressList(); // @was CAIPStartNetworkMonitor

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

static int32_t CAQueueIPData(bool isMulticast, const CAEndpoint_t *endpoint,
                             const void *data, uint32_t dataLength)
{
    VERIFY_NON_NULL_RET(endpoint, TAG, "remoteEndpoint", -1);
    VERIFY_NON_NULL_RET(data, TAG, "data", -1);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, TAG, "Invalid Data Length");
        return -1;
    }

#ifdef SINGLE_THREAD

    CAIPSendData(endpoint, data, dataLength, isMulticast);
    return dataLength;

#else

    VERIFY_NON_NULL_RET(g_sendQueueHandle, TAG, "sendQueueHandle", -1);
    // Create IPData to add to queue
    CAIPData_t *ipData = CACreateIPData(endpoint, data, dataLength, isMulticast);
    if (!ipData)
    {
        OIC_LOG(ERROR, TAG, "Failed to create ipData!");
        return -1;
    }
    // Add message to send queue
    CAQueueingThreadAddData(g_sendQueueHandle, ipData, sizeof(CAIPData_t));

#endif // SINGLE_THREAD

    return dataLength;
}

int32_t CASendIPUnicastData(const CAEndpoint_t *endpoint,
                            const void *data, uint32_t dataLength,
                            CADataType_t dataType)
{
    (void)dataType;
    return CAQueueIPData(false, endpoint, data, dataLength);
}

int32_t CASendIPMulticastData(const CAEndpoint_t *endpoint, const void *data, uint32_t dataLength,
                              CADataType_t dataType)
{
    (void)dataType;
    return CAQueueIPData(true, endpoint, data, dataLength);
}

CAResult_t CAReadIPData()
{
    CAIPPullData();
    return CA_STATUS_OK;
}

CAResult_t CAStopIP()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

#ifdef __WITH_DTLS__
    CAdeinitSslAdapter();
#endif

#ifndef SINGLE_THREAD
    if (g_sendQueueHandle && g_sendQueueHandle->threadMutex)
    {
        CAQueueingThreadStop(g_sendQueueHandle);
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

#ifndef SINGLE_THREAD

void CAIPSendDataThread(void *threadData)
{
    CAIPData_t *ipData = (CAIPData_t *) threadData;
    if (!ipData)
    {
        OIC_LOG(DEBUG, TAG, "Invalid ip data!");
        return;
    }

    if (ipData->isMulticast)
    {
        //Processing for sending multicast
        OIC_LOG(DEBUG, TAG, "Sending Multicast");
        CAIPSendData(ipData->remoteEndpoint, ipData->data, ipData->dataLen, true);
    }
    else
    {
        //Processing for sending unicast
        OIC_LOG(DEBUG, TAG, "Sending Unicast");
#ifdef __WITH_DTLS__
        if (ipData->remoteEndpoint && ipData->remoteEndpoint->flags & CA_SECURE)
        {
            OIC_LOG(DEBUG, TAG, "Sending encrypted");
            CAResult_t result = CAencryptSsl(ipData->remoteEndpoint, ipData->data, ipData->dataLen);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, TAG, "CAencryptSsl failed: %d", result);
            }
            OIC_LOG_V(DEBUG, TAG, "CAencryptSsl returned with result[%d]", result);
        }
        else
        {
            OIC_LOG(DEBUG, TAG, "Sending unencrypted");
            CAIPSendData(ipData->remoteEndpoint, ipData->data, ipData->dataLen, false);
        }
#else
        CAIPSendData(ipData->remoteEndpoint, ipData->data, ipData->dataLen, false);
#endif
    }
}

#endif

#ifndef SINGLE_THREAD
// create IP packet for sending
CAIPData_t *CACreateIPData(const CAEndpoint_t *remoteEndpoint, const void *data,
                           uint32_t dataLength, bool isMulticast)
{
    VERIFY_NON_NULL_RET(remoteEndpoint, TAG, "remoteEndpoint is NULL", NULL);
    VERIFY_NON_NULL_RET(data, TAG, "IPData is NULL", NULL);

    CAIPData_t *ipData = (CAIPData_t *) OICMalloc(sizeof(*ipData));
    if (!ipData)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return NULL;
    }

    ipData->remoteEndpoint = CACloneEndpoint(remoteEndpoint);
    ipData->data = (void *) OICMalloc(dataLength);
    if (!ipData->data)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        CAFreeIPData(ipData);
        return NULL;
    }

    memcpy(ipData->data, data, dataLength);
    ipData->dataLen = dataLength;

    ipData->isMulticast = isMulticast;

    return ipData;
}

void CAFreeIPData(CAIPData_t *ipData)
{
    VERIFY_NON_NULL_VOID(ipData, TAG, "ipData is NULL");

    CAFreeEndpoint(ipData->remoteEndpoint);
    OICFree(ipData->data);
    OICFree(ipData);
}

void CADataDestroyer(void *data, uint32_t size)
{
    if (size < sizeof(CAIPData_t))
    {
        OIC_LOG_V(ERROR, TAG, "Destroy data too small %p %d", data, size);
    }
    CAIPData_t *etdata = (CAIPData_t *) data;

    CAFreeIPData(etdata);
}

#endif // SINGLE_THREAD

// CAVEAT: only lists unicast sockets
// @rewrite: udp_get_local_endpoints @was caipserver0:CAGetIPInterfaceInformation
CAResult_t udp_get_local_endpoints(CAEndpoint_t **info, size_t *size)
{
    VERIFY_NON_NULL_MSG(info, TAG, "info is NULL");
    VERIFY_NON_NULL_MSG(size, TAG, "size is NULL");

    // GAR: get live list of CAInterface_t for all IFs (via getifaddrs)
    u_arraylist_t *iflist = CAIPGetInterfaceInformation(0);
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

        if ((ifitem->family == AF_INET6 && !udp_ipv6enabled) ||
            (ifitem->family == AF_INET && !udp_ipv4enabled))
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

    /* create one CAEndpoint_t per IF */
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
        if ((ifitem->family == AF_INET6 && !udp_ipv6enabled) ||
            (ifitem->family == AF_INET && !udp_ipv4enabled))
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

CAResult_t CAGetLinkLocalZoneId(uint32_t ifindex, char **zoneId)
{
    return CAGetLinkLocalZoneIdInternal(ifindex, zoneId);
}
