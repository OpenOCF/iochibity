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
#include <assert.h>
/* #ifndef __cplusplus */
/* #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L */
/* #if defined(static_assert) */
/* #pragma message "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" */
/* #endif */
/* #endif /\* __STDC_VERSION__ *\/ */
/* #endif /\* !__cplusplus *\/ */
/* #pragma message(VAR_NAME_VALUE(__STDC__)) */
/* #pragma message(VAR_NAME_VALUE(__STDC_VERSION__)) */
/* #pragma message(VAR_NAME_VALUE(__cplusplus)) */
/* #pragma message(VAR_NAME_VALUE(static_assert)) */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <limits.h>		/* UINT_MAX */

#if INTERFACE
#include <stdint.h>
#endif	/* INTERFACE */

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "coap_config.h"
/* FIXME PORTABILITY: one of these is required for coap_time.h */
#ifdef _MSC_VER
#define HAVE_WS2TCPIP_H
#endif
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

/** The coap scheme */
// UNUSED #define OC_COAP_SCHEME "coap://"

/** the first outgoing sequence number will be 1*/
#if INTERFACE
#define OC_OFFSET_SEQUENCE_NUMBER (0) /* src: ocstackinternal.h */
#endif

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

    /** ERROR in stack.*/
    OC_STACK_ERROR = 255
    /** Error status code - END HERE.*/
} OCStackResult;                /* src::: octypes.h */

/**
 * Handle to an OCDoResource invocation.
 */
/* src: octypes.h */
typedef void * OCDoHandle;

/**
 * Handle to an OCResource object owned by the OCStack.
 */
/* src: octypes.h */
typedef void * OCResourceHandle;

/**
 * Handle to an OCRequest object owned by the OCStack.
 */
/* src: octypes.h */
typedef void * OCRequestHandle;

#endif	/* EXPORT_INTERFACE */


//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------
static OCStackState stackState = OC_STACK_UNINITIALIZED;

// resource vars => oocf_resource_list.c

#ifdef RA_ADAPTER
//TODO: revisit this design
static bool gRASetInfo = false;
#endif
OCDeviceEntityHandler defaultDeviceHandler;
void* defaultDeviceHandlerCallbackParameter = NULL;
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

/**
 *  OCDoResource methods to dispatch the request
 */
#if EXPORT_INTERFACE
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
 * } OCMethod; */               /* src: octypes.h */

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
} StackQueryTypes;              /* src: ocresourcehandler.h */

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
} OCQualityOfService;           /* src: octypes.h */
#endif	/* INTERFACE */


//TODO: we should allow the server to define this
#define MAX_OBSERVE_AGE (0x2FFFFUL)

#define MILLISECONDS_PER_SECOND   (1000)

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

static void OCEnterInitializer(void)
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
        // Yield execution to the thread that is holding the lock.
        sleep(0);
    }
}

static void OCLeaveInitializer(void)
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

// FIXME: get rid of OCDevAddr?
void CopyEndpointToDevAddr(const CAEndpoint_t *in, OCDevAddr *out)
{
    VERIFY_NON_NULL_NR(in, FATAL);
    VERIFY_NON_NULL_NR(out, FATAL);

    out->adapter = (OCTransportAdapter)in->adapter;
    out->flags = CAToOCTransportFlags(in->flags);
    OICStrcpy(out->addr, sizeof(out->addr), in->addr);
    OICStrcpy(out->remoteId, sizeof(out->remoteId), in->remoteId);
    out->port = in->port;
    /* out->ifindex = in->ifindex; */
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
    /* out->ifindex = in->ifindex; */
}

void FixUpClientResponse(OCClientResponse *cr)
{
    VERIFY_NON_NULL_NR(cr, FATAL);

    cr->addr = &cr->devAddr;
    cr->connType = (OCConnectivityType)
        ((cr->devAddr.adapter << CT_ADAPTER_SHIFT) | (cr->devAddr.flags & CT_MASK_FLAGS));
}

/* OCSendRequest => oocf_client.c */

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
/* => oocf_server.c */
OCStackResult OCStackFeedBack(uint8_t *token, uint8_t tokenLength, uint8_t status)
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
/* DELETE */
OCStackResult CAResponseToOCStackResult(CAResponseResult_t caCode)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, resp: %u", __func__, caCode);
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
    OIC_LOG_V(DEBUG, TAG, "%s EXIT, resp: %u", __func__, ret);
    return ret;
}

