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

/**
 * Logging tag for module name.
 */
#define TAG "OIC_CA_IP_ADAP"

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

/**
 * Queue handle for Send Data.
 */
static CAQueueingThread_t *g_sendQueueHandle = NULL;
#endif

/**
 * List of the CAEndpoint_t* that has a stack-owned IP address.
 */
static u_arraylist_t *g_ownIpEndpointList = NULL;

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
#ifdef __WITH_DTLS__
static ssize_t CAIPPacketSendCB(CAEndpoint_t *endpoint,
                                const void *data, size_t dataLength);
#endif

/* static void CAUpdateStoredIPAddressInfo(CANetworkStatus_t status); */

#ifndef SINGLE_THREAD

/* static CAResult_t CAIPInitializeQueueHandles(); */

/* static void CAIPDeinitializeQueueHandles(); */

/* static void CAIPSendDataThread(void *threadData); */

/* static CAIPData_t *CACreateIPData(const CAEndpoint_t *remoteEndpoint, */
/*                                   const void *data, uint32_t dataLength, */
/*                                   bool isMulticast); */

/* void CAFreeIPData(CAIPData_t *ipData); */

/* static void CADataDestroyer(void *data, uint32_t size); */

void CAUpdateStoredIPAddressInfo(CANetworkStatus_t status)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    if (CA_INTERFACE_UP == status)
    {
        OIC_LOG(DEBUG, TAG, "UDP adapter status is on. Store the own IP address info");

        CAEndpoint_t *eps = NULL;
        size_t numOfEps = 0;

        CAResult_t res = CAGetIPInterfaceInformation(&eps, &numOfEps);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CAGetIPInterfaceInformation failed");
            return;
        }

        for (size_t i = 0; i < numOfEps; i++)
        {
            u_arraylist_add(g_ownIpEndpointList, (void *)&eps[i]);
        }
    }
    else // CA_INTERFACE_DOWN
    {
        OIC_LOG(DEBUG, TAG, "IP adapter status is off. Remove the own IP address info");

        CAEndpoint_t *headEp = u_arraylist_get(g_ownIpEndpointList, 0);
        OICFree(headEp);
        headEp = NULL;

        size_t len = u_arraylist_length(g_ownIpEndpointList);
        for (size_t i = len; i > 0; i--)
        {
            u_arraylist_remove(g_ownIpEndpointList, i - 1);
        }
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

static CAResult_t CAIPInitializeQueueHandles()
{
    // Check if the message queue is already initialized
    if (g_sendQueueHandle)
    {
        OIC_LOG(DEBUG, TAG, "send queue handle is already initialized!");
        return CA_STATUS_OK;
    }

    g_ownIpEndpointList = u_arraylist_create();
    if (!g_ownIpEndpointList)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed! (g_ownIpEndpointList)");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Create send message queue
    g_sendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_sendQueueHandle)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed! (g_sendQueueHandle)");
        u_arraylist_free(&g_ownIpEndpointList);
        g_ownIpEndpointList = NULL;
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_sendQueueHandle,
#ifdef DEBUG_THREADING
						   "g_sendQueueHandle",
#endif
			   // (const ca_thread_pool_t)caglobals.ip.threadpool,
                                (const ca_thread_pool_t)udp_threadpool,
                                CAIPSendDataThread, CADataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        OICFree(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
        u_arraylist_free(&g_ownIpEndpointList);
        g_ownIpEndpointList = NULL;
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

static void CAIPDeinitializeQueueHandles()
{
    CAQueueingThreadDestroy(g_sendQueueHandle);
    OICFree(g_sendQueueHandle);
    g_sendQueueHandle = NULL;

    // Since the items in g_ownIpEndpointList are allocated once in a big chunk, we only need to
    // free the first item. Another location this is done is in the CA_INTERFACE_DOWN handler
    // in CAUpdateStoredIPAddressInfo().
    OICFree(u_arraylist_get(g_ownIpEndpointList, 0));
    u_arraylist_free(&g_ownIpEndpointList);
    g_ownIpEndpointList = NULL;
}

#endif // SINGLE_THREAD

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

    size_t len = u_arraylist_length(g_ownIpEndpointList);
    for (size_t i = 0; i < len; i++)
    {
        CAEndpoint_t *ownIpEp = u_arraylist_get(g_ownIpEndpointList, i);
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
            .GetNetInfo           = &CAGetIPInterfaceInformation,
            .readData             = &CAReadIPData,
            .terminate            = &CATerminateIP,
            .cType                = CA_ADAPTER_IP
        };
    //registerCallback(udpController);
    CARegisterCallback(udpController);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
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
    CAIPCreateNetworkAddressList();

#ifdef SINGLE_THREAD
    uint16_t unicastPort = 55555;
    // Address is hardcoded as we are using Single Interface

    // udp_config_data_sockets();

    CAResult_t ret = CAIPStartServer();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(DEBUG, TAG, "CAIPStartServer failed[%d]", ret);
        return ret;
    }
#else
    if (CA_STATUS_OK != CAIPInitializeQueueHandles())
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize Queue Handle");
        CATerminateIP();
        return CA_STATUS_FAILED;
    }

    // Start send queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(g_sendQueueHandle))
    {
        OIC_LOG(ERROR, TAG, "Failed to Start Send Data Thread");
        return CA_STATUS_FAILED;
    }

    CAResult_t ret = udp_config_data_sockets((const ca_thread_pool_t)udp_threadpool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "udp_config_data_sockets failed, code %d", ret);
        return ret;
    }

    // CAResult_t ret = CAIPStartServer((const ca_thread_pool_t)caglobals.ip.threadpool);
    // CAIPStartServer creates the ports etc.
    ret = CAIPStartServer((const ca_thread_pool_t)udp_threadpool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to start server![%d]", ret);
        return ret;
    }

#endif

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

// @rewrite: this is unnecessary, just call CAIPStartListenServer directly
/* CAResult_t CAStartIPListeningServer() */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); */
/*     CAResult_t ret = CAIPStartListenServer(); */
/*     if (CA_STATUS_OK != ret) */
/*     { */
/*         OIC_LOG_V(ERROR, TAG, "Failed to start listening server![%d]", ret); */
/*         return ret; */
/*     } */

/*     return CA_STATUS_OK; */
/* } */

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
    CAIPDeinitializeQueueHandles();
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
