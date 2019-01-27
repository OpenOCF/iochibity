#include "oocf_resource_list.h"

/* src: ocstack.h */
OCResource *headResource = NULL;
OCResource *tailResource = NULL;
OCResourceHandle platformResource = {0};
OCResourceHandle deviceResource = {0};
OCResourceHandle introspectionResource = {0};
OCResourceHandle introspectionPayloadResource = {0};
OCResourceHandle wellKnownResource = {0};
#ifdef MQ_BROKER
OCResourceHandle brokerResource = {0};
#endif

/**
 * Initialize resource data structures, variables, etc.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult initResources()
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
    if (result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "OCCreateResource %s rc: %d", OC_RSRVD_PRESENCE_URI, result);
	goto exit;
    }
    //make resource inactive
    result = OCChangeResourceProperty(
            &(((OCResource *) presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 0);
    if (result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "OCChangeResourceProperty rc: %d", result);
    }
#endif
    result = SRMInitSecureResources();
    if (result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "SRMInitSecureResources rc: %d", result);
	goto exit;
    }

    /* now create "core" resources: /oic/res, /oic/d, /oic/p */
    result = OCCreateResource(&wellKnownResource,
			      OC_RSRVD_RESOURCE_TYPE_RES,
			      OC_RSRVD_INTERFACE_LL,
			      OC_RSRVD_WELL_KNOWN_URI, /* /oic/res */
			      NULL,
			      NULL,
			      0);
    if(result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "OCCreateResource %s rc: %d", OC_RSRVD_WELL_KNOWN_URI, result);
	goto exit;
    }

    result = BindResourceInterfaceToResource((OCResource *)wellKnownResource,
                                                     OC_RSRVD_INTERFACE_DEFAULT);
    if(result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "BindResourceInterfaceToResource %s :: %s rc: %d",
		  OC_RSRVD_INTERFACE_DEFAULT, OC_RSRVD_WELL_KNOWN_URI, result);
	goto exit;
    }

    CreateResetProfile();
    result = OCCreateResource(&deviceResource,
			      OC_RSRVD_RESOURCE_TYPE_DEVICE,
			      OC_RSRVD_INTERFACE_DEFAULT,
			      OC_RSRVD_DEVICE_URI, /* /oic/d */
			      NULL,
			      NULL,
			      OC_DISCOVERABLE);
    if(result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "OCCreateResource %s rc: %d", OC_RSRVD_DEVICE_URI, result);
	goto exit;
    }

    result = BindResourceInterfaceToResource((OCResource *)deviceResource,
					     OC_RSRVD_INTERFACE_READ);
    if(result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "BindResourceInterfaceToResource %s :: %s rc: %d",
		  OC_RSRVD_INTERFACE_READ, OC_RSRVD_DEVICE_URI, result);
    }

    result = OCCreateResource(&platformResource,
			      OC_RSRVD_RESOURCE_TYPE_PLATFORM,
			      OC_RSRVD_INTERFACE_DEFAULT,
			      OC_RSRVD_PLATFORM_URI, /* /oic/p */
			      NULL,
			      NULL,
			      OC_DISCOVERABLE);
    if(result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "OCCreateResource %s rc: %d", OC_RSRVD_PLATFORM_URI, result);
	goto exit;
    }
    result = BindResourceInterfaceToResource((OCResource *)platformResource,
					     OC_RSRVD_INTERFACE_READ);
    if (result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "BindResourceInterfaceToResource %s :: %s rc: %d",
		  OC_RSRVD_INTERFACE_READ, OC_RSRVD_PLATFORM_URI, result);
    }

    result = OCCreateResource(&introspectionResource,
			      OC_RSRVD_RESOURCE_TYPE_INTROSPECTION,
			      OC_RSRVD_INTERFACE_DEFAULT,
			      OC_RSRVD_INTROSPECTION_URI_PATH, /* introspection */
			      NULL,
			      NULL,
			      OC_DISCOVERABLE | OC_SECURE);
    if (result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "OCCreateResource %s rc: %d", OC_RSRVD_INTROSPECTION_URI_PATH, result);
	goto exit;
    }

    result = BindResourceInterfaceToResource((OCResource *)introspectionResource,
					     OC_RSRVD_INTERFACE_READ);
    if (result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "BindResourceInterfaceToResource %s :: %s rc: %d",
		  OC_RSRVD_INTERFACE_READ, OC_RSRVD_INTROSPECTION_URI_PATH, result);
    }

    result = OCCreateResource(&introspectionPayloadResource,
			      OC_RSRVD_RESOURCE_TYPE_INTROSPECTION_PAYLOAD,
			      OC_RSRVD_INTERFACE_DEFAULT,
			      OC_RSRVD_INTROSPECTION_PAYLOAD_URI_PATH, /* /introspection/payload;; */
			      NULL,
			      NULL,
			      0);
    if (result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "OCCreateResource %s rc: %d", OC_RSRVD_INTROSPECTION_PAYLOAD_URI_PATH, result);
	goto exit;
    }
    result = BindResourceInterfaceToResource((OCResource *)introspectionPayloadResource,
					     OC_RSRVD_INTERFACE_READ);
    if (result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "BindResourceInterfaceToResource %s :: %s rc: %d",
		  OC_RSRVD_INTERFACE_READ, OC_RSRVD_INTROSPECTION_PAYLOAD_URI_PATH, result);
    }

    // Initialize Device Properties
    result = InitializeDeviceProperties();
    if (result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "InitializeDeviceProperties rc: %d", result);
    }

    // Initialize platform ID of OC_RSRVD_RESOURCE_TYPE_PLATFORM.
    // Multiple devices or applications running on the same IoTivity platform should have the same
    // platform ID.
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
	    if (OC_STACK_OK != result) {
	    }
	} else {
            result = OC_STACK_ERROR;
            OIC_LOG(ERROR, TAG, "Failed OCConvertUuidToString() for platform ID.");
        }
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
 exit:
    return result;
}

