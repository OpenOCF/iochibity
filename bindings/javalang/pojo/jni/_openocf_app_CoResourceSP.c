/* This is part of the Service Programming Interface (SPI), which
   provides services to the lower-level OpenOCF stack. The service
   points in the SPI are only accessed from below; they are never
   accessed by application code. */

#include "_openocf_app_CoResourceSP.h"

#include <jni.h>
#include "jni_init.h"
#include "_threads.h"

#include "openocf_app_CoResourceSP.ids.h"

#include "_openocf_app_CoResourceSP.h"

#include "org_iochibity_Exceptions.h"

THREAD_LOCAL txn_t* tls_txn;

/** OBSOLETE
 * @brief Convert OCClientResponse to an `InboundResponse` object
 *
 * Allocate an `InboundResponse` java object, then use data from incoming
 * `OCClientResponse` to initialize it.  Return the initialize object.
 * Called internally by `c_CoResourceSP_coReact`.
 *
 * @param [in] env JNIEnv pointer
 * @param [in] c_OCClientResponse response data from server
 * @return newly allocated and initialized `InboundResponse` object
 */
/* jobject OCClientResponse_to_InboundResponse(JNIEnv* env, */
/* 					    OCDoHandle c_TxnId, */
/* 					    OCClientResponse* c_OCClientResponse) */
/* { */
/*     printf("%s: OCClientResponse_to_InboundResponse ENTRY\n", __FILE__); */
/*     jobject j_InboundResponse = (*env)->NewObject(env, K_INBOUND_RESPONSE, MID_INRESP_CTOR); */
/*     if (j_InboundResponse == NULL) { */
/* 	printf("NewObject failed for InboundResponse\n"); */
/* 	return NULL; */
/*     } */

/*     /\* set the c struct handle *\/ */
/*     (*env)->SetIntField(env, j_InboundResponse, FID_MESSAGE_HANDLE, (jlong)c_OCClientResponse); */

/*     /\* OCDevAddr *\/ */
/*     jobject j_Endpoint = (*env)->NewObject(env, K_ENDPOINT, MID_EP_CTOR); */
/*     if (j_Endpoint == NULL) { */
/* 	printf("NewObject failed for Endpoint\n"); */
/* 	return NULL; */
/*     } */
/*     (*env)->SetIntField(env, j_Endpoint, */
/* 			FID_EP_NETWORK_PROTOCOL, c_OCClientResponse->devAddr.adapter); */

/*     /\* OCTransportFlags *\/ */
/*     printf("TRANSPORT FLAGS: 0x%08X\n", c_OCClientResponse->devAddr.flags); */
/*     (*env)->SetIntField(env, j_Endpoint, */
/* 			FID_EP_NETWORK_POLICIES, c_OCClientResponse->devAddr.flags >> 4); */
/*     (*env)->SetIntField(env, j_Endpoint, */
/* 			FID_EP_NETWORK_SCOPE, c_OCClientResponse->devAddr.flags & 0x000F); */
/*     (*env)->SetBooleanField(env, j_Endpoint, */
/* 			    FID_EP_TRANSPORT_SECURITY, c_OCClientResponse->devAddr.flags & 0x0010); */

/*     (*env)->SetIntField(env, j_Endpoint, */
/* 			FID_EP_PORT, c_OCClientResponse->devAddr.port); */

/*     jstring j_addr = (*env)->NewStringUTF(env, c_OCClientResponse->devAddr.addr); */
/*     (*env)->SetObjectField(env, j_Endpoint, FID_EP_ADDRESS, j_addr); */

/*     (*env)->SetIntField(env, j_Endpoint, FID_EP_IFINDEX, c_OCClientResponse->devAddr.ifindex); */

/*     /\* jstring j_route = (*env)->NewStringUTF(env, c_OCClientResponse->devAddr.routeData); *\/ */
/*     /\* (*env)->SetObjectField(env, j_Endpoint, FID_EP_ROUTE_DATA, j_route); *\/ */

/*     /\* (*env)->SetObjectField(env, j_InboundResponse, FID_INRESP_REMOTE_DEVADDR, j_Endpoint); *\/ */
    
/*     /\* connectivity type *\/ */
/*     /\* note: this duplicates info in the devaddr *\/ */
/*     printf("RESPONSE CONNECTION TYPE: 0x%08X\n",  c_OCClientResponse->connType); */
/*     (*env)->SetIntField(env, j_InboundResponse, FID_INRESP_CONN_TYPE, c_OCClientResponse->connType); */

