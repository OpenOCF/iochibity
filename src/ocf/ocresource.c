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

/*
 * Expose POSIX.1-2008 base specification,
 * Refer http://pubs.opengroup.org/onlinepubs/9699919799/
 */
#define _POSIX_C_SOURCE 200809L
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "ocresource.h"

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif


/* #include "coap/coap.h" */
#include "cbor.h"

#include "utlist.h"

/// Module Name
#define TAG "OIC_RI_RESOURCE"

/**
 * Common JSON string components used by the stack to build JSON strings.
 * These details are exposed in ocstackconfig.h file in the form of documentation.
 * Remember to update the documentation there if these are changed.
 */
#define OC_JSON_PREFIX                     "{\"oic\":["
#define OC_JSON_PREFIX_LEN                 (sizeof(OC_JSON_PREFIX) - 1)
#define OC_JSON_SUFFIX                     "]}"
#define OC_JSON_SUFFIX_LEN                 (sizeof(OC_JSON_SUFFIX) - 1)
#define OC_JSON_SEPARATOR                  ','
#define OC_JSON_SEPARATOR_STR              ","

/**
 * Static values for various JSON attributes.
 */
#define OC_RESOURCE_OBSERVABLE   1
#define OC_RESOURCE_SECURE       1

/**
 *  OIC Virtual resources supported by every OIC device.
 */
#if EXPORT_INTERFACE
typedef enum
{
    /** unknown URI.*/
    OC_UNKNOWN_URI =0,

    /** "/oic/res".*/
    OC_WELL_KNOWN_URI,

    /** "/oic/d" .*/
    OC_DEVICE_URI,

    /** "/oic/p" .*/
    OC_PLATFORM_URI,

    /** "/oic/res/d/type" .*/
    OC_RESOURCE_TYPES_URI,
#ifdef ROUTING_GATEWAY
    /** "/oic/gateway" .*/
    OC_GATEWAY_URI,
#endif
#ifdef WITH_PRESENCE
    /** "/oic/ad" .*/
    OC_PRESENCE,
#endif

#ifdef MQ_BROKER
    /** "/oic/ps" .*/
    OC_MQ_BROKER_URI,
#endif

#ifdef TCP_ADAPTER
    /** "/oic/ping" .*/
    OC_KEEPALIVE_RESOURCE_URI,
#endif

    /** "/oic/introspection" .*/
    OC_INTROSPECTION_URI,

    /** "/oic/introspection/payload" .*/
    OC_INTROSPECTION_PAYLOAD_URI,

    /** Max items in the list */
    OC_MAX_VIRTUAL_RESOURCES    //<s Max items in the list

} OCVirtualResources;

/**
 * The type of handling required to handle a request.
 */
typedef enum
{
    OC_RESOURCE_VIRTUAL = 0,
    OC_RESOURCE_NOT_COLLECTION_WITH_ENTITYHANDLER,
    OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER,
    OC_RESOURCE_COLLECTION_WITH_ENTITYHANDLER,
    OC_RESOURCE_COLLECTION_DEFAULT_ENTITYHANDLER,
    OC_RESOURCE_DEFAULT_DEVICE_ENTITYHANDLER,
    OC_RESOURCE_NOT_SPECIFIED
} ResourceHandling;
#endif	/* INTERFACE */

#if EXPORT_INTERFACE

/** Macro Definitions for observers */

/** Observer not interested. */
#define OC_OBSERVER_NOT_INTERESTED       (0)

/** Observer still interested. */
#define OC_OBSERVER_STILL_INTERESTED     (1)

/** Failed communication. */
#define OC_OBSERVER_FAILED_COMM          (2)

/** Introspection URI.*/
#define OC_RSRVD_INTROSPECTION_URI_PATH            "/introspection"

/** Introspection payload URI.*/
#define OC_RSRVD_INTROSPECTION_PAYLOAD_URI_PATH    "/introspection/payload"

/**
 * Forward declarations
 */

/* struct rsrc_t; */

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
typedef struct OCAttribute /* was: attr_t */ {

    /** Points to next resource in list.*/
    struct OCAttribute *next;

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
    OCAttribute *rsrcAttributes; /* GAR:misnamed, should be Properties */

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

    OCResourceProperty resourceProperties ; /* GAR: misnamed; should be policies, not properties */

    /* @note: Methods supported by this resource should be based on the interface targeted
     * i.e. look into the interface structure based on the query request Can be removed here;
     * place holder for the note above.*/
    /* method_t methods; */

    /** Observer(s); linked list.*/
    ResourceObserver *observersHead; /* GAR: misnamed, should be ResourceWatcher watchersHead; */

    /** Sequence number for observable resources. Per the CoAP standard it is a 24 bit value.*/
    uint32_t sequenceNum;

    /** Pointer of ActionSet which to support group action.*/
    OCActionSet *actionsetHead;

    /** The instance identifier for this web link in an array of web links - used in links. */
    /* GAR: uniqueStr, OCIdentity, uniqueUUID not referenced in source code!? */
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
 * Resource Properties.
 * The value of a policy property is defined as bitmap.
 * The LSB represents OC_DISCOVERABLE and Second LSB bit represents OC_OBSERVABLE and so on.
 * Not including the policy property is equivalent to zero.
 *
 */
typedef enum
{
    /** When none of the bits are set, the resource is non-secure, non-discoverable &
     *  non-observable by the client.*/
    OC_RES_PROP_NONE = (0),

    /** When this bit is set, the resource is allowed to be discovered by clients.*/
    OC_DISCOVERABLE  = (1 << 0),

    /** When this bit is set, the resource is allowed to be observed by clients.*/
    OC_OBSERVABLE    = (1 << 1),

    /** When this bit is set, the resource is initialized, otherwise the resource
     *  is 'inactive'. 'inactive' signifies that the resource has been marked for
     *  deletion or is already deleted.*/
    OC_ACTIVE        = (1 << 2),

    /** When this bit is set, the resource has been marked as 'slow'.
     * 'slow' signifies that responses from this resource can expect delays in
     *  processing its requests from clients.*/
    OC_SLOW          = (1 << 3),

    /** When this bit is set, the resource supports access via non-secure endpoints. */
    OC_NONSECURE     = (1 << 6),

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
    /** When this bit is set, the resource is a secure resource.*/ /* GAR: ie. supports access via secure endpoint */
    OC_SECURE        = (1 << 4),
#else
    OC_SECURE        = (0),
#endif

    /** When this bit is set, the resource is allowed to be discovered only
     *  if discovery request contains an explicit querystring.
     *  Ex: GET /oic/res?rt=oic.sec.acl */
    OC_EXPLICIT_DISCOVERABLE   = (1 << 5)

#ifdef WITH_MQ
    /** When this bit is set, the resource is allowed to be published */
    // @todo
    // Since this property is not defined on OCF Spec. it should be set 0 until define it
    ,OC_MQ_PUBLISHER     = (0)
#endif

#ifdef MQ_BROKER
    /** When this bit is set, the resource is allowed to be notified as MQ broker.*/
    // @todo
    // Since this property is not defined on OCF Spec. it should be set 0 until define it
    ,OC_MQ_BROKER        = (0)
#endif
} OCResourceProperty; /* GAR: misnamed, should be OCResourcePolicy */

#endif	/* EXPORT_INTERFACE */

// Using 1k as block size since most persistent storage implementations use a power of 2.
#define INTROSPECTION_FILE_SIZE_BLOCK  1024

/* #define VERIFY_SUCCESS(op) { if (op != (OC_STACK_OK)) \
 *             {OIC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} } */

/**
 * Default cbor payload size. This value is increased in case of CborErrorOutOfMemory.
 * The value of payload size is increased up to CBOR_MAX_SIZE.
 */
static const uint16_t CBOR_SIZE = 512;

/**
 * Max cbor size payload.
 */
static const uint16_t CBOR_MAX_SIZE = 4400;

extern OCResource *headResource;
extern bool g_multicastServerStopped;

//-----------------------------------------------------------------------------
// Default resource entity handler function
//-----------------------------------------------------------------------------
OCEntityHandlerResult defaultResourceEHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest * request, void* callbackParam)
{
    //TODO ("Implement me!!!!");
    // TODO:  remove silence unused param warnings
    (void) flag;
    (void) request;
    (void) callbackParam;
    return  OC_EH_OK; // Making sure that the Default EH and the Vendor EH have matching signatures
}

/* This method will retrieve the port at which the secure resource is hosted */
static OCStackResult GetSecurePortInfo(OCDevAddr *endpoint, uint16_t *port)
{
    uint16_t p = 0;

    if (endpoint->adapter == OC_ADAPTER_IP)
    {
        if (endpoint->flags & OC_IP_USE_V6)
        {
            p = caglobals.ip.u6s.port;
        }
        else if (endpoint->flags & OC_IP_USE_V4)
        {
            p = caglobals.ip.u4s.port;
        }
    }

    *port = p;
    return OC_STACK_OK;
}

#ifdef TCP_ADAPTER
/* This method will retrieve the tcp port */
OCStackResult GetTCPPortInfo(OCDevAddr *endpoint, uint16_t *port, bool secured)
{
    if (NULL == endpoint)
    {
        OIC_LOG(ERROR, TAG, "GetTCPPortInfo failed!");
        return OC_STACK_ERROR;
    }

    uint16_t p = 0;
    if (endpoint->adapter == OC_ADAPTER_IP)
    {
        if (endpoint->flags & OC_IP_USE_V4)
        {
            p = secured ? caglobals.tcp.ipv4s.port : caglobals.tcp.ipv4.port;
        }
        else if (endpoint->flags & OC_IP_USE_V6)
        {
            p = secured ? caglobals.tcp.ipv6s.port : caglobals.tcp.ipv6.port;
        }
    }

    *port = p;
    return OC_STACK_OK;
}
#endif

/*
 * Function will extract 0, 1 or 2 filters from query.
 * More than 2 filters or unsupported filters will result in error.
 * If both filters are of the same supported type, the 2nd one will be picked.
 * Resource and device filters in the SAME query are NOT validated
 * and resources will likely not clear filters.
 */
/**
 * Extract interface and resource type from the query.
 *
 * @param query is the request received from the client
 * @param filterOne will include result if the interface is included in the query.
 * @param filterTwo will include result if the resource type is included in the query.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure
 */
OCStackResult ExtractFiltersFromQuery(const char *query, char **filterOne, char **filterTwo)
{
    if (!query)
    {
        OIC_LOG(ERROR, TAG, "Query is empty!");
        return OC_STACK_INVALID_QUERY;
    }
    char *key = NULL;
    char *value = NULL;
    char *queryDup = NULL;
    char *restOfQuery = NULL;
    char *keyValuePair = NULL;
    int numKeyValuePairsParsed = 0;

    *filterOne = NULL;
    *filterTwo = NULL;

    queryDup = OICStrdup(query);
    if (NULL == queryDup)
    {
        OIC_LOG(ERROR, TAG, "Creating duplicate string failed!");
        return OC_STACK_NO_MEMORY;
    }

    OIC_LOG_V(INFO, TAG, "Extracting params from %s", queryDup);

    OCStackResult eCode = OC_STACK_INVALID_QUERY;
    if (strnlen(queryDup, MAX_QUERY_LENGTH) >= MAX_QUERY_LENGTH)
    {
        OIC_LOG(ERROR, TAG, "Query exceeds maximum length.");
        goto exit;
    }

    keyValuePair = strtok_r (queryDup, OC_QUERY_SEPARATOR, &restOfQuery);

    while(keyValuePair)
    {
        if (numKeyValuePairsParsed >= 2)
        {
            OIC_LOG(ERROR, TAG, "More than 2 queries params in URI.");
            goto exit;
        }

        key = strtok_r(keyValuePair, OC_KEY_VALUE_DELIMITER, &value);

        if (!key || !value)
        {
            goto exit;
        }
        else if (strncasecmp(key, OC_RSRVD_INTERFACE, sizeof(OC_RSRVD_INTERFACE) - 1) == 0)
        {
            *filterOne = value;     // if
        }
        else if (strncasecmp(key, OC_RSRVD_RESOURCE_TYPE, sizeof(OC_RSRVD_RESOURCE_TYPE) - 1) == 0)
        {
            *filterTwo = value;     // rt
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "Unsupported query key: %s", key);
            goto exit;
        }
        ++numKeyValuePairsParsed;

        keyValuePair = strtok_r(NULL, OC_QUERY_SEPARATOR, &restOfQuery);
    }

    if (*filterOne)
    {
        *filterOne = OICStrdup(*filterOne);
        if (NULL == *filterOne)
        {
            OIC_LOG(ERROR, TAG, "Creating duplicate string failed!");
            eCode = OC_STACK_NO_MEMORY;
            goto exit;
        }
    }

    if (*filterTwo)
    {
        *filterTwo = OICStrdup(*filterTwo);
        if (NULL == *filterTwo)
        {
            OIC_LOG(ERROR, TAG, "Creating duplicate string failed!");
            OICFree(*filterOne);
            eCode = OC_STACK_NO_MEMORY;
            goto exit;
        }
    }

    OICFree(queryDup);
    OIC_LOG_V(INFO, TAG, "Extracted params if: %s and rt: %s.", *filterOne, *filterTwo);
    return OC_STACK_OK;

