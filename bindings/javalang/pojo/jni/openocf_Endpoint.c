
/**
 * @file openocf_utils_EndPoint.c
 * @author Gregg Reynolds
 * @date December 2016
 *
 * @brief JNI implementation of EndPoint Java API (OCDevAddr)
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "_threads.h"

//#include "openocf_Endpoint.h"
#include "openocf_Endpoint.ids.h"
#include "jni_utils.h"
#include "jni_init.h"
#include "org_iochibity_Exceptions.h"
//#include "uarraylist.h"

#include "map_keys.h"

#include "openocf.h"
/* #include "oic_malloc.h" */
/* #include "ocpayload.h" */
/* #include "ocresource.h" */
/* #include "ocresourcehandler.h" */
/* #include "ocstack.h" */

/* Endpoint = OCDevAddr */
jclass    K_ENDPOINT                      = NULL;
jmethodID MID_EP_CTOR                     = NULL;
jfieldID  FID_EP_HANDLE         = NULL;

jfieldID  FID_EP_NETWORK_FLAGS            = NULL;
jfieldID  FID_EP_NETWORK_POLICIES         = NULL;
jfieldID  FID_EP_NETWORK_SCOPE            = NULL;
jfieldID  FID_EP_TRANSPORT_SECURITY       = NULL;
jfieldID  FID_EP_PORT                     = NULL;
jfieldID  FID_EP_ADDRESS                  = NULL;
jfieldID  FID_EP_IFINDEX                  = NULL;
jfieldID  FID_EP_ROUTE_DATA               = NULL;


int init_Endpoint(JNIEnv* env)
{
    jclass klass = NULL;

    if (K_ENDPOINT == NULL) {
	klass = (*env)->FindClass(env, FQCN_ENDPOINT); // "openocf/Endpoint");
	JNI_ASSERT_NULL(klass, FINDCLASS_FAIL(FQCN_ENDPOINT), 0);
	K_ENDPOINT = (jclass)(*env)->NewGlobalRef(env, klass);
	(*env)->DeleteLocalRef(env, klass);
    }
    MID_EP_CTOR = (*env)->GetMethodID(env, K_ENDPOINT, "<init>", "()V");
    if (MID_EP_CTOR == 0) {
	printf("ERROR: GetMethodID failed for ctor of Endpoint.\n");
	return -1;
    }

    FID_EP_HANDLE = (*env)->GetFieldID(env, K_ENDPOINT, "_handle", "J");
    JNI_ASSERT_NULL(FID_EP_HANDLE,
                    ERR_MSG(ERR_FLD, "Endpoint._handle"), -1);
}

    /* FID_EP_NETWORK_FLAGS = (*env)->GetFieldID(env, K_ENDPOINT, "networkFlags", "I"); */
    /* if (FID_EP_NETWORK_FLAGS == NULL) { */
    /* 	printf("ERROR:  GetFieldID failed for networkFlags for Endpoint\n"); */
    /* 	fflush(NULL); */
    /* 	return OC_EH_INTERNAL_SERVER_ERROR; */
    /* } */
    /* FID_EP_NETWORK_POLICIES= (*env)->GetFieldID(env, K_ENDPOINT, "networkPolicies", "B"); */
    /* if (FID_EP_NETWORK_POLICIES == NULL) { */
    /* 	printf("ERROR:  GetFieldID failed for 'networkPolicy' of Endpoint\n"); */
    /* 	fflush(NULL); */
    /* 	return OC_EH_INTERNAL_SERVER_ERROR; */
    /* } */
    /* FID_EP_NETWORK_SCOPE= (*env)->GetFieldID(env, K_ENDPOINT, "networkScope", "B"); */
    /* if (FID_EP_NETWORK_SCOPE == NULL) { */
    /* 	printf("ERROR:  GetFieldID failed for 'networkPolicy' of Endpoint\n"); */
    /* 	fflush(NULL); */
    /* 	return OC_EH_INTERNAL_SERVER_ERROR; */
    /* } */
    /* FID_EP_TRANSPORT_SECURITY= (*env)->GetFieldID(env, K_ENDPOINT, "transportSecurity", "Z"); */
    /* if (FID_EP_TRANSPORT_SECURITY == NULL) { */
    /* 	printf("ERROR:  GetFieldID failed for 'transportSecurity' of Endpoint\n"); */
    /* 	fflush(NULL); */
    /* 	return OC_EH_INTERNAL_SERVER_ERROR; */
    /* } */
    /* FID_EP_PORT = (*env)->GetFieldID(env, K_ENDPOINT, "port", "I"); */
    /* if (FID_EP_PORT == NULL) { */
    /* 	printf("ERROR:  GetFieldID failed for port for Endpoint\n"); */
    /* 	fflush(NULL); */
    /* 	return OC_EH_INTERNAL_SERVER_ERROR; */
    /* } */
    /* FID_EP_ADDRESS = (*env)->GetFieldID(env, K_ENDPOINT, "address", "Ljava/lang/String;"); */
    /* if (FID_EP_ADDRESS == NULL) { */
    /* 	printf("ERROR:  GetFieldID failed for address of Endpoint\n"); */
    /* 	fflush(NULL); */
    /* 	return OC_EH_INTERNAL_SERVER_ERROR; */
    /* } */
    /* FID_EP_IFINDEX = (*env)->GetFieldID(env, K_ENDPOINT, "ifindex", "I"); */
    /* if (FID_EP_IFINDEX == NULL) { */
    /* 	printf("ERROR:  GetFieldID failed for ifindex Endpoint\n"); */
    /* 	fflush(NULL); */
    /* 	return OC_EH_INTERNAL_SERVER_ERROR; */
    /* } */
    /* FIXME */
    /* /\* OCDevAddr.routeData *\/ */
    /* if (crequest_in->devAddr.routeData) { */
    /* FID_EP_ROUTE_DATA = (*env)->GetFieldID(env, K_ENDPOINT, "routeData", "Ljava/lang/String;"); */
    /* if (FID_EP_ROUTE_DATA == NULL) { */
    /* 	    printf("ERROR: GetFieldID failed routeData of Endpoint\n"); */
    /* } */



