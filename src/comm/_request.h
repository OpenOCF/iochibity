/**
 * Request Information to be sent.
 *
 * This structure is used to hold request information.
 */
typedef struct
{
    CAMethod_t method;  /**< Name of the Method Allowed */
    CAInfo_t info;      /**< Information of the request. */
    bool isMulticast;   /**< is multicast request */
} CARequestInfo_t;

/**
 * Callback function type for request delivery.
 * @param[out]   object       Endpoint object from which the request is received.
 *                            It contains endpoint address based on the connectivity type.
 * @param[out]   requestInfo  Info for resource model to understand about the request.
 */
typedef void (*CARequestCallback)(const CAEndpoint_t *object,
                                  const CARequestInfo_t *requestInfo);
