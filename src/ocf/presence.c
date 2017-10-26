/* presence.c
 * extracted from ocstack.c */

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

#include "iotivity_config.h"
#include "iotivity_debug.h"

#include "occlientcb.h"
#include "occlientcb_api.h"
#include "ocobserve.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "presence_methods.h"
#include "ocserverrequest.h"
#include "ocstackinternal.h"
#include "ocrandom.h"
#include "oic_malloc.h"
#include "oic_string.h"

/* //src/comm/api */
#include "cainterface.h"

#include "utlist.h"

/* #ifdef __cplusplus
 * extern "C" {
 * #endif // __cplusplus */

/* #ifdef WITH_PRESENCE */

#define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OIC_LOG((logLevel), \
             __FILE__, #arg " is NULL"); return (retVal); } }

#define VERIFY_NON_NULL_V(arg) { if (!arg) {OIC_LOG(FATAL, __FILE__, #arg " is NULL");\
    goto exit;} }

OCStackResult OCProcessPresence()
{
    OCStackResult result = OC_STACK_OK;

    // the following line floods the log with messages that are irrelevant
    // to most purposes.  Uncomment as needed.
    //OIC_LOG(INFO, __FILE__, "Entering RequestPresence");
    ClientCB* cbNode = NULL;
    ClientCB* cbTemp = NULL;
    OCClientResponse clientResponse;
    OCStackApplicationResult cbResult = OC_STACK_DELETE_TRANSACTION;

    LL_FOREACH_SAFE(g_cbList, cbNode, cbTemp)
    {
        if (OC_REST_PRESENCE != cbNode->method || !cbNode->presence)
        {
            continue;
        }

        uint32_t now = GetTicks(0);
        OIC_LOG_V(DEBUG, __FILE__, "this TTL level %d",
                                                cbNode->presence->TTLlevel);
        OIC_LOG_V(DEBUG, __FILE__, "current ticks %d", now);

        if (cbNode->presence->TTLlevel > PresenceTimeOutSize)
        {
            goto exit;
        }

        if (cbNode->presence->TTLlevel < PresenceTimeOutSize)
        {
            OIC_LOG_V(DEBUG, __FILE__, "timeout ticks %d",
                    cbNode->presence->timeOut[cbNode->presence->TTLlevel]);
        }
        if (cbNode->presence->TTLlevel >= PresenceTimeOutSize)
        {
            OIC_LOG(DEBUG, __FILE__, "No more timeout ticks");

            clientResponse.sequenceNumber = 0;
            clientResponse.result = OC_STACK_PRESENCE_TIMEOUT;
            clientResponse.devAddr = *cbNode->devAddr;
            FixUpClientResponse(&clientResponse);
            clientResponse.payload = NULL;

            // Increment the TTLLevel (going to a next state), so we don't keep
            // sending presence notification to client.
            cbNode->presence->TTLlevel++;
            OIC_LOG_V(DEBUG, __FILE__, "moving to TTL level %d",
                                        cbNode->presence->TTLlevel);

            cbResult = cbNode->callBack(cbNode->context, cbNode->handle, &clientResponse);
            if (cbResult == OC_STACK_DELETE_TRANSACTION)
            {
                DeleteClientCB(cbNode);
            }
        }

        if (now < cbNode->presence->timeOut[cbNode->presence->TTLlevel])
        {
            continue;
        }

        CAEndpoint_t endpoint = {.adapter = CA_DEFAULT_ADAPTER};
        CAInfo_t requestData = {.type = CA_MSG_CONFIRM};
        CARequestInfo_t requestInfo = {.method = CA_GET};

        OIC_LOG(DEBUG, __FILE__, "time to test server presence");

        CopyDevAddrToEndpoint(cbNode->devAddr, &endpoint);

        requestData.type = CA_MSG_NONCONFIRM;
        requestData.token = cbNode->token;
        requestData.tokenLength = cbNode->tokenLength;
        requestData.resourceUri = OC_RSRVD_PRESENCE_URI;
        requestInfo.method = CA_GET;
        requestInfo.info = requestData;

        result = OCSendRequest(&endpoint, &requestInfo);
        if (OC_STACK_OK != result)
        {
            goto exit;
        }

        cbNode->presence->TTLlevel++;
        OIC_LOG_V(DEBUG, __FILE__, "moving to TTL level %d", cbNode->presence->TTLlevel);
    }
exit:
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, __FILE__, "OCProcessPresence error");
    }

    return result;
}
/* #endif // WITH_PRESENCE */

