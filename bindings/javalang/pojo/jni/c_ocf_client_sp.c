/* was: c_co_service_manager.c */

#include "c_ocf_client_sp.h"

/* #include "openocf.h" */
/* #include "oic_string.h" */
/* #include "oic_malloc.h" */
/* #include "_protocol.h" */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define TAG  "c_co_service_manager.c"

/**
 * store discovered platform in local database
 */
void register_platform(OCClientResponse* c_OCClientResponse)
{
    OIC_LOG_V(DEBUG, TAG, "%s:%d ENTRY", __func__, __LINE__);

    if (c_OCClientResponse->payload->type != PAYLOAD_TYPE_PLATFORM) {
	OIC_LOG_V(FATAL, TAG,
		  "FATAL %s %d (%s): wrong payload type\n", __FILE__, __LINE__,__func__);
	exit(1);
    }

    /* OBSOLETE OIC_LOG_DISCOVERY_RESPONSE(DEBUG, TAG, c_OCClientResponse); */

    /* Incoming response record is allocated on the stack of the
       calling routine, so it will be removed after we return.  To
       keep it around we need to make a deep copy. */

    g_Platform = OICCalloc(1, (sizeof (OCClientResponse)) );
    if (g_Platform == NULL) {
	OIC_LOG_V(FATAL, TAG,
		  "FATAL %s %d (%s): calloc failure\n", __FILE__, __LINE__,__func__);
    }
    memcpy(g_Platform, c_OCClientResponse, (sizeof (OCClientResponse)));

    g_Platform->addr = &(c_OCClientResponse->devAddr);
    /* copy the uripath string */
    /* g_Platform->resourceUri = */
    g_Platform->resourceUri = OICStrdup(c_OCClientResponse->resourceUri);

    /*GAR: OCPlatformPayload removed from 1.3 */
    /* OCPlatformPayload  *fromPP = (OCPlatformPayload*)c_OCClientResponse->payload; */
    /* OCPlatformPayload **toPP   = (OCPlatformPayload**) &(g_Platform->payload); */

    /* OCStringLL *fromSLL, **toSLL; // , *toSLLHead; */
    /* /\* 1. create new OCPlatformPayload *\/ */
    /* (*toPP)                    = (OCPlatformPayload*)OICCalloc(1, sizeof(OCPlatformPayload)); */
    /* if (!*toPP){ */
    /* 	OIC_LOG_V(FATAL, TAG, */
    /* 		  "FATAL %s %d (%s): calloc failure\n", __FILE__, __LINE__,__func__); */
    /* } */
    /* (*toPP)->base.type = PAYLOAD_TYPE_PLATFORM; */

    /* /\* 1a. copy strings *\/ */
    /* (*toPP)->uri           = OICStrdup(fromPP->uri); */

    /* /\* 1b. OCPlatformInfo *\/ */

    /* /\* 1c. copy fromPP types and interfaces *\/ */
    /* /\* types *\/ */
    /* fromSLL                    = fromPP->rt; */
    /* toSLL                      = &((*toPP)->rt); */
    /* while(fromSLL) { */
    /* 	(*toSLL)               = CloneOCStringLL(fromSLL); */
    /* 	toSLL                  = &((*toSLL)->next); */
    /* 	fromSLL                = fromSLL->next; */
    /* } */

    /* /\* interfaces *\/ */
    /* fromSLL                    = fromPP->interfaces; */
    /* toSLL                      = &((*toPP)->interfaces); */
    /* while(fromSLL) { */
    /* 	(*toSLL)               = CloneOCStringLL(fromSLL); */
    /* 	toSLL                  = &((*toSLL)->next); */
    /* 	fromSLL                = fromSLL->next; */
    /* } */

}

/**
 * store discovered devices in local database
 */
