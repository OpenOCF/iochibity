/**
 * @file openocf_Message.c  @was openocf_message_MessageRecord.c
 * @author Gregg Reynolds
 * @date December 2016
 *
 * @brief Routines to convert OCPayload structs to `message` objects.
 */

#include "openocf.h"

#include "_threads.h"

//#include "openocf_Message.h"

//#include "openocf_MessageRecord.h"
#include "jni_init.h"
#include "org_iochibity_Exceptions.h"
#include "jni_utils.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

/* INTERNAL */

#define FQCN_MESSAGE_RECORD "openocf/Message"
jclass    K_MESSAGE_RECORD                       = NULL;
jfieldID  FID_MESSAGE_HANDLE            = NULL;
jfieldID  FID_MSG_OBSERVATION_HANDLE      = NULL;
/* jmethodID MID_MSG_GET_OBSERVATION_TYPE = NULL; */
/* jfieldID  FID_MSG_METHOD                  = NULL; */
/* jmethodID MID_MSG_GET_METHOD              = NULL; */
jfieldID  FID_MSG_OPTIONS                 = NULL;
jmethodID MID_MSG_GET_OPTIONS             = NULL;
/* jfieldID  FID_MSG_PTR_OPTIONS          = NULL; */

jmethodID MID_MESSAGE_GET_EP          = NULL;
char     *MID_MESSAGE_GET_EP_NM       = "getEndpoint";
char     *MID_MESSAGE_GET_EP_T       = "getEndpoint";

/* jmethodID MID_MSG_GET_REMOTE_DEVADDR      = NULL; */


/* #ifdef IOCHIBITY    /\* 1.1.1: *\/ */

int init_Message(JNIEnv* env)
{
    jclass klass;
    klass = (*env)->FindClass(env, FQCN_MESSAGE_RECORD);
    JNI_ASSERT_NULL(klass, ERR_MSG(ERR_CLASS, FQCN_MESSAGE_RECORD), -1);
    K_MESSAGE_RECORD = (jclass)(*env)->NewGlobalRef(env, klass);
    (*env)->DeleteLocalRef(env, klass);

    /* if (MID_MSG_GET_OBSERVATION_TYPE == NULL) { */
    /* 	MID_MSG_GET_OBSERVATION_TYPE = (*env)->GetMethodID(env, K_MESSAGE_RECORD, "getObservationType", "()I"); */
    /* 	if (MID_MSG_GET_OBSERVATION_TYPE == NULL) { */
    /* 	    printf("ERROR:  GetMethodID failed for 'getObservationType' for Message"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    /* if (FID_MSG_OBSERVATION_HANDLE == NULL) { */
    /* 	FID_MSG_OBSERVATION_HANDLE = (*env)->GetFieldID(env, K_MESSAGE_RECORD, "_observationRecordHandle", "J"); */
    /* 	if (FID_MSG_OBSERVATION_HANDLE == NULL) { */
    /* 	    printf("ERROR: GetFieldID failed for '_observationRecordHandle' of Message\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */

    if (FID_MESSAGE_HANDLE == NULL) {
	FID_MESSAGE_HANDLE = (*env)->GetFieldID(env, K_MESSAGE_RECORD, "_handle", "J");
	JNI_ASSERT_NULL(FID_MESSAGE_HANDLE,
			ERR_MSG(ERR_FLD, "MessageRecord._handle"), -1);
	    }

    /* if (MID_MESSAGE_GET_EP == NULL) { */
    /* 	MID_MESSAGE_GET_EP = (*env)->GetMethodID(env, K_MESSAGE_RECORD, */
    /* 						  MID_MESSAGE_GET_EP_NM, */
    /* 						  FQCS_ENDPOINT); */
    /* 						    /\* "Lopenocf/EndPoint;"); *\/ */
    /* 	JNI_ASSERT_NULL(MID_MESSAGE_GET_EP, */
    /* 			ERR_MSG(ERR_FLD, "MessageRecord.getEndpoint"), -1); */
    /* } */

    /* if (MID_MSG_GET_REMOTE_DEVADDR == NULL) { */
    /* 	MID_MSG_GET_REMOTE_DEVADDR = (*env)->GetMethodID(env, K_MESSAGE_RECORD, */
    /* 							 "getRemoteDeviceAddress", */
    /* 							 "()" FQCS_ENDPOINT); */
    /* 							 /\* "()Lopenocf/EndPoint;"); *\/ */
    /* 	if (MID_MSG_GET_REMOTE_DEVADDR == NULL) { */
    /* 	    printf("ERROR:  GetMethodID failed for getRemoteDeviceAddress for Message\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    /* if (FID_MSG_METHOD == NULL) { */
    /* 	FID_MSG_METHOD = (*env)->GetFieldID(env, K_MESSAGE_RECORD, "_method", "I"); */
    /* 	if (FID_MSG_METHOD == NULL) { */
    /* 	    printf("ERROR:  GetFieldID failed for '_method' of Message\n"); */
    /* 	return OC_EH_INTERNAL_SERVER_ERROR; */
    /* 	} */
    /* } */
    /* if (MID_MSG_GET_METHOD == NULL) { */
    /* 	MID_MSG_GET_METHOD = (*env)->GetMethodID(env, K_kMESSAGE_RECORD, "getMethod", "()I"); */
    /* 	if (MID_MSG_GET_METHOD == NULL) { */
    /* 	    printf("ERROR:  GetMethodID failed for getMethod for Message\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */

    return 0;
}

