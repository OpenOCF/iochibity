#include "errcheck.h"

#if EXPORT_INTERFACE
/* from ocstack.c: */
/* NOT USED: #define VERIFY_SUCCESS(op, successCode) { if ((op) != (successCode)) \
 *             {OIC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} } */
#define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OIC_LOG((logLevel), \
             TAG, #arg " is NULL"); return (retVal); } }
#define VERIFY_NON_NULL_NR(arg, logLevel) { if (!(arg)) { OIC_LOG((logLevel), \
             TAG, #arg " is NULL"); return; } }
#define VERIFY_NON_NULL_V(arg) { if (!arg) {OIC_LOG(FATAL, TAG, #arg " is NULL");\
    goto exit;} }

/**
 * Macro to verify the validity of cbor operation.
 */
/* FIXME: what this really means is "GOTO_EXIT_ON_ERROR_OTHER_THAN_OUT_OF_MEMORY" */
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

#define VERIFY_PARAM_NON_NULL(log_tag, err, log_message) \
    if (NULL == (err)) \
    { \
        OIC_LOG_V(FATAL, (log_tag), "%s", (log_message)); \
        goto exit;\
    }

#define VERIFY_CBOR_NOT_OUTOFMEMORY(log_tag, err, log_message) \
    if (CborErrorOutOfMemory == (err)) \
    { \
        if ((log_tag) && (log_message)) \
        { \
            OIC_LOG_V(ERROR, (log_tag), "%s with cbor error: \'%s\'.", \
                    (log_message), (cbor_error_string(err))); \
        } \
        goto exit; \
    }

/* from ocresrouce.c */
#define VERIFY_SUCCESS_1(op) { if (op != (OC_STACK_OK)) \
            {OIC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} }


/* from srmutililty.h: */

/**
 * Macro to verify success of operation.
 * eg: VERIFY_SUCCESS(TAG, OC_STACK_OK == foo(), ERROR);
 * @note Invoking function must define "exit:" label for goto functionality to work correctly.
 */
