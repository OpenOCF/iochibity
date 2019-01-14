#ifndef jni_init_h
#define jni_init_h

#include <jni.h>
#include "jni_typecodes.h"

#include "openocf.h"

#include "_threads.h"

/* externs used in CoServiceProvider, EndPoint */

/* keep a list of all outbound requests */
typedef struct transaction
{
    OCDoHandle /* void* */ txnId;	/* key; will be returned in response_in */
    OCMethod               method;
    bool                   routingIsMulticast;
    struct transaction   * next;
} txn_t;

extern txn_t* g_txn_list;

/* struct capturing params to OCDoResource */
typedef struct RequestOut
{
    OCDoHandle           txnId;	/* key: created by OCDoResource, returned in response_in */
    /* val: */
    OCMethod             method;
    const char         * requestUri;
    OCDevAddr          * destination;
    OCPayload          * payload;
    OCConnectivityType   connectivityType;
    OCQualityOfService   qos;
    OCCallbackData     * cbData;
    OCHeaderOption     * options;
    uint8_t              numOptions;

    bool                 routingIsMulticast;

    struct RequestOut  * next;
} request_out_t;

typedef struct ResponseIn
{
  OCDoHandle        * txnId;	/* key */
  OCClientResponse  * response;	/* val */
  /* struct ResponseIn * next; */
} response_in_t;

extern response_in_t*  g_response_map;

extern THREAD_LOCAL response_in_t* tls_response_in; /**< thread local etc. */

#define RESPONSE_IN ((OCClientResponse*)tls_response_in->response)

extern THREAD_LOCAL jobject g_CoSP;             /**< blah blah */

/* extern THREAD_LOCAL OCDevAddr* tls_defaultCoAddress; */

#define FINDCLASS_FAIL(arg) "FindClass failed for " arg

extern JavaVM* g_JVM;
extern pthread_key_t pthread_key;

extern jclass    K_ANDROID_HANDLER;
extern jmethodID MID_ANDROID_SEND_EMPTY_MSG;

extern jclass   K_OPENOCF;
extern jfieldID FID_OPENOCF_UI_HANDLER;

extern jclass    K_INTEGER;		/* 32-bit ints */
extern jmethodID MID_INT_CTOR;
extern jmethodID MID_INT_INTVALUE;	/* Integer.intValue() returns int */
extern jmethodID MID_INT_VALUE_OF;	/* Integer.valueOf(int) returns Integer */
extern jclass K_LONG;			/* 64-bit ints */
extern jmethodID MID_LONG_CTOR;
extern jmethodID MID_LONG_LONGVALUE;	/* Long.longValue() returns long */
/* jclass K_BIGINTEGER		/\* arbitrary bits *\/ */
extern jclass K_BOOLEAN;
extern jmethodID MID_BOOL_CTOR;
extern jmethodID MID_BOOL_BOOLVALUE;	/* Boolean.booleanValue() returns boolean */
extern jfieldID FID_BOOL_TRUE;
extern jfieldID FID_BOOL_FALSE;

extern jclass K_DOUBLE;
extern jmethodID MID_DBL_CTOR;
extern jmethodID MID_DBL_DBLVALUE;	/* Double.doubleValue() returns double */
extern jclass K_SHORT;
extern jmethodID MID_SHORT_CTOR;
extern jmethodID MID_SHORT_SHORTVALUE;
extern jclass K_STRING;
extern jmethodID MID_STR_CTOR;

extern jclass K_LIST;
extern jmethodID MID_LIST_SIZE;
extern jmethodID MID_LIST_GET;

extern jclass K_ARRAYLIST;
extern jmethodID MID_ARRAYLIST_CTOR;
extern jmethodID MID_ARRAYLIST_CTOR_INT;
extern jmethodID MID_ARRAYLIST_ADD;

extern jclass    K_LINKED_LIST;
extern jmethodID MID_LL_CTOR;
extern jmethodID MID_LL_ADD;
extern jmethodID MID_LL_GET;

extern jclass K_HASHMAP;
extern jmethodID MID_HASHMAP_CTOR;
extern jmethodID MID_HASHMAP_CTOR_INT;
extern jmethodID MID_HASHMAP_PUT;

extern jclass K_BYTE;
extern jmethodID MID_BYTE_CTOR;
extern jclass K_OBJECT;
/* jclass K_ARRAY; - List */

extern jclass    K_OBSERVATION;
extern jfieldID  FID_OBSERVATION_HANDLE;
extern jfieldID  FID_OBSERVATION_TYPE;
extern jmethodID MID_OBSERVATION_CTOR;
extern jfieldID  FID_OBSERVATION_URI_PATH;
extern jmethodID MID_OBSERVATION_GET_URI_PATH;
extern jfieldID  FID_OBSERVATION_RTYPES;
extern jmethodID MID_OBSERVATION_GET_RTYPES;
extern jfieldID  FID_OBSERVATION_IFS;
extern jmethodID MID_OBSERVATION_GET_IFS;
extern jfieldID  FID_OBSERVATION_PROPERTIES;
extern jmethodID MID_OBSERVATION_GET_PROPERTIES;
extern jfieldID  FID_OBSERVATION_CHILDREN;
extern jmethodID MID_OBSERVATION_GET_CHILDREN;

extern jclass    K_OBSERVATION_LIST;
extern jmethodID MID_PLL_CTOR;
extern jmethodID MID_PLL_ADD;

