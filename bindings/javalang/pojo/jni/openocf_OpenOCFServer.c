/**
 * @file openocf_OpenOCFServer.c
 * @author Gregg Reynolds
 * @date December 2016
 *
 * @brief implementations of `registerX` routines
 */

#include "openocf.h"
//#include "openocf_OpenOCFServer.h"
#include "_openocf_app_ResourceSP.h"
#include "jni_init.h"
#include "org_iochibity_Exceptions.h"
#include "jni_utils.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* PRIVATE */


/* **************************************************************** */
/* PUBLIC */
/* **************************************************************** */

/** FIXME: move to OCFServerSP
 * @brief  Wrapper on `OCDoResponse`.
 *
 *
 * Example usage:
 * @code{.java}
 * ...
 * this.exhibit();
 * ...
 * @endcode
  *
 * @see c_org_iochibity_CoServiceProvider_observe_behavior
 *
 * @see Java_org_iochibity_CoServiceProvider_exhibitRequest
 *
 * @see c_resource_sp_react
 *
 */
// public static native void exhibit(ResourceSP RSP);
JNIEXPORT void JNICALL Java_openocf_engine_OCFServerSP_exhibit(JNIEnv *env, jclass kilass, jobject this_SP)
{
    printf("%s | %s: ENTRY on thread %d\n", __FILE__, __func__, (int)THREAD_ID);

    /* /\* prep_java(env); *\/ */

    /* /\* 1. alloc the OCEntityHandlerResponse struct *\/ */
    /* OCEntityHandlerResponse * c_response_out = */
    /*                     (OCEntityHandlerResponse *) OICCalloc(1, sizeof(*c_response_out)); */
    /* if (!c_response_out) { */
    /* 	THROW_EH_EXCEPTION(OC_EH_ERROR, "OCEntityHandlerResponse calloc failure\n"); */
    /* 	return; */
    /* } */
    /* /\* get the RequestIn object *\/ */
    /* jobject j_RequestIn = (*env)->GetObjectField(env, j_ObservationOut, FID_MsgRspOut_RQST_IN); */
    /* if (j_RequestIn == NULL) { */
    /* 	printf("ERROR: GetObjectField failed for FID_MsgRspOut_RQST_IN on j_ObservationOut\n"); */
    /* 	return; */
    /* } */
    /* /\* now we have the original RequestIn object; let's pull the handles from it *\/ */

    /* printf("BBBBBBBBBBBBBBBB %ld\n", (intptr_t)j_RequestIn); */

    /* /\* first we need the RequestIn class so we can get the c struct handle *\/ */
    /* jclass k_request_in = (*env)->GetObjectClass(env, j_RequestIn); */
    /* if (k_request_in == NULL) { */
    /* 	printf("ERROR: GetObjectClass failed for RequestIn object\n"); */
    /* 	return; */
    /* } */

    /* /\* get the localHandle *\/ */
    /* /\* jfieldID fid_local_handle = (*env)->GetFieldID(env, k_request_in, "localHandle", "J"); *\/ */
    /* /\* if (fid_local_handle == NULL) { *\/ */
    /* /\* 	printf("ERROR: GetFieldID failed for 'localHandle' on RequestIn\n"); *\/ */
    /* /\* 	return; *\/ */
    /* /\* } *\/ */

    /* printf("AAAAAAAAAAAAAAAA\n"); */
    /* OCEntityHandlerRequest* j_handle = (OCEntityHandlerRequest*)(intptr_t) */
    /* 	(*env)->GetLongField(env, j_RequestIn, FID_INBOUND_REQUEST_LOCAL_HANDLE); */
    /* if (j_handle == NULL) { */
    /* 	printf("ERROR: GetObjectField failed for FID_LOCAL_HANDLE on j_RequestIn\n"); */
    /* 	return; */
    /* } */
    /* printf("RESPONSE localHandle: %ld\n", (long)j_handle); */
    /* printf("RESPONSE remoteHandle: %ld\n", (long)j_handle->requestHandle); */
    /* c_response_out->requestHandle = j_handle->requestHandle; */

    /* /\* ditto for resourceHandle *\/ */
    /* printf("RESPONSE resourceHandle: %ld\n", (long)j_handle->resource); */
    /* c_response_out->resourceHandle = j_handle->resource; */

    /* /\* defaults *\/ */
    /* c_response_out->numSendVendorSpecificHeaderOptions = 0; */

    /* memset(c_response_out->sendVendorSpecificHeaderOptions, 0, */
    /* 	   sizeof c_response_out->sendVendorSpecificHeaderOptions); */
    /* memset(c_response_out->resourceUri, 0, sizeof(c_response_out->resourceUri)); */

    /* c_response_out->persistentBufferFlag = 0; */

    /* /\* ObservationList *\/ */
    /* jfieldID fid_pll = (*env)->GetFieldID(env, K_MSG_RESPONSE_OUT, */
    /* 					  "_observationList", "Lorg/iochibity/ObservationList;"); */
    /* if (fid_pll == NULL) { */
    /* 	printf("ERROR: GetFieldID failed for '_observationList' on ResponseOut\n"); */
    /* 	return; */
    /* } */
    /* jobject j_pll = (*env)->GetObjectField(env, j_ObservationOut, fid_pll); */
    /* if (j_pll != NULL) { */
    /* 	jclass k_pll = (*env)->GetObjectClass(env, j_pll); */
    /* 	if (k_pll == NULL) { */
    /* 	    printf("ERROR: GetObjectClass failed for ObservationList object\n"); */
    /* 	    return; */
    /* 	} */
    /* 	/\* we're going to iterate using jni, dunno about efficiency *\/ */
    /* 	jmethodID mid_pll_size = (*env)->GetMethodID(env, k_pll, "size", "()I"); */
    /* 	if (mid_pll_size == NULL) { */
    /* 	    printf("ERROR: GetMethodID failed for 'size' of ObservationList\n"); */
    /* 	    return; */
    /* 	} */
    /* 	int sz = 0; */
    /* 	sz = (*env)->CallIntMethod(env, j_pll, mid_pll_size); */
    /* 	printf("ObservationList SIZE: %d\n", sz); */

    /* 	jmethodID mid_pll_get = (*env)->GetMethodID(env, k_pll, "get", "(I)Ljava/lang/Object;"); */
    /* 	if (mid_pll_get == NULL) { */
    /* 	    printf("ERROR: GetMethodID failed for 'get' of ObservationList\n"); */
    /* 	    return; */
    /* 	} */

    /* 	/\* pfrs = PayloadForResourceState *\/ */
    /* 	/\* prep_pfrs(env); /\\* get all the field and method ids etc. needed for iteration *\\/ *\/ */

    /* 	jobject j_observation = NULL; */
    /* 	int c_type; */
    /* 	OCPayload* c_payload = NULL; */
    /* 	for (int i=0; i<sz; i++) { */
    /* 	    j_observation = (*env)->CallObjectMethod(env, j_pll, mid_pll_get, i); */
    /* 	    c_type = (*env)->GetIntField(env, j_observation, FID_OBSERVATION_TYPE); */
    /* 	    printf("OBSERVATION TYPE: %d\n", c_type); */

    /* 	    /\* the OCPayload structs are already there in the _handle fields *\/ */
    /* 	    /\* but we need to synch them with the Java objects *\/ */
    /* 	    switch(c_type) { */
    /* 	    case PAYLOAD_TYPE_INVALID: */
    /* 		printf("INVALID PAYLOAD TYPE NOT SUPPORTED\n"); */
    /* 		break; */
    /* 	    case PAYLOAD_TYPE_DISCOVERY: */
    /* 		printf("DISCOVERY PAYLOAD NOT YET SUPPORTED\n"); */
    /* 		break; */
    /* 	    case PAYLOAD_TYPE_DEVICE: */
    /* 		printf("DEVICE PAYLOAD NOT YET SUPPORTED\n"); */
    /* 		break; */
    /* 	    case PAYLOAD_TYPE_PLATFORM: */
    /* 		printf("PLATFORM PAYLOAD NOT YET SUPPORTED\n"); */
    /* 		break; */
    /* 	    case PAYLOAD_TYPE_REPRESENTATION: */
    /* 		c_payload = (OCPayload*)pfrs_to_OCRepPayload(env, j_observation); */
    /* 		break; */
    /* 	    case PAYLOAD_TYPE_SECURITY: */
    /* 		printf("SECURITY PAYLOAD NOT YET SUPPORTED\n"); */
    /* 		break; */
    /* 	    case PAYLOAD_TYPE_PRESENCE: */
    /* 		printf("PRESENCE PAYLOAD NOT YET SUPPORTED\n"); */
    /* 		break; */
    /* 		/\* obsolete as of 1.2.1: *\/ */
    /* 		/\* case PAYLOAD_TYPE_RD: *\/ */
    /* 		/\*     printf("RD PAYLOAD NOT YET SUPPORTED\n"); *\/ */
    /* 		/\*     break; *\/ */
    /* 	    default: */
    /* 		printf("UNKNOWN PAYLOAD - NOT SUPPORTED\n"); */
    /* 		break; */
    /* 	    } */
    /* 	} */
    /* 	c_response_out->payload = (OCPayload*) c_payload; */
    /* } else { */
    /* 	/\* OK: use left the payload null *\/ */
    /* 	/\* printf("ERROR: GetObjectField failed for fid_pll on j_ObservationOut\n"); *\/ */
    /* 	/\* return; *\/ */
    /* } */

    /* /\* set OCEntityHandlerResult from this._processingResult *\/ */

    /* /\* vendor header options *\/ */

    /* /\* payload: iterate over the PayloadList to create OCPayload linked list *\/ */


    /* /\* finally, send it off: *\/ */
    /* OCStackResult op_result = OC_STACK_OK; */
    /* op_result = OCDoResponse(c_response_out); */
    /* if (op_result != OC_STACK_OK) { */
    /* 	THROW_STACK_EXCEPTION(op_result, "OCDoResponse failed"); */
    /* 	/\* ehResult = OC_EH_ERROR; *\/ */
    /* 	return; */
    /* } */
    printf("%s | %s: EXIT\n", __FILE__, __func__);
}


