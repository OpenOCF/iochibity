/**
 * @file openocf_ResourceSP
 * @author Gregg Reynolds
 * @date December 2016
 *
 * @brief JNI implementation of ServiceProvider Java API
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "_threads.h"

//#include "openocf_app_ResourceSP.h"
/* #include "c_resource_sp.h" */
//#include "openocf_Endpoint.h"


#include "jni_init.h"
#include "org_iochibity_Exceptions.h"
#include "jni_utils.h"

#include "openocf.h"
/* #include "octypes.h" */
/* #include "ocresource.h" */
/* #include "ocstack.h" */
/* #include "oic_malloc.h" */
/* #include "oic_string.h" */

#define FQCN_IRESOURCE_SP                 "openocf/app/IResourceSP"
jclass  K_IRESOURCE_SP			= NULL; /* was K_ISERVICE_PROVIDER */
/* jmethodID MID_ISP_CTOR               = NULL; */
/* jmethodID MID_ISP_OBSERVE_REQUEST   = NULL; */

#define   FQCN_RESOURCE_SP                "openocf/app/ResourceSP"
jclass    K_RESOURCE_SP			= NULL;
jmethodID MID_RSP_REACT                  = NULL;
jfieldID  FID_RSP_HANDLE                 = NULL;
/* jfieldID  FID_RSP_ID                  = NULL; */
jfieldID  FID_RSP_URI_PATH               = NULL;
jfieldID  FID_RSP_TYPES                  = NULL;
jfieldID  FID_RSP_INTERFACES             = NULL;
jfieldID  FID_RSP_PROPERTIES             = NULL;
jfieldID  FID_RSP_CHILDREN               = NULL;
jfieldID  FID_RSP_ACTION_SET             = NULL;
jfieldID  FID_RSP_POLICIES               = NULL;

int init_ResourceSPs(JNIEnv* env)
{
    /* first, the interface IResourceSP */
    jclass klass = NULL;
    klass = (*env)->FindClass(env, FQCN_IRESOURCE_SP);
    JNI_ASSERT_NULL(klass, "FindClass failed for " FQCN_IRESOURCE_SP "\n", 0);
    K_IRESOURCE_SP = (jclass)(*env)->NewGlobalRef(env, klass);
    (*env)->DeleteLocalRef(env, klass);

    /* then, the abstract class ResourceSP */
    klass = (*env)->FindClass(env, FQCN_RESOURCE_SP);
    JNI_ASSERT_NULL(klass, FINDCLASS_FAIL(FQCN_RESOURCE_SP), 0);
    K_RESOURCE_SP = (jclass)(*env)->NewGlobalRef(env, klass);
    (*env)->DeleteLocalRef(env, klass);

    if (MID_RSP_REACT == NULL) {
	MID_RSP_REACT = (*env)->GetMethodID(env, K_RESOURCE_SP,
					    "react",
					    "(Lopenocf/message/InboundRequest;)V");
	if (MID_RSP_REACT == NULL) {
	    printf("ERROR: GetMethodID failed for 'react' of " FQCN_RESOURCE_SP "\n");
	    return -1;
	}
    }
    if (FID_RSP_HANDLE == NULL) {
	FID_RSP_HANDLE = (*env)->GetFieldID(env, K_RESOURCE_SP, "_handle", J_LONG);
	if (FID_RSP_HANDLE == NULL) {
	    printf("ERROR: GetFieldID failed for '_handle' of " FQCN_RESOURCE_SP "\n");
	    return -1;
	}
    }
    if (FID_RSP_URI_PATH == NULL) {
	FID_RSP_URI_PATH = (*env)->GetFieldID(env, K_RESOURCE_SP, "_uriPath", "Ljava/lang/String;");
	if (FID_RSP_URI_PATH == NULL) {
	    printf("ERROR: GetFieldID failed for '_uriPath' of " FQCN_RESOURCE_SP "\n");
	    return -1;
	}
    }
    if (FID_RSP_TYPES == NULL) {
	FID_RSP_TYPES = (*env)->GetFieldID(env, K_RESOURCE_SP, "_types", "Ljava/util/List;");
	if (FID_RSP_TYPES == NULL) {
	    printf("ERROR: GetFieldID failed for '_types' of " FQCN_RESOURCE_SP "\n");
	    return -1;
	}
    }
    if (FID_RSP_INTERFACES == NULL) {
	FID_RSP_INTERFACES = (*env)->GetFieldID(env, K_RESOURCE_SP, "_interfaces", "Ljava/util/List;");
	if (FID_RSP_INTERFACES == NULL) {
	    printf("ERROR: GetFieldID failed for '_interfaces' of " FQCN_RESOURCE_SP "\n");
	    return -1;
	}
    }
    if (FID_RSP_PROPERTIES == NULL) {
	FID_RSP_PROPERTIES = (*env)->GetFieldID(env, K_RESOURCE_SP, "_properties", FQCS_PMAP);
	if (FID_RSP_PROPERTIES == NULL) {
	    printf("ERROR: GetFieldID failed for '_properties' of " FQCN_RESOURCE_SP "\n");
	    return -1;
	}
    }
    if (FID_RSP_CHILDREN == NULL) {
	FID_RSP_CHILDREN = (*env)->GetFieldID(env, K_RESOURCE_SP, "_children", "Ljava/util/List;");
	if (FID_RSP_CHILDREN == NULL) {
	    printf("ERROR: GetFieldID failed for '_children' of " FQCN_RESOURCE_SP "\n");
	    return -1;
	}
    }
    if (FID_RSP_ACTION_SET == NULL) {
	FID_RSP_ACTION_SET = (*env)->GetFieldID(env, K_RESOURCE_SP, "_actionSet", "Ljava/util/List;");
	if (FID_RSP_ACTION_SET == NULL) {
	    printf("ERROR: GetFieldID failed for '_actionSet' of " FQCN_RESOURCE_SP "\n");
	    return -1;
	}
    }
    if (FID_RSP_POLICIES == NULL) {
	FID_RSP_POLICIES = (*env)->GetFieldID(env, K_RESOURCE_SP, "_policies", J_INT);
	if (FID_RSP_POLICIES == NULL) {
	    printf("ERROR: GetFieldID failed for '_policies' " FQCN_RESOURCE_SP "\n");
	    return -1;
	}
    }

    return 0;
}

