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
#if EXPORT_INTERFACE
struct CARequestInfo
{
    CAMethod_t method;  /**< Name of the Method Allowed */

    // FIXME: rename info -> coap_msg
    CAInfo_t info;      /**< Unpacked CoAP msg. */
    bool isMulticast;   /**< is multicast request */

    CAEndpoint_t dest_ep;       /* sb origin_ep? this is the ep of requestor? */

    // from OCServerProtocolRequest, OCServerRequest
    uint8_t delayedResNeeded;   /* For delayed Response.*/
    /* uint8_t reqMorePacket; */ // OCServerProtocolRequest
    uint8_t requestComplete;      /* pdu is not a complete msg? */

    // from OCServerRequest
    uint8_t numResponses;
    OCEHResponseHandler ehResponseHandler;
    uint8_t slowFlag;           /* Flag indicating slow response.*/
    uint8_t notificationFlag;   /* Flag indicating notification.*/
    uint32_t observationOption;
    OCStackResult observeResult;

/** Node entry in red-black tree of linked lists.*/
    RBL_ENTRY(CARequestInfo) entry; /*  */
};

/**
 * Handle to an OCRequest object owned by the OCStack.
 */
/* src: octypes.h */
typedef struct CARequestInfo *OCRequestHandle;

#endif  /* EXPORT_INTERFACE */

#if INTERFACE
/**
 * Response information. Used for both inbound and outbound response messages.
 */
typedef struct                  /* wrapped CoAP PCU */
{
    CAResponseResult_t result;  /**< Result for response by resource model */
    CAInfo_t info;              /**< CoAP msg, unpacked */
    bool isMulticast;
} CAResponseInfo_t;
#endif

OCMode myStackMode = 0;
