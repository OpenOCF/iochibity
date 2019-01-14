/**
 * @file openocf_OpenOCFClient.c
 * @author Gregg Reynolds
 * @date December 2016
 *
 * @brief implementations of `registerX` routines etc.
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

//#include "openocf_OpenOCFClient.h"
#include "openocf_app_CoResourceSP.ids.h"
#include "_openocf_app_CoResourceSP.h"
#include "jni_init.h"
#include "org_iochibity_Exceptions.h"
#include "jni_utils.h"

#include "openocf.h"
/* #include "ocresource.h" */
/* #include "ocstack.h" */
/* #include "octypes.h" */
/* #include "logger.h" */

/* PRIVATE */

/* We use tls_CoRSP to "pin" the Java object implementing the coReact callback. */
/* FIXME: make sure it gets freed appropriately */
/* FIXME: cannot be TLS */
THREAD_LOCAL jobject tls_CoRSP			= NULL;

/* FIXME: cannot be TLS */
extern THREAD_LOCAL request_out_t* tls_request_out;

#define TAG  "CO_SERVICE_MANAGER"

/* **************************************************************** */
/* PUBLIC */
/* **************************************************************** */

/*
 * Class:     openocf_engine_OCFClientSP
 * Method:    RegisterDefaultServiceRoutine
 * Signature: (Ljava/lang/Object;Ljava/lang/Object;)I
 */
JNIEXPORT void JNICALL Java_openocf_engine_OCFClientSP_registerDefaultServiceRoutine
(JNIEnv * env, jclass klass, jobject j_resource_service_provider, jobject j_callback_param)
{
    OC_UNUSED(env);
    OC_UNUSED(klass);
    OC_UNUSED(j_resource_service_provider);
    OC_UNUSED(j_callback_param);
}

/*
 * Class:     openocf_engine_OCFClientSP
 * Method:    registerDefaultCoResourceSP
 * Signature: (Lorg/iochibity/CoResourceSP;)Lorg/iochibity/CoResourceSP;
 */
JNIEXPORT jobject JNICALL Java_openocf_engine_OCFClientSP_registerDefaultCoResourceSP
(JNIEnv * env, jclass klass, jobject j_CoResourceSP)
{
    return j_CoResourceSP;
}

/*
 * Class:     openocf_engine_OCFClientSP
 * Method:    registerCoResourceSP
 * Signature: (Lorg/iochibity/CoResourceSP;)Lorg/iochibity/CoResourceSP;
 */
JNIEXPORT jobject JNICALL Java_openocf_engine_OCFClientSP_registerCoResourceSP
(JNIEnv * env, jclass klass, jobject j_CoResourceSP)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    /* pseudo-registration - coSPs call CoResourceSP.coExhibit,
       which is a native method
       Java_org_iochibity_CoResourceSP_coExhibit, which runs
       OCDoResource, which registers the coSP (resource).

       IOW, the low-level service manager registers the cosp at the
       start of each transaction, then deregisters it after response
       is handled, unless it is a multicast request and tls_deactivate
       is false

       iow, coSPs are one-off affairs, unlike SPs, which are
       registered once and for all and then start observing.
     */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return NULL;
}

/*
 * Class:     openocf_engine_OCFClientSP
 * Method:    registeredCoResourceSPs
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_openocf_engine_OCFClientSP_registeredCoResourceSPs
(JNIEnv * env, jclass klass)
{
    OIC_LOG_V(DEBUG, __FILE__, "[%d] %s ENTRY", __LINE__, __func__);
    /* OIC_LOG_DISCOVERY_RESPONSE(DEBUG, TAG, g_OCClientResponse); */

    // int oocf_cosp_mgr_list_coresource_uris(/* out */ const char ***uri_list)

    // return list of URLs?
    OIC_LOG_V(DEBUG, __FILE__, "[%d] %s EXIT", __LINE__, __func__);
    return NULL;
}

/*
 * Class:     openocf_engine_OCFClientSP
 * Method:    getRelatedCoResourceSPs
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_openocf_engine_OCFClientSP_getRelatedCoResourceSPs
 (JNIEnv * env, jclass klass)
{
    return NULL;
}

/*
 * Class:     openocf_CoResourceSP
 * Method:    coExhibit
 * Signature: ()V
 */
