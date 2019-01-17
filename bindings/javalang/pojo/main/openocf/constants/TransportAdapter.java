package openocf.constants;

public class TransportAdapter
{
    // typedef enum
    // {
    //     /** value zero indicates discovery.*/
    //     OC_DEFAULT_ADAPTER = 0,
    public static final int DEFAULT = 0;

    //     /** IPv4 and IPv6, including 6LoWPAN.*/
    //     OC_ADAPTER_IP           = (1 << 0),
    public static final int IP = (1 << 0);

    //     /** GATT over Bluetooth LE.*/
    //     OC_ADAPTER_GATT_BTLE    = (1 << 1),
    public static final int GATT_BTLE    = (1 << 1);

    //     /** RFCOMM over Bluetooth EDR.*/
    //     OC_ADAPTER_RFCOMM_BTEDR = (1 << 2),
    public static final int RFCOMM_BTEDR = (1 << 2);

    // #ifdef RA_ADAPTER
    //     /**Remote Access over XMPP.*/
    //     OC_ADAPTER_REMOTE_ACCESS = (1 << 3),
    // #endif
    //     /** CoAP over TCP.*/
    //     OC_ADAPTER_TCP           = (1 << 4),
    public static final int TCP           = (1 << 4);

    //     /** NFC Transport for Messaging.*/
    //     OC_ADAPTER_NFC           = (1 << 5)
    public static final int NFC           = (1 << 5);
    // } OCTransportAdapter;
}