/*
 * Class:     openocf_engine_OCFServerSP
 * Method:    resourceCount
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL Java_openocf_engine_OCFServerSP_resourceCount
(JNIEnv * env, jclass klass)
{
    OC_UNUSED(env);
    OC_UNUSED(klass);
    uint8_t count = 0;
    OCStackResult op_result = OCGetNumberOfResources(&count);
    if ( op_result != OC_STACK_OK ) {
	// err
    }
    return count;
}

/* /\* */
/*  * Class:     openocf_engine_OCFServerSP */
/*  * Method:    configurePlatformSP */
/*  * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V */
/*  *\/ */
/* JNIEXPORT void JNICALL Java_openocf_engine_OCFServerSP_configurePlatformSP */
/*   (JNIEnv * env, jclass klass, */
/*    jstring j_platform_id, */
/*    jstring j_mfg_name, */
/*    jstring j_mfg_url, */
/*    jstring j_model_number, */
/*    jstring j_mfg_date, */
/*    jstring j_platform_version, */
/*    jstring j_os_version, */
/*    jstring j_hw_version, */
/*    jstring j_fw_version, */
/*    jstring j_support_url, */
/*    jstring j_sys_time) */
/* { */
/*     OC_UNUSED(klass); */
/*     static OCPlatformInfo platform_info = */
/* 	{ */
/* 	    .platformID			= "", */
/* 	    .manufacturerName		= "", */
/* 	    .manufacturerUrl		= "", */
/* 	    .modelNumber		= "", */
/* 	    .dateOfManufacture		= "", */
/* 	    .platformVersion		= "", */
/* 	    .operatingSystemVersion	= "", */
/* 	    .hardwareVersion		= "", */
/* 	    .firmwareVersion		= "", */
/* 	    .supportUrl			= "", */
/* 	    .systemTime			= ""   /\* "2015-05-15T11.04" *\/ */
/* 	}; */