/**  OBSOLETE
 * @brief Convert incoming `OCEntityHandlerRequest` to `InboundRequest`
 * object.  Internal implementation routine called by
 * `c_resource_sp_react`.  The resulting InboundRequest contains only
 * handles to the parameters.
 *
 * @param [in] env JNIEnv Pointer
 *
 * @param [in] c_EHRequest Incoming request message originating at client (CoServiceProvider)
 *
 * @param [in] c_watch_flag Indicates whether this is a `WATCH` request or an ordinary `REQUEST` request
 *
 * @return [out] j_RequestIn Java RequestIn object corresponding to `c_EHRequest` input data
 */
/* FIXME: this is called indirectly from the runtime stack, not java -
   deal with exceptions */
/* THROW_JNI_EXCEPTION won't work since we're not called from Java */
 jobject c_resource_sp_OCEntityHandlerRequest_to_RequestIn(JNIEnv* env,
							    OCEntityHandlerRequest* c_EHRequest,
							    OCEntityHandlerFlag c_watch_flag)
{
    printf("%s: %s ENTRY\n", __FILE__, __func__);

    jobject j_RequestIn = (*env)->NewObject(env, K_INBOUND_REQUEST, MID_INBOUND_REQUEST_CTOR);

    (*env)->SetLongField(env, j_RequestIn,
			 FID_MESSAGE_HANDLE, (intptr_t)c_EHRequest);

    (*env)->SetLongField(env, j_RequestIn,
			 FID_MESSAGE_HANDLE, (intptr_t)c_EHRequest);

   /* (*env)->SetLongField(env, j_RequestIn, */
    /* 			 FID_MFSP_REMOTE_RQST_HANDLE, (intptr_t)c_EHRequest->requestHandle); */
    /* (*env)->SetLongField(env, j_RequestIn, */
    /* 			 FID_MFSP_RESOURCE_HANDLE, (intptr_t)c_EHRequest->resource); */

    /* set fields in Message ancestor */
    /* OCDevAddr */
    /* jobject jdevice_address = (*env)->NewObject(env, K_ENDPOINT, MID_EP_CTOR); */
    /* (*env)->SetIntField(env, jdevice_address, FID_EP_NETWORK_PROTOCOL, c_EHRequest->devAddr.adapter); */
    /* (*env)->SetIntField(env, jdevice_address, FID_EP_NETWORK_POLICIES, c_EHRequest->devAddr.flags); */
    /* (*env)->SetIntField(env, jdevice_address, FID_EP_PORT, c_EHRequest->devAddr.port); */

    /* printf("Request-In Device Address address: %s\n", c_EHRequest->devAddr.addr); */
    /* jstring js = (*env)->NewStringUTF(env, c_EHRequest->devAddr.addr); */
    /* (*env)->SetObjectField(env, jdevice_address, FID_EP_ADDRESS, js); */
    /* (*env)->SetIntField(env, jdevice_address, FID_EP_IFINDEX, c_EHRequest->devAddr.ifindex); */

    /* (*env)->SetObjectField(env, j_RequestIn, FID_MSG_REMOTE_DEVADDR, jdevice_address); */

    /* set fields in RequestIn */
    /* js = (*env)->NewStringUTF(env, c_EHRequest->query); */
    /* (*env)->SetObjectField(env, j_RequestIn, FID_INBOUND_REQUEST_QUERY, js); */

    /* method, watch_flag and obsInfo */
    /* watch_flag =  OBSERVE | REQUEST */
    /* if watch_flag is OBSERVE, then : */
    /*     obsInfo.action = REGISTER = 0, DEREGISTER=1, NO_OPTION=2, MQ_SUBSCRIBER=3, MQ_UNSUBSCRIBER=4 */
    /*     obsInfo.Id is for ?? */
    /* NB: an OBSERVE is always also a REQUEST! e.g. flag = (OC_REQUEST_FLAG | OC_OBSERVE_FLAG) */
    /* NB: on client side, method OBSERVE is converted to GET plus Observe indicator (header option) */
    /*     on server side, method OBSERVE must be inferred from watch_flag */
    /*  so we do the obvious thing: reconstruct the OBSERVE method, but we call it WATCH */
    /* printf("WATCH FLAG:   %d\n", c_watch_flag); */
    /* printf("OBSRV ACTION: %d\n", c_EHRequest->obsInfo.action); */
    /* printf("OBSRV ID:     %d\n", c_EHRequest->obsInfo.obsId); */
    /* if (c_watch_flag & OC_OBSERVE_FLAG) { */
    /* 	if (c_EHRequest->method == OC_REST_GET) { */
    /* 	    (*env)->SetIntField(env, j_RequestIn, FID_MSG_METHOD, OC_REST_OBSERVE); */
    /* 	    /\* c_EHRequest->obsInfo.action *\/ */
    /* 	    /\* (*env)->SetIntField(env, j_RequestIn, FID_INBOUND_REQUEST_WATCH_ACTION, c_EHRequest->obsInfo.action); *\/ */
    /* 	    /\* /\\* c_EHRequest->obsInfo.Id *\\/ *\/ */
    /* 	    /\* (*env)->SetIntField(env, j_RequestIn, FID_INBOUND_REQUEST_WATCH_ID, c_EHRequest->obsInfo.obsId); *\/ */
    /* 	} else { */
    /* 	    /\* FIXME:  this should not happen? *\/ */
    /* 	} */
    /* } else { */
    /* 	(*env)->SetIntField(env, j_RequestIn, FID_MSG_METHOD, c_EHRequest->method); */
    /* } */

    /* FIXME: deal with header options: */
    /* numRcvdVendorSpecificHeaderOptions */
    /* rcvdVendorSpecificHeaderOptions; */
    /* printf("Nbr header options: %d\n", c_EHRequest->numRcvdVendorSpecificHeaderOptions); */

    /* message ID */
    /* (*env)->SetIntField(env, j_RequestIn, FID_INBOUND_REQUEST_MSG_ID, c_EHRequest->messageID); */

    /* (*env)->SetLongField(env, j_RequestIn, FID_MSG_OBSERVATION_HANDLE, (intptr_t)c_EHRequest->payload); */

    /* printf("%s: %s EXIT\n", __FILE__, __func__); */
    return j_RequestIn;
}

/* PUBLIC */