exit:
    *filterOne = NULL;
    *filterTwo = NULL;
    OICFree(queryDup);
    return eCode;
}

OCVirtualResources GetTypeOfVirtualURI(const char *uriInRequest)
{
    if (strcmp(uriInRequest, OC_RSRVD_WELL_KNOWN_URI) == 0)
    {
        return OC_WELL_KNOWN_URI;
    }
    else if (strcmp(uriInRequest, OC_RSRVD_DEVICE_URI) == 0)
    {
        return OC_DEVICE_URI;
    }
    else if (strcmp(uriInRequest, OC_RSRVD_PLATFORM_URI) == 0)
    {
        return OC_PLATFORM_URI;
    }
    else if (strcmp(uriInRequest, OC_RSRVD_RESOURCE_TYPES_URI) == 0)
    {
        return OC_RESOURCE_TYPES_URI;
    }
    else if (strcmp(uriInRequest, OC_RSRVD_INTROSPECTION_URI_PATH) == 0)
    {
        return OC_INTROSPECTION_URI;
    }
    else if (strcmp(uriInRequest, OC_RSRVD_INTROSPECTION_PAYLOAD_URI_PATH) == 0)
    {
        return OC_INTROSPECTION_PAYLOAD_URI;
    }
#ifdef ROUTING_GATEWAY
    else if (0 == strcmp(uriInRequest, OC_RSRVD_GATEWAY_URI))
    {
        return OC_GATEWAY_URI;
    }
#endif
#ifdef WITH_PRESENCE
    else if (strcmp(uriInRequest, OC_RSRVD_PRESENCE_URI) == 0)
    {
        return OC_PRESENCE;
    }
#endif //WITH_PRESENCE

#ifdef MQ_BROKER
    else if (0 == strcmp(uriInRequest, OC_RSRVD_WELL_KNOWN_MQ_URI))
    {
        return OC_MQ_BROKER_URI;
    }
#endif //MQ_BROKER

#ifdef TCP_ADAPTER
    else if (strcmp(uriInRequest, OC_RSRVD_KEEPALIVE_URI) == 0)
    {
        return OC_KEEPALIVE_RESOURCE_URI;
    }
#endif

    return OC_UNKNOWN_URI;
}

static OCStackResult getQueryParamsForFiltering (OCVirtualResources uri, char *query,
                                            char **filterOne, char **filterTwo)
{
    if(!filterOne || !filterTwo)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *filterOne = NULL;
    *filterTwo = NULL;

#ifdef WITH_PRESENCE
    if (uri == OC_PRESENCE)
    {
        //Nothing needs to be done, except for pass a OC_PRESENCE query through as OC_STACK_OK.
        OIC_LOG(INFO, TAG, "OC_PRESENCE Request for virtual resource.");
        return OC_STACK_OK;
    }
#endif

    OCStackResult result = OC_STACK_OK;

    if (query && *query)
    {
        result = ExtractFiltersFromQuery(query, filterOne, filterTwo);
    }

    return result;
}

static OCStackResult BuildDevicePlatformPayload(const OCResource *resourcePtr, OCRepPayload** payload,
    bool addDeviceId)
{
    OCRepPayload *tempPayload = OCRepPayloadCreate();

    if (!resourcePtr)
    {
        OCRepPayloadDestroy(tempPayload);
        return OC_STACK_INVALID_PARAM;
    }

    if (!tempPayload)
    {
        return OC_STACK_NO_MEMORY;
    }

    if (addDeviceId)
    {
        const char *deviceId = OCGetServerInstanceIDString();
        if (!deviceId)
        {
            OIC_LOG(ERROR, TAG, "Failed retrieving device id.");
            return OC_STACK_ERROR;
        }
        OCRepPayloadSetPropString(tempPayload, OC_RSRVD_DEVICE_ID, deviceId);
    }

    for (OCResourceType *resType = resourcePtr->rsrcType; resType; resType = resType->next)
    {
        OCRepPayloadAddResourceType(tempPayload, resType->resourcetypename);
    }

    for (OCResourceInterface *resInterface = resourcePtr->rsrcInterface; resInterface;
        resInterface = resInterface->next)
    {
        OCRepPayloadAddInterface(tempPayload, resInterface->name);
    }

    for (OCAttribute *resAttrib = resourcePtr->rsrcAttributes; resAttrib; resAttrib = resAttrib->next)
    {
        if (resAttrib->attrName && resAttrib->attrValue)
        {
            if (0 == strcmp(OC_RSRVD_DATA_MODEL_VERSION, resAttrib->attrName))
            {
                char *dmv = OCCreateString((OCStringLL *)resAttrib->attrValue);
                if (dmv)
                {
                    OCRepPayloadSetPropString(tempPayload, resAttrib->attrName, dmv);
                    OICFree(dmv);
                }
            }
            else if (0 == strcmp(OC_RSRVD_DEVICE_DESCRIPTION, resAttrib->attrName) ||
                    0 == strcmp(OC_RSRVD_DEVICE_MFG_NAME, resAttrib->attrName))
            {
                size_t dim[MAX_REP_ARRAY_DEPTH] = { 0 };
                for (OCStringLL *ll = (OCStringLL *)resAttrib->attrValue; ll && ll->next;
                     ll = ll->next->next)
                {
                    ++dim[0];
                }
                OCRepPayload **items = (OCRepPayload**)OICCalloc(dim[0], sizeof(OCRepPayload*));
                if (items)
                {
                    OCRepPayload **item = items;
                    for (OCStringLL *ll = (OCStringLL *)resAttrib->attrValue; ll && ll->next;
                         ll = ll->next->next)
                    {
                        *item = OCRepPayloadCreate();
                        if (*item)
                        {
                            OCRepPayloadSetPropString(*item, "language", ll->value);
                            OCRepPayloadSetPropString(*item, "value", ll->next->value);
                            ++item;
                        }
                    }
                }
                OCRepPayloadSetPropObjectArrayAsOwner(tempPayload, resAttrib->attrName, items, dim);
            }
            else
            {
                OCRepPayloadSetPropString(tempPayload, resAttrib->attrName, (char *)resAttrib->attrValue);
            }
        }
    }

    if (!*payload)
    {
        *payload = tempPayload;
    }
    else
    {
        OCRepPayloadAppend(*payload, tempPayload);
    }

    return OC_STACK_OK;
}

OCStackResult BuildResponseRepresentation(const OCResource *resourcePtr,
                    OCRepPayload** payload, OCDevAddr *devAddr)
{
    OCRepPayload *tempPayload = OCRepPayloadCreate();

    if (!resourcePtr)
    {
        OCRepPayloadDestroy(tempPayload);
        return OC_STACK_INVALID_PARAM;
    }

    if(!tempPayload)
    {
        return OC_STACK_NO_MEMORY;
    }

    OCRepPayloadSetPropString(tempPayload, OC_RSRVD_HREF, resourcePtr->uri);

    uint8_t numElement = 0;
    if (OC_STACK_OK == OCGetNumberOfResourceTypes((OCResource *)resourcePtr, &numElement))
    {
        size_t rtDim[MAX_REP_ARRAY_DEPTH] = {numElement, 0, 0};
        char **rt = (char **)OICMalloc(sizeof(char *) * numElement);
        for (uint8_t i = 0; i < numElement; ++i)
        {
            const char *value = OCGetResourceTypeName((OCResource *)resourcePtr, i);
            OIC_LOG_V(DEBUG, TAG, "value: %s", value);
            rt[i] = OICStrdup(value);
        }
        OCRepPayloadSetStringArrayAsOwner(tempPayload, OC_RSRVD_RESOURCE_TYPE, rt, rtDim);
    }

    numElement = 0;
    if (OC_STACK_OK == OCGetNumberOfResourceInterfaces((OCResource *)resourcePtr, &numElement))
    {
        size_t ifDim[MAX_REP_ARRAY_DEPTH] = {numElement, 0, 0};
        char **itf = (char **)OICMalloc(sizeof(char *) * numElement);
        for (uint8_t i = 0; i < numElement; ++i)
        {
            const char *value = OCGetResourceInterfaceName((OCResource *)resourcePtr, i);
            OIC_LOG_V(DEBUG, TAG, "value: %s", value);
            itf[i] = OICStrdup(value);
        }

        bool b = OCRepPayloadSetStringArrayAsOwner(tempPayload, OC_RSRVD_INTERFACE, itf, ifDim);

        if (!b)
        {
            for (uint8_t i = 0; i < numElement; i++)
            {
                OICFree(itf[i]);
            }
            OICFree(itf);
        }
    }

    for (OCAttribute *resAttrib = resourcePtr->rsrcAttributes; resAttrib; resAttrib = resAttrib->next)
    {
        if (resAttrib->attrName && resAttrib->attrValue)
        {
            OCRepPayloadSetPropString(tempPayload, resAttrib->attrName, (char *)resAttrib->attrValue);
        }
    }

    OCResourceProperty p = OCGetResourceProperties((OCResourceHandle *)resourcePtr);
    OCRepPayload *policy = OCRepPayloadCreate();
    if (!policy)
    {
        OCPayloadDestroy((OCPayload *)tempPayload);
        return OC_STACK_NO_MEMORY;
    }
    OCRepPayloadSetPropInt(policy, OC_RSRVD_BITMAP, ((p & OC_DISCOVERABLE) | (p & OC_OBSERVABLE)));
    if (p & OC_SECURE)
    {
        OCRepPayloadSetPropBool(policy, OC_RSRVD_SECURE, p & OC_SECURE);
        uint16_t securePort = 0;
        if (GetSecurePortInfo(devAddr, &securePort) != OC_STACK_OK)
        {
            securePort = 0;
        }
        OCRepPayloadSetPropInt(policy, OC_RSRVD_HOSTING_PORT, securePort);
    }
    OCRepPayloadSetPropObjectAsOwner(tempPayload, OC_RSRVD_POLICY, policy);

    if (!*payload)
    {
        *payload = tempPayload;
    }
    else
    {
        OCRepPayloadAppend(*payload, tempPayload);
    }

    return OC_STACK_OK;
}

void CleanUpDeviceProperties(OCDeviceProperties **deviceProperties)
{
    if (!deviceProperties || !(*deviceProperties))
    {
        return;
    }

    OICFreeAndSetToNull((void**)(deviceProperties));
}

static OCStackResult CreateDeviceProperties(const char *piid, OCDeviceProperties **deviceProperties)
{
    OIC_LOG(DEBUG, TAG, "CreateDeviceProperties IN");

    OCStackResult result = OC_STACK_OK;

    if (!piid || !deviceProperties)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *deviceProperties = (OCDeviceProperties*)OICCalloc(1, sizeof(OCDeviceProperties));
    if (*deviceProperties)
    {
        OICStrcpy((*deviceProperties)->protocolIndependentId, UUID_STRING_SIZE, piid);
    }
    else
    {
        result = OC_STACK_NO_MEMORY;
    }

    OIC_LOG(DEBUG, TAG, "CreateDeviceProperties OUT");

    return result;
}