/*     if (j_platform_id == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_platform_id = (*env)->NewStringUTF(env, platform_info.platformID); */
/* 	platform_info.platformID = (char*) (*env)->GetStringUTFChars(env, j_platform_id, NULL); */
/*     } else { */
/* 	platform_info.platformID = (char*) (*env)->GetStringUTFChars(env, j_platform_id, NULL); */
/* 	if (platform_info.platformID == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c platform id: [%s]\n", platform_info.platformID); */

/*     if (j_mfg_name == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_mfg_name = (*env)->NewStringUTF(env, platform_info.manufacturerName); */
/* 	platform_info.manufacturerName = (char*) (*env)->GetStringUTFChars(env, j_mfg_name, NULL); */
/*     } else { */
/* 	platform_info.manufacturerName = (char*) (*env)->GetStringUTFChars(env, j_mfg_name, NULL); */
/* 	if (platform_info.manufacturerName == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c mfg name: [%s]\n", platform_info.manufacturerName); */

/*     if (j_mfg_url == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_mfg_url = (*env)->NewStringUTF(env, platform_info.manufacturerUrl); */
/* 	platform_info.manufacturerUrl = (char*) (*env)->GetStringUTFChars(env, j_mfg_url, NULL); */
/*     } else { */
/* 	platform_info.manufacturerUrl = (char*) (*env)->GetStringUTFChars(env, j_mfg_url, NULL); */
/* 	if (platform_info.manufacturerUrl == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c mfg url: [%s]\n", platform_info.manufacturerUrl); */

/*     if (j_model_number == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_model_number = (*env)->NewStringUTF(env, platform_info.modelNumber); */
/* 	platform_info.modelNumber = (char*) (*env)->GetStringUTFChars(env, j_model_number, NULL); */
/*     } else { */
/* 	platform_info.modelNumber = (char*) (*env)->GetStringUTFChars(env, j_model_number, NULL); */
/* 	if (platform_info.modelNumber == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c model nbr: [%s]\n", platform_info.modelNumber); */

/*     if (j_mfg_date == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_mfg_date = (*env)->NewStringUTF(env, platform_info.dateOfManufacture); */
/* 	platform_info.dateOfManufacture = (char*) (*env)->GetStringUTFChars(env, j_mfg_date, NULL); */
/*     } else { */
/* 	platform_info.dateOfManufacture = (char*) (*env)->GetStringUTFChars(env, j_mfg_date, NULL); */
/* 	if (platform_info.dateOfManufacture == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c mfg date: [%s]\n", platform_info.dateOfManufacture); */

/*     if (j_platform_version == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_platform_version = (*env)->NewStringUTF(env, platform_info.platformVersion); */
/* 	platform_info.platformVersion = (char*) (*env)->GetStringUTFChars(env, j_platform_version, NULL); */
/*     } else { */
/* 	platform_info.platformVersion = (char*) (*env)->GetStringUTFChars(env, j_platform_version, NULL); */
/* 	if (platform_info.platformVersion == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c platform version: [%s]\n", platform_info.platformVersion); */

/*     if (j_os_version == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_os_version = (*env)->NewStringUTF(env, platform_info.operatingSystemVersion); */
/* 	platform_info.operatingSystemVersion = (char*) (*env)->GetStringUTFChars(env, j_os_version, NULL); */
/*     } else { */
/* 	platform_info.operatingSystemVersion = (char*) (*env)->GetStringUTFChars(env, j_os_version, NULL); */
/* 	if (platform_info.operatingSystemVersion == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c os version: [%s]\n", platform_info.operatingSystemVersion); */

