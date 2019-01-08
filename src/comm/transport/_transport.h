/* @todo: unify enums OCTransportAdapter and CATransportAdapter_t; */

/* union oocf_transport { // OCTransportAdapter and CATransportAdapter_t; */
/*     unsigned char all_transports; */
/*     struct { */
/* 	// FIXME: CA/OC_DEFAULT_ADAPTER is interpreted to mean ALL */
/* 	// adapters in ocserverrequest.c, cainterfacecontroller.c, */
/* 	// caconnectivitymanager.c */
/* 	//FIXME: delete X_DEFAULT_ADAPTER, use explicit X_ALL_ADAPTERS instead */
/* 	// unsigned char is_default : 1;  // udp? OC_DEFAULT_ADAPTER = 0,	/\* value zero indicates discovery. (?)*\/ */
/* 	// FIXME: IP is not a transport; does it mean UDP? see camessagehandler.c, search CA_DEFAULT_ADAPTER */
/* 	unsigned char is_udp : 1;  // OC_ADAPTER_IP = (1 << 0),	/\* IPv4 and IPv6, including 6LoWPAN.*\/ */
/* 	unsigned char is_tcp : 1; // OC_ADAPTER_TCP           = (1 << 4), /\* CoAP over TCP *\/ */
/* 	unsigned char is_gatt : 1;  // OC_ADAPTER_GATT_BTLE    = (1 << 1), */
/* 	unsigned char is_rfcomm : 1; // OC_ADAPTER_RFCOMM_BTEDR = (1 << 2), */
/* /\* #ifdef RA_ADAPTER *\/ */
/* /\* 	unsigned char is_ra : 1;  // OC_ADAPTER_REMOTE_ACCESS = (1 << 3), /\\* Remote Access over XMPP.*\\/ *\/ */
/* /\* #endif *\/ */
/* 	unsigned char is_nfc : 1; // OC_ADAPTER_NFC           = (1 << 5), /\** NFC Transport for Messaging.*\/ */
/* 	//OC_ALL_ADAPTERS          = 0xffffffff, same meaning as OC_DEFAULT_ADAPTER */
/*     } */
/* }; */
/* union oocf_transport transport; */
/* transport.is_udp = true; */
/* transport.is_tcp = true; */

/* @todo: unify structs CATransportFlags_t and OCTransportFlags */

/* struct oocf_transport_flags { /\* == OCTransportFlags *\/ */
/*     //CA_DEFAULT_FLAGS = 0;	/\* FIXME: meaning what? *\/ */
/*     uint16_t is_secure		: 1; // CA_SECURE          = (1 << 4),   // secure the transport path */
/*     uint16_t is_ipv6		: 1; //CA_IPV6            = (1 << 5),   // IP adapter only */
/*     uint16_t is_ipv4		: 1; // CA_IPV4            = (1 << 6),   // IP adapter only */
/*     uint16_t is_multicast	: 1; // CA_MULTICAST       = (1 << 7), */
/*     struct ipv6_scope { */
/* 	uint8_t is_interface	: 1; // CA_SCOPE_INTERFACE = 0x1, // IPv6 Interface-Local scope */
/* 	uint8_t is_link		: 1; // CA_SCOPE_LINK      = 0x2, // IPv6 Link-Local scope (default) */
/* 	uint8_t is_realm	: 1; // CA_SCOPE_REALM     = 0x3, // IPv6 Realm-Local scope */
/* 	uint8_t is_admin	: 1; // CA_SCOPE_ADMIN     = 0x4, // IPv6 Admin-Local scope */
/* 	uint8_t is_site		: 1; // CA_SCOPE_SITE      = 0x5, // IPv6 Site-Local scope */
/* 	uint8_t is_org		: 1; // CA_SCOPE_ORG       = 0x8, // IPv6 Organization-Local scope */
/* 	uint8_t is_global	: 1; // CA_SCOPE_GLOBAL    = 0xE, // IPv6 Global scope */
/*     } */
/* } */

#if EXPORT_INTERFACE
  /* OC_ prefix: from //src/ocf/octypes.h: */