static OCStackResult GenerateDeviceProperties(OCDeviceProperties **deviceProperties)
{
    OCStackResult result = OC_STACK_OK;
    OicUuid_t generatedProtocolIndependentId = {.id = {0}};
    char* protocolIndependentId = NULL;

    if (!deviceProperties)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *deviceProperties = NULL;

    // Generate a UUID for the Protocol Independent ID
    if (OCGenerateUuid(generatedProtocolIndependentId.id))
    {
        protocolIndependentId = (char*)OICCalloc(UUID_STRING_SIZE, sizeof(char));
        if (protocolIndependentId)
        {
            if (!OCConvertUuidToString(generatedProtocolIndependentId.id, protocolIndependentId))
            {
                OIC_LOG(ERROR, TAG, "ConvertUuidToStr failed");
                result = OC_STACK_ERROR;
            }
        }
        else
        {
            result = OC_STACK_NO_MEMORY;
        }
    }
    else
    {
        OIC_LOG(FATAL, TAG, "Generate UUID for Device Properties Protocol Independent ID failed!");
        result = OC_STACK_ERROR;
    }

    if (OC_STACK_OK == result)
    {
        result = CreateDeviceProperties(protocolIndependentId, deviceProperties);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CreateDeviceProperties failed");
        }
    }

    // Clean Up
    OICFreeAndSetToNull((void**)&protocolIndependentId);

    return result;
}

OCStackResult CBORPayloadToDeviceProperties(const uint8_t *payload, size_t size, OCDeviceProperties **deviceProperties)
{
    OCStackResult result = OC_STACK_OK;
    CborError cborResult = CborNoError;
    char* protocolIndependentId = NULL;
    CborParser parser;
    CborValue dpCbor;
    CborValue dpMap;

    if (!payload || (size <= 0) || !deviceProperties)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *deviceProperties = NULL;

    cbor_parser_init(payload, size, 0, &parser, &dpCbor);

    // piid: Protocol Independent ID - Mandatory
    cborResult = cbor_value_map_find_value(&dpCbor, OC_RSRVD_PROTOCOL_INDEPENDENT_ID, &dpMap);
    if ((CborNoError == cborResult) && cbor_value_is_text_string(&dpMap))
    {
        size_t len = 0;

        cborResult = cbor_value_dup_text_string(&dpMap, &protocolIndependentId, &len, NULL);
        if (CborNoError != cborResult)
        {
            OIC_LOG(ERROR, TAG, "Failed to get Protocol Independent Id!");
            result = OC_STACK_ERROR;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Protocol Independent Id is not present or invalid!");
        result = OC_STACK_ERROR;
    }

    if (OC_STACK_OK == result)
    {
        result = CreateDeviceProperties(protocolIndependentId, deviceProperties);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CreateDeviceProperties failed");
        }
    }

    // Clean Up
    OICFreeAndSetToNull((void**)&protocolIndependentId);

    return result;
}

OCStackResult DevicePropertiesToCBORPayload(const OCDeviceProperties *deviceProperties, uint8_t **payload, size_t *size)
{
    OCStackResult result = OC_STACK_OK;
    CborError cborResult = CborNoError;
    uint8_t *cborPayload = NULL;
    size_t cborLen = CBOR_SIZE;
    CborEncoder encoder;
    CborEncoder dpMap;

    if (!deviceProperties || !payload || !size || (*size > CBOR_MAX_SIZE))
    {
        return OC_STACK_INVALID_PARAM;
    }

    // Reset the CBOR length if we need to
    if (*size > 0)
    {
        cborLen = *size;
    }

    *payload = NULL;
    *size = 0;

    cborPayload = (uint8_t*)OICCalloc(1, cborLen);
    if (NULL != cborPayload)
    {
        cbor_encoder_init(&encoder, cborPayload, cborLen, 0);

        // Create the Device Properties encoder map
        cborResult = cbor_encoder_create_map(&encoder, &dpMap, CborIndefiniteLength);
        if (CborNoError != cborResult)
        {
            OIC_LOG(ERROR, TAG, "Failed to create encoder map!");
            result = OC_STACK_ERROR;
        }
    }
    else
    {
        return OC_STACK_NO_MEMORY;
    }

    // Protocol Independent ID - Mandatory
    if (OC_STACK_OK == result)
    {
        cborResult = cbor_encode_text_string(&dpMap, OC_RSRVD_PROTOCOL_INDEPENDENT_ID, strlen(OC_RSRVD_PROTOCOL_INDEPENDENT_ID));
        if (CborNoError == cborResult)
        {
            cborResult = cbor_encode_text_string(&dpMap,
                                                 deviceProperties->protocolIndependentId,
                                                 strlen(deviceProperties->protocolIndependentId));
            if (CborNoError != cborResult)
            {
                OIC_LOG(ERROR, TAG, "Failed to encode protocolIndependentId!");
                result = OC_STACK_ERROR;
            }
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Failed to encode OC_RSRVD_PROTOCOL_INDEPENDENT_ID!");
            result = OC_STACK_ERROR;
        }
    }

    // Encoding is finished
    if (OC_STACK_OK == result)
    {
        cborResult = cbor_encoder_close_container(&encoder, &dpMap);
        if (CborNoError != cborResult)
        {
            OIC_LOG(ERROR, TAG, "Failed to close dpMap container!");
            result = OC_STACK_ERROR;
        }
    }

    if (OC_STACK_OK == result)
    {
        *size = cbor_encoder_get_buffer_size(&encoder, cborPayload);
        *payload = cborPayload;
        cborPayload = NULL;
    }
    else if ((CborErrorOutOfMemory == cborResult) && (cborLen < CBOR_MAX_SIZE))
    {
        OICFreeAndSetToNull((void**)&cborPayload);

        // Realloc and try again
        cborLen += cbor_encoder_get_buffer_size(&encoder, encoder.end);
        result = DevicePropertiesToCBORPayload(deviceProperties, payload, &cborLen);
        if (OC_STACK_OK == result)
        {
            *size = cborLen;
        }
    }
    else
    {
        OICFreeAndSetToNull((void**)&cborPayload);
    }

    return result;
}

static OCStackResult UpdateDeviceInfoResourceWithDeviceProperties(const OCDeviceProperties *deviceProperties, bool updateDatabase)
{
    OCStackResult result = OC_STACK_OK;
    OCResource *resource = NULL;

    if (!deviceProperties)
    {
        return OC_STACK_INVALID_PARAM;
    }

    resource = FindResourceByUri(OC_RSRVD_DEVICE_URI);
    if (resource)
    {
        result = SetAttributeInternal(resource, OC_RSRVD_PROTOCOL_INDEPENDENT_ID, deviceProperties->protocolIndependentId, updateDatabase);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(ERROR, TAG, "OCSetPropertyValue failed to set Protocol Independent ID");
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Resource does not exist.");
        result = OC_STACK_NO_RESOURCE;
    }

    return result;
}

static OCStackResult ReadDevicePropertiesFromDatabase(OCDeviceProperties **deviceProperties)
{
    OIC_LOG_V(INFO, TAG, "%s: ENTRY", __func__);
    uint8_t *data = NULL;
    size_t size = 0;

    /* read device_properties.dat unless overridden by user */
    /* key:  DeviceProperties */
    OCStackResult result = ReadDatabaseFromPS(OC_DEVICE_PROPS_FILE_NAME, OC_JSON_DEVICE_PROPS_NAME, &data, &size);
    if (OC_STACK_OK == result)
    {
        // Read device properties from PS
        result = CBORPayloadToDeviceProperties(data, size, deviceProperties);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(WARNING, TAG, "CBORPayloadToDeviceProperties failed");
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "ReadDatabaseFromPS failed");
    }

    // Clean Up
    OICFreeAndSetToNull((void**)&data);

    return result;
}

static OCStackResult UpdateDevicePropertiesDatabase(const OCDeviceProperties *deviceProperties)
{
    OCStackResult result = OC_STACK_OK;
    uint8_t *payload = NULL;
    size_t size = 0;

    if (!deviceProperties)
    {
        return OC_STACK_INVALID_PARAM;
    }

    // Check to see if persistent storage exists. If it doesn't then
    // we just allow the device properties to exist in memory and
    // it is the application's job to manage them.
    if (!OCGetPersistentStorageHandler())
    {
        OIC_LOG(DEBUG, TAG, "Persistent Storage handler is NULL.");
        return OC_STACK_OK;
    }

    // Convert OCDeviceProperties into CBOR to use for updating Persistent Storage
    result = DevicePropertiesToCBORPayload(deviceProperties, &payload, &size);
    if ((OC_STACK_OK == result) && payload)
    {
        result = UpdateResourceInPS(OC_DEVICE_PROPS_FILE_NAME, OC_JSON_DEVICE_PROPS_NAME, payload, size);
        if (OC_STACK_OK != result)
        {
            OIC_LOG_V(ERROR, TAG, "UpdateResourceInPS failed with %d!", result);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "DevicePropertiesToCBORPayload failed with %d!", result);
    }

    // Clean Up
    OICFreeAndSetToNull((void**)&payload);

    return result;
}

OCStackResult InitializeDeviceProperties()
{
    OIC_LOG(DEBUG, TAG, "InitializeDeviceProperties IN");

    OCStackResult result = OC_STACK_OK;
    OCDeviceProperties *deviceProperties = NULL;
    bool updateDatabase = false;

    // Populate OCDeviceProperties from persistent storage
    result = ReadDevicePropertiesFromDatabase(&deviceProperties);
    if (OC_STACK_OK != result)
    {
        OIC_LOG(ERROR, TAG, "ReadDevicePropertiesFromDatabase failed");
    }

    // If the device properties in persistent storage are corrupted or
    // not available for some reason, a default OCDeviceProperties is created.
    if ((OC_STACK_OK != result) || !deviceProperties)
    {
        result = GenerateDeviceProperties(&deviceProperties);
        if (OC_STACK_OK == result)
        {
            updateDatabase = true;
        }
        else
        {
            OIC_LOG(ERROR, TAG, "GenerateDeviceProperties failed");
        }
    }

    // Set the device properties information on the device info resource. This will
    // also persist OCDeviceProperties so they can be used in the future if they are
    // not already in the database.
    if (OC_STACK_OK == result)
    {
        result = UpdateDeviceInfoResourceWithDeviceProperties(deviceProperties, updateDatabase);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(ERROR, TAG, "UpdateDeviceInfoResourceWithDeviceProperties failed");
        }
    }

    // Clean Up
    CleanUpDeviceProperties(&deviceProperties);

    OIC_LOG(DEBUG, TAG, "InitializeDeviceProperties OUT");

    return result;
}

static size_t GetIntrospectionDataSize(const OCPersistentStorage *ps)
{
    size_t size = 0;
    char buffer[INTROSPECTION_FILE_SIZE_BLOCK];
    FILE *fp;

    if (!ps)
    {
        return 0;
    }

    fp = ps->open(OC_INTROSPECTION_FILE_NAME, "rb");
    if (fp)
    {
        size_t bytesRead = 0;
        do
        {
            bytesRead = ps->read(buffer, 1, INTROSPECTION_FILE_SIZE_BLOCK, fp);
            size += bytesRead;
        } while (bytesRead);
        ps->close(fp);
    }
    return size;
}

