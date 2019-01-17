package openocf.constants;

public class PayloadType
{
    // typedef enum
    // {
    //     /** Contents of the payload are invalid */
    //     PAYLOAD_TYPE_INVALID,
    public static final int INVALID        = 0;
    //     /** The payload is an OCDiscoveryPayload */
    //     PAYLOAD_TYPE_DISCOVERY,
    public static final int DISCOVERY      = 1;
    //     /** The payload is an OCDevicePayload */
    //     PAYLOAD_TYPE_DEVICE,
    public static final int DEVICE         = 2;
    //     /** The payload is an OCPlatformPayload */
    //     PAYLOAD_TYPE_PLATFORM,
    public static final int PLATFORM       = 3;
    //     /** The payload is an OCRepPayload */
    //     PAYLOAD_TYPE_REPRESENTATION,
    public static final int REPRESENTATION = 4;
    //     /** The payload is an OCSecurityPayload */
    //     PAYLOAD_TYPE_SECURITY,
    public static final int SECURITY       = 5;
    //     /** The payload is an OCPresencePayload */
    //     PAYLOAD_TYPE_PRESENCE,
    public static final int PRESENCE       = 6;
    //     /** The payload is an OCRDPayload */
    //     PAYLOAD_TYPE_RD
    public static final int RD             = 7;
    // } OCPayloadType;
}
