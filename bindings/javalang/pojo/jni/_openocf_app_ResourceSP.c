/* This is part of the Service Programming Interface (SPI), which
   provides services to the lower-level OpenOCF stack. The service
   points in the SPI are only accessed from below; they are never
   accessed by application code. */

#include "_openocf_app_ResourceSP.h"

#include <jni.h>
#include "jni_init.h"
#include "jni_init.h"
#include "_threads.h"

#include "org_iochibity_Exceptions.h"

/**
 * @brief `react` wraps the `OCEntityHandler` callback:
 * typedef OCEntityHandlerResult (*OCEntityHandler)
 *                    (OCEntityHandlerFlag flag,
 *                     OCEntityHandlerRequest * entityHandlerRequest,
 *                     void* callbackParam);
 *
 * This routine is called by the stack on receipt of an incoming
 * request message originating at client. Calls
 * `c_resource_sp_OCEntityHandlerRequest_to_RequestIn`
 * to convert incoming request to Java object, then passes result to
 * user-defined `react` method of a `ResourceSP` object.
 *
 * @param watch_flag Indicates whether request is for WATCH (in
 * OIC-speak, OBSERVE) or not. Only two options, `OC_REQUEST_FLAG` (1)
 * or `OC_OBSERVER_FLAG` (2).  In C API, "OCEntityHandlerFlag flag".
 *
 * @param c_OCEntityHandlerRequest: inbound request
 * message.
 *
 * @param j_ResourceSP The Resource Service Provider object whose
 * `react` method will handle the request; set by
 * `ServiceManager.registerServiceProvider`.  In C API, `void*
 * callbackParam`.
 *
 * @result <OCEntityHandlerRequest> Indicates result of applying
 * user-defined handler method to incoming request.
 *
 * FIXME:
 * @see Java_org_iochibity_ServiceManager_registerServiceProvider__Lorg_iochibity_resource_sp_2
 * @see Java_org_iochibity_resource_sp_exhibit
 */

// NB: leading _ means this is a service point accessed from below
// (engine), not an app point accessed from above
OCEntityHandlerResult
_openocf_app_ResourceSP_react(OCEntityHandlerFlag watch_flag,
			      OCEntityHandlerRequest* c_OCEntityHandlerRequest,
			      void* j_ResourceSP)
{
    printf("\n%s | %s ENTRY on thread %d\n",
	   __FILE__, __func__, (int)THREAD_ID);

    /* FIXME: always called on separate thread?  multiple concurrent
       invocations get separate threads, but serialized invocations
       can reuse threads.
    */
    /* 1. set up jvm, env */
    /* http://stackoverflow.com/questions/12900695/how-to-obtain-jni-interface-pointer-jnienv-for-asynchronous-calls */
    /* http://adamish.com/blog/archives/327 */
    JNIEnv * env;
    // double check it's all ok
    int getEnvStat = (*g_JVM)->GetEnv(g_JVM, (void **)&env, JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
	/* printf("GetEnv: not attached; attaching now\n"); */
	if ((*g_JVM)->AttachCurrentThread(g_JVM, (void **) &env, NULL) != 0) {
	    printf("ERROR %s %d (%s): AttachCurrentThread failure\n", __FILE__, __LINE__,__func__);
	    return OC_EH_INTERNAL_SERVER_ERROR;
	}
    } else if (getEnvStat == JNI_OK) {
	/* printf("GetEnv: attached\n"); */
    } else if (getEnvStat == JNI_EVERSION) {
	printf("ERROR %s %d (%s): JNI version not supported\n", __FILE__, __LINE__,__func__);
	return OC_EH_INTERNAL_SERVER_ERROR;
    }
    if ((*env)->ExceptionCheck(env)) {
	(*env)->ExceptionDescribe(env);
    }

    /* validate input args */
    if (j_ResourceSP == NULL) {
	/* FIXME: use proper logging */
	printf("ERROR %s %d (%s): j_ResourceSP is NULL\n", __FILE__, __LINE__,__func__);
	return OC_EH_INTERNAL_SERVER_ERROR;
    }

    jobject j_InboundRequest = (*env)->NewObject(env, K_INBOUND_REQUEST, MID_INBOUND_REQUEST_CTOR);
    if (j_InboundRequest == NULL) {
        THROW_JNI_EXCEPTION("ERROR:  ctor InboundRequest() failed\n");
    }

    /* jobject j_RequestIn = NULL; */
    /* j_RequestIn = c_resource_sp_OCEntityHandlerRequest_to_RequestIn(env, */
    /* 								      c_OCEntityHandlerRequest, */
    /* 								      watch_flag); */

    /* fflush(NULL); */
    /* if (j_RequestIn == NULL) { */
    /*     THROW_JNI_EXCEPTION("ERROR:  OCEntityHandlerRequest_to_RequestIn failed\n"); */
    /* 	return OC_EH_INTERNAL_SERVER_ERROR; */
    /* } */

    /* now invoke the callback on the Java side */
    int op_result = OC_EH_OK;
    op_result = (*env)->CallIntMethod(env, j_ResourceSP,
				      MID_RSP_REACT,
				      j_InboundRequest);
    if (op_result != OC_STACK_OK) {
	THROW_STACK_EXCEPTION(op_result, "InboundRequest.react");
        /* printf("ERROR:  CallIntMethod failed for ResourceServiceProvider.ObserveRequest\n"); */
	return OC_EH_INTERNAL_SERVER_ERROR;
    }

    (*g_JVM)->DetachCurrentThread(g_JVM);

    /* printf("Incoming request: %ld\n", (long)c_OCEntityHandlerRequest); */
    /* printf("Incoming requestHandle: %ld\n", (long)c_OCEntityHandlerRequest->requestHandle); */
    /* printf("Incoming request param: %ld\n", (long)j_ResourceSP); */
    /* printf("Incoming request flag: %d\n", flag); */

    printf("ocf_resource_manager.c/service_routine EXIT\n");
    return op_result;
}