OCStackResult GetIntrospectionDataFromPS(uint8_t **data, size_t *size)
{
    OIC_LOG(DEBUG, TAG, "GetIntrospectionDataFromPS IN");

    FILE *fp = NULL;
    uint8_t *fsData = NULL;
    size_t fileSize = 0;
    OCStackResult ret = OC_STACK_ERROR;
    OCPersistentStorage *ps = NULL;

    if (!data || *data || !size)
    {
        return OC_STACK_INVALID_PARAM;
    }

    ps = OCGetPersistentStorageHandler();
    if (!ps)
    {
        OIC_LOG(ERROR, TAG, "Persistent Storage handler is NULL");
        goto exit;
    }

    fileSize = GetIntrospectionDataSize(ps);
    OIC_LOG_V(DEBUG, TAG, "File Read Size: %zu", fileSize);
    if (fileSize)
    {
        // allocate one more byte to accomodate null terminator for string we are reading.
        fsData = (uint8_t *)OICCalloc(1, fileSize + 1);
        if (!fsData)
        {
            OIC_LOG(ERROR, TAG, "Could not allocate memory for introspection data");
            goto exit;
        }

        fp = ps->open(OC_INTROSPECTION_FILE_NAME, "rb");
        if (!fp)
        {
            OIC_LOG(ERROR, TAG, "Could not open persistent storage file for introspection data");
            goto exit;
        }
        if (ps->read(fsData, 1, fileSize, fp) == fileSize)
        {
            *size = fileSize;
            fsData[fileSize] = '\0';
            *data = fsData;
            fsData = NULL;
            ret = OC_STACK_OK;
        }
    }
    OIC_LOG(DEBUG, TAG, "GetIntrospectionDataFromPS OUT");

exit:
    if (fp)
    {
        ps->close(fp);
    }
    if (fsData)
    {
        OICFree(fsData);
    }
    return ret;
}

OCStackResult BuildIntrospectionPayloadResponse(const OCResource *resourcePtr,
    OCPayload **payload, OCDevAddr *devAddr)
{
    OC_UNUSED(resourcePtr);
    OC_UNUSED(devAddr);

    uint8_t *introspectionData = NULL;
    size_t size = 0;
    OCStackResult ret = GetIntrospectionDataFromPS(&introspectionData, &size);
    if (OC_STACK_OK == ret)
    {
        OCIntrospectionPayload *tempPayload = OCIntrospectionPayloadCreateFromCbor(introspectionData, size);
        if (tempPayload)
        {
            *payload = (OCPayload *)tempPayload;
        }
        else
        {
            ret = OC_STACK_NO_MEMORY;
            OICFree(introspectionData);
        }
    }

    return ret;
}

OCRepPayload *BuildUrlInfoWithProtocol(const char *protocol, char *ep)
{
    OCStackResult result = OC_STACK_OK;
    char introspectionUrl[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = { 0 };
    OCRepPayload *urlInfoPayload = OCRepPayloadCreate();
    if (!urlInfoPayload)
    {
        OIC_LOG(ERROR, TAG, "Failed to create a new RepPayload");
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    snprintf(introspectionUrl, sizeof(introspectionUrl), "%s%s", ep, OC_RSRVD_INTROSPECTION_PAYLOAD_URI_PATH);

    if (!OCRepPayloadSetPropString(urlInfoPayload, OC_RSRVD_INTROSPECTION_URL, introspectionUrl))
    {
        OIC_LOG(ERROR, TAG, "Failed to add url");
        result = OC_STACK_ERROR;
        goto exit;
    }
    if (!OCRepPayloadSetPropString(urlInfoPayload, OC_RSRVD_INTROSPECTION_PROTOCOL, protocol))
    {
        OIC_LOG(ERROR, TAG, "Failed to add protocol");
        result = OC_STACK_ERROR;
        goto exit;
    }
    if (!OCRepPayloadSetPropString(urlInfoPayload, OC_RSRVD_INTROSPECTION_CONTENT_TYPE, OC_RSRVD_INTROSPECTION_CONTENT_TYPE_VALUE))
    {
        OIC_LOG(ERROR, TAG, "Failed to add content type");
        result = OC_STACK_ERROR;
        goto exit;
    }
    if (!OCRepPayloadSetPropInt(urlInfoPayload, OC_RSRVD_INTROSPECTION_VERSION, OC_RSRVD_INTROSPECTION_VERSION_VALUE))
    {
        OIC_LOG(ERROR, TAG, "Failed to add version");
        result = OC_STACK_ERROR;
        goto exit;
    }

exit:
    if (result != OC_STACK_OK)
    {
        OCRepPayloadDestroy(urlInfoPayload);
        urlInfoPayload = NULL;
    }
    return urlInfoPayload;
}

OCStackResult BuildIntrospectionResponseRepresentation(const OCResource *resourcePtr,
    OCRepPayload** payload, OCDevAddr *devAddr)
{
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };
    OCRepPayload *tempPayload = NULL;
    OCRepPayload **urlInfoPayload = NULL;
    OCStackResult ret = OC_STACK_OK;
    OCResourceType *resType = NULL;
    OCResourceInterface *resInterface = NULL;
    CAEndpoint_t *caEps = NULL;
    size_t nCaEps = 0;
    CAResult_t caResult = CA_STATUS_OK;
    OCResource *payloadResPtr = FindResourceByUri(OC_RSRVD_INTROSPECTION_PAYLOAD_URI_PATH);

    if (!payloadResPtr)
    {
        ret = OC_STACK_ERROR;
        goto exit;
    }

    if (!resourcePtr)
    {
        ret = OC_STACK_INVALID_PARAM;
        goto exit;
    }

    tempPayload = OCRepPayloadCreate();
    if (!tempPayload)
    {
        ret = OC_STACK_NO_MEMORY;
        goto exit;
    }

    if (!OCRepPayloadSetUri(tempPayload, resourcePtr->uri))
    {
        OIC_LOG(ERROR, TAG, "Failed to set payload URI");
        ret = OC_STACK_ERROR;
        goto exit;
    }

    resType = resourcePtr->rsrcType;
    while (resType)
    {
        if (!OCRepPayloadAddResourceType(tempPayload, resType->resourcetypename))
        {
            OIC_LOG(ERROR, TAG, "Failed at add resource type");
            ret = OC_STACK_ERROR;
            goto exit;
        }
        resType = resType->next;
    }

    resInterface = resourcePtr->rsrcInterface;
    while (resInterface)
    {
        if (!OCRepPayloadAddInterface(tempPayload, resInterface->name))
        {
            OIC_LOG(ERROR, TAG, "Failed to add interface");
            ret = OC_STACK_ERROR;
            goto exit;
        }
        resInterface = resInterface->next;
    }
    if (!OCRepPayloadSetPropString(tempPayload, OC_RSRVD_INTROSPECTION_NAME, OC_RSRVD_INTROSPECTION_NAME_VALUE))
    {
        OIC_LOG(ERROR, TAG, "Failed to set Name property.");
        ret = OC_STACK_ERROR;
        goto exit;
    }

    caResult = CAGetNetworkInformation(&caEps, &nCaEps);
    if (CA_STATUS_FAILED == caResult)
    {
        OIC_LOG(ERROR, TAG, "CAGetNetworkInformation failed!");
        ret = OC_STACK_ERROR;
        goto exit;
    }

    // Add a urlInfo object for each endpoint supported
    urlInfoPayload = (OCRepPayload **)OICCalloc(nCaEps, sizeof(OCRepPayload*));
    if (!urlInfoPayload)
    {
        OIC_LOG(ERROR, TAG, "Unable to allocate memory for urlInfo ");
        ret = OC_STACK_NO_MEMORY;
        goto exit;
    }

    if (caEps && nCaEps && devAddr)
    {
        if ((OC_ADAPTER_IP | OC_ADAPTER_TCP) & (devAddr->adapter))
        {
            for (size_t i = 0; i < nCaEps; i++)
            {
                CAEndpoint_t *info = caEps + i;
                char *proto = NULL;

                // consider IP or TCP adapter for payload that is visible to the client
                if (((CA_ADAPTER_IP | CA_ADAPTER_TCP) & info->adapter) &&
                    (info->ifindex == devAddr->ifindex))
                {
                    OCTpsSchemeFlags matchedTps = OC_NO_TPS;
                    if (OC_STACK_OK != OCGetMatchedTpsFlags(info->adapter,
                                                            info->flags,
                                                            &matchedTps))
                    {
                        ret = OC_STACK_ERROR;
                        goto exit;
                    }

                    if ((payloadResPtr->endpointType) & matchedTps)
                    {
                        ret = OCConvertTpsToString(matchedTps, &proto);
                        if (ret != OC_STACK_OK)
                        {
                            goto exit;
                        }

                        char *epStr = OCCreateEndpointStringFromCA(&caEps[i]);
                        urlInfoPayload[dimensions[0]] = BuildUrlInfoWithProtocol(proto, epStr);
                        if (!urlInfoPayload[dimensions[0]])
                        {
                            OIC_LOG(ERROR, TAG, "Unable to build urlInfo object for protocol");
                            ret = OC_STACK_ERROR;
                            goto exit;
                        }
                        dimensions[0] = dimensions[0] + 1;
                    }
                }
            }
        }
    }

    if (!OCRepPayloadSetPropObjectArrayAsOwner(tempPayload,
        OC_RSRVD_INTROSPECTION_URL_INFO,
        urlInfoPayload,
        dimensions))
    {
        OIC_LOG(ERROR, TAG, "Unable to add urlInfo object to introspection payload ");
        ret = OC_STACK_ERROR;
        goto exit;
    }

    if (!*payload)
    {
        *payload = tempPayload;
    }
    else
    {
        OCRepPayloadAppend(*payload, tempPayload);
    }
exit:
    if (ret != OC_STACK_OK)
    {
        OCRepPayloadDestroy(tempPayload);
        if (urlInfoPayload)
        {
            for (size_t i = 0; i < nCaEps; ++i)
            {
                OCRepPayloadDestroy(urlInfoPayload[i]);
            }
            OICFree(urlInfoPayload);
        }
    }

    return OC_STACK_OK;
}

/**
 * Prepares a Payload for response.
 */
static OCStackResult BuildVirtualResourceResponse(const OCResource *resourcePtr,
                                           OCDiscoveryPayload *payload,
                                           OCDevAddr *devAddr,
                                           CAEndpoint_t *networkInfo,
                                           size_t infoSize)
{
    if (!resourcePtr || !payload)
    {
        return OC_STACK_INVALID_PARAM;
    }
    uint16_t securePort = 0;
    if (resourcePtr->resourceProperties & OC_SECURE)
    {
       if (GetSecurePortInfo(devAddr, &securePort) != OC_STACK_OK)
       {
           securePort = 0;
       }
    }

#ifdef TCP_ADAPTER
    uint16_t tcpPort = 0;
    GetTCPPortInfo(devAddr, &tcpPort, (resourcePtr->resourceProperties & OC_SECURE));

    OCDiscoveryPayloadAddResourceWithEps(payload, resourcePtr, securePort,
                                         networkInfo, infoSize, devAddr, tcpPort);
#else
    OCDiscoveryPayloadAddResourceWithEps(payload, resourcePtr, securePort,
                                         networkInfo, infoSize, devAddr);
#endif

    return OC_STACK_OK;
}

OCResource *OC_CALL FindResourceByUri(const char* resourceUri)
{
    if(!resourceUri)
    {
        return NULL;
    }

    OCResource * pointer = headResource;
    while (pointer)
    {
        if (strcmp(resourceUri, pointer->uri) == 0)
        {
            return pointer;
        }
        pointer = pointer->next;
    }
    OIC_LOG_V(INFO, TAG, "Resource %s not found", resourceUri);
    return NULL;
}

OCStackResult CheckRequestsEndpoint(const OCDevAddr *reqDevAddr,
                                    OCTpsSchemeFlags resTpsFlags)
{
    if (!reqDevAddr)
    {
        OIC_LOG(ERROR, TAG, "OCDevAddr* is NULL!!!");
        return OC_STACK_INVALID_PARAM;
    }

    OCTpsSchemeFlags reqTpsFlags = OC_NO_TPS;
    OCStackResult result = OCGetMatchedTpsFlags((CATransportAdapter_t)reqDevAddr->adapter,
                                  (CATransportFlags_t)reqDevAddr->flags, &reqTpsFlags);

    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "Failed at get TPS flags. errcode is %d", result);
        return result;
    }

    // bit compare between request tps flags and resource tps flags
    if (reqTpsFlags & resTpsFlags)
    {
        OIC_LOG(INFO, TAG, "Request come from registered TPS");
        return OC_STACK_OK;
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Request come from unregistered TPS!!!");
        return OC_STACK_BAD_ENDPOINT;
    }
}