/*
 * Class:     openocf_utils_EndPoint
 * Method:    networkProtocol
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_openocf_utils_EndPoint_networkProtocol
(JNIEnv * env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);
  if (tls_response_in == NULL) {
    // FIXME: if our TLS var has not been initialized, we have not
    // recieved a discovery response containing the remote
    // EndPoint info.
    // FIXME: return multicast info?
    return -1;
  } else {
    return RESPONSE_IN->devAddr.adapter;
  }
}

/*
 * Class:     openocf_utils_EndPoint
 * Method:    networkFlags
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_openocf_utils_EndPoint_networkFlags
(JNIEnv * env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);
  if (tls_response_in == NULL) {
    return -1;
  } else {
    return RESPONSE_IN->devAddr.flags;
  }
}

/*
 * Class:     openocf_utils_EndPoint
 * Method:    networkScope
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_openocf_utils_EndPoint_networkScope
(JNIEnv * env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);
  if (tls_response_in == NULL) {
    return -1;
  } else {
    return RESPONSE_IN->devAddr.flags & 0x0F;
  }
}

/*
 * Class:     openocf_Endpoint
 * Method:    getRemoteId
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_openocf_Endpoint_getRemoteId(JNIEnv *env, jobject this)
{
    OCDevAddr *ep_handle = (OCDevAddr*)(*env)->GetLongField(env, this, FID_EP_HANDLE);
    return(*env)->NewStringUTF(env, ep_handle->remoteId);
}

/*
 * Class:     openocf_utils_EndPoint
 * Method:    transportIsSecure
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_utils_EndPoint_transportIsSecure
(JNIEnv * env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);
  if (tls_response_in == NULL) {
    return false;
  } else {
    return  RESPONSE_IN->devAddr.flags & 0x0010;	/* (1 << 4) */
  }
}

/*
 * Class:     openocf_utils_EndPoint
 * Method:    isIPv6
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_utils_EndPoint_isIPv6
(JNIEnv * env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);
  if (tls_response_in == NULL) {
    return false;
  } else {
    return  RESPONSE_IN->devAddr.flags & 0x0020;	/* (1 << 5) */
  }
}

/*
 * Class:     openocf_utils_EndPoint
 * Method:    isIPv4
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_utils_EndPoint_isIPv4
(JNIEnv * env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);
  if (tls_response_in == NULL) {
    return false;
  } else {
    return  RESPONSE_IN->devAddr.flags & 0x0040;	/* (1 << 6) */
  }
}

/*
 * Class:     openocf_utils_EndPoint
 * Method:    isMulticast
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_utils_EndPoint_isMulticast
(JNIEnv * env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);
  if (tls_response_in == NULL) {
    return false;
  } else {
    return RESPONSE_IN->devAddr.flags & 0x0080;	/* (1 << 7) */
  }
}


