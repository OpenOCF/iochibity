/**
 * Response information received.
 *
 * This structure is used to hold response information.
 */
#if EXPORT_INTERFACE
typedef struct
{
    CAResponseResult_t result;  /**< Result for response by resource model */
    CAInfo_t info;              /**< Information of the response */
    bool isMulticast;
} CAResponseInfo_t;
#endif	/* INTERFACE */

#if EXPORT_INTERFACE
/**
 * Callback function type for response delivery.
 * @param[out]   object           Endpoint object from which the response is received.
 * @param[out]   responseInfo     Identifier which needs to be mapped with response.
 */
typedef void (*CAResponseCallback)(const CAEndpoint_t *object,
                                   const CAResponseInfo_t *responseInfo);
#endif	/* EXPORT_INTERFACE */
