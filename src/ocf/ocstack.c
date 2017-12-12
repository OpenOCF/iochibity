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


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

/*
 * Expose POSIX.1-2008 base specification,
 * Refer http://pubs.opengroup.org/onlinepubs/9699919799/
 */
#define _POSIX_C_SOURCE 200809L
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include "ocstack.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <limits.h>		/* UINT_MAX */
#if EXPORT_INTERFACE
#include <stdint.h>
#endif	/* INTERFACE */

#ifdef HAVE_ARDUINO_TIME_H
#include "Time.h"
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include "coap_config.h"
#include "coap/coap_time.h"
#include "coap/pdu.h"

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifndef UINT32_MAX
#define UINT32_MAX   (0xFFFFFFFFUL)
#endif

/* #if defined(__WITH_DTLS__) || defined(__WITH_TLS__) */
/* #include "ocsecurity.h" */
/* #include "srmresourcestrings.h" */
/* #endif */

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
// Typedefs
//-----------------------------------------------------------------------------
typedef enum
{
    OC_STACK_UNINITIALIZED = 0,
    OC_STACK_INITIALIZED
} OCStackState;

#if EXPORT_INTERFACE
/**
 * Declares Stack Results & Errors.
 */
typedef enum			/* FIXME: align with CAResult_t */
{
    /** Success status code - START HERE.*/
    OC_STACK_OK = 0,                /** 203, 205*/
    OC_STACK_RESOURCE_CREATED,      /** 201*/
    OC_STACK_RESOURCE_DELETED,      /** 202*/
    OC_STACK_CONTINUE,
    OC_STACK_RESOURCE_CHANGED,      /** 204*/
    /** Success status code - END HERE.*/

    /** Error status code - START HERE.*/
    OC_STACK_INVALID_URI = 20,
    OC_STACK_INVALID_QUERY,         /** 400*/
    OC_STACK_INVALID_IP,
    OC_STACK_INVALID_PORT,
    OC_STACK_INVALID_CALLBACK,
    OC_STACK_INVALID_METHOD,

    /** Invalid parameter.*/
    OC_STACK_INVALID_PARAM,
    OC_STACK_INVALID_OBSERVE_PARAM,
    OC_STACK_NO_MEMORY,
    OC_STACK_COMM_ERROR,            /** 504*/
    /* FIXME: support CAResult_t codes */
    OC_STACK_CA_SERVER_STARTED_ALREADY,
    OC_STACK_CA_SERVER_NOT_STARTED,
    /* CA_DESTINATION_NOT_REACHABLE,   /\**< Destination is not reachable *\/
     * CA_SOCKET_OPERATION_FAILED,     /\**< Socket operation failed *\/
     * CA_SEND_FAILED,                 /\**< Send request failed *\/
     * CA_RECEIVE_FAILED,              /\**< Receive failed *\/
     * CA_DESTINATION_DISCONNECTED,    /\**< Destination is disconnected *\/
     * CA_STATUS_NOT_INITIALIZED,      /\**< Not Initialized*\/
     * CA_DTLS_AUTHENTICATION_FAILURE, /\**< Decryption error in DTLS *\/
     * CA_HANDLE_ERROR_OTHER_MODULE,   /\**< Error happens but it should be handled in other module *\/ */
    OC_STACK_TIMEOUT,
    OC_STACK_ADAPTER_NOT_ENABLED,
    OC_STACK_NOTIMPL,

    /** Resource not found.*/
    OC_STACK_NO_RESOURCE,           /** 404*/

    /** e.g: not supported method or interface.*/
    OC_STACK_RESOURCE_ERROR,
    OC_STACK_SLOW_RESOURCE,
    OC_STACK_DUPLICATE_REQUEST,

    /** Resource has no registered observers.*/
    OC_STACK_NO_OBSERVERS,
    OC_STACK_OBSERVER_NOT_FOUND,
    OC_STACK_VIRTUAL_DO_NOT_HANDLE,
    OC_STACK_INVALID_OPTION,        /** 402*/

    /** The remote reply contained malformed data.*/
    OC_STACK_MALFORMED_RESPONSE,
    OC_STACK_PERSISTENT_BUFFER_REQUIRED,
    OC_STACK_INVALID_REQUEST_HANDLE,
    OC_STACK_INVALID_DEVICE_INFO,
    OC_STACK_INVALID_JSON,

    /** Request is not authorized by Resource Server. */
    OC_STACK_UNAUTHORIZED_REQ,      /** 401*/
    OC_STACK_TOO_LARGE_REQ,         /** 413*/

    /** Error code from PDM */
    OC_STACK_PDM_IS_NOT_INITIALIZED,
    OC_STACK_DUPLICATE_UUID,
    OC_STACK_INCONSISTENT_DB,

    /**
     * Error code from OTM
     * This error is pushed from DTLS interface when handshake failure happens
     */
    OC_STACK_AUTHENTICATION_FAILURE,
    OC_STACK_NOT_ALLOWED_OXM,
    OC_STACK_CONTINUE_OPERATION,

    /** Request come from endpoint which is not mapped to the resource. */
    OC_STACK_BAD_ENDPOINT,

    /** Insert all new error codes here!.*/
#ifdef WITH_PRESENCE
    OC_STACK_PRESENCE_STOPPED = 128,
    OC_STACK_PRESENCE_TIMEOUT,
    OC_STACK_PRESENCE_DO_NOT_HANDLE,
#endif

    /** Request is denied by the user*/
    OC_STACK_USER_DENIED_REQ,
    OC_STACK_NOT_ACCEPTABLE,

    /** ERROR code from server */
    OC_STACK_FORBIDDEN_REQ,          /** 403*/
    OC_STACK_INTERNAL_SERVER_ERROR,  /** 500*/
    OC_STACK_GATEWAY_TIMEOUT,        /** 504*/
    OC_STACK_SERVICE_UNAVAILABLE,    /** 503*/

    /** ERROR in stack.*/
    OC_STACK_ERROR = 255
    /** Error status code - END HERE.*/
} OCStackResult;

/**
 * Handle to an OCDoResource invocation.
 */
typedef void * OCDoHandle;

/**
 * Handle to an OCResource object owned by the OCStack.
 */
typedef void * OCResourceHandle;

/**
 * Handle to an OCRequest object owned by the OCStack.
 */
typedef void * OCRequestHandle;

#endif	/* EXPORT_INTERFACE */


//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------
static OCStackState stackState = OC_STACK_UNINITIALIZED;

OCResource *headResource = NULL;
static OCResource *tailResource = NULL;
static OCResourceHandle platformResource = {0};
static OCResourceHandle deviceResource = {0};
static OCResourceHandle introspectionResource = {0};
static OCResourceHandle introspectionPayloadResource = {0};
static OCResourceHandle wellKnownResource = {0};
#ifdef MQ_BROKER
static OCResourceHandle brokerResource = {0};
#endif

static OCMode myStackMode;
#ifdef RA_ADAPTER
//TODO: revisit this design
static bool gRASetInfo = false;
#endif
OCDeviceEntityHandler defaultDeviceHandler;
void* defaultDeviceHandlerCallbackParameter = NULL;
static const char COAP_TCP_SCHEME[] = "coap+tcp:";
static const char COAPS_TCP_SCHEME[] = "coaps+tcp:";
static const char CORESPEC[] = "core";

// Persistent Storage callback handler for open/read/write/close/unlink
static OCPersistentStorage *g_PersistentStorageHandler = NULL;
// Number of users of OCStack, based on the successful calls to OCInit2 prior to OCStop
// The variable must not be declared static because it is also referenced by the unit test
uint32_t g_ocStackStartCount = 0;
// Number of threads currently executing OCInit2 or OCStop
volatile int32_t g_ocStackStartStopThreadCount = 0;

bool g_multicastServerStopped = false;

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG  "OIC_RI_STACK"
/* GAR:moved to //src/utils/errcheck.h */
/* #define VERIFY_SUCCESS(op, successCode) { if ((op) != (successCode)) \ */
/*             {OIC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} } */
/* #define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OIC_LOG((logLevel), \ */
/*              TAG, #arg " is NULL"); return (retVal); } } */
/* #define VERIFY_NON_NULL_NR(arg, logLevel) { if (!(arg)) { OIC_LOG((logLevel), \ */
/*              TAG, #arg " is NULL"); return; } } */
/* #define VERIFY_NON_NULL_V(arg) { if (!arg) {OIC_LOG(FATAL, TAG, #arg " is NULL");\ */
/*     goto exit;} } */

/**
 *  OCDoResource methods to dispatch the request
 */
#if EXPORT_INTERFACE
/**
 * Possible returned values from entity handler.
 */
typedef enum
{
    OC_EH_OK = 0,
    OC_EH_ERROR,
    OC_EH_SLOW,
    OC_EH_RESOURCE_CREATED = 201,
    OC_EH_RESOURCE_DELETED = 202,
    OC_EH_VALID = 203,
    OC_EH_CHANGED = 204,
    OC_EH_CONTENT = 205,
    OC_EH_BAD_REQ = 400,
    OC_EH_UNAUTHORIZED_REQ = 401,
    OC_EH_BAD_OPT = 402,
    OC_EH_FORBIDDEN = 403,
    OC_EH_RESOURCE_NOT_FOUND = 404,
    OC_EH_METHOD_NOT_ALLOWED = 405,
    OC_EH_NOT_ACCEPTABLE = 406,
    OC_EH_TOO_LARGE = 413,
    OC_EH_UNSUPPORTED_MEDIA_TYPE = 415,
    OC_EH_INTERNAL_SERVER_ERROR = 500,
    OC_EH_BAD_GATEWAY = 502,
    OC_EH_SERVICE_UNAVAILABLE = 503,
    OC_EH_RETRANSMIT_TIMEOUT = 504
} OCEntityHandlerResult;

/* typedef enum
 * {
 *     OC_REST_NOMETHOD       = 0,
 * 
 *     /\** Read.*\/
 *     OC_REST_GET            = (1 << 0),
 * 
 *     /\** Write.*\/
 *     OC_REST_PUT            = (1 << 1),
 * 
 *     /\** Update.*\/
 *     OC_REST_POST           = (1 << 2),
 * 
 *     /\** Delete.*\/
 *     OC_REST_DELETE         = (1 << 3),
 * 
 *     /\** Register observe request for most up date notifications ONLY.*\/
 *     OC_REST_OBSERVE        = (1 << 4),
 * 
 *     /\** Register observe request for all notifications, including stale notifications.*\/
 *     OC_REST_OBSERVE_ALL    = (1 << 5),
 * 
 * #ifdef WITH_PRESENCE
 *     /\** Subscribe for all presence notifications of a particular resource.*\/
 *     OC_REST_PRESENCE       = (1 << 7),
 * 
 * #endif
 *     /\** Allows OCDoResource caller to do discovery.*\/
 *     OC_REST_DISCOVER       = (1 << 8)
 * } OCMethod; */

/**
 * The type of query a request/response message is.
 */
typedef enum
{
    STACK_RES_DISCOVERY_NOFILTER = 0,
    STACK_RES_DISCOVERY_IF_FILTER,
    STACK_RES_DISCOVERY_RT_FILTER,
    STACK_DEVICE_DISCOVERY_DI_FILTER,
    STACK_DEVICE_DISCOVERY_DN_FILTER
} StackQueryTypes;

/**
 * Quality of Service attempts to abstract the guarantees provided by the underlying transport
 * protocol. The precise definitions of each quality of service level depend on the
 * implementation. In descriptions below are for the current implementation and may changed
 * over time.
 */
typedef enum
{
    /** Packet delivery is best effort.*/
    OC_LOW_QOS = 0,

    /** Packet delivery is best effort.*/
    OC_MEDIUM_QOS,

    /** Acknowledgments are used to confirm delivery.*/
    OC_HIGH_QOS,

    /** No Quality is defined, let the stack decide.*/
    OC_NA_QOS
} OCQualityOfService;
#endif	/* INTERFACE */


//TODO: we should allow the server to define this
#define MAX_OBSERVE_AGE (0x2FFFFUL)

#define MILLISECONDS_PER_SECOND   (1000)

// handle case that SCNd64 is not defined in arduino's inttypes.h
#if defined(WITH_ARDUINO) && !defined(SCNd64)
#define SCNd64 "lld"
#endif

//-----------------------------------------------------------------------------
// Internal functions
//-----------------------------------------------------------------------------
OCPayloadFormat CAToOCPayloadFormat(CAPayloadFormat_t caFormat)
{
    switch (caFormat)
    {
    case CA_FORMAT_UNDEFINED:
        return OC_FORMAT_UNDEFINED;
    case CA_FORMAT_APPLICATION_CBOR:
        return OC_FORMAT_CBOR;
    case CA_FORMAT_APPLICATION_VND_OCF_CBOR:
        return OC_FORMAT_VND_OCF_CBOR;
    default:
        return OC_FORMAT_UNSUPPORTED;
    }
}

static void OCEnterInitializer()
{
    for (;;)
    {
        int32_t initCount = oc_atomic_increment(&g_ocStackStartStopThreadCount);
        assert(initCount > 0);
        if (initCount == 1)
        {
            break;
        }
        OC_VERIFY(oc_atomic_decrement(&g_ocStackStartStopThreadCount) >= 0);
#if !defined(ARDUINO)
        // Yield execution to the thread that is holding the lock.
        sleep(0);
#else // ARDUINO
        assert(!"Not expecting initCount to go above 1 on Arduino");
        break;
#endif // ARDUINO
    }
}

static void OCLeaveInitializer()
{
    OC_VERIFY(oc_atomic_decrement(&g_ocStackStartStopThreadCount) >= 0);
}

bool checkProxyUri(OCHeaderOption *options, uint8_t numOptions)
{
    if (!options || 0 == numOptions)
    {
        OIC_LOG (INFO, TAG, "No options present");
        return false;
    }

    for (uint8_t i = 0; i < numOptions; i++)
    {
        if (options[i].protocolID == OC_COAP_ID && options[i].optionID == OC_RSRVD_PROXY_OPTION_ID)
        {
            OIC_LOG(DEBUG, TAG, "Proxy URI is present");
            return true;
        }
    }
    return false;
}


/**
 * Get the CoAP ticks after the specified number of milli-seconds.
 *
 * @param milliSeconds Milli-seconds.
 * @return CoAP ticks
 */
uint32_t GetTicks(uint32_t milliSeconds)
{
    coap_tick_t now;
    coap_ticks(&now);

    // Guard against overflow of uint32_t
    if (milliSeconds <= ((UINT32_MAX - (uint32_t)now) * MILLISECONDS_PER_SECOND) /
                             COAP_TICKS_PER_SECOND)
    {
        return now + (milliSeconds * COAP_TICKS_PER_SECOND)/MILLISECONDS_PER_SECOND;
    }
    else
    {
        return UINT32_MAX;
    }
}

void CopyEndpointToDevAddr(const CAEndpoint_t *in, OCDevAddr *out)
{
    VERIFY_NON_NULL_NR(in, FATAL);
    VERIFY_NON_NULL_NR(out, FATAL);

    out->adapter = (OCTransportAdapter)in->adapter;
    out->flags = CAToOCTransportFlags(in->flags);
    OICStrcpy(out->addr, sizeof(out->addr), in->addr);
    OICStrcpy(out->remoteId, sizeof(out->remoteId), in->remoteId);
    out->port = in->port;
    out->ifindex = in->ifindex;
#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
    /* This assert is to prevent accidental mismatch between address size macros defined in
     * RI and CA and cause crash here. */
    OC_STATIC_ASSERT(MAX_ADDR_STR_SIZE_CA == MAX_ADDR_STR_SIZE,
                                        "Address size mismatch between RI and CA");
    memcpy(out->routeData, in->routeData, sizeof(in->routeData));
#endif
}

void CopyDevAddrToEndpoint(const OCDevAddr *in, CAEndpoint_t *out)
{
    VERIFY_NON_NULL_NR(in, FATAL);
    VERIFY_NON_NULL_NR(out, FATAL);

    out->adapter = (CATransportAdapter_t)in->adapter;
    out->flags = OCToCATransportFlags(in->flags);
    OICStrcpy(out->addr, sizeof(out->addr), in->addr);
    OICStrcpy(out->remoteId, sizeof(out->remoteId), in->remoteId);
#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
    /* This assert is to prevent accidental mismatch between address size macros defined in
     * RI and CA and cause crash here. */
    OC_STATIC_ASSERT(MAX_ADDR_STR_SIZE_CA == MAX_ADDR_STR_SIZE,
                                        "Address size mismatch between RI and CA");
    memcpy(out->routeData, in->routeData, sizeof(in->routeData));
#endif
    out->port = in->port;
    out->ifindex = in->ifindex;
}

void FixUpClientResponse(OCClientResponse *cr)
{
    VERIFY_NON_NULL_NR(cr, FATAL);

    cr->addr = &cr->devAddr;
    cr->connType = (OCConnectivityType)
        ((cr->devAddr.adapter << CT_ADAPTER_SHIFT) | (cr->devAddr.flags & CT_MASK_FLAGS));
}

/**
 * Ensure the accept header option is set appropriatly before sending the requests and routing
 * header option is updated with destination.
 *
 * @param object CA remote endpoint.
 * @param requestInfo CA request info.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult OCSendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL(object, FATAL, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(requestInfo, FATAL, OC_STACK_INVALID_PARAM);
    OIC_TRACE_BEGIN(%s:OCSendRequest, TAG);

#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
    OCStackResult rmResult = RMAddInfo(object->routeData, requestInfo, true, NULL);
    if (OC_STACK_OK != rmResult)
    {
        OIC_LOG(ERROR, TAG, "Add destination option failed");
        OIC_TRACE_END();
        return rmResult;
    }
#endif

    // TODO: We might need to remove acceptFormat and acceptVersion fields in requestinfo->info
    // at a later stage to avoid duplication.
    uint16_t acceptVersion = OCF_VERSION_1_0_0;
    CAPayloadFormat_t acceptFormat = CA_FORMAT_APPLICATION_VND_OCF_CBOR;
    // Check settings of version option and content format.
    if (requestInfo->info.numOptions > 0 && requestInfo->info.options)
    {
        for (uint8_t i = 0; i < requestInfo->info.numOptions; i++)
        {
            if (OCF_ACCEPT_CONTENT_FORMAT_VERSION == requestInfo->info.options[i].optionID
                    && sizeof(uint16_t) == requestInfo->info.options[i].optionLength)
            {
                acceptVersion = (requestInfo->info.options[i].optionData[1] << 8)
                        + requestInfo->info.options[i].optionData[0];
            }
            else if (COAP_OPTION_ACCEPT == requestInfo->info.options[i].optionID)
            {
                if (sizeof(uint8_t) == requestInfo->info.options[i].optionLength)
                {
                    acceptFormat = CAConvertFormat(
                            *(uint8_t*) requestInfo->info.options[i].optionData);
                }
                else if (sizeof(uint16_t) == requestInfo->info.options[i].optionLength)
                {
                    acceptFormat = CAConvertFormat(
                            (requestInfo->info.options[i].optionData[1] << 8)
                                    + requestInfo->info.options[i].optionData[0]);
                }
                else
                {
                    acceptFormat = CA_FORMAT_UNSUPPORTED;
                    OIC_LOG_V(DEBUG, TAG, "option has an unsupported format");
                }
            }
        }
    }

    if (CA_FORMAT_UNDEFINED == requestInfo->info.acceptFormat)
    {
        requestInfo->info.acceptFormat = acceptFormat;
    }
    if (CA_FORMAT_APPLICATION_CBOR == requestInfo->info.acceptFormat && acceptVersion)
    {
        acceptVersion = 0;
    }
    requestInfo->info.acceptVersion = acceptVersion;

    CAResult_t result = CASendRequest(object, requestInfo);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TAG, "CASendRequest failed with CA error %u", result);
        OIC_TRACE_END();
        return CAResultToOCResult(result);
    }
    OIC_TRACE_END();
    return OC_STACK_OK;
}
//-----------------------------------------------------------------------------
// Internal API function
//-----------------------------------------------------------------------------

/* This internal function is called to update the stack with the status of */
/* observers and communication failures */
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
OCStackResult OCStackFeedBack(CAToken_t token, uint8_t tokenLength, uint8_t status)
{
    OCStackResult result = OC_STACK_ERROR;
    OCEntityHandlerRequest ehRequest = {0};
    OCResource *resource = NULL;
    ResourceObserver *observer = NULL;

    if (false == GetObserverFromResourceList(&resource, &observer, token, tokenLength))
    {
        OIC_LOG(DEBUG, TAG, "Observer is not found.");
        return OC_STACK_OBSERVER_NOT_FOUND;
    }
    assert(resource);
    assert(observer);

    switch(status)
    {
    case OC_OBSERVER_NOT_INTERESTED:
        OIC_LOG(DEBUG, TAG, "observer not interested in our notifications");
        result = FormOCEntityHandlerRequest(&ehRequest,
                                            (OCRequestHandle)NULL,
                                            OC_REST_NOMETHOD,
                                            &observer->devAddr,
                                            (OCResourceHandle)NULL,
                                            NULL,
                                            PAYLOAD_TYPE_REPRESENTATION, OC_FORMAT_CBOR,
                                            NULL, 0, 0, NULL,
                                            OC_OBSERVE_DEREGISTER,
                                            observer->observeId,
                                            0);
        if (result != OC_STACK_OK)
        {
            return result;
        }

        if (resource->entityHandler)
        {
            resource->entityHandler(OC_OBSERVE_FLAG, &ehRequest,
                                    resource->entityHandlerCallbackParam);
        }

        DeleteObserverUsingToken(resource, token, tokenLength);
        break;

    case OC_OBSERVER_STILL_INTERESTED:
        OIC_LOG(DEBUG, TAG, "observer still interested, reset the failedCount");
        observer->forceHighQos = 0;
        observer->failedCommCount = 0;
        result = OC_STACK_OK;
        break;

    case OC_OBSERVER_FAILED_COMM:
        OIC_LOG(DEBUG, TAG, "observer is unreachable");
        if (MAX_OBSERVER_FAILED_COMM <= observer->failedCommCount)
        {
            result = FormOCEntityHandlerRequest(&ehRequest,
                                                (OCRequestHandle)NULL,
                                                OC_REST_NOMETHOD,
                                                &observer->devAddr,
                                                (OCResourceHandle)NULL,
                                                NULL,
                                                PAYLOAD_TYPE_REPRESENTATION, OC_FORMAT_CBOR,
                                                NULL, 0, 0, NULL,
                                                OC_OBSERVE_DEREGISTER,
                                                observer->observeId,
                                                0);
            if (result != OC_STACK_OK)
            {
                return OC_STACK_ERROR;
            }

            if (resource->entityHandler)
            {
                resource->entityHandler(OC_OBSERVE_FLAG, &ehRequest,
                                        resource->entityHandlerCallbackParam);
            }

            DeleteObserverUsingToken(resource, token, tokenLength);
        }
        else
        {
            observer->failedCommCount++;
            observer->forceHighQos = 1;
            OIC_LOG_V(DEBUG, TAG, "Failure counter for this observer is %d",
                      observer->failedCommCount);
            result = OC_STACK_CONTINUE;
        }
        break;

    default:
        OIC_LOG(ERROR, TAG, "Unknown status");
        result = OC_STACK_ERROR;
        break;
    }
    return result;
}