/**
 * Convert OCStackResult to CAResponseResult_t.
 *
 * @param ocCode OCStackResult code.
 * @param method OCMethod method the return code replies to.
 * @return ::CA_CONTENT on OK, some other value upon failure.
 */
/* DELETE */
CAResponseResult_t OCToCAStackResult(OCStackResult ocCode, OCMethod method)
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
        case OC_STACK_INVALID_METHOD:
            ret = CA_METHOD_NOT_ALLOWED;
            break;
        case OC_STACK_TOO_LARGE_REQ:
            ret = CA_REQUEST_ENTITY_TOO_LARGE;
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
        // FIXME: An endpoint can only have one address, so only one address family. Default is ipv6
        caFlags |= CA_IPV6; // (CATransportFlags_t)(caFlags|CA_IPV6); // |CA_IPV4
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

/* RFC 6874: Representing IPv6 Zone Identifiers in Address Literals
 * and Uniform Resource Identifiers
 * => oocf_uri_utils.c
 */
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

/* => oocf_client.c? */
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

/* OCHandleResponse => oocf_client.c */

/* HandleCAResponses => oocf_client.c */

/* HandleCAErrorResponse => oocf_client.c */

/* HandleCARequests => oocf_server.c */

/* OCHandleRequests => oocf_server.c */

/* HandleStackRequests => oocf_server.c */

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
    (void) ipAddr;
    (void) port;
    OCLogInit();
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OCStackResult r = OCInit1(mode, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return r;
}

OCStackResult OC_CALL OCInit1(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags)
EXPORT
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, mode: %d", __func__, mode);
    // GAR: OC_DEFAULT_ADAPTER means all available adapters?
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
    } else {
	OIC_LOG_V(DEBUG, TAG, "%s WTF? rc: %d", __func__, result);
    }

    OCLeaveInitializer();
    OIC_LOG_V(DEBUG, TAG, "%s EXIT, rc: %d", __func__, result);
    return result;
}

/* #if defined(__clang__) || defined(__GNUC__) */
/** test clang address sanitizer: stack-use-after-return */
/* int *xtest_asan_ptr; */
/* volatile int *xtest_asan_ptr2 = 0; */
/* __attribute__((noinline)) */
/* void xtest_asan_stack_use_after_return() { */
/*   int local[100]; */
/*   xtest_asan_ptr = &local[0]; */
/* } */
/* int *xtest_asan_heap_use_after_free() */
/* { */
/*     int *array = malloc(100); */
/*     free(array); */
/*     return array[1]; */
/* } */
/* #endif */

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

/* #if defined(__clang__) || defined(__GNUC__) */
    /* clang address sanitizer tests */
    /* heap-use-after-free */
    /* int *x_asan_i = xtest_asan_heap_use_after_free(); */

    /* heap-buffer-overflow */
    /* int *array = calloc(100, sizeof(int)); */
    /* int res = array[101];  // BOOM */
    /* free(array); */

    /* stack-buffer-overflow */
    /* int stack_array[100]; */
    /* stack_array[1] = 0; */
    /* int i = stack_array[101]; */

    /* global-buffer-overflow */
    /* char c = COAP_TCP_SCHEME[10]; /\* COAP_TCP_SCHEME[] = "coap+tcp:" *\/ */

    /* stack-use-after-return - run with ASAN_OPTIONS=detect_stack_use_after_return=1 */
    /* xtest_asan_stack_use_after_return(); */
    /* int xtest_asan_i = xtest_asan_ptr[1]; */

    /* stack-use-after-scope */
    /* { int x = 0; xtest_asan_ptr2 = &x; } */
    /* *xtest_asan_ptr2 = 5; */

    /* attempting free on address which was not malloc()-ed */
    /* int xtest_asan_value = 42; */
    /* free(&xtest_asan_value); */

    /* attempting double-free */
    /* int *xtest_asan_ptr = malloc(sizeof(int)); */
    /* free(xtest_asan_ptr); free(xtest_asan_ptr); */
