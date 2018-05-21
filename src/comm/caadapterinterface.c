/* *****************************************************************
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

/**
 * @file
 *
 * This file contains the APIs for adapters to be implemented.
 */

/**
 * Adapter State to indicate the network changed notifications.
 */
typedef enum
{
    CA_ADAPTER_DISABLED,   /**< Adapter is Disabled */
    CA_ADAPTER_ENABLED     /**< Adapter is Enabled */
} CAAdapterState_t;

#if EXPORT_INTERFACE
/**
 * Transport adapter protocol. Each transport adapter will have one of
 * these structs, populated with transport-specific method
 * implementations.
 *
 * The struct is mildly misnamed, insofar as "handler" implies event
 * handling. "Controller" might be better, or at the least "Adapter",
 * e.g. "CATransportController"
 */
// @rewrite: not needed. these can all be defined directly in transport-specific files.
typedef struct
{
    /** Start Transport specific functions. */
    CAResult_t (*startAdapter)();

    /** start multicast listener */
    CAResult_t (*startListenServer)();
    //CAAdapterStartListeningServer *startListenServer;

    /** Stops receiving the multicast traffic. */
    CAResult_t (*stopListenServer)();
    //CAAdapterStopListeningServer *stopListenServer;

    CAResult_t (*startDiscoveryServer)();
    //CAAdapterStartDiscoveryServer *startDiscoveryServer;

    /** Send unicast data **/
    int32_t (*unicast)(const CAEndpoint_t *endpoint,
    			const void *data, uint32_t dataLen,
    			CADataType_t dataType);
    // CAAdapterSendUnicastData *sendData;

    int32_t                                         /**< @return nbr bytes sent */
    (*multicast)(const CAEndpoint_t *endpoint , /**< [in] */
			     const void *data     , /**< [in] */
			     uint32_t dataLen     , /**< [in] */
			     CADataType_t dataType  /**< [in] */ );

    /** Get nw interface information. **/
    CAResult_t (*GetNetInfo)(CAEndpoint_t **info, /**< [out] */
			     size_t *size     /**< [out] nbr of local connectivity structs */);
    //CAAdapterGetNetworkInfo *GetnetInfo;

    CAResult_t (*readData)();
    /* CAAdapterReadData *readData; */

    /** Stop Transport specific functions. */
    CAResult_t (*stopAdapter)();
    /* CAAdapterStop *stopAdapter; */

    void (*terminate)();
    //CAAdapterTerminate *terminate;

    CATransportAdapter_t cType;
} CAConnectivityHandler_t;

/* #if EXPORT_INTERFACE */
/* // This is only explicitly used in this the nwmonitor files, but it is */
/* // the signature of fns defined in caipadapter0, catcpadapter and */
/* // passed to nwmonitor */
/* typedef void (*CAIPAdapterStateChangeCallback)(CATransportAdapter_t adapter, */
/*                                                CANetworkStatus_t status); */
/* #endif */


/**
 * This will be used during the registration of adapters call backs to the common logic.
 * @see ::CAConnectivityHandler_t
 */
/* typedef void (*CARegisterConnectivityCallback)(CAConnectivityHandler_t handler); */
// only used to type parameter to CAInitialize<Transport>, e.g. CAInitializeIP in caipadapter0
// only one fn is defined with this signature, in cainterfacecontroller:
// static void CARegisterCallback(CAConnectivityHandler_t handler)
// same is passed by CAInitializeAdapters to CAInitializeIP, CAInitializeEDR, etc

/**
 * This will be used during the receive of network requests and response.
 * @see SendUnicastData(), SendMulticastData()
 */
//typedef void (*CANetworkPacketReceivedCallback)(const CASecureEndpoint_t *sep,
//                                            const void *data, size_t dataLen);
// not to be confused with this, from ssl:
/* typedef void (*CAPacketReceivedCallback)(const CASecureEndpoint_t *sep, */
/*                                          const void *data, size_t dataLength); */

// should be CATransportPacketRecdCB
// used in two places: transport adapters, and camessagehandler. each
// keeps one of these in a static var.
// for camessenger, it is set by
// CASetPacketReceivedCallback which is called by
// CAInitializeMessageHandler, which passes CAReceivedPacketCallback
// in cainterfacecontroller:
// static void CAReceivedPacketCallback(const CASecureEndpoint_t *sep,
//                                     const void *data, size_t dataLen)
// for transport servers: CA<Transport>SetPacketReceiveCallback (NB: missing d)
// sets transport-specific static var, e.g. g_packetReceivedCallback
// (which should be g_<transport>PacketRecdCallback)
// passed in CA<Transport>Initialize, static CAReceivedPacketCallback, same for all transports
// so we can inline this too

/**
 * This will be used to notify network changes to the connectivity common logic layer.
 */
// typedef void (*CAAdapterChangeCallback)(CATransportAdapter_t adapter, CANetworkStatus_t status);
// caipadapter0:  static CAAdapterChangeCallback g_networkChangeCallback = NULL;
// set by CAInitializeIP, which is called by interfacecontroller
// the generic implementation is CAAdapterChangedCallback from interfacecontroller