/**
 * Convert CAResponseResult_t to OCStackResult.
 *
 * @param caCode CAResponseResult_t code.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult CAResponseToOCStackResult(CAResponseResult_t caCode)
{
    OCStackResult ret = OC_STACK_ERROR;
    switch(caCode)
    {
        case CA_CREATED:
            ret = OC_STACK_RESOURCE_CREATED;
            break;
        case CA_DELETED:
            ret = OC_STACK_RESOURCE_DELETED;
            break;
        case CA_CHANGED:
            ret = OC_STACK_RESOURCE_CHANGED;
            break;
        case CA_CONTENT:
        case CA_VALID:
            ret = OC_STACK_OK;
            break;
        case CA_BAD_REQ:
            ret = OC_STACK_INVALID_QUERY;
            break;
        case CA_UNAUTHORIZED_REQ:
            ret = OC_STACK_UNAUTHORIZED_REQ;
            break;
        case CA_BAD_OPT:
            ret = OC_STACK_INVALID_OPTION;
            break;
        case CA_NOT_FOUND:
            ret = OC_STACK_NO_RESOURCE;
            break;
        case CA_RETRANSMIT_TIMEOUT:
            ret = OC_STACK_GATEWAY_TIMEOUT;
            break;
        case CA_REQUEST_ENTITY_TOO_LARGE:
            ret = OC_STACK_TOO_LARGE_REQ;
            break;
        case CA_NOT_ACCEPTABLE:
            ret = OC_STACK_NOT_ACCEPTABLE;
            break;
        case CA_FORBIDDEN_REQ:
            ret = OC_STACK_FORBIDDEN_REQ;
            break;
        case CA_INTERNAL_SERVER_ERROR:
            ret = OC_STACK_INTERNAL_SERVER_ERROR;
            break;
        case CA_SERVICE_UNAVAILABLE:
            ret = OC_STACK_SERVICE_UNAVAILABLE;
            break;
        default:
            break;
    }
    return ret;
}

/**
 * Convert OCStackResult to CAResponseResult_t.
 *
 * @param ocCode OCStackResult code.
 * @param method OCMethod method the return code replies to.
 * @return ::CA_CONTENT on OK, some other value upon failure.
 */
static CAResponseResult_t OCToCAStackResult(OCStackResult ocCode, OCMethod method)
{
    CAResponseResult_t ret = CA_INTERNAL_SERVER_ERROR;

    switch(ocCode)
    {
        case OC_STACK_OK:
           switch (method)
           {
               case OC_REST_PUT:
               case OC_REST_POST:
                   // This Response Code is like HTTP 204 "No Content" but only used in
                   // response to POST and PUT requests.
                   ret = CA_CHANGED;
                   break;
               case OC_REST_GET:
                   // This Response Code is like HTTP 200 "OK" but only used in response to
                   // GET requests.
                   ret = CA_CONTENT;
                   break;
               case OC_REST_DELETE:
                   // This Response Code is like HTTP 200 "OK" but only used in response to
                   // DELETE requests.
                   ret = CA_DELETED;
                   break;
               default:
                   // This should not happen but,
                   // give it a value just in case but output an error
                   ret = CA_CONTENT;
                   OIC_LOG_V(ERROR, TAG, "Unexpected OC_STACK_OK return code for method [%d].",
                            method);
            }
            break;
        case OC_STACK_RESOURCE_CREATED:
            ret = CA_CREATED;
            break;
        case OC_STACK_RESOURCE_DELETED:
            ret = CA_DELETED;
            break;
        case OC_STACK_RESOURCE_CHANGED:
            ret = CA_CHANGED;
            break;
        case OC_STACK_INVALID_QUERY:
            ret = CA_BAD_REQ;
            break;
        case OC_STACK_INVALID_OPTION:
            ret = CA_BAD_OPT;
            break;
        case OC_STACK_NO_RESOURCE:
            ret = CA_NOT_FOUND;
            break;
        case OC_STACK_COMM_ERROR:
            ret = CA_RETRANSMIT_TIMEOUT;
            break;
        case OC_STACK_GATEWAY_TIMEOUT:
            ret = CA_RETRANSMIT_TIMEOUT;
            break;
        case OC_STACK_NOT_ACCEPTABLE:
            ret = CA_NOT_ACCEPTABLE;
            break;
        case OC_STACK_UNAUTHORIZED_REQ:
            ret = CA_UNAUTHORIZED_REQ;
            break;
        case OC_STACK_FORBIDDEN_REQ:
            ret = CA_FORBIDDEN_REQ;
            break;
        case OC_STACK_INTERNAL_SERVER_ERROR:
            ret = CA_INTERNAL_SERVER_ERROR;
            break;
        case OC_STACK_BAD_ENDPOINT:
            ret = CA_BAD_REQ;
            break;
        case OC_STACK_SERVICE_UNAVAILABLE:
            ret = CA_SERVICE_UNAVAILABLE;
            break;
        default:
            break;
    }
    return ret;
}


/**
 * Convert OCTransportFlags_t to CATransportModifiers_t.
 *
 * @param ocConType OCTransportFlags_t input.
 * @return CATransportFlags
 */
LOCAL CATransportFlags_t OCToCATransportFlags(OCTransportFlags ocFlags)
{
    CATransportFlags_t caFlags = (CATransportFlags_t)ocFlags;

    // supply default behavior.
    if ((caFlags & (CA_IPV6|CA_IPV4)) == 0)
    {
        caFlags = (CATransportFlags_t)(caFlags|CA_IPV6|CA_IPV4);
    }
    if ((caFlags & OC_MASK_SCOPE) == 0)
    {
        caFlags = (CATransportFlags_t)(caFlags|OC_SCOPE_LINK);
    }
    return caFlags;
}

/**
 * Convert CATransportFlags_t to OCTransportModifiers_t.
 *
 * @param caConType CATransportFlags_t input.
 * @return OCTransportFlags
 */
LOCAL OCTransportFlags CAToOCTransportFlags(CATransportFlags_t caFlags)
{
    return (OCTransportFlags)caFlags;
}

