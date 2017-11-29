/* This file was automatically generated.  Do not edit! */
#include "cbor.h"
#define HAVE_WINDOWS_H 1
#if defined(HAVE_WINDOWS_H)
#include <winsock2.h>
#include <windows.h>
#include <wincrypt.h>
#include <intsafe.h>
#endif
#include <stdint.h>
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
OCStackResult RemoveCredentialByCredId(uint16_t credId);
void HandleCredOperation(SubOperationType_t cmd);
typedef struct OicSecCred_t OicSecCred_t;
void DeleteCredList(OicSecCred_t *cred);
OicSecCred_t *GetCredEntryByCredId(const uint16_t credId);
#define OXM_RANDOM_PIN_MAX_SIZE (32)
typedef struct OicUuid OicUuid;
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include <stddef.h>
#define UUID_LENGTH (128/8)
struct OicUuid {
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in unless this is defined elsewhere?
    uint8_t             id[UUID_LENGTH];
};
typedef struct OicUuid OicUuid_t;
int GetDoxmDevID(OicUuid_t *deviceuuid);
int InputUuid(OicUuid_t *uuid);
#define OWNER_PSK_LENGTH_256 256/8 //byte size of 256-bit key size
#define OWNER_PSK_LENGTH_128 128/8 //byte size of 128-bit key size
char *InputString(const char *infoText);
typedef struct OicSecKey OicSecKey;
enum OicEncodingType_t {
    OIC_ENCODING_UNKNOW = 0,
    OIC_ENCODING_RAW = 1,
    OIC_ENCODING_BASE64 = 2,
    OIC_ENCODING_PEM = 3,
    OIC_ENCODING_DER = 4	/* oic.sec.encoding.der */
};
typedef enum OicEncodingType_t OicEncodingType_t;
struct OicSecKey {
    uint8_t                *data;
    size_t                  len;

