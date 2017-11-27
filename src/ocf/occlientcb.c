/* ****************************************************************
 *
 * Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "occlientcb.h"

/* #include "iotivity_config.h" */
/* #include "occlientcb_api.h" */

#include "coap_config.h"
#include "coap/coap_time.h"

/* #include "logger.h" */
/* #include "trace.h" */
/* #include "oic_malloc.h" */

#include "utlist.h"

#include <string.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <assert.h>

/* #include "cacommon.h" */
/* #include "cainterface.h" */

/// Module Name
#define TAG "OIC_RI_CLIENTCB"

#if EXPORT_INTERFACE
/**
 * Client applications implement this callback to consume responses received from Servers.
 */
typedef OCStackApplicationResult (* OCClientResponseHandler)(void *context, OCDoHandle handle,
    OCClientResponse * clientResponse);

/**
 * Client applications using a context pointer implement this callback to delete the
 * context upon removal of the callback/context pointer from the internal callback-list.
 */
typedef void (* OCClientContextDeleter)(void *context);

/**
 * This info is passed from application to OC Stack when initiating a request to Server.
 */
typedef struct OCCallbackData
{
    /** Pointer to the context.*/
    void *context;

    /** The pointer to a function the stack will call to handle the requests.*/
    OCClientResponseHandler cb;

    /** A pointer to a function to delete the context when this callback is removed.*/
    OCClientContextDeleter cd;

#ifdef __cplusplus
    OCCallbackData() = default;
    OCCallbackData(void* ctx, OCClientResponseHandler callback, OCClientContextDeleter deleter)
        :context(ctx), cb(callback), cd(deleter){}
#endif // __cplusplus
} OCCallbackData;
#endif	/* INTERFACE */

/**
 * Forward declaration of resource type.
 */
typedef struct resourcetype_t OCResourceType;

/**
 * Data structure for holding client's callback context, methods and Time to Live,
 * connectivity Types, presence and resource type, request uri etc.
 */
#if EXPORT_INTERFACE
typedef struct ClientCB {
    /** callback method defined in application address space. */
    OCClientResponseHandler callBack; /* Always set errno = 0 before calling!  */

    /** callback context data. */
    void * context;

    /** callback method to delete context data. */
    OCClientContextDeleter deleteCallback;

    /** Qos for the request */
    CAMessageType_t type;

    /**  when a response is recvd with this token, above callback will be invoked. */
    CAToken_t token;

    /** a response is recvd with this token length.*/
    uint8_t tokenLength;

    CAHeaderOption_t *options;

    uint8_t numOptions;

    CAPayload_t payload;

    size_t payloadSize;

    CAPayloadFormat_t payloadFormat;

    /** Invocation handle tied to original call to OCDoResource().*/
    OCDoHandle handle;

    /** This is used to determine if all responses should be consumed or not.
     * (For now, only pertains to OC_REST_OBSERVE_ALL vs. OC_REST_OBSERVE functionality).*/
    OCMethod method;

    /** This is the sequence identifier the server applies to the invocation tied to 'handle'.*/
    uint32_t sequenceNumber;

    /** The canonical form of the request uri associated with the call back.*/
    char * requestUri;

    /** Remote address complete.*/
    OCDevAddr * devAddr;

#ifdef WITH_PRESENCE
    /** Struct to hold TTL info for presence.*/
    OCPresence * presence;

    /** Struct to hold a resource type name for filtering a presence interesting.*/
    OCResourceType * interestingPresenceResourceType;
#endif

    /** The connectivity type on which the request was sent on.*/
    OCConnectivityType conType;

    /** The TTL for this callback. Holds the time till when this callback can
     * still be used. TTL is set to 0 when the callback is for presence and observe.
     * Presence has ttl mechanism in the "presence" member of this struct and observes
     * can be explicitly cancelled.*/
    uint32_t TTL;

    /** next node in this list.*/
    struct ClientCB    *next;
} ClientCB;
#endif	/* INTERFACE */

//TODO: Now g_cbList is defined as extern variable since ocstack needs it to process presence.
//      This should be static variable after we make a presence feature separately.
//TODO: Now ocstack is directly accessing the clientCB list to process presence.
//      It should be avoided after we make a presence feature separately.
/**
 * Linked list of ClientCB node.
 */
