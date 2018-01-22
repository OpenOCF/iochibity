/**
 * Enums for CA return values.
 */
typedef enum
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
    CA_STATUS_FAILED =255           /**< 255 Failure */
    /* Result code - END HERE */
} CAResult_t;

/**
 * Enums for CA Response values.
 */
typedef enum
{
    /* Response status code - START HERE */
    CA_EMPTY = 0,                           /**< Empty */
    CA_CREATED = 201,                       /**< Created */
    CA_DELETED = 202,                       /**< Deleted */
    CA_VALID = 203,                         /**< Valid */
    CA_CHANGED = 204,                       /**< Changed */
    CA_CONTENT = 205,                       /**< Content */
    CA_CONTINUE = 231,                      /**< Continue */
    CA_BAD_REQ = 400,                       /**< Bad Request */
    CA_UNAUTHORIZED_REQ = 401,              /**< Unauthorized Request */
    CA_BAD_OPT = 402,                       /**< Bad Option */
    CA_FORBIDDEN_REQ = 403,                 /**< Forbidden Request */
    CA_NOT_FOUND = 404,                     /**< Not found */
    CA_METHOD_NOT_ALLOWED = 405,            /**< Method Not Allowed */
    CA_NOT_ACCEPTABLE = 406,                /**< Not Acceptable */
    CA_REQUEST_ENTITY_INCOMPLETE = 408,     /**< Request Entity Incomplete */
    CA_REQUEST_ENTITY_TOO_LARGE = 413,      /**< Request Entity Too Large */
    CA_INTERNAL_SERVER_ERROR = 500,         /**< Internal Server Error */
    CA_BAD_GATEWAY = 502,
    CA_SERVICE_UNAVAILABLE = 503,           /**< Server Unavailable */
    CA_RETRANSMIT_TIMEOUT = 504,            /**< Retransmit timeout */
    CA_PROXY_NOT_SUPPORTED = 505            /**< Proxy not enabled to service a request */
    /* Response status code - END HERE */
} CAResponseResult_t;