OCStackResult DetermineResourceHandling (const OCServerRequest *request,
                                         ResourceHandling *handling,
                                         OCResource **resource)
{
    if(!request || !handling || !resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OIC_LOG_V(INFO, TAG, "DetermineResourceHandling for %s", request->resourceUrl);

    // Check if virtual resource
    if (GetTypeOfVirtualURI(request->resourceUrl) != OC_UNKNOWN_URI)
    {
        OIC_LOG_V (INFO, TAG, "%s is virtual", request->resourceUrl);
        *handling = OC_RESOURCE_VIRTUAL;
        *resource = headResource;
        return OC_STACK_OK;
    }
    if (strlen((const char*)(request->resourceUrl)) == 0)
    {
        // Resource URL not specified
        *handling = OC_RESOURCE_NOT_SPECIFIED;
        return OC_STACK_NO_RESOURCE;
    }
    else
    {
        OCResource *resourcePtr = FindResourceByUri((const char*)request->resourceUrl);
        *resource = resourcePtr;

        // Checking resource TPS flags if resource exist in stack.
        if (resourcePtr)
        {
            OCStackResult result = CheckRequestsEndpoint(&(request->devAddr), resourcePtr->endpointType);

            if (result != OC_STACK_OK)
            {
                if (result == OC_STACK_BAD_ENDPOINT)
                {
                    OIC_LOG(ERROR, TAG, "Request come from bad endpoint. ignore request!!!");
                    return OC_STACK_BAD_ENDPOINT;
                }
                else
                {
                    OIC_LOG_V(ERROR, TAG, "Failed at get tps flag errcode: %d", result);
                    return result;
                }
            }
        }

        if (!resourcePtr)
        {
            if(defaultDeviceHandler)
            {
                *handling = OC_RESOURCE_DEFAULT_DEVICE_ENTITYHANDLER;
                return OC_STACK_OK;
            }

            // Resource does not exist
            // and default device handler does not exist
            *handling = OC_RESOURCE_NOT_SPECIFIED;
            return OC_STACK_NO_RESOURCE;
        }

        if (resourcePtr && resourcePtr->rsrcChildResourcesHead != NULL)
        {
            // Collection resource
            if (resourcePtr->entityHandler != defaultResourceEHandler)
            {
                *handling = OC_RESOURCE_COLLECTION_WITH_ENTITYHANDLER;
                return OC_STACK_OK;
            }
            else
            {
                *handling = OC_RESOURCE_COLLECTION_DEFAULT_ENTITYHANDLER;
                return OC_STACK_OK;
            }
        }
        else
        {
            // Resource not a collection
            if (resourcePtr->entityHandler != defaultResourceEHandler)
            {
                *handling = OC_RESOURCE_NOT_COLLECTION_WITH_ENTITYHANDLER;
                return OC_STACK_OK;
            }
            else
            {
                *handling = OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER;
                return OC_STACK_OK;
            }
        }
    }
}

OCStackResult EntityHandlerCodeToOCStackCode(OCEntityHandlerResult ehResult)
{
    OCStackResult result;

    switch (ehResult)
    {
        case OC_EH_OK:
        case OC_EH_CONTENT:
        case OC_EH_VALID:
            result = OC_STACK_OK;
            break;
        case OC_EH_SLOW:
            result = OC_STACK_SLOW_RESOURCE;
            break;
        case OC_EH_ERROR:
            result = OC_STACK_ERROR;
            break;
        case OC_EH_FORBIDDEN:
            result = OC_STACK_FORBIDDEN_REQ;
            break;
        case OC_EH_INTERNAL_SERVER_ERROR:
            result = OC_STACK_INTERNAL_SERVER_ERROR;
            break;
        case OC_EH_RESOURCE_CREATED:
            result = OC_STACK_RESOURCE_CREATED;
            break;
        case OC_EH_RESOURCE_DELETED:
            result = OC_STACK_RESOURCE_DELETED;
            break;
        case OC_EH_CHANGED:
            result = OC_STACK_RESOURCE_CHANGED;
            break;
        case OC_EH_RESOURCE_NOT_FOUND:
            result = OC_STACK_NO_RESOURCE;
            break;
        default:
            result = OC_STACK_ERROR;
    }

    return result;
}

static bool resourceMatchesRTFilter(OCResource *resource, char *resourceTypeFilter)
{
    if (!resource)
    {
        return false;
    }

    // Null is analogous to no filter.i.e. query is of form /oic/res?if=oic.if.baseline or /oic/res,
    // without rt query.
    if (NULL == resourceTypeFilter)
    {
        return true;
    }

    // Empty resourceType filter is analogous to error query
    // It is an error as query is of form /oic/res?rt=
    if (0 == strlen(resourceTypeFilter))
    {
        return false;
    }

    for (OCResourceType *rtPtr = resource->rsrcType; rtPtr; rtPtr = rtPtr->next)
    {
        if (0 == strcmp(rtPtr->resourcetypename, resourceTypeFilter))
        {
            return true;
        }
    }

    OIC_LOG_V(INFO, TAG, "%s does not contain rt=%s.", resource->uri, resourceTypeFilter);
    return false;
}

static bool resourceMatchesIFFilter(OCResource *resource, char *interfaceFilter)
{
    if (!resource)
    {
        return false;
    }

    // Null is analogous to no filter i.e. query is of form /oic/res?rt=core.light or /oic/res,
    // without if query.
    if (NULL == interfaceFilter)
    {
        return true;
    }

    // Empty interface filter is analogous to error query
    // It is an error as query is of form /oic/res?if=
    if (0 == strlen(interfaceFilter))
    {
        return false;
    }

    for (OCResourceInterface *ifPtr = resource->rsrcInterface; ifPtr; ifPtr = ifPtr->next)
    {
        if (0 == strcmp(ifPtr->name, interfaceFilter) ||
            0 == strcmp(OC_RSRVD_INTERFACE_LL, interfaceFilter) ||
            0 == strcmp(OC_RSRVD_INTERFACE_DEFAULT, interfaceFilter))
        {
            return true;
        }
    }

    OIC_LOG_V(INFO, TAG, "%s does not contain if=%s.", resource->uri, interfaceFilter);
    return false;
}

/*
 * If the filters are null, they will be assumed to NOT be present
 * and the resource will not be matched against them.
 * Function will return true if all non null AND non empty filters passed in find a match.
 */
static bool includeThisResourceInResponse(OCResource *resource,
                                          char *interfaceFilter,
                                          char *resourceTypeFilter)
{
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Invalid resource");
        return false;
    }

    if (resource->resourceProperties & OC_EXPLICIT_DISCOVERABLE)
    {
        /*
         * At least one valid filter should be available to
         * include the resource in discovery response
         */
        if (!(resourceTypeFilter && *resourceTypeFilter))
        {
            OIC_LOG_V(INFO, TAG, "%s no query string for EXPLICIT_DISCOVERABLE\
                resource", resource->uri);
            return false;
        }
    }
    else if (!(resource->resourceProperties & OC_ACTIVE) ||
         !(resource->resourceProperties & OC_DISCOVERABLE))
    {
        OIC_LOG_V(INFO, TAG, "%s not ACTIVE or DISCOVERABLE", resource->uri);
        return false;
    }

    return resourceMatchesIFFilter(resource, interfaceFilter) &&
           resourceMatchesRTFilter(resource, resourceTypeFilter);
}

static OCStackResult SendNonPersistantDiscoveryResponse(OCServerRequest *request,
                                OCPayload *discoveryPayload, OCEntityHandlerResult ehResult)
{
    OCEntityHandlerResponse *response = NULL;
    OCStackResult result = OC_STACK_ERROR;

    response = (OCEntityHandlerResponse *)OICCalloc(1, sizeof(*response));
    VERIFY_PARAM_NON_NULL(TAG, response, "Failed allocating OCEntityHandlerResponse");

    response->ehResult = ehResult;
    response->payload = discoveryPayload;
    response->persistentBufferFlag = 0;
    response->requestHandle = (OCRequestHandle) request;

    result = OCDoResponse(response);

    OICFree(response);
    return result;

exit:
    return OC_STACK_NO_MEMORY;
}

static OCStackResult EHRequest(OCEntityHandlerRequest *ehRequest, OCPayloadType type,
    OCServerRequest *request, OCResource *resource)
{
    return FormOCEntityHandlerRequest(ehRequest,
                                     (OCRequestHandle)request,
                                     request->method,
                                     &request->devAddr,
                                     (OCResourceHandle)resource,
                                     request->query,
                                     type,
                                     request->payloadFormat,
                                     request->payload,
                                     request->payloadSize,
                                     request->numRcvdVendorSpecificHeaderOptions,
                                     request->rcvdVendorSpecificHeaderOptions,
                                     (OCObserveAction)(request->notificationFlag ? OC_OBSERVE_NO_OPTION :
                                                       request->observationOption),
                                     (OCObservationId)0,
                                     request->coapID);
}

#ifdef RD_SERVER
/**
 * Find resources at the resource directory server. These resources are not local resources but
 * remote resources.
 *
 * @param interfaceQuery The interface query parameter.
 * @param resourceTypeQuery The resourceType query parameter.
 * @param discPayload The payload that will be added with the resource information if found at RD.
 *
 * @return ::OC_STACK_OK if any resources are found else ::OC_STACK_NO_RESOURCE.
 * In case if RD server is not started, it returns ::OC_STACK_NO_RESOURCE.
 */
static OCStackResult findResourcesAtRD(const char *interfaceQuery,
                                       const char *resourceTypeQuery,
                                       OCDevAddr *endpoint,  /* GAR unused? */
                                       OCDiscoveryPayload **discPayload)
{
    OCStackResult result = OC_STACK_NO_RESOURCE;
    if (OCGetResourceHandleAtUri(OC_RSRVD_RD_URI) != NULL)
    {
        result = OCRDDatabaseDiscoveryPayloadCreateWithEp(interfaceQuery, resourceTypeQuery,
                endpoint, (*discPayload) ? &(*discPayload)->next : discPayload);
    }
    if ((*discPayload) && (*discPayload)->resources)
    {
        result = OC_STACK_OK;
    }
    return result;
}
#endif

/**
 * Creates a discovery payload and add device id information. This information is included in all
 * /oic/res response.
 *
 * @param payload  payload that will have memory alllocated and device id information added.
 *
 * @return ::OC_STACK_OK if successful in allocating memory and adding ID information.
 * ::OC_STACK_NO_MEMORY if failed allocating the memory.
 */
static OCStackResult discoveryPayloadCreateAndAddDeviceId(OCPayload **payload)
{
    if (*payload)
    {
        OIC_LOG(DEBUG, TAG, "Payload is already allocated");
        return OC_STACK_OK;
    }

    *payload = (OCPayload *) OCDiscoveryPayloadCreate();
    VERIFY_PARAM_NON_NULL(TAG, *payload, "Failed adding device id to discovery payload.");

    {
        OCDiscoveryPayload *discPayload = (OCDiscoveryPayload *)*payload;
        discPayload->sid = (char *)OICCalloc(1, UUID_STRING_SIZE);
        VERIFY_PARAM_NON_NULL(TAG, discPayload->sid, "Failed adding device id to discovery payload.");

        const char* uid = OCGetServerInstanceIDString();
        if (uid)
        {
            memcpy(discPayload->sid, uid, UUID_STRING_SIZE);
        }

    }
    return OC_STACK_OK;
exit:
    OCPayloadDestroy(*payload);
    return OC_STACK_NO_MEMORY;
}

/**
 * Add the common properties to the payload, they are only included in case of oic.if.baseline response.
 *
 * @param discPayload payload that will have the baseline information included.
 *
 * @return ::OC_STACK_OK if successful in adding all the information. ::OC_STACK_NO_MEMORY if failed
 * allocating the memory for the baseline information.
 */
