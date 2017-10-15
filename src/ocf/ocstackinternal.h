//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//



/**
 * @file
 *
 * This file contains the Internal include file used by lower layers of the OC stack
 *
 */

#ifndef OCSTACKINTERNAL_H_
#define OCSTACKINTERNAL_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdbool.h>
#include "ocstack.h"
#include "ocstackconfig.h"
#include "occlientcb.h"
#include "ocrandom.h"

#include "cacommon.h"
#include "cainterface.h"
//GAR #include "securevirtualresourcetypes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

/** Default device entity Handler.*/
extern OCDeviceEntityHandler defaultDeviceHandler;

/** Default Callback parameter.*/
extern void* defaultDeviceHandlerCallbackParameter;

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

/** The coap scheme */
#define OC_COAP_SCHEME "coap://"

/** the first outgoing sequence number will be 1*/
#define OC_OFFSET_SEQUENCE_NUMBER (0)

/**
 * This structure will be created in occoap and passed up the stack on the server side.
 */
typedef struct
{
    /** Observe option field.*/
    uint32_t observationOption;

    /** The REST method retrieved from received request PDU.*/
    OCMethod method;

    /** the provided payload format. */
    OCPayloadFormat payloadFormat;

    /** the requested payload format. */
    OCPayloadFormat acceptFormat;

    /** the requested payload version. */
    uint16_t acceptVersion;

    /** resourceUrl will be filled in occoap using the path options in received request PDU.*/
    char resourceUrl[MAX_URI_LENGTH];

    /** resource query send by client.*/
    char query[MAX_QUERY_LENGTH];

    /** reqJSON is retrieved from the payload of the received request PDU.*/
    uint8_t *payload;

    /** qos is indicating if the request is CON or NON.*/
    OCQualityOfService qos;

    /** Number of the received vendor specific header options.*/
    uint8_t numRcvdVendorSpecificHeaderOptions;

    /** Array of received vendor specific header option .*/
    OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];

    /** Remote end-point address **/
    OCDevAddr devAddr;

    /** Token for the observe request.*/
    CAToken_t requestToken;

    /** token length.*/
    uint8_t tokenLength;

    /** The ID of CoAP PDU.*/
    uint16_t coapID;

    /** For delayed Response.*/
    uint8_t delayedResNeeded;

    /** For More packet.*/
    uint8_t reqMorePacket;

    /** The number of requested packet.*/
    uint32_t reqPacketNum;

    /** The size of requested packet.*/
    uint16_t reqPacketSize;

    /** The number of responded packet.*/
    uint32_t resPacketNum;

    /** Responded packet size.*/
    uint16_t resPacketSize;

    /** The total size of requested packet.*/
    size_t reqTotalSize;
} OCServerProtocolRequest;

/**
 * This typedef is to represent our Server Instance identification.
 */
typedef uint8_t ServerID[16];

//-----------------------------------------------------------------------------
// Internal function prototypes
//-----------------------------------------------------------------------------


/**
 * Handler function for sending a response from multiple resources, such as a collection.
 * Aggregates responses from multiple resource until all responses are received then sends the
 * concatenated response
 *
 * TODO: Need to add a timeout in case a (remote?) resource does not respond
 *
 * @param token         Token to search for.
 * @param tokenLength   Length of token.
 * @param status        Feedback status.
 * @return
 *     ::OCStackResult
 */

OCStackResult OCStackFeedBack(CAToken_t token, uint8_t tokenLength, uint8_t status);


/**
 * Handler function to execute stack requests
 *
 * @param protocolRequest      Pointer to the protocol requests from server.
 *
 * @return
 *     ::OCStackResult
 */
OCStackResult HandleStackRequests(OCServerProtocolRequest * protocolRequest);

/**
 * Ensure the accept header option is set appropriatly before sending the requests and routing
 * header option is updated with destination.
 *
 * @param object CA remote endpoint.
 * @param requestInfo CA request info.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult OCSendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo);

OCStackResult SendDirectStackResponse(const CAEndpoint_t* endPoint, const uint16_t coapID,
        const CAResponseResult_t responseResult, const CAMessageType_t type,
        const uint8_t numOptions, const CAHeaderOption_t *options,
        CAToken_t token, uint8_t tokenLength, const char *resourceUri,
        CADataType_t dataType);

/**
 * Bind a resource interface to a resource.
 *
 * @param resource Target resource.
 * @param resourceInterfaceName Resource interface.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult BindResourceInterfaceToResource(OCResource* resource,
                                            const char *resourceInterfaceName);
/**
 * Bind a resource type to a resource.
 *
 * @param resource Target resource.
 * @param resourceTypeName Name of resource type.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult BindResourceTypeToResource(OCResource *resource,
                                            const char *resourceTypeName);
/**
 * Bind a Transport Protocol Suites type to a resource.
 *
 * @param resource Target resource.
 * @param resourceTpsTypes Name of transport protocol suites type.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult BindTpsTypeToResource(OCResource *resource,
                                    OCTpsSchemeFlags resourceTpsTypes);

/**
 * Convert OCStackResult to CAResponseResult_t.
 *
 * @param ocCode OCStackResult code.
 * @param method OCMethod method the return code replies to.
 * @return ::CA_CONTENT on OK, some other value upon failure.
 */
CAResponseResult_t OCToCAStackResult(OCStackResult ocCode, OCMethod method);

/**
 * Converts a CAResult_t type to a OCStackResult type.
 *
 * @param caResult CAResult_t value to convert.
 * @return OCStackResult that was converted from the input CAResult_t value.
 */
OCStackResult CAResultToOCResult(CAResult_t caResult);