OCStackResult OC_CALL OCEncodeAddressForRFC6874(char *outputAddress,
                                                size_t outputSize,
                                                const char *inputAddress)
{
    VERIFY_NON_NULL(inputAddress,  FATAL, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(outputAddress, FATAL, OC_STACK_INVALID_PARAM);

    size_t inputLength = strnlen(inputAddress, outputSize);

    // inputSize includes the null terminator
    size_t inputSize = inputLength + 1;

    if (inputSize > outputSize)
    {
        OIC_LOG_V(ERROR, TAG,
                  "OCEncodeAddressForRFC6874 failed: "
                  "outputSize (%" PRIuPTR ") < inputSize (%" PRIuPTR ")",
                  outputSize, inputSize);

        return OC_STACK_ERROR;
    }

    char* percentChar = strchr(inputAddress, '%');

    // If there is no '%' character, then no change is required to the string.
    if (NULL == percentChar)
    {
        OICStrcpy(outputAddress, outputSize, inputAddress);
        return OC_STACK_OK;
    }

    const char* addressPart = &inputAddress[0];
    const char* scopeIdPart = percentChar + 1;

    // Sanity check to make sure this string doesn't have more '%' characters
    if (NULL != strchr(scopeIdPart, '%'))
    {
        return OC_STACK_ERROR;
    }

    // If no string follows the first '%', then the input was invalid.
    if (scopeIdPart[0] == '\0')
    {
        OIC_LOG(ERROR, TAG, "OCEncodeAddressForRFC6874 failed: Invalid input string: no scope ID!");
        return OC_STACK_ERROR;
    }

    // Check to see if the string is already encoded
    if ((scopeIdPart[0] == '2') && (scopeIdPart[1] == '5'))
    {
        OIC_LOG(ERROR, TAG, "OCEncodeAddressForRFC6874 failed: Input string is already encoded");
        return OC_STACK_ERROR;
    }

    // Fail if we don't have room for encoded string's two additional chars
    if (outputSize < (inputSize + 2))
    {
        OIC_LOG(ERROR, TAG, "OCEncodeAddressForRFC6874 failed: encoded output will not fit!");
        return OC_STACK_ERROR;
    }

    // Restore the null terminator with an escaped '%' character, per RFC 6874
    OICStrcpy(outputAddress, scopeIdPart - addressPart, addressPart);
    OICStrcat(outputAddress, outputSize, "%25");
    OICStrcat(outputAddress, outputSize, scopeIdPart);

    return OC_STACK_OK;
}

OCStackResult OC_CALL OCDecodeAddressForRFC6874(char *outputAddress,
                                                size_t outputSize,
                                                const char *inputAddress,
                                                const char *end)
{
    VERIFY_NON_NULL(inputAddress,  FATAL, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(outputAddress, FATAL, OC_STACK_INVALID_PARAM);

    if (NULL == end)
    {
        end = inputAddress + strlen(inputAddress);
    }
    size_t inputLength = end - inputAddress;

    const char *percent = strchr(inputAddress, '%');
    if (!percent || (percent > end))
    {
        OICStrcpyPartial(outputAddress, outputSize, inputAddress, inputLength);
    }
    else
    {
        if (percent[1] != '2' || percent[2] != '5')
        {
            return OC_STACK_INVALID_URI;
        }

        size_t addrlen = percent - inputAddress + 1;
        OICStrcpyPartial(outputAddress, outputSize, inputAddress, addrlen);
        OICStrcpyPartial(outputAddress + addrlen, outputSize - addrlen,
                         percent + 3, end - percent - 3);
    }

    return OC_STACK_OK;
}

OCStackResult HandleBatchResponse(char *requestUri, OCRepPayload **payload)
{
    if (requestUri && *payload)
    {
        char *interfaceName = NULL;
        char *rtTypeName = NULL;
        char *uriQuery = NULL;
        char *uriWithoutQuery = NULL;
        if (OC_STACK_OK == getQueryFromUri(requestUri, &uriQuery, &uriWithoutQuery))
        {
            if (OC_STACK_OK == ExtractFiltersFromQuery(uriQuery, &interfaceName, &rtTypeName))
            {
                if (interfaceName && (0 == strcmp(OC_RSRVD_INTERFACE_BATCH, interfaceName)))
                {
                    char *uri = (*payload)->uri;
                    if (uri && 0 != strcmp(uriWithoutQuery, uri))
                    {
                        OCRepPayload *newPayload = OCRepPayloadCreate();
                        if (newPayload)
                        {
                            OCRepPayloadSetUri(newPayload, uri);
                            newPayload->next = *payload;
                            *payload = newPayload;
                        }
                    }
                }
            }
        }

        OICFree(interfaceName);
        OICFree(rtTypeName);
        OICFree(uriQuery);
        OICFree(uriWithoutQuery);
        return OC_STACK_OK;
    }
    return OC_STACK_INVALID_PARAM;
}

/**
 * Map zoneId to endpoint address which scope is ipv6 link-local.
 * @param payload Discovery payload which has Endpoint information.
 * @param ifindex index which indicate network interface.
 */
#if defined (IP_ADAPTER)
static OCStackResult OCMapZoneIdToLinkLocalEndpoint(OCDiscoveryPayload *payload, uint32_t ifindex)
{
    if (!payload)
    {
        OIC_LOG(ERROR, TAG, "Given argument payload is NULL!!");
        return OC_STACK_INVALID_PARAM;
    }

    OCResourcePayload *curRes = payload->resources;

    while (curRes != NULL)
    {
        OCEndpointPayload* eps = curRes->eps;

        while (eps != NULL)
        {
            if (eps->family & OC_IP_USE_V6)
            {
                CATransportFlags_t scopeLevel;
                if (CA_STATUS_OK == CAGetIpv6AddrScope(eps->addr, &scopeLevel))
                {
                    if (CA_SCOPE_LINK == scopeLevel)
                    {
                        char *zoneId = NULL;
                        if (OC_STACK_OK == OCGetLinkLocalZoneId(ifindex, &zoneId))
                        {
                            assert(zoneId != NULL);
                            // put zoneId to end of addr
                            OICStrcat(eps->addr, OC_MAX_ADDR_STR_SIZE, "%25");
                            OICStrcat(eps->addr, OC_MAX_ADDR_STR_SIZE, zoneId);
                            OICFree(zoneId);
                        }
                        else
                        {
                            OIC_LOG(ERROR, TAG, "failed at parse zone-id for link-local address");
                            return OC_STACK_ERROR;
                        }
                    }
                }
                else
                {
                    OIC_LOG(ERROR, TAG, "failed at parse ipv6 scope level");
                    return OC_STACK_ERROR;
                }
            }
            eps = eps->next;
        }
        curRes = curRes->next;
    }

    return OC_STACK_OK;
}
#endif

void OC_CALL OCHandleResponse(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_TRACE_MARK(%s:OCHandleResponse:%s, TAG, responseInfo->info.resourceUri);

    OCStackApplicationResult appFeedback = OC_STACK_DELETE_TRANSACTION;

#ifdef WITH_PRESENCE
    if(responseInfo->info.resourceUri &&
        strcmp(responseInfo->info.resourceUri, OC_RSRVD_PRESENCE_URI) == 0)
    {
        HandlePresenceResponse(endPoint, responseInfo);
        return;
    }
#endif

    ClientCB *cbNode = GetClientCBUsingToken(responseInfo->info.token,
                                             responseInfo->info.tokenLength);
    if(cbNode)
    {
        OIC_LOG(INFO, TAG, "There is a cbNode associated with the response token");

        // check obs header option
        bool obsHeaderOpt = false;
        CAHeaderOption_t *options = responseInfo->info.options;
        for (uint8_t i = 0; i < responseInfo->info.numOptions; i++)
        {
            if (options && (options[i].optionID == COAP_OPTION_OBSERVE))
            {
                obsHeaderOpt = true;
                break;
            }
        }

        if(responseInfo->result == CA_EMPTY)
        {
            OIC_LOG(INFO, TAG, "Receiving A ACK/RESET for this token");
            // We do not have a case for the client to receive a RESET
            if(responseInfo->info.type == CA_MSG_ACKNOWLEDGE)
            {
                //This is the case of receiving an ACK on a request to a slow resource!
                OIC_LOG(INFO, TAG, "This is a pure ACK");
                //TODO: should we inform the client
                //      app that at least the request was received at the server?
            }
        }
        else if (CA_REQUEST_ENTITY_INCOMPLETE == responseInfo->result)
        {
            OIC_LOG(INFO, TAG, "Wait for block transfer finishing.");
            return;
        }
        else if (CA_RETRANSMIT_TIMEOUT == responseInfo->result
                || CA_NOT_ACCEPTABLE == responseInfo->result)
        {
            if (CA_RETRANSMIT_TIMEOUT == responseInfo->result)
            {
                OIC_LOG(INFO, TAG, "Receiving A Timeout for this token");
                OIC_LOG(INFO, TAG, "Calling into application address space");
            }
            else
            {
                OIC_LOG(INFO, TAG, "Server doesn't support the requested payload format");
                OIC_LOG(INFO, TAG, "Calling into application address space");
            }

            // Check if it is the case that a OCF client connects to a OIC server.
            // If so, reissue request using OIC format
            if (CA_NOT_ACCEPTABLE == responseInfo->result &&
                CA_FORMAT_UNDEFINED == responseInfo->info.payloadFormat)
            {
		OIC_LOG(DEBUG, TAG, "Response result NOT_ACCEPTABLE, format UNDEFINED");
                CARequestInfo_t requestInfo = { .method = CA_GET };

                switch (cbNode->method)
                {
                    case OC_REST_GET:
                    case OC_REST_OBSERVE:
                    case OC_REST_OBSERVE_ALL:
                        requestInfo.method = CA_GET;
                        break;
                    case OC_REST_PUT:
                        requestInfo.method = CA_PUT;
                        break;
                    case OC_REST_POST:
                        requestInfo.method = CA_POST;
                        break;
                    case OC_REST_DELETE:
                        requestInfo.method = CA_DELETE;
                        break;
                    case OC_REST_DISCOVER:
#ifdef WITH_PRESENCE
                    case OC_REST_PRESENCE:
#endif
                        // OC_REST_DISCOVER: CA_DISCOVER will become GET
                        // OC_REST_PRESENCE: Since "presence" is a stack layer only implementation.
                        //                   replacing method type with GET.
                        requestInfo.method = CA_GET;
                        break;
                    default:
			/* GAR: OC_REST_NOMETHOD???  */
                        goto proceed;
                }

                CAInfo_t requestData = {.type = cbNode->type};
                requestData.tokenLength = cbNode->tokenLength;
                requestData.token = (CAToken_t) OICMalloc(requestData.tokenLength);
                if (!requestData.token)
                {
                    OIC_LOG(ERROR, TAG, "Out of memory");
                    goto proceed;
                }
                memcpy(requestData.token, cbNode->token, requestData.tokenLength);

                if (!cbNode->options || !cbNode->numOptions)
                {
                    OIC_LOG (INFO, TAG, "No options present in cbNode");
                    requestData.options = NULL;
                    requestData.numOptions = 0;
                }
                else
                {
                    requestData.options = (CAHeaderOption_t *) OICCalloc(cbNode->numOptions,
                            sizeof(CAHeaderOption_t));
                    if (!requestData.options)
                    {
                        OIC_LOG(ERROR, TAG, "Out of memory");
                        OICFree(requestData.token);
                        goto proceed;
                    }
                    memcpy(requestData.options, cbNode->options,
                            sizeof(CAHeaderOption_t) * cbNode->numOptions);

                    requestData.numOptions = cbNode->numOptions;
                }
                if (!cbNode->payload || !cbNode->payloadSize)
                {
                    OIC_LOG (INFO, TAG, "No payload present in cbNode");
                    requestData.payload = NULL;
                    requestData.payloadSize = 0;
                }
                else
                {
                    requestData.payload = (CAPayload_t) OICCalloc(1, cbNode->payloadSize);
                    if (!requestData.payload)
                    {
                        OIC_LOG(ERROR, TAG, "out of memory");
                        OICFree(requestData.token);
                        OICFree(requestData.options);
                        goto proceed;
                    }
                    memcpy(requestData.payload, cbNode->payload, cbNode->payloadSize);
                    requestData.payloadSize = cbNode->payloadSize;
                }
                requestData.payloadFormat = CA_FORMAT_APPLICATION_CBOR;
                requestData.acceptFormat = CA_FORMAT_APPLICATION_CBOR; //?
                requestData.resourceUri = OICStrdup(cbNode->requestUri);

                requestInfo.info = requestData;

                // send request
                OCStackResult result = OCSendRequest(endPoint, &requestInfo);
                if (OC_STACK_OK == result)
                {
		    OIC_LOG_V(DEBUG, TAG, "Resending request with Accept=CBOR (OIC)");
                    return;
                }
                else
                {
                    OIC_LOG(ERROR, TAG, "Re-send request failed");
                    OICFree(requestData.token);
                    OICFree(requestData.options);
                    OICFree(requestData.payload);
                }
            }

        proceed:;
            OCClientResponse *response = (OCClientResponse *) OICCalloc(1, sizeof(*response));
            if (!response)
            {
                OIC_LOG_V(ERROR, TAG, "[%d] %s: Allocating memory for OCClientResponse failed",
			__LINE__, __func__);
                return;
            }

            response->devAddr.adapter = OC_DEFAULT_ADAPTER;
            CopyEndpointToDevAddr(endPoint, &response->devAddr);
            FixUpClientResponse(response);
            response->resourceUri = responseInfo->info.resourceUri;
            memcpy(response->identity.id, responseInfo->info.identity.id,
                    sizeof(response->identity.id));
            response->identity.id_length = responseInfo->info.identity.id_length;

            response->result = CAResponseToOCStackResult(responseInfo->result);
            cbNode->callBack(cbNode->context,
                    cbNode->handle, response);
            DeleteClientCB(cbNode);
            OICFree(response);
        } /* result != EMPTY, ENTITY_INCOMPLETE, RETRANSMIT_TIMEOUT, NOT_ACCEPTABLE */
        else if ((cbNode->method == OC_REST_OBSERVE || cbNode->method == OC_REST_OBSERVE_ALL)
                && (responseInfo->result == CA_CONTENT) && !obsHeaderOpt)
	    /* response = CONTENT means HTTP 200 "OK" but only used in response to GET requests. */
        {
            OCClientResponse *response = NULL;

            response = (OCClientResponse *)OICCalloc(1, sizeof(*response));
            if (!response)
            {
                OIC_LOG_V(ERROR, TAG, "[%d] %s: Allocating memory for OCClientResponse failed",
			__LINE__, __func__);
                return;
            }

            response->devAddr.adapter = OC_DEFAULT_ADAPTER;
            response->sequenceNumber = MAX_SEQUENCE_NUMBER + 1;
            CopyEndpointToDevAddr(endPoint, &response->devAddr);
            FixUpClientResponse(response);
            response->resourceUri = responseInfo->info.resourceUri;
            memcpy(response->identity.id, responseInfo->info.identity.id,
                                    sizeof (response->identity.id));
            response->identity.id_length = responseInfo->info.identity.id_length;
            response->result = OC_STACK_OK;

            OIC_LOG(DEBUG, TAG, "This is response of observer cancel or observer request fail");

            cbNode->callBack(cbNode->context,
                             cbNode->handle,
                             response);
            DeleteClientCB(cbNode);
            OICFree(response);
        } /* OBSERVE or OBSERVE_ALL */
        else
        {
            OIC_LOG(INFO, TAG, "This is a regular response, A client call back is found");

            OCClientResponse *response = NULL;
            OCPayloadType type = PAYLOAD_TYPE_INVALID;

            response = (OCClientResponse *)OICCalloc(1, sizeof(*response));
            if (!response)
            {
                OIC_LOG_V(ERROR, TAG, "[%d] %s: Allocating memory for response failed",
			__LINE__, __func__);
                return;
            }

            response->devAddr.adapter = OC_DEFAULT_ADAPTER;
            response->sequenceNumber = MAX_SEQUENCE_NUMBER + 1;
            CopyEndpointToDevAddr(endPoint, &response->devAddr);
            FixUpClientResponse(response);
	    response->resourceUri = OICStrdup(responseInfo->info.resourceUri);
            memcpy(response->identity.id, responseInfo->info.identity.id,
                                                sizeof (response->identity.id));
            response->identity.id_length = responseInfo->info.identity.id_length;

            response->result = CAResponseToOCStackResult(responseInfo->result);

            if(responseInfo->info.payload &&
               responseInfo->info.payloadSize)
            {   /* infer payload type from uri */
                if (SRMIsSecurityResourceURI(cbNode->requestUri))
                {
                    type = PAYLOAD_TYPE_SECURITY;
                }
                else if (cbNode->method == OC_REST_DISCOVER)
                {
                    if (strncmp(OC_RSRVD_WELL_KNOWN_URI,cbNode->requestUri,
                                sizeof(OC_RSRVD_WELL_KNOWN_URI) - 1) == 0)
                    {
                        type = PAYLOAD_TYPE_DISCOVERY;
                    }
#ifdef WITH_MQ
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_WELL_KNOWN_MQ_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_DISCOVERY;
                    }
#endif
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_DEVICE_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_PLATFORM_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
#ifdef ROUTING_GATEWAY
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_GATEWAY_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
#endif
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_RD_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
#ifdef TCP_ADAPTER
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_KEEPALIVE_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
#endif
                    else
                    {
                        OIC_LOG_V(ERROR, TAG, "Unknown Payload type in Discovery: %d %s",
                                cbNode->method, cbNode->requestUri);
                        OICFree(response);
                        return;
                    }
                }
                else if (cbNode->method == OC_REST_GET ||
                         cbNode->method == OC_REST_PUT ||
                         cbNode->method == OC_REST_POST ||
                         cbNode->method == OC_REST_OBSERVE ||
                         cbNode->method == OC_REST_OBSERVE_ALL ||
                         cbNode->method == OC_REST_DELETE)
                {
                    if (cbNode->requestUri)
                    {
                        if (0 == strcmp(OC_RSRVD_PLATFORM_URI, cbNode->requestUri))
                        {
                            type = PAYLOAD_TYPE_REPRESENTATION;
                        }
                        else if (0 == strcmp(OC_RSRVD_DEVICE_URI, cbNode->requestUri))
                        {
                            type = PAYLOAD_TYPE_REPRESENTATION;
                        }
                        if (type == PAYLOAD_TYPE_INVALID)
                        {
                            if (responseInfo->info.payloadFormat == CA_FORMAT_UNDEFINED)
                            {
                                OIC_LOG_V(INFO, TAG, "Invalid payload type; assuming PAYLOAD_TYPE_DIAGNOSTIC: %d %s",
                                        cbNode->method, cbNode->requestUri);
                                type = PAYLOAD_TYPE_DIAGNOSTIC;
                            }
                            else
                            {
                                OIC_LOG_V(INFO, TAG, "Invalid payload type; assuming PAYLOAD_TYPE_REPRESENTATION: %d %s",
                                        cbNode->method, cbNode->requestUri);
                                type = PAYLOAD_TYPE_REPRESENTATION;
                            }
                        }
                    }
                    else
                    {
                        OIC_LOG(INFO, TAG, "No Request URI, PROXY URI");
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
                }
                else
                {
                    OIC_LOG_V(ERROR, TAG, "Unknown method: %d %s",
                            cbNode->method, cbNode->requestUri);
                    OICFree(response);
                    return;
                }

                // In case of error, still want application to receive the error message.
                if (OCResultToSuccess(response->result) || PAYLOAD_TYPE_REPRESENTATION == type ||
                        PAYLOAD_TYPE_DIAGNOSTIC == type)
                {
                    if (OC_STACK_OK != OCParsePayload(&response->payload,
                            CAToOCPayloadFormat(responseInfo->info.payloadFormat),
                            type,
                            responseInfo->info.payload,
                            responseInfo->info.payloadSize))
                    {
                        OIC_LOG(ERROR, TAG, "Error: OCParsePayload");
                        OCPayloadDestroy(response->payload);
                        OICFree(response);
                        return;
                    }

                    // Check endpoints has link-local ipv6 address.
                    // if there is, map zone-id which parsed from ifindex
#if defined (IP_ADAPTER) && !defined (WITH_ARDUINO)
                    if (PAYLOAD_TYPE_DISCOVERY == response->payload->type)
                    {
                        OCDiscoveryPayload *disPayload = (OCDiscoveryPayload*)(response->payload);
                        if (OC_STACK_OK !=
                            OCMapZoneIdToLinkLocalEndpoint(disPayload, response->devAddr.ifindex))
                        {
                            OIC_LOG(ERROR, TAG, "failed at map zone-id for link-local address");
                            OCPayloadDestroy(response->payload);
                            OICFree(response);
                            return;
                        }
                    }
#endif
                }
                else
                {
                    response->resourceUri = OICStrdup(cbNode->requestUri);
                }
            }

            response->numRcvdVendorSpecificHeaderOptions = 0;
            if((responseInfo->info.numOptions > 0) && (responseInfo->info.options != NULL))
            {
                uint8_t start = 0;
                //First option always with option ID is COAP_OPTION_OBSERVE if it is available.
                if(responseInfo->info.options[0].optionID == COAP_OPTION_OBSERVE)
                {
                    size_t i;
                    uint32_t observationOption;
                    uint8_t* optionData = (uint8_t*)responseInfo->info.options[0].optionData;
                    for (observationOption=0, i=0;
                            i<sizeof(uint32_t) && i<responseInfo->info.options[0].optionLength;
                            i++)
                    {
                        observationOption =
                            (observationOption << 8) | optionData[i];
                    }
                    response->sequenceNumber = observationOption;
                    response->numRcvdVendorSpecificHeaderOptions = responseInfo->info.numOptions - 1;
                    start = 1;
                }
                else
                {
                    response->numRcvdVendorSpecificHeaderOptions = responseInfo->info.numOptions;
                }

                if(response->numRcvdVendorSpecificHeaderOptions > MAX_HEADER_OPTIONS)
                {
                    OIC_LOG(ERROR, TAG, "#header options are more than MAX_HEADER_OPTIONS");
                    OCPayloadDestroy(response->payload);
                    OICFree(response);
                    return;
                }

                for (uint8_t i = start; i < responseInfo->info.numOptions; i++)
                {
                    memcpy (&(response->rcvdVendorSpecificHeaderOptions[i - start]),
                            &(responseInfo->info.options[i]), sizeof(OCHeaderOption));
                }
            }

            if (cbNode->method == OC_REST_OBSERVE &&
                response->sequenceNumber > OC_OFFSET_SEQUENCE_NUMBER &&
                cbNode->sequenceNumber <=  MAX_SEQUENCE_NUMBER &&
                response->sequenceNumber <= cbNode->sequenceNumber)
            {
                OIC_LOG_V(INFO, TAG, "Received stale notification. Number :%d",
                                                 response->sequenceNumber);
            }
            else
            {
#ifdef RD_CLIENT
                if (cbNode->requestUri)
                {
                    // if request uri is '/oic/rd', update ins value of resource.
                    char *targetUri = strstr(cbNode->requestUri, OC_RSRVD_RD_URI);
                    if (targetUri)
                    {
                        OCUpdateResourceInsWithResponse(cbNode->requestUri, response);
                    }
                }
#endif
                // set remoteID(device ID) into OCClientResponse callback parameter
                if (OC_REST_DISCOVER == cbNode->method && PAYLOAD_TYPE_DISCOVERY == type)
                {
                    OCDiscoveryPayload *payload = (OCDiscoveryPayload*) response->payload;
                    // Payload can be empty in case of error message.
                    if (payload && payload->sid)
                    {
                        OICStrcpy(response->devAddr.remoteId, sizeof(response->devAddr.remoteId),
                                  payload->sid);
                        OIC_LOG_V(INFO, TAG, "[%d] %s: Device ID of response : %s",
				  __LINE__, __func__,
                                  response->devAddr.remoteId);

#if defined(TCP_ADAPTER) && defined(WITH_CLOUD)
                        CAConnectUserPref_t connPrefer = CA_USER_PREF_CLOUD;
                        CAResult_t ret = CAUtilCMGetConnectionUserConfig(&connPrefer);
                        if (ret == CA_STATUS_OK && connPrefer != CA_USER_PREF_CLOUD)
                        {
                            OCCMDiscoveryResource(response);
                        }
#endif
                    }
                } /* FIXME: else error? */
                if (response->payload && response->payload->type == PAYLOAD_TYPE_REPRESENTATION)
                {
                    HandleBatchResponse(cbNode->requestUri, (OCRepPayload **)&response->payload);
                }

		OIC_LOG_V(INFO, TAG, "%s: calling user CB", __func__);
		errno = 0;
		appFeedback = cbNode->callBack(cbNode->context,
					       cbNode->handle,
					       response);
		/* FIXME: check errno */

		/* now either retain or discard msg saved to co_service_provider_manager */

                cbNode->sequenceNumber = response->sequenceNumber;

                if (appFeedback & OC_STACK_KEEP_TRANSACTION) /* GAR */
                {
		    OIC_LOG(INFO, TAG, "retaining user CB");
                    // To keep discovery callbacks active.
                    cbNode->TTL = GetTicks(MAX_CB_TIMEOUT_SECONDS *
                                            MILLISECONDS_PER_SECOND);
                }
		else
                {
		    OIC_LOG(INFO, TAG, "removing user CB");
                    DeleteClientCB(cbNode);
                }
            }

            //Need to send ACK when the response is CON
            if(responseInfo->info.type == CA_MSG_CONFIRM)
            {
                SendDirectStackResponse(endPoint, responseInfo->info.messageId, CA_EMPTY,
                        CA_MSG_ACKNOWLEDGE, 0, NULL, NULL, 0, NULL, CA_RESPONSE_FOR_RES);
            }

	    /* FIXME: always free the msg; if not KEEP_PAYLOAD, then also remove it from the co_sp_mgr */
	    if (appFeedback & OC_STACK_KEEP_RESPONSE) { /* GAR */
		OIC_LOG(INFO, TAG, "retaining OCClientResponse");
		/* Client app is responsible for retaining
		   (cosp_mgr_register) and freeing
		   (cosp_mgr_unregister) the message. */
	    } else {
		OIC_LOG(INFO, TAG, "removing OCClientResponse");
		OCPayloadDestroy(response->payload);
		OICFree(response->resourceUri);
		OICFree(response);
	    }
        }
	OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
        return;
    } /* cbNode */
    OIC_LOG_V(DEBUG, TAG, "no callback found for this response message");

    OCResource *resource = NULL;
    ResourceObserver *observer = NULL;
    if (true == GetObserverFromResourceList(&resource, &observer,
                                            responseInfo->info.token,
                                            responseInfo->info.tokenLength))
	{
        OIC_LOG(INFO, TAG, "There is an observer associated with the response token");
        if(responseInfo->result == CA_EMPTY)
        {
            OIC_LOG(INFO, TAG, "Receiving A ACK/RESET for this token");
            if(responseInfo->info.type == CA_MSG_RESET)
            {
                OIC_LOG(INFO, TAG, "This is a RESET");
                OCStackFeedBack(responseInfo->info.token, responseInfo->info.tokenLength,
                        OC_OBSERVER_NOT_INTERESTED);
            }
            else if(responseInfo->info.type == CA_MSG_ACKNOWLEDGE)
            {
                OIC_LOG(INFO, TAG, "This is a pure ACK");
                OCStackFeedBack(responseInfo->info.token, responseInfo->info.tokenLength,
                        OC_OBSERVER_STILL_INTERESTED);
            }
        }
        else if(responseInfo->result == CA_RETRANSMIT_TIMEOUT)
        {
            OIC_LOG(INFO, TAG, "Receiving Time Out for an observer");
            OCStackFeedBack(responseInfo->info.token, responseInfo->info.tokenLength,
                    OC_OBSERVER_FAILED_COMM);
        }
        return;
    }

    if(!cbNode && !observer)
    {
        if(myStackMode == OC_CLIENT || myStackMode == OC_CLIENT_SERVER
           || myStackMode == OC_GATEWAY)
        {
            OIC_LOG(INFO, TAG, "This is a client, but no cbNode was found for token");
            if(responseInfo->result == CA_EMPTY)
            {
                OIC_LOG(INFO, TAG, "Receiving CA_EMPTY in the ocstack");
            }
            else
            {
                OIC_LOG(INFO, TAG, "Received a message without callbacks. Sending RESET");
                SendDirectStackResponse(endPoint, responseInfo->info.messageId, CA_EMPTY,
                                        CA_MSG_RESET, 0, NULL, NULL, 0, NULL, CA_RESPONSE_FOR_RES);
            }
        }

        if(myStackMode == OC_SERVER || myStackMode == OC_CLIENT_SERVER
           || myStackMode == OC_GATEWAY)
        {
            OIC_LOG(INFO, TAG, "This is a server, but no observer was found for token");
            if (responseInfo->info.type == CA_MSG_ACKNOWLEDGE)
            {
                OIC_LOG_V(INFO, TAG, "Received ACK at server for messageId : %d",
                                            responseInfo->info.messageId);
            }
            if (responseInfo->info.type == CA_MSG_RESET)
            {
                OIC_LOG_V(INFO, TAG, "Received RESET at server for messageId : %d",
                                            responseInfo->info.messageId);
            }
        }

        return;
    }
}

/**
 * This function will be called back by CA layer when a response is received.
 *
 * @param endPoint CA remote endpoint.
 * @param responseInfo CA response info.
 */
static void HandleCAResponses(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
{
    VERIFY_NON_NULL_NR(endPoint, FATAL);
    VERIFY_NON_NULL_NR(responseInfo, FATAL);

    OIC_LOG(INFO, TAG, "Enter HandleCAResponses");
    OIC_TRACE_BEGIN(%s:HandleCAResponses, TAG);
#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
#ifdef ROUTING_GATEWAY
    bool needRIHandling = false;
    /*
     * Routing manager is going to update either of endpoint or response or both.
     * This typecasting is done to avoid unnecessary duplication of Endpoint and responseInfo
     * RM can update "routeData" option in endPoint so that future RI requests can be sent to proper
     * destination.
     */
    OCStackResult ret = RMHandleResponse((CAResponseInfo_t *)responseInfo, (CAEndpoint_t *)endPoint,
                                         &needRIHandling);
    if(ret != OC_STACK_OK || !needRIHandling)
    {
        OIC_LOG_V(INFO, TAG, "Routing status![%d]. Not forwarding to RI", ret);
        OIC_TRACE_END();
        return;
    }
#endif

    /*
     * Put source in sender endpoint so that the next packet from application can be routed to
     * proper destination and remove "RM" coap header option before passing request / response to
     * RI as this option will make no sense to either RI or application.
     */
    RMUpdateInfo((CAHeaderOption_t **) &(responseInfo->info.options),
        (uint8_t *) &(responseInfo->info.numOptions),
        (CAEndpoint_t *)endPoint);
#endif

    OCHandleResponse(endPoint, responseInfo);

    OIC_LOG(INFO, TAG, "Exit HandleCAResponses");
    OIC_TRACE_END();
}

/*
 * This function handles error response from CA
 * code shall be added to handle the errors
 */
void HandleCAErrorResponse(const CAEndpoint_t *endPoint, const CAErrorInfo_t *errorInfo)
{
    VERIFY_NON_NULL_NR(endPoint, FATAL);
    VERIFY_NON_NULL_NR(errorInfo, FATAL);

    OIC_LOG(INFO, TAG, "Enter HandleCAErrorResponse");
    OIC_TRACE_BEGIN(%s:HandleCAErrorResponse, TAG);

    ClientCB *cbNode = GetClientCBUsingToken(errorInfo->info.token,
                                             errorInfo->info.tokenLength);
    if (cbNode)
    {
        OCClientResponse *response = NULL;

        response = (OCClientResponse *)OICCalloc(1, sizeof(*response));
        if (!response)
        {
            OIC_LOG(ERROR, TAG, "Allocating memory for response failed");
            OIC_TRACE_END();
            return;
        }

        response->devAddr.adapter = OC_DEFAULT_ADAPTER;
        CopyEndpointToDevAddr(endPoint, &response->devAddr);
        FixUpClientResponse(response);
        response->resourceUri = errorInfo->info.resourceUri;
        memcpy(response->identity.id, errorInfo->info.identity.id,
               sizeof (response->identity.id));
        response->identity.id_length = errorInfo->info.identity.id_length;
        response->result = CAResultToOCResult(errorInfo->result);

        cbNode->callBack(cbNode->context, cbNode->handle, response);
        OICFree(response);
    }

    OCResource *resource = NULL;
    ResourceObserver *observer = NULL;
    if (true == GetObserverFromResourceList(&resource, &observer,
                                            errorInfo->info.token, errorInfo->info.tokenLength))
    {
        OIC_LOG(INFO, TAG, "Receiving communication error for an observer");
        OCStackResult result = CAResultToOCResult(errorInfo->result);
        if (OC_STACK_COMM_ERROR == result)
        {
            OCStackFeedBack(errorInfo->info.token, errorInfo->info.tokenLength,
                            OC_OBSERVER_FAILED_COMM);
        }
    }

    OIC_LOG(INFO, TAG, "Exit HandleCAErrorResponse");
    OIC_TRACE_END();
}

/*
 * This function sends out Direct Stack Responses. These are responses that are not coming
 * from the application entity handler. These responses have no payload and are usually ACKs,
 * RESETs or some error conditions that were caught by the stack.
 */
OCStackResult SendDirectStackResponse(const CAEndpoint_t* endPoint, const uint16_t coapID,
        const CAResponseResult_t responseResult, const CAMessageType_t type,
        const uint8_t numOptions, const CAHeaderOption_t *options,
        CAToken_t token, uint8_t tokenLength, const char *resourceUri,
        CADataType_t dataType)
{
    OIC_LOG(DEBUG, TAG, "Entering SendDirectStackResponse");
    CAResponseInfo_t respInfo = {
        .result = responseResult
    };
    respInfo.info.messageId = coapID;
    respInfo.info.numOptions = numOptions;

    if (respInfo.info.numOptions)
    {
        respInfo.info.options =
            (CAHeaderOption_t *)OICCalloc(respInfo.info.numOptions, sizeof(CAHeaderOption_t));
        if (NULL == respInfo.info.options)
        {
            OIC_LOG(ERROR, TAG, "Calloc failed");
            return OC_STACK_NO_MEMORY;
        }
        memcpy (respInfo.info.options, options,
                sizeof(CAHeaderOption_t) * respInfo.info.numOptions);

    }

    respInfo.info.payload = NULL;
    respInfo.info.token = token;
    respInfo.info.tokenLength = tokenLength;
    respInfo.info.type = type;
    respInfo.info.resourceUri = OICStrdup (resourceUri);
    respInfo.info.acceptFormat = CA_FORMAT_UNDEFINED;
    respInfo.info.dataType = dataType;

#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
    // Add the destination to route option from the endpoint->routeData.
    bool doPost = false;
    OCStackResult result = RMAddInfo(endPoint->routeData, &respInfo, false, &doPost);
    if(OC_STACK_OK != result)
    {
        OIC_LOG_V(ERROR, TAG, "Add routing option failed [%d]", result);
        OICFree (respInfo.info.resourceUri);
        OICFree (respInfo.info.options);
        return result;
    }
    if (doPost)
    {
        OIC_LOG(DEBUG, TAG, "Sending a POST message for EMPTY ACK in Client Mode");
        CARequestInfo_t reqInfo = {.method = CA_POST };
        /* The following initialization is not done in a single initializer block as in
         * arduino, .c file is compiled as .cpp and moves it from C99 to C++11.  The latter
         * does not have designated initalizers. This is a work-around for now.
         */
        reqInfo.info.type = CA_MSG_NONCONFIRM;
        reqInfo.info.messageId = coapID;
        reqInfo.info.tokenLength = tokenLength;
        reqInfo.info.token = token;
        reqInfo.info.numOptions = respInfo.info.numOptions;
        reqInfo.info.payload = NULL;
        reqInfo.info.resourceUri = OICStrdup (OC_RSRVD_GATEWAY_URI);
        if (reqInfo.info.numOptions)
        {
            reqInfo.info.options =
                (CAHeaderOption_t *)OICCalloc(reqInfo.info.numOptions, sizeof(CAHeaderOption_t));
            if (NULL == reqInfo.info.options)
            {
                OIC_LOG(ERROR, TAG, "Calloc failed");
                OICFree (reqInfo.info.resourceUri);
                OICFree (respInfo.info.resourceUri);
                OICFree (respInfo.info.options);
                return OC_STACK_NO_MEMORY;
            }
            memcpy (reqInfo.info.options, respInfo.info.options,
                    sizeof(CAHeaderOption_t) * reqInfo.info.numOptions);

        }
        CAResult_t caResult = CASendRequest(endPoint, &reqInfo);
        OICFree (reqInfo.info.resourceUri);
        OICFree (reqInfo.info.options);
        OICFree (respInfo.info.resourceUri);
        OICFree (respInfo.info.options);
        if (CA_STATUS_OK != caResult)
        {
            OIC_LOG(ERROR, TAG, "CASendRequest error");
            return CAResultToOCResult(caResult);
        }
    }
    else
#endif
    {
        CAResult_t caResult = CASendResponse(endPoint, &respInfo);

        // resourceUri in the info field is cloned in the CA layer and
        // thus ownership is still here.
        OICFree (respInfo.info.resourceUri);
        OICFree (respInfo.info.options);
        if(CA_STATUS_OK != caResult)
        {
            OIC_LOG(ERROR, TAG, "CASendResponse error");
            return CAResultToOCResult(caResult);
        }
    }
    OIC_LOG(DEBUG, TAG, "Exit SendDirectStackResponse");
    return OC_STACK_OK;
}

/**
 * Handler function to execute stack requests
 *
 * @param protocolRequest      Pointer to the protocol requests from server.
 *
 * @return
 *     ::OCStackResult
 */
static OCStackResult HandleStackRequests(OCServerProtocolRequest * protocolRequest)
{
    OIC_LOG(INFO, TAG, "Entering HandleStackRequests (OCStack Layer)");
    OCStackResult result = OC_STACK_ERROR;
    if (!protocolRequest)
    {
        OIC_LOG(ERROR, TAG, "protocolRequest is NULL");
        return OC_STACK_INVALID_PARAM;
    }

    OCServerRequest * request = GetServerRequestUsingToken(protocolRequest->requestToken,
            protocolRequest->tokenLength);
    if (!request)
    {
        OIC_LOG(INFO, TAG, "This is a new Server Request");
        result = AddServerRequest(&request, protocolRequest->coapID,
                protocolRequest->delayedResNeeded, 0, protocolRequest->method,
                protocolRequest->numRcvdVendorSpecificHeaderOptions,
                protocolRequest->observationOption, protocolRequest->qos,
                protocolRequest->query, protocolRequest->rcvdVendorSpecificHeaderOptions,
                protocolRequest->payloadFormat, protocolRequest->payload,
                protocolRequest->requestToken, protocolRequest->tokenLength,
                protocolRequest->resourceUrl, protocolRequest->reqTotalSize,
                protocolRequest->acceptFormat, protocolRequest->acceptVersion,
                &protocolRequest->devAddr);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(ERROR, TAG, "Error adding server request");
            return result;
        }

        if(!request)
        {
            OIC_LOG(ERROR, TAG, "Out of Memory");
            return OC_STACK_NO_MEMORY;
        }

        if(!protocolRequest->reqMorePacket)
        {
            request->requestComplete = 1;
        }
    }
    else
    {
        OIC_LOG(INFO, TAG, "This is either a repeated or blocked Server Request");
    }

    if (request->requestComplete)
    {
        OIC_LOG(INFO, TAG, "This Server Request is complete");
        ResourceHandling resHandling = OC_RESOURCE_VIRTUAL;
        OCResource *resource = NULL;
        result = DetermineResourceHandling (request, &resHandling, &resource);
        if (result == OC_STACK_OK)
        {
            result = ProcessRequest(resHandling, resource, request);
        }
    }
    else
    {
        OIC_LOG(INFO, TAG, "This Server Request is incomplete");
        result = OC_STACK_CONTINUE;
    }
    return result;
}

void OCHandleRequests(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
{
    OIC_TRACE_MARK(%s:OCHandleRequests:%s, TAG, requestInfo->info.resourceUri);
    OIC_LOG(DEBUG, TAG, "Enter OCHandleRequests");
    OIC_LOG_V(INFO, TAG, "Endpoint URI : %s", requestInfo->info.resourceUri);

    if (myStackMode == OC_CLIENT)
    {
        //TODO: should the client be responding to requests?
        return;
    }

    // If the request message is Confirmable,
    // then the response SHOULD be returned in an Acknowledgement message.
    CAMessageType_t directResponseType = requestInfo->info.type;
    directResponseType = (directResponseType == CA_MSG_CONFIRM)
            ? CA_MSG_ACKNOWLEDGE : CA_MSG_NONCONFIRM;

    char * uriWithoutQuery = NULL;
    char * query = NULL;
    OCStackResult requestResult = OC_STACK_ERROR;

    requestResult = getQueryFromUri(requestInfo->info.resourceUri, &query, &uriWithoutQuery);

    if (requestResult != OC_STACK_OK || !uriWithoutQuery)
    {
        OIC_LOG_V(ERROR, TAG, "getQueryFromUri() failed with OC error code %d\n", requestResult);
        return;
    }
    OIC_LOG_V(INFO, TAG, "URI without query: %s", uriWithoutQuery);
    OIC_LOG_V(INFO, TAG, "Query : %s", query);

    OCServerProtocolRequest serverRequest = { 0 };
    if (strlen(uriWithoutQuery) < MAX_URI_LENGTH)
    {
        OICStrcpy(serverRequest.resourceUrl, sizeof(serverRequest.resourceUrl), uriWithoutQuery);
        OICFree(uriWithoutQuery);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "URI length exceeds MAX_URI_LENGTH.");
        OICFree(uriWithoutQuery);
        OICFree(query);
        return;
    }

    if (query)
    {
        if (strlen(query) < MAX_QUERY_LENGTH)
        {
            OICStrcpy(serverRequest.query, sizeof(serverRequest.query), query);
            OICFree(query);
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Query length exceeds MAX_QUERY_LENGTH.");
            OICFree(query);
            return;
        }
    }

    if ((requestInfo->info.payload) && (0 < requestInfo->info.payloadSize))
    {
        serverRequest.payloadFormat = CAToOCPayloadFormat(requestInfo->info.payloadFormat);
        serverRequest.reqTotalSize = requestInfo->info.payloadSize;
        serverRequest.payload = (uint8_t *) OICMalloc(requestInfo->info.payloadSize);
        if (!serverRequest.payload)
        {
            OIC_LOG(ERROR, TAG, "Allocation for payload failed.");
            return;
        }
        memcpy (serverRequest.payload, requestInfo->info.payload,
                requestInfo->info.payloadSize);
    }
    else
    {
        serverRequest.reqTotalSize = 0;
    }

    switch (requestInfo->method)
    {
        case CA_GET:
            serverRequest.method = OC_REST_GET;
            break;
        case CA_PUT:
            serverRequest.method = OC_REST_PUT;
            break;
        case CA_POST:
            serverRequest.method = OC_REST_POST;
            break;
        case CA_DELETE:
            serverRequest.method = OC_REST_DELETE;
            break;
        default:
            OIC_LOG_V(ERROR, TAG, "Received CA method %d not supported", requestInfo->method);
            SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_BAD_REQ,
                                    directResponseType, requestInfo->info.numOptions,
                                    requestInfo->info.options, requestInfo->info.token,
                                    requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                    CA_RESPONSE_DATA);
            OICFree(serverRequest.payload);
            return;
    }

    OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)requestInfo->info.token,
                   requestInfo->info.tokenLength);

    serverRequest.tokenLength = requestInfo->info.tokenLength;
    if (serverRequest.tokenLength)
    {
        // Non empty token
        serverRequest.requestToken = (CAToken_t)OICMalloc(requestInfo->info.tokenLength);

        if (!serverRequest.requestToken)
        {
            OIC_LOG(FATAL, TAG, "Allocation for token failed.");
            SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_INTERNAL_SERVER_ERROR,
                                    directResponseType, requestInfo->info.numOptions,
                                    requestInfo->info.options, requestInfo->info.token,
                                    requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                    CA_RESPONSE_DATA);
            OICFree(serverRequest.payload);
            return;
        }
        memcpy(serverRequest.requestToken, requestInfo->info.token, requestInfo->info.tokenLength);
    }

    serverRequest.acceptFormat = CAToOCPayloadFormat(requestInfo->info.acceptFormat);
    if (OC_FORMAT_VND_OCF_CBOR == serverRequest.acceptFormat)
    {
        serverRequest.acceptVersion = requestInfo->info.acceptVersion;
    }

    if (requestInfo->info.type == CA_MSG_CONFIRM)
    {
        serverRequest.qos = OC_HIGH_QOS;
    }
    else
    {
        serverRequest.qos = OC_LOW_QOS;
    }
    // CA does not need the following field
    // Are we sure CA does not need them? how is it responding to multicast
    serverRequest.delayedResNeeded = 0;

    serverRequest.coapID = requestInfo->info.messageId;

    CopyEndpointToDevAddr(endPoint, &serverRequest.devAddr);

    // copy vendor specific header options
    uint8_t tempNum = (requestInfo->info.numOptions);

    // Assume no observation requested and it is a pure GET.
    // If obs registration/de-registration requested it'll be fetched from the
    // options in GetObserveHeaderOption()
    serverRequest.observationOption = OC_OBSERVE_NO_OPTION;

    GetObserveHeaderOption(&serverRequest.observationOption, requestInfo->info.options, &tempNum);
    if (requestInfo->info.numOptions > MAX_HEADER_OPTIONS)
    {
        OIC_LOG(ERROR, TAG,
                "The request info numOptions is greater than MAX_HEADER_OPTIONS");
        SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_BAD_OPT,
                                directResponseType, requestInfo->info.numOptions,
                                requestInfo->info.options, requestInfo->info.token,
                                requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                CA_RESPONSE_DATA);
        OICFree(serverRequest.payload);
        OICFree(serverRequest.requestToken);
        return;
    }
    serverRequest.numRcvdVendorSpecificHeaderOptions = tempNum;
    if (serverRequest.numRcvdVendorSpecificHeaderOptions && requestInfo->info.options)
    {
        memcpy(&(serverRequest.rcvdVendorSpecificHeaderOptions), requestInfo->info.options,
               sizeof(CAHeaderOption_t) * tempNum);
    }

    requestResult = HandleStackRequests (&serverRequest);

    if (requestResult == OC_STACK_SLOW_RESOURCE)
    {
        // Send ACK to client as precursor to slow response
        if (requestInfo->info.type == CA_MSG_CONFIRM)
        {
            SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_EMPTY,
                                    CA_MSG_ACKNOWLEDGE,0, NULL, NULL, 0, NULL,
                                    CA_RESPONSE_DATA);
        }
    }
    if (requestResult == OC_STACK_RESOURCE_ERROR
            && serverRequest.observationOption == OC_OBSERVE_REGISTER)
    {
        OIC_LOG(ERROR, TAG, "Observe Registration failed due to resource error");
    }
    else if (!OCResultToSuccess(requestResult))
    {
        OIC_LOG_V(ERROR, TAG, "HandleStackRequests failed. error: %d", requestResult);

        CAResponseResult_t stackResponse = OCToCAStackResult(requestResult, serverRequest.method);

        SendDirectStackResponse(endPoint, requestInfo->info.messageId, stackResponse,
                                directResponseType, requestInfo->info.numOptions,
                                requestInfo->info.options, requestInfo->info.token,
                                requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                CA_RESPONSE_DATA);
    }
    // requestToken is fed to HandleStackRequests, which then goes to AddServerRequest.
    // The token is copied in there, and is thus still owned by this function.
    OICFree(serverRequest.payload);
    OICFree(serverRequest.requestToken);
    OIC_LOG(INFO, TAG, "Exit OCHandleRequests");
}

