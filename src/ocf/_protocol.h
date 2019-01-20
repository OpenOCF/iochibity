/* from connectivity/api/cacommon.h etc. */

/**
 * Max URI length.
 */
#define CA_MAX_URI_LENGTH 512 /* maximum size of URI for other platforms*/

/** Bit mask for Mods.*/
#define OC_MASK_MODS     (0x0FF0)
#define OC_MASK_FAMS     (OC_IP_USE_V6|OC_IP_USE_V4)

/**
 * End point identity.
 * FIXME GAR: what is end point "identity"? a UUID?
 */
typedef struct
{
    /** Identity Length */
    uint16_t id_length;

    /** Array of end point identity.*/
    unsigned char id[MAX_IDENTITY_SIZE];
} OCIdentity;			/* GAR misnamed? should be oc_uuid? */

/**
 * Universally unique identifier.
 */
typedef struct
{
    /** identitifier string.*/
    unsigned char id[UUID_IDENTITY_SIZE];
} OCUUIdentity;

/** bit shift required for connectivity adapter.*/
#define CT_ADAPTER_SHIFT 16

/** Mask Flag.*/
#define CT_MASK_FLAGS 0xFFFF

/** Mask Adapter.*/
#define CT_MASK_ADAPTER 0xFFFF0000

/**
 *  OCDoResource methods to dispatch the request
 */
typedef enum
{
    OC_REST_NOMETHOD       = 0,

    /** Read.*/
    OC_REST_GET             = (1 << 0),

    /** Write.*/
    OC_REST_PUT             = (1 << 1),

    /** Update.*/
    OC_REST_POST           = (1 << 2),

    /** Delete.*/
    OC_REST_DELETE          = (1 << 3),

    // FIXME: OBSERVE is not a method!
    /** Register observe request for most up date notifications ONLY.*/
    OC_REST_OBSERVE        = (1 << 4),

    /** Register observe request for all notifications, including stale notifications.*/
    OC_REST_OBSERVE_ALL    = (1 << 5),

#ifdef WITH_PRESENCE
    /** Subscribe for all presence notifications of a particular resource.*/
    OC_REST_PRESENCE       = (1 << 7),
#endif
    /** Allows OCDoResource caller to do discovery.*/
    OC_REST_DISCOVER       = (1 << 8)
} OCMethod;

#define OC_MASK_RESOURCE_SECURE    (OC_NONSECURE | OC_SECURE)

/**
 * Sequence number is a 24 bit field,
 * per https://tools.ietf.org/html/rfc7641.
 */
#define MAX_SEQUENCE_NUMBER              (0xFFFFFF)

/**
 *  This enum type for indicate Transport Protocol Suites
 */
typedef enum
{
    /** For initialize */
    OC_NO_TPS         = 0,

    /** coap + udp */
    OC_COAP           = 1,

    /** coaps + udp */
    OC_COAPS          = (1 << 1),

#ifdef TCP_ADAPTER
    /** coap + tcp */
    OC_COAP_TCP       = (1 << 2),

    /** coaps + tcp */
    OC_COAPS_TCP      = (1 << 3),
#endif
#ifdef HTTP_ADAPTER
    /** http + tcp */
    OC_HTTP           = (1 << 4),

    /** https + tcp */
    OC_HTTPS          = (1 << 5),
#endif
#ifdef EDR_ADAPTER
    /** coap + rfcomm */
    OC_COAP_RFCOMM    = (1 << 6),
#endif
#ifdef LE_ADAPTER
    /** coap + gatt */
    OC_COAP_GATT      = (1 << 7),
#endif
#ifdef NFC_ADAPTER
    /** coap + nfc */
    OC_COAP_NFC       = (1 << 8),
#endif
#ifdef RA_ADAPTER
    /** coap + remote_access */
    OC_COAP_RA        = (1 << 9),
#endif
    /** Allow all endpoint.*/
    OC_ALL       = 0xffff
} OCTpsSchemeFlags;

/**
 * Possible return values from client application callback
 *
 * A client application callback returns an OCStackApplicationResult to indicate whether
 * the stack should continue to keep the callback registered.
 */
typedef enum
{
    /** Make no more calls to the callback and call the OCClientContextDeleter for this callback */
    OC_STACK_DELETE_TRANSACTION = 0,
    /** Keep this callback registered and call it if an apropriate event occurs */
    OC_STACK_KEEP_TRANSACTION,
    OC_STACK_KEEP_RESPONSE	/* GAR client responsible for freeing response */
} OCStackApplicationResult;

/**
 * Application server implementations must implement this callback to consume requests OTA.
 * Entity handler callback needs to fill the resPayload of the entityHandlerRequest.
 *
 * When you set specific return value like OC_EH_CHANGED, OC_EH_CONTENT,
 * OC_EH_SLOW and etc in entity handler callback,
 * ocstack will be not send response automatically to client
 * except for error return value like OC_EH_ERROR.
 *
 * If you want to send response to client with specific result,
 * OCDoResponse API should be called with the result value.
 *
 * e.g)
 *
 * OCEntityHandlerResponse response;
 *
 * ..
 *
 * response.ehResult = OC_EH_CHANGED;
 *
 * ..
 *
 * OCDoResponse(&response)
 *
 * ..
 *
 * return OC_EH_OK;
 */
typedef OCEntityHandlerResult (*OCEntityHandler)
(OCEntityHandlerFlag flag, oocf_inbound_request /* OCEntityHandlerRequest */ * entityHandlerRequest, void* callbackParam);