/*     /\* FIXME: use id_length *\/ */
/*     jstring j_di = (*env)->NewStringUTF(env, (char*)c_OCClientResponse->identity.id); */
/*     (*env)->SetObjectField(env, j_InboundResponse, FID_INRESP_REMOTE_DI, j_di); */

/*     (*env)->SetIntField(env, j_InboundResponse, FID_INRESP_RESULT, c_OCClientResponse->result); */
/*     (*env)->SetIntField(env, j_InboundResponse, FID_INRESP_SERIAL, c_OCClientResponse->sequenceNumber); */

/*     jstring j_uri = (*env)->NewStringUTF(env, c_OCClientResponse->resourceUri); */
/*     (*env)->SetObjectField(env, j_InboundResponse, FID_INRESP_URI, j_uri); */

/*     (*env)->SetLongField(env, j_InboundResponse, */
/* 			 FID_INRESP_OBSERVATION_HANDLE, */
/* 			 (long)(intptr_t)c_OCClientResponse->payload); */

/*     /\* set optionCount and ptr_Options in Message *\/ */
/*     printf("%s: OCClientResponse_to_InboundResponse EXIT\n", __FILE__); */
/*     return j_InboundResponse; */
/* } */

/**
 * @brief Implementation of `OCClientResponseHandler` function type (occlientcb.c);
 *typedef OCStackApplicationResult (* OCClientResponseHandler)(void *context,
 *							      OCDoHandle handle, // GAR: misnamed, s/b txn_id
 * 						              OCClientResponse * clientResponse);
 *
 * Called by stack on receipt of incoming `OCClientResponse` from
 * server.  Fatal exceptions must thus exit the whole thing. (?)
 *
 * Stores the incoming `OCClientResponse` and `OCDoHandle` to
 * thread-local storage, then calls the `coReact` method of the
 * transaction's `CoResourceSP` object for handling. Creates and
 * passes an InboundResponse object.
 *
 * The `CoResourceSP` object is conveyed by the c_CoRSP parameter
 * (in the C API, void* context), which was passed (as the "context"
 * pointer of the `OCCallbackData` param of `OCDoResource`) by the
 * `coExhibit` routine of the `OCFClientSP`.
 *
 * IMPORTANT: this is an internal implementation routine, with no
 * header prototoype.  It serves as an intermediary between the stack
 * the the Java application's callback method.
 *
 *  @param [in] jni global c_CoRSP handle to `CoResourceSP` (client) object
 *  containing callback method; `context` in C API. must be freed with DeleteGlobalRef
 *
 * @param [in] c_TransactionHandle token identifying originating
 * `OCDoResource` invocation (not the same as the CoAP token)
 *
 * @param [in] c_OCClientResponse response data from server
 *
 * @return result code directive to retain or delete the CoRSP
 *
 * @see Java_openocf_app_CoResourceSP_coExhibit
 * @see Java_openocf_ServiceProvider_exhibit
 *
 * stored as cbData.cb and passed to OCDoResource by
 * openocf_engine_OCFClientSP.c. then called directly by stack.
 */