/**
 * This function will be called back by CA layer when a request is received.
 *
 * @param endPoint CA remote endpoint.
 * @param requestInfo CA request info.
 */
static void HandleCARequests(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
{
    OIC_LOG(INFO, TAG, "Enter HandleCARequests");
    OIC_TRACE_BEGIN(%s:HandleCARequests, TAG);
    if (!endPoint)
    {
        OIC_LOG(ERROR, TAG, "endPoint is NULL");
        OIC_TRACE_END();
        return;
    }

    if (!requestInfo)
    {
        OIC_LOG(ERROR, TAG, "requestInfo is NULL");
        OIC_TRACE_END();
        return;
    }

#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
#ifdef ROUTING_GATEWAY
    bool needRIHandling = false;
    bool isEmptyMsg = false;
    /*
     * Routing manager is going to update either of endpoint or request or both.
     * This typecasting is done to avoid unnecessary duplication of Endpoint and requestInfo
     * RM can update "routeData" option in endPoint so that future RI requests can be sent to proper
     * destination. It can also remove "RM" coap header option before passing request / response to
     * RI as this option will make no sense to either RI or application.
     */
    OCStackResult ret = RMHandleRequest((CARequestInfo_t *)requestInfo, (CAEndpoint_t *)endPoint,
                                        &needRIHandling, &isEmptyMsg);
    if (OC_STACK_OK != ret || !needRIHandling)
    {
        OIC_LOG_V(INFO, TAG, "Routing status![%d]. Not forwarding to RI", ret);
        OIC_TRACE_END();
        return;
    }
#endif

    /*
     * Put source in sender endpoint so that the next packet from application can be routed to
     * proper destination and remove RM header option.
     */
    RMUpdateInfo((CAHeaderOption_t **) &(requestInfo->info.options),
        (uint8_t *) &(requestInfo->info.numOptions),
        (CAEndpoint_t *)endPoint);

#ifdef ROUTING_GATEWAY
    if (isEmptyMsg)
    {
        /*
         * In Gateways, the MSGType in route option is used to check if the actual
         * response is EMPTY message(4 bytes CoAP Header).  In case of Client, the
         * EMPTY response is sent in the form of POST request which need to be changed
         * to a EMPTY response by RM.  This translation is done in this part of the code.
         */
        OIC_LOG(INFO, TAG, "This is a Empty response from the Client");
        CAResponseInfo_t respInfo = {.result = CA_EMPTY,
                                     .info.messageId = requestInfo->info.messageId,
                                     .info.type = CA_MSG_ACKNOWLEDGE};
        OCHandleResponse(endPoint, &respInfo);
    }
    else
#endif
#endif
    {
        // Normal handling of the packet
        OCHandleRequests(endPoint, requestInfo);
    }
    OIC_LOG(INFO, TAG, "Exit HandleCARequests");
    OIC_TRACE_END();
}

//-----------------------------------------------------------------------------
// Public APIs
//-----------------------------------------------------------------------------
#ifdef RA_ADAPTER
OCStackResult OC_CALL OCSetRAInfo(const OCRAInfo_t *raInfo)
{
    if (!raInfo           ||
        !raInfo->username ||
        !raInfo->hostname ||
        !raInfo->xmpp_domain)
    {

        return OC_STACK_INVALID_PARAM;
    }
    OCStackResult result = CAResultToOCResult(CASetRAInfo((const CARAInfo_t *) raInfo));
    gRASetInfo = (result == OC_STACK_OK)? true : false;

    return result;
}
#endif

OCStackResult OC_CALL OCInit(const char *ipAddr, uint16_t port, OCMode mode) EXPORT
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    (void) ipAddr;
    (void) port;
    OCStackResult r = OCInit1(mode, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return r;
}

OCStackResult OC_CALL OCInit1(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OCStackResult r = OCInit2(mode, serverFlags, clientFlags, OC_DEFAULT_ADAPTER);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT, rc: %d", __func__, r);
    return r;
}

OCStackResult OC_CALL OCInit2(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags,
                              OCTransportAdapter transportType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    // Serialize calls to start and stop the stack.
    OCEnterInitializer();

    OCStackResult result = OC_STACK_OK;

    if (g_ocStackStartCount == 0)
    {
        // This is the first call to initialize the stack so it gets to do the real work.
        result = OCInitializeInternal(mode, serverFlags, clientFlags, transportType);
    }

    if (result == OC_STACK_OK)
    {
        // Increment the start count since we're about to return success.
        assert(g_ocStackStartCount != UINT_MAX);
        assert(stackState == OC_STACK_INITIALIZED);
        g_ocStackStartCount++;
    }

    OCLeaveInitializer();
    OIC_LOG_V(DEBUG, TAG, "%s EXIT, rc: %d", __func__, result);
    return result;
}

/**
 * Initialize the stack.
 * Caller of this function must serialize calls to this function and the stop counterpart.
 * @param mode            Mode of operation.
 * @param serverFlags     The server flag used when the mode of operation is a server mode.
 * @param clientFlags     The client flag used when the mode of operation is a client mode.
 * @param transportType   The transport type.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
LOCAL OCStackResult OCInitializeInternal(OCMode mode, OCTransportFlags serverFlags,
					  OCTransportFlags clientFlags, OCTransportAdapter transportType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    if (stackState == OC_STACK_INITIALIZED)
    {
        OIC_LOG(INFO, TAG, "Subsequent calls to OCInit() without calling \
                OCStop() between them are ignored.");
        return OC_STACK_OK;
    }

    oocf_cosp_mgr_init();		/* GAR: initialize co-serviceprovider mgr */

#ifndef ROUTING_GATEWAY
    if (OC_GATEWAY == mode)
    {
        OIC_LOG(ERROR, TAG, "Routing Manager not supported");
        return OC_STACK_INVALID_PARAM;
    }
#endif

#ifdef RA_ADAPTER
    if(!gRASetInfo)
    {
        OIC_LOG(ERROR, TAG, "Need to call OCSetRAInfo before calling OCInit");
        return OC_STACK_ERROR;
    }
#endif

    OIC_LOG_V(INFO, TAG, "IoTivity version is v%s", IOTIVITY_VERSION);
    OCStackResult result = OC_STACK_ERROR;

    // Validate mode
    if (!((mode == OC_CLIENT) || (mode == OC_SERVER) || (mode == OC_CLIENT_SERVER)
        || (mode == OC_GATEWAY)))
    {
        OIC_LOG(ERROR, TAG, "Invalid mode");
        return OC_STACK_ERROR;
    }
    myStackMode = mode;

    if (mode == OC_CLIENT || mode == OC_CLIENT_SERVER || mode == OC_GATEWAY)
    {
        caglobals.client = true;
    }
    if (mode == OC_SERVER || mode == OC_CLIENT_SERVER || mode == OC_GATEWAY)
    {
        caglobals.server = true;
    }

    caglobals.serverFlags = (CATransportFlags_t)serverFlags;
    if (!(caglobals.serverFlags & CA_IPFAMILY_MASK))
    {
        caglobals.serverFlags = (CATransportFlags_t)(caglobals.serverFlags|CA_IPV4|CA_IPV6);
    }
    caglobals.clientFlags = (CATransportFlags_t)clientFlags;
    if (!(caglobals.clientFlags & CA_IPFAMILY_MASK))
    {
        caglobals.clientFlags = (CATransportFlags_t)(caglobals.clientFlags|CA_IPV4|CA_IPV6);
    }

    defaultDeviceHandler = NULL;
    defaultDeviceHandlerCallbackParameter = NULL;

#ifdef UWP_APP
    result = InitSqlite3TempDir();
    if (result != OC_STACK_OK) {OIC_LOG_V(FATAL, TAG, "%s failed!!", result); goto exit;}
#endif // UWP_APP

    result = InitializeScheduleResourceList();
    if (result != OC_STACK_OK) {OIC_LOG_V(FATAL, TAG, "%s failed!!", result); goto exit;}

    result = CAResultToOCResult(CAInitialize((CATransportAdapter_t)transportType));
    if (result != OC_STACK_OK) {OIC_LOG_V(FATAL, TAG, "%s failed!!", result); goto exit;}

    result = CAResultToOCResult(OCSelectNetwork(transportType));
    if (result != OC_STACK_OK) {OIC_LOG_V(FATAL, TAG, "%s failed!!", result); goto exit;}

    result = CAResultToOCResult(CARegisterNetworkMonitorHandler(
      OCDefaultAdapterStateChangedHandler, OCDefaultConnectionStateChangedHandler));
    if (result != OC_STACK_OK) {OIC_LOG_V(FATAL, TAG, "%s failed!!", result); goto exit;}

    switch (myStackMode)
    {
        case OC_CLIENT:
            CARegisterHandler(HandleCARequests, HandleCAResponses, HandleCAErrorResponse);
            result = CAResultToOCResult(CAStartDiscoveryServer());
            OIC_LOG(INFO, TAG, "Client mode: CAStartDiscoveryServer");
            break;
        case OC_SERVER:
            SRMRegisterHandler(HandleCARequests, HandleCAResponses, HandleCAErrorResponse);
            result = CAResultToOCResult(CAStartListeningServer());
            OIC_LOG(INFO, TAG, "Server mode: CAStartListeningServer");
            break;
        case OC_CLIENT_SERVER:
        case OC_GATEWAY:
            SRMRegisterHandler(HandleCARequests, HandleCAResponses, HandleCAErrorResponse);
            result = CAResultToOCResult(CAStartListeningServer());
            if(result == OC_STACK_OK)
            {
                result = CAResultToOCResult(CAStartDiscoveryServer());
            }
            break;
    }
    if (result != OC_STACK_OK) {OIC_LOG_V(FATAL, TAG, "%s failed!!", result); goto exit;}

#ifdef TCP_ADAPTER
    CARegisterKeepAliveHandler(HandleKeepAliveConnCB);
#endif

#ifdef WITH_PRESENCE
    PresenceTimeOutSize = sizeof (PresenceTimeOut) / sizeof (PresenceTimeOut[0]) - 1;
#endif // WITH_PRESENCE

    //Update Stack state to initialized
    stackState = OC_STACK_INITIALIZED;

    // Initialize resource
    if(myStackMode != OC_CLIENT)
    {
        result = initResources();
    }

#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
    RMSetStackMode(mode);
#ifdef ROUTING_GATEWAY
    if (OC_GATEWAY == myStackMode)
    {
        result = RMInitialize();
    }
#endif
#endif

#ifdef TCP_ADAPTER
    if (result == OC_STACK_OK)
    {
        result = InitializeKeepAlive(myStackMode);
    }
#endif

#if defined(TCP_ADAPTER) && defined(WITH_CLOUD)
    // Initialize the Connection Manager
    if (result == OC_STACK_OK)
    {
        result = OCCMInitialize();
    }
#endif

exit:
    if(result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Stack initialization error");
        TerminateScheduleResourceList();
        deleteAllResources();
        CATerminate();
        stackState = OC_STACK_UNINITIALIZED;
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return result;
}

OCStackResult OC_CALL OCStop() EXPORT
{
    OIC_LOG(INFO, TAG, "Entering OCStop");

    // Serialize calls to start and stop the stack.
    OCEnterInitializer();

    OCStackResult result = OC_STACK_OK;

    if (g_ocStackStartCount == 1)
    {
        // This is the last call to stop the stack, do the real work.
        result = OCDeInitializeInternal();
    }
    else if (g_ocStackStartCount == 0)
    {
        OIC_LOG(ERROR, TAG, "Too many calls to OCStop");
        assert(!"Too many calls to OCStop");
        result = OC_STACK_ERROR;
    }

    if (result == OC_STACK_OK)
    {
        g_ocStackStartCount--;
    }

    OCLeaveInitializer();
    return result;
}

/**
 * DeInitialize the stack.
 * Caller of this function must serialize calls to this function and the init counterpart.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
LOCAL OCStackResult OCDeInitializeInternal()
{
    assert(stackState == OC_STACK_INITIALIZED);

#ifdef WITH_PRESENCE
    // Ensure that the TTL associated with ANY and ALL presence notifications originating from
    // here send with the code "OC_STACK_PRESENCE_STOPPED" result.
    presenceResource.presenceTTL = 0;
    presenceState = OC_PRESENCE_UNINITIALIZED;
#endif // WITH_PRESENCE

#ifdef ROUTING_GATEWAY
    if (OC_GATEWAY == myStackMode)
    {
        RMTerminate();
    }
#endif

#ifdef TCP_ADAPTER
    TerminateKeepAlive(myStackMode);
#endif

    OCStackResult result = CAResultToOCResult(
            CAUnregisterNetworkMonitorHandler(OCDefaultAdapterStateChangedHandler,
                                              OCDefaultConnectionStateChangedHandler));
    if (OC_STACK_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAUnregisterNetworkMonitorHandler has failed");
    }

    TerminateScheduleResourceList();
    // Free memory dynamically allocated for resources
    deleteAllResources();
    // Remove all the client callbacks
    DeleteClientCBList();
    // Terminate connectivity-abstraction layer.
    CATerminate();

#if defined(TCP_ADAPTER) && defined(WITH_CLOUD)
    // Terminate the Connection Manager
    OCCMTerminate();
#endif

 /* FIXME: if BT
  *    // Unset cautil config
  *    CAUtilConfig_t configs = {(CATransportBTFlags_t)CA_DEFAULT_BT_FLAGS};
  *    CAUtilSetBTConfigure(configs); */

    stackState = OC_STACK_UNINITIALIZED;
    return OC_STACK_OK;
}

OCStackResult OC_CALL OCStartMulticastServer()
{
    if(stackState != OC_STACK_INITIALIZED)
    {
        OIC_LOG(ERROR, TAG, "OCStack is not initalized. Cannot start multicast server.");
        return OC_STACK_ERROR;
    }
    g_multicastServerStopped = false;
    return OC_STACK_OK;
}

OCStackResult OC_CALL OCStopMulticastServer()
{
    g_multicastServerStopped = true;
    return OC_STACK_OK;
}


/**
 * Map OCQualityOfService to CAMessageType.
 *
 * @param qos Input qos.
 *
 * @return CA message type for a given qos.
 */
static CAMessageType_t qualityOfServiceToMessageType(OCQualityOfService qos)
{
    switch (qos)
    {
        case OC_HIGH_QOS:
            return CA_MSG_CONFIRM;
        case OC_LOW_QOS:
        case OC_MEDIUM_QOS:
        case OC_NA_QOS:
        default:
            return CA_MSG_NONCONFIRM;
    }
}

/**
 *  A request uri consists of the following components in order:
 *                              example
 *  optionally one of
 *      CoAP over UDP prefix    "coap://"
 *      CoAP over TCP prefix    "coap+tcp://"
 *      CoAP over DTLS prefix   "coaps://"
 *      CoAP over TLS prefix    "coaps+tcp://"
 *  optionally one of
 *      IPv6 address            "[1234::5678]"
 *      IPv4 address            "192.168.1.1"
 *  optional port               ":5683"
 *  resource uri                "/oc/core..."
 *
 *  for PRESENCE requests, extract resource type.
 */