/**
 * @brief Wrapper for `OCDoResource`; called by called by the user to
 * send a request to a server.
 *
 * It wraps `OCDoResource`.  The parameters to `OCDoResource` are
 * reified in a hidden request_out_t struct.
 *
 * implicit params: tls_request_out, tls_response_in
 *  tls_request_out is initialized by the CoRSP ctor
 *
 *
 * @param [in] env JNIEnv pointer
 *
 * @param [in] this_CoSP reference to the calling `CoResourceSP`
 * instance
 *
 * @return void
 *
 * @throws org.iochibity.exceptions.JNIRuntimeException Indicates a
 * bad result from a JNI API routine (e.g. `GetIntField`)
 *
 * @see Java_openocf_ServiceProvider_exhibit
 * @see c_CoResourceSP_coReact
 */

/*  OBSOLETE
 * Class:     openocf_OpenOCFClient
 * Method:    coExhibit
 * Signature: (Lopenocf/message/OutboundRequest;)V
 */
/* JNIEXPORT void JNICALL */
/* Java_openocf_OpenOCFClient_coExhibit(JNIEnv *env, jclass klass, jobject the_CoRSP) */
/* { */
/*     OC_UNUSED(env); */
/*     OIC_LOG_V(DEBUG, TAG, "%s: ENTRY, tid %ld\n", __func__, (intptr_t)THREAD_ID); */

/*     /\* We will pass the_CoRSP as the callbackParam to OCDoResource. */
/*        It will be passed back on a different thread, so after we call */
/*        OCDoResource, we will store our thread-local request_out struct */
/*        in the global map, so it can later be retrieved for the */
/*        matching response.  We already have our thread-local */
/*        request_out - created by the ctor, possibly updated by app code */
/*        - so we can use it directly to parameterize OCDoReource.  Note */
/*        that request_out is an internal, implementation struct; the API */
/*        only presents CoSP getters and setters, which use request_out */
/*        as needed. */
/*     *\/ */

/*     /\* in order to be able to reliably call-back our react method, we */
/*        must pin the object *\/ */
/*     /\* FIXME: don't forget to free it. *\/ */
/*     tls_CoRSP = (*env)->NewGlobalRef(env, the_CoRSP); */
/*     OIC_LOG_V(DEBUG, TAG, "%s: this CoRSP %p\n", __func__, (void*)tls_CoRSP); */

/*     OCDoHandle c_TxnId = NULL; */
/*     txn_t* txn; */

/*     OCStackResult ret; */
/*     OCCallbackData cbData; */

/*     /\* does this request follow receipt of a response? *\/ */
/*     /\* FIXME: this logic is semi-broken.  tls_response_in will be on a */
/*        react thread, we're on an coExhibit thread, which may be */
/*        different. *\/ */

/*     if (tls_response_in == NULL) { */
/* 	/\* NULL response does not necessarily mean we're starting from */
/* 	   scratch!  it just means we're not on a react thread.  but we */
/* 	   could be (co-)reacting to a response that was recd on another */
/* 	   thread. *\/ */

/* 	/\* FIXME: use case: we're using a predefined address *\/ */
/* 	printf("tls_response_in is NULL\n"); */
/* 	printf("tls_request_out %ld\n", (intptr_t)tls_request_out); */
/* 	printf("tls_request_out->destination %ld\n", (intptr_t)tls_request_out->destination ); */
/* 	if (tls_request_out->routingIsMulticast) { */
/* 	    /\* if (tls_request_out->destination->flags & OC_MULTICAST) { *\/ */
/* 		/\* if (tls_request_out->method == OC_REST_GET) { *\/ */
/* 		tls_request_out->method = OC_REST_DISCOVER; */
/* 	    /\* } *\/ */
/* 	} */

/* 	if (tls_request_out->method == OC_REST_DISCOVER) { */
/* 	    /\* this is broken, but it's the way the C API works: *\/ */
/* 	    tls_request_out->destination = NULL; */
/* 	    tls_request_out->payload = NULL; */
/* 	    tls_request_out->routingIsMulticast = true; */
/* 	} */

