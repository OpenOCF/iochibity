#include "jni_init.h"

#include "org_iochibity_Exceptions.h"

//#include "openocf_message_InboundResponse.h"
#include "openocf_Endpoint.ids.h"
#include "map_keys.h"


#define FQCN_INBOUND_RESPONSE "openocf/message/InboundResponse"
jclass    K_INBOUND_RESPONSE                = NULL;
jmethodID MID_INRESP_CTOR               = NULL;
jfieldID  FID_INRESP_HANDLE = NULL;
jfieldID  FID_INRESP_OBSERVATION_HANDLE = NULL;
/* jfieldID  FID_INRESP_REMOTE_DEVADDR     = NULL; */

jmethodID MID_INRESP_GET_URI = NULL;
char     *MID_INRESP_GET_URI_NM = "getUri";
char     *MID_INRESP_GET_URI_T = "()Ljava/lang/String;";

jmethodID MID_INRESP_GET_EP = NULL;
char     *MID_INRESP_GET_EP_NM = "getEndpoint";
char     *MID_INRESP_GET_EP_T = "()Lopenocf/Endpoint;";

jfieldID  FID_INRESP_IS_RETAIN          = NULL;

/* jfieldID  FID_INRESP_CONN_TYPE          = NULL; */
/* jfieldID  FID_INRESP_REMOTE_DI         = NULL; */
/* jfieldID  FID_INRESP_RESULT             = NULL; */
/* jfieldID  FID_INRESP_SERIAL             = NULL; */
/* jfieldID  FID_INRESP_URI                = NULL; */
/* jfieldID  FID_INRESP_OPTIONS            = NULL; */
/* jfieldID  FID_INRESP_PTR_OPTIONS        = NULL; */
jmethodID MID_INRESP_GET_OBSERVATION    = NULL;
jmethodID MID_INRESP_GET_OPTIONS        = NULL;

