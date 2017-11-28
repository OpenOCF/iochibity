/**
 * Error information from CA
 *        contains error code and message information.
 *
 * This structure holds error information.
 */
/* #if EXPORT_INTERFACE */
typedef struct
{
    CAResult_t result;  /**< CA API request result  */
    CAInfo_t info;      /**< message information such as token and payload data
                             helpful to identify the error */
} CAErrorInfo_t;

/**
 * Callback function type for error.
 * @param[out]   object           remote device information.
 * @param[out]   errorInfo        CA Error information.
 */
typedef void (*CAErrorCallback)(const CAEndpoint_t *object,
                                const CAErrorInfo_t *errorInfo);

/**
 * Callback function type for error.
 * @param[out]   object           remote device information.
 * @param[out]   result           error information.
 */
typedef CAResult_t (*CAHandshakeErrorCallback)(const CAEndpoint_t *object,
                                               const CAErrorInfo_t *errorInfo);
/* #endif	/\* EXPORT_INTERFACE *\/ */
