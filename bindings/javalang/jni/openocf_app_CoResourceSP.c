/**
 * @file openocf_app_CoResourceSP.c
 * @author Gregg Reynolds
 * @date December 2016
 *
 * @brief JNI implementation of CoResourceSP (client) Java API:
 * `Java_openocf_app_CoResourceSP_coExhibit` and callback
 * `c_CoResourceSP_coReact`
 */

#include <string.h>
/* #include <ctype.h> */
/* #include <stdlib.h> */

/* http://www.informit.com/articles/article.aspx?p=2036582&seqNum=5
OpenBSD:  strlcpy
C11:      strncpy_s
*/

#include "_threads.h"

//#include "openocf_app_CoResourceSP.h"
#include "openocf_app_CoResourceSP.ids.h"
/* #include "_openocf_app_ClientSP.h" */
#include "_openocf_app_CoResourceSP.h"
#include "openocf_utils_Endpoint.ids.h"
#include "jni_utils.h"
#include "jni_init.h"
#include "org_iochibity_Exceptions.h"

#include "openocf.h"
#include "oic_malloc.h"
/* #include "oic_string.h" */
/* #include "ocpayload.h" */
/* #include "ocresource.h" */
/* #include "ocresourcehandler.h" */
/* #include "ocstack.h" */

/* #include "logger.h" */

/* logging */
#define TAG  "CoSP"

/* externs */

/*
  multithreading: each cosp calls into this to set up and send
  requests.  the rule is one thread per request, i.e. the cosp must
  be allocked on the same thread that calls in here.  that allows us
  to us TLS to store request state.  and this allows us to use one
  cosp object for various requests.
*/

/* _Thread_local OCDevAddr* tls_defaultCoAddress		= NULL; */

/* PRIVATE */

txn_t* g_txn_list = NULL;

THREAD_LOCAL request_out_t* tls_request_out = NULL;

THREAD_LOCAL response_in_t* tls_response_in	= NULL;

response_in_t*  g_response_map;	/* ???  */

THREAD_LOCAL bool   tls_deactivate;

#define FQCN_ICORESOURCE_SP "openocf/app/ICoResourceSP"
#define FQCS_ICORESOURCE_SP "Lopenocf/app/ICoResourceSP;"
jclass    K_ICORESOURCE_SP         = NULL; /* was: K_I_CO_RESOURCE_SP */
jmethodID MID_ICORSP_COREACT       = NULL;

jclass    K_CORESOURCE_SP          = NULL;
jmethodID MID_CORSP_COREACT        = NULL;

/* jfieldID  FID_CORSP_HANDLE      = NULL; */
jfieldID  FID_CORSP_IS_RETAIN      = NULL;
jfieldID  FID_CORSP_URI            = NULL;
jfieldID  FID_CORSP_METHOD         = NULL;
/* jfieldID  FID_CORSP_DESTINATION = NULL; */
jmethodID MID_CORSP_EXHIBIT        = NULL;

#define TYPSIG_CORESOURCESP "openocf/app/CoResourceSP"

int init_ICoResourceSP(JNIEnv* env)
{
    jclass klass;
    klass = (*env)->FindClass(env, FQCN_ICORESOURCE_SP);
    JNI_ASSERT_NULL(klass, "FindClass failed for openocf/app/ICoResourceSP\n", 0);
    K_ICORESOURCE_SP = (jclass)(*env)->NewGlobalRef(env, klass);
    (*env)->DeleteLocalRef(env, klass);

    if (MID_ICORSP_COREACT == NULL) {
	MID_ICORSP_COREACT = (*env)->GetMethodID(env, K_ICORESOURCE_SP,
						"coReact",
						 "(Lopenocf/message/InboundResponse;)V");
	if (MID_ICORSP_COREACT == NULL) {
	    printf("ERROR: GetMethodID failed for 'coReact' of ICoResourceSP\n");
	    return OC_EH_INTERNAL_SERVER_ERROR;
	}
    }
    return 0;
}

int init_CoResourceSP(JNIEnv* env)
{
    jclass klass;
    klass = (*env)->FindClass(env, TYPSIG_CORESOURCESP);
    JNI_ASSERT_NULL(klass, "FindClass failed for " TYPSIG_CORESOURCESP "\n", 0);
    K_CORESOURCE_SP = (jclass)(*env)->NewGlobalRef(env, klass);
    (*env)->DeleteLocalRef(env, klass);

    if (MID_CORSP_COREACT == NULL) {
	MID_CORSP_COREACT = (*env)->GetMethodID(env, K_CORESOURCE_SP,
						"coReact",
						 "(Lopenocf/message/InboundResponse;)V");
	if (MID_CORSP_COREACT == NULL) {
	    printf("ERROR: GetMethodID failed for 'coReact' of CoResourceSP\n");
	    return OC_EH_INTERNAL_SERVER_ERROR;
	}
    }

    if (FID_CORSP_IS_RETAIN == NULL) {
    	FID_CORSP_IS_RETAIN = (*env)->GetFieldID(env, K_CORESOURCE_SP,
						  "isRetain", "Z");
	JNI_ASSERT_NULL(FID_CORSP_IS_RETAIN,
			ERR_MSG(ERR_FLD, "CoResourceSP.isRetain"), -1);
    }

    if (FID_CORSP_URI == NULL) {
    	FID_CORSP_URI = (*env)->GetFieldID(env, K_CORESOURCE_SP,
					   "_uri", "Ljava/lang/String;");
	JNI_ASSERT_NULL(FID_CORSP_URI,
			ERR_MSG(ERR_FLD, "CoResourceSP._uri"), -1);
    }

    if (FID_CORSP_METHOD == NULL) {
    	FID_CORSP_METHOD = (*env)->GetFieldID(env, K_CORESOURCE_SP,
					      "_method", "I");
	JNI_ASSERT_NULL(FID_CORSP_METHOD,
			ERR_MSG(ERR_FLD, "CoResourceSP._method"), -1);
    }

    //FIXME: do we need coExhibit as an SPI point? who will call it?
    /* jmethodID MID_CORSP_EXHIBIT  = NULL; */
    /* if (MID_CORSP_EXHIBIT == NULL) { */
    /* 	MID_CORSP_EXHIBIT = (*env)->GetMethodID(env, K_ICORESOURCE_SP, */
    /* 						"coExhibit", "()V"); */
    /* 	if (MID_CORSP_EXHIBIT == NULL) { */
    /* 	    printf("ERROR: GetMethodID failed for 'coExhibit' of CoResourceSP\n"); */
    /* 	    return OC_EH_INTERNAL_SERVER_ERROR; */
    /* 	} */
    /* } */

    return 0;
}

/*
 * Class:     openocf_CoResourceSP
 * Method:    deactivate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_openocf_app_CoResourceSP_deactivate
(JNIEnv * env, jobject this)
{
    OC_UNUSED(env);
    OC_UNUSED(this);
    tls_deactivate = true;
}