/* extern jclass   K_PFRS; 			/\* class for PayloadForResourceState *\/ */
/* extern jfieldID FID_PFRS_URI; */
/* extern jfieldID FID_PFRS_RTYPES; */
/* extern jfieldID FID_PFRS_INTERFACES; */
/* extern jfieldID FID_PFRS_PROPERTIES; */

/* extern jclass    K_PFP;   			/\* class for PayloadForPlatform *\/ */
/* extern jfieldID  FID_PFP_URI; */
/* extern jfieldID  FID_PFP_RTYPES; */
/* extern jfieldID  FID_PFP_INTERFACES; */
/* extern jfieldID  FID_PFP_PROPERTIES; */
/* extern jmethodID MID_PFP_CTOR; */

#define  FQCN_PMAP       "openocf/utils/PropertyMap"   /* Fully-Qualified-Class-Name */
#define  FQCS_PMAP       "Lopenocf/utils/PropertyMap;" /* Fully-Qualified-Class-Signature */

extern jclass   K_PMAP; 			/* class for PropertyMap */
extern jmethodID MID_PMAP_CTOR;
extern jmethodID MID_PMAP_SIZE;
extern jmethodID MID_PMAP_ENTRYSET;
extern jmethodID MID_PMAP_GET;
extern jmethodID MID_PMAP_PUT;

extern jclass   K_SET; 			/* interface java.util.Set */
extern jmethodID MID_SET_ITERATOR;

extern jclass   K_ITER; 			/* interface java.util.Iterator */
extern jmethodID MID_ITER_HASNEXT;
extern jmethodID MID_ITER_NEXT;
extern jmethodID MID_ITER_REMOVE;

extern jclass    K_MAPENTRY; 			/* interface java.util.Map.Entry */
extern jmethodID MID_ENTRY_GETKEY;
extern jmethodID MID_ENTRY_GETVALUE;

extern jclass    K_RESOURCE_LOCAL;

extern jclass    K_MSG_FOR_SERVICE_PROVIDER;
extern jfieldID  FID_MFSP_REMOTE_RQST_HANDLE;
extern jfieldID  FID_MFSP_RESOURCE_HANDLE;

extern jclass    K_OUTBOUND_REQUEST;
extern jmethodID MID_RQO_CTOR;
extern jfieldID  FID_RQO_LOCAL_HANDLE;
extern jfieldID  FID_RQO_CO_SERVICE_PROVIDER;
extern jfieldID  FID_RQO_METHOD;
/* extern jfieldID  FID_RQO_URI_PATH; */
extern jfieldID  FID_RQO_DEST;

#include "openocf_Message.ids.h"

#include "openocf_message_InboundRequest.ids.h"

#include "openocf_message_InboundResponse.ids.h"

extern jclass    K_MSG_RESPONSE_OUT;
extern jmethodID MID_MsgRspOut_CTOR;
extern jfieldID  FID_MsgRspOut_RQST_IN;

extern jclass    K_IRESOURCE_SP;
/* extern jmethodID MID_ISP_CTOR; */
/* extern jmethodID MID_ISP_OBSERVE_REQUEST; */

extern jclass    K_RESOURCE_SP;
extern jmethodID MID_RSP_REACT;
extern jfieldID  FID_RSP_HANDLE;
/* extern jfieldID  FID_RSP_ID; */
extern jfieldID  FID_RSP_URI_PATH;
extern jfieldID  FID_RSP_TYPES;
extern jfieldID  FID_RSP_INTERFACES;
extern jfieldID  FID_RSP_PROPERTIES;
extern jfieldID  FID_RSP_CHILDREN;
extern jfieldID  FID_RSP_ACTION_SET;
extern jfieldID  FID_RSP_POLICIES;

extern jclass    K_ICORESOURCE_SP;
extern jmethodID MID_ICOSP_COREACT;

extern jclass    K_CORESOURCE_SP;
/* extern jfieldID  FID_COSP_HANDLE; */
/* extern jfieldID  FID_COSP_MSG_RESPONSE_IN; */
/* extern jfieldID  FID_COSP_METHOD; */
extern jfieldID  FID_COSP_URI_PATH;
/* extern jfieldID  FID_COSP_DESTINATION; */
extern jmethodID MID_COSP_EXHIBIT;

extern FILE* server_fopen(const char *path, const char *mode);

/*
 *
 */
/* extern int prep_java(JNIEnv* env); */

/*
*
*/
extern void prep_pfrs(JNIEnv* env);

/*
 *
 */
extern void prep_pmap(JNIEnv* env);

/*
 * Routine:   props_to_OCRepPayloadValue
 * Signature: (Lorg/iochibity/PropertyMap;)J
 */
extern OCRepPayloadValue* props_to_OCRepPayloadValue(JNIEnv* env, jobject j_propmap);

/*
 * Routine:   pfrs_to_OCRepPayloadValue
 * Signature: (Lorg/iochibity/PayloadForResourceState;)J
 */
extern OCRepPayload* pfrs_to_OCRepPayload(JNIEnv* env, jobject j_pfrs);


extern OCEntityHandlerResult c_org_iochibity_ServiceProvider_observe_request(OCEntityHandlerFlag flag,
					      OCEntityHandlerRequest * c_EntityHandlerRequest,
					      void* j_IServiceProvider);

#endif