/* WARNING: OCPlatformPayload removed in 1.3
 * OCPlatformPayload_to_Observation
 * compensate for OCPlatformInfo wierdness
 */
/* jobject OCPlatformPayload_to_Observation(JNIEnv* env, OCPlatformPayload* c_payload) */
/* { */
/*     /\* printf("OCPlatformPayload_to_Observation ENTRY\n"); *\/ */
/*     jobject j_Observation = (*env)->NewObject(env, K_OBSERVATION, MID_OBSERVATION_CTOR); */
/*     if (j_Observation == NULL) { */
/* 	THROW_JNI_EXCEPTION("NewObject failed for ObservationForPlatform\n"); */
/* 	return NULL; */
/*     } */

/*     /\* _uri *\/ */
/*     jstring j_uri = (*env)->NewStringUTF(env, c_payload->uri); */
/*     (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_URI_PATH, j_uri); */

/*     /\* _rtypes *\/ */
/*     jobject j_rtypes  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR); */
/*     if (j_rtypes == NULL) { */
/* 	printf("NewObject failed for rtypes LinkedList\n"); */
/* 	return NULL; */
/*     } */
/*     OCStringLL* c_rtypes = c_payload->rt; */
/*     jstring j_rtype; */
/*     while(c_rtypes) { */
/* 	j_rtype = (*env)->NewStringUTF(env, c_rtypes->value); */
/* 	jboolean jb = (*env)->CallBooleanMethod(env, j_rtypes, MID_LL_ADD, j_rtype); */
/* 	if (!jb) { */
/* 	    printf("CallBooleanMethod failed on MID_LL_ADD for rtypes of Platform\n"); */
/* 	} */
/* 	c_rtypes = c_rtypes->next; */
/*     } */
/*     (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_RTYPES, j_rtypes); */

/*     /\* _interfaces *\/ */
/*     jobject j_ifaces  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR); */
/*     if (j_ifaces == NULL) { */
/* 	printf("NewObject failed for ifaces LinkedList\n"); */
/* 	return NULL; */
/*     } */
/*     OCStringLL* c_ifaces = c_payload->interfaces; */
/*     jstring j_iface; */
/*     while(c_ifaces) { */
/* 	/\* printf("c resource type: %s\n", resource_types->resourcetypename); *\/ */
/* 	j_iface = (*env)->NewStringUTF(env, c_ifaces->value); */
/* 	jboolean jb = (*env)->CallBooleanMethod(env, j_ifaces, MID_LL_ADD, j_iface); */
/* 	if (!jb) { */
/* 	    printf("CallBooleanMethod failed on MID_LL_ADD for ifaces of Platform\n"); */
/* 	} */
/* 	c_ifaces = c_ifaces->next; */
/*     } */
/*     (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_IFS, j_ifaces); */

