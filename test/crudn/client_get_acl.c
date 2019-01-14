/* Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http:*www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "openocf.h"

#include "cJSON.h"
#include "coap/pdu.h"

#include <limits.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdarg.h>
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

#define TAG "client"

#define CLOG_MAIN
#include "clog.h"
const int MY_LOGGER = 0; /* Unique identifier for logger */

/* static oc_thread ocf_thread; */

int context = 1;

bool gQuitFlag = 0;

bool gError = 0;

//FILE *logfd;


/* void log_msg(const char *format, ...) */
/* { */
/*     va_list args; */

/*     va_start(args, format); */
/*     // printf(format, args); */
/*     /\* fprintf(logfd, "goodbye %s\n", "world"); *\/ */
/*     vfprintf(logfd, format, args); */
/*     va_end(args); */
/* } */

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    clog_info(CLOG(MY_LOGGER), "%s ENTRY", __func__);

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
    //clog_info(CLOG(MY_LOGGER), "JSON:\n%s", rendered);
    clog_info(CLOG(MY_LOGGER), "JSON:\n%s", rendered);
}

cJSON* links_to_json(struct oocf_inbound_response *msg) /* FIXME: split header logging from payload logging */
{
    cJSON *links;
    links = cJSON_CreateArray();
    bool OCF10;
    uint8_t vOptionData[MAX_HEADER_OPTION_DATA_LENGTH];
    size_t vOptionDataSize = sizeof(vOptionData);
    uint16_t actualDataSize = 0;
    OCGetHeaderOption(msg->rcvdVendorSpecificHeaderOptions,
		      msg->numRcvdVendorSpecificHeaderOptions,
		      OCF_OPTION_CONTENT_FORMAT_VERSION,
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
	    //snprintf(epstring, eplen + 6, "%s://[%s]:%d", endpoint->tps, endpoint->addr, endpoint->port);
	    snprintf(epstring, eplen + 6, "%s", OCCreateEndpointString(endpoint));
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
     * clog_info(CLOG(MY_LOGGER), "Resource payload rel: %s", res->rel);
     * clog_info(CLOG(MY_LOGGER), "Resource payload port: %d", res->port);
     * clog_info(CLOG(MY_LOGGER), "Resource ep tps: %s", res->eps->tps);
     * clog_info(CLOG(MY_LOGGER), "Resource ep addr: %s", res->eps->addr);
     * clog_info(CLOG(MY_LOGGER), "Resource ep port: %d", res->eps->port); */
}

cJSON* discovery_to_json(struct oocf_inbound_response *msg)
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

    /* clog_info(CLOG(MY_LOGGER), "Discovery payload type: %s",
     * 	      (pay->type == NULL) ? "(null)" : pay->type->value);
     *
     * clog_info(CLOG(MY_LOGGER), "Discovery payload iface: %s",
     * 	      (pay->iface == NULL) ? "(null)" : pay->iface->value); */

    links = links_to_json(msg);
    cJSON_AddItemToObject(root, "links", links);

    return root;
}