/* 	cbData.cb = _openocf_app_CoResourceSP_coReact; /\* in spi_coresource_sp.c *\/ */
/* 	cbData.context = (void*)(long)tls_CoRSP; */
/* 	cbData.cd = NULL; */
/* 	ret = OCDoResource(&c_TxnId,	/\* OCDoHandle = void* *\/ */
/* 			   tls_request_out->method,      // (OCMethod)c_method, */
/* 			   tls_request_out->requestUri,  // c_uri, */
/* 			   tls_request_out->destination, // c_destDevAddr, */
/* 			   tls_request_out->payload,     // OCPayload* payload */
/* 			   tls_request_out->connectivityType, */
/* 			   /\* CT_ADAPTER_IP *\/ */
/* 			   /\* CT_FLAG_SECURE,	 /\\* OCConnectivityType conn_type *\\/ *\/ */
/* 			   OC_LOW_QOS, */
/* 			   &cbData,	/\* OCCallbackData* cbData *\/ */
/* 			   NULL,	/\* OCHeaderOptions* options *\/ */
/* 			   0);	/\* uint8_t numOptions *\/ */

/* 	printf("OCDoResource executed...\n"); */

/* 	/\* first: update the TLS request data with the newly coined txnId token *\/ */
/* 	tls_request_out->txnId = c_TxnId; */

/* 	/\* now copy tls request_out to the global list, so that we can */
/* 	 * correlate with response_in in the react routine. we cannot */
/* 	 * just refer to the TLS data since it will have gone away */
/* 	 * if/when the thread died. *\/ */
/* 	/\* why not just use the global list directly, no tls? because we */
/* 	   don't have a key (txnId) until we call OCDoResource. *\/ */
/* 	/\* FIXME: do we really need anything other than method? */
/* 	   everything else will be available in response. *\/ */

/* 	txn                         = OICCalloc(sizeof (txn_t), 1); */
/* 	txn->txnId                  = tls_request_out->txnId; */
/* 	txn->method                 = tls_request_out->method; */
/* 	if (tls_request_out->destination) { */
/* 	    txn->routingIsMulticast = tls_request_out->destination->flags & OC_MULTICAST; */
/* 	} else { */
/* 	    txn->routingIsMulticast =tls_request_out->routingIsMulticast; */
/* 	} */
/* 	txn->next                   = NULL; */

/*     /\* FIXME: now delete tls_request_out?  no point in keeping it around *\/ */

/* 	/\* size_t length; /\\* this will hold the length of string copied *\\/ *\/ */
/* 	/\* char buffer[MAX_URI_LENGTH]; /\\* fast mem on stack *\\/ *\/ */
/* 	/\* length			= strlcpy(buffer, *\/ */
/* 	/\* 				  tls_request_out->requestUri, *\/ */
/* 	/\* 				  sizeof(buffer)); *\/ */
/* 	/\* if (length < sizeof(buffer)) { *\/ */
/* 	/\*     rqst->requestUri	= buffer;	/\\* it fit, use the stack buffer *\\/ *\/ */
/* 	/\* } else { *\/ */
/* 	/\*     THROW_JNI_EXCEPTION("URI too long!"); *\/ */
/* 	/\* } *\/ */

/* 	/\* // FIXME: verify the ptr stuff is heap-allocated *\/ */
/* 	/\* rqst->destination = tls_request_out->destination; *\/ */
/* 	/\* rqst->payload = tls_request_out->payload; *\/ */
/* 	/\* rqst->connectivityType = tls_request_out->connectivityType; *\/ */
/* 	/\* rqst->qos = tls_request_out->qos; *\/ */
/* 	/\* rqst->cbData = tls_request_out->cbData; *\/ */
/* 	/\* rqst->options = tls_request_out->options; *\/ */
/* 	/\* rqst->numOptions = tls_request_out->numOptions; *\/ */
/*     } else { */

/* 	printf("tls_response_in is NOT null\n"); */
/* 	/\* we have an OCClientResponse - this CoSP is interacting with an SP *\/ */
/* 	/\* that probably - but not necessarily - means that we're */
/* 	   sending a request following a discovery? *\/ */

/*       /\* But we're coExhibiting, which means we we have a */
/*        * tls_request_out, and we may be on a "driver" thread, or we */
/*        * could be on a coReacting thread, e.g. if we are coreacting to */
/*        * a server response.  In that case, the args we need for */
/*        * OCDoResource are on a react thread, in tls_response_in.  so */
/*        * if we have a tls_response_in, that must mean that we are in */
/*        * fact on the react thread - maybe we discovered a resource and */
/*        * are now trying to read it.  in that case what we're doing now */
/*        * may be part of our reaction; but since reactions are */
/*        * triggered by requests, not responses, we should call this a */
/*        * co-response, since requests are not transactionally paired */
/*        * with previous responses. */
/*        *\/ */