static OCStackResult ParseRequestUri(const char *fullUri,
                                        OCTransportAdapter adapter,
                                        OCTransportFlags flags,
                                        OCDevAddr **devAddr,
                                        char **resourceUri,
                                        char **resourceType)
{
    VERIFY_NON_NULL(fullUri, FATAL, OC_STACK_INVALID_CALLBACK);

    OCStackResult result = OC_STACK_OK;
    OCDevAddr *da = NULL;
    char *colon = NULL;
    char *end;

    // provide defaults for all returned values
    if (devAddr)
    {
        *devAddr = NULL;
    }
    if (resourceUri)
    {
        *resourceUri = NULL;
    }
    if (resourceType)
    {
        *resourceType = NULL;
    }

    // delimit url prefix, if any
    const char *start = fullUri;
    char *slash2 = strstr(start, "//");
    if (slash2)
    {
        start = slash2 + 2;
    }
    char *slash = strchr(start, '/');
    if (!slash)
    {
        return OC_STACK_INVALID_URI;
    }

    // process url scheme
    size_t prefixLen = slash2 - fullUri;
    bool istcp = false;
    if (prefixLen)
    {
        if (((prefixLen == sizeof(COAP_TCP_SCHEME) - 1) && (!strncmp(fullUri, COAP_TCP_SCHEME, prefixLen)))
        || ((prefixLen == sizeof(COAPS_TCP_SCHEME) - 1) && (!strncmp(fullUri, COAPS_TCP_SCHEME, prefixLen))))
        {
            istcp = true;
        }
    }

    // TODO: this logic should come in with unit tests exercising the various strings
    // processs url prefix, if any
    size_t urlLen = slash - start;
    // port
    uint16_t port = 0;
    size_t len = 0;
    if (urlLen && devAddr)
    {   // construct OCDevAddr
        if (start[0] == '[')
        {   // ipv6 address
            char *close = strchr(++start, ']');
            if (!close || close > slash)
            {
                return OC_STACK_INVALID_URI;
            }
            end = close;
            if (close[1] == ':')
            {
                colon = close + 1;
            }

            if (istcp)
            {
                adapter = (OCTransportAdapter)(adapter | OC_ADAPTER_TCP);
            }
            else
            {
                adapter = (OCTransportAdapter)(adapter | OC_ADAPTER_IP);
            }
            flags = (OCTransportFlags)(flags | OC_IP_USE_V6);
        }
        else
        {
            char *dot = strchr(start, '.');
            if (dot && dot < slash)
            {   // ipv4 address
                colon = strchr(start, ':');
                end = (colon && colon < slash) ? colon : slash;

                if (istcp)
                {
                    // coap over tcp
                    adapter = (OCTransportAdapter)(adapter | OC_ADAPTER_TCP);
                }
                else
                {
                    adapter = (OCTransportAdapter)(adapter | OC_ADAPTER_IP);
                }
                flags = (OCTransportFlags)(flags | OC_IP_USE_V4);
            }
            else
            {   // MAC address
                end = slash;
            }
        }
        len = end - start;
        if (len >= sizeof(da->addr))
        {
            return OC_STACK_INVALID_URI;
        }
        // collect port, if any
        if (colon && colon < slash)
        {
            for (colon++; colon < slash; colon++)
            {
                char c = colon[0];
                if (c < '0' || c > '9')
                {
                    return OC_STACK_INVALID_URI;
                }
                port = 10 * port + c - '0';
            }
        }

        len = end - start;
        if (len >= sizeof(da->addr))
        {
            return OC_STACK_INVALID_URI;
        }

        da = (OCDevAddr *)OICCalloc(sizeof (OCDevAddr), 1);
        if (!da)
        {
            return OC_STACK_NO_MEMORY;
        }

        // Decode address per RFC 6874.
        result = OCDecodeAddressForRFC6874(da->addr, sizeof(da->addr), start, end);
        if (result != OC_STACK_OK)
        {
             OICFree(*devAddr);
             return result;
        }

        da->port = port;
        da->adapter = adapter;
        da->flags = flags;
        if (!strncmp(fullUri, "coaps", 5))
        {
            da->flags = (OCTransportFlags)(da->flags|CA_SECURE);
        }
        *devAddr = da;
    }

    // process resource uri, if any
    if (slash)
    {   // request uri and query
        size_t ulen = strlen(slash); // resource uri length
        size_t tlen = 0;      // resource type length
        char *type = NULL;

        static const char strPresence[] = "/oic/ad?rt=";
        static const size_t lenPresence = sizeof(strPresence) - 1;
        if (!strncmp(slash, strPresence, lenPresence))
        {
            type = slash + lenPresence;
            tlen = ulen - lenPresence;
        }
        // resource uri
        if (resourceUri)
        {
            *resourceUri = (char *)OICMalloc(ulen + 1);
            if (!*resourceUri)
            {
                result = OC_STACK_NO_MEMORY;
                goto error;
            }
            OICStrcpy(*resourceUri, (ulen + 1), slash);
        }
        // resource type
        if (type && resourceType)
        {
            *resourceType = (char *)OICMalloc(tlen + 1);
            if (!*resourceType)
            {
                result = OC_STACK_NO_MEMORY;
                goto error;
            }

            OICStrcpy(*resourceType, (tlen + 1), type);
        }
    }

    return OC_STACK_OK;

error:
    // free all returned values
    if (devAddr)
    {
        OICFree(*devAddr);
    }
    if (resourceUri)
    {
        OICFree(*resourceUri);
    }
    if (resourceType)
    {
        OICFree(*resourceType);
    }
    return result;
}

/**
 * Discover or Perform requests on a specified resource
 * Deprecated: use OCDoRequest instead
 */
OCStackResult OC_CALL OCDoResource(OCDoHandle *handle,
                                   OCMethod method,
                                   const char *requestUri,
                                   const OCDevAddr *destination,
                                   OCPayload* payload,
                                   OCConnectivityType connectivityType,
                                   OCQualityOfService qos,
                                   OCCallbackData *cbData,
                                   OCHeaderOption *options,
                                   uint8_t numOptions)
EXPORT
{
    OIC_TRACE_BEGIN(%s:OCDoRequest, TAG);
    OCStackResult ret = OCDoRequest(handle, method, requestUri,destination, payload,
                connectivityType, qos, cbData, options, numOptions);
    OIC_TRACE_END();

    // This is the owner of the payload object, so we free it
    OCPayloadDestroy(payload);
    return ret;
}

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
static const char* ASSERT_ROLES_CTX = "Asserting roles from OCDoRequest";
static void assertRolesCB(void* ctx, bool hasError)
{
    OC_UNUSED(ctx); // Not used in release builds

    if (!hasError)
    {
        OIC_LOG_V(DEBUG, TAG, "%s: Asserting roles SUCCEEDED - ctx: %s", __func__, (char*)ctx);
    }
    else
    {
        OIC_LOG_V(DEBUG, TAG, "%s: Asserting roles FAILED - ctx: %s", __func__, (char*)ctx);
    }
}
#endif // __WITH_DTLS__ || __WITH_TLS__

/**
 * Discover or Perform requests on a specified resource
 */
OCStackResult OC_CALL OCDoRequest(OCDoHandle *handle,
                                  OCMethod method,
                                  const char *requestUri,
                                  const OCDevAddr *destination,
                                  OCPayload* payload,
                                  OCConnectivityType connectivityType,
                                  OCQualityOfService qos,
                                  OCCallbackData *cbData,
                                  OCHeaderOption *options,
                                  uint8_t numOptions)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);

    // Validate input parameters
    VERIFY_NON_NULL(cbData, FATAL, OC_STACK_INVALID_CALLBACK);
    VERIFY_NON_NULL(cbData->cb, FATAL, OC_STACK_INVALID_CALLBACK);

    OCStackResult result = OC_STACK_ERROR;
    CAResult_t caResult;
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    ClientCB *clientCB = NULL;
    OCDoHandle resHandle = NULL;
    CAEndpoint_t endpoint = {.adapter = CA_DEFAULT_ADAPTER};
    OCDevAddr tmpDevAddr = { OC_DEFAULT_ADAPTER };
    uint32_t ttl = 0;
    OCTransportAdapter adapter;
    OCTransportFlags flags;
    // the request contents are put here
    CARequestInfo_t requestInfo = {.method = CA_GET};
    // requestUri  will be parsed into the following three variables
    OCDevAddr *devAddr = NULL;
    char *resourceUri = NULL;
    char *resourceType = NULL;
    bool isProxyRequest = false;

    /*
     * Support original behavior with address on resourceUri argument.
     */
    adapter = (OCTransportAdapter)(connectivityType >> CT_ADAPTER_SHIFT);
    flags = (OCTransportFlags)(connectivityType & CT_MASK_FLAGS);
    OIC_LOG_V(DEBUG, TAG, "%s: adapter = %d, flags = %d", __func__, adapter, flags);

    if (requestUri)
    {
        result = ParseRequestUri(requestUri, adapter, flags, &devAddr, &resourceUri, &resourceType);
        if (result != OC_STACK_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "Unable to parse uri: %s", requestUri);
            goto exit;
        }
    }
    else
    {
        isProxyRequest = checkProxyUri(options, numOptions);
        if (!isProxyRequest)
        {
            OIC_LOG(ERROR, TAG, "Request doesn't contain RequestURI/Proxy URI");
            goto exit;
        }
    }

    switch (method)
    {
    case OC_REST_GET:
    case OC_REST_OBSERVE:
    case OC_REST_OBSERVE_ALL:
        requestInfo.method = CA_GET;
        break;
    case OC_REST_PUT:
        requestInfo.method = CA_PUT;
        break;
    case OC_REST_POST:
        requestInfo.method = CA_POST;
        break;
    case OC_REST_DELETE:
        requestInfo.method = CA_DELETE;
        break;
    case OC_REST_DISCOVER:
        // intentional fall through don't add break
#ifdef WITH_PRESENCE
    case OC_REST_PRESENCE:
#endif
        if (destination || devAddr)
        {
            requestInfo.isMulticast = false;
	    OIC_LOG_V(DEBUG, TAG, "Request is UNICAST");
        }
        else
        {
            tmpDevAddr.adapter = adapter;
            tmpDevAddr.flags = flags;
            destination = &tmpDevAddr;
            requestInfo.isMulticast = true;
            qos = OC_LOW_QOS;
	    OIC_LOG_V(DEBUG, TAG, "Request is MULTICAST");
        }
        // OC_REST_DISCOVER: CA_DISCOVER will become GET and isMulticast.
        // OC_REST_PRESENCE: Since "presence" is a stack layer only implementation.
        //                   replacing method type with GET.
        requestInfo.method = CA_GET;
        break;
    default:
        result = OC_STACK_INVALID_METHOD;
        goto exit;
    }

    if (!devAddr && !destination)
    {
        OIC_LOG(DEBUG, TAG, "no devAddr and no destination");
        result = OC_STACK_INVALID_PARAM;
        goto exit;
    }

    /* If not original behavior, use destination argument */
    if (destination && !devAddr)
    {
        devAddr = (OCDevAddr *)OICMalloc(sizeof (OCDevAddr));
        if (!devAddr)
        {
            result = OC_STACK_NO_MEMORY;
            goto exit;
        }
        OIC_LOG(DEBUG, TAG, "devAddr is set as destination");
        *devAddr = *destination;
    }

    if (devAddr)
    {
        OIC_LOG_V(DEBUG, TAG, "remoteId of devAddr : %s", devAddr->remoteId);

        if ((devAddr->remoteId[0] == 0)
            && destination
            && (destination->remoteId[0] != 0))
        {
            OIC_LOG_V(DEBUG, TAG, "Copying remoteId from destination parameter: %s", destination->remoteId);
            OICStrcpy(devAddr->remoteId, sizeof(devAddr->remoteId), destination->remoteId);
        }
    }

    resHandle = GenerateInvocationHandle();
    if (!resHandle)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    caResult = CAGenerateToken(&token, tokenLength);
    if (caResult != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "CAGenerateToken error");
        result= OC_STACK_ERROR;
        goto exit;
    }

    // fill in request data
    requestInfo.info.type = qualityOfServiceToMessageType(qos);
    requestInfo.info.token = token;
    requestInfo.info.tokenLength = tokenLength;

    if ((method == OC_REST_OBSERVE) || (method == OC_REST_OBSERVE_ALL))
    {
        result = CreateObserveHeaderOption (&(requestInfo.info.options),
                                    options, numOptions, OC_OBSERVE_REGISTER);
        if (result != OC_STACK_OK)
        {
            goto exit;
        }
        requestInfo.info.numOptions = numOptions + 1;
    }
    else
    {
        // Check if accept format and accept version have been set.
        uint16_t acceptVersion = OCF_VERSION_1_0_0;
        uint16_t acceptFormat = COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR;
        bool IsAcceptVersionSet = false;
        bool IsAcceptFormatSet = false;
        // Check settings of version option and content format.
        if (numOptions > 0 && options)
        {
            for (uint8_t i = 0; i < numOptions; i++)
            {
                if (OCF_ACCEPT_CONTENT_FORMAT_VERSION == options[i].optionID
                        && sizeof(uint16_t) == options[i].optionLength)
                {
                    acceptVersion = (options[i].optionData[1] << 8) + options[i].optionData[0];
                    IsAcceptVersionSet = true;
                }
                else if (COAP_OPTION_ACCEPT == options[i].optionID)
                {
                    if (sizeof(uint8_t) == options[i].optionLength)
                    {
                        acceptFormat = CAConvertFormat(*(uint8_t*)options[i].optionData);
                        IsAcceptFormatSet = true;
                    }
                    else if (sizeof(uint16_t) == options[i].optionLength)
                    {
                        acceptFormat = CAConvertFormat(
                                (options[i].optionData[1] << 8) + options[i].optionData[0]);
                        IsAcceptFormatSet = true;
                    }
                    else
                    {
                        acceptFormat = CA_FORMAT_UNSUPPORTED;
                        IsAcceptFormatSet = true;
                        OIC_LOG_V(DEBUG, TAG, "option has an unsupported format");
                    }
                }
            }
        }

        if (!IsAcceptVersionSet && !IsAcceptFormatSet)
        {
            requestInfo.info.numOptions = numOptions + 2;
        }
        else if ((IsAcceptFormatSet &&
                CA_FORMAT_APPLICATION_VND_OCF_CBOR == acceptFormat &&
                !IsAcceptVersionSet) || (IsAcceptVersionSet && !IsAcceptFormatSet))
        {
            requestInfo.info.numOptions = numOptions + 1;
        }
        else
        {
            requestInfo.info.numOptions = numOptions;
        }

        requestInfo.info.options = (CAHeaderOption_t*) OICCalloc(requestInfo.info.numOptions,
                sizeof(CAHeaderOption_t));
        if (NULL == requestInfo.info.options)
        {
            OIC_LOG(ERROR, TAG, "Calloc failed");
            result = OC_STACK_NO_MEMORY;
            goto exit;
        }
        memcpy(requestInfo.info.options, (CAHeaderOption_t*) options,
               numOptions * sizeof(CAHeaderOption_t));

        if (!IsAcceptVersionSet && !IsAcceptFormatSet)
        {
            // Append accept format and accept version to the options.
            SetHeaderOption(requestInfo.info.options, numOptions, CA_OPTION_ACCEPT, &acceptFormat,
                    sizeof(uint16_t));
            SetHeaderOption(requestInfo.info.options, numOptions + 1, OCF_ACCEPT_CONTENT_FORMAT_VERSION,
                    &acceptVersion, sizeof(uint16_t));
        }
        else if (IsAcceptFormatSet && CA_FORMAT_APPLICATION_VND_OCF_CBOR == acceptFormat
                && !IsAcceptVersionSet)
        {
            // Append accept version to the options.
            SetHeaderOption(requestInfo.info.options, numOptions, OCF_ACCEPT_CONTENT_FORMAT_VERSION,
                    &acceptVersion, sizeof(uint16_t));
        }
        else if (IsAcceptVersionSet && OCF_VERSION_1_0_0 <= acceptVersion && !IsAcceptFormatSet)
        {
            // Append accept format to the options.
            SetHeaderOption(requestInfo.info.options, numOptions, CA_OPTION_ACCEPT, &acceptFormat,
                    sizeof(uint16_t));
        }
    }

    CopyDevAddrToEndpoint(devAddr, &endpoint);

    if (payload)
    {
        uint16_t payloadVersion = OCF_VERSION_1_0_0;
        CAPayloadFormat_t payloadFormat = CA_FORMAT_APPLICATION_VND_OCF_CBOR;
        // Check version option settings
        if (numOptions > 0 && options)
        {
            for (uint8_t i = 0; i < numOptions; i++)
            {
                if (OCF_CONTENT_FORMAT_VERSION == options[i].optionID
                        && sizeof(uint16_t) == options[i].optionLength)
                {
                    payloadVersion = (options[i].optionData[1] << 8) + options[i].optionData[0];
                }
                else if (COAP_OPTION_CONTENT_FORMAT == options[i].optionID)
                {
                    if (sizeof(uint8_t) == options[i].optionLength)
                    {
                        payloadFormat = CAConvertFormat(*(uint8_t*) options[i].optionData);
                    }
                    else if (sizeof(uint16_t) == options[i].optionLength)
                    {
                        payloadFormat = CAConvertFormat(
                                (options[i].optionData[1] << 8) + options[i].optionData[0]);
                    }
                    else
                    {
                        payloadFormat = CA_FORMAT_UNSUPPORTED;
                        OIC_LOG_V(DEBUG, TAG, "option has an unsupported format");
                    }
                }
            }
        }

        requestInfo.info.payloadFormat = payloadFormat;
        if (CA_FORMAT_APPLICATION_CBOR == payloadFormat && payloadVersion)
        {
            payloadVersion = 0;
        }
        requestInfo.info.payloadVersion = payloadVersion;

        if ((result =
            OCConvertPayload(payload, CAToOCPayloadFormat(requestInfo.info.payloadFormat),
                            &requestInfo.info.payload, &requestInfo.info.payloadSize))
                != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "Failed to create CBOR Payload");
            goto exit;
        }
    }
    else
    {
        requestInfo.info.payload = NULL;
        requestInfo.info.payloadSize = 0;
        requestInfo.info.payloadFormat = CA_FORMAT_UNDEFINED;
    }

    // prepare for response
#ifdef WITH_PRESENCE
    if (method == OC_REST_PRESENCE)
    {
        char *presenceUri = NULL;
        result = OCPreparePresence(&endpoint, &presenceUri,
                                   requestInfo.isMulticast);
        if (OC_STACK_OK != result)
        {
            goto exit;
        }

        // Assign full presence uri as coap://ip:port/oic/ad to add to callback list.
        // Presence notification will form a canonical uri to
        // look for callbacks into the application.
        if (resourceUri)
        {
            OICFree(resourceUri);
        }
        resourceUri = presenceUri;
    }
#endif

    // update resourceUri onto requestInfo after check presence uri
    requestInfo.info.resourceUri = resourceUri;

    ttl = GetTicks(MAX_CB_TIMEOUT_SECONDS * MILLISECONDS_PER_SECOND);
    result = AddClientCB(&clientCB, cbData, requestInfo.info.type, token, tokenLength,
                         requestInfo.info.options, requestInfo.info.numOptions,
                         requestInfo.info.payload, requestInfo.info.payloadSize,
                         requestInfo.info.payloadFormat, &resHandle, method,
                         devAddr, resourceUri, resourceType, ttl);

    if (OC_STACK_OK != result)
    {
        goto exit;
    }

    devAddr = NULL;       // Client CB list entry now owns it
    resourceUri = NULL;   // Client CB list entry now owns it
    resourceType = NULL;  // Client CB list entry now owns it

#ifdef WITH_PRESENCE
    if (method == OC_REST_PRESENCE)
    {
        OIC_LOG(ERROR, TAG, "AddClientCB for presence done.");

        if (handle)
        {
            *handle = resHandle;
        }

        goto exit;
    }
#endif

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
    /* Check whether we should assert role certificates before making this request. */
    bool isEndpointSecure = ((endpoint.flags & CA_SECURE) != 0);
    bool requestContainsRolesUri = (strcmp(requestInfo.info.resourceUri, OIC_RSRC_ROLES_URI) == 0);
    bool requestContainsDoxmResource = (strcmp(requestInfo.info.resourceUri, OIC_RSRC_DOXM_URI) == 0);
    bool isTcpConnectivityType = (CT_ADAPTER_TCP == connectivityType);
    bool requestContainsKeepaliveUri = (strcmp(requestInfo.info.resourceUri, OC_RSRVD_KEEPALIVE_URI) == 0);

    if (isEndpointSecure && (isProxyRequest ||
        (!requestContainsRolesUri && !requestContainsDoxmResource &&
         isTcpConnectivityType && !requestContainsKeepaliveUri)))
    {
        CASecureEndpoint_t sep;
        CAResult_t caRes = CAGetSecureEndpointData(&endpoint, &sep);
        if (caRes != CA_STATUS_OK)
        {
            /*
             * This is a secure request but we do not have a secure connection with
             * this peer, try to assert roles. There's no way to tell if the peer
             * uses certificates without asking, so just try to assert roles.  If
             * it fails, that's OK, roles will get asserted "automatically" when PSK
             * credentials are used.
             */
            if (!isProxyRequest)
            {
                OIC_LOG_V(DEBUG, TAG, "%s: going to try to assert roles before doing request to %s ",
                          __func__, requestInfo.info.resourceUri);
            }

            OCDevAddr da;
            CopyEndpointToDevAddr(&endpoint, &da);
            OCStackResult assertResult = OCAssertRoles((void*)ASSERT_ROLES_CTX, &da,
                                                       &assertRolesCB);
            if (assertResult == OC_STACK_OK)
            {
                OIC_LOG_V(DEBUG, TAG, "%s: Call to OCAssertRoles succeeded", __func__);
            }
            else if (assertResult == OC_STACK_INCONSISTENT_DB)
            {
                OIC_LOG_V(DEBUG, TAG, "%s: No role certificates to assert", __func__);
            }
            else
            {
                OIC_LOG_V(DEBUG, TAG, "%s: Call to OCAssertRoles failed", __func__);
            }

            /*
             * We don't block waiting for OCAssertRoles to complete.  Because the roles assertion
             * request is queued before the actual request, it will happen first.  If it fails, we
             * log the error, but don't retry; the actually request made to OCDorequest may or may
             * not fail (with permission denied), the caller can decide whether to retry.
             */
        }

    }
#endif // __WITH_DTLS__ || __WITH_TLS__


    // send request
    result = OCSendRequest(&endpoint, &requestInfo);
    if (OC_STACK_OK != result)
    {
	OIC_LOG_V(ERROR, TAG, "%s: OCSendRequest error: %d", __func__, result);
       goto exit;
    }

    if (handle)
    {
        *handle = resHandle;
    }

exit:
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCDoResource error");
        DeleteClientCB(clientCB);
        CADestroyToken(token);
        if (handle)
        {
            *handle = NULL;
        }
        OICFree(resHandle);
    }

    OICFree(requestInfo.info.payload);
    OICFree(devAddr);
    OICFree(resourceUri);
    OICFree(resourceType);
    OICFree(requestInfo.info.options);
    return result;
}

OCStackResult OC_CALL OCCancel(OCDoHandle handle, OCQualityOfService qos, OCHeaderOption * options,
        uint8_t numOptions)
{
    /*
     * This ftn is implemented one of two ways in the case of observation:
     *
     * 1. qos == OC_NON_CONFIRMABLE. When observe is unobserved..
     *      Remove the callback associated on client side.
     *      When the next notification comes in from server,
     *      reply with RESET message to server.
     *      Keep in mind that the server will react to RESET only
     *      if the last notification was sent as CON
     *
     * 2. qos == OC_CONFIRMABLE. When OCCancel is called,
     *      and it is associated with an observe request
     *      (i.e. ClientCB->method == OC_REST_OBSERVE || OC_REST_OBSERVE_ALL),
     *      Send CON Observe request to server with
     *      observe flag = OC_RESOURCE_OBSERVE_DEREGISTER.
     *      Remove the callback associated on client side.
     */
    OCStackResult ret = OC_STACK_OK;
    CAEndpoint_t endpoint = {.adapter = CA_DEFAULT_ADAPTER};
    CARequestInfo_t requestInfo = {.method = CA_GET};

    if(!handle)
    {
        return OC_STACK_INVALID_PARAM;
    }

    ClientCB *clientCB = GetClientCBUsingHandle(handle);
    if (!clientCB)
    {
        OIC_LOG(ERROR, TAG, "Callback not found. Called OCCancel on same resource twice?");
        return OC_STACK_ERROR;
    }

    switch (clientCB->method)
    {
        case OC_REST_OBSERVE:
        case OC_REST_OBSERVE_ALL:

            OIC_LOG_V(INFO, TAG, "Canceling observation for resource %s", clientCB->requestUri);

            CopyDevAddrToEndpoint(clientCB->devAddr, &endpoint);

            if ((endpoint.adapter & CA_ADAPTER_IP) && qos != OC_HIGH_QOS)
            {
                DeleteClientCB(clientCB);
                break;
            }

            OIC_LOG(INFO, TAG, "Cancelling observation as CONFIRMABLE");

            requestInfo.info.type = qualityOfServiceToMessageType(qos);
            requestInfo.info.token = clientCB->token;
            requestInfo.info.tokenLength = clientCB->tokenLength;

            if (CreateObserveHeaderOption (&(requestInfo.info.options),
                    options, numOptions, OC_OBSERVE_DEREGISTER) != OC_STACK_OK)
            {
                return OC_STACK_ERROR;
            }
            requestInfo.info.numOptions = numOptions + 1;
            requestInfo.info.resourceUri = OICStrdup (clientCB->requestUri);


            ret = OCSendRequest(&endpoint, &requestInfo);

            if (requestInfo.info.options)
            {
                OICFree (requestInfo.info.options);
            }
            if (requestInfo.info.resourceUri)
            {
                OICFree (requestInfo.info.resourceUri);
            }

            break;

        case OC_REST_DISCOVER:
            OIC_LOG_V(INFO, TAG, "Cancelling discovery callback for resource %s",
                                           clientCB->requestUri);
            DeleteClientCB(clientCB);
            break;

#ifdef WITH_PRESENCE
        case OC_REST_PRESENCE:
            DeleteClientCB(clientCB);
            break;
#endif
        case OC_REST_GET:
        case OC_REST_PUT:
        case OC_REST_POST:
        case OC_REST_DELETE:
            OIC_LOG_V(INFO, TAG, "Cancelling request callback for resource %s",
                                           clientCB->requestUri);
            DeleteClientCB(clientCB);
            break;

        default:
            ret = OC_STACK_INVALID_METHOD;
            break;
    }

    return ret;
}