/* #endif */

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

    /* set caglobals common to all transports */
    if (mode == OC_CLIENT || mode == OC_CLIENT_SERVER || mode == OC_GATEWAY)
    {
        ocf_client = true;      // @rewrite caglobals
    }
    if (mode == OC_SERVER || mode == OC_CLIENT_SERVER || mode == OC_GATEWAY)
    {
        ocf_server = true;  // @rewrite caglobals.server = true;
    }

    //  evidently both IPv4 and IPv6 are required, so just hardcode them in caconnectivitymanager.c
    ocf_serverFlags |= (CATransportFlags_t)serverFlags; // @rewrite
    /* if (!(caglobals.serverFlags & CA_IPFAMILY_MASK)) */
    /* { */
    /*     caglobals.serverFlags = (CATransportFlags_t)(caglobals.serverFlags|CA_IPV4|CA_IPV6); */
    /* } */

    ocf_clientFlags |= (CATransportFlags_t)clientFlags; // @rewrite
    /* if (!(caglobals.clientFlags & CA_IPFAMILY_MASK)) */
    /* { */
    /*     caglobals.clientFlags = (CATransportFlags_t)(caglobals.clientFlags|CA_IPV4|CA_IPV6); */
    /* } */

    defaultDeviceHandler = NULL;
    defaultDeviceHandlerCallbackParameter = NULL;

#ifdef UWP_APP
    result = InitSqlite3TempDir();
    VERIFY_SUCCESS_2(result, OC_STACK_OK);
#endif // UWP_APP

    result = InitializeScheduleResourceList();
    VERIFY_SUCCESS_2(result, OC_STACK_OK);

    // initialize camessagehandler:
    result = CAResultToOCResult(CAInitialize((CATransportAdapter_t)transportType));
    VERIFY_SUCCESS_2(result, OC_STACK_OK);

    // start transports:
    //FIXME: OCSelectNetwork does nothing but add the nw to
    //g_selectedNetworkList in canetworkconfigurator, which serves no
    //real purpose. Then it starts the adapter. This can all be statically configured.
#ifndef DISABLE_UDP
    /* result = CAResultToOCResult(OCSelectNetwork(OC_ADAPTER_IP)); */
    /* if (result != OC_STACK_OK) {OIC_LOG_V(FATAL, TAG, "OCSelectNetwork failed for UDP, rc %d!!", result); goto exit;} */
    result = CAResultToOCResult(CAStartAdapter(CA_ADAPTER_IP));
#endif

#ifdef ENABLE_TCP
    /* result = CAResultToOCResult(OCSelectNetwork(OC_ADAPTER_TCP)); */
    /* if (result != OC_STACK_OK) {OIC_LOG_V(FATAL, TAG, "OCSelectNetwork failed for TCP, rc %d!!", result); goto exit;} */
    result = CAResultToOCResult(CAStartAdapter(CA_ADAPTER_TCP));
#endif

    result = CAResultToOCResult(CARegisterNetworkMonitorHandler(
      OCDefaultAdapterStateChangedHandler, OCDefaultConnectionStateChangedHandler));
    VERIFY_SUCCESS_2(result, OC_STACK_OK);

    switch (myStackMode)
    {
	// FIXME: initialize client/server mode statically at build time
        case OC_CLIENT:
            CARegisterHandler(HandleCARequests, HandleCAResponses, HandleCAErrorResponse);
            OIC_LOG(INFO, TAG, "Client mode: CAStartDiscoveryServer");
            result = CAResultToOCResult(CAStartDiscoveryServer());
            break;
        case OC_SERVER:
	    // FIXME: SRMRegisterHandler just calls CARegisterHandler with secure handles if DTLS
            // GAR virtual lookups removed, this registration call can be removed?
            SRMRegisterHandler(HandleCARequests, HandleCAResponses, HandleCAErrorResponse);
            OIC_LOG(INFO, TAG, "Server mode: CAStartListeningServer");
            result = CAResultToOCResult(CAStartListeningServer());
            break;
        case OC_CLIENT_SERVER:
        case OC_GATEWAY:
            SRMRegisterHandler(HandleCARequests, HandleCAResponses, HandleCAErrorResponse);
	    OIC_LOG(INFO, TAG, "ClientServer mode: CAStartListeningServer");
            result = CAResultToOCResult(CAStartListeningServer());
            if(result == OC_STACK_OK)
            {
		OIC_LOG(INFO, TAG, "ClientServer mode: CAStartDiscoveryServer");
                result = CAResultToOCResult(CAStartDiscoveryServer());
            }
            break;
    }
    VERIFY_SUCCESS_2(result, OC_STACK_OK);