/**
 * Add a resource to the end of the linked list of resources.
 *
 * @param resource Resource to be added
 */
void insertResource(OCResource *resource)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, resource %p", __func__, resource);

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
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, searching for %p", __func__, resource);

    OCResource *pointer = headResource;

    while (pointer)
    {
	OIC_LOG_V(DEBUG, TAG, "resource ptr: %p", pointer);

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
void deleteAllResources()
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
OCStackResult deleteResource(OCResource *resource)
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

OCStackResult OC_CALL OCCreateResource(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *uri,
        OCEntityHandler entityHandler,
        void *callbackParam,
        uint8_t resourceProperties) EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s %s ENTRY >>>>>>>>>>>>>>>>", __func__, uri);

    OCStackResult r = OCCreateResourceWithEp(handle,
                                  resourceTypeName,
                                  resourceInterfaceName,
                                  uri, entityHandler,
                                  callbackParam,
                                  resourceProperties,
                                  OC_ALL);
    if (r != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "OCCreateResourceWithEp %s rc: %d", uri, r);
    }
    OIC_LOG_V(INFO, TAG, "%s %s EXIT <<<<<<<<<<<<<<<<", __func__, uri);
    return r;
}

OCStackResult OC_CALL OCCreateResourceWithEp(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *uri, OCEntityHandler entityHandler,
        void *callbackParam,
        uint8_t resourceProperties, /* FIXME: naming. resourcePolicies */
        OCTpsSchemeFlags resourceTpsTypes)
{

    OCResource *pointer = NULL;
    OCStackResult result = OC_STACK_ERROR;

    OIC_LOG_V(DEBUG, TAG, "%s ENTRY; uri: %s", __func__, uri);

    /* Why no resources for OC_CLIENT? */
    /* if(myStackMode == OC_CLIENT) */
    /* { */
    /* 	OIC_LOG_V(ERROR, TAG, "%s: invalid stack mode OC_CLIENT", __func__); */
    /*     return OC_STACK_INVALID_PARAM; */
    /* } */

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

    // Set resource to secure if caller did not specify
    if ((resourceProperties & OC_MASK_RESOURCE_SECURE) == 0)
    {
        OIC_LOG_V(INFO, TAG, "%s: Creating Resource %s as OC_SECURE by default.", __func__, uri);
        resourceProperties |= OC_SECURE;
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

    OIC_LOG(INFO, TAG, "Entering OCBindResource");

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

/**
 * Delete all of the dynamically allocated elements that were created for the resource.
 *
 * @param resource Specified resource.
 */
void deleteResourceElements(OCResource *resource)
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
        if (pointer->attrName && (0 == strcmp(OC_RSRVD_DATA_MODEL_VERSION, pointer->attrName) ||
                                  0 == strcmp(OC_RSRVD_DEVICE_DESCRIPTION, pointer->attrName) ||
                                  0 == strcmp(OC_RSRVD_DEVICE_MFG_NAME, pointer->attrName)))
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








