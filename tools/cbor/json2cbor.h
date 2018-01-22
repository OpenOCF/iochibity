/* This file was automatically generated.  Do not edit! */
#define HAVE_STDBOOL_H 1
#if defined(HAVE_STDBOOL_H)
#include <stdbool.h>
#endif
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
void OCLogInit(FILE *fd);
int main(int argc,char *argv[]);
extern FILE *logfd;
extern FILE *logfd;
#if defined(HAVE_WINDOWS_H)
#include <winsock2.h>
#include <windows.h>
#include <Bcrypt.h>
#endif
#include <stdint.h>
#include <stdbool.h>
#define UUID_STRING_SIZE (37)
char *OICStrcpy(char *dest,size_t destSize,const char *source);
#define OC_RSRVD_PROTOCOL_INDEPENDENT_ID "piid"
extern const char *OIC_JSON_RLIST_NAME;
extern const char *OIC_JSON_RLIST_NAME;
extern const char *OIC_JSON_PERIOD_NAME;
extern const char *OIC_JSON_PERIOD_NAME;
extern const char *OIC_JSON_CREDUSAGE_NAME;
extern const char *OIC_JSON_CREDUSAGE_NAME;
extern const char *OIC_JSON_REVOCATION_STATUS_NAME;
extern const char *OIC_JSON_REVOCATION_STATUS_NAME;
extern const char *OIC_JSON_OPTDATA_NAME;
extern const char *OIC_JSON_OPTDATA_NAME;
extern const char *OIC_JSON_PUBLICDATA_NAME;
extern const char *OIC_JSON_PUBLICDATA_NAME;
#define __WITH_DTLS__ /**/
extern const char *OIC_JSON_ENCODING_NAME;
extern const char *OIC_JSON_ENCODING_NAME;
extern const char *OIC_JSON_DATA_NAME;
extern const char *OIC_JSON_DATA_NAME;
extern const char *OIC_JSON_PRIVATEDATA_NAME;
extern const char *OIC_JSON_PRIVATEDATA_NAME;
extern const char *OIC_JSON_CREDTYPE_NAME;
extern const char *OIC_JSON_CREDTYPE_NAME;
extern const char *OIC_JSON_CREDID_NAME;
extern const char *OIC_JSON_CREDID_NAME;
extern const char *OIC_JSON_CREDS_NAME;
extern const char *OIC_JSON_CREDS_NAME;
#define WARNING 2
#include <stdlib.h>
extern const char *OIC_SEC_ENCODING_DER;
extern const char *OIC_SEC_ENCODING_DER;
extern const char *OIC_SEC_ENCODING_PEM;
extern const char *OIC_SEC_ENCODING_PEM;
extern const char *OIC_SEC_ENCODING_BASE64;
extern const char *OIC_SEC_ENCODING_BASE64;
extern const char *OIC_SEC_ENCODING_RAW;
extern const char *OIC_SEC_ENCODING_RAW;
#include "cbor.h"
#if defined(HAVE_WINDOWS_H)
#include <wincrypt.h>
#include <intsafe.h>
#endif
enum OicEncodingType_t {
    OIC_ENCODING_UNKNOW = 0,
    OIC_ENCODING_RAW = 1,
    OIC_ENCODING_BASE64 = 2,
    OIC_ENCODING_PEM = 3,
    OIC_ENCODING_DER = 4	/* oic.sec.encoding.der */
};
typedef enum OicEncodingType_t OicEncodingType_t;
extern const char *OIC_JSON_SM_NAME;
extern const char *OIC_JSON_SM_NAME;
enum OicSecDpom_t {
    MULTIPLE_SERVICE_SERVER_DRIVEN    = (0x1 << 0),
    SINGLE_SERVICE_SERVER_DRIVEN      = (0x1 << 1),
    SINGLE_SERVICE_CLIENT_DRIVEN      = (0x1 << 2),
};
typedef enum OicSecDpom_t OicSecDpom_t;
extern const char *OIC_JSON_OM_NAME;
extern const char *OIC_JSON_OM_NAME;
extern const char *OIC_JSON_TM_NAME;
extern const char *OIC_JSON_TM_NAME;
enum OicSecDpm {
    NORMAL                          = 0x0,
    RESET                           = (0x1 << 0),
    TAKE_OWNER                      = (0x1 << 1),
    BOOTSTRAP_SERVICE               = (0x1 << 2),
    SECURITY_MANAGEMENT_SERVICES    = (0x1 << 3),
    PROVISION_CREDENTIALS           = (0x1 << 4),
    PROVISION_ACLS                  = (0x1 << 5),
    VERIFY_SOFTWARE_VERSION         = (0x1 << 6),
    UPDATE_SOFTWARE                 = (0x1 << 7),
#ifdef MULTIPLE_OWNER
    TAKE_SUB_OWNER                  = (0x1 << 13),
#endif
};
typedef enum OicSecDpm OicSecDpm;
typedef enum OicSecDpm OicSecDpm_t;
extern const char *OIC_JSON_CM_NAME;
extern const char *OIC_JSON_CM_NAME;
extern const char *OIC_JSON_ISOP_NAME;
extern const char *OIC_JSON_ISOP_NAME;
#if (__STDC_VERSION__ >= 201112L) /* C11 */ && defined(__STDC__)
#include <assert.h>
#endif
#if defined(_MSC_VER) 
#include <assert.h>
#endif
#define HAVE__BOOL 1
#if !defined(HAVE__BOOL) && !(defined(HAVE_STDBOOL_H))
#   define _Bool signed char
#endif
#if !(defined(HAVE_STDBOOL_H))
# define bool _Bool
#endif
extern const char *OIC_JSON_P_NAME;
extern const char *OIC_JSON_P_NAME;
extern const char *OIC_JSON_S_NAME;
extern const char *OIC_JSON_S_NAME;
extern const char *OIC_JSON_DOS_NAME;
extern const char *OIC_JSON_DOS_NAME;
extern const char *OIC_JSON_DEVOWNERID_NAME;
extern const char *OIC_JSON_DEVOWNERID_NAME;
extern const char *OIC_JSON_DEVICE_ID_NAME;
extern const char *OIC_JSON_DEVICE_ID_NAME;
#if defined(MULTIPLE_OWNER)
typedef unsigned int OicSecMomType_t;
extern const char *OIC_JSON_MOM_NAME;
extern const char *OIC_JSON_MOM_NAME;
#endif
extern const char *OIC_JSON_OWNED_NAME;
extern const char *OIC_JSON_OWNED_NAME;
typedef enum {
    NO_SECURITY_MODE                = 0x0,
    SYMMETRIC_PAIR_WISE_KEY         = (0x1 << 0),
    SYMMETRIC_GROUP_KEY             = (0x1 << 1),
    ASYMMETRIC_KEY                  = (0x1 << 2),
    SIGNED_ASYMMETRIC_KEY           = (0x1 << 3),
    PIN_PASSWORD                    = (0x1 << 4),
    ASYMMETRIC_ENCRYPTION_KEY       = (0x1 << 5),
}OicSecCredType_t;
extern const char *OIC_JSON_SUPPORTED_CRED_TYPE_NAME;
extern const char *OIC_JSON_SUPPORTED_CRED_TYPE_NAME;
extern const char *OIC_JSON_OXM_SEL_NAME;
extern const char *OIC_JSON_OXM_SEL_NAME;
enum OicSecOxm_t {
    OIC_JUST_WORKS                          = 0x0, /* oic.sec.doxm.jw */
    OIC_RANDOM_DEVICE_PIN                   = 0x1,
    OIC_MANUFACTURER_CERTIFICATE            = 0x2,
    OIC_DECENTRALIZED_PUBLIC_KEY            = 0x3,
    OIC_OXM_COUNT,
#ifdef MULTIPLE_OWNER
    OIC_PRECONFIG_PIN                       = 0xFF00,
#endif //MULTIPLE_OWNER
    OIC_MV_JUST_WORKS                       = 0xFF01,
    OIC_CON_MFG_CERT                        = 0xFF02,
};
typedef enum OicSecOxm_t OicSecOxm_t;
extern const char *OIC_JSON_OXMS_NAME;
extern const char *OIC_JSON_OXMS_NAME;
extern const char *OIC_JSON_ROWNERID_NAME;
extern const char *OIC_JSON_ROWNERID_NAME;
extern const char *OIC_JSON_ROWNERID_NAME;
typedef struct OicSecValidity_t OicSecValidity_t;
struct OicSecValidity_t {
    char* period; // 0:R:S:Y:String
    char** recurrences; // 1:R:M:Y:Array of String
    size_t recurrenceLen; // the number of elts in recurrence
    OicSecValidity_t *next;
};
extern const char *OIC_JSON_VALIDITY_NAME;
extern const char *OIC_JSON_VALIDITY_NAME;
extern const char *OIC_JSON_PERMISSION_NAME;
extern const char *OIC_JSON_PERMISSION_NAME;
extern const char *OIC_JSON_WC_MINUS_NAME;
extern const char *OIC_JSON_WC_MINUS_NAME;
extern const char *OIC_JSON_WC_PLUS_NAME;
extern const char *OIC_JSON_WC_PLUS_NAME;
extern const char *OIC_JSON_WC_ASTERISK_NAME;
extern const char *OIC_JSON_WC_ASTERISK_NAME;
extern const char *OIC_JSON_WC_NAME;
extern const char *OIC_JSON_WC_NAME;
extern const char *OIC_JSON_IF_NAME;
extern const char *OIC_JSON_IF_NAME;
extern const char *OIC_JSON_RT_NAME;
extern const char *OIC_JSON_RT_NAME;
extern const char *OIC_JSON_REL_NAME;
extern const char *OIC_JSON_REL_NAME;
extern const char *OIC_JSON_HREF_NAME;
extern const char *OIC_JSON_HREF_NAME;
typedef struct OicSecRsrc_t OicSecRsrc_t;
enum OicSecAceResourceWildcard {
    NO_WILDCARD = 0,
    ALL_DISCOVERABLE,       // maps to "+" in JSON/CBOR
    ALL_NON_DISCOVERABLE,   // maps to "-" in JSON/CBOR
    ALL_RESOURCES           // maps to "*" in JSON/CBOR
};
typedef enum OicSecAceResourceWildcard OicSecAceResourceWildcard;
typedef enum OicSecAceResourceWildcard OicSecAceResourceWildcard_t;
struct OicSecRsrc_t {
    char *href; // 0:R:S:Y:String
    char *rel; // 1:R:S:N:String
    char** types; // 2:R:S:N:String Array
    size_t typeLen; // the number of elts in types
    char** interfaces; // 3:R:S:N:String Array
    size_t interfaceLen; // the number of elts in interfaces
    OicSecAceResourceWildcard_t wildcard;
    OicSecRsrc_t *next;
};
extern const char *OIC_JSON_RESOURCES_NAME;
extern const char *OIC_JSON_RESOURCES_NAME;
extern const char *OIC_JSON_AUTHCRYPT_NAME;
extern const char *OIC_JSON_AUTHCRYPT_NAME;
extern const char *OIC_JSON_ANONCLEAR_NAME;
extern const char *OIC_JSON_ANONCLEAR_NAME;
char *OICStrdup(const char *str);
extern const char *OIC_JSON_CONNTYPE_NAME;
extern const char *OIC_JSON_CONNTYPE_NAME;
extern const char *OIC_JSON_UUID_NAME;
extern const char *OIC_JSON_UUID_NAME;
extern const char *OIC_JSON_SUBJECT_NAME;
extern const char *OIC_JSON_SUBJECT_NAME;
#include "coap/uri.h"
#define WITH_PRESENCE /**/
typedef enum {
    /** Success status code - START HERE.*/
    OC_STACK_OK = 0,                /** 203, 205*/
    OC_STACK_RESOURCE_CREATED,      /** 201*/
    OC_STACK_RESOURCE_DELETED,      /** 202*/
    OC_STACK_CONTINUE,
    OC_STACK_RESOURCE_CHANGED,      /** 204*/
    /** Success status code - END HERE.*/

    /** Error status code - START HERE.*/
    OC_STACK_INVALID_URI = 20,
    OC_STACK_INVALID_QUERY,         /** 400*/
    OC_STACK_INVALID_IP,
    OC_STACK_INVALID_PORT,
    OC_STACK_INVALID_CALLBACK,
    OC_STACK_INVALID_METHOD,

    /** Invalid parameter.*/
    OC_STACK_INVALID_PARAM,
    OC_STACK_INVALID_OBSERVE_PARAM,
    OC_STACK_NO_MEMORY,
    OC_STACK_COMM_ERROR,            /** 504*/
    /* FIXME: support CAResult_t codes */
    OC_STACK_CA_SERVER_STARTED_ALREADY,
    OC_STACK_CA_SERVER_NOT_STARTED,
    /* CA_DESTINATION_NOT_REACHABLE,   /\**< Destination is not reachable *\/
     * CA_SOCKET_OPERATION_FAILED,     /\**< Socket operation failed *\/
     * CA_SEND_FAILED,                 /\**< Send request failed *\/
     * CA_RECEIVE_FAILED,              /\**< Receive failed *\/
     * CA_DESTINATION_DISCONNECTED,    /\**< Destination is disconnected *\/
     * CA_STATUS_NOT_INITIALIZED,      /\**< Not Initialized*\/
     * CA_DTLS_AUTHENTICATION_FAILURE, /\**< Decryption error in DTLS *\/
     * CA_HANDLE_ERROR_OTHER_MODULE,   /\**< Error happens but it should be handled in other module *\/ */
    OC_STACK_TIMEOUT,
    OC_STACK_ADAPTER_NOT_ENABLED,
    OC_STACK_NOTIMPL,

    /** Resource not found.*/
    OC_STACK_NO_RESOURCE,           /** 404*/

    /** e.g: not supported method or interface.*/
    OC_STACK_RESOURCE_ERROR,
    OC_STACK_SLOW_RESOURCE,
    OC_STACK_DUPLICATE_REQUEST,

    /** Resource has no registered observers.*/
    OC_STACK_NO_OBSERVERS,
    OC_STACK_OBSERVER_NOT_FOUND,
    OC_STACK_VIRTUAL_DO_NOT_HANDLE,
    OC_STACK_INVALID_OPTION,        /** 402*/

    /** The remote reply contained malformed data.*/
    OC_STACK_MALFORMED_RESPONSE,
    OC_STACK_PERSISTENT_BUFFER_REQUIRED,
    OC_STACK_INVALID_REQUEST_HANDLE,
    OC_STACK_INVALID_DEVICE_INFO,
    OC_STACK_INVALID_JSON,

    /** Request is not authorized by Resource Server. */
    OC_STACK_UNAUTHORIZED_REQ,      /** 401*/
    OC_STACK_TOO_LARGE_REQ,         /** 413*/

    /** Error code from PDM */
    OC_STACK_PDM_IS_NOT_INITIALIZED,
    OC_STACK_DUPLICATE_UUID,
    OC_STACK_INCONSISTENT_DB,

    /**
     * Error code from OTM
     * This error is pushed from DTLS interface when handshake failure happens
     */
    OC_STACK_AUTHENTICATION_FAILURE,
    OC_STACK_NOT_ALLOWED_OXM,
    OC_STACK_CONTINUE_OPERATION,

    /** Request come from endpoint which is not mapped to the resource. */
    OC_STACK_BAD_ENDPOINT,

    /** Insert all new error codes here!.*/
#ifdef WITH_PRESENCE
    OC_STACK_PRESENCE_STOPPED = 128,
    OC_STACK_PRESENCE_TIMEOUT,
    OC_STACK_PRESENCE_DO_NOT_HANDLE,
#endif

    /** Request is denied by the user*/
    OC_STACK_USER_DENIED_REQ,
    OC_STACK_NOT_ACCEPTABLE,

    /** ERROR code from server */
    OC_STACK_FORBIDDEN_REQ,          /** 403*/
    OC_STACK_INTERNAL_SERVER_ERROR,  /** 500*/
    OC_STACK_GATEWAY_TIMEOUT,        /** 504*/
    OC_STACK_SERVICE_UNAVAILABLE,    /** 503*/

    /** ERROR in stack.*/
    OC_STACK_ERROR = 255
    /** Error status code - END HERE.*/
}OCStackResult;
#if defined(_WIN32)
#include <BaseTsd.h>
#endif
#define HAVE_STRNCASECMP 1
#if defined(HAVE__STRNICMP) && defined(_MSC_VER) && defined(HAVE_STRNCASECMP)
#include <string.h>
#endif
#if !(defined(HAVE__STRNICMP)) && defined(_MSC_VER) && defined(HAVE_STRNCASECMP)
#include <string.h>
#endif
#if !(defined(_MSC_VER)) && defined(HAVE_STRNCASECMP)
#include <string.h>
#endif
#if defined(HAVE__STRNICMP		) && !(defined(HAVE_STRNCASECMP))
#include <string.h>
#endif
#if defined(_WIN32)
#  define OC_CALL   __stdcall
#endif
#if !(defined(_WIN32))
#  define OC_CALL
#endif
typedef struct OicUuid OicUuid;
#define HAVE_UNISTD_H 1
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#define UUID_LENGTH (128/8)
struct OicUuid {
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in unless this is defined elsewhere?
    uint8_t             id[UUID_LENGTH];
};
typedef struct OicUuid OicUuid_t;
OCStackResult OC_CALL ConvertStrToUuid(const char *strUuid,OicUuid_t *uuid);
extern const char *WILDCARD_RESOURCE_URI;
extern const char *WILDCARD_RESOURCE_URI;
extern const char *OIC_JSON_SUBJECTID_NAME;
extern const char *OIC_JSON_SUBJECTID_NAME;
extern const char *OIC_JSON_ACEID_NAME;
extern const char *OIC_JSON_ACEID_NAME;
typedef struct OicSecAce_t OicSecAce_t;
typedef enum {
    OicSecAceUuidSubject = 0, /* Default to this type. */
    OicSecAceRoleSubject,
    OicSecAceConntypeSubject
}OicSecAceSubjectType;
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
typedef struct OicSecRole OicSecRole;
#endif
#define ROLEID_LENGTH 64 // 64-byte authority max length
#define ROLEAUTHORITY_LENGTH 64 // 64-byte authority max length
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
struct OicSecRole {
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    char id[ROLEID_LENGTH];                 // 0:R:S:Y:String
    char authority[ROLEAUTHORITY_LENGTH];   // 1:R:S:N:String
};
typedef struct OicSecRole OicSecRole_t;
#endif
enum OicSecConntype {
    AUTH_CRYPT, // any subject requesting over authenticated and encrypted channel
    ANON_CLEAR, // any subject requesting over anonymous and unencrypted channel
};
typedef enum OicSecConntype OicSecConntype;
typedef enum OicSecConntype OicSecConntype_t;
struct OicSecAce_t {
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    OicSecAceSubjectType subjectType;
    union                               // 0:R:S:Y:{roletype|didtype|"*"}
    {
        OicUuid_t subjectuuid;          // Only valid for subjectType == OicSecAceUuidSubject
        OicSecRole_t subjectRole;       // Only valid for subjectType == OicSecAceRoleSubject
        OicSecConntype_t subjectConn;   // Only valid for subjectType == OicSecAceConntypeSubject
    };
    OicSecRsrc_t *resources;            // 1:R:M:Y:Resource
    uint16_t permission;                // 2:R:S:Y:UINT16
    OicSecValidity_t *validities;       // 3:R:M:N:Time-interval
    uint16_t aceid;                     // mandatory in ACE2
#ifdef MULTIPLE_OWNER
    OicUuid_t* eownerID;                //4:R:S:N:oic.uuid
#endif
    OicSecAce_t *next;
};
#define DEBUG 0
#if !defined(OC_LOG_LEVEL)
#define OC_MINIMUM_LOG_LEVEL    (DEBUG)
#endif
#if !(!defined(OC_LOG_LEVEL))
#define OC_MINIMUM_LOG_LEVEL    (OC_LOG_LEVEL)
#endif
#define OC_LOG_PRIVATE_DATA (1 << 31)
#define HAVE_TIME_H 1
#if defined(HAVE_TIME_H)
#include <time.h>
#endif
#define HAVE_SYS_TIME_H 1
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
typedef struct oc_mutex_internal *oc_mutex;
void oc_mutex_lock(oc_mutex mutex);
extern oc_mutex log_mutex;
void OCLog(int level,const char *tag,const char *logStr);
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
void oc_mutex_unlock(oc_mutex mutex);
#define TB_LOG /**/
#if defined(TB_LOG)
#define OIC_LOG(level, tag, logStr) \
    do { \
    if (((int)OC_MINIMUM_LOG_LEVEL) <= ((int)(level & (~OC_LOG_PRIVATE_DATA)))) \
	    oc_mutex_lock(log_mutex); \
	    OCLog((level), (__FILE__ ":" TOSTRING(__LINE__)), (logStr));	\
	    oc_mutex_unlock(log_mutex); \
    } while(0)
