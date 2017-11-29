/* This file was automatically generated.  Do not edit! */
void HandleAclOperation(const SubOperationType_t cmd);
typedef struct OicSecAce_t OicSecAce_t;
typedef enum {
    OicSecAceUuidSubject = 0, /* Default to this type. */
    OicSecAceRoleSubject,
    OicSecAceConntypeSubject
}OicSecAceSubjectType;
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
#define __WITH_DTLS__ /**/
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
typedef struct OicSecValidity_t OicSecValidity_t;
struct OicSecValidity_t {
    char* period; // 0:R:S:Y:String
    char** recurrences; // 1:R:M:Y:Array of String
    size_t recurrenceLen; // the number of elts in recurrence
    OicSecValidity_t *next;
};
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
int InputAceData(OicSecAce_t *ace);
#define PERMISSION_CREATE       (1 << 0)
#define PERMISSION_READ         (1 << 1)
#define PERMISSION_WRITE        (1 << 2)
#define PERMISSION_DELETE       (1 << 3)
#define PERMISSION_NOTIFY       (1 << 4)
#define PERMISSION_FULL_CONTROL (PERMISSION_CREATE | \
                                 PERMISSION_READ | \
                                 PERMISSION_WRITE | \
                                 PERMISSION_DELETE | \
                                 PERMISSION_NOTIFY)
#include <stdlib.h>
void *OICCalloc(size_t num,size_t size);
char *InputString(const char *infoText);
int InputUuid(OicUuid_t *uuid);
int InputNumber(const char *infoText);
extern const char *OIC_JSON_ROWNERID_NAME;
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
# define true 1
#endif
#define HAVE__BOOL 1
#if !defined(HAVE__BOOL) && !(defined(HAVE_STDBOOL_H))
#   define _Bool signed char
#endif
#if !(defined(HAVE_STDBOOL_H))
# define bool _Bool
#endif
size_t PrintAcl(void);
extern const char *OIC_JSON_PERMISSION_NAME;
#if defined(MULTIPLE_OWNER)
extern const char *OIC_JSON_EOWNERID_NAME;
#endif
extern const char *OIC_JSON_ANONCLEAR_NAME;
extern const char *OIC_JSON_AUTHCRYPT_NAME;
extern const char *OIC_JSON_CONNTYPE_NAME;
extern const char *OIC_JSON_ROLEID_NAME;
extern const char *OIC_JSON_AUTHORITY_NAME;
extern const char *OIC_JSON_ROLE_NAME;
void PrintUuid(const OicUuid_t *uuid);
void PrintString(const char *text);
extern OicUuid_t WILDCARD_SUBJECT_ID;
extern const char *OIC_JSON_UUID_NAME;
extern const char *OIC_JSON_SUBJECT_NAME;
extern const char *OIC_JSON_ACEID_NAME;
extern const char *OIC_JSON_IF_NAME;
extern const char *OIC_JSON_RT_NAME;
extern const char *OIC_JSON_REL_NAME;
extern const char *OIC_JSON_HREF_NAME;
void PrintStringArray(const char **array,size_t length);
extern const char *OIC_JSON_RESOURCES_NAME;
extern const char *OIC_JSON_PERIOD_NAME;
void FreeRsrc(OicSecRsrc_t *rsrc);
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
typedef struct OicSecAcl_t OicSecAcl_t;
struct OicSecAcl_t {
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    OicUuid_t           rownerID;        // 0:R:S:Y:oic.uuid
    OicSecAce_t         *aces; // 1:R:M:N:ACE
};
typedef enum {
    OIC_SEC_ACL_UNKNOWN = 0,
    OIC_SEC_ACL_V1 = 1,
    OIC_SEC_ACL_V2 = 2
}OicSecAclVersion_t;
OCStackResult AclToCBORPayload(const OicSecAcl_t *secAcl,OicSecAclVersion_t aclVersion,uint8_t **payload,size_t *size);
void OICFree(void *ptr);
OicSecAcl_t *CBORPayloadToAcl(const uint8_t *cborPayload,const size_t size);
extern const char *OIC_JSON_ACL_NAME;
OCStackResult GetSecureVirtualDatabaseFromPS(const char *resourceName,uint8_t **data,size_t *size);
void RefreshACL(void);
void DeleteACLList(OicSecAcl_t *acl);
void DeInitACL(void);