// was: c_CoResourceSP_coReact
OCStackApplicationResult _openocf_app_CoResourceSP_coReact(void* c_CoRSP,
							   OCDoHandle c_TxnId,
							   OCClientResponse* c_OCClientResponse)
{
    // FIXME: switch ERROR back to DEBUG
    OIC_LOG_V(DEBUG, __FILE__, "%s ENTRY, %ld", __func__, (intptr_t)THREAD_ID);
    OIC_LOG_V(DEBUG, __FILE__, "%s txn id:", __func__);
    OIC_LOG_BUFFER(DEBUG, __FILE__, (const uint8_t *) c_TxnId, CA_MAX_TOKEN_LEN);

    OIC_LOG_V(DEBUG, __FILE__, "%s c_CoRSP: %p", __func__, c_CoRSP);

    /* We're going to create an InboundResponse object and pass it to
       the client's coReact method.  We retain the data in
       thread-local storage rather than writing it to the Java object,
       so the client must not pass the object to a different
       thread. */
    tls_response_in = OICCalloc(sizeof (response_in_t), 1);
    tls_response_in->txnId    = c_TxnId;
    tls_response_in->response = c_OCClientResponse;

    /* now look up the corresponding request_out record, using c_TxnId as key. */
    if (g_txn_list) {
	tls_txn = g_txn_list;
	while (tls_txn) {
	    if ( memcmp( tls_txn->txnId, c_TxnId, CA_MAX_TOKEN_LEN ) == 0 ) {
		OIC_LOG_V(DEBUG, __FILE__, "%s: found rqst record for txn id", __func__);
		break;
	    } else {
	    	if (tls_txn->next) {
	    	    tls_txn = tls_txn->next;
	    	} else {
		    OIC_LOG_V(ERROR, __FILE__, "%s: txn key not found!", __func__);
		    tls_txn = NULL;
		    break;
	    	}
	    }
	}
    } else {
	OIC_LOG_V(ERROR, __FILE__, "%s: missing transaction list!", __func__);
    }

    /* if response is OCDiscoveryPayload (containing
       OCResourcePayloads) or OCResourceCollectionPayload (containing
       OCLinksPayloads), then database the resource records (qua remote
       SP descriptors).
     */

    /* now pass control to java app code */
    /* 1. set up: attach this thread to JVM */
    /* http://stackoverflow.com/questions/12900695/how-to-obtain-jni-interface-pointer-jnienv-for-asynchronous-calls */
    /* http://adamish.com/blog/archives/327 */
    /* detach is handled by pthread_destructor created in JNI_OnLoad */
    JNIEnv * env;
    // FIXMED double check it's all ok
    int getEnvStat = (*g_JVM)->GetEnv(g_JVM, (void **)&env, JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
	OIC_LOG_V(INFO, _FILE__, "GetEnv: not attached; attaching now\n");
	/* if ((*g_JVM)->AttachCurrentThreadAsDaemon(g_JVM, (void **) &env, NULL) != 0) { */
	if ((*g_JVM)->AttachCurrentThread(g_JVM, (void **) &env, NULL) != 0) {
	    OIC_LOG_V(ERROR, "FATAL %s %d (%s): AttachCurrentThread failure\n", __FILE__, __LINE__,__func__);
	    exit(1);
	    /* OIC_LOG_V(FATAL, __FILE__, "ERROR %s %d (%s): AttachCurrentThread failure\n", */
	    /* 	      __FILE__, __LINE__,__func__); */
	    /* return OC_STACK_DELETE_TRANSACTION; */
	}
    } else if (getEnvStat == JNI_OK) {
	OIC_LOG(INFO, __FILE__, "GetEnv: attached");
    } else if (getEnvStat == JNI_EVERSION) {
	OIC_LOG_V(ERROR, "FATAL %s %d (%s): JNI version not supported\n", __FILE__, __LINE__,__func__);
	exit(1);
	/* return OC_STACK_DELETE_TRANSACTION; */
    }
    if ((*env)->ExceptionCheck(env)) {
	OIC_LOG_V(ERROR, "FATAL %s %d (%s): EXCEPTION", __FILE__, __LINE__,__func__);
	(*env)->ExceptionDescribe(env);
    }

    /* register with pthread destructor */
    pthread_setspecific(pthread_key, (void*)env);

    /* if ctx param is null something went wrong */
    if (c_CoRSP == NULL) {
	/* printf("ERROR %s %d (%s): ctx param is NULL for c_CoResourceSP_coReact\n", */
	/*        __FILE__, __LINE__,__func__); */
	OIC_LOG_V(ERROR, __FILE__, "ERROR %s (line %d): ctx param is NULL for c_CoResourceSP_coReact",
		  __func__,__LINE__);
	return OC_STACK_DELETE_TRANSACTION;
    }

    /* switch(c_OCClientResponse->payload->type) { */
    /* case PAYLOAD_TYPE_DISCOVERY: */
    /* 	OIC_LOG_V(DEBUG, __FILE__, "[%d] %s: PAYLOAD_TYPE_DISCOVERY", __LINE__, __func__); */

    /* 	/\* register_resources(c_OCClientResponse); *\/ */

    /* 	/\* printf("VERIFICATION:\n"); *\/ */
    /* 	/\* OIC_LOG_DISCOVERY_RESPONSE(DEBUG, __FILE__, g_OCClientResponse); *\/ */
    /* 	break; */
    /* case PAYLOAD_TYPE_DEVICE: */
    /* 	OIC_LOG_V(DEBUG, __FILE__, "%s: PAYLOAD_TYPE_DEVICE", __func__); */
    /* 	/\* register_device(c_OCClientResponse); *\/ */
    /* 	break; */
    /* case PAYLOAD_TYPE_PLATFORM: */
    /* 	OIC_LOG_V(DEBUG, __FILE__, "%s: PAYLOAD_TYPE_PLATFORM", __func__); */
    /* 	/\* register_platform(c_OCClientResponse); *\/ */
    /* 	break; */
    /* case PAYLOAD_TYPE_REPRESENTATION: */
    /* 	OIC_LOG_V(DEBUG, __FILE__, "%s: PAYLOAD_TYPE_REPRESENTATION", __func__); */
    /* 	break; */
    /* case PAYLOAD_TYPE_SECURITY: */
    /* 	OIC_LOG_V(DEBUG, __FILE__, "%s: PAYLOAD_TYPE_SECURITY", __func__); */
    /* 	break; */
    /* case PAYLOAD_TYPE_PRESENCE: */
    /* 	OIC_LOG_V(DEBUG, __FILE__, "%s: PAYLOAD_TYPE_PRESENCE", __func__); */
    /* 	break; */
    /* /\* case PAYLOAD_TYPE_RD: *\/ */
    /* /\* 	OIC_LOG_V(DEBUG, __FILE__, "%s: PAYLOAD_TYPE_RD", __func__); *\/ */
    /* /\* 	break; *\/ */
    /* 	/\*GAR: v 1.3 types: *\/ */
    /* case PAYLOAD_TYPE_DIAGNOSTIC: */
    /* 	OIC_LOG_V(DEBUG, __FILE__, "%s: PAYLOAD_TYPE_DIAGNOSTIC", __func__); */
    /* 	break; */
    /* case PAYLOAD_TYPE_INTROSPECTION: */
    /* 	OIC_LOG_V(DEBUG, __FILE__, "%s: PAYLOAD_TYPE_INTROSPECTION", __func__); */
    /* 	break; */
    /* case PAYLOAD_TYPE_INVALID: */
    /* 	OIC_LOG_V(DEBUG, __FILE__, "%s: PAYLOAD_TYPE_INVALID", __func__); */
    /* 	break; */
    /* default: */
    /* 	break; */
    /* } */

    /* /\* 1. Extract ref to coSP class from the CoRSP callback param (object) *\/ */
    /* jclass k_cosp = (*env)->GetObjectClass(env, (jobject)c_CoRSP); */
    /* if((*env)->ExceptionOccurred(env)) { return OC_STACK_DELETE_TRANSACTION; } */
    /* if (k_cosp == NULL) { */
    /* 	OIC_LOG_V(ERROR, __FILE__, "[%d] %s: GetObjectClass failed for CoResourceSP object", */
    /* 	       __LINE__,__func__); */
    /* 	return OC_STACK_DELETE_TRANSACTION; */
    /* } */

    /* /\* 2. Pull the coReact method ID from the coSP class *\/ */
    /* jmethodID mid_cosp_coReact = (*env)->GetMethodID(env, k_cosp, */
    /* 						     "coReact", */
    /* 						     "(Lopenocf/message/InboundResponse;)V"); */
    /* if (mid_cosp_coReact == NULL) { */
    /* 	OIC_LOG_V(ERROR, __FILE__, */
    /* 		  "%s (line %d): GetMethodID failed for mid_cosp_coReact of CoResourceSP", */
    /* 		  __func__, __LINE__); */
    /* 	return OC_STACK_DELETE_TRANSACTION; */
    /* } */

    /* jobject j_InboundResponse = OCClientResponse_to_InboundResponse(env, */
    /* 								    c_TxnId, */
    /* 								    c_OCClientResponse); */

    jobject j_InboundResponse = (*env)->NewObject(env, K_INBOUND_RESPONSE, MID_INRESP_CTOR);
    if (j_InboundResponse == NULL) {
	THROW_JNI_EXCEPTION("InboundResponse() (ctor)");;
    }

    /* 3. call user's coReact method of the CoResourceSP */
    OIC_LOG_V(INFO, __FILE__, "[%d] %s: Calling user coReact method", __LINE__, __func__);
    int coReact_result;
    /* coReact_result = (*env)->CallIntMethod(env, (jobject)c_CoRSP, */
    /* 					   mid_cosp_coReact, j_InboundResponse); */

    (*env)->CallVoidMethod(env, (jobject)c_CoRSP,
			  MID_ICORSP_COREACT, j_InboundResponse);
			  /* mid_cosp_coReact, j_InboundResponse); */

    jthrowable e = (*env)->ExceptionOccurred(env);
    if(e) {
	OIC_LOG_V(ERROR, __FILE__, "CAUGHT EXCEPTION thrown by user coReact method");

	jclass throwable_class = (*env)->FindClass(env, "java/lang/Throwable");

	/* jmethodID mid_throwable_getCause = */
	/*     (*env)->GetMethodID(env, throwable_class, */
	/* 			"getCause", */
	/* 			"()Ljava/lang/Throwable;"); */
	/* jmethodID mid_throwable_getStackTrace = */
	/*     (*env)->GetMethodID(env, throwable_class, */
	/* 			"getStackTrace", */
	/* 			"()[Ljava/lang/StackTraceElement;"); */
	jmethodID mid_throwable_toString =
	    (*env)->GetMethodID(env, throwable_class,
				"toString",
				"()Ljava/lang/String;");
	jmethodID mid_throwable_getMessage =
	    (*env)->GetMethodID(env, throwable_class,
				"getMessage",
				"()Ljava/lang/String;");

/* jclass frame_class = (*env)->FindClass("java/lang/StackTraceElement"); */
/* jmethodID mid_frame_toString = */
/*     (*env)->GetMethodID(frame_class, */
/*                       "toString", */
/*                       "()Ljava/lang/String;"); */

	/* jclass clazz = (*env)->GetObjectClass(env, e); */
	/* if (clazz == NULL) { */
	/*     OIC_LOG_V(ERROR, __FILE__, "GetObjectClass failed foor throwable"); */
	/* } */

	/* jmethodID getMessage = (*env)->GetMethodID(env, clazz, */
	/* 					"getMessage", */
	/* 					"()Ljava/lang/String;"); */

	jstring message = (jstring)(*env)->CallObjectMethod(env, e, mid_throwable_getMessage);
	jstring s = (jstring)(*env)->CallObjectMethod(env, e, mid_throwable_toString);
	//OIC_LOG_V(ERROR, __FILE__, "BAAAR %s", message);

	const char *mstr = (*env)->GetStringUTFChars(env, s, NULL);

	OIC_LOG_V(ERROR, __FILE__, "[%d]: %s", __LINE__, s);

	(*env)->ExceptionClear(env); // clears the exception; e seems to remain valid
	(*env)->ReleaseStringUTFChars(env, message, mstr);
	(*env)->DeleteLocalRef(env, message);
	(*env)->DeleteLocalRef(env, throwable_class);
	// FIXME: delete the methods
	(*env)->DeleteLocalRef(env, e);

	return OC_STACK_DELETE_TRANSACTION;
    }

    /* we're done with the JVM */
    /* FIXME: free everything - this routine does not return to the JVM */


    /* if (tls_deactivate) { */
    /* 	/\* free the global cosp that we pinned in coExhibit *\/ */
    /* 	(*env)->DeleteGlobalRef(env, (jobject)c_CoRSP); */
    /* 	OIC_LOG(DEBUG, __FILE__, "c_CoResourceSP_react EXIT, deactivating handler"); */
    /* 	return OC_STACK_DELETE_TRANSACTION; */
    /* } else { */

    /* 	if (tls_txn->routingIsMulticast) { */
    /* 	    OIC_LOG_V(DEBUG, __FILE__, "[%d] %s EXITing multicast handler", __LINE__, __func__); */
    /* 	    return coReact_result; */
    /* 	    /\* return (OC_STACK_KEEP_TRANSACTION); *\/ */
    /* 	    /\* return (OC_STACK_KEEP_TRANSACTION | OC_STACK_KEEP_PAYLOAD); *\/ */
    /* 	    /\* return (OC_STACK_DELETE_TRANSACTION | OC_STACK_KEEP_PAYLOAD); *\/ */
    /* 	} else { */
    /* 	    /\* free the global cosp that we pinned in coExhibit *\/ */
    /* 	    (*env)->DeleteGlobalRef(env, (jobject)c_CoRSP); */
    /* 	    OIC_LOG_V(DEBUG, __FILE__, "[%d] %s EXITing unicast handler", */
    /* 		    __LINE__, __func__); */
    /* 	    /\* always remove unicast CB *\/ */
    /* 	    return (coReact_result & OC_STACK_DELETE_TRANSACTION); */
    /* 	} */

    /* retention */
    /* Client is responsible for retaining txn and msg */
    /* it does this by setting the RETAIN flags on the CoRSP and InboundResponse */

    bool retain_msg = (*env)->GetBooleanField(env, j_InboundResponse, FID_INRESP_IS_RETAIN);

    bool retain_corsp = (*env)->GetBooleanField(env, (jobject)c_CoRSP, FID_CORSP_IS_RETAIN);

    OCStackApplicationResult result = 0;
    if (retain_msg) result |= OC_STACK_KEEP_RESPONSE;
    if (retain_corsp) result |=  OC_STACK_KEEP_TRANSACTION;

    return result;
}