int init_InboundResponse(JNIEnv* env)
{
    jclass klass;
    klass = (*env)->FindClass(env, FQCN_INBOUND_RESPONSE);
    JNI_ASSERT_NULL(klass, ERR_MSG(ERR_CLASS, FQCN_INBOUND_RESPONSE), -1);
    K_INBOUND_RESPONSE = (jclass)(*env)->NewGlobalRef(env, klass);
    (*env)->DeleteLocalRef(env, klass);

    if (MID_INRESP_CTOR == NULL) {
	MID_INRESP_CTOR = (*env)->GetMethodID(env, K_INBOUND_RESPONSE, "<init>", "()V");
	JNI_ASSERT_NULL(MID_INRESP_CTOR, ERR_MSG(ERR_METHOD, "<init> for InboundResponse"), -1);
    }

    if (FID_INRESP_IS_RETAIN == NULL) {
    	FID_INRESP_IS_RETAIN = (*env)->GetFieldID(env, K_INBOUND_RESPONSE,
						  "isRetain", "Z");
	JNI_ASSERT_NULL(FID_INRESP_IS_RETAIN,
			ERR_MSG(ERR_FLD, "InboundResponse.isRetain"), -1);
    }

    if (MID_INRESP_GET_EP == NULL) {
	MID_INRESP_GET_EP = (*env)->GetMethodID(env, K_INBOUND_RESPONSE,
						MID_INRESP_GET_EP_NM,
						MID_INRESP_GET_EP_T);
	JNI_ASSERT_NULL(MID_INRESP_GET_EP,
			ERR_MSG(ERR_METHOD, "InboundResponse.getEndpoint"), -1);
    }

    if (MID_INRESP_GET_URI == NULL) {
	MID_INRESP_GET_URI = (*env)->GetMethodID(env, K_INBOUND_RESPONSE,
						 MID_INRESP_GET_URI_NM,
						 MID_INRESP_GET_URI_T);
	JNI_ASSERT_NULL(MID_INRESP_GET_EP,
			ERR_MSG(ERR_METHOD, "InboundResponse.getURI"), -1);
    }

    /* if (MID_INRESP_GET_REMOTE_DEVADDR == NULL) { */
    /* 	MID_INRESP_GET_REMOTE_DEVADDR = (*env)->GetMethodID(env, K_INBOUND_RESPONSE, */
    /* 							      "getRemoteDeviceAddress", */
    /* 							      "()Lopenocf/Endpoint;"); */
    /* 	if (MID_INRESP_GET_REMOTE_DEVADDR == NULL) { */
    /* 	    printf("ERROR:  GetMethodID failed for getRemoteDeviceAddress for InboundResponse\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    /* if (FID_INRESP_CONN_TYPE == NULL) { */
    /* 	FID_INRESP_CONN_TYPE = (*env)->GetFieldID(env, K_INBOUND_RESPONSE, "connType", "I"); */
    /* 	if (FID_INRESP_CONN_TYPE == NULL) { */
    /* 	    printf("ERROR: GetFieldID failed for 'connType' of InboundResponse\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */

    /* if (FID_INRESP_REMOTE_DI == NULL) { */
    /* 	FID_INRESP_REMOTE_DI = (*env)->GetFieldID(env, K_INBOUND_RESPONSE, */
    /* 						   "_secID", "Ljava/lang/String;"); */
    /* 	JNI_ASSERT_NULL(FID_INRESP_REMOTE_DI, */
    /* 			ERR_MSG(ERR_FLD, "InboundResponse._secID"), -1); */
    /* 	/\* if (FID_INRESP_REMOTE_DI == NULL) { *\/ */
    /* 	/\*     printf("ERROR: GetFieldID failed for 'secID' of InboundResponse\n"); *\/ */
    /* 	/\*     return -1; *\/ */
    /* 	/\* } *\/ */
    /* } */
    /* if (FID_INRESP_RESULT == NULL) { */
    /* 	FID_INRESP_RESULT = (*env)->GetFieldID(env, K_INBOUND_RESPONSE, "resourceSPResult", "I"); */
    /* 	JNI_ASSERT_NULL(FID_INRESP_RESULT, */
    /* 			ERR_MSG(ERR_FLD, "InboundResponse.resourceSPResult"), -1); */
    /* } */

    /* if (FID_INRESP_SERIAL == NULL) { */
    /* 	FID_INRESP_SERIAL = (*env)->GetFieldID(env, K_INBOUND_RESPONSE, */
    /* 					       "notificationSerial", "I"); */
    /* 	JNI_ASSERT_NULL(FID_INRESP_SERIAL, */
    /* 			ERR_MSG(ERR_FLD, "InboundResponse.notificationSerial"), -1); */
    /* } */

    /* if (FID_INRESP_URI == NULL) { */
    /* 	FID_INRESP_URI = (*env)->GetFieldID(env, K_INBOUND_RESPONSE, "_uriPath", "Ljava/lang/String;"); */
    /* 	if (FID_INRESP_URI == NULL) { */
    /* 	    printf("ERROR: GetFieldID failed for 'uri' of InboundResponse\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    /* if (MID_INRESP_GET_OBSERVATION == NULL) { */
    /* 	MID_INRESP_GET_OBSERVATION = (*env)->GetMethodID(env, K_INBOUND_RESPONSE, */
    /* 						    "getPDUObservation", */
    /* 						    "()Lopenocf/ObservationList;"); */
    /* 	if (MID_INRESP_GET_OBSERVATION == NULL) { */
    /* 	    printf("ERROR: GetFieldID failed for 'getPDUObservation' of InboundResponse\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    /* if (FID_INRESP_PTR_OPTIONS == NULL) { */
    /* 	FID_INRESP_PTR_OPTIONS = (*env)->GetFieldID(env, K_INBOUND_RESPONSE, "ptr_Options", "J"); */
    /* 	if (FID_INRESP_PTR_OPTIONS == NULL) { */
    /* 	    printf("ERROR: GetFieldID failed for 'ptr_Options' of InboundResponse\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    /* if (FID_INRESP_OPTION_COUNT == NULL) { */
    /* 	FID_INRESP_OPTION_COUNT = (*env)->GetFieldID(env, K_INBOUND_RESPONSE, "optionCount", "I"); */
    /* 	if (FID_INRESP_OPTION_COUNT == NULL) { */
    /* 	    printf("ERROR: GetFieldID failed for 'optionCount' of InboundResponse\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    /* if (MID_INRESP_GET_OPTIONS == NULL) { */
    /* 	MID_INRESP_GET_OPTIONS = (*env)->GetMethodID(env, K_INBOUND_RESPONSE, */
    /* 						    "getOptions", "()Ljava/util/List;"); */
    /* 	if (MID_INRESP_GET_OPTIONS == NULL) { */
    /* 	    printf("ERROR: GetFieldID failed for 'getOptions' of InboundResponse\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    return 0;
}

/* /\* */
/*  * Class:     openocf_app_CoResourceSP */
/*  * Method:    getUri */
/*  * Signature: ()Ljava/lang/String; */
/*  *\/ */
/* JNIEXPORT jstring JNICALL */
/* Java_openocf_app_CoResourceSP_getUri(JNIEnv * env, jobject this) */

/*
 * Class:     openocf_message_InboundResponse
 * Method:    getUri
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_openocf_message_InboundResponse_getUri(JNIEnv *env, jobject this)
{
    OC_UNUSED(env);
    OC_UNUSED(this);
    jstring j_uri = (*env)->NewStringUTF(env, tls_response_in->response->resourceUri);
    return  j_uri;
}

/*
 * Class:     openocf_message_InboundResponse
 * Method:    getMethod
 * Signature: ()I
 *
 * InboundResponse does not contain method.  The method will be
 * implicit in the coReact callback, _if_ we use a different callback
 * for each method.  But that is not required; we could in principle
 * use the same callback for different operations _if_ we could pull
 * the method from the inbound response. So our only option is to find
 * the request associated with this response (using the coap token?)
 */
JNIEXPORT jint JNICALL
Java_openocf_message_InboundResponse_getMethod(JNIEnv *env, jobject this)
{
    // FIXME
    return 0;
}

/*
 * Class:     openocf_message_InboundResponse
 * Method:    getResult
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_openocf_message_InboundResponse_getResult(JNIEnv *env, jobject this)
{
    OC_UNUSED(env);
    OC_UNUSED(this);
    return (jint)tls_response_in->response->result;
}

/*
 * Class:     openocf_message_InboundResponse
 * Method:    getNotificationSerial
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_openocf_message_InboundResponse_getNotificationSerial(JNIEnv *env, jobject this)
{
    OC_UNUSED(env);
    OC_UNUSED(this);
    return (jint)tls_response_in->response->sequenceNumber;
}

/*
 * Class:     openocf_message_InboundResponse
 * Method:    getIdentity
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_openocf_message_InboundResponse_getIdentity(JNIEnv *env, jobject this)
{
    OC_UNUSED(env);
    OC_UNUSED(this);

    OCIdentity *identity = &(tls_response_in->response->identity);
    return (*env)->NewStringUTF(env, identity->id);
}

/*
 * Class:     openocf_message_InboundResponse
 * Method:    getEndpoint
 * Signature: ()Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL
Java_openocf_message_InboundResponse_getEndpoint(JNIEnv *env , jobject this)
{
    OC_UNUSED(env);
    OC_UNUSED(this);
    /* OIC_LOG_V("%s ENTRY"); */

    OCDevAddr *ep_handle = &(tls_response_in->response->devAddr);

    jobject j_Endpoint = (*env)->NewObject(env, K_ENDPOINT, MID_EP_CTOR);
    if (j_Endpoint == NULL) {
	THROW_JNI_EXCEPTION("Endpoint() (ctor)");;
    }

    (*env)->SetLongField(env, j_Endpoint,
			 FID_EP_HANDLE,
			 (long)(intptr_t)ep_handle);
    if ((*env)->ExceptionCheck(env)) {
	(*env)->ExceptionDescribe(env);
    }
    return j_Endpoint;
}

