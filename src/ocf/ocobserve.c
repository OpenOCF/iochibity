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

#include "ocobserve.h"

#include <string.h>

#include "utlist.h"

/* FIXME PORTABILITY: one of these is required for coap_time.h */
/* use --per_file_copt cmd line option? */
#include <time.h>
#include "coap_config.h"
#include "coap/coap_time.h"
#include "coap/pdu.h"

// Module Name
#define MOD_NAME "ocobserve"

#define TAG  "OIC_RI_OBSERVE"

/**
 * Unique identifier for each observation request. Used when observations are
 * registered or de-registered. Used by entity handler to signal specific
 * observers to be notified of resource changes.
 * There can be maximum of 256 observations per server.
 */
#if EXPORT_INTERFACE
/* src: octypes.h */
typedef uint8_t OCObservationId;
#endif	/* INTERFACE */

/**
 * Action associated with observation.
 */
#if EXPORT_INTERFACE
/* src: octypes.h */
typedef enum
{
    /** To Register. */
    OC_OBSERVE_REGISTER = 0,

    /** To Deregister. */
    OC_OBSERVE_DEREGISTER = 1,

    /** Others. */
    OC_OBSERVE_NO_OPTION = 2,   /* wtf? */

} OCObserveAction;
#endif	/* INTERFACE */

/**
 * Possible returned values from entity handler.
 */
#if EXPORT_INTERFACE
/* src: octypes.h */
typedef struct
{
    /** Action associated with observation request.*/
    OCObserveAction action;

    /** Identifier for observation being registered/deregistered.*/
    OCObservationId obsId;
} OCObservationInfo;

/** Maximum number of observers to reach */

/* src: ocobserve.h */
#define MAX_OBSERVER_FAILED_COMM         (2)

/** Maximum number of observers to reach for resources with low QOS */
/* src: ocobserve.h */
#define MAX_OBSERVER_NON_COUNT           (3)

/**
 *  MAX_OBSERVER_TTL_SECONDS sets the maximum time to live (TTL) for notification.
 *  60 sec/min * 60 min/hr * 24 hr/day
 */
/* src: ocobserve.h */
#define MAX_OBSERVER_TTL_SECONDS     (60 * 60 * 24)

/* src: ocobserve.h */
#define MILLISECONDS_PER_SECOND   (1000)
#endif	/* EXPORT_INTERFACE */

/**
 * Forward declaration of resource.
 */
/* typedef struct OCResource OCResource; */

/**
 * Forward declaration of resource type.
 */
/* src: ocobserve.h */
// typedef struct resourcetype_t OCResourceType;

/**
 * Data structure to hold informations for each registered observer.
 */
#if EXPORT_INTERFACE
/* src: ocobserve.h */
typedef struct ResourceObserver
{
    /** Observation Identifier for request.*/
    OCObservationId observeId;

    /** URI of observed resource.*/
    char *resUri;

    /** Query.*/
    char *query;

    /** token for the observe request.*/
    uint8_t *token;

    /** token length for the observe request.*/
    uint8_t tokenLength;

    /** Remote Endpoint. */
    CAEndpoint_t devAddr; // OCDevAddr devAddr;

    /** Quality of service of the request.*/
    OCQualityOfService qos;

    /** number of times the server failed to reach the observer.*/
    uint8_t failedCommCount;

    /** number of times the server sent NON notifications.*/
    uint8_t lowQosCount;

    /** force the qos value to CON.*/
    uint8_t forceHighQos;

    /** The TTL for this callback. TTL is set to 24 hours.
     * A server send a notification in a confirmable message every 24 hours.
     * This prevents a client that went away or is no logger interested
     * from remaining in the list of observers indefinitely.*/
    uint32_t TTL;

    /** next node in this list.*/
    struct ResourceObserver *next;

    /** requested payload encoding format. */
    OCPayloadFormat acceptFormat;

    /** requested payload content version. */
    uint16_t acceptVersion;

} ResourceObserver;
#endif	/* EXPORT_INTERFACE */

