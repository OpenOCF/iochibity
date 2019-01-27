#include "openocf.h"
#include "logging.h"

#include "cJSON.h"

static void log_msg(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    // printf(format, args);
    /* fprintf(logfd, "goodbye %s\n", "world"); */
    vfprintf(logfd, format, args);
    va_end(args);
}

static void print_json(OCDiscoveryPayload *payload)
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

/* FIXME: move to coresource_inspector.c */
/* static char text[MSG_MAX][80];		/\* since message is const we need a work buffer *\/ */

int decode_resource(int index, char *ptrs[], char *buffer, int *count)
{
    int ocf_version;
    OCResourcePayload *resource = oocf_coresource_db_mgr_get_coresource(index, &ocf_version);
    OIC_LOG_V(INFO, TAG, "ocf version: %d", ocf_version);

    int i = 0;

    ptrs[i++] = buffer;
    sprintf(buffer, "<C>                    </R>Coresource Link Inspector<!R>                    ");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;


    sprintf(buffer, " anchor: %s", resource->anchor);
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    /* Common Properties for all resources: rt, if, n, id */

    /* OIC_LOG_V(INFO, TAG, "instance: %d", i);
     * sprintf(buffer, "ins: %d", i + 1);
     * ptrs[i++] = buffer;
     * buffer += strlen(buffer) + 1; */

    /* Mandatory props for discoverable resource (resource): href, rt, if */

    sprintf(buffer, "*href:\t%s", resource->uri);
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    sprintf(buffer, " rel:\t%s", resource->rel);
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    OCStringLL *rtype = resource->types;
    if (rtype) {
	sprintf(buffer, "*rt[]:");
	buffer += strlen(buffer);
	while (rtype) {
	    sprintf(buffer, "\t%s", rtype->value);
	    buffer += strlen(buffer) + 1;
	    ptrs[i++] = buffer;
	    rtype = rtype->next;
	}
    }
    OIC_LOG_V(DEBUG, TAG, "BUF: %p, %d, %s", buffer, i, ptrs[i-1]);

    OCStringLL *iface = resource->interfaces;
    if (iface) {
	sprintf(buffer, "*if[]:");
	buffer += strlen(buffer);
	while (iface) {
	    sprintf(buffer, "\t%s", iface->value);
	    buffer += strlen(buffer) + 1;
	    ptrs[i++] = buffer;
	    iface = iface->next;
	}
    }


    /* cJSON_AddItemToObject(l, "n", cJSON_CreateNull()); /\* name *\/ */
    sprintf(buffer, " n:\t%s", "(null)");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    /* cJSON_AddItemToObject(l, "id", cJSON_CreateNull()); /\* resource identifier *\/ */
    sprintf(buffer, " id:\t%s", "(null)");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    /* cJSON_AddItemToObject(l, "di", cJSON_CreateNull()); /\* device id *\/ */
    sprintf(buffer, " di:\t%s", "(null)");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    /* cJSON_AddItemToObject(l, "title", cJSON_CreateNull()); */
    sprintf(buffer, " title:\t%s", "(null)");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    /* cJSON_AddItemToObject(l, "type", cJSON_CreateArray()); /\* media type *\/ */
    sprintf(buffer, " media type:\t%s", "(null)");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    /* bm constants are in struct OCResourceProperty */
    sprintf(buffer, " policy bm: discoverable? %s", (resource->bitmap & OC_DISCOVERABLE) ? "T" : "F");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;
    sprintf(buffer, "\t    observable?   %s", (resource->bitmap & OC_OBSERVABLE) ? "T" : "F");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

     /* legacy: "sec" and "port" not used for OCF 1.0, which uses eps instead */
    if ( ocf_version == OCF_VERSION_1_0_0 || ocf_version == OCF_VERSION_1_1_0) {
	sprintf(buffer, " eps:");
	buffer += strlen(buffer);
	/* ptrs[i++] = buffer; */

	OCEndpointPayload *endpoint = resource->eps;
	while(endpoint) {
	    OIC_LOG_V(DEBUG, TAG, "ENDPOINT RAW ADDR: %s:%d", endpoint->addr, endpoint->port);
	    OIC_LOG_V(DEBUG, TAG, "ENDPOINT FAMILY: 0x%X", endpoint->family);
	    int eplen = strlen(endpoint->tps)
		+ 3		/* :// */
		+ strlen(endpoint->addr)
		+ 2				/* [  ] */
		+ 10				/* "pri: %d" */
		+ 1; 		/* : */
	    char *epstring = malloc(eplen + 6); /* largest val for port is 5 chars (uint16) */
	    if (endpoint->family & OC_IP_USE_V6) { /* (1 << 5) */
		snprintf(buffer, eplen + 6, "\t%s://[%s]:%d pri: %d",
			 endpoint->tps, endpoint->addr, endpoint->port, endpoint->pri);
	    } else if (endpoint->family & OC_IP_USE_V4) { /* (1 << 5) */

		snprintf(buffer, eplen + 6, "\t%s://%s:%d pri: %d",
			 endpoint->tps, endpoint->addr, endpoint->port, endpoint->pri);
	    }
	    endpoint = endpoint->next;
	    buffer += strlen(buffer) + 1;
	    ptrs[i++] = buffer;
	}

    } else {
        /* cJSON_AddItemToObject(policy, "sec", cJSON_CreateBool(resource->secure)); /\* security *\/
         * cJSON_AddNumberToObject(policy, "port", resource->port); /\* secure port *\/ */
    }



    *count = i - 1;
    return 0;
}

 /* FIXME: split header logging from payload logging */