/*     /\* OCPlatformInfo - convert to PropertyMap *\/ */
/*     jobject j_pmap  = (*env)->NewObject(env, K_PMAP, MID_PMAP_CTOR); */
/*     if (j_pmap == NULL) { */
/* 	THROW_JNI_EXCEPTION("NewObject failed for PropertyMap\n"); */
/* 	return NULL; */
/*     } */
/*     jstring j_n = (*env)->NewStringUTF(env, OC_RSRVD_PLATFORM_ID); */
/*     jstring j_s = (*env)->NewStringUTF(env, c_payload->info.platformID); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_MFG_NAME); */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->info.manufacturerName); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_MFG_URL); */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->info.manufacturerUrl); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_MODEL_NUM); */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->info.modelNumber); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_MFG_DATE); */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->info.dateOfManufacture); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_PLATFORM_VERSION); */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->info.platformVersion); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_OS_VERSION); */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->info.operatingSystemVersion); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_HARDWARE_VERSION); */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->info.hardwareVersion); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_FIRMWARE_VERSION); */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->info.firmwareVersion); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_SUPPORT_URL); */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->info.supportUrl); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_SYSTEM_TIME); */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->info.systemTime); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_PROPERTIES, j_pmap); */

/*     /\* printf("OCPlatformPayload_to_Observation EXIT\n"); *\/ */
/*     return j_Observation; */
/* } */

/* WARNING: OCDevicePayload removed in 1.3
 * OCDevicePayload_to_Observation
 * compensate for OCDeviceInfo wierdness
 */
/* jobject OCDevicePayload_to_Observation(JNIEnv* env, OCDevicePayload* c_payload) */
/* { */
/*     /\* printf("OCDevicePayload_to_Observation ENTRY\n"); *\/ */
/*     jobject j_Observation = (*env)->NewObject(env, K_OBSERVATION, MID_OBSERVATION_CTOR); */
/*     if (j_Observation == NULL) { */
/* 	THROW_JNI_EXCEPTION("NewObject failed for ObservationForDevice\n"); */
/* 	return NULL; */
/*     } */

/*     /\* _rtypes *\/ */
/*     jobject j_rtypes  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR); */
/*     if (j_rtypes == NULL) { */
/* 	printf("NewObject failed for rtypes LinkedList\n"); */
/* 	return NULL; */
/*     } */
/*     OCStringLL* c_rtypes = c_payload->types; */
/*     jstring j_rtype; */
/*     while(c_rtypes) { */
/* 	j_rtype = (*env)->NewStringUTF(env, c_rtypes->value); */
/* 	jboolean jb = (*env)->CallBooleanMethod(env, j_rtypes, MID_LL_ADD, j_rtype); */
/* 	if (!jb) { */
/* 	    printf("CallBooleanMethod failed on MID_LL_ADD for rtypes for Devices\n"); */
/* 	} */
/* 	c_rtypes = c_rtypes->next; */
/*     } */
/*     (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_RTYPES, j_rtypes); */

/*     /\* _interfaces *\/ */
/*     jobject j_ifaces  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR); */
/*     if (j_ifaces == NULL) { */
/* 	printf("NewObject failed for ifaces LinkedList\n"); */
/* 	return NULL; */
/*     } */
/*     OCStringLL* c_ifaces = c_payload->interfaces; */
/*     jstring j_iface; */
/*     while(c_ifaces) { */
/* 	/\* printf("c resource type: %s\n", resource_types->resourcetypename); *\/ */
/* 	j_iface = (*env)->NewStringUTF(env, c_ifaces->value); */
/* 	jboolean jb = (*env)->CallBooleanMethod(env, j_ifaces, MID_LL_ADD, j_iface); */
/* 	if (!jb) { */
/* 	    printf("CallBooleanMethod failed on MID_LL_ADD for ifaces of Devices\n"); */
/* 	} */
/* 	c_ifaces = c_ifaces->next; */
/*     } */
/*     (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_IFS, j_ifaces); */