/* #define VERIFY_NON_NULL(arg) { if (!arg) {OIC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} } */

/**
 * Determine observe QOS based on the QOS of the request.
 * The qos passed as a parameter overrides what the client requested.
 * If we want the client preference taking high priority make:
 *     qos = resourceObserver->qos;
 *
 * @param method RESTful method.
 * @param resourceObserver Observer.
 * @param appQoS Quality of service.
 * @return The quality of service of the observer.
 */
OCQualityOfService DetermineObserverQoS(OCMethod method,
					ResourceObserver * resourceObserver,
					OCQualityOfService appQoS)
{
    if (!resourceObserver)
    {
        OIC_LOG(ERROR, TAG, "DetermineObserverQoS called with invalid resourceObserver");
        return OC_NA_QOS;
    }

    OCQualityOfService decidedQoS = appQoS;
    if (appQoS == OC_NA_QOS)
    {
        decidedQoS = resourceObserver->qos;
    }

    if (appQoS != OC_HIGH_QOS)
    {
        OIC_LOG_V(INFO, TAG, "Current NON count for this observer is %d",
                resourceObserver->lowQosCount);
#ifdef WITH_PRESENCE
        if ((resourceObserver->forceHighQos \
                || resourceObserver->lowQosCount >= MAX_OBSERVER_NON_COUNT) \
                && method != OC_REST_PRESENCE)
#else
        if (resourceObserver->forceHighQos \
                || resourceObserver->lowQosCount >= MAX_OBSERVER_NON_COUNT)
#endif
        {
            resourceObserver->lowQosCount = 0;
            // at some point we have to to send CON to check on the
            // availability of observer
            OIC_LOG(INFO, TAG, "This time we are sending the  notification as High qos");
            decidedQoS = OC_HIGH_QOS;
        }
        else
        {
            (resourceObserver->lowQosCount)++;
        }
    }
    return decidedQoS;
}

/**
 * Create a get request and pass to entityhandler to notify specific observer.
 *
 * @param observer Observer that need to be notified.
 * @param qos Quality of service of resource.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult SendObserveNotification(ResourceObserver *observer,
                                      uint32_t sequenceNum,
                                      OCQualityOfService qos)
{
    OIC_LOG_V(INFO, __FILE__, "%s: ENTRY", __func__);
    OCStackResult result = OC_STACK_ERROR;
    /* OCServerRequest * request = NULL; */

    /* result = AddServerRequest(&request, 0, 0, 1, OC_REST_GET, */
    /*                           0, sequenceNum, qos, */
    /*                           observer->query, NULL, OC_FORMAT_UNDEFINED, NULL, */
    /*                           observer->token, observer->tokenLength, */
    /*                           observer->resUri, 0, observer->acceptFormat, */
    /*                           observer->acceptVersion, &observer->devAddr); */

    struct oocf_msg_coap_request *request = (struct oocf_msg_coap_request*)OICMalloc(sizeof(struct oocf_msg_coap_request));
    request->delayedResNeeded = 0;
    request->notificationFlag = 1;
    request->method = CA_GET;
    request->observationOption = sequenceNum;
    request->info.messageId = 0;
    request->info.type = qos;
    request->info.numOptions = 0;
    request->info.options = NULL;
    request->info.payloadFormat = OC_FORMAT_UNDEFINED;
    request->info.payload_cbor = NULL;
    request->info.payloadSize = 0;
    request->info.acceptFormat = observer->acceptFormat;
    request->info.acceptVersion = observer->acceptVersion;
    if (observer->tokenLength) {
        request->info.token = (uint8_t*)OICMalloc(observer->tokenLength);
        VERIFY_NON_NULL_1(request->info.token);
        memcpy(request->info.token, observer->token, observer->tokenLength);
    }
    request->info.tokenLength = observer->tokenLength;

    request->info.resourceUri = (char *)OICMalloc(MAX_URI_LENGTH);
    OICStrcpy(request->info.resourceUri, MAX_URI_LENGTH, observer->resUri);
    OICStrcat(request->info.resourceUri, MAX_URI_LENGTH, observer->query);

    memcpy(&request->dest_ep, &observer->devAddr, sizeof(CAEndpoint_t));
    request = _oocf_request_cache_put(request);

