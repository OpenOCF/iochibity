/* This file was automatically generated.  Do not edit! */
void HandleDoxmOperation(const SubOperationType_t cmd);
#include <stdint.h>
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
int InputUuid(OicUuid_t *uuid);
#include "cbor.h"
#define HAVE_WINDOWS_H 1
#if defined(HAVE_WINDOWS_H)
#include <winsock2.h>
#include <windows.h>
#include <wincrypt.h>
#include <intsafe.h>
#endif
typedef enum {
    NO_SECURITY_MODE                = 0x0,
    SYMMETRIC_PAIR_WISE_KEY         = (0x1 << 0),
    SYMMETRIC_GROUP_KEY             = (0x1 << 1),
    ASYMMETRIC_KEY                  = (0x1 << 2),
    SIGNED_ASYMMETRIC_KEY           = (0x1 << 3),
    PIN_PASSWORD                    = (0x1 << 4),
    ASYMMETRIC_ENCRYPTION_KEY       = (0x1 << 5),
}OicSecCredType_t;
#include <stdlib.h>
void *OICCalloc(size_t num,size_t size);
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
# define true 1
#endif
int InputNumber(const char *infoText);
#if !(defined(HAVE_STDBOOL_H))
# define false 0
#endif
#define HAVE__BOOL 1
#if !defined(HAVE__BOOL) && !(defined(HAVE_STDBOOL_H))
#   define _Bool signed char
#endif
#if !(defined(HAVE_STDBOOL_H))
# define bool _Bool
#endif
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
extern const char *OIC_JSON_ROWNERID_NAME;
extern const char *OIC_JSON_DEVOWNERID_NAME;
void PrintUuid(const OicUuid_t *uuid);
extern const char *OIC_JSON_DEVICE_ID_NAME;
#if defined(MULTIPLE_OWNER)
extern const char *OIC_JSON_MOM_NAME;
#endif
extern const char *OIC_JSON_SUPPORTED_CRED_TYPE_NAME;
void PrintInt(int value);
extern const char *OIC_JSON_OXM_SEL_NAME;
void PrintIntArray(const int *array,size_t length);
extern const char *OIC_JSON_OXMS_NAME;
void PrintString(const char *text);
extern const char *OIC_JSON_OWNED_NAME;
void PrintDoxm(void);
#include <stdio.h>
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
OCStackResult UpdateSecureResourceInPS(const char *resourceName,const uint8_t *payload,size_t size);
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
OCStackResult DoxmToCBORPayload(const OicSecDoxm_t *doxm,uint8_t **payload,size_t *size);
void OICFree(void *ptr);
OCStackResult CBORPayloadToDoxm(const uint8_t *cborPayload,size_t size,OicSecDoxm_t **secDoxm);
extern const char *OIC_JSON_DOXM_NAME;
OCStackResult GetSecureVirtualDatabaseFromPS(const char *resourceName,uint8_t **data,size_t *size);
void RefreshDoxm(void);
int GetDoxmDevID(OicUuid_t *deviceuuid);
void DeleteDoxmBinData(OicSecDoxm_t *doxm);
void DeInitDoxm(void);