#endif
#if !(defined(TB_LOG))
#define OIC_LOG(level, tag, logStr)
#endif
#define VERIFY_SUCCESS(tag, op, logLevel) do{ if (!(op)) \
            {OIC_LOG((logLevel), tag, #op " failed!!"); goto exit; } }while(0)
extern const char *OIC_JSON_ACES_NAME;
extern const char *OIC_JSON_ACES_NAME;
extern const char *OIC_JSON_ACLIST2_NAME;
extern const char *OIC_JSON_ACLIST2_NAME;
extern const char *OIC_JSON_ACLIST_NAME;
extern const char *OIC_JSON_ACLIST_NAME;
#define VERIFY_NOT_NULL_RETURN(tag, arg, logLevel, retValue) do { if (NULL == (arg)) \
            { OIC_LOG((logLevel), tag, #arg " is NULL"); return retValue; } } while(0)
#define LOCAL static
typedef struct _OCDeviceProperties _OCDeviceProperties;
struct _OCDeviceProperties {
    /** Protocol Independent Id.*/
    char protocolIndependentId[UUID_STRING_SIZE];
};
typedef struct _OCDeviceProperties OCDeviceProperties;
void CleanUpDeviceProperties(OCDeviceProperties **deviceProperties);
void OCLogv(int level,const char *tag,int line_nbr,const char *format,...);
#if defined(TB_LOG)
#define OIC_LOG_V(level, tag, ...) \
    do { \
    if (((int)OC_MINIMUM_LOG_LEVEL) <= ((int)(level & (~OC_LOG_PRIVATE_DATA)))) \
	    OCLogv((level), __FILE__, __LINE__, __VA_ARGS__); \
    } while(0)
#endif
#if !(defined(TB_LOG))
#define OIC_LOG_V(level, tag, ...)
#endif
#define ERROR 3
#define VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(log_tag, err, log_message) \
    if ((CborNoError != (err)) && (CborErrorOutOfMemory != (err))) \
    { \
        if ((log_tag) && (log_message)) \
        { \
            OIC_LOG_V(ERROR, (log_tag), "%s with cbor error: \'%s\'.", \
                    (log_message), (cbor_error_string(err))); \
        } \
        goto exit; \
    }
void *OICCalloc(size_t num,size_t size);
OCStackResult DevicePropertiesToCBORPayload(const OCDeviceProperties *deviceProperties,uint8_t **payload,size_t *size);
LOCAL OCDeviceProperties *JSONToDPBin(const char *jsonStr);
#define OC_JSON_DEVICE_PROPS_NAME "DeviceProperties"
typedef struct OicSecCred_t OicSecCred_t;
void DeleteCredList(OicSecCred_t *cred);
OCStackResult CredToCBORPayloadWithRowner(const OicSecCred_t *credS,const OicUuid_t *rownerId,uint8_t **cborPayload,size_t *cborSize,int secureFlag);
LOCAL OicSecCred_t *JSONToCredBinWithRowner(const char *jsonStr,OicUuid_t *rownerId);
typedef struct OicSecKey OicSecKey;
struct OicSecKey {
    uint8_t                *data;
    size_t                  len;

    // TODO: This field added as workaround. Will be replaced soon.
    OicEncodingType_t encoding;

};
typedef struct OicSecKey OicSecKey_t;
typedef struct OicSecOpt OicSecOpt;
struct OicSecOpt {
    uint8_t                *data;
    size_t                  len;

    OicEncodingType_t encoding;
    bool                revstat;
};
typedef struct OicSecOpt OicSecOpt_t;
struct OicSecCred_t {
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    uint16_t            credId;         // 0:R:S:Y:UINT16
    OicUuid_t           subject;        // 1:R:S:Y:oic.uuid
    // If roleId.id is all zeroes, this property is not set.
    OicSecRole_t        roleId;         // 2:R:M:N:oic.sec.roletype
    OicSecCredType_t    credType;       // 3:R:S:Y:oic.sec.credtype
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
    OicSecKey_t         publicData;     // own cerificate chain
    char            *credUsage;            // 4:R:S:N:String
    OicSecOpt_t        optionalData;   // CA's cerificate chain
#endif /* __WITH_DTLS__  or __WITH_TLS__*/
    OicSecKey_t         privateData;    // 6:R:S:N:oic.sec.key
    char                *period;        // 7:R:S:N:String
    OicUuid_t            rownerID;      // 8:R:S:Y:oic.uuid
#ifdef MULTIPLE_OWNER
    OicUuid_t            *eownerID;     //9:R:S:N:oic.uuid
#endif //MULTIPLE_OWNER
    OicSecCred_t        *next;
};
extern const char *OIC_JSON_CRED_NAME;
extern const char *OIC_JSON_CRED_NAME;
typedef struct OicSecAmacl_t OicSecAmacl_t;
struct OicSecAmacl_t {
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    size_t              resourcesLen;   // the number of elts in Resources
    char                **resources;    // 0:R:M:Y:String
    OicSecAmacl_t         *next;
};
void DeleteAmaclList(OicSecAmacl_t *amacl);
OCStackResult AmaclToCBORPayload(const OicSecAmacl_t *amaclS,uint8_t **cborPayload,size_t *cborSize);
LOCAL OicSecAmacl_t *JSONToAmaclBin(const char *jsonStr);
extern const char *OIC_JSON_AMACL_NAME;
extern const char *OIC_JSON_AMACL_NAME;
typedef struct OicSecDoxm OicSecDoxm;
struct OicSecDoxm {
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    OicSecOxm_t         *oxm;           // 1:R:M:N:UINT16
    size_t              oxmLen;         // the number of elts in Oxm
    OicSecOxm_t         oxmSel;         // 2:R/W:S:Y:UINT16
    OicSecCredType_t    sct;            // 3:R:S:Y:oic.sec.credtype
    bool                owned;          // 4:R:S:Y:Boolean
    OicUuid_t           deviceID;       // 6:R:S:Y:oic.uuid
    bool                dpc;            // 7:R:S:Y:Boolean
    OicUuid_t           owner;          // 8:R:S:Y:oic.uuid
#ifdef MULTIPLE_OWNER
    OicSecSubOwner_t* subOwners;        //9:R/W:M:N:oic.uuid
    OicSecMom_t *mom;                   //10:R/W:S:N:oic.sec.mom
#endif //MULTIPLE_OWNER
    OicUuid_t           rownerID;       // 11:R:S:Y:oic.uuid
};
typedef struct OicSecDoxm OicSecDoxm_t;
void DeleteDoxmBinData(OicSecDoxm_t *doxm);
OCStackResult DoxmToCBORPayload(const OicSecDoxm_t *doxm,uint8_t **payload,size_t *size);
LOCAL OicSecDoxm_t *JSONToDoxmBin(const char *jsonStr);
extern const char *OIC_JSON_DOXM_NAME;
extern const char *OIC_JSON_DOXM_NAME;
typedef struct OicSecPstat OicSecPstat;
typedef struct OicSecDostype OicSecDostype;
enum OicSecDeviceOnboardingState {
    DOS_RESET = 0,
    DOS_RFOTM,
    DOS_RFPRO,
    DOS_RFNOP,
    DOS_SRESET,
    DOS_STATE_COUNT
};
typedef enum OicSecDeviceOnboardingState OicSecDeviceOnboardingState;
typedef enum OicSecDeviceOnboardingState OicSecDeviceOnboardingState_t;
struct OicSecDostype {
    OicSecDeviceOnboardingState_t state;
    bool                          pending;
};
typedef struct OicSecDostype OicSecDostype_t;
struct OicSecPstat {
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    OicSecDostype_t     dos;            // -:RW:S:Y:oic.sec.dostype
    bool                isOp;           // 0:R:S:Y:Boolean
    OicSecDpm_t         cm;             // 1:R:S:Y:oic.sec.dpmtype
    OicSecDpm_t         tm;             // 2:RW:S:Y:oic.sec.dpmtype
    OicSecDpom_t        om;             // 4:RW:M:Y:oic.sec.dpom
    size_t              smLen;          // the number of elts in Sm
    OicSecDpom_t        *sm;            // 5:R:M:Y:oic.sec.dpom
    uint16_t            commitHash;     // 6:R:S:Y:oic.sec.sha256
    OicUuid_t           rownerID;       // 7:R:S:Y:oic.uuid
};
typedef struct OicSecPstat OicSecPstat_t;
void DeletePstatBinData(OicSecPstat_t *pstat);
OCStackResult PstatToCBORPayload(const OicSecPstat_t *pstat,uint8_t **payload,size_t *size);
LOCAL OicSecPstat_t *JSONToPstatBin(const char *jsonStr);
extern const char *OIC_JSON_PSTAT_NAME;
extern const char *OIC_JSON_PSTAT_NAME;
typedef struct OicSecAcl_t OicSecAcl_t;
struct OicSecAcl_t {
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    OicUuid_t           rownerID;        // 0:R:S:Y:oic.uuid
    OicSecAce_t         *aces; // 1:R:M:N:ACE
};
void DeleteACLList(OicSecAcl_t *acl);
typedef enum {
    OIC_SEC_ACL_UNKNOWN = 0,
    OIC_SEC_ACL_V1 = 1,
    OIC_SEC_ACL_V2 = 2
}OicSecAclVersion_t;
OCStackResult AclToCBORPayload(const OicSecAcl_t *secAcl,OicSecAclVersion_t aclVersion,uint8_t **payload,size_t *size);
LOCAL OicSecAcl_t *JSONToAclBin(OicSecAclVersion_t *aclVersion,const char *jsonStr);
extern const char *OIC_JSON_ACL_NAME;
extern const char *OIC_JSON_ACL_NAME;
void OICFree(void *ptr);
#define INFO 1
#if !(defined(HAVE_STDBOOL_H))
# define false 0
# define true 1
#endif
void OICFreeAndSetToNull(void **ptr);
void *OICMalloc(size_t size);
#define FATAL 4
#define VERIFY_NOT_NULL(tag, arg, logLevel) do{ if (NULL == (arg)) \
            { OIC_LOG((logLevel), tag, #arg " is NULL"); goto exit; } }while(0)
#if !(defined(_WIN32))
extern const size_t DB_FILE_SIZE_BLOCK;
#endif
#if !defined(SIZE_MAX)
#  define SIZE_MAX ((size_t)-1)
#endif