/* OCStackResult AddServerRequest (OCServerRequest ** request, */
/*                                 uint16_t coapMessageID, */
/*                                 uint8_t delayedResNeeded, */
/*                                 uint8_t notificationFlag, */
/*                                 OCMethod method, */
/*                                 uint8_t numRcvdVendorSpecificHeaderOptions, */
/*                                 uint32_t observationOption, */
/*                                 OCQualityOfService qos, */
/*                                 char * query, */
/*                                 OCHeaderOption * rcvdVendorSpecificHeaderOptions, */
/*                                 OCPayloadFormat payloadFormat, */
/*                                 uint8_t * payload, */
/*                                 uint8_t *requestToken, */
/*                                 uint8_t tokenLength, */
/*                                 char * resourceUrl, */
/*                                 size_t payloadSize, */
/*                                 OCPayloadFormat acceptFormat, */
/*                                 uint16_t acceptVersion, */
/*                                 const OCDevAddr *devAddr) */

    if (request)
    {
        request->observeResult = OC_STACK_OK;
        if (result == OC_STACK_OK)
        {
            ResourceHandling resHandling = OC_RESOURCE_VIRTUAL;
            OCResource *resource = NULL;
            result = DetermineResourceHandling (request, &resHandling, &resource);
            if (result == OC_STACK_OK)
            {
                result = ProcessRequest(resHandling, resource, request);
                // Reset Observer TTL.
                observer->TTL = GetTicks(MAX_OBSERVER_TTL_SECONDS * MILLISECONDS_PER_SECOND);
            }
        }
    }

    return result;
exit:
    OICFree(request->info.token);
    OICFree(request);
    return OC_STACK_NO_MEMORY;

}

/* #ifdef WITH_PRESENCE
 * OCStackResult SendAllObserverNotification (OCMethod method, OCResource *resPtr, uint32_t maxAge,
 *         OCPresenceTrigger trigger, OCResourceType *resourceType, OCQualityOfService qos)
 * #else */
OCStackResult SendAllObserverNotification (OCMethod method, OCResource *resPtr, uint32_t maxAge,
        OCQualityOfService qos)