/*     if (j_hw_version == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_hw_version = (*env)->NewStringUTF(env, platform_info.hardwareVersion); */
/* 	platform_info.hardwareVersion = (char*) (*env)->GetStringUTFChars(env, j_hw_version, NULL); */
/*     } else { */
/* 	platform_info.hardwareVersion = (char*) (*env)->GetStringUTFChars(env, j_hw_version, NULL); */
/* 	if (platform_info.hardwareVersion == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c hw version: [%s]\n", platform_info.hardwareVersion); */

/*     if (j_fw_version == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_fw_version = (*env)->NewStringUTF(env, platform_info.firmwareVersion); */
/* 	platform_info.firmwareVersion = (char*) (*env)->GetStringUTFChars(env, j_fw_version, NULL); */
/*     } else { */
/* 	platform_info.firmwareVersion = (char*) (*env)->GetStringUTFChars(env, j_fw_version, NULL); */
/* 	if (platform_info.firmwareVersion == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c firmware version: [%s]\n", platform_info.firmwareVersion); */

/*     if (j_support_url == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_support_url = (*env)->NewStringUTF(env, platform_info.supportUrl); */
/* 	platform_info.supportUrl = (char*) (*env)->GetStringUTFChars(env, j_support_url, NULL); */
/*     } else { */
/* 	platform_info.supportUrl = (char*) (*env)->GetStringUTFChars(env, j_support_url, NULL); */
/* 	if (platform_info.supportUrl == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c support url: [%s]\n", platform_info.supportUrl); */

/*     if (j_sys_time == NULL) { */
/* 	printf("platform id is null\n"); */
/* 	j_sys_time = (*env)->NewStringUTF(env, platform_info.systemTime); */
/* 	platform_info.systemTime = (char*) (*env)->GetStringUTFChars(env, j_sys_time, NULL); */
/*     } else { */
/* 	platform_info.systemTime = (char*) (*env)->GetStringUTFChars(env, j_sys_time, NULL); */
/* 	if (platform_info.systemTime == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c system time: [%s]\n", platform_info.systemTime); */

/*     printf("Setting platform info...\n"); */
/*     OCStackResult op_result; */
/*     op_result = OCSetPlatformInfo(platform_info); */
/*     if (op_result != OC_STACK_OK) { */
/* 	// FIXME do sth! */
/*     } */

/*     (*env)->ReleaseStringUTFChars(env, j_platform_id, platform_info.platformID); */
/*     (*env)->ReleaseStringUTFChars(env, j_mfg_name, platform_info.manufacturerName); */
/*     (*env)->ReleaseStringUTFChars(env, j_mfg_url, platform_info.manufacturerUrl); */
/*     (*env)->ReleaseStringUTFChars(env, j_model_number, platform_info.modelNumber); */
/*     (*env)->ReleaseStringUTFChars(env, j_mfg_date, platform_info.dateOfManufacture); */
/*     (*env)->ReleaseStringUTFChars(env, j_platform_version, platform_info.platformVersion); */
/*     (*env)->ReleaseStringUTFChars(env, j_os_version, platform_info.operatingSystemVersion); */
/*     (*env)->ReleaseStringUTFChars(env, j_hw_version, platform_info.hardwareVersion); */
/*     (*env)->ReleaseStringUTFChars(env, j_fw_version, platform_info.firmwareVersion); */
/*     (*env)->ReleaseStringUTFChars(env, j_support_url, platform_info.supportUrl); */
/*     (*env)->ReleaseStringUTFChars(env, j_sys_time, platform_info.systemTime); */
/* } */

/* /\* */
/*  * Class:     openocf_engine_OCFServerSP */
/*  * Method:    configureDeviceSP */
/*  * Signature: (Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)V */
/*  *\/ */
/* JNIEXPORT void JNICALL Java_openocf_engine_OCFServerSP_configureDeviceSP */
/*   (JNIEnv * env, jclass klass, */
/*    jstring j_device_name, */
/*    jobjectArray j_types, */
/*    jstring j_spec_version, */
/*    jobjectArray j_data_model_versions) */
/* { */
/*     OC_UNUSED(env); */
/*     OC_UNUSED(klass); */
/*     OC_UNUSED(j_types); */
/*     OC_UNUSED(j_data_model_versions); */
/*     static OCDeviceInfo device_info = */
/* 	{ */
/* 	    .deviceName = "", /\* Default Device Name", *\/ */
/* 	    /\* OCStringLL *types; *\/ */
/* 	    .types = NULL, */
/* 	    .specVersion = "0.0.0", /\* device specification version *\/ */
/* 	    // .dataModelVersions = "minDeviceModelVersion" */
/* 	    .dataModelVersions = NULL */
/* 	}; */

/*     if (j_device_name == NULL) { */
/* 	printf("device name is null, using default\n"); */
/* 	j_device_name = (*env)->NewStringUTF(env, device_info.deviceName); */
/* 	device_info.deviceName = (char*) (*env)->GetStringUTFChars(env, j_device_name, NULL); */
/*     } else { */
/* 	device_info.deviceName = (char*) (*env)->GetStringUTFChars(env, j_device_name, NULL); */
/* 	if (device_info.deviceName == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c device name: [%s]\n", device_info.deviceName); */