#define VERIFY_SUCCESS(tag, op, logLevel) do{ if (!(op)) \
            {OIC_LOG((logLevel), tag, #op " failed!!"); goto exit; } }while(0)

/**
 * Macro to verify expression evaluates to bool true.
 * eg: VERIFY_TRUE_OR_EXIT(TAG, OC_STACK_OK == foo(), ERROR);
 * @note Invoking function must define "exit:" label for goto functionality to work correctly.
 */
#define VERIFY_TRUE_OR_EXIT(tag, op, logLevel) do{ if (!(op)) \
            {OIC_LOG_V((logLevel), tag, "%s:" #op "evaluates to false!",__func__); \
            goto exit; } }while(0)

/**
 * Macro to verify success of operation.
 * eg: VERIFY_SUCCESS_RETURN(TAG, OC_STACK_OK == foo(), ERROR, OC_STACK_ERROR);
 */
#define VERIFY_SUCCESS_RETURN(tag, op, logLevel, retValue) do { if (!(op)) \
            {OIC_LOG((logLevel), tag, #op " failed!!"); return retValue;} } while(0)

/**
 * Macro to verify argument is not equal to NULL.
 * eg: VERIFY_NOT_NULL(TAG, ptrData, ERROR);
 * @note Invoking function must define "exit:" label for goto functionality to work correctly.
 */
#define VERIFY_NOT_NULL(tag, arg, logLevel) do{ if (NULL == (arg)) \
            { OIC_LOG((logLevel), tag, #arg " is NULL"); goto exit; } }while(0)

/**
 * Macro to verify argument is not equal to NULL.
 * eg: VERIFY_NOT_NULL_RETURN(TAG, ptrData, ERROR, OC_STACK_ERROR);
 */
#define VERIFY_NOT_NULL_RETURN(tag, arg, logLevel, retValue) do { if (NULL == (arg)) \
            { OIC_LOG((logLevel), tag, #arg " is NULL"); return retValue; } } while(0)

/* from occonnectionmanager.c */
/* #define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OIC_LOG((logLevel), \
 *              TAG, #arg " is NULL"); return (retVal); } } */
/* #define VERIFY_NON_NULL_NR(arg, logLevel) { if (!(arg)) { OIC_LOG((logLevel), \
 *              TAG, #arg " is NULL"); return; } } */

/* from ocendpoint.c */
#define VERIFY_NON_NULL_1(arg) { if (!arg) {OIC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }
#define VERIFY_GT_ZERO(arg) { if (arg < 1) {OIC_LOG(FATAL, TAG, #arg " < 1"); goto exit;} }
#define VERIFY_GT(arg1, arg2) { if (arg1 <= arg2) {OIC_LOG(FATAL, TAG, #arg1 " <= " #arg2); goto exit;} }
#define VERIFY_LT_OR_EQ(arg1, arg2) { if (arg1 > arg2) {OIC_LOG(FATAL, TAG, #arg1 " > " #arg2); goto exit;} }
#define VERIFY_SNPRINTF_RET(arg1, arg2) \
    { if (0 > arg1 || arg1 >= arg2) {OIC_LOG(FATAL, TAG, "Error (snprintf)"); goto exit;} } \

/* from ocobserve.c */
/* #define VERIFY_NON_NULL(arg) { if (!arg) {OIC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} } */

/* from ocserverrequest.c */
/* #define VERIFY_NON_NULL(arg) { if (!arg) {OIC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} } */

/* from oickeepalive.c */
/* NOT USED: #define VERIFY_SUCCESS(op, successCode) { if ((op) != (successCode)) \
 *             {OIC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} } */

/* #define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OIC_LOG((logLevel), \
 *              TAG, #arg " is NULL"); return (retVal); } } */

/* #define VERIFY_NON_NULL_NR(arg, logLevel) { if (!(arg)) { OIC_LOG((logLevel), \
 *              TAG, #arg " is NULL"); return; } } */

/* #define VERIFY_NON_NULL_V(arg) { if (!arg) {OIC_LOG_V(FATAL, TAG, "%s is NULL", #arg);\
 *     goto exit;} } */

/* from caifaddrs.c */
/* #define VERIFY_NON_NULL(arg) { if (!arg) {OIC_LOG(ERROR, TAG, #arg " is NULL"); goto exit;} } */

/* from oicresourcedirectory.c */
/* #define VERIFY_NON_NULL(arg) \
 * if (!(arg)) \
 * { \
 *     OIC_LOG(ERROR, TAG, #arg " is NULL"); \
 *     result = OC_STACK_NO_MEMORY; \
 *     goto exit; \
 * } */

/* from presence.c */
/* #define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OIC_LOG((logLevel), \
 *              __FILE__, #arg " is NULL"); return (retVal); } } */

/* #define VERIFY_NON_NULL_V(arg) { if (!arg) {OIC_LOG(FATAL, __FILE__, #arg " is NULL");\
 *     goto exit;} } */

/* from cacommonutil.h */
/**
 * Macro to verify the validity of input argument.
 *
 * @param  expr         Expression to verify.
 * @param  log_tag      Log tag.
 * @param  log_message  Log message.
 */
#define VERIFY_TRUE(expr, log_tag, log_message) \
    VERIFY_TRUE_RET((expr), (log_tag), (log_message), CA_STATUS_INVALID_PARAM)

/**
 * Macro to verify the validity of input argument.
 *
 * @param  expr         Expression to verify.
 * @param  log_tag      Log tag.
 * @param  log_message  Log message.
 * @param  ret          Return value.
 */
#define VERIFY_TRUE_RET(expr, log_tag, log_message, ret) \
    if (!(expr)) \
    { \
        OIC_LOG_V(ERROR, (log_tag), "Invalid input: %s", (log_message)); \
        return (ret); \
    } \

/**
 * Macro to verify the validity of input argument.
 *
 * @param  arg  log level
 * @param  log_tag  log tag
 * @param  log_message  log message
 * @param  ret  return value
 */
#define VERIFY_NON_NULL_RET(arg, log_tag, log_message, ret) \
    VERIFY_TRUE_RET(NULL != (arg), (log_tag), (log_message), (ret))

/**
 * Macro to verify the validity of input argument.
 *
 * @param  arg  log level
 * @param  log_tag  log tag
 * @param  log_message  log message
 */
#define VERIFY_NON_NULL_MSG(arg, log_tag, log_message) \
    VERIFY_NON_NULL_RET((arg), (log_tag), (log_message), CA_STATUS_INVALID_PARAM)
/* #define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OIC_LOG((logLevel), \
 *              TAG, #arg " is NULL"); return (retVal); } } */

/**
 * Macro to verify the validity of input argument.
 *
 * @param  arg  log level
 * @param  log_tag  log tag
 * @param  log_message  log message
 */
#define VERIFY_NON_NULL_VOID(arg, log_tag, log_message) \
    if (NULL == (arg)) { \
        OIC_LOG_V(ERROR, (log_tag), "Invalid input:%s", (log_message)); \
        return; \
    } \

#endif