/* #endif */
{
    OIC_LOG_V(INFO, __FILE__, "%s: ENTRY", __func__);
    if (!resPtr)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (!resPtr->observersHead)
    {
        OIC_LOG(INFO, TAG, "Resource has no observers");
        return OC_STACK_NO_OBSERVERS;
    }

    OCStackResult result = OC_STACK_ERROR;
    ResourceObserver * resourceObserver = resPtr->observersHead;
/* #ifdef WITH_PRESENCE
 *     struct oocf_msg_coap_request * request = NULL;
 * #endif */
    bool observeErrorFlag = false;

    // Find clients that are observing this resource
    while (resourceObserver)
    {
/* #ifdef WITH_PRESENCE
 *         if (method != OC_REST_PRESENCE)
 *         {
 * #endif */
            qos = DetermineObserverQoS(method, resourceObserver, qos);
            result = SendObserveNotification(resourceObserver, resPtr->sequenceNum, qos);
/* #ifdef WITH_PRESENCE
 *         }
 *         else
 *         {
 *             OCEntityHandlerResponse ehResponse = {0};
 *
 *             //This is effectively the implementation for the presence entity handler.
 *             OIC_LOG(DEBUG, TAG, "This notification is for Presence");
 *             result = AddServerRequest(&request, 0, 0, 1, OC_REST_GET,
 *                     0, resPtr->sequenceNum, qos, resourceObserver->query,
 *                     NULL, OC_FORMAT_UNDEFINED, NULL,
 *                     resourceObserver->token, resourceObserver->tokenLength,
 *                     resourceObserver->resUri, 0, resourceObserver->acceptFormat,
 *                     resourceObserver->acceptVersion, &resourceObserver->devAddr);
 *
 *             if (result == OC_STACK_OK)
 *             {
 *                 OCPresencePayload* presenceResBuf = OCPresencePayloadCreate(
 *                         resPtr->sequenceNum, maxAge, trigger,
 *                         resourceType ? resourceType->resourcetypename : NULL);
 *
 *                 if (!presenceResBuf)
 *                 {
 *                     return OC_STACK_NO_MEMORY;
 *                 }
 *
 *                 if (result == OC_STACK_OK)
 *                 {
 *                     ehResponse.ehResult = OC_EH_OK;
 *                     ehResponse.payload = (OCPayload*)presenceResBuf;
 *                     ehResponse.persistentBufferFlag = 0;
 *                     ehResponse.requestHandle = (OCRequestHandle) request;
 *                     OICStrcpy(ehResponse.resourceUri, sizeof(ehResponse.resourceUri),
 *                             resourceObserver->resUri);
 *                     result = OCDoResponse(&ehResponse);
 *                 }
 *
 *                 OCPresencePayloadDestroy(presenceResBuf);
 *             }
 *         }
 * #endif */

        // Since we are in a loop, set an error flag to indicate at least one error occurred.
        if (result != OC_STACK_OK)
        {
            observeErrorFlag = true;
        }

        resourceObserver = resourceObserver->next;
    }

    if (observeErrorFlag)
    {
        OIC_LOG(ERROR, TAG, "Observer notification error");
        result = OC_STACK_ERROR;
    }
    return result;
}

