/**
 * Enums for CA return values.
 */
typedef enum // FIXME: where does this belong?
{
    /* Result code - START HERE */
    CA_STATUS_OK = 0,               /**< Success */
    CA_STATUS_INVALID_PARAM,        /**< 1 Invalid Parameter */
    CA_ADAPTER_NOT_ENABLED,         /**< 2 Adapter is not enabled */
    CA_SERVER_STARTED_ALREADY,      /**< 3 Server is started already */
    CA_SERVER_NOT_STARTED,          /**< 4 Server is not started */
    CA_DESTINATION_NOT_REACHABLE,   /**< 5 Destination is not reachable */
    CA_SOCKET_OPERATION_FAILED,     /**< 6 Socket operation failed */
    CA_SEND_FAILED,                 /**< 7 Send request failed */
    CA_RECEIVE_FAILED,              /**< 8 Receive failed */
    CA_MEMORY_ALLOC_FAILED,         /**< 9 Memory allocation failed */
    CA_REQUEST_TIMEOUT,             /**< 10 Request is Timeout */
    CA_DESTINATION_DISCONNECTED,    /**< 11 Destination is disconnected */
    CA_NOT_SUPPORTED,               /**< 12 Not supported */
    CA_STATUS_NOT_INITIALIZED,      /**< 13 Not Initialized*/
    CA_DTLS_HANDSHAKE_FAILURE,      /**< 14 DTLS handshake failed */
    CA_DTLS_AUTHENTICATION_FAILURE, /**< 15 Decryption error in DTLS */
    CA_CONTINUE_OPERATION,          /**< 16 Error happens but current operation should continue */
    CA_HANDLE_ERROR_OTHER_MODULE,   /**< 17 Error happens but it should be handled in other module */
    CA_STATUS_NOT_FOUND,            /**< Not Found*/
    CA_STATUS_FAILED =255           /**< 255 Failure */
    /* Result code - END HERE */
} CAResult_t;