/*     if (j_spec_version == NULL) { */
/* 	printf("spec version is null, using default\n"); */
/* 	j_spec_version = (*env)->NewStringUTF(env, device_info.specVersion); */
/* 	device_info.specVersion = (char*) (*env)->GetStringUTFChars(env, j_spec_version, NULL); */
/*     } else { */
/* 	device_info.specVersion = (char*) (*env)->GetStringUTFChars(env, j_spec_version, NULL); */
/* 	if (device_info.specVersion == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */
/*     printf("c platform id: [%s]\n", device_info.specVersion); */



/*     printf("Setting device info...\n"); */
/*     OCStackResult op_result; */
/*     op_result = OCSetDeviceInfo(device_info); */
/*     switch (op_result) { */
/*     case OC_STACK_OK: */
/* 	break; */
/*     case OC_STACK_INVALID_PARAM: */
/* 	THROW_STACK_EXCEPTION(op_result, "Java_org_iochibity_OCF_setDeviceInfo"); */
/* 	/\* throw_invalid_param(env, "Java_org_iochibity_OCF_setDeviceInfo"); *\/ */
/* 	break; */
/*     default: */
/*         printf("Device Registration failed with result %d!\n", op_result); */
/* 	THROW_STACK_EXCEPTION(op_result, "UNKNOWN"); */
/*     } */

/*     (*env)->ReleaseStringUTFChars(env, j_device_name, device_info.deviceName); */
/*     (*env)->ReleaseStringUTFChars(env, j_spec_version, device_info.specVersion); */
/* } */

/*
 * Class:     openocf_engine_OCFServerSP
 * Method:    RegisterDefaultServiceRoutine
 * Signature: (Ljava/lang/Object;Ljava/lang/Object;)I
 */
JNIEXPORT void JNICALL Java_openocf_engine_OCFServerSP_registerDefaultServiceRoutine
(JNIEnv * env, jclass klass, jobject j_resource_service_provider, jobject j_callback_param)
{
    OC_UNUSED(env);
    OC_UNUSED(klass);
    OC_UNUSED(j_resource_service_provider);
    OC_UNUSED(j_callback_param);
}

/* **************************************************************** */
// below OBSOLETE?

/*
 * Class:     openocf_engine_OCFServerSP
 * Method:    registerServiceProvider
 * Signature: (Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;Lorg/iochibity/IResourceServiceProvider;B)Lorg/iochibity/ResourceLocal;
 */
/* JNIEXPORT jobject JNICALL Java_openocf_engine_OCFServerSP_registerServiceProvider__Ljava_lang_String_2_3Ljava_lang_String_2_3Ljava_lang_String_2Lorg_iochibity_IServiceProvider_2B */
/* (JNIEnv * env, jclass klass, */
/*  jstring j_uri, */
/*  jobjectArray j_tnames, */
/*  jobjectArray j_ifnames, */
/*  jobject j_ServiceProvider, */
/*    /\* jobject j_callback_param, *\/ */
/*    jbyte   j_policies) */
/* { */
/*     OC_UNUSED(klass); */
/*     printf("Java_openocf_engine_OCFServerSP_registerServiceProvider ENTRY\n"); */

/*     jobject j_resource_type_name = (*env)->GetObjectArrayElement(env, j_tnames, 0); */
/*     const char *c_resource_type_name = ""; */
/*     if (j_resource_type_name == NULL) { */
/* 	j_resource_type_name = (*env)->NewStringUTF(env, c_resource_type_name); */
/* 	c_resource_type_name = (char*) (*env)->GetStringUTFChars(env, j_resource_type_name, NULL); */
/*     } else { */
/* 	c_resource_type_name = (char*) (*env)->GetStringUTFChars(env, j_resource_type_name, NULL); */
/* 	if (c_resource_type_name == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	    return NULL; */
/* 	} */
/*     } */
/*     jobject j_resource_if_name = (*env)->GetObjectArrayElement(env, j_ifnames, 0); */
/*     const char *c_resource_if_name = ""; */
/*     if (j_resource_if_name == NULL) { */
/* 	j_resource_if_name = (*env)->NewStringUTF(env, c_resource_if_name); */
/* 	c_resource_if_name = (char*) (*env)->GetStringUTFChars(env, j_resource_if_name, NULL); */
/*     } else { */
/* 	c_resource_if_name = (char*) (*env)->GetStringUTFChars(env, j_resource_if_name, NULL); */
/* 	if (c_resource_if_name == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	    return NULL; */
/* 	} */
/*     } */

/*     const char *c_uri = ""; */
/*     if (j_uri == NULL) { */
/* 	j_uri = (*env)->NewStringUTF(env, c_uri); */
/* 	c_uri = (char*) (*env)->GetStringUTFChars(env, j_uri, NULL); */
/*     } else { */
/* 	c_uri = (char*) (*env)->GetStringUTFChars(env, j_uri, NULL); */
/* 	if (c_uri == NULL) { */
/* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
/* 	} */
/*     } */

/*     /\* prep callback *\/ */
/*     /\* (*env)->GetJavaVM(env, &g_jvm); *\/ */
/*     /\* bool returnValue = true; *\/ */

/*     jobject x_ServiceProvider = NULL; */
/*     /\* jobject x_callback_param = NULL; *\/ */
/*     if (j_ServiceProvider == NULL) { */
/* 	THROW_STACK_EXCEPTION(OC_STACK_INVALID_PARAM, "ResourceServiceProvider must not be null"); */
/* 	return NULL; */
/*     } */
/*     // convert local to global references to be passed */
/*     x_ServiceProvider = (*env)->NewGlobalRef(env, j_ServiceProvider); */
/*     jclass k_rsp = (*env)->GetObjectClass(env, x_ServiceProvider); */
/*     if (k_rsp == NULL) { */
/* 	THROW_JNI_EXCEPTION("GetObjectClass for ResourceServiceProvider impl"); */
/*     } */

