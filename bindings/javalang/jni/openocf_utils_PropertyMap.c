/**
 * @file openocf_PropertyMap.c
 * @author Gregg Reynolds
 * @date December 2016
 *
 * @brief Implementation of property management functions
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "openocf_utils_PropertyMap.h"
#include "jni_init.h"
#include "jni_utils.h"
#include "org_iochibity_Exceptions.h"

#include "openocf.h"
/* #include "ocpayload.h" */
/* #include "ocresource.h" */
/* #include "ocresourcehandler.h" */
/* #include "ocstack.h" */

jclass    K_PMAP                          = NULL; /* class for PropertyMap */
jmethodID MID_PMAP_CTOR                   = NULL;
jmethodID MID_PMAP_SIZE                   = NULL;
jmethodID MID_PMAP_ENTRYSET               = NULL;
jmethodID MID_PMAP_GET                    = NULL;
jmethodID MID_PMAP_PUT                    = NULL;

int init_pmap(JNIEnv* env)
{
    /* printf("init_pmap ENTRY\n"); */
    /* iteration over property map in java: */
    /* Iterator it = mp.entrySet().iterator(); */
    /* while (it.hasNext()) { */
    /*     Map.Entry pair = (Map.Entry)it.next(); */
    /*     System.out.println(pair.getKey() + " = " + pair.getValue()); */
    /*     it.remove(); // avoids a ConcurrentModificationException */
    /* } */

    jclass klass;
    /* PropertyMap */
    if (K_PMAP == NULL) {
	klass = (*env)->FindClass(env, FQCN_PMAP);
	JNI_ASSERT_NULL(klass, "FindClass failed for " FQCN_PMAP "\n", 0);
	K_PMAP = (jclass)(*env)->NewGlobalRef(env, klass);
	(*env)->DeleteLocalRef(env, klass);
    }
    if (MID_PMAP_CTOR == NULL) {
	MID_PMAP_CTOR = (*env)->GetMethodID(env, K_PMAP, "<init>", J_NULLARY J_VOID);
	if (MID_PMAP_CTOR == 0) {
	    printf("ERROR: GetMethodID failed for PropertyMap ctor.\n");
	    return -1;
	}
    }
    MID_PMAP_PUT = (*env)->GetMethodID(env, K_PMAP,
				     "put",
				     "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    if (MID_PMAP_PUT == NULL) {
	printf("ERROR: GetMethodID failed for put method of pm.\n");
	return -1;
    }
    if (MID_PMAP_SIZE == NULL) {
	MID_PMAP_SIZE = (*env)->GetMethodID(env, K_PMAP, "size", "()I");
	if (MID_PMAP_SIZE == NULL) {
	    printf("ERROR: GetMethodID failed for 'size' of PropertyMap\n");
	    return -1;
	}
    }
    if (MID_PMAP_ENTRYSET == NULL) {
	MID_PMAP_ENTRYSET = (*env)->GetMethodID(env, K_PMAP, "entrySet", "()Ljava/util/Set;");
	if (MID_PMAP_ENTRYSET == NULL) {
	    printf("ERROR: GetMethodID failed for 'entrySet' of PropertyMap\n");
	    return -1;
	}
    }
    if (MID_PMAP_GET == NULL) {
	MID_PMAP_GET = (*env)->GetMethodID(env, K_PMAP, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
	if (MID_PMAP_GET == NULL) {
	    printf("ERROR: GetMethodID failed for 'get' of PropertyMap\n");
	    return -1;
	}
    }
     /* Map.Entry */
    if (K_MAPENTRY == NULL) {
	klass = (*env)->FindClass(env, "java/util/Map$Entry");
	JNI_ASSERT_NULL(klass, "FindClass failed for java/util/Map$Entry\n", 0);
	K_MAPENTRY = (jclass)(*env)->NewGlobalRef(env, klass);
	(*env)->DeleteLocalRef(env, klass);
    }
    if (MID_ENTRY_GETKEY == NULL) {
	MID_ENTRY_GETKEY = (*env)->GetMethodID(env, K_MAPENTRY, "getKey", "()Ljava/lang/Object;");
	if (MID_ENTRY_GETKEY == NULL) {
	    printf("ERROR: GetMethodID failed for 'getKey' of Map$Entry\n");
	    return -1;
	}
    }
    if (MID_ENTRY_GETVALUE == NULL) {
	MID_ENTRY_GETVALUE = (*env)->GetMethodID(env, K_MAPENTRY, "getValue", "()Ljava/lang/Object;");
	if (MID_ENTRY_GETVALUE == NULL) {
	    printf("ERROR: GetMethodID failed for 'getValue' of Map$Entry\n");
	    return -1;
	}
    }
    /* printf("init_pmap EXIT\n"); */
    return 0;
}

bool OCRepPayloadSetProp(OCRepPayload* payload, const char* name,
				void* value, OCRepPayloadPropType type);

/* INTERNAL */

jlong getHandle(JNIEnv* env, jobject this)
{
    /* first get handle to OC struct */
    jclass j_clazz = NULL;
    jlong j_handle = 0;
    j_clazz = (*env)->GetObjectClass(env, this);
    if (j_clazz != NULL) {
	/* 1. get handle to OCResource */
        jfieldID fid_handle = (*env)->GetFieldID(env, j_clazz, "handle", "J");
        if (fid_handle == NULL) {
	    THROW_JNI_EXCEPTION("GetFieldID failed for 'handle'\n");
	    return 0;
	} else {
    	    j_handle = (*env)->GetLongField(env, this, fid_handle);
    	    /* printf("HANDLE: %ld\n", j_handle); */
	    /* c_handle = (OCResource*) j_handle; */
	    /* printf("c resource uri: %s\n", c_handle->uri); */
	}
	/* stack/ocresource.h:
	   typedef struct resourcetype_t {
	     struct resourcetype_t *next;
	     char *resourcetypename;
	   } OCResourceType;
	*/
    } else {
	THROW_JNI_EXCEPTION("GetObjectClass failed for PayloadProperties\n");
    }
    return j_handle;
}

/*
 * Class:     org_iochibity_PayloadProperties
 * Method:    getProp
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_iochibity_PayloadProperties_getProp
(JNIEnv * env, jobject this, jstring name)
{
    printf("Java_org_iochibity_PayloadProperties_getProp ENTRY\n");
    jclass klass = NULL;
    klass = (*env)->GetObjectClass(env, this);
    if (klass == NULL) {
	THROW_JNI_EXCEPTION("GetObjectClass failed for PayloadProperties object\n");
	return 0;
    }
    /* OCRepPayloadValue* payload_property = NULL; */
    jfieldID fid_handle = (*env)->GetFieldID(env, klass, "handleOCRepPayload", "J");
    if (fid_handle == NULL) {
	THROW_JNI_EXCEPTION("GetFieldID failed for 'handle' on PayloadProperties\n");
	return 0;
    }
    OCRepPayload* payload_rep = NULL;
    payload_rep = (OCRepPayload*)(intptr_t) (*env)->GetLongField(env, this, fid_handle);
    OCRepPayloadValue* vs = payload_rep->values;
    while(vs)
    {
        if (0 == strcmp(vs->name, (char*)name))
        {
	    break;
        }
        vs = vs->next;
    }

    jobject retval = NULL;
    if (vs) {
	switch(vs->type) {
	case OCREP_PROP_INT:
	    /* retval = int2integer(env, payload_property->i); */
	    /* printf("int2integer returned %d\n", retval); */
	    printf("FOUND PROPERTY: %s = %lld\n", (char*) name, vs->i);
	    break;
	case OCREP_PROP_DOUBLE:
	    break;
	case OCREP_PROP_BOOL:
	    break;
	case OCREP_PROP_STRING:
	    break;
	case OCREP_PROP_BYTE_STRING:
	    break;
	case OCREP_PROP_OBJECT:
	    break;
	case OCREP_PROP_ARRAY:
	    break;
	case OCREP_PROP_NULL:
	    break;
	}
    }
    return retval;
}