static OCStackResult addDiscoveryBaselineCommonProperties(OCDiscoveryPayload *discPayload)
{
    if (!discPayload)
    {
        OIC_LOG(ERROR, TAG, "Payload is not allocated");
        return OC_STACK_INVALID_PARAM;
    }

    OCGetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME, (void **)&discPayload->name);

    discPayload->type = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
    VERIFY_PARAM_NON_NULL(TAG, discPayload->type, "Failed adding rt to discovery payload.");
    discPayload->type->value = OICStrdup(OC_RSRVD_RESOURCE_TYPE_RES);
    VERIFY_PARAM_NON_NULL(TAG, discPayload->type, "Failed adding rt value to discovery payload.");

    OCResourcePayloadAddStringLL(&discPayload->iface, OC_RSRVD_INTERFACE_LL);
    OCResourcePayloadAddStringLL(&discPayload->iface, OC_RSRVD_INTERFACE_DEFAULT);
    VERIFY_PARAM_NON_NULL(TAG, discPayload->iface, "Failed adding if to discovery payload.");

    return OC_STACK_OK;

exit:
    return OC_STACK_NO_MEMORY;
}

static bool isUnicast(OCServerRequest *request)
{
    bool isMulticast = request->devAddr.flags & OC_MULTICAST;
    return (isMulticast == false &&
           (request->devAddr.adapter != OC_ADAPTER_RFCOMM_BTEDR) &&
           (request->devAddr.adapter != OC_ADAPTER_GATT_BTLE));
}

/**
 * Handle registering/deregistering of observers of virtual resources.  Currently only the
 * well-known virtual resource (/oic/res) may be observable.
 *
 * @param request a virtual resource server request
 *
 * @return ::OC_STACK_OK on success, ::OC_STACK_DUPLICATE_REQUEST when registering a duplicate
 *         observer, some other value upon failure.
 */
static OCStackResult HandleVirtualObserveRequest(OCServerRequest *request)
{
    OCStackResult result = OC_STACK_OK;
    if (request->notificationFlag)
    {
        // The request is to send an observe payload, not register/deregister an observer
        goto exit;
    }
    OCVirtualResources virtualUriInRequest;
    virtualUriInRequest = GetTypeOfVirtualURI(request->resourceUrl);
    if (virtualUriInRequest != OC_WELL_KNOWN_URI)
    {
        // OC_WELL_KNOWN_URI is currently the only virtual resource that may be observed
        goto exit;
    }
    OCResource *resourcePtr;
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
                                                       request->requestToken,
                                                       request->tokenLength);
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
            result = AddObserver ((const char*)(request->resourceUrl),
                                  (const char *)(request->query),
                                  obsId, request->requestToken, request->tokenLength,
                                  resourcePtr, request->qos, request->acceptFormat,
                                  request->acceptVersion, &request->devAddr);
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
                                           request->requestToken, request->tokenLength);
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