/* #ifdef WITH_PRESENCE */
OCStackResult OCChangeResourceProperty(OCResourceProperty * inputProperty,
        OCResourceProperty resourceProperties, uint8_t enable)
{
    if (!inputProperty)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (resourceProperties
            > (OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SLOW))
    {
        OIC_LOG(ERROR, __FILE__, "Invalid property");
        return OC_STACK_INVALID_PARAM;
    }
    if(!enable)
    {
        *inputProperty = (OCResourceProperty) (*inputProperty & ~(resourceProperties));
    }
    else
    {
        *inputProperty = (OCResourceProperty) (*inputProperty | resourceProperties);
    }
    return OC_STACK_OK;
}
/* #endif */


/* #ifdef WITH_PRESENCE */
OCStackResult OC_CALL OCStartPresence(const uint32_t ttl)
{
    OIC_LOG(INFO, __FILE__, "Entering OCStartPresence");
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    if (NULL == presenceResource.handle)
    {
        OIC_LOG(ERROR, __FILE__, "Invalid Presence Resource Handle: Not Initialized");
        return OC_STACK_ERROR;
    }

    OCChangeResourceProperty(
            &(((OCResource *)presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 1);

    if (OC_MAX_PRESENCE_TTL_SECONDS < ttl)
    {
        presenceResource.presenceTTL = OC_MAX_PRESENCE_TTL_SECONDS;
        OIC_LOG(INFO, __FILE__, "Setting Presence TTL to max value");
    }
    else if (0 == ttl)
    {
        presenceResource.presenceTTL = OC_DEFAULT_PRESENCE_TTL_SECONDS;
        OIC_LOG(INFO, __FILE__, "Setting Presence TTL to default value");
    }
    else
    {
        presenceResource.presenceTTL = ttl;
    }
    OIC_LOG_V(DEBUG, __FILE__, "Presence TTL is %" PRIu32 " seconds", presenceResource.presenceTTL);

    if (OC_PRESENCE_UNINITIALIZED == presenceState)
    {
        presenceState = OC_PRESENCE_INITIALIZED;

        OCDevAddr devAddr = { OC_DEFAULT_ADAPTER };

        CAToken_t caToken = NULL;
        CAResult_t caResult = CAGenerateToken(&caToken, tokenLength);
        if (caResult != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, __FILE__, "CAGenerateToken error");
            CADestroyToken(caToken);
            return OC_STACK_ERROR;
        }

        AddObserver(OC_RSRVD_PRESENCE_URI, NULL, 0, caToken, tokenLength,
                (OCResource *) presenceResource.handle, OC_LOW_QOS, OC_FORMAT_UNDEFINED,
                OCF_VERSION_1_0_0, &devAddr);
        CADestroyToken(caToken);
    }

    // Each time OCStartPresence is called
    // a different random 32-bit integer number is used
    ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();

    return SendPresenceNotification(((OCResource *)presenceResource.handle)->rsrcType,
            OC_PRESENCE_TRIGGER_CREATE);
}

OCStackResult OC_CALL OCStopPresence()
{
    OIC_LOG(INFO, __FILE__, "Entering OCStopPresence");
    OCStackResult result = OC_STACK_ERROR;

    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();

    // make resource inactive
    result = OCChangeResourceProperty(
            &(((OCResource *) presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 0);
    }

    if(result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, __FILE__,
                      "Changing the presence resource properties to ACTIVE not successful");
        return result;
    }

    return SendStopNotification();
}
/* #endif */

/* #ifdef WITH_PRESENCE */
OCStackResult SendPresenceNotification(OCResourceType *resourceType,
        OCPresenceTrigger trigger)
{
    OIC_LOG_V(INFO, __FILE__, "%s: ENTRY", __func__);
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

        result = SendAllObserverNotificationWithPresence(method, resPtr, maxAge,
                trigger, resourceType, OC_LOW_QOS);
    }

    OIC_LOG_V(INFO, __FILE__, "%s: EXIT", __func__);
    return result;
}