/*     /\* OCDeviceInfo - convert to PropertyMap *\/ */
/*     jobject j_pmap  = (*env)->NewObject(env, K_PMAP, MID_PMAP_CTOR); */
/*     if (j_pmap == NULL) { */
/* 	THROW_JNI_EXCEPTION("NewObject failed for PropertyMap\n"); */
/* 	return NULL; */
/*     } */
/*     jstring j_n = (*env)->NewStringUTF(env, OC_RSRVD_DEVICE_ID);         /\* di *\/ */
/*     jstring j_s = (*env)->NewStringUTF(env, c_payload->sid); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_DEVICE_NAME);               /\* n *\/ */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->deviceName); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_SPEC_VERSION);              /\* icv *\/ */
/*     j_s = (*env)->NewStringUTF(env,  c_payload->specVersion); */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s); */

/*     /\* data model version(s) - csv list *\/ */
/*     j_n = (*env)->NewStringUTF(env, OC_RSRVD_DATA_MODEL_VERSION);       /\* dmv *\/ */
/*     jobject j_dmvs  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR); */
/*     if (j_dmvs == NULL) { */
/* 	printf("NewObject failed for dmvs LinkedList\n"); */
/* 	return NULL; */
/*     } */
/*     OCStringLL* c_dmvs = c_payload->dataModelVersions; */
/*     jstring j_dmv; */
/*     while(c_dmvs) { */
/* 	j_dmv = (*env)->NewStringUTF(env, c_dmvs->value); */
/* 	jboolean jb = (*env)->CallBooleanMethod(env, j_dmvs, MID_LL_ADD, j_dmv); */
/* 	if (!jb) { */
/* 	    printf("CallBooleanMethod failed on MID_LL_ADD for dmvs of Devices\n"); */
/* 	} */
/* 	c_dmvs = c_dmvs->next; */
/*     } */
/*     (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_dmvs); */

/*     (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_PROPERTIES, j_pmap); */

/*     /\* printf("OCDevicePayload_to_Observation EXIT\n"); *\/ */
/*     return j_Observation; */
/* } */

/* #endif	/\* IOCHIBITY defined *\/ */

/*
 * OCResourcePayload_to_Observation
 */
jobject OCResourcePayload_to_Observation(JNIEnv* env, OCResourcePayload* c_payload)
{
    /* printf("OCResourcePayload_to_Observation ENTRY\n"); */
    jobject j_Observation = (*env)->NewObject(env, K_OBSERVATION, MID_OBSERVATION_CTOR);
    if (j_Observation == NULL) {
	THROW_JNI_EXCEPTION("NewObject failed for resource Observation\n");
	return NULL;
    }

    /* _rtypes */
    jobject j_rtypes  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR);
    if (j_rtypes == NULL) {
	printf("NewObject failed for rtypes LinkedList\n");
	return NULL;
    }
    OCStringLL* c_rtypes = c_payload->types;
    jstring j_rtype;
    while(c_rtypes) {
	j_rtype = (*env)->NewStringUTF(env, c_rtypes->value);
	jboolean jb = (*env)->CallBooleanMethod(env, j_rtypes, MID_LL_ADD, j_rtype);
	if (!jb) {
	    printf("CallBooleanMethod failed on MID_LL_ADD for rtypes for Devices\n");
	}
	c_rtypes = c_rtypes->next;
    }
    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_RTYPES, j_rtypes);

    /* _interfaces */
    jobject j_ifaces  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR);
    if (j_ifaces == NULL) {
	printf("NewObject failed for ifaces LinkedList\n");
	return NULL;
    }
    OCStringLL* c_ifaces = c_payload->interfaces;
    jstring j_iface;
    while(c_ifaces) {
	/* printf("c resource type: %s\n", resource_types->resourcetypename); */
	j_iface = (*env)->NewStringUTF(env, c_ifaces->value);
	jboolean jb = (*env)->CallBooleanMethod(env, j_ifaces, MID_LL_ADD, j_iface);
	if (!jb) {
	    printf("CallBooleanMethod failed on MID_LL_ADD for ifaces of Devices\n");
	}
	c_ifaces = c_ifaces->next;
    }
    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_IFS, j_ifaces);

    /* _uri */
    jstring j_uri = (*env)->NewStringUTF(env, c_payload->uri);
    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_URI_PATH, j_uri);

    /* properties */
    jobject j_pmap  = (*env)->NewObject(env, K_PMAP, MID_PMAP_CTOR);
    if (j_pmap == NULL) {
	THROW_JNI_EXCEPTION("NewObject failed for PropertyMap\n");
	return NULL;
    }
    jstring j_n = (*env)->NewStringUTF(env, "bitmap");
    jobject j_obj =  (*env)->NewObject(env, K_BYTE, MID_BYTE_CTOR, c_payload->bitmap);
    (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_obj);

    j_n = (*env)->NewStringUTF(env, "secure");
    j_obj =  (*env)->NewObject(env, K_BOOLEAN, MID_BOOL_CTOR, c_payload->secure);
    (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_obj);

    j_n = (*env)->NewStringUTF(env, "port");
    j_obj =  (*env)->NewObject(env, K_INTEGER, MID_INT_CTOR, (int)c_payload->port);
    (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_obj);

    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_PROPERTIES, j_pmap);

    return j_Observation;
}