/*
 * Class:     openocf_message_InboundResponse
 * Method:    getCoAPOptions
 * Signature: ()Ljava/util/List;
 *
 * returns List<Map>
*/
JNIEXPORT jobject JNICALL
Java_openocf_message_InboundResponse_getCoAPOptions(JNIEnv *env, jobject this)
{
    jobject j_option_list  = (*env)->NewObject(env, K_ARRAYLIST, MID_ARRAYLIST_CTOR);
    if (j_option_list == NULL) { THROW_JNI_EXCEPTION("option_list ArrayList() (ctor)"); }

    struct oocf_coap_options *options = tls_response_in->response->rcvdVendorSpecificHeaderOptions;
    for (int i=0; i < tls_response_in->response->numRcvdVendorSpecificHeaderOptions; i++) {
    jobject j_option  = (*env)->NewObject(env, K_ARRAYLIST, MID_ARRAYLIST_CTOR);

    }

    return NULL;
}


#include "cJSON.h"

static jobject link_to_map(OCResourcePayload *link)
{

}

/* Create a Java ArrayList of links.  A link is a Map. */
static jobject links_to_list(OCClientResponse *msg)
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
	jobject linkMap = link_to_map(link);
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

static jobject discovery_to_jmap(JNIEnv *env, OCClientResponse *msg)
{
    OCDiscoveryPayload *payload = (OCDiscoveryPayload*)msg->payload;

    jobject ores;		/* object result */
    jstring j_sval;

    jboolean bres;			/* boolean result */

    /* create the Payload HashMap object */
    jobject j_payload_map = (*env)->NewObject(env, K_HASHMAP, MID_HASHMAP_CTOR);
    if (j_payload_map == NULL) { THROW_JNI_EXCEPTION("payload HashMap() (ctor)"); }

    // cJSON_AddItemToObject(root, "href", cJSON_CreateString("oic/res"));

    /* di (di) */
    j_sval = (*env)->NewStringUTF(env, payload->di);
    ores = (*env)->CallObjectMethod(env, j_payload_map, MID_HASHMAP_PUT, KEY_OCF_DI, j_sval);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
    (*env)->DeleteLocalRef(env, j_sval);

    /* name (n) */
    j_sval = (*env)->NewStringUTF(env, payload->name);
    ores = (*env)->CallObjectMethod(env, j_payload_map, MID_HASHMAP_PUT, KEY_OCF_NAME, j_sval);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
    (*env)->DeleteLocalRef(env, j_sval);

    /* iface (if) - ArrayList<String> */
    jobject j_if_list  = (*env)->NewObject(env, K_ARRAYLIST, MID_ARRAYLIST_CTOR);
    if (j_if_list == NULL) { THROW_JNI_EXCEPTION("if_list ArrayList() (ctor)"); }

    OCStringLL *iface = payload->iface;
    //int i = 0;
    while (iface) {
	printf("iface: %s", iface->value);
	j_sval = (*env)->NewStringUTF(env, iface->value);
	bres = (*env)->CallBooleanMethod(env, j_if_list, MID_ARRAYLIST_ADD, j_sval);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_sval);
	//i++;
	iface = iface->next;
    }
    ores = (*env)->CallObjectMethod(env, j_payload_map, MID_HASHMAP_PUT, KEY_OCF_IF, j_if_list);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
    (*env)->DeleteLocalRef(env, j_if_list);

    /* type (rt) - ArrayList<String> */
    jobject j_rt_list  = (*env)->NewObject(env, K_ARRAYLIST, MID_ARRAYLIST_CTOR);
    if (j_rt_list == NULL) { THROW_JNI_EXCEPTION("rt ArrayList() (ctor)"); }
    OCStringLL *rt = payload->type;
    while (rt) {
	printf("rt: %s", rt->value);
	j_sval = (*env)->NewStringUTF(env, rt->value);
	bres = (*env)->CallBooleanMethod(env, j_rt_list, MID_ARRAYLIST_ADD, j_sval);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_sval);
	rt = rt->next;
    }
    ores = (*env)->CallObjectMethod(env, j_payload_map, MID_HASHMAP_PUT, KEY_OCF_RT, j_rt_list);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
    (*env)->DeleteLocalRef(env, j_rt_list);

    /* OCResourcePayload *resources (linked list) - property "links" (mandatory) */
    /* List<Map> */
    jobject j_link_list  = (*env)->NewObject(env, K_ARRAYLIST, MID_ARRAYLIST_CTOR);
    if (j_link_list == NULL) { THROW_JNI_EXCEPTION("links ArrayList() (ctor) NewObject"); }
    /* each link is a Map */
    OCResourcePayload *link = payload->resources;
    while (link) {
	jobject j_link_map = (*env)->NewObject(env, K_HASHMAP, MID_HASHMAP_CTOR);
	if (j_link_map == NULL) { THROW_JNI_EXCEPTION("payload HashMap() (ctor)"); }
	/* uri String (href) */
	j_sval = (*env)->NewStringUTF(env, link->uri);
	ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_OCF_HREF, j_sval);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_sval);
	/* buri String (base uri) NOT IMPLEMENTED by iotivity? */
	j_sval = (*env)->NewStringUTF(env, "");
	ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_OCF_BURI, j_sval);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_sval);
	/* rel String*/
	j_sval = (*env)->NewStringUTF(env, link->rel);
	ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_OCF_LINK_RELATION, j_sval);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_sval);
	/* anchor String*/
	j_sval = (*env)->NewStringUTF(env, link->anchor);
	ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_OCF_ANCHOR, j_sval);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_sval);

	/* types (rt) List<String> */
	jobject j_link_rt_list = NULL;
	j_link_rt_list  = (*env)->NewObject(env, K_ARRAYLIST, MID_ARRAYLIST_CTOR);
	if (j_link_rt_list == NULL) { THROW_JNI_EXCEPTION("link rt ArrayList() (ctor)"); }
	OCStringLL *link_rt = link->types;
	while (link_rt) {
	    j_sval = (*env)->NewStringUTF(env, link_rt->value);
	    bres = (*env)->CallBooleanMethod(env, j_link_rt_list, MID_ARRAYLIST_ADD, j_sval);
	    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	    (*env)->DeleteLocalRef(env, j_sval);
	    link_rt = link_rt->next;
	}
	ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_OCF_RT, j_link_rt_list);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_link_rt_list);

	/* interfaces List<String> (if) */
	j_if_list = NULL;
	j_if_list  = (*env)->NewObject(env, K_ARRAYLIST, MID_ARRAYLIST_CTOR);
	if (j_if_list == NULL) { THROW_JNI_EXCEPTION("link rt ArrayList() (ctor)"); }
	OCStringLL *iface = link->interfaces;
	while (iface) {
	    j_sval = (*env)->NewStringUTF(env, iface->value);
	    bres = (*env)->CallBooleanMethod(env, j_if_list, MID_ARRAYLIST_ADD, j_sval);
	    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	    (*env)->DeleteLocalRef(env, j_sval);
	    iface = iface->next;
	}
	ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_OCF_IF, j_if_list);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_if_list);

	/* bitmap uint8_t Integer (bm, policy bitmask) */
	jobject j_bm = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
						      (int)link->bitmap);
	ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_OCF_POLICY_BITMASK, j_bm);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_bm);

	if (link->bitmap & 0x01) {
	    ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_DISCOVERABLE, J_TRUE);
	} else {
	    ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_DISCOVERABLE, J_FALSE);
	}
	if (link->bitmap & 0x02) {
	    ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_OBSERVABLE, J_TRUE);
	} else {
	    ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_OBSERVABLE, J_FALSE);
	}

	/* secure Boolean (not used by OCF 1.0) */
	/* port int (not used by OCF 1.0) */

	/* NB: iotivity does not support all link properties, see D.2, oic.oic-link in the spec */

	/* eps List<Map> of OCEndpointPayload */
	jobject j_ep_list  = (*env)->NewObject(env, K_ARRAYLIST, MID_ARRAYLIST_CTOR);
	if (j_ep_list == NULL) { THROW_JNI_EXCEPTION("links ArrayList() (ctor) NewObject"); }
	/* each ep is a Map */
	OCEndpointPayload *ep = link->eps;
	while (ep) {
	    jobject j_ep_map = (*env)->NewObject(env, K_HASHMAP, MID_HASHMAP_CTOR);
	    if (j_ep_map == NULL) { THROW_JNI_EXCEPTION("ep HashMap() (ctor)"); }
	    /* tps String (transport protocol suite) */
	    j_sval = (*env)->NewStringUTF(env, ep->tps);
	    ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_OCF_TPS, j_sval);
	    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	    (*env)->DeleteLocalRef(env, j_sval);
	    /* addr String */
	    j_sval = (*env)->NewStringUTF(env, ep->addr);
	    ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_OCF_ADDR, j_sval);
	    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	    (*env)->DeleteLocalRef(env, j_sval);
	    /* port uint16_t */
	    jobject j_port = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, ep->port);
	    ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_OCF_PORT, j_port);
	    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	    (*env)->DeleteLocalRef(env, j_port);
	    /* transport flags enum bitfield */
	    jobject j_tflags = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
							      ep->family);
	    ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_TRANSPORT_FLAGS,
					    j_tflags);
	    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	    (*env)->DeleteLocalRef(env, j_tflags);
	    /* IPv6 boolean */
	    if ( ep->family & 0x20 ) {
		ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_IPV6, J_TRUE);
	    } else {
		ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_IPV6, J_FALSE);
	    }
	    /* IPv4 boolean */
	    if ( ep->family & 0x40 ) {
		ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_IPV4, J_TRUE);
	    } else {
		ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_IPV4, J_FALSE);
	    }
	    /* security flag */
	    if ( ep->family & 0x10 ) {
		ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_SECURE, J_TRUE);
	    } else {
		ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_SECURE, J_FALSE);
	    }
	    /* mcast flag */
	    if ( ep->family & 0x80 ) {
		ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_MCAST, J_TRUE);
	    } else {
		ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_MCAST, J_FALSE);
	    }
	    /* priority uint16_t (pri) */
	    jobject j_pri = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, ep->pri);
	    ores = (*env)->CallObjectMethod(env, j_ep_map, MID_HASHMAP_PUT, KEY_OCF_PRIORITY, j_pri);
	    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	    (*env)->DeleteLocalRef(env, j_pri);

	    /* add the ep to the list of eps */
	    bres = (*env)->CallBooleanMethod(env, j_ep_list, MID_ARRAYLIST_ADD, j_ep_map);
	    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	    (*env)->DeleteLocalRef(env, j_ep_map);
	    ep = ep->next;
	}
	ores = (*env)->CallObjectMethod(env, j_link_map, MID_HASHMAP_PUT, KEY_OCF_EPS, j_ep_list);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_ep_list);

	/* now add the link map to list of links, then iterate */
	bres = (*env)->CallBooleanMethod(env, j_link_list, MID_ARRAYLIST_ADD, j_link_map);
	if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
	(*env)->DeleteLocalRef(env, j_link_map);
	link = link->next;
    }

    ores = (*env)->CallObjectMethod(env, j_payload_map, MID_HASHMAP_PUT, KEY_OCF_LINKS, j_link_list);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
    (*env)->DeleteLocalRef(env, j_link_list);

    /* jobject j_index = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, */
    /* 						     devaddr->ifindex); */



    /* cJSON_AddItemToObject(root, "mpro", cJSON_CreateNull()); */


    /* /\* OIC_LOG_V(INFO, TAG, "Discovery payload type: %s", */
    /*  * 	      (pay->type == NULL) ? "(null)" : pay->type->value); */
    /*  * */
    /*  * OIC_LOG_V(INFO, TAG, "Discovery payload iface: %s", */
    /*  * 	      (pay->iface == NULL) ? "(null)" : pay->iface->value); *\/ */

    /* links = links_to_json(msg); */

    /* cJSON_AddItemToObject(root, "links", links); */
    /* char* rendered = cJSON_Print(root); */
    return j_payload_map;
}