void register_device(OCClientResponse* c_OCClientResponse)
{
    OIC_LOG_V(DEBUG, TAG, "%s:%d ENTRY", __func__, __LINE__);

    if (c_OCClientResponse->payload->type != PAYLOAD_TYPE_DEVICE) {
	OIC_LOG_V(FATAL, TAG,
		  "FATAL %s %d (%s): wrong payload type\n", __FILE__, __LINE__,__func__);
	exit(1);
    }

    /* OBSOLETE: OIC_LOG_DISCOVERY_RESPONSE(DEBUG, TAG, c_OCClientResponse); */

    /* Incoming response record is allocated on the stack of the
       calling routine, so it will be removed after we return.  To
       keep it around we need to make a deep copy. */

    g_Device = OICCalloc(1, (sizeof (OCClientResponse)) );
    if (g_Device == NULL) {
	OIC_LOG_V(FATAL, TAG,
		  "FATAL %s %d (%s): calloc failure\n", __FILE__, __LINE__,__func__);
    }
    memcpy(g_Device, c_OCClientResponse, (sizeof (OCClientResponse)));

    g_Device->addr = &(c_OCClientResponse->devAddr);
    /* copy the uripath string */
    /* g_Device->resourceUri = */
    g_Device->resourceUri = OICStrdup(c_OCClientResponse->resourceUri);

    /*GAR: OCDevicePayload removed from 1.3 */
    /* OCDevicePayload  *fromPP = (OCDevicePayload*)c_OCClientResponse->payload; */
    /* OCDevicePayload **toPP   = (OCDevicePayload**) &(g_Device->payload); */

    /* OCStringLL *fromSLL, **toSLL; // , *toSLLHead; */
    /* /\* 1. create new OCDevicePayload *\/ */
    /* (*toPP)                  = (OCDevicePayload*)OICCalloc(1, sizeof(OCDevicePayload)); */
    /* if (!*toPP){ */
    /* 	OIC_LOG_V(FATAL, TAG, */
    /* 		  "FATAL %s %d (%s): calloc failure\n", __FILE__, __LINE__,__func__); */
    /* } */
    /* (*toPP)->base.type = PAYLOAD_TYPE_DEVICE; */

    /* /\* 1a. copy strings *\/ */
    /* (*toPP)->sid           = OICStrdup(fromPP->sid); */
    /* (*toPP)->deviceName    = OICStrdup(fromPP->deviceName); */
    /* (*toPP)->specVersion   = OICStrdup(fromPP->specVersion); */

    /* /\* 1b. copy OCStringLLs *\/ */
    /* /\* dataModelVersions *\/ */
    /* fromSLL                    = fromPP->dataModelVersions; */
    /* toSLL                      = &((*toPP)->dataModelVersions); */
    /* while(fromSLL) { */
    /* 	(*toSLL)               = CloneOCStringLL(fromSLL); */
    /* 	toSLL                  = &((*toSLL)->next); */
    /* 	fromSLL                = fromSLL->next; */
    /* } */

    /* /\* types *\/ */
    /* fromSLL                    = fromPP->types; */
    /* toSLL                      = &((*toPP)->types); */
    /* while(fromSLL) { */
    /* 	(*toSLL)               = CloneOCStringLL(fromSLL); */
    /* 	toSLL                  = &((*toSLL)->next); */
    /* 	fromSLL                = fromSLL->next; */
    /* } */

    /* /\* interfaces *\/ */
    /* fromSLL                    = fromPP->interfaces; */
    /* toSLL                      = &((*toPP)->interfaces); */
    /* while(fromSLL) { */
    /* 	(*toSLL)               = CloneOCStringLL(fromSLL); */
    /* 	toSLL                  = &((*toSLL)->next); */
    /* 	fromSLL                = fromSLL->next; */
    /* } */
    OIC_LOG_V(DEBUG, TAG, "%s:%d EXIT", __func__, __LINE__);
}

/**
 * store discovered resources in local database
 *
 * FIXME: do we need to do this? why not just retain a reference to the root resource?
 *
 */