/*
 * OCDiscoveryPayload_to_Observation
 */
jobject OCDiscoveryPayload_to_Observation(JNIEnv* env, OCDiscoveryPayload* c_payload)
{
    /* printf("OCDiscoveryPayload_to_Observation ENTRY\n"); */
    jobject j_Observation = (*env)->NewObject(env, K_OBSERVATION, MID_OBSERVATION_CTOR);
    if (j_Observation == NULL) {
	THROW_JNI_EXCEPTION("NewObject failed for ObservationForDevice\n");
	return NULL;
    }

    /* _rtypes */
    jobject j_rtypes  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR);
    if (j_rtypes == NULL) {
	printf("NewObject failed for rtypes LinkedList\n");
	return NULL;
    }
    OCStringLL* c_rtypes = c_payload->type;
    jstring j_rtype;
    while(c_rtypes) {
	j_rtype = (*env)->NewStringUTF(env, c_rtypes->value);
	jboolean jb = (*env)->CallBooleanMethod(env, j_rtypes, MID_LL_ADD, j_rtype);
	if (!jb) {
	    printf("CallBooleanMethod failed on MID_LL_ADD for rtypes for Devices\n");
	}
	c_rtypes = c_rtypes->next;
    }
    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_RTYPES, j_rtypes);

    /* _interfaces */
    jobject j_ifaces  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR);
    if (j_ifaces == NULL) {
	printf("NewObject failed for ifaces LinkedList\n");
	return NULL;
    }
    OCStringLL* c_ifaces = c_payload->iface;
    jstring j_iface;
    while(c_ifaces) {
	/* printf("c resource type: %s\n", resource_types->resourcetypename); */
	j_iface = (*env)->NewStringUTF(env, c_ifaces->value);
	jboolean jb = (*env)->CallBooleanMethod(env, j_ifaces, MID_LL_ADD, j_iface);
	if (!jb) {
	    printf("CallBooleanMethod failed on MID_LL_ADD for ifaces of Devices\n");
	}
	c_ifaces = c_ifaces->next;
    }
    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_IFS, j_ifaces);

    /* _uri */
    /*GAR: removed in 1.3
    jstring j_uri = (*env)->NewStringUTF(env, c_payload->uri);
    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_URI_PATH, j_uri);
    */

    /* convert remaining flds to PropertyMap */
    jobject j_pmap  = (*env)->NewObject(env, K_PMAP, MID_PMAP_CTOR);
    if (j_pmap == NULL) {
	THROW_JNI_EXCEPTION("NewObject failed for Discovery PropertyMap\n");
	return NULL;
    }

    /*GAR: removed in 1.3
    jstring j_n = (*env)->NewStringUTF(env, "baseURI");
    jstring j_s = (*env)->NewStringUTF(env, c_payload->baseURI);
    (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s);
    */

    jstring j_n;
    jstring j_s;
    j_n = (*env)->NewStringUTF(env, OC_RSRVD_DEVICE_ID);         /* di */
    j_s = (*env)->NewStringUTF(env, c_payload->sid);
    (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s);

    j_n = (*env)->NewStringUTF(env, OC_RSRVD_DEVICE_NAME);               /* n */
    j_s = (*env)->NewStringUTF(env,  c_payload->name);
    (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_n, j_s);

    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_PROPERTIES, j_pmap);

    /* now the resource list */
    jobject j_resource_list  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR);
    if (j_resource_list == NULL) {
	printf("NewObject failed for resource_list LinkedList\n");
	return NULL;
    }
    OCResourcePayload* c_resource_list = c_payload->resources;
    jobject j_resource;
    while(c_resource_list) {
	j_resource = OCResourcePayload_to_Observation(env, c_resource_list);
	jboolean jb = (*env)->CallBooleanMethod(env, j_resource_list, MID_LL_ADD, j_resource);
	if (!jb) {
	    printf("CallBooleanMethod failed on MID_LL_ADD for resource_list of Devices\n");
	}
	c_resource_list = c_resource_list->next;
    }

    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_CHILDREN, j_resource_list);

    /* printf("OCDiscoveryPayload_to_Observation EXIT\n"); */
    return j_Observation;
}