    // TODO: This field added as workaround. Will be replaced soon.
    OicEncodingType_t encoding;

};
typedef struct OicSecKey OicSecKey_t;
char *OICStrdup(const char *str);
int InputNumber(const char *infoText);
extern const char *OIC_JSON_ROWNERID_NAME;
OCStackResult GetCredRownerId(OicUuid_t *rowneruuid);
#if (__STDC_VERSION__ >= 201112L) /* C11 */ && defined(__STDC__)
#include <assert.h>
#endif
#if defined(_MSC_VER) 
#include <assert.h>
#endif
#define HAVE_STDBOOL_H 1
#if defined(HAVE_STDBOOL_H)
#include <stdbool.h>
#endif
#if !(defined(HAVE_STDBOOL_H))
# define false 0
#endif
extern const char *OIC_JSON_REVOCATION_STATUS_NAME;
extern const char *OIC_JSON_OPTDATA_NAME;
extern const char *MF_TRUST_CA;
extern const char *TRUST_CA;
typedef struct ByteArray ByteArray;
struct ByteArray {
    uint8_t *data;    /**< Pointer to the byte array */
    size_t len;      /**< Data size */
};
typedef struct ByteArray ByteArray_t;
#define __WITH_DTLS__ /**/
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
typedef struct {
    ByteArray_t crt;    /**< own certificate chain as a null-terminated PEM string of certificates */
    ByteArray_t key;    /**< own private key as binary-encoded DER */
    ByteArray_t ca;     /**< trusted CAs as a null-terminated PEM string of certificates */
    ByteArray_t crl;    /**< trusted CRLs as binary-encoded DER */
}PkiInfo_t;
#endif
extern const char *OIC_JSON_PUBLICDATA_NAME;
extern const char *MF_PRIMARY_CERT;
extern const char *PRIMARY_CERT;
extern const char *OIC_JSON_CREDUSAGE_NAME;
void PrintBuffer(const uint8_t *buf,size_t bufLen);
extern const char *OIC_JSON_DATA_NAME;
extern const char *OIC_JSON_ENCODING_NAME;
extern const char *OIC_JSON_PRIVATEDATA_NAME;
extern const char *OIC_JSON_CREDTYPE_NAME;
#if defined(MULTIPLE_OWNER)
extern const char *OIC_JSON_EOWNERID_NAME;
#endif
void PrintUuid(const OicUuid_t *uuid);
void PrintString(const char *text);
extern OicUuid_t WILDCARD_SUBJECT_ID;
extern const char *OIC_JSON_SUBJECTID_NAME;
void PrintInt(int value);
extern const char *OIC_JSON_CREDID_NAME;
#if !(defined(HAVE_STDBOOL_H))
# define true 1
#endif
#define HAVE__BOOL 1
#if !defined(HAVE__BOOL) && !(defined(HAVE_STDBOOL_H))
#   define _Bool signed char
#endif
#if !(defined(HAVE_STDBOOL_H))
# define bool _Bool
#endif
void PrintCredList(const OicSecCred_t *creds);
typedef enum {
    NO_SECURITY_MODE                = 0x0,
    SYMMETRIC_PAIR_WISE_KEY         = (0x1 << 0),
    SYMMETRIC_GROUP_KEY             = (0x1 << 1),
    ASYMMETRIC_KEY                  = (0x1 << 2),
    SIGNED_ASYMMETRIC_KEY           = (0x1 << 3),
    PIN_PASSWORD                    = (0x1 << 4),
    ASYMMETRIC_ENCRYPTION_KEY       = (0x1 << 5),
}OicSecCredType_t;
#include <inttypes.h>
#define DEBUG 0
#if !defined(OC_LOG_LEVEL)
#define OC_MINIMUM_LOG_LEVEL    (DEBUG)
#endif
#if !(!defined(OC_LOG_LEVEL))
#define OC_MINIMUM_LOG_LEVEL    (OC_LOG_LEVEL)
#endif
#define OC_LOG_PRIVATE_DATA (1 << 31)
void OCLogv(int level,const char *tag,int line_nbr,const char *format,...);
#define TB_LOG /**/
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
const OicSecCred_t *GetCredList();
#include <stdlib.h>
void *OICRealloc(void *ptr,size_t size);
#define WARNING 2
inline void ParseDerKey(ByteArray_t *key,const char *usage,uint16_t credId){
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    if (NULL == key || NULL == usage)
    {
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return;
    }

    OicSecCred_t *temp = NULL;
    key->len = 0;
    LL_FOREACH(((OicSecCred_t *)GetCredList()), temp)
    {
        if (SIGNED_ASYMMETRIC_KEY == temp->credType &&
            0 == strcmp(temp->credUsage, usage) &&
            temp->credId == credId)
        {
            key->data = OICRealloc(key->data, key->len + temp->privateData.len);
            memcpy(key->data + key->len, temp->privateData.data, temp->privateData.len);
            key->len += temp->privateData.len;
            OIC_LOG_V(DEBUG, TAG, "Key for %s found", usage);
        }
    }
    if (0 == key->len)
    {
        OIC_LOG_V(WARNING, TAG, "Key for %s not found", usage);
    }
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
};
typedef enum {
    /// The encode, decode operation was successful
    B64_OK = 0,
    /// Invalid input parameter.
    B64_INVALID_PARAM,
    /// The output buffer is not large enough for the encoded or decoded data.
    B64_OUTPUT_BUFFER_TOO_SMALL,
    /// Indicates error encoding or decoding.
    B64_ERROR
}B64Result;
B64Result b64Decode(const char *in,const size_t inLen,uint8_t *outBuf,size_t outBufSize,size_t *outLen);
void *OICCalloc(size_t num,size_t size);
#define B64DECODE_OUT_SAFESIZE(x) (((x)*3)/4)
void *OICMalloc(size_t size);
void *memmem(const void *haystack,size_t haystackLen,const void *needle,size_t needleLen);
#include <stdio.h>
OCStackResult UpdateSecureResourceInPS(const char *resourceName,const uint8_t *payload,size_t size);
OCStackResult CredToCBORPayload(const OicSecCred_t *credS,uint8_t **cborPayload,size_t *cborSize,int secureFlag);
OCStackResult AddCredential(OicSecCred_t *newCred);
OCStackResult DeInitCredResource();
OCStackResult SetCredRownerId(const OicUuid_t *newROwner);
void OICFree(void *ptr);
OCStackResult CBORPayloadToCred(const uint8_t *cborPayload,size_t size,OicSecCred_t **secCred,OicUuid_t **rownerid);
extern const char *OIC_JSON_CRED_NAME;
OCStackResult GetSecureVirtualDatabaseFromPS(const char *resourceName,uint8_t **data,size_t *size);
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
void RefreshCred();
#if !defined(_GNU_SOURCE)
# define _GNU_SOURCE 1
#endif