#ifdef TCP_ADAPTER
    CARegisterKeepAliveHandler(HandleKeepAliveConnCB);
#endif

#ifdef WITH_PRESENCE
    PresenceTimeOutSize = sizeof (PresenceTimeOut) / sizeof (PresenceTimeOut[0]) - 1;
#endif // WITH_PRESENCE

    //Update Stack state to initialized
    stackState = OC_STACK_INITIALIZED;

    // Initialize resource
    /* if(myStackMode != OC_CLIENT) */
    /* { */
    /*     result = initResources(); */
    /* } */
    result = initResources();

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
        result = CAResultToOCResult(CAInitializePing());
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
        OIC_LOG_V(ERROR, TAG, "Stack initialization error: %d", result);
        TerminateScheduleResourceList();
        deleteAllResources();
        CATerminate();
        stackState = OC_STACK_UNINITIALIZED;
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT, rc: %u", __func__, result);
    return result;
}

OCStackResult OC_CALL OCStop(void) EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);

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
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
    return result;
}

/**
 * DeInitialize the stack.
 * Caller of this function must serialize calls to this function and the init counterpart.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
LOCAL OCStackResult OCDeInitializeInternal(void)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
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
    CATerminatePing();
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
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
    return OC_STACK_OK;
}

OCStackResult OC_CALL OCStartMulticastServer(void)
{
    if(stackState != OC_STACK_INITIALIZED)
    {
        OIC_LOG(ERROR, TAG, "OCStack is not initalized. Cannot start multicast server.");
        return OC_STACK_ERROR;
    }
    g_multicastServerStopped = false;
    return OC_STACK_OK;
}

OCStackResult OC_CALL OCStopMulticastServer(void)
{
    g_multicastServerStopped = true;
    return OC_STACK_OK;
}


/* qualityOfServiceToMessageType => oocf_client.c */

/* ParseRequestUri => oocf_uri.c */


/* OCDoResource => oocf_client.c */

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
const char* ASSERT_ROLES_CTX = "Asserting roles from OCDoRequest";
void assertRolesCB(void* ctx, bool hasError)
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

/* OCDoRequest => oocf_client.c */

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
    struct CARequestInfo requestInfo = {.method = CA_GET};

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

            requestInfo.info.type = qualityOfServiceToCoAPMessageType(qos);
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
 * @param[in] persistentStorageHandler  Pointers to open, read, write, close & unlink handlers.
 * @return
 *     OC_STACK_OK    - No errors; Success
 *     OC_STACK_INVALID_PARAM - Invalid parameter
 */
OCStackResult OC_CALL OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
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
    } else {
	OIC_LOG(ERROR, TAG, "NULL persistent storage handler passed");
	return OC_STACK_INVALID_PARAM;
    }
    g_PersistentStorageHandler = persistentStorageHandler;
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
    return OC_STACK_OK;
}

OCPersistentStorage *OC_CALL OCGetPersistentStorageHandler(void)
{
    return g_PersistentStorageHandler;
}