struct ClientCB *g_cbList = NULL;

//-------------------------------------------------------------------------------------------------
// Local functions
//-------------------------------------------------------------------------------------------------
static void DeleteClientCBInternal(ClientCB * cbNode)
{
    assert(cbNode);

    OIC_TRACE_BEGIN(%s:DeleteClientCB, TAG);
    OIC_LOG(INFO, TAG, "Deleting token");
    OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)cbNode->token, cbNode->tokenLength);
    OIC_TRACE_BUFFER("OIC_RI_CLIENTCB:DeleteClientCB:token:",
                     (const uint8_t *)cbNode->token, cbNode->tokenLength);

    LL_DELETE(g_cbList, cbNode);
    CADestroyToken(cbNode->token);
    OICFree(cbNode->devAddr);
    OICFree(cbNode->handle);
    if (cbNode->requestUri)
    {
        OIC_LOG_V(INFO, TAG, "Deleting callback with uri %s", cbNode->requestUri);
        OIC_TRACE_MARK(%s:DeleteClientCB:uri:%s, TAG, cbNode->requestUri);
        OICFree(cbNode->requestUri);
    }
    if (cbNode->options)
    {
        OICFree(cbNode->options);
    }
    if(cbNode->payload)
    {
        OICFree(cbNode->payload);
    }
#ifdef WITH_PRESENCE
    if (cbNode->presence)
    {
        OICFree(cbNode->presence->timeOut);
        OICFree(cbNode->presence);
    }
    if (cbNode->method == OC_REST_PRESENCE)
    {
        OCResourceType * pointer = cbNode->interestingPresenceResourceType;
        OCResourceType * next = NULL;
        while (pointer)
        {
            next = pointer->next;
            OICFree(pointer->resourcetypename);
            OICFree(pointer);
            pointer = next;
        }
    }
#endif // WITH_PRESENCE

    if (cbNode->deleteCallback)
    {
        cbNode->deleteCallback(cbNode->context);
    }

    OICFree(cbNode);
    cbNode = NULL;

    OIC_TRACE_END();
}

/*
 * This function checks if the node is past its time to live and
 * deletes it if timed-out. Calling this function with a  presence or observe
 * callback with ttl set to 0 will not delete anything as presence nodes have
 * their own mechanisms for timeouts. A null argument will cause the function to
 * silently return.
 */
static void CheckAndDeleteTimedOutCB(ClientCB * cbNode)
{
    assert(cbNode);

    if (cbNode->TTL == 0)
    {
        return;
    }
    coap_tick_t now;
    coap_ticks(&now);

    if (cbNode->TTL < now)
    {
        OIC_LOG(INFO, TAG, "Deleting timed-out callback");
        DeleteClientCBInternal(cbNode);
    }
}

#ifdef WITH_PRESENCE
/**
 * Inserts a new resource type filter into this cb node.
 */
static OCStackResult InsertResourceTypeFilter(ClientCB * cbNode,
                                              char * resourceTypeName)
{
    assert(cbNode);
    assert(resourceTypeName);

    // Form a new resourceType member.
    OCResourceType * newResourceType = (OCResourceType *) OICMalloc(sizeof(OCResourceType));
    if (!newResourceType)
    {
        OIC_LOG(ERROR, TAG, "Failed to OICMalloc to newResourceType");
        return OC_STACK_NO_MEMORY;
    }

    newResourceType->next = NULL;
    newResourceType->resourcetypename = resourceTypeName;

    LL_APPEND(cbNode->interestingPresenceResourceType, newResourceType);
    return OC_STACK_OK;
}
#endif // WITH_PRESENCE

