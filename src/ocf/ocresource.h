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

/**
 * @file
 *
 * This file contains the definition, types and interfaces for resource and attributes
 *
 */


#ifndef OCRESOURCE_H_
#define OCRESOURCE_H_

#include "ocstackconfig.h"
#include "occlientcb.h"
#include "ocobserve.h"

/** Macro Definitions for observers */

/** Observer not interested. */
#define OC_OBSERVER_NOT_INTERESTED       (0)

/** Observer still interested. */
#define OC_OBSERVER_STILL_INTERESTED     (1)

/** Failed communication. */
#define OC_OBSERVER_FAILED_COMM          (2)

/**
 *Virtual Resource Presence Attributes
 */

#ifdef WITH_PRESENCE
typedef struct PRESENCERESOURCE{
    OCResourceHandle handle;
    uint32_t presenceTTL;
} PresenceResource;
#endif

/** Introspection URI.*/
#define OC_RSRVD_INTROSPECTION_URI_PATH            "/introspection"

/** Introspection payload URI.*/
#define OC_RSRVD_INTROSPECTION_PAYLOAD_URI_PATH    "/introspection/payload"

/**
 * Forward declarations
 */

struct rsrc_t;

/**
 * following structure will be created in occollection.
 */

typedef struct occapability {
    /** Linked list; for multiple capabilities.*/
    struct occapability* next;

    /** It is a name about resource capability. */
    char *capability;

    /** It is mean status of capability. */
    char *status;
} OCCapability;

/**
 * following structure will be created in occollection.
 */

typedef struct ocaction {
    /** linked list; for multiple actions. */
    struct ocaction *next;

    /** Target Uri. It will be used to execute the action. */
    char *resourceUri;

    /** head pointer of a linked list of capability nodes.*/
    OCCapability* head;
} OCAction;

/**
 * following structure will be created in occollection.
 */

typedef struct ocactionset
{
    /** linked list; for list of action set. */
    struct ocactionset *next;

    /** Name of the action set.*/
    char *actionsetName;

    /** Time stamp.*/
    long int timesteps;

    /** Type of action.*/
    unsigned int type;

    /** head pointer of a linked list of Actions.*/
    OCAction* head;
} OCActionSet;

/**
 * Data structure for holding name and data types for each OIC resource.
 */
typedef struct resourcetype_t {

    /** linked list; for multiple types on resource. */
    struct resourcetype_t *next;

    /**
     * Name of the type; this string is ‘.’ (dot) separate list of segments where each segment is a
     * namespace and the final segment is the type; type and sub-types can be separate with
     * ‘-‘ (dash) usually only two segments would be defined. Either way this string is meant to be
     * human friendly and is used opaquely and not parsed by code. This name is used in the “rt=”
     * parameter of a resource description when resources are introspected and is also use in the
     * " <base URI>/types " list of available types.
    */
    char *resourcetypename;
} OCResourceType;

/**
 * Data structure for data type and definition for attributes that the resource exposes.
 */
typedef struct attr_t {

    /** Points to next resource in list.*/
    struct attr_t *next;

    /** The name of the attribute; used to look up the attribute in list.
     *  for a given attribute SHOULD not be changed once assigned.
     */
    char *attrName;

    /** value of the attribute as void. To support both string and ::OCStringLL value*/
    void *attrValue;
} OCAttribute;

/**
 * Data structure for holding a resource interface
 */
typedef struct resourceinterface_t {

    /** linked list; for multiple interfaces on resource.*/
    struct resourceinterface_t *next;

    /** Name of the interface; this is ‘.’ (dot) separate list of segments where each segment is a
     * namespace and the final segment is the interface; usually only two segments would be
     * defined. Either way this string is opaque and not parsed by segment.*/
    char *name ;

    /** Supported content types to serialize request and response on this interface
     * (REMOVE for V1 – only jSON for all but core.ll that uses Link Format)*/
#if 0
    char *inputContentType ;
    char *outputContentType ;
#endif
    /** Future placeholder for access control and policy.*/
} OCResourceInterface;

/**
 * Data structure for holding child resources associated with a collection
 */
typedef struct OCChildResource {
    struct OCResource *rsrcResource;
    struct OCChildResource *next;
} OCChildResource;

/**
 * Data structure for holding data type and definition for OIC resource.
 */
typedef struct OCResource {

    /** Points to next resource in list.*/
    struct OCResource *next;

    /** Relative path on the device; will be combined with base url to create fully qualified path.*/
    char *uri;

    /** Resource type(s); linked list.*/
    OCResourceType *rsrcType;

    /** Resource interface(s); linked list.*/
    OCResourceInterface *rsrcInterface;

    /** Resource attributes; linked list.*/
    OCAttribute *rsrcAttributes;

    /** Array of pointers to resources; can be used to represent a container of resources.
     * (i.e. hierarchies of resources) or for reference resources (i.e. for a resource collection).*/

    /** Child resource(s); linked list.*/
    OCChildResource *rsrcChildResourcesHead;

    /** Pointer to function that handles the entity bound to the resource.
     *  This handler has to be explicitly defined by the programmer.*/
    OCEntityHandler entityHandler;

    /** Callback parameter.*/
    void * entityHandlerCallbackParam;

    /** Properties on the resource – defines meta information on the resource.
     * (ACTIVE, DISCOVERABLE etc ). */

    OCResourceProperty resourceProperties ;

    /* @note: Methods supported by this resource should be based on the interface targeted
     * i.e. look into the interface structure based on the query request Can be removed here;
     * place holder for the note above.*/
    /* method_t methods; */

    /** Observer(s); linked list.*/
    ResourceObserver *observersHead;

    /** Sequence number for observable resources. Per the CoAP standard it is a 24 bit value.*/
    uint32_t sequenceNum;

    /** Pointer of ActionSet which to support group action.*/
    OCActionSet *actionsetHead;

    /** The instance identifier for this web link in an array of web links - used in links. */
    union
    {
        /** An ordinal number that is not repeated - must be unique in the collection context. */
        int64_t ins;
        /** Any unique string including a URI. */
        char *uniqueStr;
        /** Use UUID for universal uniqueness - used in /oic/res to identify the device. */
        OCIdentity uniqueUUID;
    };

    /** Resource endpoint type(s). */
    OCTpsSchemeFlags endpointType;
} OCResource;

/**
 * Checks if an observation Id already exists among the resources.
 *
 * @param[in]  observationId        Observation Id to check.
 *
 * @return true if it exists, false if not.
 */
bool IsObservationIdExisting(const OCObservationId observationId);

/**
 * Search the list of resource for an observer that has the specified token.
 *
 * @param[out] outResource          Resource pointer that the observer belong to.
 * @param[out] outObserver          Observer pointer that is associated with the token.
 * @param[in]  token                Token to search for.
 * @param[in]  tokenLength          Length of token.
 *
 * @return true if the resource and the observer are found, false if not.
 */
bool GetObserverFromResourceList(OCResource **outResource, ResourceObserver **outObserver,
                                 const CAToken_t token, uint8_t tokenLength);

/**
 * Give a stack feedback so that entityHandler gets called with an observe de-register option
 * and it will delete all observers associated with the specified device's address.
 *
 * @param[in]  devAddr              Device's address.
 */
void GiveStackFeedBackObserverNotInterested(const OCDevAddr *devAddr);

#endif /* OCRESOURCE_H_ */
