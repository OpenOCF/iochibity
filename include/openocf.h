#ifndef OPENOCF_H_
#define OPENOCF_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define OC_RC_SUCCESS 1
#define OC_RC_FAILURE -1

/* from octypes.h */
typedef enum
{
    /** Success status code - START HERE.*/
    /* OC_STACK_SUCCESS = 0, */
    /* OC_STACK_RESOURCE_CREATED, */
    /* OC_STACK_RESOURCE_DELETED, */
    /* OC_STACK_CONTINUE, */
    /* OC_STACK_RESOURCE_CHANGED, */
    /* /\** Success status code - END HERE.*\/ */

    /* /\** Error status code - START HERE.*\/ */
    /* OC_STACK_INVALID_URI = 20, */
    /* OC_STACK_INVALID_QUERY, */
    /* OC_STACK_INVALID_IP, */
    /* OC_STACK_INVALID_PORT, */
    /* OC_STACK_INVALID_CALLBACK, */
    /* OC_STACK_INVALID_METHOD, */

    /* /\** Invalid parameter.*\/ */
    OC_RC_INVALID_PARAM = 26,
    /* OC_STACK_INVALID_OBSERVE_PARAM, */
    OC_RC_NO_MEMORY = 28,
    /* OC_STACK_COMM_ERROR, */
    /* OC_STACK_TIMEOUT,		  /\* 30 *\/ */
    /* OC_STACK_ADAPTER_NOT_ENABLED, */
    OC_RC_NOTIMPL = 32,

/*     /\** Resource not found.*\/ */
/*     OC_STACK_NO_RESOURCE, */

/*     /\** e.g: not supported method or interface.*\/ */
/*     OC_STACK_RESOURCE_ERROR, */
/*     OC_STACK_SLOW_RESOURCE, */
/*     OC_STACK_DUPLICATE_REQUEST, */

/*     /\** Resource has no registered observers.*\/ */
/*     OC_STACK_NO_OBSERVERS, */
/*     OC_STACK_OBSERVER_NOT_FOUND, */
/*     OC_STACK_VIRTUAL_DO_NOT_HANDLE, */
/*     OC_STACK_INVALID_OPTION,	/\* 40 *\/ */

/*     /\** The remote reply contained malformed data.*\/ */
/*     OC_STACK_MALFORMED_RESPONSE, */
/*     OC_STACK_PERSISTENT_BUFFER_REQUIRED, */
/*     OC_STACK_INVALID_REQUEST_HANDLE, */
/*     OC_STACK_INVALID_DEVICE_INFO, */
/*     OC_STACK_INVALID_JSON, */

/*     /\** Request is not authorized by Resource Server. *\/ */
/*     OC_STACK_UNAUTHORIZED_REQ, */
/*     OC_STACK_TOO_LARGE_REQ, */

/*     /\** Error code from PDM *\/ */
/*     OC_STACK_PDM_IS_NOT_INITIALIZED, */
/*     OC_STACK_DUPLICATE_UUID, */
/*     OC_STACK_INCONSISTENT_DB,	/\* 50 *\/ */

/*     /\** */
/*      * Error code from OTM */
/*      * This error is pushed from DTLS interface when handshake failure happens */
/*      *\/ */
/*     OC_STACK_AUTHENTICATION_FAILURE, */

/*     /\** Insert all new error codes here!.*\/ */
/*     OC_STACK_OBSERVER_REGISTRATION_FAILURE, */
/*     OC_STACK_RESOURCE_UNOBSERVABLE, */
/*     OC_STACK_METHOD_NOT_ALLOWED, */

/* #ifdef WITH_PRESENCE */
/*     OC_STACK_PRESENCE_STOPPED = 128, */
/*     OC_STACK_PRESENCE_TIMEOUT, */
/*     OC_STACK_PRESENCE_DO_NOT_HANDLE, */
/* #endif */
    /** ERROR in stack.*/
    OC_RC_ERROR = 255
    /** Error status code - END HERE.*/
} OCReturnCode;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // OPENOCF_H_