OCStackResult SendListObserverNotification (OCResource * resource,
                                            OCObservationId  *obsIdList,
                                            uint8_t numberOfIds,
                                            const OCRepPayload *payload,
                                            uint32_t maxAge,
                                            OCQualityOfService qos)
{
    (void)maxAge;
    if (!resource || !obsIdList || !payload)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (!resource->observersHead)
    {
        OIC_LOG(INFO, TAG, "Resource has no observers");
        return OC_STACK_NO_OBSERVERS;
    }

    uint8_t numIds = numberOfIds;
    ResourceObserver *observer = NULL;
    uint8_t numSentNotification = 0;
    /* OCServerRequest * request = NULL; */
    OCStackResult result = OC_STACK_ERROR;
    bool observeErrorFlag = false;

    OIC_LOG(INFO, TAG, "Entering SendListObserverNotification");
    while(numIds)
    {
        observer = GetObserverUsingId (resource, *obsIdList);
        if (observer)
        {
            qos = DetermineObserverQoS(OC_REST_GET, observer, qos);

            /* result = AddServerRequest(&request, 0, 0, 1, OC_REST_GET, */
            /*                           0, resource->sequenceNum, qos, observer->query, */
            /*                           NULL, OC_FORMAT_UNDEFINED, NULL, observer->token, observer->tokenLength, */
            /*                           observer->resUri, 0, observer->acceptFormat, */
            /*                           observer->acceptVersion, */
            /*                           &observer->devAddr); */

            struct oocf_msg_coap_request *request = (struct oocf_msg_coap_request*)OICMalloc(sizeof(struct oocf_msg_coap_request));
            request->delayedResNeeded = 0;
            request->notificationFlag = 1;
            request->method = CA_GET;
            request->observationOption = resource->sequenceNum;
            request->info.messageId = 0;
            request->info.type = qos;
            request->info.numOptions = 0;
            request->info.options = NULL;
            request->info.payloadFormat = OC_FORMAT_UNDEFINED;
            request->info.payload_cbor = NULL;
            request->info.payloadSize = 0;
            request->info.acceptFormat = observer->acceptFormat;
            request->info.acceptVersion = observer->acceptVersion;
            if (observer->tokenLength) {
                request->info.token = (uint8_t*)OICMalloc(observer->tokenLength);
                // VERIFY_NON_NULL_1(request->info.token);
                if ( request->info.token == NULL ) {
                    OICFree(request);
                    return OC_STACK_ERROR;
                }
                memcpy(request->info.token, observer->token, observer->tokenLength);
            }
            request->info.tokenLength = observer->tokenLength;

            request->info.resourceUri = (char *)OICMalloc(MAX_URI_LENGTH);
            OICStrcpy(request->info.resourceUri, MAX_URI_LENGTH, observer->resUri);
            OICStrcat(request->info.resourceUri, MAX_URI_LENGTH, observer->query);

            memcpy(&request->dest_ep, &observer->devAddr, sizeof(CAEndpoint_t));
            request = _oocf_request_cache_put(request);


            if (request)
            {
                request->observeResult = OC_STACK_OK;
                if (result == OC_STACK_OK)
                {
                    OCEntityHandlerResponse ehResponse = {0};
                    ehResponse.ehResult = OC_EH_OK;
                    ehResponse.payload = (OCPayload*)OCRepPayloadCreate();
                    if (!ehResponse.payload)
                    {
                        //DeleteServerRequest(request);
                        _oocf_request_cache_delete(request);                        continue;
                    }
                    memcpy(ehResponse.payload, payload, sizeof(*payload));
                    ehResponse.persistentBufferFlag = 0;
                    ehResponse.requestHandle = (OCRequestHandle) request;
                    result = OCDoResponse(&ehResponse);
                    if (result == OC_STACK_OK)
                    {
                        OIC_LOG_V(INFO, TAG, "Observer id %d notified.", *obsIdList);

                        // Increment only if OCDoResponse is successful
                        numSentNotification++;

                        OICFree(ehResponse.payload);
                    }
                    else
                    {
                        OIC_LOG_V(INFO, TAG, "Error notifying observer id %d.", *obsIdList);
                    }
                    // Reset Observer TTL.
                    observer->TTL =
                            GetTicks(MAX_OBSERVER_TTL_SECONDS * MILLISECONDS_PER_SECOND);
                }
                else
                {
                    //DeleteServerRequest(request);
                    _oocf_request_cache_delete(request);
                }
            }
            // Since we are in a loop, set an error flag to indicate
            // at least one error occurred.
            if (result != OC_STACK_OK)
            {
                observeErrorFlag = true;
            }
        }
        obsIdList++;
        numIds--;
    }

    if (numSentNotification == numberOfIds && !observeErrorFlag)
    {
        return OC_STACK_OK;
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Observer notification error");
        return OC_STACK_ERROR;
    }
}

OCStackResult GenerateObserverId (OCObservationId *observationId)
{
    OIC_LOG(INFO, TAG, "Entering GenerateObserverId");
    VERIFY_NON_NULL_1(observationId);

    do
    {
        if (!OCGetRandomBytes((uint8_t*)observationId, sizeof(OCObservationId)))
        {
            OIC_LOG(ERROR, TAG, "Failed to generate random observationId");
            goto exit;
        }
    // Check if observation Id already exists.
    } while (IsObservationIdExisting(*observationId));

    OIC_LOG_V(INFO, TAG, "GeneratedObservation ID is %u", *observationId);

    return OC_STACK_OK;
exit:
    return OC_STACK_ERROR;
}