/**
 * @brief   Register Persistent storage callback.
 * @param   persistentStorageHandler [IN] Pointers to open, read, write, close & unlink handlers.
 * @return
 *     OC_STACK_OK    - No errors; Success
 *     OC_STACK_INVALID_PARAM - Invalid parameter
 */
OCStackResult OC_CALL OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
{
    OIC_LOG(INFO, TAG, "RegisterPersistentStorageHandler !!");
    if(persistentStorageHandler)
    {
        if( !persistentStorageHandler->open ||
                !persistentStorageHandler->close ||
                !persistentStorageHandler->read ||
                !persistentStorageHandler->unlink ||
                !persistentStorageHandler->write)
        {
            OIC_LOG(ERROR, TAG, "The persistent storage handler is invalid");
            return OC_STACK_INVALID_PARAM;
        }
    }
    g_PersistentStorageHandler = persistentStorageHandler;
    return OC_STACK_OK;
}

OCPersistentStorage *OC_CALL OCGetPersistentStorageHandler()
{
    return g_PersistentStorageHandler;
}

OCStackResult OC_CALL OCProcess(void) EXPORT
{
    /* OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); */
    if (stackState == OC_STACK_UNINITIALIZED)
    {
        OIC_LOG(ERROR, TAG, "OCProcess has failed. ocstack is not initialized");
        return OC_STACK_ERROR;
    }
#ifdef WITH_PRESENCE
    OCProcessPresence();
#endif
    CAHandleRequestResponse();

#ifdef ROUTING_GATEWAY
    RMProcess();
#endif

#ifdef TCP_ADAPTER
    ProcessKeepAlive();
#endif
    return OC_STACK_OK;
}

OCStackResult OC_CALL OCSetDefaultDeviceEntityHandler(OCDeviceEntityHandler entityHandler,
                                                      void* callbackParameter)
{
    defaultDeviceHandler = entityHandler;
    defaultDeviceHandlerCallbackParameter = callbackParameter;

    return OC_STACK_OK;
}

OCTpsSchemeFlags OC_CALL OCGetSupportedEndpointTpsFlags()
{
    return OCGetSupportedTpsFlags();
}

OCStackResult OC_CALL OCCreateResource(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *uri,
        OCEntityHandler entityHandler,
        void *callbackParam,
        uint8_t resourceProperties)
EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s %s ENTRY >>>>>>>>>>>>>>>>", __func__, uri);

    OCStackResult r = OCCreateResourceWithEp(handle,
                                  resourceTypeName,
                                  resourceInterfaceName,
                                  uri, entityHandler,
                                  callbackParam,
                                  resourceProperties,
                                  OC_ALL);
    OIC_LOG_V(INFO, TAG, "%s %s EXIT <<<<<<<<<<<<<<<<", __func__, uri);
    return r;
}

OCStackResult OC_CALL OCCreateResourceWithEp(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *uri, OCEntityHandler entityHandler,
        void *callbackParam,
        uint8_t resourceProperties,
        OCTpsSchemeFlags resourceTpsTypes)
{

    OCResource *pointer = NULL;
    OCStackResult result = OC_STACK_ERROR;

    OIC_LOG_V(DEBUG, TAG, "%s ENTRY; uri: %s", __func__, uri);

    if(myStackMode == OC_CLIENT)
    {
        return OC_STACK_INVALID_PARAM;
    }
    // Validate parameters
    if(!uri || uri[0]=='\0' || strlen(uri)>=MAX_URI_LENGTH )
    {
        OIC_LOG(ERROR, TAG, "URI is empty or too long");
        return OC_STACK_INVALID_URI;
    }
    // Is it presented during resource discovery?
    if (!handle || !resourceTypeName || resourceTypeName[0] == '\0' )
    {
        OIC_LOG(ERROR, TAG, "Input parameter is NULL");
        return OC_STACK_INVALID_PARAM;
    }

    if (!resourceInterfaceName || strlen(resourceInterfaceName) == 0)
    {
        resourceInterfaceName = OC_RSRVD_INTERFACE_DEFAULT;
    }

#ifdef MQ_PUBLISHER
    resourceProperties = resourceProperties | OC_MQ_PUBLISHER;
#endif
    // Make sure resourceProperties bitmask has allowed properties specified
    if (resourceProperties
            > (OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SLOW | OC_NONSECURE | OC_SECURE |
               OC_EXPLICIT_DISCOVERABLE
#ifdef MQ_PUBLISHER
               | OC_MQ_PUBLISHER
#endif
#ifdef MQ_BROKER
               | OC_MQ_BROKER
#endif
               ))
    {
        OIC_LOG(ERROR, TAG, "Invalid property");
        return OC_STACK_INVALID_PARAM;
    }

    // Checking resourceTpsTypes param
    OCTpsSchemeFlags validTps = OC_NO_TPS;
    validTps = (OCTpsSchemeFlags)(validTps | OC_COAP | OC_COAPS);
#ifdef TCP_ADAPTER
    validTps = (OCTpsSchemeFlags)(validTps | OC_COAP_TCP | OC_COAPS_TCP);
#endif
#ifdef HTTP_ADAPTER
    validTps = (OCTpsSchemeFlags)(validTps | OC_HTTP | OC_HTTP);
#endif
#ifdef EDR_ADAPTER
    validTps = (OCTpsSchemeFlags)(validTps | OC_COAP_RFCOMM);
#endif
#ifdef LE_ADAPTER
    validTps = (OCTpsSchemeFlags)(validTps | OC_COAP_GATT);
#endif
#ifdef NFC_ADAPTER
    validTps = (OCTpsSchemeFlags)(validTps | OC_COAP_NFC);
#endif
#ifdef RA_ADAPTER
    validTps = (OCTpsSchemeFlags)(validTps | OC_COAP_RA);
#endif

    if ((resourceTpsTypes < OC_COAP) || ((resourceTpsTypes != OC_ALL) &&
                                         (resourceTpsTypes > validTps)))
    {
        OIC_LOG(ERROR, TAG, "Invalid TPS Types OC_ALL");
        return OC_STACK_INVALID_PARAM;
    }

    // If the headResource is NULL, then no resources have been created...
    pointer = headResource;
    if (pointer)
    {
        // At least one resources is in the resource list, so we need to search for
        // repeated URLs, which are not allowed.  If a repeat is found, exit with an error
        while (pointer)
        {
            if (strncmp(uri, pointer->uri, MAX_URI_LENGTH) == 0)
            {
                OIC_LOG_V(ERROR, TAG, "Resource %s already exists", uri);
                return OC_STACK_INVALID_PARAM;
            }
            pointer = pointer->next;
        }
    }
    // Create the pointer and insert it into the resource list
    pointer = (OCResource *) OICCalloc(1, sizeof(OCResource));
    if (!pointer)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }
    pointer->sequenceNum = OC_OFFSET_SEQUENCE_NUMBER;

    insertResource(pointer);

    // Set the uri
    pointer->uri = OICStrdup(uri);
    if (!pointer->uri)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    // Set resource to nonsecure if caller did not specify
    if ((resourceProperties & OC_MASK_RESOURCE_SECURE) == 0)
    {
        resourceProperties |= OC_NONSECURE;
    }
    OIC_LOG_V(DEBUG, TAG, "Resource properties: 0x%X", resourceProperties);

    // Set properties.  Set OC_ACTIVE
    pointer->resourceProperties = (OCResourceProperty) (resourceProperties
            | OC_ACTIVE);

    OIC_LOG_V(DEBUG, TAG, "Binding resource...");

    // Add the resourcetype to the resource
    result = BindResourceTypeToResource(pointer, resourceTypeName);
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Error adding resourcetype");
        goto exit;
    }

    // Add the resourceinterface to the resource
    result = BindResourceInterfaceToResource(pointer, resourceInterfaceName);
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Error adding resourceinterface");
        goto exit;
    }

    result = BindTpsTypeToResource(pointer, resourceTpsTypes);
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Error adding resource TPS types");
        goto exit;
    }

    // If an entity handler has been passed, attach it to the newly created
    // resource.  Otherwise, set the default entity handler.
    if (entityHandler)
    {
        pointer->entityHandler = entityHandler;
        pointer->entityHandlerCallbackParam = callbackParam;
    }
    else
    {
        pointer->entityHandler = defaultResourceEHandler;
        pointer->entityHandlerCallbackParam = NULL;
    }

    // Initialize a pointer indicating child resources in case of collection
    pointer->rsrcChildResourcesHead = NULL;

    // Initialize a pointer indicating observers to this resource
    pointer->observersHead = NULL;

    *handle = pointer;
    result = OC_STACK_OK;

#ifdef WITH_PRESENCE
    if (presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(pointer->rsrcType, OC_PRESENCE_TRIGGER_CREATE);
    }
#endif
exit:
    if (result != OC_STACK_OK)
    {
        // Deep delete of resource and other dynamic elements that it contains
        deleteResource(pointer);
    }
    return result;
}

OCStackResult OC_CALL OCBindResource(
        OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
{
    OCResource *resource = NULL;
    OCChildResource *tempChildResource = NULL;
    OCChildResource *newChildResource = NULL;

    OIC_LOG(DEBUG, TAG, "Entering OCBindResource");

    // Validate parameters
    VERIFY_NON_NULL(collectionHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(resourceHandle, ERROR, OC_STACK_ERROR);
    // Container cannot contain itself
    if (collectionHandle == resourceHandle)
    {
        OIC_LOG(ERROR, TAG, "Added handle equals collection handle");
        return OC_STACK_INVALID_PARAM;
    }

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *) collectionHandle);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Collection handle not found");
        return OC_STACK_INVALID_PARAM;
    }

    // Look for an open slot to add add the child resource.
    // If found, add it and return success

    tempChildResource = resource->rsrcChildResourcesHead;

    while(resource->rsrcChildResourcesHead && tempChildResource->next)
    {
        // TODO: what if one of child resource was deregistered without unbinding?
        tempChildResource = tempChildResource->next;
    }

    // Do memory allocation for child resource
    newChildResource = (OCChildResource *) OICCalloc(1, sizeof(OCChildResource));
    if(!newChildResource)
    {
        OIC_LOG(ERROR, TAG, "Adding new child resource is failed due to memory allocation failure");
        return OC_STACK_ERROR;
    }

    newChildResource->rsrcResource = (OCResource *) resourceHandle;
    newChildResource->next = NULL;

    if(!resource->rsrcChildResourcesHead)
    {
        resource->rsrcChildResourcesHead = newChildResource;
    }
    else {
        tempChildResource->next = newChildResource;
    }

    OIC_LOG(INFO, TAG, "resource bound");

#ifdef WITH_PRESENCE
    if (presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(((OCResource *) resourceHandle)->rsrcType,
                OC_PRESENCE_TRIGGER_CHANGE);
    }
#endif

    return OC_STACK_OK;
}

OCStackResult OC_CALL OCUnBindResource(
        OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
{
    OCResource *resource = NULL;
    OCChildResource *tempChildResource = NULL;
    OCChildResource *tempLastChildResource = NULL;

    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    // Validate parameters
    VERIFY_NON_NULL(collectionHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(resourceHandle, ERROR, OC_STACK_ERROR);
    // Container cannot contain itself
    if (collectionHandle == resourceHandle)
    {
        OIC_LOG(ERROR, TAG, "removing handle equals collection handle");
        return OC_STACK_INVALID_PARAM;
    }

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *) collectionHandle);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Collection handle not found");
        return OC_STACK_INVALID_PARAM;
    }

    // Look for an open slot to add add the child resource.
    // If found, add it and return success
    if(!resource->rsrcChildResourcesHead)
    {
        OIC_LOG(INFO, TAG, "resource not found in collection");

        // Unable to add resourceHandle, so return error
        return OC_STACK_ERROR;

    }

    tempChildResource = resource->rsrcChildResourcesHead;

    while (tempChildResource)
    {
        if(tempChildResource->rsrcResource == resourceHandle)
        {
            // if resource going to be unbinded is the head one.
            if( tempChildResource == resource->rsrcChildResourcesHead )
            {
                OCChildResource *temp = resource->rsrcChildResourcesHead->next;
                OICFree(resource->rsrcChildResourcesHead);
                resource->rsrcChildResourcesHead = temp;
                temp = NULL;
            }
            else
            {
                OCChildResource *temp = tempChildResource->next;
                OICFree(tempChildResource);
                if (tempLastChildResource)
                {
                    tempLastChildResource->next = temp;
                    temp = NULL;
                }
            }

            OIC_LOG(INFO, TAG, "resource unbound");

            // Send notification when resource is unbounded successfully.
#ifdef WITH_PRESENCE
            if (presenceResource.handle)
            {
                ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
                SendPresenceNotification(((OCResource *) resourceHandle)->rsrcType,
                        OC_PRESENCE_TRIGGER_CHANGE);
            }
#endif
            tempChildResource = NULL;
            tempLastChildResource = NULL;

            return OC_STACK_OK;

        }

        tempLastChildResource = tempChildResource;
        tempChildResource = tempChildResource->next;
    }

    OIC_LOG(INFO, TAG, "resource not found in collection");

    tempChildResource = NULL;
    tempLastChildResource = NULL;

    // Unable to add resourceHandle, so return error
    return OC_STACK_ERROR;
}

static bool ValidateResourceTypeInterface(const char *resourceItemName)
{
    if (!resourceItemName)
    {
        return false;
    }
    // Per RFC 6690 only registered values must follow the first rule below.
    // At this point in time the only values registered begin with "core", and
    // all other values are specified as opaque strings where multiple values
    // are separated by a space.
    if (strncmp(resourceItemName, CORESPEC, sizeof(CORESPEC) - 1) == 0)
    {
        for(size_t index = sizeof(CORESPEC) - 1;  resourceItemName[index]; ++index)
        {
            if (resourceItemName[index] != '.'
                && resourceItemName[index] != '-'
                && (resourceItemName[index] < 'a' || resourceItemName[index] > 'z')
                && (resourceItemName[index] < '0' || resourceItemName[index] > '9'))
            {
                return false;
            }
        }
    }
    else
    {
        for (size_t index = 0; resourceItemName[index]; ++index)
        {
            if (resourceItemName[index] == ' '
                || resourceItemName[index] == '\t'
                || resourceItemName[index] == '\r'
                || resourceItemName[index] == '\n')
            {
                return false;
            }
        }
    }

    return true;
}

/**
 * Bind a resource type to a resource.
 *
 * @param resource Target resource.
 * @param resourceTypeName Name of resource type.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
LOCAL OCStackResult BindResourceTypeToResource(OCResource *resource,
						const char *resourceTypeName)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OCResourceType *pointer = NULL;
    char *str = NULL;
    OCStackResult result = OC_STACK_ERROR;

    VERIFY_NON_NULL(resourceTypeName, ERROR, OC_STACK_INVALID_PARAM);

    if (!ValidateResourceTypeInterface(resourceTypeName))
    {
        OIC_LOG(ERROR, TAG, "resource type illegal (see RFC 6690)");
        return OC_STACK_INVALID_PARAM;
    }

    pointer = (OCResourceType *) OICCalloc(1, sizeof(OCResourceType));
    if (!pointer)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    str = OICStrdup(resourceTypeName);
    if (!str)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }
    pointer->resourcetypename = str;
    pointer->next = NULL;

    insertResourceType(resource, pointer);
    result = OC_STACK_OK;

exit:
    if (result != OC_STACK_OK)
    {
        OICFree(pointer);
        OICFree(str);
    }

    return result;
}

/**
 * Bind a resource interface to a resource.
 *
 * @param resource Target resource.
 * @param resourceInterfaceName Resource interface.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult BindResourceInterfaceToResource(OCResource* resource,
					      const char *resourceInterfaceName)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OCResourceInterface *pointer = NULL;
    char *str = NULL;
    OCStackResult result = OC_STACK_ERROR;

    VERIFY_NON_NULL(resourceInterfaceName, ERROR, OC_STACK_INVALID_PARAM);

    if (!ValidateResourceTypeInterface(resourceInterfaceName))
    {
        OIC_LOG(ERROR, TAG, "resource /interface illegal (see RFC 6690)");
        return OC_STACK_INVALID_PARAM;
    }

    OIC_LOG_V(DEBUG, TAG, "Binding %s interface to %s", resourceInterfaceName, resource->uri);

    pointer = (OCResourceInterface *) OICCalloc(1, sizeof(OCResourceInterface));
    if (!pointer)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    str = OICStrdup(resourceInterfaceName);
    if (!str)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }
    pointer->name = str;

    // Bind the resourceinterface to the resource
    insertResourceInterface(resource, pointer);

    result = OC_STACK_OK;

    exit:
    if (result != OC_STACK_OK)
    {
        OICFree(pointer);
        OICFree(str);
    }

    return result;
}

/**
 * Bind a Transport Protocol Suites type to a resource.
 *
 * @param resource Target resource.
 * @param resourceTpsTypes Name of transport protocol suites type.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
LOCAL OCStackResult BindTpsTypeToResource(OCResource *resource,
					   OCTpsSchemeFlags resourceTpsTypes)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Resource pointer is NULL!!!");
        return OC_STACK_INVALID_PARAM;
    }

    OCTpsSchemeFlags supportedTps = OC_NO_TPS;
    OCStackResult result = OCGetSupportedEndpointFlags(resourceTpsTypes,
                                                       &supportedTps);

    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Failed at get supported endpoint flags");
        return result;
    }

    // If there isn`t any enabled flag, return error for notify to user.
    if (OC_NO_TPS == supportedTps)
    {
        OIC_LOG_V(ERROR, TAG, "There isn`t any enabled flag on resource %s", resource->uri);
        return OC_STACK_BAD_ENDPOINT;
    }

    OIC_LOG_V(DEBUG, TAG, "Binding TPS flags 0x%X to %s", supportedTps, resource->uri);
    resource->endpointType = supportedTps;
    return result;
}

OCStackResult OC_CALL OCBindResourceTypeToResource(OCResourceHandle handle,
        const char *resourceTypeName)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OCStackResult result = OC_STACK_ERROR;
    OCResource *resource = NULL;

    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Resource not found");
        return OC_STACK_ERROR;
    }

    result = BindResourceTypeToResource(resource, resourceTypeName);

#ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(resource->rsrcType, OC_PRESENCE_TRIGGER_CHANGE);
    }
#endif

    return result;
}

OCStackResult OC_CALL OCBindResourceInterfaceToResource(OCResourceHandle handle,
        const char *resourceInterfaceName)
{

    OCStackResult result = OC_STACK_ERROR;
    OCResource *resource = NULL;

    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Resource not found");
        return OC_STACK_ERROR;
    }

    result = BindResourceInterfaceToResource(resource, resourceInterfaceName);

#ifdef WITH_PRESENCE
    if (presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(resource->rsrcType, OC_PRESENCE_TRIGGER_CHANGE);
    }
#endif

    return result;
}

OCStackResult OC_CALL OCGetNumberOfResources(uint8_t *numResources)
{
    OCResource *pointer = headResource;

    VERIFY_NON_NULL(numResources, ERROR, OC_STACK_INVALID_PARAM);
    *numResources = 0;
    while (pointer)
    {
        *numResources = *numResources + 1;
        pointer = pointer->next;
    }
    return OC_STACK_OK;
}

OCResourceHandle OC_CALL OCGetResourceHandle(uint8_t index)
{
    OCResource *pointer = headResource;

    for( uint8_t i = 0; i < index && pointer; ++i)
    {
        pointer = pointer->next;
    }
    return (OCResourceHandle) pointer;
}

OCStackResult OC_CALL OCDeleteResource(OCResourceHandle handle)
{
    if (!handle)
    {
        OIC_LOG(ERROR, TAG, "Invalid handle for deletion");
        return OC_STACK_INVALID_PARAM;
    }

    OCResource *resource = findResource((OCResource *) handle);
    if (resource == NULL)
    {
        OIC_LOG(ERROR, TAG, "Resource not found");
        return OC_STACK_NO_RESOURCE;
    }

    if (deleteResource((OCResource *) handle) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Error deleting resource");
        return OC_STACK_ERROR;
    }

    return OC_STACK_OK;
}

const char *OC_CALL OCGetResourceUri(OCResourceHandle handle)
{
    OCResource *resource = NULL;

    resource = findResource((OCResource *) handle);
    if (resource)
    {
        return resource->uri;
    }
    return (const char *) NULL;
}

OCResourceProperty OC_CALL OCGetResourceProperties(OCResourceHandle handle)
{
    OCResource *resource = NULL;

    resource = findResource((OCResource *) handle);
    if (resource)
    {
        return resource->resourceProperties;
    }
    return (OCResourceProperty)-1;
}

OCStackResult OC_CALL OCSetResourceProperties(OCResourceHandle handle, uint8_t resourceProperties)
{
    OCResource *resource = NULL;

    resource = findResource((OCResource *) handle);
    if (resource == NULL)
    {
        OIC_LOG(ERROR, TAG, "Resource not found");
        return OC_STACK_NO_RESOURCE;
    }
    resource->resourceProperties = (OCResourceProperty) (resource->resourceProperties | resourceProperties);
    return OC_STACK_OK;
}

OCStackResult OC_CALL OCClearResourceProperties(OCResourceHandle handle, uint8_t resourceProperties)
{
    OCResource *resource = NULL;

    resource = findResource((OCResource *) handle);
    if (resource == NULL)
    {
        OIC_LOG(ERROR, TAG, "Resource not found");
        return OC_STACK_NO_RESOURCE;
    }
    resource->resourceProperties = (OCResourceProperty) (resource->resourceProperties & ~resourceProperties);
    return OC_STACK_OK;
}

OCStackResult OC_CALL OCGetNumberOfResourceTypes(OCResourceHandle handle,
        uint8_t *numResourceTypes)
{
    OCResource *resource = NULL;
    OCResourceType *pointer = NULL;

    VERIFY_NON_NULL(numResourceTypes, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);

    *numResourceTypes = 0;

    resource = findResource((OCResource *) handle);
    if (resource)
    {
        pointer = resource->rsrcType;
        while (pointer)
        {
            *numResourceTypes = *numResourceTypes + 1;
            pointer = pointer->next;
        }
    }
    return OC_STACK_OK;
}

const char *OC_CALL OCGetResourceTypeName(OCResourceHandle handle, uint8_t index)
{
    OCResourceType *resourceType = NULL;

    resourceType = findResourceTypeAtIndex(handle, index);
    if (resourceType)
    {
        return resourceType->resourcetypename;
    }
    return (const char *) NULL;
}

OCStackResult OC_CALL OCGetNumberOfResourceInterfaces(OCResourceHandle handle,
        uint8_t *numResourceInterfaces)
{
    OCResourceInterface *pointer = NULL;
    OCResource *resource = NULL;

    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(numResourceInterfaces, ERROR, OC_STACK_INVALID_PARAM);

    *numResourceInterfaces = 0;
    resource = findResource((OCResource *) handle);
    if (resource)
    {
        pointer = resource->rsrcInterface;
        while (pointer)
        {
            *numResourceInterfaces = *numResourceInterfaces + 1;
            pointer = pointer->next;
        }
    }
    return OC_STACK_OK;
}

const char *OC_CALL OCGetResourceInterfaceName(OCResourceHandle handle, uint8_t index)
{
    OCResourceInterface *resourceInterface = NULL;

    resourceInterface = findResourceInterfaceAtIndex(handle, index);
    if (resourceInterface)
    {
        return resourceInterface->name;
    }
    return (const char *) NULL;
}

OCResourceHandle OC_CALL OCGetResourceHandleFromCollection(OCResourceHandle collectionHandle,
        uint8_t index)
{
    OCResource *resource = NULL;
    OCChildResource *tempChildResource = NULL;
    uint8_t num = 0;

    resource = findResource((OCResource *) collectionHandle);
    if (!resource)
    {
        return NULL;
    }

    tempChildResource = resource->rsrcChildResourcesHead;

    while(tempChildResource)
    {
        if( num == index )
        {
            return tempChildResource->rsrcResource;
        }
        num++;
        tempChildResource = tempChildResource->next;
    }

    // In this case, the number of resource handles in the collection exceeds the index
    tempChildResource = NULL;
    return NULL;
}

OCStackResult OC_CALL OCBindResourceHandler(OCResourceHandle handle,
        OCEntityHandler entityHandler,
        void* callbackParam)
{
    OCResource *resource = NULL;

    // Validate parameters
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *)handle);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Resource not found");
        return OC_STACK_ERROR;
    }

    // Bind the handler
    resource->entityHandler = entityHandler;
    resource->entityHandlerCallbackParam = callbackParam;

#ifdef WITH_PRESENCE
    if (presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(resource->rsrcType, OC_PRESENCE_TRIGGER_CHANGE);
    }
#endif

    return OC_STACK_OK;
}

OCEntityHandler OC_CALL OCGetResourceHandler(OCResourceHandle handle)
{
    OCResource *resource = NULL;

    resource = findResource((OCResource *)handle);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Resource not found");
        return NULL;
    }

    // Bind the handler
    return resource->entityHandler;
}

/**
 * Increment resource sequence number.  Handles rollover.
 *
 * @param resPtr Pointer to resource.
 */