OCStackResult OC_CALL OCProcess(void) EXPORT
{
    if (stackState == OC_STACK_UNINITIALIZED)
    {
        OIC_LOG(ERROR, TAG, "OCProcess has failed. ocstack is not initialized");
        return OC_STACK_ERROR;
    }
#ifdef WITH_PRESENCE
    OCProcessPresence();
#endif

    if (g_isInitialized)
	oocf_handle_inbound_messages(); // @was CAHandleRequestResponse

#ifdef ROUTING_GATEWAY
    RMProcess();
#endif

#ifdef TCP_ADAPTER
    ProcessKeepAlive();
    CAProcessPing();
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

OCTpsSchemeFlags OC_CALL OCGetSupportedEndpointTpsFlags(void)
{
    return OCGetSupportedTpsFlags();
}

/* OCCreateResource => oocf_resource_list.c */

/* OCCreateResourceWithEp => oocf_resource_list.c */

/* OCBindResource => oocf_resource_list.c */

/* OCUnBindResource => oocf_resource_list.c */

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
OCStackResult BindResourceTypeToResource(OCResource *resource,
					 const char *resourceTypeName) EXPORT
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
OCStackResult BindTpsTypeToResource(OCResource *resource,
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

/* OCGetNumberOfResources => oocf_resource_list.c */

/* OCGetResourceHandle => oocf_resource_list.c */

/* OCDeleteResource => oocf_resource_list.c */

const char *OC_CALL OCGetResourceUri(OCResourceHandle handle) EXPORT
{
    OCResource *resource = NULL;

    resource = findResource((OCResource *) handle);
    if (resource)
    {
        return resource->uri;
    }
    return (const char *) NULL;
}

OCResourceProperty OC_CALL OCGetResourceProperties(OCResourceHandle handle) EXPORT
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

#ifdef WITH_PRESENCE
OCStackResult SendPresenceNotification(OCResourceType *resourceType,
        OCPresenceTrigger trigger)
{
    OIC_LOG(INFO, TAG, "SendPresenceNotification");
    OCResource *resPtr = NULL;
    OCStackResult result = OC_STACK_ERROR;
    OCMethod method = OC_REST_PRESENCE;
    uint32_t maxAge = 0;
    resPtr = findResource((OCResource *) presenceResource.handle);
    if(NULL == resPtr)
    {
        return OC_STACK_NO_RESOURCE;
    }

    if((((OCResource *) presenceResource.handle)->resourceProperties) & OC_ACTIVE)
    {
        maxAge = presenceResource.presenceTTL;

        result = SendAllObserverNotification(method, resPtr, maxAge,
                trigger, resourceType, OC_LOW_QOS);
    }

    return result;
}

OCStackResult SendStopNotification(void)
{
    OIC_LOG(INFO, TAG, "SendStopNotification");
    OCResource *resPtr = NULL;
    OCStackResult result = OC_STACK_ERROR;
    OCMethod method = OC_REST_PRESENCE;
    resPtr = findResource((OCResource *) presenceResource.handle);
    if(NULL == resPtr)
    {
        return OC_STACK_NO_RESOURCE;
    }

    // maxAge is 0. ResourceType is NULL.
    result = SendAllObserverNotification(method, resPtr, 0, OC_PRESENCE_TRIGGER_DELETE,
            NULL, OC_LOW_QOS);

    return result;
}

#endif // WITH_PRESENCE
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
        // FIXME: the Iotivity code uses SendAllObserverNotification???
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

/* send outbound response */
OCStackResult OC_CALL OCDoResponse(OCEntityHandlerResponse *ehResponse) EXPORT
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_TRACE_BEGIN(%s:OCDoResponse, TAG);
    OCStackResult result = OC_STACK_ERROR;
    // OCServerRequest *serverRequest = NULL;
    struct CARequestInfo *request = NULL;

    OIC_LOG_V(DEBUG, TAG, "%s result: 0x%04x", __func__, ehResponse->ehResult);

    // Validate input parameters
    VERIFY_NON_NULL(ehResponse, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(ehResponse->requestHandle, ERROR, OC_STACK_INVALID_PARAM);

    // Normal response
    // Get pointer to request info
    // serverRequest = (OCServerRequest *)ehResponse->requestHandle;
    request = (struct CARequestInfo *)ehResponse->requestHandle;
    VERIFY_NON_NULL(request->ehResponseHandler, ERROR, OC_STACK_INVALID_PARAM);
    if(request)
    {
        // response handler in ocserverrequest.c. Usually HandleSingleResponse.
        // ehResponseHandler will be either HandleSingleResponse or HandleAggregateResponse
        result = request->ehResponseHandler(ehResponse);
    }

    OIC_TRACE_END();
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return result;
}

//-----------------------------------------------------------------------------
// Private internal function definitions
//-----------------------------------------------------------------------------

/* GenerateInvocationHandle => oocf_client.c */

/* initResources => oocf_resource_list.c */

/* insertResource => oocf_resource_list.c */

/* findResource => oocf_resource_list.c */

/* deleteAllResources => oocf_resource_list.c */

/* deleteResource => oocf_resource_list.c */

/* deleteResourceElements => oocf_resource_list.c */

/* deleteResourceType => oocf_resource_list.c */

/* deleteResourceInterface => oocf_resource_list.c */

/* OCDeleteResourceAttributes => oocf_resource_list.c */

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
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
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
 * This function splits the URI using the '?' delimiter.
 * "uriWithoutQuery" is the block of characters between the beginning
 * till the delimiter or '\0' which ever comes first.
 * "query" is whatever is to the right of the delimiter if present.
 * No delimiter sets the query to NULL.
 * If either are present, they will be malloc'ed into the params 2, 3.
 * The first param, *uri is left untouched.

 * NOTE: This function does not account for whitespace at the end of the URI NOR
 *       malformed URIs with '??'. Whitespace at the end will be assumed to be
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
OCStackResult getQueryFromUri(const char * uri, char** query, char ** uriWithoutQuery)
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
    static OicUuid_t sid;

    if (OC_STACK_OK != GetDoxmDeviceID(&sid))
    {
        OIC_LOG(FATAL, TAG, "Generate UUID for Server Instance failed!");
        return NULL;
    }

    return &sid;
}

const char* OC_CALL OCGetServerInstanceIDString(void)
{
    static char sidStr[UUID_STRING_SIZE];

    const OicUuid_t *sid = OCGetServerInstanceID();
    if(sid && !OCConvertUuidToString(sid->id, sidStr))
    {
        OIC_LOG(FATAL, TAG, "Generate UUID String for Server Instance failed!");
        return NULL;
    }

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
// @rewrite: networks support is configured at build time, not runtime
/* LOCAL CAResult_t OCSelectNetwork(OCTransportAdapter transportType) */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "OCSelectNetwork [%d]", transportType); */
/*     CAResult_t retResult = CA_STATUS_FAILED; */
/*     CAResult_t caResult = CA_STATUS_OK; */

/*     // evidently the only thing CASelectNetwork does is calll */
/*     // CAAddNetworkType, which adds nw to a list */
/*     // g_selectedNetworkList, which moreover is only ever used in */
/*     // canetworkconfigurator.c.  this is silly since it can all be */
/*     // statically configured at compile time. */

/* #ifndef DISABLE_UDP */
/*     OIC_LOG(DEBUG, TAG, "calling CASelectNetwork for CA_ADAPTER_IP"); */
/*     caResult = CASelectNetwork(CA_ADAPTER_IP); */
/*     if (caResult != CA_STATUS_OK) { */
/* 	OIC_LOG(ERROR, TAG, "Unable to support UDP"); */
/* 	// Throw an exception? If the build is configured for an adapter, it should initialize */
/*     } */
/*     // NB: static init: CASelectNetwork can be replaced by calling directly: */
/*     // CAStartUDP(); */
/* #endif */

/* #ifdef ENABLE_TCP */
/*     OIC_LOG(DEBUG, TAG, "calling CASelectNetwork for CA_ADAPTER_TCP"); */
/*     caResult = CASelectNetwork(CA_ADAPTER_TCP); */
/*     if (caResult != CA_STATUS_OK) { */
/* 	OIC_LOG_V(ERROR, TAG, "Unable to support TCP, code %d", caResult); */
/* 	// Throw an exception? If the build is configured for an adapter, it should initialize */
/*     } */
/* #endif */

/*     //etc. */

/*     if (retResult != CA_STATUS_OK) */
/*     { */
/*         return caResult; // Returns error of appropriate transport that failed fatally. */
/*     } */

/*     return retResult; */
/* } */

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
bool OCResultToSuccess(OCStackResult ocResult)
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
OCStackResult SetHeaderOption(CAHeaderOption_t *caHdrOpt, size_t numOptions,
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
                                        void* optionData, size_t optionDataLength) EXPORT
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

/**
 * default adapter state change callback method
 *
 * @param adapter   CA network adapter type.
 * @param enabled   current adapter state.
 */
void OCDefaultAdapterStateChangedHandler(CATransportAdapter_t adapter, bool enabled)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OC_UNUSED(adapter);
    OC_UNUSED(enabled);

    /* 1.4: */
/* #ifdef WITH_PRESENCE */
/*     if (presenceResource.handle) */
/*     { */
/*         OCResource *resourceHandle = (OCResource *)presenceResource.handle; */
/*         resourceHandle->sequenceNum = OCGetRandom(); */
/*         SendPresenceNotification(resourceHandle->rsrcType, OC_PRESENCE_TRIGGER_CHANGE); */
/*     } */
/* #endif */
}