OCStackResult AddObserver (const char         *resUri,
                           const char         *query,
                           OCObservationId     obsId,
                           uint8_t            *token,
                           uint8_t             tokenLength,
                           OCResource         *resHandle,
                           OCQualityOfService  qos,
                           OCPayloadFormat     acceptFormat,
                           uint16_t            acceptVersion,
                           const CAEndpoint_t *devAddr)
                           /* const OCDevAddr    *devAddr) */
{
    // Check if resource exists and is observable.
    if (!resHandle)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (!(resHandle->resourceProperties & OC_OBSERVABLE))
    {
        return OC_STACK_RESOURCE_ERROR;
    }

    if (!resUri || !token)
    {
        return OC_STACK_INVALID_PARAM;
    }

    ResourceObserver *obsNode = (ResourceObserver *) OICCalloc(1, sizeof(ResourceObserver));
    if (obsNode)
    {
        obsNode->observeId = obsId;

        obsNode->resUri = OICStrdup(resUri);
        VERIFY_NON_NULL_1(obsNode->resUri);

        obsNode->qos = qos;
        obsNode->acceptFormat = acceptFormat;
        obsNode->acceptVersion = acceptVersion;
        if (query)
        {
            obsNode->query = OICStrdup(query);
            VERIFY_NON_NULL_1(obsNode->query);
        }
        // If tokenLength is zero, the return value depends on the
        // particular library implementation (it may or may not be a null pointer).
        if (tokenLength)
        {
            obsNode->token = (uint8_t*)OICMalloc(tokenLength);
            VERIFY_NON_NULL_1(obsNode->token);
            memcpy(obsNode->token, token, tokenLength);
        }
        obsNode->tokenLength = tokenLength;

        obsNode->devAddr = *devAddr;

        if ((strcmp(resUri, OC_RSRVD_PRESENCE_URI) == 0))
        {
            obsNode->TTL = 0;
        }
        else
        {
            obsNode->TTL = GetTicks(MAX_OBSERVER_TTL_SECONDS * MILLISECONDS_PER_SECOND);
        }

        LL_APPEND (resHandle->observersHead, obsNode);

        return OC_STACK_OK;
    }

exit:
    if (obsNode)
    {
        OICFree(obsNode->resUri);
        OICFree(obsNode->query);
        OICFree(obsNode);
    }
    return OC_STACK_NO_MEMORY;
}

/*
 * This function checks if the node is past its time to live and
 * deletes it if timed-out. Calling this function with a  presence callback
 * with ttl set to 0 will not delete anything as presence nodes have
 * their own mechanisms for timeouts. A null argument will cause the function to
 * silently return.
 */
static void CheckTimedOutObserver(ResourceObserver* observer, OCResource *resource)
{
    if (!observer || observer->TTL == 0)
    {
        return;
    }

    coap_tick_t now = 0;
    coap_ticks(&now);

    if (observer->TTL < now)
    {
        // Send confirmable notification message to observer.
        OIC_LOG(INFO, TAG, "Sending High-QoS notification to observer");
        SendObserveNotification(observer, resource->sequenceNum, OC_HIGH_QOS);
    }
}

ResourceObserver* GetObserverUsingId(OCResource *resource,
                                     const OCObservationId observeId)
{
    ResourceObserver *out = NULL;

    LL_FOREACH (resource->observersHead, out)
    {
        if (out->observeId == observeId)
        {
            return out;
        }
        CheckTimedOutObserver(out, resource);
    }

    if (!out)
    {
        OIC_LOG(INFO, TAG, "Observer node not found!!");
    }
    return NULL;
}

ResourceObserver* GetObserverUsingToken(OCResource *resource,
                                        const uint8_t *token, uint8_t tokenLength)
{
    if (token)
    {
        OIC_LOG(INFO, TAG, "Looking for token");
        OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)token, tokenLength);

        ResourceObserver *out = NULL;
        LL_FOREACH (resource->observersHead, out)
        {
            /* de-annotate below line if want to see all token in cbList */
            //OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)out->token, tokenLength);
            if ((memcmp(out->token, token, tokenLength) == 0))
            {
                OIC_LOG(INFO, TAG, "Found in observer list");
                return out;
            }
            CheckTimedOutObserver(out, resource);
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Passed in NULL token");
    }

    OIC_LOG(INFO, TAG, "Observer node not found!!");
    return NULL;
}