static OCStackResult HandleVirtualResource (OCServerRequest *request, OCResource* resource)
{
    if (!request || !resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCPayload* payload = NULL;
    char *interfaceQuery = NULL;
    char *resourceTypeQuery = NULL;

    OIC_LOG(INFO, TAG, "Entering HandleVirtualResource");

    OCVirtualResources virtualUriInRequest = GetTypeOfVirtualURI (request->resourceUrl);

#ifdef TCP_ADAPTER
    if (OC_KEEPALIVE_RESOURCE_URI == virtualUriInRequest)
    {
        // Received request for a keepalive
        OIC_LOG(INFO, TAG, "Request is for KeepAlive Request");
        return HandleKeepAliveRequest(request, resource);
    }
#endif

    OCStackResult discoveryResult = OC_STACK_ERROR;
    if (request->method == OC_REST_PUT || request->method == OC_REST_POST ||
        request->method == OC_REST_DELETE)
    {
        OIC_LOG_V(ERROR, TAG, "Resource : %s not permitted for method: %d",
            request->resourceUrl, request->method);
        return OC_STACK_UNAUTHORIZED_REQ;
    }

    discoveryResult = HandleVirtualObserveRequest(request);
    if (discoveryResult == OC_STACK_DUPLICATE_REQUEST)
    {
        // server requests are usually free'd when the response is sent out
        // for the request in ocserverrequest.c : HandleSingleResponse()
        // Since we are making an early return and not responding, the server request
        // needs to be deleted.
        DeleteServerRequest (request);
        discoveryResult = OC_STACK_OK;
        goto exit;
    }
    else if (discoveryResult != OC_STACK_OK)
    {
        goto exit;
    }

    // Step 1: Generate the response to discovery request
    if (virtualUriInRequest == OC_WELL_KNOWN_URI
#ifdef MQ_BROKER
            || virtualUriInRequest == OC_MQ_BROKER_URI
#endif
            )
    {
        if (g_multicastServerStopped && !isUnicast(request))
        {
            // Ignore the discovery request
            DeleteServerRequest(request);
            discoveryResult = OC_STACK_CONTINUE;
            goto exit;
        }

        CAEndpoint_t *networkInfo = NULL;
        size_t infoSize = 0;

        CAResult_t caResult = CAGetNetworkInformation(&networkInfo, &infoSize);
        if (CA_STATUS_FAILED == caResult)
        {
            OIC_LOG(ERROR, TAG, "CAGetNetworkInformation has error on parsing network infomation");
            return OC_STACK_ERROR;
        }

        discoveryResult = getQueryParamsForFiltering (virtualUriInRequest, request->query,
                &interfaceQuery, &resourceTypeQuery);
        VERIFY_SUCCESS_1(discoveryResult);

        if (!interfaceQuery && !resourceTypeQuery)
        {
            // If no query is sent, default interface is used i.e. oic.if.ll.
            interfaceQuery = OICStrdup(OC_RSRVD_INTERFACE_LL);
        }

        discoveryResult = discoveryPayloadCreateAndAddDeviceId(&payload);
        VERIFY_PARAM_NON_NULL(TAG, payload, "Failed creating Discovery Payload.");
        VERIFY_SUCCESS_1(discoveryResult);

        OCDiscoveryPayload *discPayload = (OCDiscoveryPayload *)payload;
        if (interfaceQuery && 0 == strcmp(interfaceQuery, OC_RSRVD_INTERFACE_DEFAULT))
        {
            discoveryResult = addDiscoveryBaselineCommonProperties(discPayload);
            VERIFY_SUCCESS_1(discoveryResult);
        }
        OCResourceProperty prop = OC_DISCOVERABLE;
#ifdef MQ_BROKER
        prop = (OC_MQ_BROKER_URI == virtualUriInRequest) ? OC_MQ_BROKER : prop;
#endif
        for (; resource && discoveryResult == OC_STACK_OK; resource = resource->next)
        {
            // This case will handle when no resource type and it is oic.if.ll.
            // Do not assume check if the query is ll
            if (!resourceTypeQuery &&
                (interfaceQuery && 0 == strcmp(interfaceQuery, OC_RSRVD_INTERFACE_LL)))
            {
                // Only include discoverable type
                if (resource->resourceProperties & prop)
                {
                    discoveryResult = BuildVirtualResourceResponse(resource,
                                                                   discPayload,
                                                                   &request->devAddr,
                                                                   networkInfo,
                                                                   infoSize);
                }
            }
            else if (includeThisResourceInResponse(resource, interfaceQuery, resourceTypeQuery))
            {
                discoveryResult = BuildVirtualResourceResponse(resource,
                                                               discPayload,
                                                               &request->devAddr,
                                                               networkInfo,
                                                               infoSize);
            }
            else
            {
                discoveryResult = OC_STACK_OK;
            }
        }
        if (discPayload->resources == NULL)
        {
            discoveryResult = OC_STACK_NO_RESOURCE;
            OCPayloadDestroy(payload);
            payload = NULL;
        }

        if (networkInfo)
        {
            OICFree(networkInfo);
        }
#ifdef RD_SERVER
        discoveryResult = findResourcesAtRD(interfaceQuery, resourceTypeQuery, &request->devAddr,
                (OCDiscoveryPayload **)&payload);
#endif
    }
    else if (virtualUriInRequest == OC_DEVICE_URI)
    {
        OCResource *resourcePtr = FindResourceByUri(OC_RSRVD_DEVICE_URI);
        VERIFY_PARAM_NON_NULL(TAG, resourcePtr, "Device URI not found.");
        discoveryResult = BuildDevicePlatformPayload(resourcePtr, (OCRepPayload **)&payload, true);
    }
    else if (virtualUriInRequest == OC_PLATFORM_URI)
    {
        OCResource *resourcePtr = FindResourceByUri(OC_RSRVD_PLATFORM_URI);
        VERIFY_PARAM_NON_NULL(TAG, resourcePtr, "Platform URI not found.");
        discoveryResult = BuildDevicePlatformPayload(resourcePtr, (OCRepPayload **)&payload, false);
    }
#ifdef ROUTING_GATEWAY
    else if (OC_GATEWAY_URI == virtualUriInRequest)
    {
        // Received request for a gateway
        OIC_LOG(INFO, TAG, "Request is for Gateway Virtual Request");
        discoveryResult = RMHandleGatewayRequest(request);
    }
#endif
    else if (OC_INTROSPECTION_URI == virtualUriInRequest)
    {
        // Received request for introspection
        OCResource *resourcePtr = FindResourceByUri(OC_RSRVD_INTROSPECTION_URI_PATH);
        VERIFY_PARAM_NON_NULL(TAG, resourcePtr, "Introspection URI not found.");
        discoveryResult = BuildIntrospectionResponseRepresentation(resourcePtr, (OCRepPayload **)&payload, &request->devAddr);
        OIC_LOG(INFO, TAG, "Request is for Introspection");
    }
    else if (OC_INTROSPECTION_PAYLOAD_URI == virtualUriInRequest)
    {
        // Received request for introspection payload
        OCResource *resourcePtr = FindResourceByUri(OC_RSRVD_INTROSPECTION_PAYLOAD_URI_PATH);
        VERIFY_PARAM_NON_NULL(TAG, resourcePtr, "Introspection Payload URI not found.");
        discoveryResult = BuildIntrospectionPayloadResponse(resourcePtr, &payload, &request->devAddr);
        OIC_LOG(INFO, TAG, "Request is for Introspection Payload");
    }
    /**
     * Step 2: Send the discovery response
     *
     * Iotivity should respond to discovery requests in below manner:
     * 1)If query filter matching fails and discovery request is multicast,
     *   it should NOT send any response.
     * 2)If query filter matching fails and discovery request is unicast,
     *   it should send an error(RESOURCE_NOT_FOUND - 404) response.
     * 3)If Server does not have any 'DISCOVERABLE' resources and discovery
     *   request is multicast, it should NOT send any response.
     * 4)If Server does not have any 'DISCOVERABLE' resources and discovery
     *   request is unicast, it should send an error(RESOURCE_NOT_FOUND - 404) response.
     */

#ifdef WITH_PRESENCE
    if ((virtualUriInRequest == OC_PRESENCE) &&
        (resource->resourceProperties & OC_ACTIVE))
    {
        // Need to send ACK when the request is CON.
        if (request->qos == OC_HIGH_QOS)
        {
            CAEndpoint_t endpoint = { .adapter = CA_DEFAULT_ADAPTER };
            CopyDevAddrToEndpoint(&request->devAddr, &endpoint);
            SendDirectStackResponse(&endpoint, request->coapID, CA_EMPTY, CA_MSG_ACKNOWLEDGE,
                                    0, NULL, NULL, 0, NULL, CA_RESPONSE_FOR_RES);
        }
        DeleteServerRequest(request);

        // Presence uses observer notification api to respond via SendPresenceNotification.
        SendPresenceNotification(resource->rsrcType, OC_PRESENCE_TRIGGER_CHANGE);
    }
    else
#endif
#if ROUTING_GATEWAY
    // Gateway uses the RMHandleGatewayRequest to respond to the request.
    if (OC_GATEWAY_URI != virtualUriInRequest)
#endif
    {
	//GAR        OIC_LOG_PAYLOAD(DEBUG, payload);
        if(discoveryResult == OC_STACK_OK)
        {
            SendNonPersistantDiscoveryResponse(request, payload, OC_EH_OK);
        }
        else // Error handling
        {
            if (isUnicast(request))
            {
                OIC_LOG_V(ERROR, TAG, "Sending a (%d) error to (%d) discovery request",
                    discoveryResult, virtualUriInRequest);
                SendNonPersistantDiscoveryResponse(request, NULL,
                    (discoveryResult == OC_STACK_NO_RESOURCE) ?
                        OC_EH_RESOURCE_NOT_FOUND : OC_EH_ERROR);
            }
            else // Multicast
            {
                // Ignoring the discovery request as per RFC 7252, Section #8.2
                OIC_LOG(INFO, TAG, "Silently ignoring the request since no useful data to send.");
                // the request should be removed.
                // since it never remove and causes a big memory waste.
                DeleteServerRequest(request);
            }
            discoveryResult = OC_STACK_CONTINUE;
        }
    }

exit:
    if (interfaceQuery)
    {
        OICFree(interfaceQuery);
    }

    if (resourceTypeQuery)
    {
        OICFree(resourceTypeQuery);
    }
    OCPayloadDestroy(payload);

    // To ignore the message, OC_STACK_CONTINUE is sent
    return discoveryResult;
}

static OCStackResult
HandleDefaultDeviceEntityHandler(OCServerRequest *request)
{
    if (!request)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerRequest ehRequest = {0};
    OIC_LOG(INFO, TAG, "Entering HandleResourceWithDefaultDeviceEntityHandler");
    OCStackResult result = EHRequest(&ehRequest, PAYLOAD_TYPE_REPRESENTATION, request, NULL);
    VERIFY_SUCCESS_1(result);

    // At this point we know for sure that defaultDeviceHandler exists
    ehResult = defaultDeviceHandler(OC_REQUEST_FLAG, &ehRequest,
                                  (char*) request->resourceUrl, defaultDeviceHandlerCallbackParameter);
    if(ehResult == OC_EH_SLOW)
    {
        OIC_LOG(INFO, TAG, "This is a slow resource");
        request->slowFlag = 1;
    }
    else if(ehResult == OC_EH_ERROR)
    {
        DeleteServerRequest(request);
    }
    result = EntityHandlerCodeToOCStackCode(ehResult);
exit:
    OCPayloadDestroy(ehRequest.payload);
    return result;
}

static OCStackResult
HandleResourceWithEntityHandler(OCServerRequest *request,
                                OCResource *resource)
{
    if(!request || ! resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_ERROR;
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerFlag ehFlag = OC_REQUEST_FLAG;
    ResourceObserver *resObs = NULL;

    OCEntityHandlerRequest ehRequest = {0};

    OIC_LOG(INFO, TAG, "Entering HandleResourceWithEntityHandler");
    OCPayloadType type = PAYLOAD_TYPE_REPRESENTATION;
    // check the security resource
    if (request /* GAR: this is always true: && request->resourceUrl */
	&& SRMIsSecurityResourceURI(request->resourceUrl))
    {
        type = PAYLOAD_TYPE_SECURITY;
    }

    result = EHRequest(&ehRequest, type, request, resource);
    VERIFY_SUCCESS_1(result);

    if(ehRequest.obsInfo.action == OC_OBSERVE_NO_OPTION)
    {
        OIC_LOG(INFO, TAG, "No observation requested");
        ehFlag = OC_REQUEST_FLAG;
    }
    else if(ehRequest.obsInfo.action == OC_OBSERVE_REGISTER)
    {
        OIC_LOG(INFO, TAG, "Observation registration requested");

        ResourceObserver *obs = GetObserverUsingToken(resource,
                                                      request->requestToken, request->tokenLength);

        if (obs)
        {
            OIC_LOG (INFO, TAG, "Observer with this token already present");
            OIC_LOG (INFO, TAG, "Possibly re-transmitted CON OBS request");
            OIC_LOG (INFO, TAG, "Not adding observer. Not responding to client");
            OIC_LOG (INFO, TAG, "The first request for this token is already ACKED.");

            // server requests are usually free'd when the response is sent out
            // for the request in ocserverrequest.c : HandleSingleResponse()
            // Since we are making an early return and not responding, the server request
            // needs to be deleted.
            DeleteServerRequest (request);
            return OC_STACK_OK;
        }

        result = GenerateObserverId(&ehRequest.obsInfo.obsId);
        VERIFY_SUCCESS_1(result);

        result = AddObserver ((const char*)(request->resourceUrl),
                (const char *)(request->query),
                ehRequest.obsInfo.obsId, request->requestToken, request->tokenLength,
                resource, request->qos, request->acceptFormat,
                request->acceptVersion, &request->devAddr);

        if(result == OC_STACK_OK)
        {
            OIC_LOG(INFO, TAG, "Added observer successfully");
            request->observeResult = OC_STACK_OK;
            ehFlag = (OCEntityHandlerFlag)(OC_REQUEST_FLAG | OC_OBSERVE_FLAG);
        }
        else if (result == OC_STACK_RESOURCE_ERROR)
        {
            OIC_LOG(INFO, TAG, "The Resource is not active, discoverable or observable");
            request->observeResult = OC_STACK_ERROR;
            ehFlag = OC_REQUEST_FLAG;
        }
        else
        {
            // The error in observeResult for the request will be used when responding to this
            // request by omitting the observation option/sequence number.
            request->observeResult = OC_STACK_ERROR;
            OIC_LOG(ERROR, TAG, "Observer Addition failed");
            ehFlag = OC_REQUEST_FLAG;
            DeleteServerRequest(request);
            goto exit;
        }

    }
    else if(ehRequest.obsInfo.action == OC_OBSERVE_DEREGISTER)
    {
        OIC_LOG(INFO, TAG, "Deregistering observation requested");

        resObs = GetObserverUsingToken (resource,
                                        request->requestToken, request->tokenLength);

        if (NULL == resObs)
        {
            // Stack does not contain this observation request
            // Either token is incorrect or observation list is corrupted
            result = OC_STACK_ERROR;
            goto exit;
        }
        ehRequest.obsInfo.obsId = resObs->observeId;
        ehFlag = (OCEntityHandlerFlag)(ehFlag | OC_OBSERVE_FLAG);

        result = DeleteObserverUsingToken (resource,
                                           request->requestToken, request->tokenLength);

        if(result == OC_STACK_OK)
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
            DeleteServerRequest(request);
            goto exit;
        }
    }
    else
    {
        result = OC_STACK_ERROR;
        goto exit;
    }

    ehResult = resource->entityHandler(ehFlag, &ehRequest, resource->entityHandlerCallbackParam);
    if(ehResult == OC_EH_SLOW)
    {
        OIC_LOG(INFO, TAG, "This is a slow resource");
        request->slowFlag = 1;
    }
    else if(ehResult == OC_EH_ERROR)
    {
        DeleteServerRequest(request);
    }
    result = EntityHandlerCodeToOCStackCode(ehResult);
exit:
    OCPayloadDestroy(ehRequest.payload);
    return result;
}

static OCStackResult HandleCollectionResourceDefaultEntityHandler(OCServerRequest *request,
                                                                  OCResource *resource)
{
    if (!request || !resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCEntityHandlerRequest ehRequest = {0};
    OCStackResult result = EHRequest(&ehRequest, PAYLOAD_TYPE_REPRESENTATION, request, resource);
    if(result == OC_STACK_OK)
    {
        result = DefaultCollectionEntityHandler (OC_REQUEST_FLAG, &ehRequest);
    }

    OCPayloadDestroy(ehRequest.payload);
    return result;
}

OCStackResult
ProcessRequest(ResourceHandling resHandling, OCResource *resource, OCServerRequest *request)
{
    OCStackResult ret = OC_STACK_OK;

    switch (resHandling)
    {
        case OC_RESOURCE_VIRTUAL:
        {
            ret = HandleVirtualResource (request, resource);
            break;
        }
        case OC_RESOURCE_DEFAULT_DEVICE_ENTITYHANDLER:
        {
            ret = HandleDefaultDeviceEntityHandler(request);
            break;
        }
        case OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER:
        {
            OIC_LOG(INFO, TAG, "OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER");
            return OC_STACK_ERROR;
        }
        case OC_RESOURCE_NOT_COLLECTION_WITH_ENTITYHANDLER:
        {
            ret = HandleResourceWithEntityHandler (request, resource);
            break;
        }
        case OC_RESOURCE_COLLECTION_WITH_ENTITYHANDLER:
        {
            ret = HandleResourceWithEntityHandler (request, resource);
            break;
        }
        case OC_RESOURCE_COLLECTION_DEFAULT_ENTITYHANDLER:
        {
            ret = HandleCollectionResourceDefaultEntityHandler (request, resource);
            break;
        }
        case OC_RESOURCE_NOT_SPECIFIED:
        {
            ret = OC_STACK_NO_RESOURCE;
            break;
        }
        default:
        {
            OIC_LOG(INFO, TAG, "Invalid Resource Determination");
            return OC_STACK_ERROR;
        }
    }
    return ret;
}

OCStackResult OC_CALL OCSetPlatformInfo(OCPlatformInfo info)
EXPORT
{
    OCResource *resource = NULL;
    if (!info.platformID || !info.manufacturerName)
    {
        OIC_LOG(ERROR, TAG, "No value specified.");
        goto exit;
    }
    if (0 == strlen(info.platformID) || 0 == strlen(info.manufacturerName))
    {
        OIC_LOG(ERROR, TAG, "The passed value cannot be empty");
        goto exit;
    }
    if ((info.manufacturerName && strlen(info.manufacturerName) > MAX_PLATFORM_NAME_LENGTH) ||
        (info.manufacturerUrl && strlen(info.manufacturerUrl) > MAX_PLATFORM_URL_LENGTH) ||
        (info.modelNumber && strlen(info.modelNumber) > MAX_PLATFORM_NAME_LENGTH) ||
        (info.platformVersion && strlen(info.platformVersion) > MAX_PLATFORM_NAME_LENGTH) ||
        (info.operatingSystemVersion && strlen(info.operatingSystemVersion) > MAX_PLATFORM_NAME_LENGTH) ||
        (info.hardwareVersion && strlen(info.hardwareVersion) > MAX_PLATFORM_NAME_LENGTH) ||
        (info.firmwareVersion && strlen(info.firmwareVersion) > MAX_PLATFORM_NAME_LENGTH) ||
        (info.supportUrl && strlen(info.supportUrl) > MAX_PLATFORM_URL_LENGTH))
    {
        OIC_LOG(ERROR, TAG, "The passed value is bigger than permitted.");
        goto exit;
    }

    /*
     * @todo (IOT-1541) There are several versions of a UUID structure and conversion
     * methods scattered around the IoTivity code base.  They need to be combined
     * into one PAL API.
     */
    uint8_t uuid[UUID_SIZE];
    if (!OCConvertStringToUuid(info.platformID, uuid))
    {
        OIC_LOG_V(ERROR, TAG, "Platform ID is not a UUID.");
        goto exit;
    }

    resource = FindResourceByUri(OC_RSRVD_PLATFORM_URI);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Platform Resource does not exist.");
        goto exit;
    }
    OIC_LOG(INFO, TAG, "Entering OCSetPlatformInfo");
    VERIFY_SUCCESS_1(OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_PLATFORM_ID, info.platformID));
    VERIFY_SUCCESS_1(OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_MFG_NAME, info.manufacturerName));
    OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_MFG_URL, info.manufacturerUrl);
    OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_MODEL_NUM, info.modelNumber);
    OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_MFG_DATE, info.dateOfManufacture);
    OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_PLATFORM_VERSION, info.platformVersion);
    OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_OS_VERSION, info.operatingSystemVersion);
    OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_HARDWARE_VERSION, info.hardwareVersion);
    OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_FIRMWARE_VERSION, info.firmwareVersion);
    OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_SUPPORT_URL, info.supportUrl);
    OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_SYSTEM_TIME, info.systemTime);
    OIC_LOG(INFO, TAG, "Platform parameter initialized successfully.");
    return OC_STACK_OK;

exit:
    return OC_STACK_INVALID_PARAM;
}