/**
 * This will be used to notify connection changes to the connectivity common logic layer.
 */
typedef void (*CAConnectionChangeCallback)(const CAEndpoint_t *info, bool isConnected);

/**
 * This will be used to notify error result to the connectivity common logic layer.
 */
typedef void (*CAErrorHandleCallback)(const CAEndpoint_t *endpoint,
                                      const void *data, size_t dataLen,
                                      CAResult_t result);
#endif	/* INTERFACE */


// OBSOLETE:
/**
 * Starting connectivity adapters and each adapter have transport specific behavior.
 * Transport Specific Behavior:
 * WIFI/ETH connectivity Starts unicast server on  all available IPs and defined
 * port number as per specification.
 * EDR will not start any specific servers.
 * LE will not start any specific servers.
 * @return ::CA_STATUS_OK or ::CA_STATUS_FAILED
 *  ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
//typedef CAResult_t CAAdapterStart();

/**
 * Starting listening server for receiving multicast search requests
 * Transport Specific Behavior:
 * WIFI/ETH Starts multicast server on  all available IPs and defined
 * port number and as per specification.
 * EDR  Starts RFCOMM Server with prefixed UUID as per specification.
 * LE Start GATT Server with prefixed UUID and Characteristics as per OIC Specification.
 * @return ::CA_STATUS_OK or ::CA_STATUS_FAILED
 * ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
//typedef CAResult_t CAAdapterStartListeningServer();

/**
 * Stopping listening server to not receive multicast search requests
 * Transport Specific Behavior:
 * WIFI/ETH Stops multicast server on  all available IPs. This is required for the
 * thin device that call this function once all local resources are pushed to the
 * resource directory.
 * @return ::CA_STATUS_OK or ::CA_STATUS_FAILED
 * ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
//typedef CAResult_t CAAdapterStopListeningServer();

/**
 * for starting discovery servers for receiving multicast advertisements
 * Transport Specific Behavior:
 * WIFI/ETH Starts multicast server on all available IPs and defined port
 * number as per OIC Specification.
 * EDR Starts RFCOMM Server with prefixed UUID as per OIC Specification.
 * LE Starts GATT Server with prefixed UUID and Characteristics as per OIC Specification.
 * @return ::CA_STATUS_OK or ::CA_STATUS_FAILED
 * ERROR CODES (::CAResult_t error codes in cacommon.h).
 */

// GAR: the idea seems to be that clients need to listen, but only to
// service discovery requests. However, for IP this just calls
// CAIPStartListenServer, same one used on servers
//typedef CAResult_t CAAdapterStartDiscoveryServer();

/**
 * Sends data to the endpoint using the adapter connectivity.
 * Note: length must be > 0.
 * @param[in]   endpoint        Remote Endpoint information (like ipaddress , port,
 * reference uri and connectivity type) to which the unicast data has to be sent.
 * @param[in]   data            Data which required to be sent.
 * @param[in]   dataLen         Size of data to be sent.
 * @param[in]   dataType        Data type which is REQUEST or RESPONSE.
 * @return The number of bytes sent on the network. Return value equal to -1 indicates error.
 */
/* typedef int32_t CAAdapterSendUnicastData(const CAEndpoint_t *endpoint, */
/* 					 const void *data, uint32_t dataLen, */
/* 					 CADataType_t dataType); */

/**
 * Sends Multicast data to the endpoint using the adapter connectivity.
 * Note: length must be > 0.
 * @param[in]   endpoint        Remote Endpoint information (like ipaddress , port,
 * @param[in]   data            Data which required to be sent.
 * @param[in]   dataLen         Size of data to be sent.
 * @param[in]   dataType        Data type which is REQUEST or RESPONSE.
 * @return The number of bytes sent on the network. Return value equal to -1 indicates error.
 */
/* typedef int32_t CAAdapterSendMulticastData(const CAEndpoint_t *endpoint, */
/* 					   const void *data, uint32_t dataLen, */
/* 					   CADataType_t dataType); */

/**
 * Get Network Information.
 * @param[out]   info           Local connectivity information structures
 * @param[out]   size           Number of local connectivity structures.
 * @return ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h)
 */
//typedef CAResult_t CAAdapterGetNetworkInfo(CAEndpoint_t **info, size_t *size);

/**
 * Read Synchronous API callback.
 * @return ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h)
 */
//typedef CAResult_t CAAdapterReadData();

/**
 * Stopping the adapters and close socket connections.
 * Transport Specific Behavior:
 * WIFI/ETH Stops all listening servers and close sockets.
 * EDR Stops all RFCOMM servers and close sockets.
 * LE Stops all GATT servers and close sockets.
 * @return CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
// typedef CAResult_t CAAdapterStop();

/**
 * Terminate the connectivity adapter.Configuration information will be deleted from
 * further use. Freeing Memory of threadpool and mutexs and cleanup will be done.
 */
// typedef void CAAdapterTerminate();