OCStackResult DeleteObserverUsingToken (OCResource *resource,
                                        uint8_t *token, uint8_t tokenLength)
{
    if (!token)
    {
        return OC_STACK_INVALID_PARAM;
    }

    ResourceObserver *obsNode = GetObserverUsingToken (resource, token, tokenLength);
    if (obsNode)
    {
        OIC_LOG_V(INFO, TAG, "deleting observer id  %u with token", obsNode->observeId);
        OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)obsNode->token, tokenLength);
        LL_DELETE (resource->observersHead, obsNode);
        OICFree(obsNode->resUri);
        OICFree(obsNode->query);
        OICFree(obsNode->token);
        OICFree(obsNode);
        obsNode = NULL;
    }
    // it is ok if we did not find the observer...
    return OC_STACK_OK;
}

void DeleteObserverList(OCResource *resource)
{
    ResourceObserver *out = NULL;
    ResourceObserver *tmp = NULL;
    LL_FOREACH_SAFE(resource->observersHead, out, tmp)
    {
        DeleteObserverUsingToken(resource, out->token, out->tokenLength);
    }
    resource->observersHead = NULL;
}

/*
 * CA layer expects observe registration/de-reg/notiifcations to be passed as a header
 * option, which breaks the protocol abstraction requirement between RI & CA, and
 * has to be fixed in the future. The function below adds the header option for observe.
 * It should be noted that the observe header option is assumed to be the first option
 * in the list of user defined header options and hence it is inserted at the front
 * of the header options list and number of options adjusted accordingly.
 */
OCStackResult
CreateObserveHeaderOption (CAHeaderOption_t **caHdrOpt,
                           OCHeaderOption *ocHdrOpt,
                           uint8_t numOptions,
                           uint8_t observeFlag)
{
    if (!caHdrOpt)
    {
        return OC_STACK_INVALID_PARAM;
    }

    if (numOptions > 0 && !ocHdrOpt)
    {
        OIC_LOG (INFO, TAG, "options are NULL though number is non zero");
        return OC_STACK_INVALID_PARAM;
    }

    CAHeaderOption_t *tmpHdrOpt = NULL;

    tmpHdrOpt = (CAHeaderOption_t *) OICCalloc ((numOptions+1), sizeof(CAHeaderOption_t));
    if (NULL == tmpHdrOpt)
    {
        return OC_STACK_NO_MEMORY;
    }
    //tmpHdrOpt[0].protocolID = CA_COAP_ID;
    tmpHdrOpt[0].optionID = COAP_OPTION_OBSERVE;
    tmpHdrOpt[0].optionLength = sizeof(uint8_t);
    tmpHdrOpt[0].optionData[0] = observeFlag;
    for (uint8_t i = 0; i < numOptions; i++)
    {
        memcpy (&(tmpHdrOpt[i+1]), &(ocHdrOpt[i]), sizeof(CAHeaderOption_t));
    }

    *caHdrOpt = tmpHdrOpt;
    return OC_STACK_OK;
}

/*
 * CA layer passes observe information to the RI layer as a header option, which
 * breaks the protocol abstraction requirement between RI & CA, and has to be fixed
 * in the future. The function below removes the observe header option and processes it.
 * It should be noted that the observe header option is always assumed to be the first
 * option in the list of user defined header options and hence it is deleted from the
 * front of the header options list and the number of options is adjusted accordingly.
 */
OCStackResult
GetObserveHeaderOption (uint32_t * observationOption,
                        CAHeaderOption_t *options,
                        uint8_t * numOptions)
{
    if (!observationOption)
    {
        return OC_STACK_INVALID_PARAM;
    }

    if (!options || !numOptions)
    {
        OIC_LOG (INFO, TAG, "No options present");
        return OC_STACK_OK;
    }

    for(uint8_t i = 0; i < *numOptions; i++)
    {
        if (options[i].optionID == COAP_OPTION_OBSERVE)
            /* && */
            /* options[i].protocolID == CA_COAP_ID) */
        {
            *observationOption = options[i].optionData[0];
            for(uint8_t c = i; c < *numOptions-1; c++)
            {
                options[i] = options[i+1];
            }
            (*numOptions)--;
            return OC_STACK_OK;
        }
    }
    return OC_STACK_OK;
}

 /* FIXME: dup name, also in ocresource.c */