typedef enum
{
    /** value zero indicates discovery.*/
    OC_DEFAULT_ADAPTER = 0,
    /* CA_DEFAULT_ADAPTER = 0, */

    /** IPv4 and IPv6, including 6LoWPAN.*/
    OC_ADAPTER_IP           = (1 << 0),
    /* CA_ADAPTER_IP            = (1 << 0),   // IPv4 and IPv6, including 6LoWPAN */
    /* CT_ADAPTER_IP           = (1 << 16), */

    /** GATT over Bluetooth LE.*/
    OC_ADAPTER_GATT_BTLE    = (1 << 1),
    /* CA_ADAPTER_GATT_BTLE     = (1 << 1),   // GATT over Bluetooth LE */
    /* CT_ADAPTER_GATT_BTLE    = (1 << 17), */

    /** RFCOMM over Bluetooth EDR.*/
    OC_ADAPTER_RFCOMM_BTEDR = (1 << 2),
    /* CA_ADAPTER_RFCOMM_BTEDR  = (1 << 2),   // RFCOMM over Bluetooth EDR */
    /* CT_ADAPTER_RFCOMM_BTEDR = (1 << 18), */

#ifdef RA_ADAPTER
    /**Remote Access over XMPP.*/
    OC_ADAPTER_REMOTE_ACCESS = (1 << 3),
    /* CA_ADAPTER_REMOTE_ACCESS = (1 << 3),   // Remote Access over XMPP. */
    /* CT_ADAPTER_REMOTE_ACCESS = (1 << 19), */
#endif

    /** CoAP over TCP.*/
    OC_ADAPTER_TCP           = (1 << 4),
    /* CA_ADAPTER_TCP           = (1 << 4),   // CoAP over TCP */
    /* CT_ADAPTER_TCP     = (1 << 20), */

    /** NFC Transport for Messaging.*/
    OC_ADAPTER_NFC           = (1 << 5),
    /* CA_ADAPTER_NFC           = (1 << 5),   // NFC Adapter */
    /* CT_ADAPTER_NFC     = (1 << 21), */

    OC_ALL_ADAPTERS          = 0xffffffff
    /* CA_ALL_ADAPTERS          = 0xffffffff */
} OCTransportAdapter;
/* } CATransportAdapter_t; */

/**
 *  Enum layout assumes some targets have 16-bit integer (e.g., Arduino).
 */
/* GAR: corresponding constants, e.g. CT_FLAG_SECURE, in OCConnectivityType*/
typedef enum
{
    /** default flag is 0*/
    OC_DEFAULT_FLAGS = 0,
    /* CA_DEFAULT_FLAGS = 0, */

    /** Insecure transport is the default (subject to change).*/
    /** secure the transport path*/
    OC_FLAG_SECURE     = (1 << 4),
    // Insecure transport is the default (subject to change)
    /* CA_SECURE          = (1 << 4),   // 0x10 secure the transport path */

    /** IPv4 & IPv6 auto-selection is the default.*/
    /** IP & TCP adapter only.*/
    OC_IP_USE_V6       = (1 << 5), /* 0x20 */
    /* CA_IPV6            = (1 << 5),   // IP adapter only */

    /** IP & TCP adapter only.*/
    OC_IP_USE_V4       = (1 << 6), /* 0x40 */
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

} OCTransportFlags;             /* ==  CATransportFlags_t */

#define CA_IPFAMILY_MASK (CA_IPV6|CA_IPV4)
#define CA_SCOPE_MASK 0xf     // mask scope bits above

/** Bit mask for scope.*/
#define OC_MASK_SCOPE    (0x000F)
#endif                        /* EXPORT_INTERFACE */

/* from cacommon.h */
typedef enum
{
    CA_DEFAULT_BT_FLAGS = 0,           /**< default BT flags. */
    /** flags for BLE transport */
    CA_LE_ADV_DISABLE   = 0x1,         /**< disable BLE advertisement. */
    CA_LE_ADV_ENABLE    = 0x2,         /**< enable BLE advertisement. */
    CA_LE_SERVER_DISABLE = (1 << 4),   /**< disable gatt server. */
    /** flags for EDR transport */
    CA_EDR_SERVER_DISABLE = (1 << 7)   /**< disable EDR server. */
} CATransportBTFlags_t;