/*
 * Class:     openocf_utils_EndPoint
 * Method:    port
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_openocf_utils_EndPoint_port
(JNIEnv * env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);
  if (tls_response_in == NULL) {
    return -1;
  } else {
    return RESPONSE_IN->devAddr.port;
  }
}

/*
 * Class:     openocf_utils_EndPoint
 * Method:    ipAddress
 * Signature: ()Ljava/lang/String;
 */
 jstring JNICALL Java_openocf_utils_EndPoint_ipAddress
(JNIEnv * env, jobject this)
{
    OC_UNUSED(env);
    OC_UNUSED(this);
    /* printf("%s : %s ENTRY, %d\n", __FILE__, __func__, (intptr_t)THREAD_ID); */
    /* printf("TLS: %d\n", tls_response_in); */

    if (tls_response_in) {
	return(*env)->NewStringUTF(env, RESPONSE_IN->devAddr.addr);
    } else {
	// FIXME: if our TLS var has not been initialized, we have not
	// recieved a discovery response containing the remote
	// EndPoint info.
	printf("FIXME: implement %s\n", __func__);
	return NULL;
    }
}

/*
 * Class:     openocf_utils_EndPoint
 * Method:    ifindex
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_openocf_utils_EndPoint_ifindex
(JNIEnv * env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);
  if (tls_response_in == NULL) {
    return -1;
  } else {
    return RESPONSE_IN->devAddr.ifindex;
  }
}

/*
 * Class:     openocf_utils_EndPoint
 * Method:    routeData
 * Signature: ()Ljava/lang/String;
 */
/* JNIEXPORT jstring JNICALL Java_openocf_utils_EndPoint_routeData */
/* (JNIEnv * env, jobject this) */
/* { */
/*   OC_UNUSED(env); */
/*   OC_UNUSED(this); */
/*   return RESPONSE_IN->routeData; */
/* } */

/*
 * Class:     openocf_Endpoint
 * Method:    getIPAddress
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_openocf_Endpoint_getIPAddress
(JNIEnv *env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);

  OCDevAddr *ep = (OCDevAddr*)(*env)->GetLongField(env, this, FID_EP_HANDLE);

  return(*env)->NewStringUTF(env, ep->addr);
}

/*
 * Class:     openocf_Endpoint
 * Method:    setIPAddress
 * Signature: (Ljava/lang/String;)Lopenocf/Endpoint;
 */
/* JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setIPAddress */
/* (JNIEnv *env, jobject this, jstring j_ipaddr){} */

/*
 * Class:     openocf_Endpoint
 * Method:    getBLEAddress
 * Signature: ()Ljava/lang/String;
 */
/* JNIEXPORT jstring JNICALL Java_openocf_Endpoint_getBLEAddress */
/* (JNIEnv *env, jobject this){} */

/*
 * Class:     openocf_Endpoint
 * Method:    getPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_openocf_Endpoint_getPort
(JNIEnv *env, jobject this)
{
  OC_UNUSED(env);
  OC_UNUSED(this);

  OCDevAddr *ep = (*env)->GetLongField(env, this, FID_EP_HANDLE);

  return ep->port;
}

/*
 * Class:     openocf_Endpoint
 * Method:    setPort
 * Signature: (I)Lopenocf/Endpoint;
 */
/* JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setPort */
/* (JNIEnv *env, jobject this, jint j_port){} */

/* /\* */
/*  * Class:     openocf_Endpoint */
/*  * Method:    getNetworkAdapter */
/*  * Signature: ()I */
/*  *\/ */
/* JNIEXPORT jint JNICALL Java_openocf_Endpoint_getNetworkAdapter */
/*   (JNIEnv *env, jobject); */

/* /\* */
/*  * Class:     openocf_Endpoint */
/*  * Method:    getNetworkFlags */
/*  * Signature: ()I */
/*  *\/ */
/* JNIEXPORT jint JNICALL Java_openocf_Endpoint_getNetworkFlags */
/*   (JNIEnv *env, jobject); */

/* /\* */
/*  * Class:     openocf_Endpoint */
/*  * Method:    isTransportSecure */
/*  * Signature: ()Z */
/*  *\/ */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isTransportSecure(JNIEnv *env, jobject this)
{
    jlong handle = (*env)->GetLongField(env, this, FID_EP_HANDLE);
    OCDevAddr *ep_handle = (OCDevAddr*) handle;
    return ep_handle->flags & OC_FLAG_SECURE;
}