static void incrementSequenceNumber(OCResource * resPtr)
{
    // Increment the sequence number
    resPtr->sequenceNum += 1;
    if (resPtr->sequenceNum == MAX_SEQUENCE_NUMBER)
    {
        resPtr->sequenceNum = OC_OFFSET_SEQUENCE_NUMBER+1;
    }
    return;
}

OCStackResult OC_CALL OCNotifyAllObservers(OCResourceHandle handle, OCQualityOfService qos)
{
    OCResource *resPtr = NULL;
    OCStackResult result = OC_STACK_ERROR;
    OCMethod method = OC_REST_NOMETHOD;
    uint32_t maxAge = 0;

    OIC_LOG(INFO, TAG, "Notifying all observers");
#ifdef WITH_PRESENCE
    if(handle == presenceResource.handle)
    {
        return OC_STACK_OK;
    }
#endif // WITH_PRESENCE
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_ERROR);

    // Verify that the resource exists
    resPtr = findResource ((OCResource *) handle);
    if (NULL == resPtr)
    {
        return OC_STACK_NO_RESOURCE;
    }
    else
    {
        //only increment in the case of regular observing (not presence)
        incrementSequenceNumber(resPtr);
        method = OC_REST_OBSERVE;
        maxAge = MAX_OBSERVE_AGE;
#ifdef WITH_PRESENCE
        result = SendAllObserverNotificationWithPresence (method, resPtr, maxAge,
                OC_PRESENCE_TRIGGER_DELETE, NULL, qos);
#else
        result = SendAllObserverNotification (method, resPtr, maxAge, qos);
#endif
        return result;
    }
}