/*
 * OCRepPayload_to_Observation
 */
jobject OCRepPayload_to_Observation(JNIEnv* env, OCRepPayload* c_payload)
{
    printf("OCRepPayload_to_Observation ENTRY\n");
    jobject j_Observation = (*env)->NewObject(env, K_OBSERVATION, MID_OBSERVATION_CTOR);
    if (j_Observation == NULL) {
	THROW_JNI_EXCEPTION("NewObject failed for rep Observation\n");
	return NULL;
    }

    /* _rtypes */
    jobject j_rtypes  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR);
    if (j_rtypes == NULL) {
	printf("NewObject failed for rtypes LinkedList\n");
	return NULL;
    }
    OCStringLL* c_rtypes = c_payload->types;
    jstring j_rtype;
    while(c_rtypes) {
	j_rtype = (*env)->NewStringUTF(env, c_rtypes->value);
	jboolean jb = (*env)->CallBooleanMethod(env, j_rtypes, MID_LL_ADD, j_rtype);
	if (!jb) {
	    printf("CallBooleanMethod failed on MID_LL_ADD for rtypes for Devices\n");
	}
	c_rtypes = c_rtypes->next;
    }
    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_RTYPES, j_rtypes);
    /* _interfaces */
    jobject j_ifaces  = (*env)->NewObject(env, K_LINKED_LIST, MID_LL_CTOR);
    if (j_ifaces == NULL) {
	printf("NewObject failed for ifaces LinkedList\n");
	return NULL;
    }
    OCStringLL* c_ifaces = c_payload->interfaces;
    jstring j_iface;
    while(c_ifaces) {
	/* printf("c resource type: %s\n", resource_types->resourcetypename); */
	j_iface = (*env)->NewStringUTF(env, c_ifaces->value);
	jboolean jb = (*env)->CallBooleanMethod(env, j_ifaces, MID_LL_ADD, j_iface);
	if (!jb) {
	    printf("CallBooleanMethod failed on MID_LL_ADD for ifaces of Devices\n");
	}
	c_ifaces = c_ifaces->next;
    }
    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_IFS, j_ifaces);

    /* _uri */
    jstring j_uri = (*env)->NewStringUTF(env, c_payload->uri);
    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_URI_PATH, j_uri);

    /* properties */
    jobject j_pmap  = (*env)->NewObject(env, K_PMAP, MID_PMAP_CTOR);
    if (j_pmap == NULL) {
	THROW_JNI_EXCEPTION("NewObject failed for Discovery PropertyMap\n");
	return NULL;
    }

    OCRepPayloadValue* vmap = c_payload->values;
    jstring j_key;
    jobject j_val;
    while (vmap) {
	j_key = (*env)->NewStringUTF(env, vmap->name);
	switch(vmap->type) {
	case OCREP_PROP_NULL:
	    break;
	case OCREP_PROP_INT:
	    j_val = (*env)->NewObject(env, K_INTEGER, MID_INT_CTOR, vmap->i);
	    (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_key, j_val);
	    break;
	case OCREP_PROP_DOUBLE:
	    j_val = (*env)->NewObject(env, K_DOUBLE, MID_DBL_CTOR, vmap->d);
	    (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_key, j_val);
	    break;
	case OCREP_PROP_BOOL:
	    j_val = (*env)->NewObject(env, K_BOOLEAN, MID_BOOL_CTOR, vmap->b);
	    (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_key, j_val);
	    break;
	case OCREP_PROP_STRING:
	    j_val = (*env)->NewStringUTF(env, vmap->str);
	    /* j_val = (*env)->NewObject(env, K_STRING, MID_STR_CTOR, j_s); */
	    (*env)->CallObjectMethod(env, j_pmap, MID_PMAP_PUT, j_key, j_val);
	    break;
	case OCREP_PROP_BYTE_STRING:
	    break;
	case OCREP_PROP_OBJECT:
	    break;
	case OCREP_PROP_ARRAY:
	    break;
	default:
	    break;
	}
	vmap = vmap->next;
    }
    (*env)->SetObjectField(env, j_Observation, FID_OBSERVATION_PROPERTIES, j_pmap);

    printf("OCRepPayload_to_Observation EXIT\n");
    return j_Observation;
}

