/* from connectivity/api/cacommon.h etc. */

/**
 * URI for the OIC base.CA considers relative URI as the URI.
 */
typedef char *CAURI_t;          /* FIXME: where does this belong? */

/**
 * Max URI length.
 */
#define CA_MAX_URI_LENGTH 512 /* maximum size of URI for other platforms*/

#define CA_IPFAMILY_MASK (CA_IPV6|CA_IPV4)
#define CA_SCOPE_MASK 0xf     // mask scope bits above

/** Bit mask for scope.*/
#define OC_MASK_SCOPE    (0x000F)

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

/**
 * Data structure to encapsulate IPv4/IPv6/Contiki/lwIP device addresses.
 * OCDevAddr must be the same as CAEndpoint (in CACommon.h).
 */
/* FIXME GAR: i.e. this is an abstraction for IP addresses? then why not put it in comm/ip/foo? */
#include <stdint.h>
typedef struct
{
    /** adapter type.*/
    OCTransportAdapter      adapter;

    /** transport modifiers.*/
    OCTransportFlags        flags;

    /** for IP.*/
    uint16_t                port;

    /** address for all adapters.*/
    /* GAR FIXME: meaning depends on adapter (=nw) type? */
    /* e.g. BLE address is 48 bits (6 bytes) http://microchipdeveloper.com/wireless:ble-link-layer-address */
    char                    addr[MAX_ADDR_STR_SIZE];

    /** usually zero for default interface.*/
    uint32_t                ifindex;

    /** destination GatewayID:ClientId.*/
    char                    routeData[MAX_ADDR_STR_SIZE];

    /** device UUID of remote. */
    char                    remoteId[MAX_IDENTITY_SIZE];

} OCDevAddr;

/**
 * This enum type includes elements of both ::OCTransportAdapter and ::OCTransportFlags.
 * It is defined conditionally because the smaller definition limits expandability on 32/64 bit
 * integer machines, and the larger definition won't fit into an enum on 16-bit integer machines
 * like Arduino.
 *
 * This structure must directly correspond to ::OCTransportAdapter and ::OCTransportFlags.
 */
typedef enum
{
    /** use when defaults are ok. */
    CT_DEFAULT = 0,

    /** IPv4 and IPv6, including 6LoWPAN.*/
    CT_ADAPTER_IP           = (1 << 16),

    /** GATT over Bluetooth LE.*/
    CT_ADAPTER_GATT_BTLE    = (1 << 17),

    /** RFCOMM over Bluetooth EDR.*/
    CT_ADAPTER_RFCOMM_BTEDR = (1 << 18),

#ifdef RA_ADAPTER
    /** Remote Access over XMPP.*/
    CT_ADAPTER_REMOTE_ACCESS = (1 << 19),
#endif
    /** CoAP over TCP.*/
    CT_ADAPTER_TCP     = (1 << 20),

    /** NFC Transport.*/
    CT_ADAPTER_NFC     = (1 << 21),

    /** Insecure transport is the default (subject to change).*/

    /** secure the transport path.*/
    CT_FLAG_SECURE     = (1 << 4),

    /** IPv4 & IPv6 autoselection is the default.*/

    /** IP adapter only.*/
    CT_IP_USE_V6       = (1 << 5),

    /** IP adapter only.*/
    CT_IP_USE_V4       = (1 << 6),

    /** Link-Local multicast is the default multicast scope for IPv6.
     * These are placed here to correspond to the IPv6 address bits.*/

    /** IPv6 Interface-Local scope(loopback).*/
    CT_SCOPE_INTERFACE = 0x1,

    /** IPv6 Link-Local scope (default).*/
    CT_SCOPE_LINK      = 0x2,

    /** IPv6 Realm-Local scope.*/
    CT_SCOPE_REALM     = 0x3,

    /** IPv6 Admin-Local scope.*/
    CT_SCOPE_ADMIN     = 0x4,

    /** IPv6 Site-Local scope.*/
    CT_SCOPE_SITE      = 0x5,

    /** IPv6 Organization-Local scope.*/
    CT_SCOPE_ORG       = 0x8,

    /** IPv6 Global scope.*/
    CT_SCOPE_GLOBAL    = 0xE,
} OCConnectivityType;

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
    OC_REST_GET            = (1 << 0),

    /** Write.*/
    OC_REST_PUT            = (1 << 1),

    /** Update.*/
    OC_REST_POST           = (1 << 2),

    /** Delete.*/
    OC_REST_DELETE         = (1 << 3),

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
 * Transport Protocol IDs.
 */
typedef enum
{
    /** For invalid ID.*/
    OC_INVALID_ID   = (1 << 0),

    /* For coap ID.*/
    OC_COAP_ID      = (1 << 1)
} OCTransportProtocolID;

/**
 * Sequence number is a 24 bit field,
 * per https://tools.ietf.org/html/rfc7641.
 */
#define MAX_SEQUENCE_NUMBER              (0xFFFFFF)

/**
 * This structure will be used to define the vendor specific header options to be included
 * in communication packets.
 */
typedef struct OCHeaderOption
{
    /** The protocol ID this option applies to.*/
    OCTransportProtocolID protocolID;

    /** The header option ID which will be added to communication packets.*/
    uint16_t optionID;

    /** its length 191.*/
    uint16_t optionLength;

    /** pointer to its data.*/
    uint8_t optionData[MAX_HEADER_OPTION_DATA_LENGTH];

/* #ifdef __cplusplus
 *     OCHeaderOption() = default;
 *     OCHeaderOption(OCTransportProtocolID pid,
 *                    uint16_t optId,
 *                    uint16_t optlen,
 *                    const uint8_t* optData)
 *         : protocolID(pid),
 *           optionID(optId),
 *           optionLength(optlen)
 *     {
 *
 *         // parameter includes the null terminator.
 *         optionLength = optionLength < MAX_HEADER_OPTION_DATA_LENGTH ?
 *                         optionLength : MAX_HEADER_OPTION_DATA_LENGTH;
 *         memcpy(optionData, optData, optionLength);
 *         optionData[optionLength - 1] = '\0';
 *     }
 * #endif // __cplusplus */
} OCHeaderOption;

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
(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest, void* callbackParam);

/**
 * Device Entity handler need to use this call back instead of OCEntityHandler.
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
typedef OCEntityHandlerResult (*OCDeviceEntityHandler)
(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest, char* uri, void* callbackParam);
