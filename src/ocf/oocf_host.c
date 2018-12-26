#include "oocf_host.h"

/**
 * Host Mode of Operation.
 */
#if INTERFACE
typedef enum
{
    OC_CLIENT = 0,
    OC_SERVER,
    OC_CLIENT_SERVER,
    OC_GATEWAY          /**< Client server mode along with routing capabilities.*/
} OCMode;
#endif

/**
 * Request Information. Used for both outbound and inbound request messages.
 */
#if INTERFACE
typedef struct
{
    CAMethod_t method;  /**< Name of the Method Allowed */
    CAInfo_t info;      /**< Unpacked CoAP msg. */
    bool isMulticast;   /**< is multicast request */
    // FIXME: add endpoint info
} CARequestInfo_t;

/**
 * Response information. Used for both inbound and outbound response messages.
 */
typedef struct
{
    CAResponseResult_t result;  /**< Result for response by resource model */
    CAInfo_t info;              /**< CoAP msg, unpacked */
    bool isMulticast;
} CAResponseInfo_t;
#endif

OCMode myStackMode = 0;