/* /\* */
/*  * Class:     openocf_Endpoint */
/*  * Method:    setTransportSecure */
/*  * Signature: (Z)Lopenocf/Endpoint; */
/*  *\/ */
/* JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setTransportSecure */
/*   (JNIEnv *env, jobject, jboolean); */

/* /\* */
/*  * Class:     openocf_Endpoint */
/*  * Method:    isTransportUDP */
/*  * Signature: ()Z */
/*  *\/ */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isTransportUDP(JNIEnv *env, jobject this)
{
    jlong handle = (*env)->GetLongField(env, this, FID_EP_HANDLE);
    OCDevAddr *ep_handle = (OCDevAddr*) handle;
    return ep_handle->adapter & OC_ADAPTER_IP;
}

/* /\* */
/*  * Class:     openocf_Endpoint */
/*  * Method:    setTransportUDP */
/*  * Signature: (Z)Lopenocf/Endpoint; */
/*  *\/ */
/* JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setTransportUDP */
/*   (JNIEnv *env, jobject, jboolean); */

/*
 * Class:     openocf_Endpoint
 * Method:    isTransportTCP
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isTransportTCP(JNIEnv *env, jobject this)
{
    jlong handle = (*env)->GetLongField(env, this, FID_EP_HANDLE);
}

/*
 * Class:     openocf_Endpoint
 * Method:    setTransportTCP
 * Signature: (Z)Lopenocf/Endpoint;
 */
/* JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setTransportTCP */
/*   (JNIEnv *env, jobject this, jboolean j_torf){} */

/*
 * Class:     openocf_Endpoint
 * Method:    isTransportGATT
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isTransportGATT(JNIEnv *env, jobject this)
{
    jlong handle = (*env)->GetLongField(env, this, FID_EP_HANDLE);
    OCDevAddr *ep_handle = (OCDevAddr*) handle;
    return ep_handle->adapter & OC_ADAPTER_GATT_BTLE;
}


/*
 * Class:     openocf_Endpoint
 * Method:    setTransportGATT
 * Signature: (Z)Lopenocf/Endpoint;
 */
/* JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setTransportGATT */
/*   (JNIEnv *env, jobject this, jboolean j_torf){} */

/*
 * Class:     openocf_Endpoint
 * Method:    isTransportRFCOMM
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isTransportRFCOMM(JNIEnv *env, jobject this)
{
    jlong handle = (*env)->GetLongField(env, this, FID_EP_HANDLE);
    OCDevAddr *ep_handle = (OCDevAddr*) handle;
    return ep_handle->adapter & OC_ADAPTER_RFCOMM_BTEDR;
}


/*
 * Class:     openocf_Endpoint
 * Method:    setTransportRFCOMM
 * Signature: (Z)Lopenocf/Endpoint;
 */
/* JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setTransportRFCOMM */
/* (JNIEnv *env, jobject this, jboolean j_torf){} */

/*
 * Class:     openocf_Endpoint
 * Method:    isTransportNFC
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isTransportNFC(JNIEnv *env, jobject this)
{
    jlong handle = (*env)->GetLongField(env, this, FID_EP_HANDLE);
    OCDevAddr *ep_handle = (OCDevAddr*) handle;
    return ep_handle->adapter & OC_ADAPTER_NFC;
}

/*
 * Class:     openocf_Endpoint
 * Method:    setTransportNFC
 * Signature: (Z)Lopenocf/Endpoint;
 */
/* JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setTransportNFC */
/*   (JNIEnv *env, jobject this, jboolean j_torf){} */