/*
 * Class:     org_iochibity_PayloadProperties
 * Method:    setProp
 * Signature: (Ljava/lang/String;Ljava/lang/Object;I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_iochibity_PayloadProperties_setProp
(JNIEnv * env, jobject this, jstring j_name, jobject j_value, jlong type)
{
    printf("Java_org_iochibity_PayloadProperties_setProp ENTRY\n");
    jclass klass = NULL;
    klass = (*env)->GetObjectClass(env, this);
    if (klass == NULL) {
	THROW_JNI_EXCEPTION("GetObjectClass failed for PayloadProperties object\n");
	return 0;
    }
    jfieldID fid_handle = (*env)->GetFieldID(env, klass, "handleOCRepPayload", "J");
    if (fid_handle == NULL) {
	THROW_JNI_EXCEPTION("GetFieldID failed for 'handleOCRepPayload' on PayloadProperties\n");
	return 0;
    }
    OCRepPayload* payload_rep = NULL;
    payload_rep = (OCRepPayload*)(intptr_t) (*env)->GetLongField(env, this, fid_handle);
    if (payload_rep == NULL) {
	THROW_JNI_EXCEPTION("GetLongField failed for handleOCRepPayload\n");
    }

    char* c_name = (char*) (*env)->GetStringUTFChars(env, j_name, NULL);
    printf("cname: %s\n", c_name);
    printf("payload rep uri: %s\n", payload_rep->uri);
    printf("value: %d\n", (int)j_value);

    jobject retval = NULL;
    bool rc;
    switch(type) {
    case OCREP_PROP_INT:
	/* rc = OCRepPayloadSetPropInt(payload_rep, (char*) c_name, (long long)j_value); */
	rc = OCRepPayloadSetPropInt(payload_rep, "TEMPR", 72);
	printf("OCRepPayloadSetPropInt rc: %d\n", rc);
	break;
    case OCREP_PROP_DOUBLE:
	break;
    case OCREP_PROP_BOOL:
	break;
    case OCREP_PROP_STRING:
	break;
    case OCREP_PROP_BYTE_STRING:
	break;
    case OCREP_PROP_OBJECT:
	break;
    case OCREP_PROP_ARRAY:
	break;
    case OCREP_PROP_NULL:
	break;
    }
    printf("Java_org_iochibity_PayloadProperties_setProp EXIT\n");
    return retval;
}