OCStackResult OC_CALL OCSetDeviceInfo(OCDeviceInfo info)
EXPORT
{
    OCResource *resource = FindResourceByUri(OC_RSRVD_DEVICE_URI);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Device Resource does not exist.");
        goto exit;
    }
    if (!info.deviceName || info.deviceName[0] == '\0')
    {
        OIC_LOG(ERROR, TAG, "Null or empty device name.");
       return OC_STACK_INVALID_PARAM;
    }

    if (OCGetServerInstanceIDString() == NULL)
    {
        OIC_LOG(INFO, TAG, "Device ID generation failed");
        goto exit;
    }

    VERIFY_SUCCESS_1(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME, info.deviceName));
    for (OCStringLL *temp = info.types; temp; temp = temp->next)
    {
        if (temp->value)
        {
            VERIFY_SUCCESS_1(OCBindResourceTypeToResource(resource, temp->value));
        }
    }
    VERIFY_SUCCESS_1(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_SPEC_VERSION, info.specVersion ?
        info.specVersion: OC_SPEC_VERSION));

    if (info.dataModelVersions)
    {
        char *dmv = OCCreateString(info.dataModelVersions);
        VERIFY_PARAM_NON_NULL(TAG, dmv, "Failed allocating dataModelVersions");
        OCStackResult r = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DATA_MODEL_VERSION, dmv);
        OICFree(dmv);
        VERIFY_SUCCESS_1(r);
    }
    else
    {
        VERIFY_SUCCESS_1(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DATA_MODEL_VERSION,
            OC_DATA_MODEL_VERSION));
    }
    OIC_LOG(INFO, TAG, "Device parameter initialized successfully.");
    return OC_STACK_OK;

exit:
    return OC_STACK_ERROR;
}

static OCStackResult OC_CALL OCGetAttribute(const OCResource *resource, const char *attribute, void **value)
{
    if (!resource || !attribute)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (0 == strlen(attribute))
    {
        return OC_STACK_INVALID_PARAM;
    }
    // Special attributes - these values are not in rsrcAttributes
    if (0 == strcmp(OC_RSRVD_DEVICE_ID, attribute))
    {
        *value = OICStrdup(OCGetServerInstanceIDString());
        return OC_STACK_OK;
    }
    if (0 == strcmp(OC_RSRVD_RESOURCE_TYPE, attribute))
    {
        *value = NULL;
        for (OCResourceType *resType = resource->rsrcType; resType; resType = resType->next)
        {
            OCResourcePayloadAddStringLL((OCStringLL**)&value, resType->resourcetypename);
        }
        return OC_STACK_OK;
    }
    if (0 == strcmp(OC_RSRVD_INTERFACE, attribute))
    {
        *value = NULL;
        for (OCResourceInterface *resInterface = resource->rsrcInterface; resInterface;
             resInterface = resInterface->next)
        {
            OCResourcePayloadAddStringLL((OCStringLL**)&value, resInterface->name);
        }
        return OC_STACK_OK;
    }
    // Generic attributes
    for (OCAttribute *temp = resource->rsrcAttributes; temp; temp = temp->next)
    {
        if (0 == strcmp(attribute, temp->attrName))
        {
            // A special case as this type return OCStringLL
            if (0 == strcmp(OC_RSRVD_DATA_MODEL_VERSION, attribute) ||
                    0 == strcmp(OC_RSRVD_DEVICE_DESCRIPTION, attribute) ||
                    0 == strcmp(OC_RSRVD_DEVICE_MFG_NAME, attribute))
            {
                *value = CloneOCStringLL((OCStringLL *)temp->attrValue);
                return OC_STACK_OK;
            }
            else
            {
                *value = OICStrdup((char *)temp->attrValue);
                return OC_STACK_OK;
            }
        }
    }
    return OC_STACK_NO_RESOURCE;
}

OCStackResult OC_CALL OCGetPropertyValue(OCPayloadType type, const char *prop, void **value)
{
    if (!prop)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (strlen(prop) == 0)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (*value)
    {
        *value = NULL;
    }
    OCStackResult res =  OC_STACK_NO_RESOURCE;
    if (PAYLOAD_TYPE_DEVICE == type || PAYLOAD_TYPE_PLATFORM == type)
    {
        const char *pathType = (type == PAYLOAD_TYPE_DEVICE) ? OC_RSRVD_DEVICE_URI : OC_RSRVD_PLATFORM_URI;
        OCResource *resource = FindResourceByUri(pathType);
        if (!resource)
        {
            return OC_STACK_NO_RESOURCE;
        }

        res = OCGetAttribute(resource, prop, value);
    }
    return res;
}

/**
 * Sets the value of an attribute on a resource.
 */
LOCAL OCStackResult SetAttributeInternal(OCResource *resource,
                                          const char *attribute,
                                          const void *value,
                                          bool updateDatabase)
{
    OCAttribute *resAttrib = NULL;

    // Read-only attributes - these values are set via other APIs
    if (0 == strcmp(attribute, OC_RSRVD_RESOURCE_TYPE) ||
            0 == strcmp(attribute, OC_RSRVD_INTERFACE) ||
            0 == strcmp(attribute, OC_RSRVD_DEVICE_ID))
    {
        return OC_STACK_INVALID_PARAM;
    }

    // See if the attribute already exists in the list.
    for (resAttrib = resource->rsrcAttributes; resAttrib; resAttrib = resAttrib->next)
    {
        if (0 == strcmp(attribute, resAttrib->attrName))
        {
            // Found, free the old value.
            if (0 == strcmp(OC_RSRVD_DATA_MODEL_VERSION, resAttrib->attrName) ||
                    0 == strcmp(OC_RSRVD_DEVICE_DESCRIPTION, resAttrib->attrName) ||
                    0 == strcmp(OC_RSRVD_DEVICE_MFG_NAME, resAttrib->attrName))
            {
                OCFreeOCStringLL((OCStringLL *)resAttrib->attrValue);
            }
            else
            {
                OICFree((char *)resAttrib->attrValue);
            }
            break;
        }
    }

    // If not already in the list, add it.
    if (NULL == resAttrib)
    {
        resAttrib = (OCAttribute *)OICCalloc(1, sizeof(OCAttribute));
        VERIFY_PARAM_NON_NULL(TAG, resAttrib, "Failed allocating OCAttribute");
        resAttrib->attrName = OICStrdup(attribute);
        VERIFY_PARAM_NON_NULL(TAG, resAttrib->attrName, "Failed allocating attribute name");
        resAttrib->next = resource->rsrcAttributes;
        resource->rsrcAttributes = resAttrib;
    }

    // Fill in the new value.
    if (0 == strcmp(OC_RSRVD_DATA_MODEL_VERSION, attribute))
    {
        resAttrib->attrValue = OCCreateOCStringLL((char *)value);
    }
    else if (0 == strcmp(OC_RSRVD_DEVICE_DESCRIPTION, attribute) ||
            0 == strcmp(OC_RSRVD_DEVICE_MFG_NAME, attribute))
    {
        resAttrib->attrValue = CloneOCStringLL((OCStringLL *)value);
    }
    else
    {
        resAttrib->attrValue = OICStrdup((char *)value);
    }
    VERIFY_PARAM_NON_NULL(TAG, resAttrib->attrValue, "Failed allocating attribute value");

    // The resource has changed from what is stored in the database. Update the database to
    // reflect the new value.
    if (updateDatabase)
    {
        OCDeviceProperties *deviceProperties = NULL;

        OCStackResult result = CreateDeviceProperties((const char*)value, &deviceProperties);
        if (OC_STACK_OK == result)
        {
            result = UpdateDevicePropertiesDatabase(deviceProperties);
            if (OC_STACK_OK != result)
            {
                OIC_LOG(ERROR, TAG, "UpdateDevicePropertiesDatabase failed!");
            }

            CleanUpDeviceProperties(&deviceProperties);
        }
        else
        {
            OIC_LOG(ERROR, TAG, "CreateDeviceProperties failed!");
        }
    }

    return OC_STACK_OK;

exit:
    OCDeleteResourceAttributes(resAttrib);
    return OC_STACK_NO_MEMORY;
}

static OCStackResult IsDatabaseUpdateNeeded(const char *attribute, const void *value, bool *update)
{
    OCStackResult result = OC_STACK_OK;
    void *currentPIID = NULL;

    if (!attribute || !value || !update)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *update = false;

    // Protocol Independent ID
    if (0 == strcmp(OC_RSRVD_PROTOCOL_INDEPENDENT_ID, attribute))
    {
        result = OCGetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_PROTOCOL_INDEPENDENT_ID, &currentPIID);
        if (OC_STACK_OK == result)
        {
            // PIID has already been set on the resource and stored in the database. Check to see
            // if the value is changing so the database can be updated accordingly.
            if (0 != strcmp((char *)currentPIID, (char*)value))
            {
                *update = true;
            }
        }
        else if (OC_STACK_NO_RESOURCE == result)
        {
            // PIID has not been set yet so we should always update the database.
            *update = true;
            result = OC_STACK_OK;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG,
                "Call to OCGetPropertyValue for the current PIID failed with error: %d", result);
        }
    }

    // Clean Up
    OICFreeAndSetToNull(&currentPIID);

    return result;
}

OCStackResult OC_CALL OCSetAttribute(OCResource *resource, const char *attribute, const void *value)
{
    bool updateDatabase = false;

    // Check to see if we also need to update the database for this attribute. If the current
    // value matches what is stored in the database we can skip the update and an unnecessary
    // write.
    if (OC_STACK_OK != IsDatabaseUpdateNeeded(attribute, value, &updateDatabase))
    {
        OIC_LOG_V(WARNING, TAG,
            "Could not determine if a database update was needed for %s. Proceeding without updating the database.",
            attribute);
        updateDatabase = false;
    }

    return SetAttributeInternal(resource, attribute, value, updateDatabase);
}

OCStackResult OC_CALL OCSetPropertyValue(OCPayloadType type, const char *prop, const void *value)
{
    if (!prop || !value)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (strlen(prop) == 0)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult res = OC_STACK_ERROR;
    if (PAYLOAD_TYPE_DEVICE == type || PAYLOAD_TYPE_PLATFORM == type)
    {
        const char *pathType = (type == PAYLOAD_TYPE_DEVICE) ? OC_RSRVD_DEVICE_URI : OC_RSRVD_PLATFORM_URI;
        OCResource *resource = FindResourceByUri(pathType);
        if (!resource)
        {
            OIC_LOG(ERROR, TAG, "Resource does not exist.");
        }
        else
        {
            res = OCSetAttribute(resource, prop, value);
        }
    }

    return res;
}

bool IsObservationIdExisting(const OCObservationId observationId)
{
    OCResource *resource = NULL;
    LL_FOREACH(headResource, resource)
    {
        if (NULL != GetObserverUsingId(resource, observationId))
        {
            return true;
        }
    }
    return false;
}

bool GetObserverFromResourceList(OCResource **outResource, ResourceObserver **outObserver,
                                 const CAToken_t token, uint8_t tokenLength)
{
    OCResource *resPtr = NULL;
    ResourceObserver* obsPtr = NULL;
    LL_FOREACH(headResource, resPtr)
    {
        obsPtr = GetObserverUsingToken(resPtr, token, tokenLength);
        if (obsPtr)
        {
            *outResource = resPtr;
            *outObserver = obsPtr;
            return true;
        }
    }

    *outResource = NULL;
    *outObserver = NULL;
    return false;
}

void GiveStackFeedBackObserverNotInterested(const OCDevAddr *devAddr)
{
    if (!devAddr)
    {
        return;
    }

    OIC_LOG_V(INFO, TAG, "Observer(%s:%u) is not interested anymore", devAddr->addr,
              devAddr->port);

    OCResource *resource = NULL;
    ResourceObserver *observer = NULL;
    ResourceObserver *tmp = NULL;

    LL_FOREACH(headResource, resource)
    {
        LL_FOREACH_SAFE(resource->observersHead, observer, tmp)
        {
            if ((strcmp(observer->devAddr.addr, devAddr->addr) == 0)
                    && observer->devAddr.port == devAddr->port)
            {
                OCStackFeedBack(observer->token, observer->tokenLength,
                                OC_OBSERVER_NOT_INTERESTED);
            }
        }
    }
}