void register_resources(OCClientResponse* c_OCClientResponse)
{
    OIC_LOG_V(DEBUG, __FILE__, "[%d] %s: ENTRY", __LINE__, __func__);

    if (c_OCClientResponse->payload->type != PAYLOAD_TYPE_DISCOVERY) {
	OIC_LOG_V(FATAL, __FILE__,
		  "FATAL %s %d (%s): wrong payload type\n", __FILE__, __LINE__,__func__);
	exit(1);
    }

    OIC_LOG_V(DEBUG, __FILE__, "[%d] resource count: %ld", __LINE__,
	      OCDiscoveryPayloadGetResourceCount((OCDiscoveryPayload*)c_OCClientResponse->payload));

    /* OBSOLETE OIC_LOG_DISCOVERY_RESPONSE(DEBUG, __FILE__, c_OCClientResponse); */

    /* Incoming response record is allocated on the stack of the
       calling routine, so it will be removed after we return.  To
       keep it around we need to make a deep copy. */
    //FIXME: leak
    g_Resources = (OCClientResponse *)OICCalloc(1, sizeof (OCClientResponse));
    memcpy(g_Resources, c_OCClientResponse, sizeof (OCClientResponse));

    g_Resources->addr = &(g_Resources->devAddr);
    /* copy the uripath string */
    /* g_Resources->resourceUri = */
    g_Resources->resourceUri = OICStrdup(c_OCClientResponse->resourceUri);

    /* copy payload; NB: fromDP is a linked list */
    OCDiscoveryPayload  *fromDP = (OCDiscoveryPayload*)c_OCClientResponse->payload;
    OCDiscoveryPayload **toDP   = (OCDiscoveryPayload**) &(g_Resources->payload);

    OCStringLL *fromSLL, **toSLL; // , *toSLLHead;
    while(fromDP) {
	OIC_LOG_V(DEBUG, __FILE__, "[%d] discovery payload sid: %s", __LINE__, fromDP->sid);
	OIC_LOG_V(DEBUG, __FILE__, "[%d] discovery payload name: %s", __LINE__, fromDP->name);

	/* 1. create new OCDiscoveryPayload */
	(*toDP)                    = (OCDiscoveryPayload*)OCDiscoveryPayloadCreate();

	/* 1a. copy fromDP sid, baseURI, name, uri strings */
	(*toDP)->sid               = OICStrdup(fromDP->sid);

	/*GAR baseURI removed from OCDiscoveryPayload in 1.3
	(*toDP)->baseURI           = OICStrdup(fromDP->baseURI);
	*/
	OIC_LOG_V(DEBUG, __FILE__, "3AAAAAAAAAAAAAAAA");
	(*toDP)->name              = OICStrdup(fromDP->name);

	/*GAR uri removed from OCDiscoveryPayload in 1.3
	(*toDP)->uri               = OICStrdup(fromDP->uri);
	*/
	OIC_LOG_V(DEBUG, __FILE__, "4AAAAAAAAAAAAAAAA");

	/* 1b. copy fromDP types and interfaces */
	/* types */
	// if (fromDP->type)  // may be null; why is it there?
	fromSLL                    = fromDP->type;
	toSLL                      = &((*toDP)->type);
	while(fromSLL) {
	    OIC_LOG_V(DEBUG, __FILE__, "[%d] discovery payload type: %s", __LINE__, fromDP->type->value);
	    (*toSLL)               = CloneOCStringLL(fromSLL);
	    toSLL                  = &((*toSLL)->next);
	    fromSLL                = fromSLL->next;
	}

	OIC_LOG_V(DEBUG, __FILE__, "BAAAAAAAAAAAAAAAA");
	/* interfaces */
	// if (fromDP->iface)  // may be null; why is it there?
	fromSLL                    = fromDP->iface;
	toSLL                      = &((*toDP)->iface);
	while(fromSLL != 0x0) {
	    OIC_LOG_V(DEBUG, __FILE__, "[%d] discovery payload interface: %s", __LINE__, fromDP->iface->value);
	    (*toSLL)               = CloneOCStringLL(fromSLL);
	    toSLL                  = &((*toSLL)->next);
	    fromSLL                = fromSLL->next;
	}

	OIC_LOG_V(DEBUG, __FILE__, "CAAAAAAAAAAAAAAAA");
	/* 1c. copy resources "This structure holds the old /oic/res response"
	   the resource info is here, not in the OCDiscoveryPayload */
	/* resources (i.e. OCResourcePayload list) */
	OCResourcePayload*  fromRP = fromDP->resources;
	OCResourcePayload **toRP   = &((*toDP)->resources);
	while(fromRP != 0x0) {
	    OIC_LOG_V(DEBUG, __FILE__, "[%d] resource uri: %s", __LINE__, fromRP->uri);

	    (*toRP)                = (OCResourcePayload*)OICCalloc(sizeof (OCResourcePayload), 1);
	    (*toRP)->uri           = OICStrdup(fromRP->uri);
	    (*toRP)->bitmap        = fromRP->bitmap;
	    (*toRP)->secure        = fromRP->secure;
	    (*toRP)->port          = fromRP->port;
#ifdef TCP_ADAPTER
	    (*toRP)->tcpPort       = fromRP->tcpPort;
#endif

	    /* resource types */
	    fromSLL                = fromRP->types;
	    toSLL                  = &((*toRP)->types);
	    while(fromSLL) {
		(*toSLL)           = CloneOCStringLL(fromSLL);
		toSLL              = &((*toSLL)->next);
		fromSLL            = fromSLL->next;
	    }
	    /* interfaces */
	    fromSLL                = fromRP->interfaces;
	    toSLL                  = &((*toRP)->interfaces);
	    while(fromSLL) {
		(*toSLL)           = CloneOCStringLL(fromSLL);
		toSLL              = &((*toSLL)->next);
		fromSLL            = fromSLL->next;
	    }
	    fromRP                 = fromRP->next;
	    toRP                   = &((*toRP)->next);
	}

	/* step to next DiscoveryPayload */
	fromDP = fromDP->next;
	toDP   = &((*toDP)->next);
    }
    OIC_LOG_V(DEBUG, __FILE__, "[%d] %s: EXIT", __LINE__, __func__);
}