/**
 * default connection state change callback method
 *
 * @param info          CAEndpoint which has address, port and etc.
 * @param isConnected   current connection state.
 */
// FIXME: move to comm/nw file
void OCDefaultConnectionStateChangedHandler(const CAEndpoint_t *info, bool isConnected)
{
    OIC_LOG(DEBUG, TAG, "OCDefaultConnectionStateChangedHandler");

    /*
     * If the client observes one or more resources over a reliable connection,
     * then the CoAP server (or intermediary in the role of the CoAP server)
     * MUST remove all entries associated with the client endpoint from the lists
     * of observers when the connection is either closed or times out.
     */
    if (!isConnected)
    {
        OCDevAddr devAddr = { OC_DEFAULT_ADAPTER }; /* FIXME: do proper initialization of struct */
        CopyEndpointToDevAddr(info, &devAddr);

        // remove observer list with remote device address.
        GiveStackFeedBackObserverNotInterested(&devAddr);
    }
}

/* OCGetDeviceId => oocf_device.c */

/* OCSetDeviceId => oocf_device.c */

/* OCGetDeviceOwnedState => oocf_device.c */

#ifdef IP_ADAPTER
OCStackResult OC_CALL OCGetLinkLocalZoneId(uint32_t ifindex, char **zoneId)
{
    return CAResultToOCResult(CAGetLinkLocalZoneId(ifindex, zoneId));
}
#endif