/*     OCResourceHandle c_resource_handle; */
/*     OCStackResult op_result; */
/*     op_result = OCCreateResource( &c_resource_handle,    /\* **Resource *\/ */
/* 				  c_resource_type_name,  /\* const char *resourceTypeName, *\/ */
/* 				  c_resource_if_name,    /\* const char *resourceInterfaceName, *\/ */
/* 				  c_uri,                 /\* const char *uri, *\/ */
/* 				  _openocf_app_ResourceSP_react, */
/* 				  /\* c_resource_sp_react, *\/ */
/* 				  /\* x_callback_param, *\/ */
/* 				  x_ServiceProvider, */
/* 				  (uint8_t) j_policies);  /\* uint8_t resourceProperties *\/ */

/*     if (op_result != OC_STACK_OK) THROW_STACK_EXCEPTION(op_result, "OCCreateResource failed"); */

/*     /\* we already have j_ServiceProvider, the ServiceProvider object; */
/*        now we just need to link it to the resource created by */
/*        OCCreateResource and populate it *\/ */

/*     /\* jmethodID mid_resource_ctor = (*env)->GetMethodID(env, K_RESOURCE_LOCAL, "<init>", "()V"); *\/ */
/*     /\* if (mid_resource_ctor == NULL) { *\/ */
/*     /\* 	THROW_JNI_EXCEPTION("GetMethodID failed for ResourceLocal ctor\n"); *\/ */
/*     /\* 	return NULL; *\/ */
/*     /\* }    /\\* insert handle to OCResource *\\/ *\/ */
/*     /\* if (K_RESOURCE_LOCAL != NULL) { /\\* cannot be null in this case *\\/ *\/ */
/*     /\*     jfieldID fid_handle = (*env)->GetFieldID(env, K_RESOURCE_LOCAL, "handle", "J"); *\/ */
/*     /\*     if (fid_handle != NULL) { *\/ */
/* 	    (*env)->SetLongField(env, j_ServiceProvider, */
/* 				 FID_RSP_HANDLE, (intptr_t) (OCResource*)c_resource_handle); */
/* 	/\* } *\/ */
/*         /\* jfieldID urifield = (*env)->GetFieldID(env, K_RESOURCE_LOCAL, "uri", "Ljava/lang/String;"); *\/ */
/*         /\* if (urifield != NULL) { *\/ */
/*             jstring jString = (*env)->NewStringUTF(env, ((OCResource*)c_resource_handle)->uri); */
/*             if (jString != NULL) { */
/*                 (*env)->SetObjectField(env, j_ServiceProvider, */
/* 				       FID_RSP_URI_PATH, jString); */
/*             } */
/* 	/\* } *\/ */

/*         /\* jfieldID fid_policies = (*env)->GetFieldID(env, K_RESOURCE_LOCAL, "policies", "I"); *\/ */
/*         /\* if (fid_policies != NULL) { *\/ */
/* 	    (*env)->SetIntField(env, j_ServiceProvider, */
/* 				FID_RSP_POLICIES, */
/* 				(jint)((OCResource*)c_resource_handle)->resourceProperties); */
/* 	/\* } else { *\/ */
/* 	/\*     THROW_JNI_EXCEPTION("SetIntField failed for fid_policies\n"); *\/ */
/* 	/\* } *\/ */

/* 	    /\* FIXME: deal with OCResource.sequenceNumber *\/ */

/* 	/\* instance id ∪nion *\/ */
/* 	/\* printf("instance ORD:  %d\n", ((OCResource*)c_resource_handle)->ins); *\/ */
/* 	/\* printf("instance str:  '%s'\n", ((OCResource*)c_resource_handle)->uniqueStr); *\/ */
/* 	/\* printf("instance uuid ln: %d\n", ((OCResource*)c_resource_handle)->uniqueUUID.id_length); *\/ */
/* 	/\* printf("instance uuid: %s\n", ((OCResource*)c_resource_handle)->uniqueUUID.id); *\/ */

/* 	    /\* FIXME: deal with OCResource instance Id union *\/ */
/* 	if ( ((OCResource*)c_resource_handle)->ins > 0) { */
/* 	    printf("instance ORD:  %d\n", ((OCResource*)c_resource_handle)->ins); */
/* 	} else if ( ((OCResource*)c_resource_handle)->uniqueStr != NULL) { */
/* 	    printf("instance str:  '%s'\n", ((OCResource*)c_resource_handle)->uniqueStr); */
/* 	} else if ( ((OCResource*)c_resource_handle)->uniqueUUID.id_length > 0) { */
/* 	    printf("instance uuid: %s\n", ((OCResource*)c_resource_handle)->uniqueUUID.id); */
/* 	} else { */
/* 	    /\* printf("NO INSTANCE ID\n"); *\/ */
/* 	} */
/*     /\* } *\/ */

