#ifndef TRANSPORT_TYPES_H_
#define TRANSPORT_TYPES_H_

#include "iotivity_config.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

  /* CA_ prefix: from //src/comm/api/cacommon.h: */
  /* OC_ prefix: from //src/ocf/octypes.h: */

typedef enum
{
    /** value zero indicates discovery.*/
    OC_DEFAULT_ADAPTER = 0,
    /* CA_DEFAULT_ADAPTER = 0, */

    /** IPv4 and IPv6, including 6LoWPAN.*/
    OC_ADAPTER_IP           = (1 << 0),
    /* CA_ADAPTER_IP            = (1 << 0),   // IPv4 and IPv6, including 6LoWPAN */

    /** GATT over Bluetooth LE.*/
    OC_ADAPTER_GATT_BTLE    = (1 << 1),
    /* CA_ADAPTER_GATT_BTLE     = (1 << 1),   // GATT over Bluetooth LE */

    /** RFCOMM over Bluetooth EDR.*/
    OC_ADAPTER_RFCOMM_BTEDR = (1 << 2),
    /* CA_ADAPTER_RFCOMM_BTEDR  = (1 << 2),   // RFCOMM over Bluetooth EDR */

#ifdef RA_ADAPTER
    /**Remote Access over XMPP.*/
    OC_ADAPTER_REMOTE_ACCESS = (1 << 3),
    /* CA_ADAPTER_REMOTE_ACCESS = (1 << 3),   // Remote Access over XMPP. */
#endif

    /** CoAP over TCP.*/
    OC_ADAPTER_TCP           = (1 << 4),
    /* CA_ADAPTER_TCP           = (1 << 4),   // CoAP over TCP */

    /** NFC Transport for Messaging.*/
    OC_ADAPTER_NFC           = (1 << 5),
    /* CA_ADAPTER_NFC           = (1 << 5),   // NFC Adapter */

    OC_ALL_ADAPTERS          = 0xffffffff
    /* CA_ALL_ADAPTERS          = 0xffffffff */
} OCTransportAdapter;
/* } CATransportAdapter_t; */


/**
 *  Enum layout assumes some targets have 16-bit integer (e.g., Arduino).
 */
typedef enum
{
    /** default flag is 0*/
    OC_DEFAULT_FLAGS = 0,
    /* CA_DEFAULT_FLAGS = 0, */

    /** Insecure transport is the default (subject to change).*/
    /** secure the transport path*/
    OC_FLAG_SECURE     = (1 << 4),
    // Insecure transport is the default (subject to change)
    /* CA_SECURE          = (1 << 4),   // secure the transport path */

    /** IPv4 & IPv6 auto-selection is the default.*/
    /** IP & TCP adapter only.*/
    OC_IP_USE_V6       = (1 << 5),
    /* CA_IPV6            = (1 << 5),   // IP adapter only */

    /** IP & TCP adapter only.*/
    OC_IP_USE_V4       = (1 << 6),
    /* CA_IPV4            = (1 << 6),   // IP adapter only */

    /** Multicast only.*/
    OC_MULTICAST       = (1 << 7),
    // Indication that a message was received by multicast.
    /* CA_MULTICAST       = (1 << 7), */

    /** Link-Local multicast is the default multicast scope for IPv6.
     *  These are placed here to correspond to the IPv6 multicast address bits.*/

    /** IPv6 Interface-Local scope (loopback).*/
    OC_SCOPE_INTERFACE = 0x1,
    /* CA_SCOPE_INTERFACE = 0x1, // IPv6 Interface-Local scope */

    /** IPv6 Link-Local scope (default).*/
    OC_SCOPE_LINK      = 0x2,
    /* CA_SCOPE_LINK      = 0x2, // IPv6 Link-Local scope (default) */

    /** IPv6 Realm-Local scope. */
    OC_SCOPE_REALM     = 0x3,
    /* CA_SCOPE_REALM     = 0x3, // IPv6 Realm-Local scope */

    /** IPv6 Admin-Local scope. */
    OC_SCOPE_ADMIN     = 0x4,
    /* CA_SCOPE_ADMIN     = 0x4, // IPv6 Admin-Local scope */

    /** IPv6 Site-Local scope. */
    OC_SCOPE_SITE      = 0x5,
    /* CA_SCOPE_SITE      = 0x5, // IPv6 Site-Local scope */

    /** IPv6 Organization-Local scope. */
    OC_SCOPE_ORG       = 0x8,
    /* CA_SCOPE_ORG       = 0x8, // IPv6 Organization-Local scope */

    /**IPv6 Global scope. */
    OC_SCOPE_GLOBAL    = 0xE,
    /* CA_SCOPE_GLOBAL    = 0xE, // IPv6 Global scope */

} OCTransportFlags;
/* } CATransportFlags_t; */


