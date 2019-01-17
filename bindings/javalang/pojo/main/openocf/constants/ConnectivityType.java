package openocf.constants;

public class ConnectivityType
// typedef enum
{
    /** use when defaults are ok. */
    public static final int DEFAULT = 0;

    /** IPv4 and IPv6, including 6LoWPAN.*/
    public static final int ADAPTER_IP           = (1 << 16);

    /** GATT over Bluetooth LE.*/
    public static final int ADAPTER_GATT_BTLE    = (1 << 17);

    /** RFCOMM over Bluetooth EDR.*/
    public static final int ADAPTER_RFCOMM_BTEDR = (1 << 18);

// #ifdef RA_ADAPTER
//     /** Remote Access over XMPP.*/
//     public static final int ADAPTER_REMOTE_ACCESS = (1 << 19);
// #endif
    /** CoAP over TCP.*/
    public static final int ADAPTER_TCP     = (1 << 20);

    /** NFC Transport.*/
    public static final int ADAPTER_NFC     = (1 << 21);

    /** Insecure transport is the default (subject to change).*/

    /** secure the transport path.*/
    public static final int FLAG_SECURE     = (1 << 4);

    /** IPv4 & IPv6 autoselection is the default.*/

    /** IP adapter only.*/
    public static final int IP_USE_V6       = (1 << 5);

    /** IP adapter only.*/
    public static final int IP_USE_V4       = (1 << 6);

    /** Link-Local multicast is the default multicast scope for IPv6.
     * These are placed here to correspond to the IPv6 address bits.*/

    /** IPv6 Interface-Local scope(loopback).*/
    public static final int SCOPE_INTERFACE = 0x1;

    /** IPv6 Link-Local scope (default).*/
    public static final int SCOPE_LINK      = 0x2;

    /** IPv6 Realm-Local scope.*/
    public static final int SCOPE_REALM     = 0x3;

    /** IPv6 Admin-Local scope.*/
    public static final int SCOPE_ADMIN     = 0x4;

    /** IPv6 Site-Local scope.*/
    public static final int SCOPE_SITE      = 0x5;

    /** IPv6 Organization-Local scope.*/
    public static final int SCOPE_ORG       = 0x8;

    /** IPv6 Global scope.*/
    public static final int SCOPE_GLOBAL    = 0xE;
}