/*
 * Class:     openocf_Endpoint
 * Method:    getAdapter
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_openocf_Endpoint_getAdapter
  (JNIEnv *env, jobject this){}

/*
 * Class:     openocf_Endpoint
 * Method:    isNetworkIP
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isNetworkIP
  (JNIEnv *env, jobject this){}

/*
 * Class:     openocf_Endpoint
 * Method:    isNetworkIPv4
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isNetworkIPv4(JNIEnv *env, jobject this)
{
    OCDevAddr *ep_handle = (OCDevAddr*)(*env)->GetLongField(env, this, FID_EP_HANDLE);
    return ep_handle->flags & OC_IP_USE_V4;
}


/*
 * Class:     openocf_Endpoint
 * Method:    setNetworkIPv4
 * Signature: (Z)Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setNetworkIPv4
  (JNIEnv *env, jobject this, jboolean j_torf){}

/*
 * Class:     openocf_Endpoint
 * Method:    isNetworkIPv6
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isNetworkIPv6(JNIEnv *env, jobject this)
{
    OCDevAddr *ep_handle = (OCDevAddr*)(*env)->GetLongField(env, this, FID_EP_HANDLE);
    return ep_handle->flags & OC_IP_USE_V6;
}

/*
 * Class:     openocf_Endpoint
 * Method:    setNetworkIPv6
 * Signature: (Z)Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setNetworkIPv6
(JNIEnv *env, jobject this, jboolean j_torf){}

/*
 * Class:     openocf_Endpoint
 * Method:    getNetworkScope
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_openocf_Endpoint_getNetworkScope
(JNIEnv *env, jobject this){}

/*
 * Class:     openocf_Endpoint
 * Method:    isScopeInterface
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isScopeInterface(JNIEnv *env, jobject this)
{
    OCDevAddr *ep_handle = (OCDevAddr*)(*env)->GetLongField(env, this, FID_EP_HANDLE);
    return ep_handle->flags & OC_SCOPE_INTERFACE;
}

/*
 * Class:     openocf_Endpoint
 * Method:    setScopeInterface
 * Signature: (Z)Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setScopeInterface
  (JNIEnv *env, jobject this, jboolean j_torf){}

/*
 * Class:     openocf_Endpoint
 * Method:    isScopeLink
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isScopeLink(JNIEnv *env, jobject this)
{
    OCDevAddr *ep_handle = (OCDevAddr*)(*env)->GetLongField(env, this, FID_EP_HANDLE);
    return ep_handle->flags & OC_SCOPE_LINK;
}

/*
 * Class:     openocf_Endpoint
 * Method:    setScopeLink
 * Signature: (Z)Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setScopeLink
  (JNIEnv *env, jobject this, jboolean j_torf){}

/*
 * Class:     openocf_Endpoint
 * Method:    isScopeRealm
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isScopeRealm
  (JNIEnv *env, jobject this){}

/*
 * Class:     openocf_Endpoint
 * Method:    setScopeRealm
 * Signature: (Z)Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setScopeRealm
  (JNIEnv *env, jobject this, jboolean j_torf){}

/*
 * Class:     openocf_Endpoint
 * Method:    isScopeAdmin
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isScopeAdmin
  (JNIEnv *env, jobject this){}

/*
 * Class:     openocf_Endpoint
 * Method:    setScopeAdmin
 * Signature: (Z)Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setScopeAdmin
  (JNIEnv *env, jobject this, jboolean j_torf){}

/*
 * Class:     openocf_Endpoint
 * Method:    isScopeSite
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isScopeSite
  (JNIEnv *env, jobject this){}

/*
 * Class:     openocf_Endpoint
 * Method:    setScopeSite
 * Signature: (Z)Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setScopeSite
(JNIEnv *env, jobject this, jboolean j_torf){}

/*
 * Class:     openocf_Endpoint
 * Method:    isScopeOrg
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isScopeOrg
(JNIEnv *env, jobject this){}

/*
 * Class:     openocf_Endpoint
 * Method:    setScopeOrg
 * Signature: (Z)Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setScopeOrg
(JNIEnv *env, jobject this, jboolean j_torf){}

/*
 * Class:     openocf_Endpoint
 * Method:    isScopeGlobal
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isScopeGlobal
  (JNIEnv *env, jobject this){}

/*
 * Class:     openocf_Endpoint
 * Method:    setScopeGlobal
 * Signature: (Z)Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setScopeGlobal
  (JNIEnv *env, jobject this, jboolean j_torf){}

/*
 * Class:     openocf_Endpoint
 * Method:    isRoutingMulticast
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_openocf_Endpoint_isRoutingMulticast(JNIEnv *env, jobject this)
{
    OCDevAddr *ep_handle = (OCDevAddr*)(*env)->GetLongField(env, this, FID_EP_HANDLE);
    return ep_handle->flags & OC_MULTICAST;
}

/*
 * Class:     openocf_Endpoint
 * Method:    setRoutingMulticast
 * Signature: (Z)Lopenocf/Endpoint;
 */
JNIEXPORT jobject JNICALL Java_openocf_Endpoint_setRoutingMulticast
(JNIEnv *env, jobject this, jboolean j_torf){}