/* OCSelectCipherSuite => sec/oocf_cipher_suites.c */

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
        assert(((*scope) & ~ALL_OC_SCOPES) == 0); /* FIXME: how could this be wrong? */
        return OC_STACK_OK;
    }

    return CAResultToOCResult(caResult);
}

#ifdef TCP_ADAPTER

static void OCPongHandler(void *context, CAEndpoint_t endpoint, bool withCustody)
{
    OIC_LOG_V(DEBUG, TAG, "Received pong from [%s]", endpoint.addr);
    (void) withCustody;
    OCCallbackData *cbData = (OCCallbackData *)context;
    OCClientResponse clientResponse;
    memset(&clientResponse, 0, sizeof(OCClientResponse));
    CopyEndpointToDevAddr(&endpoint, &clientResponse.devAddr);
    clientResponse.connType = CT_ADAPTER_TCP;
    clientResponse.result = OC_STACK_OK;
    FixUpClientResponse(&clientResponse);
    cbData->cb(cbData->context, NULL, &clientResponse);
}

static void OCPongDeleter(void *context)
{
    OCCallbackData *cbData = (OCCallbackData *)context;
    cbData->cd(cbData->context);
    OICFree(cbData);
}

OCStackResult OC_CALL OCSendPingMessage(const OCDevAddr *devAddr, bool withCustody, OCCallbackData *cbData)
{
    OIC_LOG_V(DEBUG, TAG, "Sending ping message to [%s]", devAddr->addr);

    CAPongCallbackData pongCbData;
    OCCallbackData *cbDataCopy = (OCCallbackData *)OICMalloc(sizeof(OCCallbackData));

    if (NULL == cbDataCopy)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate memory for callback data");
        return OC_STACK_NO_MEMORY;
    }

    *cbDataCopy = *cbData;
    pongCbData.context = cbDataCopy;
    pongCbData.cb = OCPongHandler;
    pongCbData.cd = OCPongDeleter;

    CAEndpoint_t endpoint;
    CopyDevAddrToEndpoint(devAddr, &endpoint);

    return CAResultToOCResult(CASendPingMessage(&endpoint, withCustody, &pongCbData));
}

#endif // TCP_ADAPTER