/* EXTERNAL */

/*
 * Class:     org_iochibity_Message
 * Method:    getObservationRecords
 * Signature: ()Lorg/iochibity/ObservationList;
 */
JNIEXPORT jobject JNICALL Java_org_iochibity_Message_getObservationRecords
(JNIEnv * env, jobject this)
{
    OC_UNUSED(env);
    OC_UNUSED(this);
    /* printf("Java_org_iochibity_Message_getObservationRecords ENTRY\n"); */

    OCPayload* c_payload = (OCPayload*)(intptr_t)
	(*env)->GetLongField(env, this, FID_MSG_OBSERVATION_HANDLE);
    if (c_payload == NULL) {
	// ok - maybe no payload
	/* THROW_JNI_EXCEPTION("GetLongField failed for FID_MSG_PAYLOAD_HANDLE on Message"); */
	return NULL;
    }

    /* create a new PayloadList */
    jobject j_OBSLL  = (*env)->NewObject(env, K_OBSERVATION_LIST, MID_PLL_CTOR);
    if (j_OBSLL == NULL) {
	THROW_JNI_EXCEPTION("NewObject failed for ObservationList\n");
	return NULL;
    }

    /* create new Observation object and adding to list */
    jobject j_observation = NULL;
    jboolean j_b;
    while (c_payload != NULL) {
	switch (c_payload->type) {
	case PAYLOAD_TYPE_DISCOVERY:
	    j_observation = OCDiscoveryPayload_to_Observation(env, (OCDiscoveryPayload*) c_payload);
	    /* add observation to observation list */
	    j_b = (*env)->CallBooleanMethod(env, j_OBSLL, MID_PLL_ADD, j_observation);
	    if (!j_b) {
		THROW_JNI_EXCEPTION("CallBooleanMethod failed for mid_add\n");
	    }
	    c_payload = (OCPayload*) ((OCDiscoveryPayload*)c_payload)->next;
	    break;

	    // Iotivity 1.3+: OCDevicePayload has been removed.
/* 	case PAYLOAD_TYPE_DEVICE: */
/* #ifdef IOCHIBITY    /\* 1.1.1: *\/ */
/* 	    j_observation = OCDevicePayload_to_Observation(env, (OCDevicePayload*) c_payload); */
/* #else */
/* 	    printf("XXXXXXXXXXXXXXXX: received PAYLOAD_TYPE_DEVICE\n"); */
/* #endif */
/* 	    /\* add observation to observation list *\/ */
/* 	    j_b = (*env)->CallBooleanMethod(env, j_OBSLL, MID_PLL_ADD, j_observation); */
/* 	    if (!j_b) { */
/* 		THROW_JNI_EXCEPTION("CallBooleanMethod failed for mid_add\n"); */
/* 	    } */
/* 	    c_payload = NULL; */
/* 	    break; */

	    // Iotivity 1.3+: OCPlatformPayload has been removed.
/* 	case PAYLOAD_TYPE_PLATFORM: */
/* #ifdef IOCHIBITY    /\* 1.1.1: *\/ */
/* 	    j_observation = OCPlatformPayload_to_Observation(env, (OCPlatformPayload*) c_payload); */
/* #else */
/* 	    printf("XXXXXXXXXXXXXXXX: received PAYLOAD_TYPE_PLATFORM\n"); */
/* #endif */
/* 	    /\* add observation to observation list *\/ */
/* 	    j_b = (*env)->CallBooleanMethod(env, j_OBSLL, MID_PLL_ADD, j_observation); */
/* 	    if (!j_b) { */
/* 		THROW_JNI_EXCEPTION("CallBooleanMethod failed for mid_add\n"); */
/* 	    } */
/* 	    c_payload = NULL; */
/* 	    break; */
	case PAYLOAD_TYPE_REPRESENTATION:
	    j_observation = OCRepPayload_to_Observation(env, (OCRepPayload*) c_payload);
	    /* add observation to observation list */
	    j_b = (*env)->CallBooleanMethod(env, j_OBSLL, MID_PLL_ADD, j_observation);
	    if (!j_b) {
		THROW_JNI_EXCEPTION("CallBooleanMethod failed for mid_add\n");
	    }
	    c_payload = (OCPayload*) ((OCRepPayload*)c_payload)->next;
	    break;
	case PAYLOAD_TYPE_INVALID:
	case PAYLOAD_TYPE_SECURITY:
	/* case PAYLOAD_TYPE_PRESENCE: */
	/* case PAYLOAD_TYPE_RD: */
	default:
	    c_payload = NULL;
	    break;
	}
    }
    /* printf("Java_org_iochibity_Message_getObservationRecords EXIT\n"); */
    return j_OBSLL;
}