static void discovery_to_json(OCClientResponse *msg)
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
			  cJSON_CreateString(payload->di) : cJSON_CreateNull());
    cJSON_AddItemToObject(root, "mpro", cJSON_CreateNull());
    cJSON_AddItemToObject(root, "di", cJSON_CreateString(payload->di));

    /* OIC_LOG_V(INFO, TAG, "Discovery payload type: %s",
     * 	      (pay->type == NULL) ? "(null)" : pay->type->value);
     *
     * OIC_LOG_V(INFO, TAG, "Discovery payload iface: %s",
     * 	      (pay->iface == NULL) ? "(null)" : pay->iface->value); */

    links = links_to_json(msg);
    cJSON_AddItemToObject(root, "links", links);
    char* rendered = cJSON_Print(root);
    printf(rendered);
    //return rendered;
    // return root;
}

static cJSON* representation_to_json(OCClientResponse *msg)
{
    printf("Payload type: %d\n", msg->payload->type);
    OCRepPayload *payload = (OCRepPayload*)msg->payload;
    cJSON *root;
    root = cJSON_CreateObject();
    if (payload->uri) {
	cJSON_AddItemToObject(root, "href", cJSON_CreateString(payload->uri));
    }
    const char *ts[10];
    int i = 0;
    OCStringLL *types = payload->types;
    while (types) {
	/* OIC_LOG_V(INFO, TAG, "rtype: %s", types->value); */
	ts[i] = types->value;
	//ts++;
	i++;
	types = types->next;
    }
    cJSON_AddItemToObject(root, "rt", cJSON_CreateStringArray(ts, i));

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
    char* rendered = cJSON_Print(root);
    return rendered;
}

