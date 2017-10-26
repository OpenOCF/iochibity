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

#include "openocf.h"

#include "gui.h"

#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#define TAG ("occlient")

pthread_t ocf_thread;
int gQuitFlag = 0;

FILE *logfd;

int log_msg(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    // printf(format, args);
    /* fprintf(logfd, "goodbye %s\n", "world"); */
    vfprintf(logfd, format, args);
    va_end(args);
}

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}

void print_json(OCDiscoveryPayload *payload)
{
    cJSON *root;
    cJSON *fmt;
    char  *rendered;
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
    fmt = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "format", fmt);
    cJSON_AddStringToObject(fmt, "type", "rect");
    cJSON_AddNumberToObject(fmt, "width", 1920);
    cJSON_AddNumberToObject(fmt, "height", 1080);
    cJSON_AddFalseToObject (fmt, "interlace");
    cJSON_AddNumberToObject(fmt, "frame rate", 24);
    rendered = cJSON_Print(root);
    OIC_LOG_V(INFO, TAG, "JSON:\n%s", rendered);
}

cJSON* links_to_json(OCClientResponse *msg) /* FIXME: split header logging from payload logging */
{
    cJSON *links;
    links = cJSON_CreateArray();
    bool OCF10;
    uint8_t vOptionData[MAX_HEADER_OPTION_DATA_LENGTH];
    size_t vOptionDataSize = sizeof(vOptionData);
    uint16_t actualDataSize = 0;
    OCGetHeaderOption(msg->rcvdVendorSpecificHeaderOptions,
		      msg->numRcvdVendorSpecificHeaderOptions,
		      OCF_CONTENT_FORMAT_VERSION,
		      vOptionData,
		      vOptionDataSize,
		      &actualDataSize);
    if (actualDataSize) {
	uint16_t content_format_version = (vOptionData[0] * 0x0100) + vOptionData[1];
	if (OC_SPEC_VERSION_VALUE == content_format_version)
	    {
		OCF10 = true;
	    }
    }
    OCResourcePayload* link = ( (OCDiscoveryPayload*)msg->payload)->resources;
    int i = 1;
    while (link) {
	cJSON *l = cJSON_CreateObject();
	/* Common Properties for all resources: rt, if, n, id */

	cJSON_AddNumberToObject(l, "ins", i); /* link instance (optional) */
	cJSON_AddItemToObject(l, "href", cJSON_CreateString(link->uri));

	/* Mandatory for discoverable resource (link): href, rt, if */
	cJSON *rts = cJSON_CreateArray();
	OCStringLL *rtype = link->types;
	while (rtype) {
	    cJSON_AddItemToArray(rts, cJSON_CreateString(rtype->value));
	    rtype = rtype->next;
	}
	cJSON_AddItemToObject(l, "rt", rts); /* rt = resource type */

	cJSON *ifs = cJSON_CreateArray();
	OCStringLL *iface = link->interfaces;
	while (iface) {
	    cJSON_AddItemToArray(ifs, cJSON_CreateString(iface->value));
	    iface = iface->next;
	}
	cJSON_AddItemToObject(l, "if", ifs); /* if = interface */

	cJSON_AddItemToObject(l, "n", cJSON_CreateNull()); /* name */
	cJSON_AddItemToObject(l, "id", cJSON_CreateNull()); /* resource identifier */

	cJSON_AddItemToObject(l, "di", cJSON_CreateNull()); /* device id */
	cJSON_AddItemToObject(l, "title", cJSON_CreateNull());
	cJSON_AddItemToObject(l, "type", cJSON_CreateArray()); /* media type */

	cJSON_AddItemToObject(l, "anchor", cJSON_CreateString(link->anchor));
	/* cJSON_AddItemToObject(l, "rel", cJSON_CreateString(link->rel)); */

	cJSON *policy = cJSON_CreateObject();
	cJSON_AddNumberToObject(policy, "bm", link->bitmap); /* bitmask: discoverable? observable? */
	/* legacy: "sec" and "port" not used for OCF 1.0, which uses eps instead */
	if ( !OCF10 ) {
	    cJSON_AddItemToObject(policy, "sec", cJSON_CreateBool(link->secure)); /* security */
	    cJSON_AddNumberToObject(policy, "port", link->port); /* secure port */
	}
	cJSON_AddItemToObject(l, "p", policy);		     /* policy */

	OCEndpointPayload *endpoint = link->eps;
	cJSON *eps = cJSON_CreateArray();
	int k = 1;
	while(endpoint) {
	    cJSON *ep = cJSON_CreateObject();
	    /* char port[INT_MAX + 1];
	     * snprintf(port, INT_MAX, "%d", endpoint->port); */
	    int eplen = strlen(endpoint->tps)
		+ 3		/* :// */
		+ strlen(endpoint->addr)
		+ 1; 		/* : */
	    char *epstring = malloc(eplen + 6); /* largest val for port is 5 chars (uint16) */
	    snprintf(epstring, eplen + 6, "%s://%s:%d", endpoint->tps, endpoint->addr, endpoint->port);
	    cJSON_AddItemToObject(ep, "ep", cJSON_CreateString(epstring));
	    free(epstring);
	    /* cJSON_AddItemToObject(ep, "tps", cJSON_CreateString(endpoint->tps));
	     * cJSON_AddItemToObject(ep, "addr", cJSON_CreateString(endpoint->addr));
	     * cJSON_AddNumberToObject(ep, "port", endpoint->port); */
	    cJSON_AddNumberToObject(ep, "pri", endpoint->pri);
	    cJSON_AddItemToArray(eps, ep);
	    endpoint = endpoint->next;
	    k++;
	}
	cJSON_AddItemToObject(l, "eps", eps);
	cJSON_AddItemToArray(links, l);
	link = link->next;
	i++;
    }
    return links;
    /*
     * OIC_LOG_V(INFO, TAG, "Resource payload rel: %s", res->rel);
     * OIC_LOG_V(INFO, TAG, "Resource payload port: %d", res->port);
     * OIC_LOG_V(INFO, TAG, "Resource ep tps: %s", res->eps->tps);
     * OIC_LOG_V(INFO, TAG, "Resource ep addr: %s", res->eps->addr);
     * OIC_LOG_V(INFO, TAG, "Resource ep port: %d", res->eps->port); */
}

