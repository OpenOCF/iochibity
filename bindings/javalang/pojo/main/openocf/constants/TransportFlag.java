package openocf.constants;

public class TransportFlag
{
    // /**
    //  *  Enum layout assumes some targets have 16-bit integer (e.g., Arduino).
    //  */
    // typedef enum
    // {
    // 	/** default flag is 0*/
    // 	OC_DEFAULT_FLAGS = 0,
    public static final int DEFAULT = 0;

    // 	/** Insecure transport is the default (subject to change).*/
    // 	/** secure the transport path*/
    // 	OC_FLAG_SECURE     = (1 << 4),
    public static final int SECURE     = (1 << 4);

    // 	/** IPv4 & IPv6 auto-selection is the default.*/
    // 	/** IP & TCP adapter only.*/
    // 	OC_IP_USE_V6       = (1 << 5),
    public static final int IP_USE_V6       = (1 << 5);

    // 	/** IP & TCP adapter only.*/
    // 	OC_IP_USE_V4       = (1 << 6),
    public static final int IP_USE_V4       = (1 << 6);

    // 	/** Multicast only.*/
    // 	OC_MULTICAST       = (1 << 7),
    public static final int MULTICAST       = (1 << 7);

    // 	/** Link-Local multicast is the default multicast scope for IPv6.
    // 	 *  These are placed here to correspond to the IPv6 multicast address bits.*/

    // 	/** IPv6 Interface-Local scope (loopback).*/
    // 	OC_SCOPE_INTERFACE = 0x1,
    public static final int SCOPE_INTERFACE = 0x1;

    // 	/** IPv6 Link-Local scope (default).*/
    // 	OC_SCOPE_LINK      = 0x2,
    public static final int SCOPE_LINK      = 0x2;

    // 	/** IPv6 Realm-Local scope. */
    // 	OC_SCOPE_REALM     = 0x3,
    public static final int SCOPE_REALM     = 0x3;

    // 	/** IPv6 Admin-Local scope. */
    // 	OC_SCOPE_ADMIN     = 0x4,
    public static final int SCOPE_ADMIN     = 0x4;

    // 	/** IPv6 Site-Local scope. */
    // 	OC_SCOPE_SITE      = 0x5,
    public static final int SCOPE_SITE      = 0x5;

    // 	/** IPv6 Organization-Local scope. */
    // 	OC_SCOPE_ORG       = 0x8,
    public static final int SCOPE_ORG       = 0x8;

    // 	/**IPv6 Global scope. */
    // 	OC_SCOPE_GLOBAL    = 0xE,
    public static final int SCOPE_GLOBAL    = 0xE;

    // } OCTransportFlags;
}