/**
 * Converts a OCStackResult type to a bool type.
 *
 * @param ocResult OCStackResult value to convert.
 * @return true on success, false upon failure.
 */
bool OCResultToSuccess(OCStackResult ocResult);

/**
 * Map OCQualityOfService to CAMessageType.
 *
 * @param qos Input qos.
 *
 * @return CA message type for a given qos.
 */
CAMessageType_t qualityOfServiceToMessageType(OCQualityOfService qos);

void CopyEndpointToDevAddr(const CAEndpoint_t *in, OCDevAddr *out);

void CopyDevAddrToEndpoint(const OCDevAddr *in, CAEndpoint_t *out);

/**
 * Get the CoAP ticks after the specified number of milli-seconds.
 *
 * @param milliSeconds Milli-seconds.
 * @return CoAP ticks
 */
uint32_t GetTicks(uint32_t milliSeconds);

/**
 * Extract interface and resource type from the query.
 *
 * @param query is the request received from the client
 * @param filterOne will include result if the interface is included in the query.
 * @param filterTwo will include result if the resource type is included in the query.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure
 */
OCStackResult ExtractFiltersFromQuery(const char *query, char **filterOne, char **filterTwo);

#if defined(RD_CLIENT) || defined(RD_SERVER)
/**
 * This function binds an resource unique ins value to the resource. This can be only called
 * when the stack has received a response from resource-directory.
 *
 * @param requestUri URI of the resource.
 * @param response Response from queries to remote servers.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult OCUpdateResourceInsWithResponse(const char *requestUri,
                                              const OCClientResponse *response);
#endif

/**
 * Delete all of the dynamically allocated elements that were created for the resource attributes.
 *
 * @param rsrcAttributes Specified resource attribute.
 */
void OCDeleteResourceAttributes(OCAttribute *rsrcAttributes);

#ifndef TCP_ADAPTER
/**
 * Add resource payload with endpoint payload to discovery payload.
 *
 * @param payload       Pointer to discovery payload.
 * @param res           Pointer to OCresource structure.
 * @param securePort    Secure port number.
 * @param networkInfo   List of CAEndpoint_t.
 * @param infoSize      Size of CAEndpoint_t list.
 * @param devAddr       Pointer to OCDevAddr structure.
 */
void OCDiscoveryPayloadAddResourceWithEps(OCDiscoveryPayload *payload, const OCResource *res,
                                          uint16_t securePort, void *networkInfo, size_t infoSize,
                                          const OCDevAddr *devAddr);
#else
/**
 * Add resource payload with endpoint payload to discovery payload.
 *
 * @param payload       Pointer to discovery payload.
 * @param res           Pointer to OCresource structure.
 * @param securePort    Secure port number.
 * @param networkInfo   List of CAEndpoint_t.
 * @param infoSize      Size of CAEndpoint_t list.
 * @param devAddr       Pointer to OCDevAddr structure.
 * @param tcpPort       TCP port number.
 */
void OCDiscoveryPayloadAddResourceWithEps(OCDiscoveryPayload *payload, const OCResource *res,
                                          uint16_t securePort, void *networkInfo, size_t infoSize,
                                          const OCDevAddr *devAddr, uint16_t tcpPort);

/* This method will retrieve the tcp port */
OCStackResult GetTCPPortInfo(OCDevAddr *endpoint, uint16_t *port, bool secured);
#endif

/**
 * This function creates list of OCEndpointPayload structure,
 * which matches with the resource's endpointType from list of
 * CAEndpoint_t.
 *
 * @param[in] resource the resource
 * @param[in] devAddr devAddr Structure pointing to the address.
 * @param[in] networkInfo array of CAEndpoint_t
 * @param[in] infoSize size of array
 * @param[out] listHead pointer to HeadNode pointer
 * @param[out] epSize size of array(set NULL not to use it)
 * @param[out] selfEp endpoint that matches devAddr for use in anchor(set NULL not to use it)
 *
 * @return if success return pointer else NULL
 */
OCEndpointPayload* CreateEndpointPayloadList(const OCResource *resource,
    const OCDevAddr *devAddr, CAEndpoint_t *networkInfo,
    size_t infoSize, OCEndpointPayload **listHead, size_t* epSize, OCEndpointPayload** selfEp);

/*
* This function returns to destroy endpoint payload
*
*/
void OC_CALL OCEndpointPayloadDestroy(OCEndpointPayload* payload);

// Check on Accept Version option.
bool OCRequestIsOCFContentFormat(OCEntityHandlerRequest *ehRequest, bool* isOCFContentFormat);

/**
 * Finds a resource type in an OCResourceType link-list.
 *
 * @param resourceTypeList The link-list to be searched through.
 * @param resourceTypeName The key to search for.
 *
 * @return Resource type that matches the key (ie. resourceTypeName) or
 *      NULL if there is either an invalid parameter or this function was unable to find the key.
 */
OCResourceType *findResourceType(OCResourceType * resourceTypeList, const char * resourceTypeName);

void FixUpClientResponse(OCClientResponse *cr);

/**
 * Find a resource in the linked list of resources.
 *
 * @param resource Resource to be found.
 * @return Pointer to resource that was found in the linked list or NULL if the resource was not
 *         found.
 */
OCResource *findResource(OCResource *resource);

OCPayloadFormat CAToOCPayloadFormat(CAPayloadFormat_t caFormat);

void CopyEndpointToDevAddr(const CAEndpoint_t *in, OCDevAddr *out);

uint32_t GetTicks(uint32_t milliSeconds);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* OCSTACKINTERNAL_H_ */