cJSON* discovery_to_json(OCClientResponse *msg)
{
    OCDiscoveryPayload *payload = (OCDiscoveryPayload*)msg->payload;
    cJSON *root;
    cJSON *links;
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "href", cJSON_CreateString("oic/res"));
    const char *ts[1];
    ts[0] = "oic.wk.res";
    cJSON_AddItemToObject(root, "rt", cJSON_CreateStringArray(ts, 1));
    const char *ifaces[2];
    ifaces[0] = "oic.if.baseline";
    ifaces[1] = "oic.if.ll";
    cJSON_AddItemToObject(root, "if", cJSON_CreateStringArray(ifaces, 2));
    cJSON_AddItemToObject(root, "n", payload->name?
			  cJSON_CreateString(payload->sid) : cJSON_CreateNull());
    cJSON_AddItemToObject(root, "mpro", cJSON_CreateNull());
    cJSON_AddItemToObject(root, "di", cJSON_CreateString(payload->sid));

    /* OIC_LOG_V(INFO, TAG, "Discovery payload type: %s",
     * 	      (pay->type == NULL) ? "(null)" : pay->type->value);
     *
     * OIC_LOG_V(INFO, TAG, "Discovery payload iface: %s",
     * 	      (pay->iface == NULL) ? "(null)" : pay->iface->value); */

    links = links_to_json(msg);
    cJSON_AddItemToObject(root, "links", links);

    return root;
}

