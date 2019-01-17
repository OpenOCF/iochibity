#include "jni_init.h"

#include "org_iochibity_Exceptions.h"

//k
//#include "openocf_msg_OutboundRequest.h"
#include "openocf_Endpoint.ids.h"

int init_OutboundRequest(JNIEnv* env)
{
    jclass klass;
    klass = (*env)->FindClass(env, "openocf/OutboundRequest");
    JNI_ASSERT_NULL(klass, "FindClass failed for openocf/OutboundRequest\n", 0);
    K_OUTBOUND_REQUEST = (jclass)(*env)->NewGlobalRef(env, klass);
    (*env)->DeleteLocalRef(env, klass);

    if (MID_RQO_CTOR == NULL) {
	MID_RQO_CTOR = (*env)->GetMethodID(env, K_OUTBOUND_REQUEST,
					   "<init>", "(Lopenocf/app/ICoResourceSP;)V");
	if (MID_RQO_CTOR == NULL) {
	    printf("ERROR:  GetMethodID failed for ctor for OutboundRequest\n");
	    return OC_EH_INTERNAL_SERVER_ERROR;
	}
    }
    if (FID_RQO_LOCAL_HANDLE == NULL) {
	FID_RQO_LOCAL_HANDLE = (*env)->GetFieldID(env, K_OUTBOUND_REQUEST, "_localHandle", "J");
	if (FID_RQO_LOCAL_HANDLE == NULL) {
	    printf("ERROR: GetFieldID failed for '_localHandle' of OutboundRequest\n");
	    return OC_EH_INTERNAL_SERVER_ERROR;
	}
    }
    /* if (FID_RQO_CORESOURCE_SP == NULL) { */
    /* 	FID_RQO_CORESOURCE_SP = (*env)->GetFieldID(env, K_OUTBOUND_REQUEST, */
    /* 						       "coResourceSP", */
    /* 						       "Lopenocf/app/ICoResourceSP;"); */
    /* 	if (FID_RQO_CORESOURCE_SP == NULL) { */
    /* 	    printf("ERROR: GetFieldID failed for 'coResourceSP' of OutboundRequest\n"); */
    /* 	    return OC_EH_INTERNAL_SERVER_ERROR; */
    /* 	} */
    /* } */
    if (FID_RQO_METHOD == NULL) {
	FID_RQO_METHOD = (*env)->GetFieldID(env, K_OUTBOUND_REQUEST, "_method", "I");
	if (FID_RQO_METHOD == NULL) {
	    printf("ERROR: GetFieldID failed for '_method' of OutboundRequest\n");
	    return OC_EH_INTERNAL_SERVER_ERROR;
	}
    }
    /* if (FID_RQO_URI_PATH == NULL) { */
    /* 	FID_RQO_URI_PATH = (*env)->GetFieldID(env, K_OUTBOUND_REQUEST, */
    /* 					      "_uriPath", */
    /* 					      "Ljava/lang/String;"); */
    /* 	if (FID_RQO_URI_PATH == NULL) { */
    /* 	    printf("ERROR: GetFieldID failed for '_uriPath' of OutboundRequest\n"); */
    /* 	    return OC_EH_INTERNAL_SERVER_ERROR; */
    /* 	} */
    /* } */
    if (FID_RQO_DEST == NULL) {
	FID_RQO_DEST = (*env)->GetFieldID(env, K_OUTBOUND_REQUEST,
					  "_ep",
					  FQCS_ENDPOINT);
	if (FID_RQO_DEST == NULL) {
	    printf("ERROR: GetFieldID failed for '_ep' of OutboundRequest\n");
	    return OC_EH_INTERNAL_SERVER_ERROR;
	}
    }
    return 0;
}