/* /\* */
/*  * Class:     org_iochibity_Message */
/*  * Method:    getObservationType */
/*  * Signature: ()I */
/*  *\/ */
/* JNIEXPORT jint JNICALL Java_org_iochibity_Message_getObservationType */
/* (JNIEnv * env, jobject this) */
/* { */
/*     OC_UNUSED(env); */
/*     OC_UNUSED(this); */
/*     /\* printf("Java_org_iochibity_Message_getObservationType ENTRY\n"); *\/ */
/*     /\* 1. get payloadHandle from this *\/ */
/*     OCPayload* j_payload = (OCPayload*)(intptr_t) */
/* 	(*env)->GetLongField(env, this, FID_OBIN_PAYLOAD_HANDLE); */
/*     if (j_payload == NULL) { */
/* 	// ok, maybe no payload? */
/* 	/\* THROW_JNI_EXCEPTION("GetLongField failed for FID_MSG_PAYLOAD_HANDLE on Message"); *\/ */
/* 	return -1; */
/*     } else */
/* 	return j_payload->type; */
/* } */

/*
 * Class:     org_iochibity_Message
 * Method:    getOptions
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_org_iochibity_Message_getOptions
(JNIEnv * env, jobject this)
{
    OC_UNUSED(env);
    OC_UNUSED(this);
    printf("%s: Java_org_iochibity_Message_getOptions\n", __FILE__);

    /* FIXME: implement */

    /* if (FID_DRSPI_PTR_OPTIONS == NULL) { */
    /* 	FID_DRSPI_PTR_OPTIONS = (*env)->GetFieldID(env, K_OBSERVATION_IN, "ptr_Options", "J"); */
    /* 	if (FID_DRSPI_PTR_OPTIONS == NULL) { */
    /* 	    THROW_JNI_EXCEPTION("GetFieldID failed for 'ptr_Options' of ObservationIn\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    /* if (FID_DRSPI_OPTION_COUNT == NULL) { */
    /* 	FID_DRSPI_OPTION_COUNT = (*env)->GetFieldID(env, K_OBSERVATION_IN, "optionCount", "I"); */
    /* 	if (FID_DRSPI_OPTION_COUNT == NULL) { */
    /* 	    THROW_JNI_EXCEPTION("GetFieldID failed for 'optionCount' of ObservationIn\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    /* if (MID_DRSPI_GET_OPTIONS == NULL) { */
    /* 	MID_DRSPI_GET_OPTIONS = (*env)->GetMethodID(env, K_OBSERVATION_IN, */
    /* 						    "getOptions", "()Ljava/util/List;"); */
    /* 	if (MID_DRSPI_GET_OPTIONS == NULL) { */
    /* 	    THROW_JNI_EXCEPTION("GetFieldID failed for 'getOptions' of ObservationIn\n"); */
    /* 	    return -1; */
    /* 	} */
    /* } */
    return NULL;
}