void log_header_options (OCClientResponse *clientResponse)
{
    /* COAP option numbers enumerated in libcoap pdu.h; except for the
       OCF custom ones: OCF-Content-Format-Version (2049) and
       OCF-Accept-Content-Format-Version (2053) */
    /* See RFC 7252, section 5.10 for option definitions */

    uint16_t content_format = 0;
    uint16_t content_format_version = 0;
    uint16_t option_id  = 0;
    uint16_t option_len = 0;
    for (int i=0; i < clientResponse->numRcvdVendorSpecificHeaderOptions; i++) {
	OIC_LOG_V(INFO, TAG, "\tOption %d:", i);

	/* With OCF 1.0, the version of the content (payload) format must also be negotiated.
	 * OCF 1.0 section 12.2.5 says messages must always indicate format version etc..
	 * COAP_OPTION_ACCEPT_VERSION 2049 = OCF-Accept-Content-Format-Version (set by client requesting payload)
	 * COAP_OPTION_CONTENT_VERSION 2053 = OCF-Content-Format-Version (set if msg has payload)
	 * Values:  2048 = 0x0800 = OCF version 1.0; 2112 = 0x0840 = OIC version 1.1 */

	/* So we expect two headers, one to indicate the payload format, and
	   another to indicate format version */

	OIC_LOG_V(INFO, TAG, "\t\t protocol id FIXME: %d",
		  clientResponse->rcvdVendorSpecificHeaderOptions[i].protocolID);

	option_id = clientResponse->rcvdVendorSpecificHeaderOptions[i].optionID;
	option_len = clientResponse->rcvdVendorSpecificHeaderOptions[i].optionLength;
	switch (option_id) {
	case  COAP_OPTION_ACCEPT:
	    /* Client and server negotiate content (payload) format using COAP header options
	     * COAP_OPTION_ACCEPT 12 and COAP_OPTION_CONTENT_VERSION; options are:
	     * application/cbor 60 = 0x3C
	     * application/vnd.ocf+cbor 10000 = 0x2710
	     * application/json 50 = 0x32
	     * see: https://www.iana.org/assignments/core-parameters/core-parameters.xhtml */
	    OIC_LOG_V(INFO, TAG, "\t\t Accept (code %d)",
		      option_id);
	    /* uint */
	    break;
	case OCF_ACCEPT_CONTENT_FORMAT_VERSION:
	    OIC_LOG_V(INFO, TAG, "\t\t OCF-Accept-Content-Version-Format (code %d), len %d",
		      option_id, option_len);
	    /* 2 byte uint */
	    break;

	case  COAP_OPTION_CONTENT_FORMAT: /* uint */
	    content_format =
		(clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[0] * 0x0100
		 + clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[1]);
	    OIC_LOG_V(INFO, TAG, "\t\t Content-Format (%d) = %s (%d)",
		      option_id,
		      /* option_len, */
		      (COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR
		      == content_format)? "application/vnd.ocf+cbor"
		      : (COAP_MEDIATYPE_APPLICATION_CBOR
			 == content_format_version)? "application/cbor"
		      : (COAP_MEDIATYPE_APPLICATION_JSON
			 == content_format_version)? "application/json"
		      : "(UNKNOWN)",
		       content_format);
	    break;
	    /* duplicate of COAP_OPTION_CONTENT_FORMAT: COAP_OPTION_CONTENT_TYPE */

	case OCF_CONTENT_FORMAT_VERSION:
	    /* 2 byte uint */
	    content_format_version =
		(clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[0] * 0x0100
		 + clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[1]);
	    OIC_LOG_V(INFO, TAG, "\t\t OCF-Content-Version-Format (%d) = %s (%d)",
		      option_id,
		      /* option_len, */
		      (OCF_VERSION_1_0_0 == content_format_version)? "OCF 1.0.0"
		      : (OCF_VERSION_1_1_0 == content_format_version)? "OCF 1.1.0"
		      : "(UNKNOWN)",
		       content_format_version);
	    break;
	case  COAP_OPTION_IF_MATCH:
	    /* opaque */
	    OIC_LOG_V(INFO, TAG, "\t\t If-Match (code %d)", option_id);
	    break;
	case  COAP_OPTION_URI_HOST:
	    /* string */
	    OIC_LOG_V(INFO, TAG, "\t\t Uri-Host (code %d)", option_id);
	    break;
	case  COAP_OPTION_ETAG:
	    OIC_LOG_V(INFO, TAG, "\t\t ETag (code %d)",
		      option_id);
	    /* empty */
	    break;
	case  COAP_OPTION_IF_NONE_MATCH:
	    OIC_LOG_V(INFO, TAG, "\t\t If-None-Match (code %d)",
		      option_id);
	    /* empty */
	    break;
	case  COAP_OPTION_URI_PORT:
	    OIC_LOG_V(INFO, TAG, "\t\t Uri-Port (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_LOCATION_PATH:
	    OIC_LOG_V(INFO, TAG, "\t\t Location-Path (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_URI_PATH:
	    OIC_LOG_V(INFO, TAG, "\t\t Uri-Path (code %d)",
		      option_id);
	    /* string */
	    break;

	case  COAP_OPTION_MAXAGE:
	    OIC_LOG_V(INFO, TAG, "\t\t Max-Age (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_URI_QUERY:
	    OIC_LOG_V(INFO, TAG, "\t\t Uri-Query (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_LOCATION_QUERY:
	    OIC_LOG_V(INFO, TAG, "\t\t Location-Query (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_PROXY_URI:
	    OIC_LOG_V(INFO, TAG, "\t\t Proxy-Uri (code %d)",
		      option_id);
	    /* string */
	case  COAP_OPTION_PROXY_SCHEME:
	    OIC_LOG_V(INFO, TAG, "\t\t Proxy-Scheme (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_SIZE1:
	    OIC_LOG_V(INFO, TAG, "\t\t Size1 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_SIZE2:
	    OIC_LOG_V(INFO, TAG, "\t\t Size2 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_OBSERVE:
	    OIC_LOG_V(INFO, TAG, "\t\t Observe (code %d)",
		      option_id);
	    /* empty/uint */
	    break;
	    /* duplicate of COAP_OPTON_OBSERVE */
	    /* case  COAP_OPTION_SUBSCRIPTION:
	     *     OIC_LOG_V(INFO, TAG, "\t\t Observe (code %d)",
	     * 	  option_id);
	     *     /\* empty/uint *\/
	     *     break; */
	case  COAP_OPTION_BLOCK2:
	    OIC_LOG_V(INFO, TAG, "\t\t Block2 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_BLOCK1:
	    OIC_LOG_V(INFO, TAG, "\t\t Block1 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_MAX_OPT:
	    OIC_LOG_V(INFO, TAG, "\t\t Max-Opt (code %d)",
		      option_id);
	    break;
	default:
	    OIC_LOG_V(INFO, TAG, "\t\t UNKOWN (code %d)",
		      option_id);
	}
	/* for (int k = 0; k < clientResponse->rcvdVendorSpecificHeaderOptions[i].optionLength; k++) {
	 *     OIC_LOG_V(INFO, TAG, "\t\t datum[%d]: 0x%X", k,
	 * 	      clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[k]); */
    }
}

void log_endpoint_info(OCClientResponse *clientResponse)
{
    OIC_LOG_V(INFO, TAG, "Origin addr: %s:%d", clientResponse->devAddr.addr, clientResponse->devAddr.port);
    OIC_LOG_V(INFO, TAG, "Origin ifindex: %d", clientResponse->devAddr.ifindex);
    OIC_LOG_V(INFO, TAG, "Origin route data: %s", clientResponse->devAddr.routeData);
    OIC_LOG_V(INFO, TAG, "Origin device ID: %s", clientResponse->devAddr.remoteId);

    switch ( clientResponse->devAddr.adapter) {
    case OC_DEFAULT_ADAPTER: /** value zero indicates discovery.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: DEFAULT (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_IP:	/* (1 << 0) IPv4 and IPv6, including 6LoWPAN.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: UDP/IP (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_GATT_BTLE: /* (1 << 1) GATT over Bluetooth LE.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: GATT/BTLE (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_RFCOMM_BTEDR: /* (1 << 2) RFCOMM over Bluetooth EDR.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: RFCOMM/BLEDR (%d)", clientResponse->devAddr.adapter);
	break;
#ifdef RA_ADAPTER
    case OC_ADAPTER_REMOTE_ACCESS: /* (1 << 3) Remote Access over XMPP.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: XMPP (%d)", clientResponse->devAddr.adapter);
	break;
#endif
    case OC_ADAPTER_TCP:    /* (1 << 4) CoAP over TCP.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: TCP (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_NFC:    /* (1 << 5) NFC Transport for Messaging.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: NFC (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ALL_ADAPTERS: /* 0xffffffff CA_ALL_ADAPTERS */
	OIC_LOG_V(INFO, TAG, "Transport adapter: ALL (%d)", clientResponse->devAddr.adapter);
	break;
    default:
	break;
    }

    if ( OC_DEFAULT_FLAGS == clientResponse->devAddr.flags)
	OIC_LOG_V(INFO, TAG, "DEFAULT FLAGS (%d)", clientResponse->devAddr.flags);

    /** Insecure transport is the default (subject to change).*/
    /** secure the transport path*/
    if (OC_FLAG_SECURE & clientResponse->devAddr.flags) /* (1 << 4) */
	OIC_LOG_V(INFO, TAG, "Transport security: TRUE");
    else OIC_LOG_V(INFO, TAG, "Transport security: FALSE");

    /** IPv4 & IPv6 auto-selection is the default.*/
    /** if adapter = IP (UDP) or TCP*/
    OIC_LOG_V(INFO, TAG, "Network protocols: %s %s",
	      ( (OC_IP_USE_V4 & clientResponse->devAddr.flags) > 0)? /* (1 << 6) */
	      "IPv4" : "",
	      ( (OC_IP_USE_V6 & clientResponse->devAddr.flags) > 0)? /* (1 << 5) */
	      "IPv6" : "");

    OIC_LOG_V(INFO, TAG, "Transport flags: 0x%08X", clientResponse->devAddr.flags);

    /* OIC_LOG_V(INFO, TAG, "IPv6 Scopes: %s%s%s%s%s%s%s",
     * 	      ((OC_SCOPE_INTERFACE & clientResponse->devAddr.flags) > 0)? /\* 0x1 *\/
     * 	      "Interface-Local" : "",
     * 	      ((OC_SCOPE_LINK & clientResponse->devAddr.flags) > 0)? /\* 0x2 *\/
     * 	      ", Link-Local" : "",
     * 	      ((OC_SCOPE_REALM & clientResponse->devAddr.flags) > 0)? /\* 0x3 *\/
     * 	      ", Realm-Local" : "",
     * 	      ((OC_SCOPE_ADMIN & clientResponse->devAddr.flags) > 0)? /\* 0x4 *\/
     * 	      ", Admin-Local" : "",
     * 	      ((OC_SCOPE_SITE & clientResponse->devAddr.flags) > 0)? /\* 0x5 *\/
     * 	      ", Site-Local" : "",
     * 	      ((OC_SCOPE_ORG & clientResponse->devAddr.flags) > 0)? /\* 0x8 *\/
     * 	      ", Organization-Local" : "",
     * 	      ((OC_SCOPE_GLOBAL & clientResponse->devAddr.flags) > 0)? /\* 0xE *\/
     * 	      ", Global" : ""); */

    /* /\** if adapter = IP (UDP) or TCP*\/
     * if (OC_IP_USE_V4 & clientResponse->devAddr.flags) /\* (1 << 6) *\/
     * 	OIC_LOG_V(INFO, TAG, "Network protocol: IPv4"); */

    /** Multicast only.*/
    if (OC_MULTICAST & clientResponse->devAddr.flags) /* (1 << 7) */
	OIC_LOG_V(INFO, TAG, "Multicast? TRUE");
    else OIC_LOG_V(INFO, TAG, "Multicast? FALSE");

    OIC_LOG_V(INFO, TAG, "IPv6 Scopes:");
    /** Link-Local multicast is the default multicast scope for IPv6.
     *  These are placed here to correspond to the IPv6 multicast address bits.*/

    /** IPv6 Interface-Local scope (loopback).*/
    if (OC_SCOPE_INTERFACE & clientResponse->devAddr.flags) /* 0x1 */
    	OIC_LOG_V(INFO, TAG, "\tInterface-Local");

    /** IPv6 Link-Local scope (default).*/
    if (OC_SCOPE_LINK & clientResponse->devAddr.flags) /* 0x2 */
    	OIC_LOG_V(INFO, TAG, "\tLink-Local");

    /** IPv6 Realm-Local scope. */
    if (OC_SCOPE_REALM & clientResponse->devAddr.flags) /* 0x3 */
    	OIC_LOG_V(INFO, TAG, "\tRealm-Local");

    /** IPv6 Admin-Local scope. */
    if (OC_SCOPE_ADMIN & clientResponse->devAddr.flags) /* 0x4 */
    	OIC_LOG_V(INFO, TAG, "\tAdmin-Local");

    /** IPv6 Site-Local scope. */
    if (OC_SCOPE_SITE & clientResponse->devAddr.flags) /* 0x5 */
    	OIC_LOG_V(INFO, TAG, "\tSite-Local");

    /** IPv6 Organization-Local scope. */
    if (OC_SCOPE_ORG & clientResponse->devAddr.flags) /* 0x8 */
    	OIC_LOG_V(INFO, TAG, "\tOrganization-Local");

    /**IPv6 Global scope. */
    if (OC_SCOPE_GLOBAL & clientResponse->devAddr.flags) /* 0x# */
    	OIC_LOG_V(INFO, TAG, "\tGlobal");
}

void log_payload_type(OCPayload *payload)
{
    switch (payload->type) {
    case PAYLOAD_TYPE_INVALID:
	OIC_LOG_V(INFO, TAG, "Message payload type: INVALID");
	break;
    case PAYLOAD_TYPE_DISCOVERY:
	OIC_LOG_V(INFO, TAG, "Message payload type: DISCOVERY");
	break;
    case PAYLOAD_TYPE_DEVICE:
	OIC_LOG_V(INFO, TAG, "Message payload type: DEVICE");
	break;
    case PAYLOAD_TYPE_PLATFORM:
	OIC_LOG_V(INFO, TAG, "Message payload type: PLATFORM");
	break;
    case PAYLOAD_TYPE_REPRESENTATION:
	OIC_LOG_V(INFO, TAG, "Message payload type: REPRESENTATION");
	break;
    case PAYLOAD_TYPE_SECURITY:
	OIC_LOG_V(INFO, TAG, "Message payload type: SECURITY");
	break;
    case PAYLOAD_TYPE_PRESENCE:
	OIC_LOG_V(INFO, TAG, "Message payload type: PRESENCE");
	break;
    case PAYLOAD_TYPE_DIAGNOSTIC:
	OIC_LOG_V(INFO, TAG, "Message payload type: DIAGNOSTIC");
	break;
    case PAYLOAD_TYPE_INTROSPECTION:
	OIC_LOG_V(INFO, TAG, "Message payload type: INTROSPECTION");
	break;
    default:
	OIC_LOG_V(INFO, TAG, "Message payload type: UNKNOWN");
	break;
    }
}

void log_discovery_message(OCClientResponse *clientResponse)
{
    OIC_LOG(INFO, TAG, "================ Response Message ================");
    /* payload type should be 1 */
    log_payload_type(clientResponse->payload);
    OIC_LOG_V(INFO, TAG, "Message seq nbr: %d", clientResponse->sequenceNumber);
    OIC_LOG_V(INFO, TAG, "Origin uri: %s", clientResponse->resourceUri);
    OIC_LOG_V(INFO, TAG, "Origin Identity: %d %s", clientResponse->identity.id_length, clientResponse->identity.id);
    OIC_LOG_V(INFO, TAG, "Origin result: %d", clientResponse->result);

    log_endpoint_info(clientResponse);

    OIC_LOG_V(INFO, TAG, "Header Options (%d):", clientResponse->numRcvdVendorSpecificHeaderOptions);
    log_header_options(clientResponse);

    cJSON *discovery_json = discovery_to_json(clientResponse);
    char* rendered = cJSON_Print(discovery_json);
    OIC_LOG(INFO, TAG, "Discovery payload:\n");
    log_msg("%s\n", rendered);
    free(rendered);
}


// This is a function called back when resources are discovered
OCStackApplicationResult resource_discovery_cb(void* ctx,
						  OCDoHandle h,
						  OCClientResponse * clientResponse) {
    OIC_LOG(INFO, TAG, "Entering resource_discovery_cb (Application Layer CB)");

    log_discovery_message(clientResponse);

    //return OC_STACK_DELETE_TRANSACTION;
    return OC_STACK_KEEP_TRANSACTION | OC_STACK_KEEP_RESPONSE;
}

void discover_resources ()
{
    /* Start a discovery query*/
    OCCallbackData cbData;
    cbData.cb = resource_discovery_cb;
    cbData.context = NULL;
    cbData.cd = NULL;
    char szQueryUri[MAX_QUERY_LENGTH] = { 0 };
    strcpy(szQueryUri, OC_MULTICAST_DISCOVERY_URI);
    OIC_LOG_V(INFO, TAG, "Starting Discovery >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    if (OCDoResource(NULL,
		     OC_REST_DISCOVER,
		     szQueryUri,
		     NULL, NULL,
		     CT_DEFAULT,
		     OC_LOW_QOS,
		     &cbData, NULL, 0) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }
}

void list_resource_uris ()
{
    char **ruris = oocf_cosp_list_resource_uris();
    if (NULL == ruris)
	OIC_LOG(DEBUG, TAG, "oocf_cosp_list_resource_uris fail");

    /* OIC_LOG_V(DEBUG, TAG, "ruris: %p", ruris); */

    while (*ruris) {
    	OIC_LOG_V(DEBUG, TAG, "Resource URI %s", *ruris);
    	ruris++;
    }
}

void* ocf_routine(void *arg) {
    OIC_LOG_V(INFO, TAG, "Starting occlient");

    /* Initialize OCStack*/
    if (OCInit(NULL, 0, OC_CLIENT) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }



    // Break from loop with Ctrl+C
    OIC_LOG(INFO, TAG, "Entering occlient main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag) {

        if (OCProcess() != OC_STACK_OK) {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }
	fflush(logfd);		/* FIXME */
        sleep(1);
    }

    OIC_LOG(INFO, TAG, "Exiting occlient main loop...");

    if (OCStop() != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
    }

    return 0;
}

int main ()
{
    logfd = fopen("openocf.log", "w");
    /* fprintf(logfd, "hello %s\n", "world"); */
    oocf_log_hook_stdout(log_msg);
    fflush(logfd);

    int err = pthread_create(&(ocf_thread), NULL, &ocf_routine, NULL);
    if (err != 0)
	log_msg("\nCan't create OCF thread :[%s]", strerror(err));
    else
	log_msg("\n OCF thread created successfully\n");

    run_gui();
    fclose(logfd);
}