//-------------------------------------------------------------------------------------------------
// Internal APIs
//-------------------------------------------------------------------------------------------------
OCStackResult AddClientCB(ClientCB** clientCB, OCCallbackData* cbData,
                          CAMessageType_t type,
                          CAToken_t token, uint8_t tokenLength,
                          CAHeaderOption_t *options, uint8_t numOptions,
                          CAPayload_t payload, size_t payloadSize,
                          CAPayloadFormat_t payloadFormat,
                          OCDoHandle *handle, OCMethod method,
                          OCDevAddr *devAddr, char *requestUri,
                          char *resourceTypeName, uint32_t ttl)
{
    if (!clientCB || !cbData || !handle || tokenLength > CA_MAX_TOKEN_LEN)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OIC_TRACE_BEGIN(%s:AddClientCB, TAG);

    ClientCB *cbNode = NULL;

#ifdef WITH_PRESENCE
    if (method == OC_REST_PRESENCE)
    {   // Retrieve the presence callback structure for this specific requestUri.
        cbNode = GetClientCBUsingUri(requestUri);
    }

    if (!cbNode)// If it does not already exist, create new node.
#endif // WITH_PRESENCE
    {
        cbNode = (ClientCB*) OICMalloc(sizeof(ClientCB));
        if (!cbNode)
        {
            *clientCB = NULL;
            goto exit;
        }

        OIC_LOG(INFO, TAG, "Adding client callback with token");
        OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)token, tokenLength);
        OIC_TRACE_BUFFER("OIC_RI_CLIENTCB:AddClientCB:token:",
                         (const uint8_t *)token, tokenLength);
        cbNode->callBack = cbData->cb;
        cbNode->context = cbData->context;
        cbNode->deleteCallback = cbData->cd;

        if (!options || !numOptions)
        {
            OIC_LOG (INFO, TAG, "No options present");
            cbNode->options = NULL;
            cbNode->numOptions = 0;
        }
        else
        {
            cbNode->options = (CAHeaderOption_t *) OICCalloc(numOptions, sizeof(CAHeaderOption_t));
            if (!cbNode->options)
            {
                OIC_LOG(ERROR, TAG, "Out of memory");
                OICFree(cbNode);
                return OC_STACK_NO_MEMORY;
            }
            memcpy(cbNode->options, options, sizeof(CAHeaderOption_t) * numOptions);
            cbNode->numOptions = numOptions;
        }

        if (!payload || !payloadSize)
        {
            OIC_LOG (INFO, TAG, "No payload present");
            cbNode->payload = NULL;
            cbNode->payloadSize = 0;
        }
        else
        {
            cbNode->payload = (CAPayload_t) OICCalloc(1, payloadSize);
            if (!cbNode->payload)
            {
                OIC_LOG(ERROR, TAG, "Out of memory");
                if (cbNode->options)
                {
                    OICFree(cbNode->options);
                }
                OICFree(cbNode);
                return OC_STACK_NO_MEMORY;
            }
            memcpy(cbNode->payload, payload, payloadSize);
            cbNode->payloadSize = payloadSize;
        }

        cbNode->payloadFormat = payloadFormat;
        cbNode->type = type;
        //Note: token memory is allocated in the caller OCDoResource
        //but freed in DeleteClientCB
        cbNode->token = token;
        cbNode->tokenLength = tokenLength;
        cbNode->handle = *handle;
        cbNode->method = method;
        cbNode->sequenceNumber = 0;
#ifdef WITH_PRESENCE
        cbNode->presence = NULL;
        cbNode->interestingPresenceResourceType = NULL;
#endif // WITH_PRESENCE

        if (method == OC_REST_PRESENCE ||
            method == OC_REST_OBSERVE  ||
            method == OC_REST_OBSERVE_ALL)
        {
            cbNode->TTL = 0;
        }
        else
        {
            cbNode->TTL = ttl;
        }
        cbNode->requestUri = requestUri;    // I own it now
        cbNode->devAddr = devAddr;          // I own it now
        OIC_LOG_V(INFO, TAG, "Added Callback for uri : %s", requestUri);
        OIC_TRACE_MARK(%s:AddClientCB:uri:%s, TAG, requestUri);
        LL_APPEND(g_cbList, cbNode);
        *clientCB = cbNode;
    }