jobject get_payload(JNIEnv *env, OCClientResponse *resp)
{
    switch (resp->payload->type) {
    case PAYLOAD_TYPE_INVALID:
	OIC_LOG_V(INFO, TAG, "Message payload type: INVALID");
	break;
    case PAYLOAD_TYPE_DISCOVERY:
	OIC_LOG_V(INFO, TAG, "Message payload type: DISCOVERY");
	// discovery_to_json(resp);
	return discovery_to_jmap(env, resp);
	break;
    case PAYLOAD_TYPE_DEVICE:
	OIC_LOG_V(INFO, TAG, "Message payload type: DEVICE");
	break;
    case PAYLOAD_TYPE_PLATFORM:
	OIC_LOG_V(INFO, TAG, "Message payload type: PLATFORM");
	break;
    case PAYLOAD_TYPE_REPRESENTATION:
	OIC_LOG_V(INFO, TAG, "Message payload type: REPRESENTATION");
	return representation_to_json(resp);
	break;
    case PAYLOAD_TYPE_SECURITY:
	OIC_LOG_V(INFO, TAG, "Message payload type: SECURITY");
	break;
    /* case PAYLOAD_TYPE_PRESENCE: */
    /*     OIC_LOG_V(INFO, TAG, "Message payload type: PRESENCE"); */
    /*     break; */
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


/*
 * Class:     openocf_message_InboundResponse
 * Method:    toMap
 * Signature: ()Ljava/util/Map;
 */
JNIEXPORT jobject JNICALL
Java_openocf_message_InboundResponse_toMap(JNIEnv *env, jobject this)
{
    OC_UNUSED(env);
    OC_UNUSED(this);
    printf("InboundReponse.toMap ENTRY\n");

    jobject ores;		/* object result */
    jstring j_sval;		/* string map value */

    oocf_inbound_response *resp = tls_response_in->response;

    /* create the InboundResponse Map object */
    jobject j_rmap = (*env)->NewObject(env, K_HASHMAP, MID_HASHMAP_CTOR);
    if (j_rmap == NULL) { THROW_JNI_EXCEPTION("resp HashMap() (ctor)"); }

    OCDevAddr *devaddr = &(resp->devAddr);

    jobject j_devaddr_map = (*env)->NewObject(env, K_HASHMAP, MID_HASHMAP_CTOR);
    if (j_devaddr_map == NULL) { THROW_JNI_EXCEPTION("devaddr HashMap() (ctor)"); }

    /* devaddr address */
    j_sval = (*env)->NewStringUTF(env, devaddr->addr);
    ores = (*env)->CallObjectMethod(env, j_devaddr_map, MID_HASHMAP_PUT, KEY_ADDR, j_sval);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* devaddr port */
    jobject j_port = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
						  devaddr->port);
    ores = (*env)->CallObjectMethod(env, j_devaddr_map, MID_HASHMAP_PUT, KEY_PORT, j_port);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* devaddr adapter */
    jobject j_transport_adapter = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
						  devaddr->adapter);
    ores = (*env)->CallObjectMethod(env, j_devaddr_map, MID_HASHMAP_PUT, KEY_TRANSPORT_ADAPTER,
				    j_transport_adapter);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
    (*env)->DeleteLocalRef(env, j_transport_adapter);

    /* devaddr transport flags */
    jobject j_transport_flags = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
						  devaddr->adapter);
    ores = (*env)->CallObjectMethod(env, j_devaddr_map, MID_HASHMAP_PUT, KEY_TRANSPORT_FLAGS,
				    j_transport_flags);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
    (*env)->DeleteLocalRef(env, j_transport_flags);

    /* devaddr IF index */
    jobject j_index = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
						     devaddr->ifindex);
    ores = (*env)->CallObjectMethod(env, j_devaddr_map, MID_HASHMAP_PUT, KEY_INDEX, j_index);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* put the devaddr map into the response map */
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_DEVADDR, j_devaddr_map);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* OCConnectivityType (enum) connType */
    /* connType combines, transport, network, security, and ipv6 scope flags */
    printf("connType: 0x%08X\n", resp->connType);
    jobject j_conntype = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, resp->connType);
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_CONN_TYPE, j_conntype);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
    (*env)->DeleteLocalRef(env, j_conntype);

    int adapter = resp->connType >> 16;
    printf("adapter: 0x%08X\n", adapter);
    jobject j_adapter = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, adapter);
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_TRANSPORT, j_adapter);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* UDP boolean */
    if ( resp->connType & 0x10000 ) {
	ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_UDP, J_TRUE);
    } else {
	ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_UDP, J_FALSE);
    }
    /* TCP boolean */
    if ( resp->connType & 0x100000 ) {
	ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_TCP, J_TRUE);
    } else {
	ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_TCP, J_FALSE);
    }
    /* IPv4 boolean */
    bool ipv4 = resp->connType & 0x0040;
    printf("ipv4: %d\n", ipv4);
    if (ipv4)
	ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_IPV4, J_TRUE);
    else
	ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_IPV4, J_FALSE);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
    /* IPv6 boolean */
    bool ipv6 = resp->connType & 0x0020;
    printf("ipv6: %d\n", ipv6);
    if (ipv6) {
	printf("ipv6 true\n");
	ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_IPV6, J_TRUE);
    } else {
	printf("ipv6 false\n");
	ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_IPV6, J_FALSE);
    }
    /* Secure boolean */
    bool secure = resp->connType & 0x0010;
    printf("secure? %d\n", secure);
    if (secure) {
	ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_SECURE, J_TRUE);
    } else {
	printf("ipv6 false\n");
	ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_SECURE, J_FALSE);
    }

    int scope = resp->connType & 0x000F;
    printf("scope: 0x%02X\n", scope);
    jobject j_scope = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, scope);
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_SCOPE, j_scope);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* OCIdentity identity */
    printf("Identity length: %d\n", resp->identity.id_length);
    char id[MAX_IDENTITY_SIZE + 1] = { '\0' };
    strncpy(&id, resp->identity.id, resp->identity.id_length);
    jstring s = (*env)->NewStringUTF(env, &id);
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_REMOTE_IDENTITY, s);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* OCStackResult (enum) result */
    jobject j_result = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, resp->result);
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_REMOTE_RESULT, j_result);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* uint32_t sequenceNumber */
    jobject j_serial = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, resp->sequenceNumber);
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_SERIAL, j_serial);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* const char * resourceUri; */
    j_sval = NULL;
    j_sval = (*env)->NewStringUTF(env, resp->resourceUri);
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_URI, j_sval);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* OCPayload *payload; */
    jobject j_payload_type = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
							    resp->payload->type);
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_PAYLOAD_TYPE, j_payload_type);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

    /* char *payload= get_payload(env, resp); */
    /* j_sval = (*env)->NewStringUTF(env, payload); */
    /* ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_PAYLOAD, j_sval); */
    /* if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); } */
    jobject j_payload = get_payload(env, resp); /* HashMap */
    if (j_payload == NULL) {
	printf("NO PAYLOAD XXXXXXXXXXXXXXXX\n");
    }
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_PAYLOAD, j_payload);
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }


    /* uint8_t numRcvdVendorSpecificHeaderOptions */
    printf("numRcvdVendorSpecificHeaderOptions: %d\n", resp->numRcvdVendorSpecificHeaderOptions);
    /* OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS] */
    jobject j_options  = (*env)->NewObject(env, K_ARRAYLIST, MID_ARRAYLIST_CTOR_INT, 8);
    if (j_options == NULL) { THROW_JNI_EXCEPTION("coap options ArrayList(int) (ctor)"); }
    /* A CoAP Option is an (id, value) pair */
    int k, v;
    jboolean bres; // bool result
    OCHeaderOption * rcvdOptions = resp->rcvdVendorSpecificHeaderOptions;
    /* FIXME: the OCHeaderOption struct also contains a "protocolID", deal with it */
    for (int i = 0; i < resp->numRcvdVendorSpecificHeaderOptions; i++) {

	jobject j_option = (*env)->NewObject(env, K_HASHMAP, MID_HASHMAP_CTOR);
	if (j_option == NULL) { THROW_JNI_EXCEPTION("coap options HashMap() (ctor)"); }

	k = 0; v = 0;
	/* if (((OCHeaderOption)rcvdOptions[i]).protocolID == OC_COAP_ID) */
	/*     { */
		k = ((OCHeaderOption)rcvdOptions[i]).optionID;
		jobject j_k = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, k);
		v = rcvdOptions[i].optionData[0] * 256 + rcvdOptions[i].optionData[1];
		jobject j_v = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, v);
		ores = (*env)->CallObjectMethod(env, j_option, MID_HASHMAP_PUT, j_k, j_v);
		if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }
        /*     } else { */
	/*     // error ??? */
	/* } */
	bres = (*env)->CallBooleanMethod(env, j_options, MID_ARRAYLIST_ADD, j_option);
    }
    ores = (*env)->CallObjectMethod(env, j_rmap, MID_HASHMAP_PUT, KEY_COAP_OPTIONS, j_options);

    // just to be safe:
    (*env)->DeleteLocalRef(env, j_sval);

    return j_rmap;

}