/* 	/\* so a rule is: if you want to react to a server response, */
/* 	   you must do it on the thread that is running coReact. do */
/* 	   not spin off a worker thread and run coExhibit from there. */
/* 	*\/ */

/* 	/\* FIXME: a little doco and terminology will help: if a client */
/* 	   wants to react to a server's response, it can only do so by */
/* 	   starting a new interaction, which has no protocol-defined */
/* 	   relation to the previous one.  So we can call this a */
/* 	   co-reaction; the CoSP's co-reaction to a response is dual */
/* 	   to the SP's reaction to a request.  Maybe we should even */
/* 	   call the CoSP's response handler "coReact" instead of */
/* 	   "react" to highlight the difference.  Reaction will always */
/* 	   result in a response that is paired with the request, */
/* 	   whereas CoReaction never results in a response, although it */
/* 	   may involve new transactions. */
/* 	*\/ */

/*       /\* FIXME: pull method from the dual react thread *\/ */
/*       /\* TEMPORARY: pull from java object: *\/ */
/*       /\* OCMethod c_method = (OCMethod)(*env)->GetIntField(env, *\/ */
/*       /\* 							the_CoRSP, *\/ */
/*       /\* 							FID_COSP_METHOD); *\/ */
/*       /\* if (c_method == 0) { *\/ */
/*       /\* 	THROW_JNI_EXCEPTION("Method OC_REST_NOMETHOD (0) not allowed"); *\/ */
/*       /\* 	return; *\/ */
/*       /\* } *\/ */
/*       /\* uri *\/ */
/*       /\* For thread safety, we must use the Uri from the response *\/ */
/*       /\* char* c_uri tls_response_in->resourceUri, NULL); *\/ */

/*       /\* if (c_uri == NULL) { *\/ */
/*       /\* 	THROW_JNI_EXCEPTION("No resourceUri in tls_response_in\n"); *\/ */
/*       /\* 	return; *\/ */
/*       /\* } *\/ */

/*       /\* ADDRESSING *\/ */
/*       /\* MULTICAST: dev addr is null, connectivity type is used *\/ */
/*       /\* UNICAST:   dev addr is used, connectivity type is ignored? *\/ */

/*       OCDevAddr* c_destDevAddr = NULL; */
/*       /\* jobject j_destDevAddr = NULL; *\/ */
/*       /\* j_destDevAddr = (*env)->GetObjectField(env, tls_CoRSP, FID_COSP_DESTINATION); *\/ */

/*       printf("UNICASTING\n"); */
/*       /\* we have an OCDevAddr from a response *\/ */
/*       c_destDevAddr = &(tls_response_in->response->devAddr); */

/*       cbData.cb = _openocf_app_CoResourceSP_coReact; */
/*       cbData.context = (void*)(long)tls_CoRSP; */
/*       cbData.cd = NULL; */
/*       /\* ret = OCDoResource(&c_TxnId,	/\\* OCDoHandle = void* *\\/ *\/ */
/*       /\* 			 (OCMethod)c_method, *\/ */
/*       /\* 			 c_uri, *\/ */
/*       /\* 			 c_destDevAddr, *\/ */
/*       /\* 			 NULL,		 /\\* OCPayload* payload *\\/ *\/ */
/*       /\* 			 CT_DEFAULT, *\/ */
/*       /\* 			 /\\* CT_ADAPTER_IP *\\/ *\/ */
/*       /\* 			 /\\* CT_FLAG_SECURE,	 /\\\* OCConnectivityType conn_type *\\\/ *\\/ *\/ */
/*       /\* 			 OC_LOW_QOS, *\/ */
/*       /\* 			 &cbData,	/\\* OCCallbackData* cbData *\\/ *\/ */
/*       /\* 			 NULL,	/\\* OCHeaderOptions* options *\\/ *\/ */
/*       /\* 			 0);	/\\* uint8_t numOptions *\\/ *\/ */