#ifdef WITH_PRESENCE
    else
    {
        // Ensure that the handle the SDK hands back up to the application layer for the
        // OCDoResource call matches the found ClientCB Node.
        *clientCB = cbNode;

        if (cbData->cd)
        {
            cbData->cd(cbData->context);
        }

        CADestroyToken(token);
        OICFree(*handle);
        OICFree(requestUri);
        OICFree(devAddr);
        *handle = cbNode->handle;
    }

    if (method == OC_REST_PRESENCE && resourceTypeName)
    {
        OIC_TRACE_END();
        // Amend the found or created node by adding a new resourceType to it.
        return InsertResourceTypeFilter(cbNode, (char *)resourceTypeName);
        // I own resourceTypName now.
    }
    else
    {
        OICFree(resourceTypeName);
    }
#else
    OICFree(resourceTypeName);
#endif

    OIC_TRACE_END();
    return OC_STACK_OK;

exit:
    OIC_TRACE_END();
    return OC_STACK_NO_MEMORY;
}

void DeleteClientCB(ClientCB * cbNode)
{
    OIC_LOG_V(INFO, __FILE__, "[%d] %s: ENTRY", __LINE__, __func__);
    if (cbNode)
    {
        ClientCB* out = NULL;
        ClientCB* tmp = NULL;
        LL_FOREACH_SAFE(g_cbList, out, tmp)
        {
            if (cbNode == out)
            {
                DeleteClientCBInternal(out);
                break;
            }
        }
    }
    OIC_LOG_V(INFO, __FILE__, "[%d] %s: EXIT", __LINE__, __func__);
}

void DeleteClientCBList()
{
    ClientCB* out = NULL;
    ClientCB* tmp = NULL;
    LL_FOREACH_SAFE(g_cbList, out, tmp)
    {
        DeleteClientCBInternal(out);
    }
    g_cbList = NULL;
}

ClientCB* GetClientCBUsingToken(const CAToken_t token,
                                const uint8_t tokenLength)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    if (!token || tokenLength > CA_MAX_TOKEN_LEN || tokenLength <= 0)
    {
        OIC_LOG(ERROR, TAG, "Invalid parameter");
        return NULL;
    }

    OIC_LOG (INFO, TAG, "Looking for token");
    OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)token, tokenLength);

    ClientCB* out = NULL;
    ClientCB* tmp = NULL;
    LL_FOREACH_SAFE(g_cbList, out, tmp)
    {
        /* de-annotate below line if want to see all token in g_cbList */
        //OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)out->token, tokenLength);
        if (memcmp(out->token, token, tokenLength) == 0)
        {
            OIC_LOG(INFO, TAG, "Found in callback list");
            return out;
        }
        CheckAndDeleteTimedOutCB(out);
    }

    OIC_LOG(INFO, TAG, "Callback Not found!");
    return NULL;
}

ClientCB* GetClientCBUsingHandle(const OCDoHandle handle)
{
    if (!handle)
    {
        OIC_LOG(ERROR, TAG, "Invalid parameter");
        return NULL;
    }

    OIC_LOG(INFO, TAG,  "Looking for handle");

    ClientCB* out = NULL;
    ClientCB* tmp = NULL;
    LL_FOREACH_SAFE(g_cbList, out, tmp)
    {
        if (out->handle == handle)
        {
            OIC_LOG(INFO, TAG, "Found in callback list");
            return out;
        }
        CheckAndDeleteTimedOutCB(out);
    }

    OIC_LOG(INFO, TAG, "Callback Not found!");
    return NULL;
}

#ifdef WITH_PRESENCE		/* GAR: why? */
ClientCB* GetClientCBUsingUri(const char *requestUri)
{
    if (!requestUri)
    {
        OIC_LOG(ERROR, TAG, "Invalid parameter");
        return NULL;
    }

    OIC_LOG_V(INFO, TAG, "Looking for uri %s", requestUri);

    ClientCB* out = NULL;
    ClientCB* tmp = NULL;
    LL_FOREACH_SAFE(g_cbList, out, tmp)
    {
        /* de-annotate below line if want to see all uri in g_cbList */
        //OIC_LOG_V(INFO, TAG, "%s", out->requestUri);
        if (out->requestUri && strcmp(out->requestUri, requestUri ) == 0)
        {
            OIC_LOG(INFO, TAG, "Found in callback list");
            return out;
        }
        CheckAndDeleteTimedOutCB(out);
    }

    OIC_LOG(INFO, TAG, "Callback Not found!");
    return NULL;
}
#endif // WITH_PRESENCE