OCStackResult SendStopNotification()
{
    OIC_LOG(INFO, __FILE__, "SendStopNotification");
    OCResource *resPtr = NULL;
    OCStackResult result = OC_STACK_ERROR;
    OCMethod method = OC_REST_PRESENCE;
    resPtr = findResource((OCResource *) presenceResource.handle);
    if(NULL == resPtr)
    {
        return OC_STACK_NO_RESOURCE;
    }

    // maxAge is 0. ResourceType is NULL.
    result = SendAllObserverNotificationWithPresence(method, resPtr, 0, OC_PRESENCE_TRIGGER_DELETE,
            NULL, OC_LOW_QOS);

    return result;
}
/* #endif // WITH_PRESENCE */

OCStackResult ResetPresenceTTL(ClientCB *cbNode, uint32_t maxAgeSeconds)
{
    uint32_t lowerBound  = 0;
    uint32_t higherBound = 0;

    if (!cbNode || !cbNode->presence || !cbNode->presence->timeOut)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OIC_LOG_V(INFO, __FILE__, "Update presence TTL, time is %u", GetTicks(0));

    cbNode->presence->TTL = maxAgeSeconds;

    for (int index = 0; index < PresenceTimeOutSize; index++)
    {
        // Guard against overflow
        if (cbNode->presence->TTL < (UINT32_MAX/(MILLISECONDS_PER_SECOND*PresenceTimeOut[index]))
                                     * 100)
        {
            lowerBound = GetTicks((PresenceTimeOut[index] *
                                  cbNode->presence->TTL *
                                  MILLISECONDS_PER_SECOND)/100);
        }
        else
        {
            lowerBound = GetTicks(UINT32_MAX);
        }

        if (cbNode->presence->TTL < (UINT32_MAX/(MILLISECONDS_PER_SECOND*PresenceTimeOut[index+1]))
                                     * 100)
        {
            higherBound = GetTicks((PresenceTimeOut[index + 1] *
                                   cbNode->presence->TTL *
                                   MILLISECONDS_PER_SECOND)/100);
        }
        else
        {
            higherBound = GetTicks(UINT32_MAX);
        }

        cbNode->presence->timeOut[index] = OCGetRandomRange(lowerBound, higherBound);

        OIC_LOG_V(DEBUG, __FILE__, "lowerBound timeout  %d", lowerBound);
        OIC_LOG_V(DEBUG, __FILE__, "higherBound timeout %d", higherBound);
        OIC_LOG_V(DEBUG, __FILE__, "timeOut entry  %d", cbNode->presence->timeOut[index]);
    }

    cbNode->presence->TTLlevel = 0;

    OIC_LOG_V(DEBUG, __FILE__, "this TTL level %d", cbNode->presence->TTLlevel);
    return OC_STACK_OK;
}

const char *OC_CALL convertTriggerEnumToString(OCPresenceTrigger trigger)
{
    if (trigger == OC_PRESENCE_TRIGGER_CREATE)
    {
        return OC_RSRVD_TRIGGER_CREATE;
    }
    else if (trigger == OC_PRESENCE_TRIGGER_CHANGE)
    {
        return OC_RSRVD_TRIGGER_CHANGE;
    }
    else
    {
        return OC_RSRVD_TRIGGER_DELETE;
    }
}

OCPresenceTrigger OC_CALL convertTriggerStringToEnum(const char * triggerStr)
{
    if(!triggerStr)
    {
        return OC_PRESENCE_TRIGGER_CREATE;
    }
    else if(strcmp(triggerStr, OC_RSRVD_TRIGGER_CREATE) == 0)
    {
        return OC_PRESENCE_TRIGGER_CREATE;
    }
    else if(strcmp(triggerStr, OC_RSRVD_TRIGGER_CHANGE) == 0)
    {
        return OC_PRESENCE_TRIGGER_CHANGE;
    }
    else
    {
        return OC_PRESENCE_TRIGGER_DELETE;
    }
}

/**
 * The cononical presence allows constructed URIs to be string compared.
 *
 * requestUri must be a char array of size CA_MAX_URI_LENGTH
 */