/*       /\* now update the handle field *\/ */
/*       /\* FIXME: thread safety *\/ */
/*       /\* (*env)->SetLongField(env, tls_CoRSP, FID_COSP_HANDLE, (intptr_t)c_TxnId); *\/ */
/*     } */

/*     OIC_LOG_V(DEBUG, TAG, "%s txn id:", __func__); */
/*     OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *) c_TxnId, CA_MAX_TOKEN_LEN); */

/*     /\* FIXME: synch access to global g_txn_list *\/ */
/*     if (g_txn_list) { */
/* 	txn_t*  t = g_txn_list; */
/* 	while (t) { */
/* 	    if (t->next) { */
/* 		t = t->next; */
/* 	    } else { */
/* 		break; */
/* 	    } */
/* 	} */
/* 	t->next = txn; */
/*     } else { */
/* 	g_txn_list = txn; */
/*     } */

/*     OIC_LOG_V(DEBUG, TAG, "%s: EXIT", __func__); */
/*     OIC_LOG(DEBUG, TAG, ""); */
/*     return; */
/* } */

OCDevAddr * ep_2_devaddr(JNIEnv *env, jobject j_ep)
{
    if ( (*env)->IsSameObject(env, j_ep, NULL) )
	return NULL;
    else {
	return NULL;
    }
}

/*
 * Class:     openocf_OpenOCFClient
 * Method:    _coExhibit
 * Signature: (ILjava/lang/String;Lopenocf/utils/Endpoint;ILopenocf/app/CoResourceSP;Ljava/util/List;)V
 */
JNIEXPORT void JNICALL
Java_openocf_OpenOCFClient__1coExhibit(JNIEnv *env, jclass klass,
				       jint j_method,
				       jstring j_uri,
				       jobject j_ep,
				       jint j_qos,
				       jobject j_CoRSP,
				       jobject j_coap_options)
{
    OCDoHandle c_TxnId = NULL;
    OCStackResult ret;
    OCCallbackData cbData;

    const char *c_uri = (char*) (*env)->GetStringUTFChars(env, j_uri, NULL);

    const OCDevAddr *c_devaddr = ep_2_devaddr(env, j_ep);

    /* pin app CoRSP to global var, so it will accessible when response arrives */
    /* FIXME: don't forget to unpin it! */
    /* we will pass it as the context param to OCDoRequest */
    /* FIXME: this cannot be TLS */
    tls_CoRSP = (*env)->NewGlobalRef(env, j_CoRSP);
    OIC_LOG_V(DEBUG, TAG, "%s: this CoRSP %p\n", __func__, (void*)tls_CoRSP);

    /* First, store Method in CoRSP. We do this so that it will be
       available to the response handler (since inbound responses do
       not contain the method). This allows one CoRSP to handle
       multiple response types. */
    (*env)->SetIntField(env, j_CoRSP,
			 FID_CORSP_METHOD,
			 j_method);

    cbData.cb = _openocf_app_CoResourceSP_coReact;
    cbData.context = (void*)(long)tls_CoRSP;
    cbData.cd = NULL;

    /* OCDoResource is DEPRECATED */
    ret = OCDoRequest(&c_TxnId,	/* OCDoHandle = void* */
		      (int)j_method,	/* (OCMethod) */
		      (char*)c_uri,
		      c_devaddr,
		      NULL,     // OCPayload* payload
		      CT_DEFAULT,	      /* connectivity type */
		      /* CT_ADAPTER_IP */
		      /* CT_FLAG_SECURE,	 /\* OCConnectivityType conn_type *\/ */
		      (int)j_qos,
		      &cbData,	/* OCCallbackData* cbData */
		      NULL,	/* OCHeaderOptions* options */
		      0);	/* uint8_t numOptions */

    printf("OCDoRequest executed...\n");

    /* ret= OCDoRequest(OCDoHandle *handle, */
    /*                               OCMethod method, */
    /*                               const char *requestUri, */
    /*                               const OCDevAddr *destination, */
    /*                               OCPayload* payload, */
    /*                               OCConnectivityType connectivityType, */
    /*                               OCQualityOfService qos, */
    /*                               OCCallbackData *cbData, */
    /*                               OCHeaderOption *options, */
    /*                               uint8_t numOptions) */

    /* cleanup */
   (*env)->ReleaseStringUTFChars(env, j_uri, c_uri);

}

