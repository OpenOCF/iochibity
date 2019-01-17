#include "jni_init.h"

#include "org_iochibity_Exceptions.h"

// #include "openocf_message_InboundRequest.h"
#include "openocf_message_InboundRequest.ids.h"

#define FQCN_INBOUND_REQUEST "openocf/message/InboundRequest"
jclass    K_INBOUND_REQUEST                 = NULL;
jmethodID MID_INBOUND_REQUEST_CTOR          = NULL;
jfieldID  FID_INBOUND_REQUEST_HANDLE  = NULL;
jmethodID MID_INBOUND_REQUEST_IS_WATCH  = NULL;
jfieldID  FID_INBOUND_REQUEST_METHOD        = NULL;
jmethodID MID_INBOUND_REQUEST_GET_METHOD    = NULL;
jfieldID  FID_INBOUND_REQUEST_QUERY         = NULL;
jmethodID MID_INBOUND_REQUEST_GET_QUERY     = NULL;
jfieldID  FID_INBOUND_REQUEST_MSG_ID        = NULL;
jmethodID MID_INBOUND_REQUEST_GET_MSG_ID    = NULL;

    /* InboundRequest (was MsgForResourceSP) */
    /* klass = (*env)->FindClass(env, "openocf/MsgForResourceSP"); */
    /* JNI_ASSERT_NULL(klass, "FindClass failed for openocf/MsgForResourceSP\n", -1); */
    /* K_MSG_FOR_RESOURCE_SP = (jclass)(*env)->NewGlobalRef(env, klass); */
    /* (*env)->DeleteLocalRef(env, klass); */

    /* if (FID_MFSP_RESOURCE_HANDLE == NULL) { */
    /* 	FID_MFSP_RESOURCE_HANDLE = (*env)->GetFieldID(env, */
    /* 						      K_MSG_FOR_RESOURCE_SP, */
    /* 						      "_resourceHandle", "J"); */
    /* 	if (FID_MFSP_RESOURCE_HANDLE == NULL) { */
    /* 	    printf("ERROR:  GetFieldID failed for '_resourceHandle' for MsgForResourceSP"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    /* if (FID_MFSP_REMOTE_RQST_HANDLE == NULL) { */
    /* 	FID_MFSP_REMOTE_RQST_HANDLE = (*env)->GetFieldID(env, */
    /* 						      K_MSG_FOR_RESOURCE_SP, */
    /* 						      "_resourceHandle", "J"); */
    /* 	if (FID_MFSP_REMOTE_RQST_HANDLE == NULL) { */
    /* 	    printf("ERROR:  GetFieldID failed for '_resourceHandle' for MsgForResourceSP"); */
    /* 	    return -1; */
    /* 	} */
    /* } */



int init_InboundRequest(JNIEnv* env)
{
    jclass klass;
    klass = (*env)->FindClass(env, FQCN_INBOUND_REQUEST);
    JNI_ASSERT_NULL(klass, ERR_MSG(ERR_CLASS, FQCN_INBOUND_REQUEST), -1);
    K_INBOUND_REQUEST = (jclass)(*env)->NewGlobalRef(env, klass);
    (*env)->DeleteLocalRef(env, klass);

    if (MID_INBOUND_REQUEST_CTOR == NULL) {
	MID_INBOUND_REQUEST_CTOR = (*env)->GetMethodID(env, K_INBOUND_REQUEST, "<init>", "()V");
	JNI_ASSERT_NULL(MID_INBOUND_REQUEST_CTOR,
			ERR_MSG(ERR_METHOD, "InboundRequest() (ctor)"), -1);
    }

    /* _handle - ptr to InboundRequest object */
    if (FID_INBOUND_REQUEST_HANDLE == NULL) {
    	FID_INBOUND_REQUEST_HANDLE = (*env)->GetFieldID(env, K_INBOUND_REQUEST,
							       "_handle", "J");
	JNI_ASSERT_NULL(FID_INBOUND_REQUEST_HANDLE,
			ERR_MSG(ERR_FLD, "InboundRequest._handle"), -1);
    }

    if (MID_INBOUND_REQUEST_IS_WATCH == NULL) {
    	MID_INBOUND_REQUEST_IS_WATCH = (*env)->GetMethodID(env, K_INBOUND_REQUEST,
							    "isWatch", "()Z");
	JNI_ASSERT_NULL(MID_INBOUND_REQUEST_IS_WATCH,
			ERR_MSG(ERR_METHOD, "InboundRequest.isWatch"), -1);
    }

    if (MID_INBOUND_REQUEST_GET_METHOD == NULL) {
    	MID_INBOUND_REQUEST_GET_METHOD = (*env)->GetMethodID(env, K_INBOUND_REQUEST,
							      "getMethod", "()I");
	JNI_ASSERT_NULL(MID_INBOUND_REQUEST_GET_METHOD,
			ERR_MSG(ERR_METHOD, "InboundRequest.getMethod"), -1);
    }

    /* query */
    /* if (FID_INBOUND_REQUEST_QUERY == NULL) { */
    /* 	FID_INBOUND_REQUEST_QUERY = (*env)->GetFieldID(env, K_INBOUND_REQUEST, */
    /* 							"_query", "Ljava/lang/String;"); */
    /* 	JNI_ASSERT_NULL(FID_INBOUND_REQUEST_QUERY, */
    /* 			ERR_MSG(ERR_FLD, "InboundRequest._query"), -1); */
    /* } */

    if (MID_INBOUND_REQUEST_GET_QUERY == NULL) {
	MID_INBOUND_REQUEST_GET_QUERY = (*env)->GetMethodID(env, K_INBOUND_REQUEST,
						"getQueryString", "()Ljava/lang/String;");
	if (MID_INBOUND_REQUEST_GET_QUERY == NULL) {
	    printf("ERROR:  GetMethodID failed for 'getQueryString' for InboundRequest\n");
	    return -1;
	}
    }


    /* remote device address - handled by Message*/


    /* msg id */
    if (FID_INBOUND_REQUEST_MSG_ID == NULL) {
	FID_INBOUND_REQUEST_MSG_ID = (*env)->GetFieldID(env, K_INBOUND_REQUEST, "_messageId", "I");
	if (FID_INBOUND_REQUEST_MSG_ID == NULL) {
	    printf("ERROR:  GetFieldID failed for '_messageId' of InboundRequest\n");
	    return OC_EH_INTERNAL_SERVER_ERROR;
	}
    }
    /* if (MID_INBOUND_REQUEST_GET_MSG_ID == NULL) { */
    /* 	MID_INBOUND_REQUEST_GET_MSG_ID = (*env)->GetMethodID(env, K_INBOUND_REQUEST, */
    /* 							 "getRemoteDeviceAddress", */
    /* 							 "()Lopenocf/EndPoint;"); */
    /* 	if (MID_INBOUND_REQUEST_GET_MSG_ID == NULL) { */
    /* 	    printf("ERROR:  GetMethodID failed for getRemoteDeviceAddress for InboundRequest\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */

    return 0;
}

/*
 * Class:     openocf_message_InboundRequest
 * Method:    getMethod
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_openocf_message_InboundRequest_getMethod(JNIEnv * env, jobject this)
{
    OCEntityHandlerRequest *handle = (OCEntityHandlerRequest*)
	(*env)->GetLongField(env, this, FID_INBOUND_REQUEST_HANDLE);
    return (jint) handle->requestHandle->method;
}