void log_header_options (struct oocf_inbound_response  *clientResponse)
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
	clog_info(CLOG(MY_LOGGER), "\tOption %d:", i);

	/* With OCF 1.0, the version of the content (payload) format must also be negotiated.
	 * OCF 1.0 section 12.2.5 says messages must always indicate format version etc..
	 * COAP_OPTION_ACCEPT_VERSION 2049 = OCF-Accept-Content-Format-Version (set by client requesting payload)
	 * COAP_OPTION_CONTENT_VERSION 2053 = OCF-Content-Format-Version (set if msg has payload)
	 * Values:  2048 = 0x0800 = OCF version 1.0; 2112 = 0x0840 = OIC version 1.1 */

	/* So we expect two headers, one to indicate the payload format, and
	   another to indicate format version */

	/* clog_info(CLOG(MY_LOGGER), "\t\t protocol id FIXME: %d", */
	/* 	  clientResponse->rcvdVendorSpecificHeaderOptions[i].protocolID); */

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
	    clog_info(CLOG(MY_LOGGER), "\t\t Accept (code %d)",
		      option_id);
	    /* uint */
	    break;
	case OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION:
	    clog_info(CLOG(MY_LOGGER), "\t\t OCF-Accept-Content-Version-Format (code %d), len %d",
		      option_id, option_len);
	    /* 2 byte uint */
	    break;

	case  COAP_OPTION_CONTENT_FORMAT: /* uint */
	    content_format =
		(clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[0] * 0x0100
		 + clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[1]);
	    clog_info(CLOG(MY_LOGGER), "\t\t Content-Format (%d) = %s (%d)",
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

	case OCF_OPTION_CONTENT_FORMAT_VERSION:
	    /* 2 byte uint */
	    content_format_version =
		(clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[0] * 0x0100
		 + clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[1]);
	    clog_info(CLOG(MY_LOGGER), "\t\t OCF-Content-Version-Format (%d) = %s (%d)",
		      option_id,
		      /* option_len, */
		      (OCF_VERSION_1_0_0 == content_format_version)? "OCF 1.0.0"
		      : (OCF_VERSION_1_1_0 == content_format_version)? "OCF 1.1.0"
		      : "(UNKNOWN)",
		       content_format_version);
	    break;
	case  COAP_OPTION_IF_MATCH:
	    /* opaque */
	    clog_info(CLOG(MY_LOGGER), "\t\t If-Match (code %d)", option_id);
	    break;
	case  COAP_OPTION_URI_HOST:
	    /* string */
	    clog_info(CLOG(MY_LOGGER), "\t\t Uri-Host (code %d)", option_id);
	    break;
	case  COAP_OPTION_ETAG:
	    clog_info(CLOG(MY_LOGGER), "\t\t ETag (code %d)",
		      option_id);
	    /* empty */
	    break;
	case  COAP_OPTION_IF_NONE_MATCH:
	    clog_info(CLOG(MY_LOGGER), "\t\t If-None-Match (code %d)",
		      option_id);
	    /* empty */
	    break;
	case  COAP_OPTION_URI_PORT:
	    clog_info(CLOG(MY_LOGGER), "\t\t Uri-Port (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_LOCATION_PATH:
	    clog_info(CLOG(MY_LOGGER), "\t\t Location-Path (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_URI_PATH:
	    clog_info(CLOG(MY_LOGGER), "\t\t Uri-Path (code %d)",
		      option_id);
	    /* string */
	    break;

	case  COAP_OPTION_MAXAGE:
	    clog_info(CLOG(MY_LOGGER), "\t\t Max-Age (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_URI_QUERY:
	    clog_info(CLOG(MY_LOGGER), "\t\t Uri-Query (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_LOCATION_QUERY:
	    clog_info(CLOG(MY_LOGGER), "\t\t Location-Query (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_PROXY_URI:
	    clog_info(CLOG(MY_LOGGER), "\t\t Proxy-Uri (code %d)",
		      option_id);
	    /* string */
	case  COAP_OPTION_PROXY_SCHEME:
	    clog_info(CLOG(MY_LOGGER), "\t\t Proxy-Scheme (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_SIZE1:
	    clog_info(CLOG(MY_LOGGER), "\t\t Size1 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_SIZE2:
	    clog_info(CLOG(MY_LOGGER), "\t\t Size2 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_OBSERVE:
	    clog_info(CLOG(MY_LOGGER), "\t\t Observe (code %d)",
		      option_id);
	    /* empty/uint */
	    break;
	    /* duplicate of COAP_OPTON_OBSERVE */
	    /* case  COAP_OPTION_SUBSCRIPTION:
	     *     clog_info(CLOG(MY_LOGGER), "\t\t Observe (code %d)",
	     * 	  option_id);
	     *     /\* empty/uint *\/
	     *     break; */
	case  COAP_OPTION_BLOCK2:
	    clog_info(CLOG(MY_LOGGER), "\t\t Block2 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_BLOCK1:
	    clog_info(CLOG(MY_LOGGER), "\t\t Block1 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_MAX_OPT:
	    clog_info(CLOG(MY_LOGGER), "\t\t Max-Opt (code %d)",
		      option_id);
	    break;
	default:
	    clog_info(CLOG(MY_LOGGER), "\t\t UNKOWN (code %d)",
		      option_id);
	}
	/* for (int k = 0; k < clientResponse->rcvdVendorSpecificHeaderOptions[i].optionLength; k++) {
	 *     clog_info(CLOG(MY_LOGGER), "\t\t datum[%d]: 0x%X", k,
	 * 	      clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[k]); */
    }
}

void log_endpoint_info(struct oocf_inbound_response  *clientResponse)
{
    clog_info(CLOG(MY_LOGGER), "Origin addr: %s:%d", clientResponse->devAddr.addr, clientResponse->devAddr.port);
    clog_info(CLOG(MY_LOGGER), "Origin ifindex: %d", clientResponse->devAddr.ifindex);
    clog_info(CLOG(MY_LOGGER), "Origin route data: %s", clientResponse->devAddr.routeData);
    clog_info(CLOG(MY_LOGGER), "Origin device ID: %s", clientResponse->devAddr.remoteId);

    switch ( clientResponse->devAddr.adapter) {
    case OC_DEFAULT_ADAPTER: /** value zero indicates discovery.*/
	clog_info(CLOG(MY_LOGGER), "Transport adapter: DEFAULT (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_IP:	/* (1 << 0) IPv4 and IPv6, including 6LoWPAN.*/
	clog_info(CLOG(MY_LOGGER), "Transport adapter: UDP/IP (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_GATT_BTLE: /* (1 << 1) GATT over Bluetooth LE.*/
	clog_info(CLOG(MY_LOGGER), "Transport adapter: GATT/BTLE (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_RFCOMM_BTEDR: /* (1 << 2) RFCOMM over Bluetooth EDR.*/
	clog_info(CLOG(MY_LOGGER), "Transport adapter: RFCOMM/BLEDR (%d)", clientResponse->devAddr.adapter);
	break;
#ifdef RA_ADAPTER
    case OC_ADAPTER_REMOTE_ACCESS: /* (1 << 3) Remote Access over XMPP.*/
	clog_info(CLOG(MY_LOGGER), "Transport adapter: XMPP (%d)", clientResponse->devAddr.adapter);
	break;
#endif
    case OC_ADAPTER_TCP:    /* (1 << 4) CoAP over TCP.*/
	clog_info(CLOG(MY_LOGGER), "Transport adapter: TCP (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_NFC:    /* (1 << 5) NFC Transport for Messaging.*/
	clog_info(CLOG(MY_LOGGER), "Transport adapter: NFC (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ALL_ADAPTERS: /* 0xffffffff CA_ALL_ADAPTERS */
	clog_info(CLOG(MY_LOGGER), "Transport adapter: ALL (%d)", clientResponse->devAddr.adapter);
	break;
    default:
	break;
    }

    if ( OC_DEFAULT_FLAGS == clientResponse->devAddr.flags)
	clog_info(CLOG(MY_LOGGER), "DEFAULT FLAGS (%d)", clientResponse->devAddr.flags);

    /** Insecure transport is the default (subject to change).*/
    /** secure the transport path*/
    if (OC_FLAG_SECURE & clientResponse->devAddr.flags) /* (1 << 4) */
	clog_info(CLOG(MY_LOGGER), "Transport security: TRUE");
    else clog_info(CLOG(MY_LOGGER), "Transport security: FALSE");

    /** IPv4 & IPv6 auto-selection is the default.*/
    /** if adapter = IP (UDP) or TCP*/
    clog_info(CLOG(MY_LOGGER), "Network protocols: %s %s",
	      ( (OC_IP_USE_V4 & clientResponse->devAddr.flags) > 0)? /* (1 << 6) */
	      "IPv4" : "",
	      ( (OC_IP_USE_V6 & clientResponse->devAddr.flags) > 0)? /* (1 << 5) */
	      "IPv6" : "");

    clog_info(CLOG(MY_LOGGER), "Transport flags: 0x%08X", clientResponse->devAddr.flags);

    /* clog_info(CLOG(MY_LOGGER), "IPv6 Scopes: %s%s%s%s%s%s%s",
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
     * 	clog_info(CLOG(MY_LOGGER), "Network protocol: IPv4"); */

    /** Multicast only.*/
    if (OC_MULTICAST & clientResponse->devAddr.flags) /* (1 << 7) */
	clog_info(CLOG(MY_LOGGER), "Multicast? TRUE");
    else clog_info(CLOG(MY_LOGGER), "Multicast? FALSE");

    clog_info(CLOG(MY_LOGGER), "IPv6 Scopes:");
    /** Link-Local multicast is the default multicast scope for IPv6.
     *  These are placed here to correspond to the IPv6 multicast address bits.*/

    /** IPv6 Interface-Local scope (loopback).*/
    if (OC_SCOPE_INTERFACE & clientResponse->devAddr.flags) /* 0x1 */
    	clog_info(CLOG(MY_LOGGER), "\tInterface-Local");

    /** IPv6 Link-Local scope (default).*/
    if (OC_SCOPE_LINK & clientResponse->devAddr.flags) /* 0x2 */
    	clog_info(CLOG(MY_LOGGER), "\tLink-Local");

    /** IPv6 Realm-Local scope. */
    if (OC_SCOPE_REALM & clientResponse->devAddr.flags) /* 0x3 */
    	clog_info(CLOG(MY_LOGGER), "\tRealm-Local");

    /** IPv6 Admin-Local scope. */
    if (OC_SCOPE_ADMIN & clientResponse->devAddr.flags) /* 0x4 */
    	clog_info(CLOG(MY_LOGGER), "\tAdmin-Local");

    /** IPv6 Site-Local scope. */
    if (OC_SCOPE_SITE & clientResponse->devAddr.flags) /* 0x5 */
    	clog_info(CLOG(MY_LOGGER), "\tSite-Local");

    /** IPv6 Organization-Local scope. */
    if (OC_SCOPE_ORG & clientResponse->devAddr.flags) /* 0x8 */
    	clog_info(CLOG(MY_LOGGER), "\tOrganization-Local");

    /**IPv6 Global scope. */
    if (OC_SCOPE_GLOBAL & clientResponse->devAddr.flags) /* 0x# */
    	clog_info(CLOG(MY_LOGGER), "\tGlobal");
}

void log_payload_type(OCPayload *payload)
{
    switch (payload->type) {
    case PAYLOAD_TYPE_INVALID:
	clog_info(CLOG(MY_LOGGER), "Message payload type: INVALID");
	break;
    case PAYLOAD_TYPE_DISCOVERY:
	clog_info(CLOG(MY_LOGGER), "Message payload type: DISCOVERY");
	break;
    case PAYLOAD_TYPE_DEVICE:
	clog_info(CLOG(MY_LOGGER), "Message payload type: DEVICE");
	break;
    case PAYLOAD_TYPE_PLATFORM:
	clog_info(CLOG(MY_LOGGER), "Message payload type: PLATFORM");
	break;
    case PAYLOAD_TYPE_REPRESENTATION:
	clog_info(CLOG(MY_LOGGER), "Message payload type: REPRESENTATION");
	break;
    case PAYLOAD_TYPE_SECURITY:
	clog_info(CLOG(MY_LOGGER), "Message payload type: SECURITY");
	break;
    /* case PAYLOAD_TYPE_PRESENCE: */
    /*     clog_info(CLOG(MY_LOGGER), "Message payload type: PRESENCE"); */
    /*     break; */
    case PAYLOAD_TYPE_DIAGNOSTIC:
	clog_info(CLOG(MY_LOGGER), "Message payload type: DIAGNOSTIC");
	break;
    case PAYLOAD_TYPE_INTROSPECTION:
	clog_info(CLOG(MY_LOGGER), "Message payload type: INTROSPECTION");
	break;
    default:
	clog_info(CLOG(MY_LOGGER), "Message payload type: UNKNOWN");
	break;
    }
}

void log_discovery_message(struct oocf_inbound_response  *clientResponse)
{
    clog_info(CLOG(MY_LOGGER), "================ Response Message ================");
    /* payload type should be 1 */
    log_payload_type(clientResponse->payload);
    clog_info(CLOG(MY_LOGGER), "Message seq nbr: %d", clientResponse->sequenceNumber);
    clog_info(CLOG(MY_LOGGER), "Origin uri: %s", clientResponse->resourceUri);
    clog_info(CLOG(MY_LOGGER), "Origin Identity: %d %s", clientResponse->identity.id_length, clientResponse->identity.id);
    clog_info(CLOG(MY_LOGGER), "Origin result: %d", clientResponse->result);

    log_endpoint_info(clientResponse);

    clog_info(CLOG(MY_LOGGER), "Header Options (%d):", clientResponse->numRcvdVendorSpecificHeaderOptions);
    log_header_options(clientResponse);

    cJSON *discovery_json = discovery_to_json(clientResponse);
    char* rendered = cJSON_Print(discovery_json);
    /* clog_info(CLOG(MY_LOGGER), "Discovery payload:\n"); */
    //    OIC_LOG_STR(DEBUG, TAG, "
    clog_info(CLOG(MY_LOGGER), "Discovery payload: %s\n", rendered);

    free(rendered);
}

OCStackApplicationResult react_to_get_acl2(void* ctx, OCDoHandle handle,
                                           oocf_inbound_response * inbound_response)
{
    if (inbound_response == NULL) {
        clog_info(CLOG(MY_LOGGER), "react_to_get received NULL inbound_response");
        return   OC_STACK_DELETE_TRANSACTION;
    }

    if (ctx == (void*)&context)
    {

    }

    if (inbound_response->result != OC_STACK_OK) { // CA_CONTENT) {
        clog_info(CLOG(MY_LOGGER), "Response Error %u", inbound_response->result);
        exit(EXIT_FAILURE);
    } else {
        clog_info(CLOG(MY_LOGGER), "received response to request: GET /oic/sec/acl2");
    }

    clog_info(CLOG(MY_LOGGER), "SEQUENCE NUMBER: %d", inbound_response->sequenceNumber);
    //OIC_LOG_PAYLOAD(INFO, inbound_response->payload);
    clog_info(CLOG(MY_LOGGER), ("=============> Get Response"));

    if (inbound_response->numRcvdVendorSpecificHeaderOptions > 0)
    {
        clog_info(CLOG(MY_LOGGER), "Received CoAP options:");
        uint8_t i = 0;
        struct oocf_coap_options *rcvdOptions /* array */ = inbound_response->rcvdVendorSpecificHeaderOptions;
        for( i = 0; i < inbound_response->numRcvdVendorSpecificHeaderOptions; i++) {
            clog_info(CLOG(MY_LOGGER),
                      "Received CoAP option ID: %u == %s",
                      ((CAHeaderOption_t)rcvdOptions[i]).optionID,
                      log_coap_option_string(rcvdOptions, i));

            uint16_t val = rcvdOptions[i].optionData[0] * 256
                + rcvdOptions[i].optionData[1];
            char *valstr = log_coap_option_value_string(rcvdOptions, i, val);

            clog_info(CLOG(MY_LOGGER), "Received CoAP option value: %u == %s",
                      val, valstr);
        }
    }
    gQuitFlag = 1;
    return OC_STACK_DELETE_TRANSACTION;
}

void get_acl(OCDiscoveryPayload* discovery_payload, CATransportAdapter_t transport)
{
    if (!discovery_payload)
    {
        clog_info(CLOG(MY_LOGGER), "discovery payload is NULL!!!");
        return;
    }

    OCDevAddr dest_ep;
    memset(&dest_ep, 0, sizeof(dest_ep));
    dest_ep.adapter = transport;
    dest_ep.flags = CA_IPV6 | CA_SECURE;

    OCResourcePayload* discovered_resource = discovery_payload->resources;
    bool found = false;
    OCEndpointPayload *eps;
    char *acl_path = "/oic/sec/acl2";
    char resource_url[MAX_URI_QUERY];

    while (discovered_resource)
    {
        if (strcmp(discovered_resource->uri, acl_path) == 0) {
            clog_info(CLOG(MY_LOGGER), "discovered /oic/sec/acl resource");
            /* use highest priority ep (ipv6 link local in OpenOCF) */
            eps = discovered_resource->eps;
            while (eps) {
                if (eps->pri == 1) { /* linklocal */
                    clog_info(CLOG(MY_LOGGER), "found ep with pri=1: %s", OCCreateEndpointString(eps));
                    clog_info(CLOG(MY_LOGGER), "addr family: 0x%04x", eps->family);
                    clog_info(CLOG(MY_LOGGER), "scope is linklocal? %d", oocf_addr_is_scoped(eps->addr, CA_SCOPE_LINK));
                    if ((strcmp(eps->tps, (transport == OOCF_UDP)? "coaps" : "coap+tcp") == 0)
                        && (strlen(eps->addr) < INET6_ADDRSTRLEN)
                        && (eps->family & CA_IPV6)
                        && oocf_addr_is_scoped(eps->addr, CA_SCOPE_LINK)) {
                        clog_info(CLOG(MY_LOGGER), "found ep with ipv6, link scope");
                        dest_ep.port = eps->port;
                        memcpy(dest_ep.addr, eps->addr, sizeof(dest_ep.addr));
                        sprintf(resource_url,
                                "%s%s", OCCreateEndpointString(eps), acl_path);
                        found =true;
                        break;
                    }
                }
                eps = eps->next;
            }
        }
        discovered_resource = discovered_resource->next;
    }
    if (found) {
        clog_info(CLOG(MY_LOGGER), "sending GET request to:");
        clog_info(CLOG(MY_LOGGER), "%s", resource_url);
        clog_info(CLOG(MY_LOGGER), "dest_ep flags is 0x%04x", dest_ep.flags);
        clog_info(CLOG(MY_LOGGER), "dest_ep adapter is %d", dest_ep.adapter);

        OCDoHandle handle;
        OCCallbackData cbData;
        cbData.cb = react_to_get_acl2;
        cbData.context = (void*)&context;
        cbData.cd = NULL;

        OCMethod method = OC_REST_GET;

        OCStackResult ret = OCDoRequest(&handle,
                                        method,
                                        resource_url, // acl_path,
                                        &dest_ep,
                                        NULL, /* payload */
                                        CT_ADAPTER_IP,
                                        OC_LOW_QOS,
                                        &cbData,
                                        NULL, /* options */
                                        0);   /* option count */

        if (ret != OC_STACK_OK)
            {
                clog_error(CLOG(MY_LOGGER), "OCDoRequest returns error %d with method %d", ret, method);
                exit(EXIT_FAILURE);
            }

    }
    else
    {
        clog_info(CLOG(MY_LOGGER), "Endpoints infomation not found on given payload!!!");
        gError = true;
        gQuitFlag = 1;
    }
}

// OCClientResponseHandler
OCStackApplicationResult resource_discovery_cb(void* ctx,
                                               OCDoHandle h,
                                               struct oocf_inbound_response  *inbound_response) {

    clog_debug(CLOG(MY_LOGGER), "%s ENTRY, tid %d", __func__, pthread_self());

    if (inbound_response == NULL)
    {
        clog_info(CLOG(MY_LOGGER), "resource_discovery_cb received NULL inbound_response");
        return   OC_STACK_DELETE_TRANSACTION;
    }

    /* 1. update behaviors_observed_log */
    /*  */

    if (inbound_response->result == OC_STACK_OK) { // CA_CONTENT) { /* coap 205 CONTENT, for GET requests */
        clog_info(CLOG(MY_LOGGER), "received response to discovery request: GET /oic/res, res: %d",
                  inbound_response->result);
    } else {
        clog_info(CLOG(MY_LOGGER), "resource_discovery_cb error %u", inbound_response->result);
        exit(EXIT_FAILURE);
    }

    log_discovery_message(inbound_response);

    printf(":");
    get_acl((OCDiscoveryPayload*)inbound_response->payload, OOCF_UDP);

    clog_debug(CLOG(MY_LOGGER), "%s EXIT", __func__);
    return OC_STACK_DELETE_TRANSACTION;
}

void run()
{
    OCDoHandle handle;
    /* Start a discovery query*/
    OCCallbackData cbData;
    cbData.cb = resource_discovery_cb;
    cbData.context = NULL;
    cbData.cd = NULL;
    char szQueryUri[MAX_QUERY_LENGTH] = { 0 };
    strcpy(szQueryUri, OC_MULTICAST_DISCOVERY_URI);

    /* FIXME: coap header options */

    clog_info(CLOG(MY_LOGGER), "Starting Resource Discovery >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    /* OCDoResource is deprecated, use OCDoRequest */
    if (OCDoRequest(&handle,	      /* OCDoHandle */
		    OC_REST_DISCOVER, /* method */
		    szQueryUri,       /* request uri */
		    NULL,	      /* destination ep */
		    NULL,	      /* payload - caller must free */
		    CT_DEFAULT,	      /* connectivity type */
		    OC_LOW_QOS,	      /* qos */
		    &cbData,	      /* callback */
		    NULL,	      /* header options */
		    0		      /* nbr hdr options */
		    ) != OC_STACK_OK) {
        clog_error(CLOG(MY_LOGGER), "OCStack resource error");
    }
}

void list_resource_uris ()
{
    /* char **ruris = oocf_cosp_list_resource_uris(); */
    /* if (NULL == ruris) */
    /* 	OIC_LOG(DEBUG, TAG, "oocf_cosp_list_resource_uris fail"); */

    /* /\* clog_debug(CLOG(MY_LOGGER), "ruris: %p", ruris); *\/ */

    /* while (*ruris) { */
    /* 	clog_debug(CLOG(MY_LOGGER), "Resource URI %s", *ruris); */
    /* 	ruris++; */
    /* } */
}

void* ocf_runloop(void *arg) {
    clog_info(CLOG(MY_LOGGER), "Starting client");

    // Break from loop with Ctrl+C
    clog_info(CLOG(MY_LOGGER), "Entering client main loop...");
    signal(SIGINT, handleSigInt);
    int i = 0;

    while (!gQuitFlag) {
	clog_info(CLOG(MY_LOGGER), "process loop %d, tid %d", i++, pthread_self());
        if (OCProcess() != OC_STACK_OK) {
            clog_error(CLOG(MY_LOGGER), "OCStack process error");
            return 0;
        }
	//fflush(logfd);		/* FIXME */
        sleep(1);
    }

    clog_info(CLOG(MY_LOGGER), "Exiting client main loop...");

    if (OCStop() != OC_STACK_OK) {
        clog_error(CLOG(MY_LOGGER), "OCStack stop error");
    }
    clog_debug(CLOG(MY_LOGGER), "%s EXIT", __func__);
    return 0;
}

//static char SVR_CONFIG_FILE[] = "examples/discovery/client_config.cbor";
static char SVR_CONFIG_FILE[] = "test/crudn/client_config.cbor";

/* local fopen for svr database overrides default filename */
FILE* server_fopen(const char *path, const char *mode)
{
    clog_info(CLOG(MY_LOGGER), "%s path %s", __func__, path);

    if (0 == strcmp(path, SVR_DB_DAT_FILE_NAME)) { /* "oic_svr_db.dat" */
        // clog_info(CLOG(MY_LOGGER), "%s opening %s", __func__, SVR_CONFIG_FILE);
	FILE *f = fopen(SVR_CONFIG_FILE, mode);
	if (f == NULL) {
            clog_info(CLOG(MY_LOGGER), "PS file open failed %d %s", errno, strerror(errno));
	    exit(EXIT_FAILURE);
	}
	return f;
    }
    else
    {
	return fopen(path, mode);
    }
}

int main ()
{
    /* Initialize the logger */
    int r;
    r = clog_init_path(MY_LOGGER, "logs/client.log");
    if (r != 0) {
        fprintf(stderr, "Logger initialization failed.\n");
        return 1;
    }

    /* Set minimum log level to info (default: debug) */
    clog_set_level(MY_LOGGER, CLOG_INFO);

    clog_info(CLOG(MY_LOGGER), "clogger initialized");

    /* OCLogInit(NULL); */
    /* /\* logfd = fopen("./logs/client.log", "w"); *\/ */
    /* /\* OCLogHookFd(logfd); *\/ */
    /* clog_debug(CLOG(MY_LOGGER), "%s ENTRY, tid %d", __func__, pthread_self()); */

    OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };

    /* Initialize OCStack. Do this here rather than in the work
       thread, to ensure initialization is complete before sending any
       request. */
    //if (OCInit(NULL, 0, OC_CLIENT) != OC_STACK_OK) {
    if (oocf_init(OC_CLIENT, &ps) != OC_STACK_OK) {
        clog_info(CLOG(MY_LOGGER), "OCStack init error");
        // clog_error(CLOG(MY_LOGGER), "OCStack init error");
        clog_free(MY_LOGGER);
        return 0;
    }
    fprintf(stdout, "OpenOCF logfile: %s\n", oocf_get_logfile_name());
    fflush(stdout);
    clog_info(CLOG(MY_LOGGER), "logfile: %s", oocf_get_logfile_name());

    /* We send a discovery request before we start processing incoming
       messages. */
    run();

    clog_info(CLOG(MY_LOGGER), "Entering client message processing loop...");
    /* Since this demo does not provide a UI for user interaction, we
     * can run this loop on the main thread. If we had a UI, we would
     * need to run it on a worker thread. */
    signal(SIGINT, handleSigInt); /* Control-C to exit */

    while (!gQuitFlag) {
	static int i = 0;
        printf(".");
	clog_info(CLOG(MY_LOGGER), "process loop %d, tid %d", i++, pthread_self());
	/* "OCProcess" means "receive incoming messages and dispatch
	   to application handers - in this case,
	   resource_discovery_cb. Each incoming message is dispatched
	   on its own thread. */
        if (OCProcess() != OC_STACK_OK) {
            printf("ERROR: OCStack\n");
            clog_error(CLOG(MY_LOGGER), "OCStack process error");
            clog_free(MY_LOGGER);
            return 0;
        }
        fflush(stdout);
	//fflush(logfd);		/* FIXME */
        usleep(200000);         /* 0.2 seconds */
    }
    if (gError)
        printf("ERROR\n");
    else
        printf("OK\n");

    clog_info(CLOG(MY_LOGGER), "Exiting client main loop...");

    if (OCStop() != OC_STACK_OK) {
        clog_error(CLOG(MY_LOGGER), "OCStack stop error");
    }


    /* int err = pthread_create(&(ocf_thread), NULL, &ocf_routine, NULL);
     * if (err != 0)
     * 	clog_debug(CLOG(MY_LOGGER), "Can't create OCF thread :[%s]", strerror(err));
     * else
     * 	clog_debug(CLOG(MY_LOGGER), "\n OCF thread created successfully\n"); */

    clog_debug(CLOG(MY_LOGGER), "%s EXIT", __func__);
    /* pthread_exit(NULL); */

    /* fclose(logfd); */

    /* Clean up */
    clog_free(MY_LOGGER);
}
