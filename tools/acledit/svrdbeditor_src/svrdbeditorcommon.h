/* This file was automatically generated.  Do not edit! */
#include "coap/uri.h"
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
OCStackResult OC_CALL ConvertStrToUuid(const char *strUuid,OicUuid_t *uuid);
#include <stdlib.h>
extern size_t WILDCARD_SUBJECT_ID_LEN;
extern OicUuid_t WILDCARD_SUBJECT_ID;
int InputUuid(OicUuid_t *uuid);
char *OICStrdup(const char *str);
char *InputString(const char *infoText);
int InputNumber(const char *infoText);
void PrintBuffer(const uint8_t *buf,size_t bufLen);
void PrintString(const char *text);
void PrintInt(int value);
void PrintStringArray(const char **array,size_t length);
void PrintIntArray(const int *array,size_t length);
void OICFree(void *ptr);
OCStackResult ConvertUuidToStr(const OicUuid_t *uuid,char **strUuid);
void PrintUuid(const OicUuid_t *uuid);