OCStackResult
OC_CALL OCNotifyListOfObservers (OCResourceHandle handle,
                                 OCObservationId  *obsIdList,
                                 uint8_t          numberOfIds,
                                 const OCRepPayload       *payload,
                                 OCQualityOfService qos)
{
    OIC_LOG(INFO, TAG, "Entering OCNotifyListOfObservers");

    OCResource *resPtr = NULL;
    //TODO: we should allow the server to define this
    uint32_t maxAge = MAX_OBSERVE_AGE;

    VERIFY_NON_NULL(handle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(obsIdList, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(payload, ERROR, OC_STACK_ERROR);

    resPtr = findResource ((OCResource *) handle);
    if (NULL == resPtr || myStackMode == OC_CLIENT)
    {
        return OC_STACK_NO_RESOURCE;
    }
    else
    {
        incrementSequenceNumber(resPtr);
    }
    return (SendListObserverNotification(resPtr, obsIdList, numberOfIds,
            payload, maxAge, qos));
}

OCStackResult OC_CALL OCDoResponse(OCEntityHandlerResponse *ehResponse)
{
    OIC_TRACE_BEGIN(%s:OCDoResponse, TAG);
    OCStackResult result = OC_STACK_ERROR;
    OCServerRequest *serverRequest = NULL;

    OIC_LOG(INFO, TAG, "Entering OCDoResponse");

    // Validate input parameters
    VERIFY_NON_NULL(ehResponse, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(ehResponse->requestHandle, ERROR, OC_STACK_INVALID_PARAM);

    // Normal response
    // Get pointer to request info
    serverRequest = (OCServerRequest *)ehResponse->requestHandle;
    if(serverRequest)
    {
        // response handler in ocserverrequest.c. Usually HandleSingleResponse.
        result = serverRequest->ehResponseHandler(ehResponse);
    }

    OIC_TRACE_END();
    return result;
}

//-----------------------------------------------------------------------------
// Private internal function definitions
//-----------------------------------------------------------------------------
/**
 * Generate handle of OCDoResource invocation for callback management.
 *
 * @return Generated OCDoResource handle.
 */
LOCAL OCDoHandle GenerateInvocationHandle()
{
    OCDoHandle handle = NULL;
    // Generate token here, it will be deleted when the transaction is deleted
    handle = (OCDoHandle) OICMalloc(sizeof(uint8_t[CA_MAX_TOKEN_LEN]));
    if (handle)
    {
        if (!OCGetRandomBytes((uint8_t*)handle, sizeof(uint8_t[CA_MAX_TOKEN_LEN])))
        {
            OICFree(handle);
            return NULL;
        }
    }

    return handle;
}

/**
 * Initialize resource data structures, variables, etc.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
LOCAL OCStackResult initResources()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OCStackResult result = OC_STACK_OK;

    headResource = NULL;
    tailResource = NULL;
    // Init Virtual Resources
#ifdef WITH_PRESENCE
    presenceResource.presenceTTL = OC_DEFAULT_PRESENCE_TTL_SECONDS;

    result = OCCreateResource(&presenceResource.handle,
            OC_RSRVD_RESOURCE_TYPE_PRESENCE,
            "core.r",
            OC_RSRVD_PRESENCE_URI,
            NULL,
            NULL,
            OC_OBSERVABLE);
    //make resource inactive
    result = OCChangeResourceProperty(
            &(((OCResource *) presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 0);
#endif
    if (result == OC_STACK_OK)
    {
        result = SRMInitSecureResources();
    }

    if(result == OC_STACK_OK)
    {
        result = OCCreateResource(&wellKnownResource,
                                  OC_RSRVD_RESOURCE_TYPE_RES,
                                  OC_RSRVD_INTERFACE_LL,
                                  OC_RSRVD_WELL_KNOWN_URI,
                                  NULL,
                                  NULL,
                                  0);
        if(result == OC_STACK_OK)
        {
            result = BindResourceInterfaceToResource((OCResource *)wellKnownResource,
                                                     OC_RSRVD_INTERFACE_DEFAULT);
        }
    }

    if(result == OC_STACK_OK)
    {
        CreateResetProfile();
        result = OCCreateResource(&deviceResource,
                                  OC_RSRVD_RESOURCE_TYPE_DEVICE,
                                  OC_RSRVD_INTERFACE_DEFAULT,
                                  OC_RSRVD_DEVICE_URI,
                                  NULL,
                                  NULL,
                                  OC_DISCOVERABLE);
        if(result == OC_STACK_OK)
        {
            result = BindResourceInterfaceToResource((OCResource *)deviceResource,
                                                     OC_RSRVD_INTERFACE_READ);
        }
    }

    if(result == OC_STACK_OK)
    {
        result = OCCreateResource(&platformResource,
                                  OC_RSRVD_RESOURCE_TYPE_PLATFORM,
                                  OC_RSRVD_INTERFACE_DEFAULT,
                                  OC_RSRVD_PLATFORM_URI,
                                  NULL,
                                  NULL,
                                  OC_DISCOVERABLE);
        if(result == OC_STACK_OK)
        {
            result = BindResourceInterfaceToResource((OCResource *)platformResource,
                                                     OC_RSRVD_INTERFACE_READ);
        }
    }

    if (result == OC_STACK_OK)
    {
        result = OCCreateResource(&introspectionResource,
                                  OC_RSRVD_RESOURCE_TYPE_INTROSPECTION,
                                  OC_RSRVD_INTERFACE_DEFAULT,
                                  OC_RSRVD_INTROSPECTION_URI_PATH,
                                  NULL,
                                  NULL,
                                  OC_DISCOVERABLE | OC_SECURE);
        if (result == OC_STACK_OK)
        {
            result = BindResourceInterfaceToResource((OCResource *)introspectionResource,
                                                     OC_RSRVD_INTERFACE_READ);
        }
    }

    if (result == OC_STACK_OK)
    {
        result = OCCreateResource(&introspectionPayloadResource,
                                  OC_RSRVD_RESOURCE_TYPE_INTROSPECTION_PAYLOAD,
                                  OC_RSRVD_INTERFACE_DEFAULT,
                                  OC_RSRVD_INTROSPECTION_PAYLOAD_URI_PATH,
                                  NULL,
                                  NULL,
                                  0);
        if (result == OC_STACK_OK)
        {
            result = BindResourceInterfaceToResource((OCResource *)introspectionPayloadResource,
                                                     OC_RSRVD_INTERFACE_READ);
        }
    }

    // Initialize Device Properties
    if (OC_STACK_OK == result)
    {
        result = InitializeDeviceProperties();
    }

    // Initialize platform ID of OC_RSRVD_RESOURCE_TYPE_PLATFORM.
    // Multiple devices or applications running on the same IoTivity platform should have the same
    // platform ID.
    if (OC_STACK_OK == result)
    {
        uint8_t platformID[OIC_UUID_LENGTH];
        char uuidString[UUID_STRING_SIZE];

        if (!OICGetPlatformUuid(platformID))
        {
            OIC_LOG(WARNING, TAG, "Failed OICGetPlatformUuid(), generate random uuid.");
            OCGenerateUuid(platformID);
        }

        if (OCConvertUuidToString(platformID, uuidString))
        {
            // Set the platform ID.
            // Application can overwrite the value set here by calling similar
            // OCSetPropertyValue(OC_RSRVD_PLATFORM_ID, ...).
            result = OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_PLATFORM_ID, uuidString);
        }
        else
        {
            result = OC_STACK_ERROR;
            OIC_LOG(ERROR, TAG, "Failed OCConvertUuidToString() for platform ID.");
        }
    }

    return result;
}

/**
 * Add a resource to the end of the linked list of resources.
 *
 * @param resource Resource to be added
 */
LOCAL void insertResource(OCResource *resource)
{
    if (!headResource)
    {
        headResource = resource;
        tailResource = resource;
    }
    else
    {
        tailResource->next = resource;
        tailResource = resource;
    }
    resource->next = NULL;
}

/**
 * Find a resource in the linked list of resources.
 *
 * @param resource Resource to be found.
 * @return Pointer to resource that was found in the linked list or NULL if the resource was not
 *         found.
 */
OCResource *findResource(OCResource *resource)
{
    OCResource *pointer = headResource;

    while (pointer)
    {
        if (pointer == resource)
        {
            return resource;
        }
        pointer = pointer->next;
    }
    return NULL;
}

/**
 * Delete all of the resources in the resource list.
 */
LOCAL void deleteAllResources()
{
    OCResource *pointer = headResource;
    OCResource *temp = NULL;

    while (pointer)
    {
        temp = pointer->next;
#ifdef WITH_PRESENCE
        if (pointer != (OCResource *) presenceResource.handle)
        {
#endif // WITH_PRESENCE
            deleteResource(pointer);
#ifdef WITH_PRESENCE
        }
#endif // WITH_PRESENCE
        pointer = temp;
    }
    memset(&platformResource, 0, sizeof(platformResource));
    memset(&deviceResource, 0, sizeof(deviceResource));
    memset(&wellKnownResource, 0, sizeof(wellKnownResource));
#ifdef MQ_BROKER
    memset(&brokerResource, 0, sizeof(brokerResource));
#endif

    SRMDeInitSecureResources();

#ifdef WITH_PRESENCE
    // Ensure that the last resource to be deleted is the presence resource. This allows for all
    // presence notification attributed to their deletion to be processed.
    deleteResource((OCResource *) presenceResource.handle);
    memset(&presenceResource, 0, sizeof(presenceResource));
#endif // WITH_PRESENCE
}


/**
 * Delete resource specified by handle.  Deletes resource and all resourcetype and resourceinterface
 * linked lists.
 *
 * @param handle Handle of resource to be deleted.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
LOCAL OCStackResult deleteResource(OCResource *resource)
{
    OCResource *prev = NULL;
    OCResource *temp = NULL;
    if(!resource)
    {
        OIC_LOG(DEBUG,TAG,"resource is NULL");
        return OC_STACK_INVALID_PARAM;
    }

    OIC_LOG_V (INFO, TAG, "Deleting resource %s", resource->uri);

    temp = headResource;
    while (temp)
    {
        if (temp == resource)
        {
            // Invalidate all Resource Properties.
            resource->resourceProperties = (OCResourceProperty) 0;
#ifdef WITH_PRESENCE
            if(resource != (OCResource *) presenceResource.handle)
            {
#endif // WITH_PRESENCE
                OCNotifyAllObservers((OCResourceHandle)resource, OC_HIGH_QOS);
#ifdef WITH_PRESENCE
            }

            if(presenceResource.handle)
            {
                ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
                SendPresenceNotification(resource->rsrcType, OC_PRESENCE_TRIGGER_DELETE);
            }
#endif
            // Only resource in list.
            if (temp == headResource && temp == tailResource)
            {
                headResource = NULL;
                tailResource = NULL;
            }
            // Deleting head.
            else if (temp == headResource)
            {
                headResource = temp->next;
            }
            // Deleting tail.
            else if (temp == tailResource && prev)
            {
                tailResource = prev;
                tailResource->next = NULL;
            }
            else if (prev)
            {
                prev->next = temp->next;
            }

            deleteResourceElements(temp);
            OICFree(temp);
            temp = NULL;
            return OC_STACK_OK;
        }
        else
        {
            prev = temp;
            temp = temp->next;
        }
    }

    return OC_STACK_ERROR;
}

/**
 * Delete all of the dynamically allocated elements that were created for the resource.
 *
 * @param resource Specified resource.
 */
LOCAL void deleteResourceElements(OCResource *resource)
{
    if (!resource)
    {
        return;
    }

    if (resource->uri)
    {
        OICFree(resource->uri);
    }
    if (resource->rsrcType)
    {
        deleteResourceType(resource->rsrcType);
    }
    if (resource->rsrcInterface)
    {
        deleteResourceInterface(resource->rsrcInterface);
    }
    if (resource->rsrcChildResourcesHead)
    {
        OICFree(resource->rsrcChildResourcesHead);
    }
    if (resource->rsrcAttributes)
    {
        OCDeleteResourceAttributes(resource->rsrcAttributes);
    }

    DeleteObserverList(resource);
}

/**
 * Delete all of the dynamically allocated elements that were created for the resource type.
 *
 * @param resourceType Specified resource type.
 */
LOCAL void deleteResourceType(OCResourceType *resourceType)
{
    OCResourceType *next = NULL;

    for (OCResourceType *pointer = resourceType; pointer; pointer = next)
    {
        next = pointer->next;
        if (pointer->resourcetypename)
        {
            OICFree(pointer->resourcetypename);
        }
        OICFree(pointer);
    }
}

/**
 * Delete all of the dynamically allocated elements that were created for the resource interface.
 *
 * @param resourceInterface Specified resource interface.
 */
LOCAL void deleteResourceInterface(OCResourceInterface *resourceInterface)
{
    OCResourceInterface *next = NULL;
    for (OCResourceInterface *pointer = resourceInterface; pointer; pointer = next)
    {
        next = pointer->next;
        if (pointer->name)
        {
            OICFree(pointer->name);
        }
        OICFree(pointer);
    }
}

/**
 * Delete all of the dynamically allocated elements that were created for the resource attributes.
 *
 * @param rsrcAttributes Specified resource attribute.
 */
void OCDeleteResourceAttributes(OCAttribute *rsrcAttributes)
{
    OCAttribute *next = NULL;
    for (OCAttribute *pointer = rsrcAttributes; pointer; pointer = next)
    {
        next = pointer->next;
        if (pointer->attrName && 0 == strcmp(OC_RSRVD_DATA_MODEL_VERSION, pointer->attrName))
        {
            OCFreeOCStringLL((OCStringLL *)pointer->attrValue);
        }
        else if (pointer->attrValue)
        {
            OICFree(pointer->attrValue);
        }
        if (pointer->attrName)
        {
            OICFree(pointer->attrName);
        }
        OICFree(pointer);
    }
}

/**
 * Insert a resource type into a resource's resource type linked list.
 * If resource type already exists, it will not be inserted and the
 * resourceType will be free'd.
 * resourceType->next should be null to avoid memory leaks.
 * Function returns silently for null args.
 *
 * @param resource Resource where resource type is to be inserted.
 * @param resourceType Resource type to be inserted.
 */
LOCAL void insertResourceType(OCResource *resource,
        OCResourceType *resourceType)
{
    OCResourceType *pointer = NULL;
    OCResourceType *previous = NULL;
    if (!resource || !resourceType)
    {
        return;
    }
    // resource type list is empty.
    else if (!resource->rsrcType)
    {
        resource->rsrcType = resourceType;
    }
    else
    {
        pointer = resource->rsrcType;

        while (pointer)
        {
            if (!strcmp(resourceType->resourcetypename, pointer->resourcetypename))
            {
                OIC_LOG_V(INFO, TAG, "Type %s already exists", resourceType->resourcetypename);
                OICFree(resourceType->resourcetypename);
                OICFree(resourceType);
                return;
            }
            previous = pointer;
            pointer = pointer->next;
        }

        if (previous)
        {
            previous->next = resourceType;
        }
    }
    resourceType->next = NULL;

    OIC_LOG_V(DEBUG, TAG, "Added type %s to %s", resourceType->resourcetypename, resource->uri);
}

/**
 * Get a resource type at the specified index within a resource.
 *
 * @param handle Handle of resource.
 * @param index Index of resource type.
 *
 * @return Pointer to resource type if found, NULL otherwise.
 */
LOCAL OCResourceType *findResourceTypeAtIndex(OCResourceHandle handle,
					       uint8_t index)
{
    OCResource *resource = NULL;
    OCResourceType *pointer = NULL;

    // Find the specified resource
    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        return NULL;
    }

    // Make sure a resource has a resourcetype
    if (!resource->rsrcType)
    {
        return NULL;
    }

    // Iterate through the list
    pointer = resource->rsrcType;
    for(uint8_t i = 0; i< index && pointer; ++i)
    {
        pointer = pointer->next;
    }
    return pointer;
}

/**
 * Finds a resource type in an OCResourceType link-list.
 *
 * @param resourceTypeList The link-list to be searched through.
 * @param resourceTypeName The key to search for.
 *
 * @return Resource type that matches the key (ie. resourceTypeName) or
 *      NULL if there is either an invalid parameter or this function was unable to find the key.
 */
OCResourceType *findResourceType(OCResourceType * resourceTypeList, const char * resourceTypeName)
{
    if(resourceTypeList && resourceTypeName)
    {
        OCResourceType * rtPointer = resourceTypeList;
        while(resourceTypeName && rtPointer)
        {
            OIC_LOG_V(DEBUG, TAG, "current resourceType : %s", rtPointer->resourcetypename);
            if(rtPointer->resourcetypename &&
                    strcmp(resourceTypeName, (const char *)
                    (rtPointer->resourcetypename)) == 0)
            {
                break;
            }
            rtPointer = rtPointer->next;
        }
        return rtPointer;
    }
    return NULL;
}

/*
 * Insert a new interface into interface linked list only if not already present.
 * If alredy present, 2nd arg is free'd.
 * Default interface will always be first if present.
 */
/**
 * Insert a resource interface into a resource's resource interface linked list.
 * If resource interface already exists, it will not be inserted and the
 * resourceInterface will be free'd.
 * resourceInterface->next should be null to avoid memory leaks.
 *
 * @param resource Resource where resource interface is to be inserted.
 * @param resourceInterface Resource interface to be inserted.
 */
LOCAL void insertResourceInterface(OCResource *resource,
				    OCResourceInterface *newInterface)
{
    OCResourceInterface *pointer = NULL;
    OCResourceInterface *previous = NULL;

    newInterface->next = NULL;

    OCResourceInterface **firstInterface = &(resource->rsrcInterface);

    if (!*firstInterface)
    {
        // If first interface is not oic.if.baseline, by default add it as first interface type.
        if (0 == strcmp(newInterface->name, OC_RSRVD_INTERFACE_DEFAULT))
        {
            *firstInterface = newInterface;
        }
        else
        {
            OCStackResult result = BindResourceInterfaceToResource(resource,
                                                                    OC_RSRVD_INTERFACE_DEFAULT);
            if (result != OC_STACK_OK)
            {
                OICFree(newInterface->name);
                OICFree(newInterface);
                return;
            }
            if (*firstInterface)
            {
                (*firstInterface)->next = newInterface;
            }
        }
    }
    // If once add oic.if.baseline, later too below code take care of freeing memory.
    else if (strcmp(newInterface->name, OC_RSRVD_INTERFACE_DEFAULT) == 0)
    {
        if (strcmp((*firstInterface)->name, OC_RSRVD_INTERFACE_DEFAULT) == 0)
        {
            OICFree(newInterface->name);
            OICFree(newInterface);
            return;
        }
        // This code will not hit anymore, keeping
        else
        {
            newInterface->next = *firstInterface;
            *firstInterface = newInterface;
        }
    }
    else
    {
        pointer = *firstInterface;
        while (pointer)
        {
            if (strcmp(newInterface->name, pointer->name) == 0)
            {
                OICFree(newInterface->name);
                OICFree(newInterface);
                return;
            }
            previous = pointer;
            pointer = pointer->next;
        }

        if (previous)
        {
            previous->next = newInterface;
        }
    }
}

/**
 * Get a resource interface at the specified index within a resource.
 *
 * @param handle Handle of resource.
 * @param index Index of resource interface.
 *
 * @return Pointer to resource interface if found, NULL otherwise.
 */
LOCAL OCResourceInterface *findResourceInterfaceAtIndex(OCResourceHandle handle, uint8_t index)
{
    OCResource *resource = NULL;
    OCResourceInterface *pointer = NULL;

    // Find the specified resource
    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        return NULL;
    }

    // Make sure a resource has a resourceinterface
    if (!resource->rsrcInterface)
    {
        return NULL;
    }

    // Iterate through the list
    pointer = resource->rsrcInterface;

    for (uint8_t i = 0; i < index && pointer; ++i)
    {
        pointer = pointer->next;
    }
    return pointer;
}

/*
 * This function splits the uri using the '?' delimiter.
 * "uriWithoutQuery" is the block of characters between the beginning
 * till the delimiter or '\0' which ever comes first.
 * "query" is whatever is to the right of the delimiter if present.
 * No delimiter sets the query to NULL.
 * If either are present, they will be malloc'ed into the params 2, 3.
 * The first param, *uri is left untouched.

 * NOTE: This function does not account for whitespace at the end of the uri NOR
 *       malformed uri's with '??'. Whitespace at the end will be assumed to be
 *       part of the query.
 */
/**
 * Extract query from a URI.
 *
 * @param uri Full URI with query.
 * @param query Pointer to string that will contain query.
 * @param newURI Pointer to string that will contain URI.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
/* static OCStackResult getQueryFromUri(const char * uri, char** resourceType, char ** newURI); */
LOCAL OCStackResult getQueryFromUri(const char * uri, char** query, char ** uriWithoutQuery)
{
    if(!uri)
    {
        return OC_STACK_INVALID_URI;
    }
    if(!query || !uriWithoutQuery)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *query           = NULL;
    *uriWithoutQuery = NULL;

    size_t uriWithoutQueryLen = 0;
    size_t queryLen = 0;
    size_t uriLen = strlen(uri);

    char *pointerToDelimiter = strstr(uri, "?");

    uriWithoutQueryLen = pointerToDelimiter == NULL ? uriLen : (size_t)(pointerToDelimiter - uri);
    queryLen = pointerToDelimiter == NULL ? 0 : uriLen - uriWithoutQueryLen - 1;

    if (uriWithoutQueryLen)
    {
        *uriWithoutQuery =  (char *) OICCalloc(uriWithoutQueryLen + 1, 1);
        if (!*uriWithoutQuery)
        {
            goto exit;
        }
        OICStrcpy(*uriWithoutQuery, uriWithoutQueryLen +1, uri);
    }
    if (queryLen)
    {
        *query = (char *) OICCalloc(queryLen + 1, 1);
        if (!*query)
        {
            OICFree(*uriWithoutQuery);
            *uriWithoutQuery = NULL;
            goto exit;
        }
        OICStrcpy(*query, queryLen + 1, pointerToDelimiter + 1);
    }

    return OC_STACK_OK;

    exit:
        return OC_STACK_NO_MEMORY;
}

static const OicUuid_t* OC_CALL OCGetServerInstanceID(void)
{
    static bool generated = false;
    static OicUuid_t sid;
    if (generated)
    {
        return &sid;
    }

    if (OC_STACK_OK != GetDoxmDeviceID(&sid))
    {
        OIC_LOG(FATAL, TAG, "Generate UUID for Server Instance failed!");
        return NULL;
    }
    generated = true;
    return &sid;
}

const char* OC_CALL OCGetServerInstanceIDString(void)
{
    static bool generated = false;
    static char sidStr[UUID_STRING_SIZE];

    if (generated)
    {
        return sidStr;
    }

    const OicUuid_t *sid = OCGetServerInstanceID();
    if(sid && !OCConvertUuidToString(sid->id, sidStr))
    {
        OIC_LOG(FATAL, TAG, "Generate UUID String for Server Instance failed!");
        return NULL;
    }

    generated = true;
    return sidStr;
}

/*
 * Attempts to initialize every network interface that the CA Layer might have compiled in.
 *
 * Note: At least one interface must succeed to initialize. If all calls to @ref CASelectNetwork
 * return something other than @ref CA_STATUS_OK, then this function fails.
 * @param transportType  OCTransportAdapter value to select.
 * @return ::CA_STATUS_OK on success, some other value upon failure.
 */
LOCAL CAResult_t OCSelectNetwork(OCTransportAdapter transportType)
{
    OIC_LOG_V(DEBUG, TAG, "OCSelectNetwork [%d]", transportType);
    CAResult_t retResult = CA_STATUS_FAILED;
    CAResult_t caResult = CA_STATUS_OK;

    CATransportAdapter_t connTypes[] = {
            CA_ADAPTER_IP,
            CA_ADAPTER_RFCOMM_BTEDR,
            CA_ADAPTER_GATT_BTLE,
            CA_ADAPTER_NFC
#ifdef RA_ADAPTER
            ,CA_ADAPTER_REMOTE_ACCESS
#endif

#ifdef TCP_ADAPTER
            ,CA_ADAPTER_TCP
#endif
        };
    int numConnTypes = sizeof(connTypes)/sizeof(connTypes[0]);

    for(int i = 0; i < numConnTypes; i++)
    {
        // If CA status is not initialized, CASelectNetwork() will not be called.
        if (caResult != CA_STATUS_NOT_INITIALIZED)
        {
            if ((connTypes[i] & transportType) || (OC_DEFAULT_ADAPTER == transportType))
            {
                OIC_LOG_V(DEBUG, TAG, "call CASelectNetwork [%d]", connTypes[i]);
                caResult = CASelectNetwork(connTypes[i]);
                if (caResult == CA_STATUS_OK)
                {
                    retResult = CA_STATUS_OK;
                }
            }
            else
            {
                OIC_LOG_V(DEBUG, TAG, "there is no transport type [%d]", connTypes[i]);
            }
        }
    }

    if (retResult != CA_STATUS_OK)
    {
        return caResult; // Returns error of appropriate transport that failed fatally.
    }

    return retResult;
}


/**
 * Converts a CAResult_t type to a OCStackResult type.
 *
 * @param caResult CAResult_t value to convert.
 * @return OCStackResult that was converted from the input CAResult_t value.
 */
OCStackResult CAResultToOCResult(CAResult_t caResult) /* GAR: support all CAResult_t codes */
{
    switch (caResult)
    {
        case CA_STATUS_OK:
            return OC_STACK_OK;
        case CA_STATUS_INVALID_PARAM:
            return OC_STACK_INVALID_PARAM;
        case CA_ADAPTER_NOT_ENABLED:
            return OC_STACK_ADAPTER_NOT_ENABLED;
        case CA_SERVER_STARTED_ALREADY:
            /* return OC_STACK_CA_SERVER_STARTED_ALREADY; */
            return OC_STACK_OK;
        case CA_SERVER_NOT_STARTED:
            /* return OC_STACK_ERROR; */
            return OC_STACK_CA_SERVER_NOT_STARTED;
        case CA_DESTINATION_NOT_REACHABLE:
            return OC_STACK_COMM_ERROR;
        case CA_SOCKET_OPERATION_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_SEND_FAILED:
            return OC_STACK_COMM_ERROR;
            /* return OC_STACK_CA_SEND_FAILED; */
        case CA_RECEIVE_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_MEMORY_ALLOC_FAILED:
            return OC_STACK_NO_MEMORY;
        case CA_REQUEST_TIMEOUT:
            return OC_STACK_TIMEOUT;
        case CA_DESTINATION_DISCONNECTED:
            return OC_STACK_COMM_ERROR;
        case CA_STATUS_FAILED:
            return OC_STACK_ERROR;
        case CA_NOT_SUPPORTED:
            return OC_STACK_NOTIMPL;
        case CA_HANDLE_ERROR_OTHER_MODULE:
            return OC_STACK_COMM_ERROR;
        case CA_CONTINUE_OPERATION:
            return OC_STACK_CONTINUE_OPERATION;
        default:
            return OC_STACK_ERROR;
    }
}


/**
 * Converts a OCStackResult type to a bool type.
 *
 * @param ocResult OCStackResult value to convert.
 * @return true on success, false upon failure.
 */
LOCAL bool OCResultToSuccess(OCStackResult ocResult)
{
    switch (ocResult)
    {
        case OC_STACK_OK:
        case OC_STACK_RESOURCE_CREATED:
        case OC_STACK_RESOURCE_DELETED:
        case OC_STACK_CONTINUE:
        case OC_STACK_RESOURCE_CHANGED:
        case OC_STACK_SLOW_RESOURCE:
            return true;
        default:
            return false;
    }
}

OCStackResult OC_CALL OCSetProxyURI(const char *uri)
{
    return CAResultToOCResult(CASetProxyUri(uri));
}

#if defined(RD_CLIENT) || defined(RD_SERVER)
OCStackResult OC_CALL OCBindResourceInsToResource(OCResourceHandle handle, int64_t ins)
{
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);

    OCResource *resource = NULL;

    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Resource not found");
        return OC_STACK_ERROR;
    }

    resource->ins = ins;

    return OC_STACK_OK;
}

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
                                              const OCClientResponse *response)
{
    // Validate input parameters
    VERIFY_NON_NULL(requestUri, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(response, ERROR, OC_STACK_INVALID_PARAM);

    char *targetUri = (char *) OICMalloc(strlen(requestUri) + 1);
    if (!targetUri)
    {
        return OC_STACK_NO_MEMORY;
    }
    strncpy(targetUri, requestUri, strlen(requestUri) + 1);

    if (response->result == OC_STACK_RESOURCE_CHANGED) // publish message
    {
        OIC_LOG(DEBUG, TAG, "update the ins of published resource");

        if (strcmp(OC_RSRVD_RD_URI, targetUri) == 0)
        {
            // Update resource unique id in stack.
            if (response)
            {
                if (response->payload)
                {
                    OCRepPayload *rdPayload = (OCRepPayload *) response->payload;
                    OCRepPayload **links = NULL;
                    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };
                    if (OCRepPayloadGetPropObjectArray(rdPayload, OC_RSRVD_LINKS,
                                                       &links, dimensions))
                    {
                        size_t i = 0;
                        for (; i < dimensions[0]; i++)
                        {
                            char *uri = NULL;
                            if (OCRepPayloadGetPropString(links[i], OC_RSRVD_HREF, &uri))
                            {
                                OCResourceHandle handle = OCGetResourceHandleAtUri(uri);
                                int64_t ins = 0;
                                if (OCRepPayloadGetPropInt(links[i], OC_RSRVD_INS, &ins))
                                {
                                    OCBindResourceInsToResource(handle, ins);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (response->result == OC_STACK_RESOURCE_DELETED) // delete message
    {
        OIC_LOG(DEBUG, TAG, "update the ins of deleted resource with 0");

        uint8_t numResources = 0;
        OCGetNumberOfResources(&numResources);

        char *ins = strstr(targetUri, OC_RSRVD_INS);
        if (!ins)
        {
            for (uint8_t i = 0; i < numResources; i++)
            {
                OCResourceHandle resHandle = OCGetResourceHandle(i);
                if (resHandle)
                {
                    OCBindResourceInsToResource(resHandle, 0);
                }
            }
        }
        else
        {
            const char *token = "&";
            char *iterTokenPtr = NULL;
            char *start = strtok_r(targetUri, token, &iterTokenPtr);

             while (start != NULL)
             {
                 char *query = start;
                 query = strstr(query, OC_RSRVD_INS);
                 if (query)
                 {
                     // Arduino's AVR-GCC doesn't support strtoll().
                     int64_t queryIns;
                     int matchedItems = sscanf(query + 4, "%" SCNd64, &queryIns);

                     if (0 == matchedItems)
                     {
                         OICFree(targetUri);
                         return OC_STACK_INVALID_QUERY;
                     }

                     for (uint8_t i = 0; i < numResources; i++)
                     {
                         OCResourceHandle resHandle = OCGetResourceHandle(i);
                         if (resHandle)
                         {
                             int64_t resIns = 0;
                             if (OC_STACK_OK == OCGetResourceIns(resHandle, &resIns))
                             {
                                 if (queryIns && queryIns == resIns)
                                 {
                                     OCBindResourceInsToResource(resHandle, 0);
                                     break;
                                 }
                             }
                             else
                             {
                                 OIC_LOG(ERROR, TAG, "Get resource instance failed!");
                             }
                         }
                     }
                 }
                 start = strtok_r(NULL, token, &iterTokenPtr);
             }
        }
    }

    OICFree(targetUri);
    return OC_STACK_OK;
}
#endif

OCStackResult OC_CALL OCGetResourceIns(OCResourceHandle handle, int64_t* ins)
{
    OCResource *resource = NULL;

    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(ins, ERROR, OC_STACK_INVALID_PARAM);

    resource = findResource((OCResource *) handle);
    if (resource)
    {
        *ins = resource->ins;
        return OC_STACK_OK;
    }
    return OC_STACK_ERROR;
}
#endif // RD_CLIENT || RD_SERVER

OCResourceHandle OC_CALL OCGetResourceHandleAtUri(const char *uri)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource uri is NULL");
        return NULL;
    }

    OCResource *pointer = headResource;

    while (pointer)
    {
        if (strncmp(uri, pointer->uri, MAX_URI_LENGTH) == 0)
        {
            OIC_LOG_V(DEBUG, TAG, "Found Resource %s", uri);
            return pointer;
        }
        pointer = pointer->next;
    }
    return NULL;
}

/**
 * Set Header Option.
 * @param caHdrOpt            Pointer to existing options
 * @param numOptions          Number of existing options.
 * @param optionID            COAP option ID.
 * @param optionData          Option data value.
 * @param optionDataLength    Size of Option data value.

 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
LOCAL OCStackResult SetHeaderOption(CAHeaderOption_t *caHdrOpt, size_t numOptions,
				     uint16_t optionID, void* optionData, size_t optionDataLength)
{
    if (!caHdrOpt)
    {
        return OC_STACK_INVALID_PARAM;
    }

    if (!optionData)
    {
        OIC_LOG (INFO, TAG, "optionData are NULL");
        return OC_STACK_INVALID_PARAM;
    }

    caHdrOpt[numOptions].protocolID = CA_COAP_ID;
    caHdrOpt[numOptions].optionID = optionID;
    caHdrOpt[numOptions].optionLength =
            (optionDataLength < MAX_HEADER_OPTION_DATA_LENGTH) ?
                    (uint16_t) optionDataLength : MAX_HEADER_OPTION_DATA_LENGTH;
    memcpy(caHdrOpt[numOptions].optionData, (const void*) optionData,
            caHdrOpt[numOptions].optionLength);

    return OC_STACK_OK;
}

OCStackResult OC_CALL OCSetHeaderOption(OCHeaderOption* ocHdrOpt, size_t* numOptions, uint16_t optionID,
                                        void* optionData, size_t optionDataLength)
{
    if (!ocHdrOpt)
    {
        OIC_LOG (INFO, TAG, "Header options are NULL");
        return OC_STACK_INVALID_PARAM;
    }

    if (!optionData)
    {
        OIC_LOG (INFO, TAG, "optionData are NULL");
        return OC_STACK_INVALID_PARAM;
    }

    if (!numOptions)
    {
        OIC_LOG (INFO, TAG, "numOptions is NULL");
        return OC_STACK_INVALID_PARAM;
    }

    if (*numOptions >= MAX_HEADER_OPTIONS)
    {
        OIC_LOG (INFO, TAG, "Exceeding MAX_HEADER_OPTIONS");
        return OC_STACK_NO_MEMORY;
    }

    ocHdrOpt += *numOptions;
    ocHdrOpt->protocolID = OC_COAP_ID;
    ocHdrOpt->optionID = optionID;
    ocHdrOpt->optionLength =
            (optionDataLength < MAX_HEADER_OPTION_DATA_LENGTH) ?
                    (uint16_t)optionDataLength : MAX_HEADER_OPTION_DATA_LENGTH;
    memcpy(ocHdrOpt->optionData, (const void*) optionData, ocHdrOpt->optionLength);
    *numOptions += 1;

    return OC_STACK_OK;
}

OCStackResult OC_CALL OCGetHeaderOption(OCHeaderOption* ocHdrOpt, size_t numOptions,
                                        uint16_t optionID, void* optionData, size_t optionDataLength,
                                        uint16_t* receivedDataLength)
EXPORT
{
    if (!ocHdrOpt || !numOptions)
    {
        OIC_LOG (INFO, TAG, "No options present");
        return OC_STACK_OK;
    }

    if (!optionData)
    {
        OIC_LOG (INFO, TAG, "optionData are NULL");
        return OC_STACK_INVALID_PARAM;
    }

    if (!receivedDataLength)
    {
        OIC_LOG (INFO, TAG, "receivedDataLength is NULL");
        return OC_STACK_INVALID_PARAM;
    }

    for (size_t i = 0; i < numOptions; i++)
    {
        if (ocHdrOpt[i].optionID == optionID)
        {
            if (optionDataLength >= ocHdrOpt->optionLength)
            {
                memcpy(optionData, ocHdrOpt[i].optionData, ocHdrOpt[i].optionLength);
                *receivedDataLength = ocHdrOpt[i].optionLength;
                return OC_STACK_OK;
            }
            else
            {
                OIC_LOG (ERROR, TAG, "optionDataLength is less than the length of received data");
                return OC_STACK_ERROR;
            }
        }
    }
    return OC_STACK_OK;
}

/**
 * default adapter state change callback method
 *
 * @param adapter   CA network adapter type.
 * @param enabled   current adapter state.
 */
LOCAL void OCDefaultAdapterStateChangedHandler(CATransportAdapter_t adapter, bool enabled)
{
    OIC_LOG(DEBUG, TAG, "OCDefaultAdapterStateChangedHandler");

    OC_UNUSED(adapter);
    OC_UNUSED(enabled);

#ifdef WITH_PRESENCE
    if (presenceResource.handle)
    {
        OCResource *resourceHandle = (OCResource *)presenceResource.handle;
        resourceHandle->sequenceNum = OCGetRandom();
        SendPresenceNotification(resourceHandle->rsrcType, OC_PRESENCE_TRIGGER_CHANGE);
    }
#endif
}

/**
 * default connection state change callback method
 *
 * @param info          CAEndpoint which has address, port and etc.
 * @param isConnected   current connection state.
 */
LOCAL void OCDefaultConnectionStateChangedHandler(const CAEndpoint_t *info, bool isConnected)
{
    OIC_LOG(DEBUG, TAG, "OCDefaultConnectionStateChangedHandler");

#ifdef WITH_PRESENCE
    if (presenceResource.handle)
    {
        OCResource *resourceHandle = (OCResource *)presenceResource.handle;
        resourceHandle->sequenceNum = OCGetRandom();
        SendPresenceNotification(resourceHandle->rsrcType, OC_PRESENCE_TRIGGER_CHANGE);
    }
#endif

    /*
     * If the client observes one or more resources over a reliable connection,
     * then the CoAP server (or intermediary in the role of the CoAP server)
     * MUST remove all entries associated with the client endpoint from the lists
     * of observers when the connection is either closed or times out.
     */
    if (!isConnected)
    {
        OCDevAddr devAddr = { OC_DEFAULT_ADAPTER };
        CopyEndpointToDevAddr(info, &devAddr);

        // remove observer list with remote device address.
        GiveStackFeedBackObserverNotInterested(&devAddr);
    }
}

OCStackResult OC_CALL OCGetDeviceId(OCUUIdentity *deviceId)
{
    OicUuid_t oicUuid;
    OCStackResult ret = OC_STACK_ERROR;

    ret = GetDoxmDeviceID(&oicUuid);
    if (OC_STACK_OK == ret)
    {
        memcpy(deviceId, &oicUuid, UUID_IDENTITY_SIZE);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Device ID Get error");
    }
    return ret;
}

OCStackResult OC_CALL OCSetDeviceId(const OCUUIdentity *deviceId)
{
    OicUuid_t oicUuid;
    OCStackResult ret = OC_STACK_ERROR;

    memcpy(&oicUuid, deviceId, UUID_LENGTH);
    for (int i = 0; i < UUID_LENGTH; i++)
    {
        OIC_LOG_V(INFO, TAG, "Set Device Id %x", oicUuid.id[i]);
    }
    ret = SetDoxmDeviceID(&oicUuid);
    return ret;
}

OCStackResult OC_CALL OCGetDeviceOwnedState(bool *isOwned)
{
    bool isDeviceOwned = true;
    OCStackResult ret = OC_STACK_ERROR;

    ret = GetDoxmIsOwned(&isDeviceOwned);
    if (OC_STACK_OK == ret)
    {
        *isOwned = isDeviceOwned;
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Device Owned State Get error");
    }
    return ret;
}

#ifdef IP_ADAPTER
OCStackResult OC_CALL OCGetLinkLocalZoneId(uint32_t ifindex, char **zoneId)
{
    return CAResultToOCResult(CAGetLinkLocalZoneId(ifindex, zoneId));
}
#endif

OCStackResult OC_CALL OCSelectCipherSuite(uint16_t cipher, OCTransportAdapter adapterType)
{
    // OCTransportAdapter and CATransportAdapter_t are using the same bits for each transport.
    OC_STATIC_ASSERT((unsigned int)OC_ADAPTER_IP == (unsigned int)CA_ADAPTER_IP,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_ADAPTER_GATT_BTLE == (unsigned int)CA_ADAPTER_GATT_BTLE,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_ADAPTER_RFCOMM_BTEDR == (unsigned int)CA_ADAPTER_RFCOMM_BTEDR,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_ADAPTER_TCP == (unsigned int)CA_ADAPTER_TCP,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_ADAPTER_NFC == (unsigned int)CA_ADAPTER_NFC,
        "OC/CA bit mismatch");

#ifdef RA_ADAPTER
    OC_STATIC_ASSERT(
        (unsigned int)OC_ADAPTER_REMOTE_ACCESS
            == (unsigned int)CA_ADAPTER_REMOTE_ACCESS, "OC/CA bit mismatch");

    #define ALL_OC_ADAPTER_TYPES (OC_ADAPTER_IP | OC_ADAPTER_GATT_BTLE | OC_ADAPTER_RFCOMM_BTEDR |\
                                  OC_ADAPTER_TCP | OC_ADAPTER_NFC | OC_ADAPTER_REMOTE_ACCESS)
#else
    #define ALL_OC_ADAPTER_TYPES (OC_ADAPTER_IP | OC_ADAPTER_GATT_BTLE | OC_ADAPTER_RFCOMM_BTEDR |\
                                  OC_ADAPTER_TCP | OC_ADAPTER_NFC)
#endif

    assert((adapterType & ~ALL_OC_ADAPTER_TYPES) == 0);

    return CAResultToOCResult(CASelectCipherSuite(cipher, (CATransportAdapter_t)adapterType));
}

OCStackResult OC_CALL OCGetIpv6AddrScope(const char *addr, OCTransportFlags *scope)
{
    // OCTransportFlags and CATransportFlags_t are using the same bits for each scope.
    OC_STATIC_ASSERT((unsigned int)OC_SCOPE_INTERFACE == (unsigned int)CA_SCOPE_INTERFACE,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_SCOPE_LINK == (unsigned int)CA_SCOPE_LINK,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_SCOPE_REALM == (unsigned int)CA_SCOPE_REALM,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_SCOPE_ADMIN == (unsigned int)CA_SCOPE_ADMIN,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_SCOPE_SITE == (unsigned int)CA_SCOPE_SITE,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_SCOPE_ORG == (unsigned int)CA_SCOPE_ORG,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_SCOPE_GLOBAL == (unsigned int)CA_SCOPE_GLOBAL,
        "OC/CA bit mismatch");

    #define ALL_OC_SCOPES (OC_SCOPE_INTERFACE | OC_SCOPE_LINK | OC_SCOPE_REALM | OC_SCOPE_ADMIN |\
                           OC_SCOPE_SITE | OC_SCOPE_ORG | OC_SCOPE_GLOBAL)

    CAResult_t caResult = CAGetIpv6AddrScope(addr, (CATransportFlags_t *)scope);

    if (CA_STATUS_OK == caResult)
    {
        assert(((*scope) & ~ALL_OC_SCOPES) == 0);
        return OC_STACK_OK;
    }

    return CAResultToOCResult(caResult);
}