OCStackResult HandleVirtualObserveRequest(struct oocf_msg_coap_request *request)
{
    OCStackResult result = OC_STACK_OK;
    if (request->notificationFlag)
    {
        // The request is to send an observe payload, not register/deregister an observer
        goto exit;
    }
    OCVirtualResources virtualUriInRequest = OC_UNKNOWN_URI;
    virtualUriInRequest = GetTypeOfVirtualURI(getPathFromRequestURL(request->info.resourceUri));
    if (virtualUriInRequest != OC_WELL_KNOWN_URI)
    {
        // OC_WELL_KNOWN_URI is currently the only virtual resource that may be observed
        goto exit;
    }
    OCResource *resourcePtr = NULL;
    resourcePtr = FindResourceByUri(OC_RSRVD_WELL_KNOWN_URI);
    if (NULL == resourcePtr)
    {
        OIC_LOG(FATAL, TAG, "Well-known URI not found.");
        result = OC_STACK_ERROR;
        goto exit;
    }
    if (request->observationOption == OC_OBSERVE_REGISTER)
    {
        OIC_LOG(INFO, TAG, "Observation registration requested");
        ResourceObserver *obs = GetObserverUsingToken (resourcePtr,
                                                       request->info.token,
                                                       request->info.tokenLength);
        if (obs)
        {
            OIC_LOG (INFO, TAG, "Observer with this token already present");
            OIC_LOG (INFO, TAG, "Possibly re-transmitted CON OBS request");
            OIC_LOG (INFO, TAG, "Not adding observer. Not responding to client");
            OIC_LOG (INFO, TAG, "The first request for this token is already ACKED.");
            result = OC_STACK_DUPLICATE_REQUEST;
            goto exit;
        }
        OCObservationId obsId;
        result = GenerateObserverId(&obsId);
        if (result == OC_STACK_OK)
        {
            result = AddObserver ((const char*)(getPathFromRequestURL(request->info.resourceUri)),
                                  (const char *)(getQueryFromRequestURL(request->info.resourceUri)),
                                  obsId, request->info.token, request->info.tokenLength,
                                  resourcePtr, request->info.type /* qos */, request->info.acceptFormat,
                                  request->info.acceptVersion,
                                  &request->dest_ep); // devAddr);
        }
        if (result == OC_STACK_OK)
        {
            OIC_LOG(INFO, TAG, "Added observer successfully");
            request->observeResult = OC_STACK_OK;
        }
        else if (result == OC_STACK_RESOURCE_ERROR)
        {
            OIC_LOG(INFO, TAG, "The Resource is not active, discoverable or observable");
            request->observeResult = OC_STACK_ERROR;
        }
        else
        {
            // The error in observeResult for the request will be used when responding to this
            // request by omitting the observation option/sequence number.
            request->observeResult = OC_STACK_ERROR;
            OIC_LOG(ERROR, TAG, "Observer Addition failed");
        }
    }
    else if (request->observationOption == OC_OBSERVE_DEREGISTER)
    {
        OIC_LOG(INFO, TAG, "Deregistering observation requested");
        result = DeleteObserverUsingToken (resourcePtr,
                                           request->info.token,
                                           request->info.tokenLength);
        if (result == OC_STACK_OK)
        {
            OIC_LOG(INFO, TAG, "Removed observer successfully");
            request->observeResult = OC_STACK_OK;
            // There should be no observe option header for de-registration response.
            // Set as an invalid value here so we can detect it later and remove the field in response.
            request->observationOption = MAX_SEQUENCE_NUMBER + 1;
        }
        else
        {
            request->observeResult = OC_STACK_ERROR;
            OIC_LOG(ERROR, TAG, "Observer Removal failed");
        }
    }
    // Whether the observe request succeeded or failed, the request is processed as normal
    // and excludes/includes the OBSERVE option depending on request->observeResult
    result = OC_STACK_OK;

exit:
    return result;
}