static int FormCanonicalPresenceUri(const CAEndpoint_t *endpoint,
                                    char *presenceUri, bool isMulticast)
{
    VERIFY_NON_NULL(endpoint   , FATAL, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(presenceUri, FATAL, OC_STACK_INVALID_PARAM);

    if (isMulticast)
    {
        OIC_LOG(DEBUG, __FILE__, "Make Multicast Presence URI");
        return snprintf(presenceUri, CA_MAX_URI_LENGTH, "%s", OC_RSRVD_PRESENCE_URI);
    }

    CAEndpoint_t *ep = (CAEndpoint_t *)endpoint;
    if (ep->adapter == CA_ADAPTER_IP)
    {
        if ((ep->flags & CA_IPV6) && !(ep->flags & CA_IPV4))
        {
            if ('\0' == ep->addr[0])  // multicast
            {
                return snprintf(presenceUri, CA_MAX_URI_LENGTH, OC_RSRVD_PRESENCE_URI);
            }
            else
            {
                char addressEncoded[CA_MAX_URI_LENGTH] = {0};

                OCStackResult result = OCEncodeAddressForRFC6874(addressEncoded,
                                                                 sizeof(addressEncoded),
                                                                 ep->addr);

                if (OC_STACK_OK != result)
                {
                    return -1;
                }

                return snprintf(presenceUri, CA_MAX_URI_LENGTH, "coap://[%s]:%u%s",
                        addressEncoded, ep->port, OC_RSRVD_PRESENCE_URI);
            }
        }
        else
        {
            if ('\0' == ep->addr[0])  // multicast
            {
                OICStrcpy(ep->addr, sizeof(ep->addr), OC_MULTICAST_IP);
                ep->port = OC_MULTICAST_PORT;
            }
            return snprintf(presenceUri, CA_MAX_URI_LENGTH, "coap://%s:%u%s",
                    ep->addr, ep->port, OC_RSRVD_PRESENCE_URI);
        }
    }

    // might work for other adapters (untested, but better than nothing)
    return snprintf(presenceUri, CA_MAX_URI_LENGTH, "coap://%s%s", ep->addr,
                    OC_RSRVD_PRESENCE_URI);
}

OCStackResult HandlePresenceResponse(const CAEndpoint_t *endpoint,
                            const CAResponseInfo_t *responseInfo)
{
    VERIFY_NON_NULL(endpoint, FATAL, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(responseInfo, FATAL, OC_STACK_INVALID_PARAM);

    OCStackApplicationResult cbResult = OC_STACK_DELETE_TRANSACTION;
    ClientCB * cbNode = NULL;
    char *resourceTypeName = NULL;
    OCClientResponse *response = NULL;
    OCStackResult result = OC_STACK_ERROR;
    uint32_t maxAge = 0;
    int uriLen;
    char presenceUri[CA_MAX_URI_LENGTH];

    int presenceSubscribe = 0;
    int multicastPresenceSubscribe = 0;

    if (responseInfo->result != CA_CONTENT)
    {
        OIC_LOG_V(ERROR, __FILE__, "HandlePresenceResponse failed %d", responseInfo->result);
        return OC_STACK_ERROR;
    }

    response = (OCClientResponse *)OICCalloc(1, sizeof(*response));
    if (!response)
    {
            OIC_LOG(ERROR, __FILE__, "Allocating memory for response failed");
            return OC_STACK_ERROR;
    }
    response->devAddr.adapter = OC_DEFAULT_ADAPTER;

    response->payload = NULL;
    response->result = OC_STACK_OK;

    CopyEndpointToDevAddr(endpoint, &response->devAddr);
    FixUpClientResponse(response);

    if (responseInfo->info.payload)
    {
        result = OCParsePayload(&response->payload,
                CAToOCPayloadFormat(responseInfo->info.payloadFormat),
                PAYLOAD_TYPE_PRESENCE,
                responseInfo->info.payload,
                responseInfo->info.payloadSize);

        if(result != OC_STACK_OK)
        {
            OIC_LOG(ERROR, __FILE__, "Presence parse failed");
            goto exit;
        }
        if(!response->payload || response->payload->type != PAYLOAD_TYPE_PRESENCE)
        {
            OIC_LOG(ERROR, __FILE__, "Presence payload was wrong type");
            result = OC_STACK_ERROR;
            goto exit;
        }
        response->sequenceNumber = ((OCPresencePayload*)response->payload)->sequenceNumber;
        resourceTypeName = ((OCPresencePayload*)response->payload)->resourceType;
        maxAge = ((OCPresencePayload*)response->payload)->maxAge;
    }

    // check for unicast presence
    uriLen = FormCanonicalPresenceUri(endpoint, presenceUri,
                                      responseInfo->isMulticast);
    if (uriLen < 0 || (size_t)uriLen >= sizeof (presenceUri))
    {
        result = OC_STACK_INVALID_URI;
        goto exit;
    }
    OIC_LOG(INFO, __FILE__, "check for unicast presence");
    cbNode = GetClientCBUsingUri(presenceUri);
    if (cbNode)
    {
        presenceSubscribe = 1;
    }
    else
    {
        // check for multicast presence
        OIC_LOG(INFO, __FILE__, "check for multicast presence");
        cbNode = GetClientCBUsingUri(OC_RSRVD_PRESENCE_URI);
        if (cbNode)
        {
            multicastPresenceSubscribe = 1;
        }
    }

    if (!presenceSubscribe && !multicastPresenceSubscribe)
    {
        OIC_LOG(INFO, __FILE__, "Received a presence notification, "
                "but need to register presence callback, ignoring");
        goto exit;
    }

    if (presenceSubscribe)
    {
        if(cbNode->sequenceNumber == response->sequenceNumber)
        {
            OIC_LOG(INFO, __FILE__, "No presence change");
            ResetPresenceTTL(cbNode, maxAge);
            OIC_LOG_V(INFO, __FILE__, "ResetPresenceTTL - TTLlevel:%d\n", cbNode->presence->TTLlevel);
            goto exit;
        }

        if(maxAge == 0)
        {
            OIC_LOG(INFO, __FILE__, "Stopping presence");
            response->result = OC_STACK_PRESENCE_STOPPED;
            if(cbNode->presence)
            {
                OICFree(cbNode->presence->timeOut);
                OICFree(cbNode->presence);
                cbNode->presence = NULL;
            }
        }
        else
        {
            if(!cbNode->presence)
            {
                cbNode->presence = (OCPresence *)OICMalloc(sizeof (OCPresence));

                if(!(cbNode->presence))
                {
                    OIC_LOG(ERROR, __FILE__, "Could not allocate memory for cbNode->presence");
                    result = OC_STACK_NO_MEMORY;
                    goto exit;
                }

                VERIFY_NON_NULL_V(cbNode->presence);
                cbNode->presence->timeOut = NULL;
                cbNode->presence->timeOut = (uint32_t *)
                        OICMalloc(PresenceTimeOutSize * sizeof(uint32_t));
                if(!(cbNode->presence->timeOut)){
                    OIC_LOG(ERROR, __FILE__,
                                  "Could not allocate memory for cbNode->presence->timeOut");
                    OICFree(cbNode->presence);
                    result = OC_STACK_NO_MEMORY;
                    goto exit;
                }
            }

            ResetPresenceTTL(cbNode, maxAge);

            cbNode->sequenceNumber = response->sequenceNumber;
        }
    }
    else
    {
        // This is the multicast case
        OIC_LOG(INFO, __FILE__, "this is the multicast presence");
        if (0 == maxAge)
        {
            OIC_LOG(INFO, __FILE__, "Stopping presence");
            response->result = OC_STACK_PRESENCE_STOPPED;
        }
    }

    // Ensure that a filter is actually applied.
    if (resourceTypeName && cbNode->interestingPresenceResourceType)
    {
        OIC_LOG_V(INFO, __FILE__, "find resource type : %s", resourceTypeName);
        if(!findResourceType(cbNode->interestingPresenceResourceType, resourceTypeName))
        {
            goto exit;
        }
    }

    OIC_LOG(INFO, __FILE__, "Callback for presence");

    cbResult = cbNode->callBack(cbNode->context, cbNode->handle, response);

    if (cbResult == OC_STACK_DELETE_TRANSACTION)
    {
        DeleteClientCB(cbNode);
    }

exit:
    OCPayloadDestroy(response->payload);
    OICFree(response);
    return result;
}

OCStackResult OCPreparePresence(CAEndpoint_t *endpoint,
                                       char **requestUri,
                                       bool isMulticast)
{
    char uri[CA_MAX_URI_LENGTH];

    FormCanonicalPresenceUri(endpoint, uri, isMulticast);

    *requestUri = OICStrdup(uri);
    if (!*requestUri)
    {
        return OC_STACK_NO_MEMORY;
    }

    return OC_STACK_OK;
}

/* from ocpayload.h: */
OCPresencePayload* OC_CALL OCPresencePayloadCreate(uint32_t seqNum, uint32_t maxAge,
        OCPresenceTrigger trigger, const char* resourceType)
{
    OCPresencePayload* payload = (OCPresencePayload*)OICCalloc(1, sizeof(OCPresencePayload));
    if (!payload)
    {
        return NULL;
    }

    payload->base.type = PAYLOAD_TYPE_PRESENCE;
    payload->sequenceNumber = seqNum;
    payload->maxAge = maxAge;
    payload->trigger = trigger;
    payload->resourceType = OICStrdup(resourceType);
    return payload;
}

void OC_CALL OCPresencePayloadDestroy(OCPresencePayload* payload)
{
    if (!payload)
    {
        return;
    }
    OICFree(payload->resourceType);
    OICFree(payload);
}

/* #ifdef WITH_PRESENCE */
OCStackResult SendAllObserverNotificationWithPresence (OCMethod method,
						       OCResource *resPtr,
						       uint32_t maxAge,
						       OCPresenceTrigger trigger,
						       OCResourceType *resourceType,
						       OCQualityOfService qos)
/* #else
 * OCStackResult SendAllObserverNotification (OCMethod method, OCResource *resPtr, uint32_t maxAge,
 *         OCQualityOfService qos)
 * #endif */
{
    OIC_LOG_V(INFO, __FILE__, "%s: ENTRY", __func__);
    if (!resPtr)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (!resPtr->observersHead)
    {
        OIC_LOG(INFO, __FILE__, "Resource has no observers");
        return OC_STACK_NO_OBSERVERS;
    }

    OCStackResult result = OC_STACK_ERROR;
    ResourceObserver * resourceObserver = resPtr->observersHead;
    OCServerRequest * request = NULL;
    bool observeErrorFlag = false;

    // Find clients that are observing this resource
    while (resourceObserver)
    {
#ifdef WITH_PRESENCE
        if (method != OC_REST_PRESENCE)
        {
#endif
            qos = DetermineObserverQoS(method, resourceObserver, qos);
            result = SendObserveNotification(resourceObserver, resPtr->sequenceNum, qos);
#ifdef WITH_PRESENCE
        }
        else
        {
            OCEntityHandlerResponse ehResponse = {0};

            //This is effectively the implementation for the presence entity handler.
            OIC_LOG(DEBUG, __FILE__, "This notification is for Presence");
            result = AddServerRequest(&request, 0, 0, 1, OC_REST_GET,
                    0, resPtr->sequenceNum, qos, resourceObserver->query,
                    NULL, OC_FORMAT_UNDEFINED, NULL,
                    resourceObserver->token, resourceObserver->tokenLength,
                    resourceObserver->resUri, 0, resourceObserver->acceptFormat,
                    resourceObserver->acceptVersion, &resourceObserver->devAddr);

            if (result == OC_STACK_OK)
            {
                OCPresencePayload* presenceResBuf = OCPresencePayloadCreate(
                        resPtr->sequenceNum, maxAge, trigger,
                        resourceType ? resourceType->resourcetypename : NULL);

                if (!presenceResBuf)
                {
                    return OC_STACK_NO_MEMORY;
                }

                if (result == OC_STACK_OK)
                {
                    ehResponse.ehResult = OC_EH_OK;
                    ehResponse.payload = (OCPayload*)presenceResBuf;
                    ehResponse.persistentBufferFlag = 0;
                    ehResponse.requestHandle = (OCRequestHandle) request;
                    OICStrcpy(ehResponse.resourceUri, sizeof(ehResponse.resourceUri),
                            resourceObserver->resUri);
                    result = OCDoResponse(&ehResponse);
                }

                OCPresencePayloadDestroy(presenceResBuf);
            }
        }
#endif

        // Since we are in a loop, set an error flag to indicate at least one error occurred.
        if (result != OC_STACK_OK)
        {
            observeErrorFlag = true;
        }

        resourceObserver = resourceObserver->next;
    }

    if (observeErrorFlag)
    {
        OIC_LOG(ERROR, __FILE__, "Observer notification error");
        result = OC_STACK_ERROR;
    }
    return result;
}

/* #ifdef __cplusplus
 * }
 * #endif // __cplusplus */