static cJSON* links_to_json(OCClientResponse *msg)
{
    /* We need to to know the OCF version in order to decode the payload */
    int ocf_version = oocf_ocf_version(msg);
    if (ocf_version < 0) {
	OIC_LOG_V(ERROR, TAG, "OCF_CONTENT_FORMAT_VERSION option error");
    }

    cJSON *links;
    links = cJSON_CreateArray();
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
	if ( ocf_version != OCF_VERSION_1_0_0
	     && ocf_version != OCF_VERSION_1_1_0) {
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

static cJSON* discovery_to_json(OCClientResponse *msg)
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

static cJSON* representation_to_json(OCRepPayload *payload)
{
    cJSON *root;
    root = cJSON_CreateObject();
    if (payload->uri) {
	cJSON_AddItemToObject(root, "href", cJSON_CreateString(payload->uri));
    }

    int i;

    if (payload->types) {
	const char *ts[10];
	i = 0;
	OCStringLL *types = payload->types;
	while (types) {
	    /* OIC_LOG_V(INFO, TAG, "rtype: %s", types->value); */
	    ts[i] = types->value;
	    //ts++;
	    i++;
	    types = types->next;
	}
	cJSON_AddItemToObject(root, "rt", cJSON_CreateStringArray(ts, i));
    }

    if (payload->interfaces) {
	const char *ifaces[10];
	OCStringLL *ifs = payload->interfaces;
	i = 0;
	while (ifs) {
	    /* OIC_LOG_V(INFO, TAG, "iface: %s", ifs->value); */
	    ifaces[i] = ifs->value;
	    //ts++;
	    i++;
	    ifs = ifs->next;
	}
	cJSON_AddItemToObject(root, "if", cJSON_CreateStringArray(ifaces, i));
    }

    if (payload->values) {
	const char *props[10];
	OCRepPayloadValue *val = payload->values;
	i = 0;
	while (val) {
	    if (strncmp(val->name, "bm", 2) == 0) {
		OIC_LOG_V(INFO, TAG, "bm type: %d", val->type);
		char *bm[2];
		bm[0] = (val->i & OC_DISCOVERABLE)? "discoverable" : "non-discoverable";
		bm[1] = (val->i & OC_OBSERVABLE)? "observable" : "non-observable";
		cJSON_AddItemToObject(root, val->name, cJSON_CreateStringArray(bm, 2));
	    } else {
		switch(val->type) {
		case OCREP_PROP_NULL:
		    cJSON_AddItemToObject(root, val->name, cJSON_CreateString(""));
		    break;
		case OCREP_PROP_INT:
		    cJSON_AddNumberToObject(root, val->name, val->i);
		    break;
		case OCREP_PROP_DOUBLE:
		    cJSON_AddNumberToObject(root, val->name, val->d);
		    break;
		case OCREP_PROP_BOOL:
		    cJSON_AddItemToObject(root, val->name, cJSON_CreateBool(val->b));
		    break;
		case OCREP_PROP_STRING:
		    cJSON_AddStringToObject(root, val->name, val->str);
		    break;
		case OCREP_PROP_BYTE_STRING:
		    cJSON_AddItemToObject(root, val->name, cJSON_CreateString("bytestring"));
		    break;
		case OCREP_PROP_OBJECT:
		    cJSON_AddItemToObject(root, val->name, representation_to_json(val->obj));
		    break;
		case OCREP_PROP_ARRAY:
		    cJSON_AddItemToObject(root, val->name, cJSON_CreateString("array"));
		    break;
		}
	    }
	    i++;
	    val = val->next;
	}
    }

    return root;
}

void log_representation_msg(OCClientResponse *clientResponse)
{
    cJSON *representation_json = representation_to_json((OCRepPayload*)clientResponse->payload);
    char* rendered = cJSON_Print(representation_json);

    char fname[256];
    sprintf(fname, "./logs/client/rep_%p.txt", clientResponse);
    OIC_LOG_V(INFO, TAG, "representation json filename: %s", fname);
    FILE *fd = fopen(fname, "w");
    if (fd == NULL) {
        OIC_LOG_V(INFO, TAG, "fopen %s err: %s", fname, strerror(errno));
	exit(EXIT_FAILURE);
    }
    fprintf(fd, "%s", rendered);
    fclose(fd);
}

static void log_payload_type(OCPayload *payload)
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

static void log_discovery_message(OCClientResponse *clientResponse)
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
    /* log_msg("%s\n", rendered); */

    /* char cwd[256];
     * if (getcwd(cwd, sizeof(cwd)) != NULL)
     * 	OIC_LOG_V(INFO, TAG, "cwd: %s", cwd); */

    char fname[256];
    sprintf(fname, "./logs/client/msg_%p.txt", clientResponse);
    OIC_LOG_V(INFO, TAG, "json filename: %s", fname);
    FILE *fd = fopen(fname, "w");
    if (fd == NULL) {
	OIC_LOG_V(INFO, TAG, "fopen err: %s", strerror(errno));
    }
    fprintf(fd, "%s", rendered);
    fclose(fd);

    free(rendered);
}

void log_discovery_payload(OCClientResponse *clientResponse)
{
    cJSON *discovery_json = discovery_to_json(clientResponse);
    char* rendered = cJSON_Print(discovery_json);

    char fname[256];
    sprintf(fname, "./logs/client/disc_%p.txt", clientResponse);
    OIC_LOG_V(INFO, TAG, "json filename: %s", fname);
    FILE *fd = fopen(fname, "w");
    if (fd == NULL) {
        OIC_LOG_V(INFO, TAG, "fopen %s err: %s", fname, strerror(errno));
	exit(EXIT_FAILURE);
    }
    fprintf(fd, "%s", rendered);
    fclose(fd);
}