/*
 * Class:     openocf_Endpoint
 * Method:    getIfIndex
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_openocf_Endpoint_getIfIndex(JNIEnv *env, jobject this)
{
    OCDevAddr *ep_handle = (OCDevAddr*)(*env)->GetLongField(env, this, FID_EP_HANDLE);
    return ep_handle->ifindex;
}

/*
 * Class:     openocf_Endpoint
 * Method:    getIfName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_openocf_Endpoint_getIfName(JNIEnv *env, jobject this)
{
    OCDevAddr *ep_handle = (OCDevAddr*)(*env)->GetLongField(env, this, FID_EP_HANDLE);
    char *ptr;
    char n[IF_NAMESIZE];
    ptr = if_indextoname(ep_handle->ifindex, &n);
    return(*env)->NewStringUTF(env, n);

}

/*
 * Class:     openocf_Endpoint
 * Method:    getLocalEndpoints
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL
Java_openocf_Endpoint_getLocalEndpoints(JNIEnv *env, jclass klass)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    jobject j_eps  = (*env)->NewObject(env, K_ARRAYLIST, MID_ARRAYLIST_CTOR_INT, 8);
    if (j_eps == NULL) {
	OIC_LOG_V(DEBUG, TAG, "NewObject failed for eps ArrayList");
	// printf("NewObject failed for eps ArrayList\n");
	return NULL;
    }

    jobject j_ep = NULL;

    jstring j_val = NULL;
    jint    j_ival = 0;

    jobject j_true = (*env)->GetStaticObjectField(env, K_BOOLEAN, FID_BOOL_TRUE);
    jobject j_false = (*env)->GetStaticObjectField(env, K_BOOLEAN, FID_BOOL_FALSE);

    u_arraylist_t *local_eps = oocf_get_local_endpoints();

    size_t len = u_arraylist_length(local_eps);
    OIC_LOG_V(DEBUG, TAG, "Local EP count: %d", len);

    bool bres;
    jobject ores;

    for (size_t i = 0; i < len; i++)
    {
	OIC_LOG_V(DEBUG, TAG, "EP %d", i);
        CAEndpoint_t *local_ep = u_arraylist_get(local_eps, i);

	j_ep  = (*env)->NewObject(env, K_HASHMAP, MID_HASHMAP_CTOR);
	if (j_ep == NULL) { OIC_LOG(INFO, TAG, "NewObject failed for ep HashMap"); return NULL;	}

	/* IP Address */
	j_val = (*env)->NewStringUTF(env, local_ep->addr);
	ores = (*env)->CallObjectMethod(env, j_ep, MID_HASHMAP_PUT, KEY_ADDR, j_val);

	/* Port */
	jobject port = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
						      local_ep->port);
	ores = (*env)->CallObjectMethod(env, j_ep, MID_HASHMAP_PUT, KEY_PORT, port);

	/* IF index */
	jobject index = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
						      local_ep->ifindex);
	ores = (*env)->CallObjectMethod(env, j_ep, MID_HASHMAP_PUT, KEY_INDEX, index);

	/* Transport (Adapter) */
	jobject transport = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
							   local_ep->adapter);
	ores = (*env)->CallObjectMethod(env, j_ep, MID_HASHMAP_PUT, KEY_TRANSPORT, transport);

	/* Secure? */
	if ( local_ep->flags & CA_SECURE )
	    ores = (*env)->CallObjectMethod(env, j_ep, MID_HASHMAP_PUT,
					    KEY_SECURE, j_true);
	else
	    ores = (*env)->CallObjectMethod(env, j_ep, MID_HASHMAP_PUT,
					    KEY_SECURE, j_false);

	/* Multicast? */
	if ( local_ep->flags & CA_MULTICAST )
	    ores = (*env)->CallObjectMethod(env, j_ep, MID_HASHMAP_PUT,
				     KEY_MCAST, j_true);
	else
	    ores = (*env)->CallObjectMethod(env, j_ep, MID_HASHMAP_PUT,
				     KEY_MCAST, j_false);


	/* local_ep->remoteId;	/\* String *\/ */

	/* store the ep map in the eps list */
	bres = (*env)->CallBooleanMethod(env, j_eps, MID_ARRAYLIST_ADD, j_ep);
	(*env)->DeleteLocalRef(env, j_ep);
	(*env)->DeleteLocalRef(env, j_ival);
    }

    // FIXME: just to be safe
    return j_eps;
}
