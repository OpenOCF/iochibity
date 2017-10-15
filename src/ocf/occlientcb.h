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

/**
 * @file
 *
 * This file contains the definition and types for client's callback mode and functions.
 */


#ifndef OC_CLIENT_CB_H_
#define OC_CLIENT_CB_H_

#include "ocstack.h"
#include "ocresource.h"
#include "ocpresence.h"

#include "cacommon.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Forward declaration of resource type.
 */
typedef struct resourcetype_t OCResourceType;

/**
 * Data structure for holding client's callback context, methods and Time to Live,
 * connectivity Types, presence and resource type, request uri etc.
 */
typedef struct ClientCB {
    /** callback method defined in application address space. */
    OCClientResponseHandler callBack;

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

//TODO: Now ocstack is directly accessing the clientCB list to process presence.
//      It should be avoided after we make a presence feature separately.
/**
 * Linked list of ClientCB node.
 */
extern struct ClientCB *g_cbList;

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // OC_CLIENT_CB_H_