/*     /\* (*env)->ReleaseStringUTFChars(env, j_ServiceProvider_type_name, c_resource_type_name); *\/ */
/*     /\* (*env)->ReleaseStringUTFChars(env, j_ServiceProvider_if_name, c_resource_if_name); *\/ */
/*     (*env)->ReleaseStringUTFChars(env, j_uri, c_uri); */

/*     printf("openocf_engine_OCFServerSP/registerServiceProvider EXIT\n"); */
/*     return j_ServiceProvider; */
/* } */

/*
 * Class:     openocf_OpenOCFServer
 * Method:    registerResourceSP
 * Signature: (Lopenocf/app/ResourceSP;)Lopenocf/app/ResourceSP;
 */
JNIEXPORT jobject JNICALL
Java_openocf_OpenOCFServer_registerResourceSP(JNIEnv *env, jclass klass, jobject j_ResourceSP)
{
    OC_UNUSED(klass);
    /* printf("%s registerServiceProvider ENTRY\n", __FILE__); */
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    /* rt List<String> */
    /* we set up here, because me must pass one rt to createresource */
    /* after the resource is created we will add the remaining types */
    jobject j_Types = (*env)->GetObjectField(env, j_ResourceSP, FID_RSP_TYPES);
    int types_count = (*env)->CallIntMethod(env, j_Types, MID_LIST_SIZE);
    const char *c_resource_type_name = "";
    jobject j_resource_type_name = (*env)->CallObjectMethod(env, j_Types, MID_LL_GET, 0);
    if (j_resource_type_name == NULL) {
	j_resource_type_name = (*env)->NewStringUTF(env, c_resource_type_name);
	c_resource_type_name = (char*) (*env)->GetStringUTFChars(env, j_resource_type_name, NULL);
    } else {
	c_resource_type_name = (char*) (*env)->GetStringUTFChars(env, j_resource_type_name, NULL);
	if (c_resource_type_name == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	    return NULL;
	}
    }

    jobject j_Interfaces = (*env)->GetObjectField(env, j_ResourceSP, FID_RSP_INTERFACES);
    int if_count = (*env)->CallIntMethod(env, j_Interfaces, MID_LIST_SIZE);
    jobject j_resource_if_name = (*env)->CallObjectMethod(env, j_Interfaces, MID_LL_GET, 0);
    const char *c_resource_if_name = "";
    if (j_resource_if_name == NULL) {
	j_resource_if_name = (*env)->NewStringUTF(env, c_resource_if_name);
	c_resource_if_name = (char*) (*env)->GetStringUTFChars(env, j_resource_if_name, NULL);
    } else {
	c_resource_if_name = (char*) (*env)->GetStringUTFChars(env, j_resource_if_name, NULL);
	if (c_resource_if_name == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	    return NULL;
	}
    }
    jobject j_Uri = (*env)->GetObjectField(env, j_ResourceSP, FID_RSP_URI_PATH);
    const char *c_uri = "";
    if (j_Uri == NULL) {
	j_Uri = (*env)->NewStringUTF(env, c_uri);
	c_uri = (char*) (*env)->GetStringUTFChars(env, j_Uri, NULL);
    } else {
	c_uri = (char*) (*env)->GetStringUTFChars(env, j_Uri, NULL);
	if (c_uri == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }

    int c_policies = (*env)->GetIntField(env, j_ResourceSP, FID_RSP_POLICIES);

    /* prep callback */
    /* (*env)->GetJavaVM(env, &g_jvm); */
    /* bool returnValue = true; */

    jobject x_ServiceProvider = NULL;
    if (j_ResourceSP == NULL) {
	THROW_STACK_EXCEPTION(OC_STACK_INVALID_PARAM, "ResourceServiceProvider must not be null");
	return NULL;
    }
    // convert local to global references to be passed
    x_ServiceProvider = (*env)->NewGlobalRef(env, j_ResourceSP);
    jclass k_rsp = (*env)->GetObjectClass(env, x_ServiceProvider);
    if (k_rsp == NULL) {
	THROW_JNI_EXCEPTION("GetObjectClass for ResourceServiceProvider impl");
    }

    OCResourceHandle c_resource_handle;
    OCStackResult op_result;
    op_result = OCCreateResource( &c_resource_handle,    /* **Resource */
				  c_resource_type_name,  /* const char *resourceTypeName, */
				  c_resource_if_name,    /* const char *resourceInterfaceName, */
				  c_uri,                 /* const char *uri, */
				  _openocf_app_ResourceSP_react,
				  /* c_resource_sp_react, */
				  x_ServiceProvider,
				  (uint8_t) c_policies);  /* uint8_t resourceProperties */

    if (op_result != OC_STACK_OK) {
	THROW_STACK_EXCEPTION(op_result, "OCCreateResource failed");
    }

    OIC_LOG_V(INFO, TAG, "Created resource, handle %p", c_resource_handle);

    /* add rts */
    char *ctype = NULL;
    for (int i = 1; i < types_count; i++) {
	jstring j_type = (*env)->CallObjectMethod(env, j_Types, MID_LIST_GET, i);
	ctype = (char*) (*env)->GetStringUTFChars(env, j_type, NULL);
	op_result = OCBindResourceTypeToResource((OCResource*)c_resource_handle, ctype);
    }

    /* add ifs */
    char *iface = NULL;
    for (int i = 1; i < if_count; i++) {
	jstring j_iface = (*env)->CallObjectMethod(env, j_Interfaces, MID_LIST_GET, i);
	iface = (char*) (*env)->GetStringUTFChars(env, j_iface, NULL);
	op_result = OCBindResourceInterfaceToResource((OCResource*)c_resource_handle, iface);
    }

    (*env)->SetLongField(env, j_ResourceSP,
			 FID_RSP_HANDLE, (intptr_t)c_resource_handle);

        /* jfieldID urifield = (*env)->GetFieldID(env, K_RESOURCE_LOCAL, "uri", "Ljava/lang/String;"); */
        /* if (urifield != NULL) { */
            jstring jString = (*env)->NewStringUTF(env, ((OCResource*)c_resource_handle)->uri);
            if (jString != NULL) {
                (*env)->SetObjectField(env, j_ResourceSP,
				       FID_RSP_URI_PATH, jString);
            }
	/* } */

        /* jfieldID fid_policies = (*env)->GetFieldID(env, K_RESOURCE_LOCAL, "policies", "I"); */
        /* if (fid_policies != NULL) { */
	    (*env)->SetIntField(env, j_ResourceSP,
				FID_RSP_POLICIES,
				(jint)((OCResource*)c_resource_handle)->resourceProperties);
	/* } else { */
	/*     THROW_JNI_EXCEPTION("SetIntField failed for fid_policies\n"); */
	/* } */

	    /* FIXME: deal with OCResource.sequenceNumber */

	/* instance id ∪nion */
	/* printf("instance ORD:  %d\n", ((OCResource*)c_resource_handle)->ins); */
	/* printf("instance str:  '%s'\n", ((OCResource*)c_resource_handle)->uniqueStr); */
	/* printf("instance uuid ln: %d\n", ((OCResource*)c_resource_handle)->uniqueUUID.id_length); */
	/* printf("instance uuid: %s\n", ((OCResource*)c_resource_handle)->uniqueUUID.id); */

	    /* FIXME: deal with OCResource instance Id union */
	/* if ( ((OCResource*)c_resource_handle)->ins > 0) { */
	/*     printf("Resource instance nbr:  %d\n", ((OCResource*)c_resource_handle)->ins); */
	/* } else if ( ((OCResource*)c_resource_handle)->uniqueStr != NULL) { */
	/*     printf("instance str:  '%s'\n", ((OCResource*)c_resource_handle)->uniqueStr); */
	/* } else if ( ((OCResource*)c_resource_handle)->uniqueUUID.id_length > 0) { */
	/*     printf("instance uuid: %s\n", ((OCResource*)c_resource_handle)->uniqueUUID.id); */
	/* } else { */
	/*     /\* printf("NO INSTANCE ID\n"); *\/ */
	/* } */
    /* } */

    /* (*env)->ReleaseStringUTFChars(env, j_ResourceSP_type_name, c_resource_type_name); */
    /* (*env)->ReleaseStringUTFChars(env, j_ResourceSP_if_name, c_resource_if_name); */
    (*env)->ReleaseStringUTFChars(env, j_Uri, c_uri);

    /* printf("%s registerServiceProvider EXIT\n", __FILE__); */
    return j_ResourceSP;
}

/*
 * Class:     openocf_engine_OCFServerSP
 * Method:    registeredResourceSPs
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_openocf_engine_OCFServerSP_registeredResourceSPs
  (JNIEnv *, jclass);

/*
 * Class:     openocf_engine_OCFServerSP
 * Method:    getRelatedResourceSPs
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_openocf_engine_OCFServerSP_getRelatedResourceSPs
  (JNIEnv *, jclass);


/* /\* */
/*  * Class:     openocf_engine_OCFServerSP */
/*  * Method:    registerDefaultCoServiceProvider */
/*  * Signature: (Lorg/iochibity/CoServiceProvider;)V */
/*  *\/ */
/* JNIEXPORT void JNICALL Java_openocf_engine_OCFServerSP_registerDefaultCoServiceProvider */
/* (JNIEnv * env, jclass klass, jobject j_CoServiceProvider) */
/* { */
/* } */

/* /\* */
/*  * Class:     openocf_engine_OCFServerSP */
/*  * Method:    registerCoServiceProvider */
/*  * Signature: (Lorg/iochibity/CoServiceProvider;)V */
/*  *\/ */
/* JNIEXPORT void JNICALL Java_openocf_engine_OCFServerSP_registerCoServiceProvider */
/* (JNIEnv * env, jclass klass, jobject j_CoServiceProvider) */
/* { */
/* } */

/* /\* */
/*  * Class:     openocf_engine_OCFServerSP */
/*  * Method:    registeredCoServiceProviders */
/*  * Signature: ()Ljava/util/List; */
/*  *\/ */
/* JNIEXPORT jobject JNICALL Java_openocf_engine_OCFServerSP_registeredCoServiceProviders */
/* (JNIEnv * env, jclass klass) */
/* { */
/*     return NULL; */
/* } */

/* /\* */
/*  * Class:     openocf_engine_OCFServerSP */
/*  * Method:    getRelatedCoServiceProviders */
/*  * Signature: ()Ljava/util/List; */
/*  *\/ */
/* JNIEXPORT jobject JNICALL Java_openocf_engine_OCFServerSP_getRelatedCoServiceProviders */
/*  (JNIEnv * env, jclass klass) */
/* { */
/*     return NULL; */
/* } */