typedef enum
{
    /** default flag is 0.*/
    OC_DEFAULT_BT_FLAGS = 0,
    /* CA_DEFAULT_BT_FLAGS = 0, */

    // flags for BLE transport
    /** disable BLE advertisement.*/
    OC_LE_ADV_DISABLE   = 0x1,
    /* CA_LE_ADV_DISABLE   = 0x1,   // disable BLE advertisement. */

    /** enable BLE advertisement.*/
    OC_LE_ADV_ENABLE    = 0x2,
    /* CA_LE_ADV_ENABLE    = 0x2,   // enable BLE advertisement. */

    /** disable gatt server.*/
    OC_LE_SERVER_DISABLE = (1 << 4),
    /* CA_LE_SERVER_DISABLE = (1 << 4),   // disable gatt server. */

    /** disable rfcomm server.*/
    OC_EDR_SERVER_DISABLE = (1 << 7)
    // flags for EDR transport
    /* CA_EDR_SERVER_DISABLE = (1 << 7) */
} OCTransportBTFlags_t;
/* } CATransportBTFlags_t; */

/*
 * MAX_ADDR_STR_SIZE_CA - fromm octypes.h and cacommon.h, both of
 * which contained copies of this code.
 */
#ifdef RA_ADAPTER
#define MAX_ADDR_STR_SIZE (256)
#else
/*
 * Max Address could be "coaps+tcp://[xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:yyy.yyy.yyy.yyy]:xxxxx"
 * Which is 65, +1 for null terminator => 66
 * OCDevAddr (defined in OCTypes.h) must be the same
 * as CAEndpoint_t (defined here)
 */
#define MAX_ADDR_STR_SIZE (66)
#endif
#define MAX_ADDR_STR_SIZE_CA MAX_ADDR_STR_SIZE

/**
 * Max header options data length.
 */
#ifdef ARDUINO
#define CA_MAX_HEADER_OPTION_DATA_LENGTH 20
#else
#define CA_MAX_HEADER_OPTION_DATA_LENGTH 1024
#endif

  // from comm/api/cacommon.h, also used in ocf/ocobserve.h etc.:
/**
 * Token information for mapping the request and responses by resource model.
 */
typedef char *CAToken_t;

/**
 * Transport Protocol IDs for additional options.
 */
typedef enum
{
    CA_INVALID_ID = (1 << 0),   /**< Invalid ID */
    CA_COAP_ID = (1 << 1)       /**< COAP ID */
} CATransportProtocolID_t;


  // from comm/api/cacommon.h, also used in ocf/ocobserve.h etc.:
/**
 * Header options structure to be filled.
 *
 * This structure is used to hold header information.
 */
typedef struct
{
    CATransportProtocolID_t protocolID;                     /**< Protocol ID of the Option */
    uint16_t optionID;                                      /**< The header option ID which will be
                                                            added to communication packets */
    uint16_t optionLength;                                  /**< Option Length **/
    char optionData[CA_MAX_HEADER_OPTION_DATA_LENGTH];      /**< Optional data values**/
} CAHeaderOption_t;



#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* TRANSPORT_TYPES_H_ */
