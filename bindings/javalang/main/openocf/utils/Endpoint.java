package openocf.utils;

import java.util.List;

import openocf.app.ICoResourceSP;
import openocf.app.CoResourceSP;

// Note that we have several "endpoint" structs:

// ocf/ocendpoint.c:

// typedef struct CAEndpoint_s
// {
//     CATransportAdapter_t    adapter;    // adapter type
//     CATransportFlags_t      flags;      // transport modifiers
//     uint16_t                port;       // for IP
//     char                    addr[MAX_ADDR_STR_SIZE_CA]; // address for all
//     uint32_t                ifindex;    // usually zero for default interface
//     char                    remoteId[CA_MAX_IDENTITY_SIZE]; // device ID of remote device
// #if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
//     char                    routeData[MAX_ADDR_STR_SIZE_CA]; /**< GatewayId:ClientId of
//                                                                     destination. **/
// #endif
// } CAEndpoint_t;

// ocf/_protocol.h:
// typedef struct
// {
//     OCTransportAdapter      adapter;
//     OCTransportFlags        flags;
//     uint16_t                port;
//     char                    addr[MAX_ADDR_STR_SIZE];
//     uint32_t                ifindex;
//     char                    routeData[MAX_ADDR_STR_SIZE];
//     char                    remoteId[MAX_IDENTITY_SIZE];
// } OCDevAddr;

// NOTE: "OCDevAddr must be the same as CAEndpoint (in CACommon.h)." But
// routeData is not ifdef'ed in OCDevAddr, and the order is reversed.
/*
 * An OCDevAddr is a "Data structure to encapsulate
 * IPv4/IPv6/Contiki/lwIP device addresses. OCDevAddr must be the same
 * as CAEndpoint (in CACommon.h)."
 */

// ocf/ocpayload.c:

// typedef struct OCEndpointPayload
// {
//     char* tps;
//     char* addr;
//     OCTransportFlags family;
//     uint16_t port;
//     uint16_t pri;
//     struct OCEndpointPayload* next;
// } OCEndpointPayload;

// OCEndpointPayload is used inside OCResourcePayload.

// The difference: OCEndpointPayload does not contain an interface index, remoteId, routeData

// And we often have e.g. OCDevAddr *endpoint, and things like:

// ocstack.c :: CopyDevAddrToEndpoint(const OCDevAddr *in, CAEndpoint_t *out)

// So we dispense with the "device" part and just call this an EndPoint

public class Endpoint	// OCDevAddr
{

    private long _handle; // ref to underlying C struct?

    // native public int       networkFlags();  // OCTransportFlags flags

    // // FIXME: support the predicates from CoResourceSP e.g. transportIsUDP etc.
    // native public boolean   transportIsSecure(); // flags & 0x0008
    // native public boolean   isIPv4();	         // flags & 0x0010
    // native public boolean   isIPv6();            // flags & 0x0020
    // native public boolean   isMulticast();       // flags & 0x0040

    // native public int       port();		// uint16_t

    // native public String    ipAddress();          // char addr[MAX_ADDR_STR_SIZE];

    // native public int       ifindex();		// uint32_t, usually zero for default interface.

    // // /* GAR: FIXME: this allows stack and app code to differ */
    // // #if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
    // //     char                    routeData[MAX_ADDR_STR_SIZE]; //destination GatewayID:ClientId
    // native public String    routeData();
    // // #endif
    // // } OCDevAddr;

    // wrap OCDevAddr
    // Address: char OCDevAddr.addr[MAX_ADDR_STR_SIZE];
    // Meaning of addr field depends on nw (adapter) type
    // address getters return null if type is inappropriate
    native public String            getIPAddress(); // excluding port
    native public Endpoint          setIPAddress(String ipaddr); // excluding port

    native public String            getBLEAddress();
    // etc. for other supported networks

    native public int               getPort();		// uint16_t OCDevAddr.port
    native public Endpoint          setPort(int port);	// uint16_t OCDevAddr.port

    // Raw bitfields
    native public int               getNetworkAdapter(); // OCTransportAdapter OCDevAddr.adapter (enum)
    native public int               getNetworkFlags();    // OCTransportFlags OCDevAddr.flags (enum)

    // networkFlags (devAddr.flags), broken out
    // CT_FLAG_SECURE = (1 << 4) = flags & 0x0008
    native public boolean           isTransportSecure();
    native public Endpoint      setTransportSecure(boolean torf);

    // Transport Protocol flags: mutually exclusive, setting one resets all the others.
    // (comments: xx_ is for CT_ or OC_)
    native public boolean           isTransportUDP();    // xx_ADAPTER_IP (misnamed) (1 << 16)
    native public Endpoint      setTransportUDP(boolean torf);

    native public boolean           isTransportTCP();    // xx_ADAPTER_TCP (1 << 20)
    native public Endpoint      setTransportTCP(boolean torf);

    native public boolean           isTransportGATT();   // xx_ADAPTER_GATT_BTLE (1 << 17)
    native public Endpoint      setTransportGATT(boolean torf);

    native public boolean           isTransportRFCOMM(); // xx_ADAPTER_RFCOMM_BTEDR (1 << 18)
    native public Endpoint      setTransportRFCOMM(boolean torf);

    native public boolean           isTransportNFC();    // xx_ADAPTER_NFC (1 << 21)
    native public Endpoint      setTransportNFC(boolean torf);

    // IP flags: only needed to select version of IP protocol
    native public int               getAdapter();   // OCTransportAdapter adapter;
    // // OC_ADAPTER_IP, OC_ADAPTER_GATT_BLE, etc.

    native public boolean            isNetworkIP(); // == transportIsUDP
    // native public Endpoint           setNetworkIsIP(boolean torf);

    native public boolean           isNetworkIPv4();     // xx_IP_USE_V4 (flags & 0x0010)
    native public Endpoint setNetworkIPv4(boolean torf);

    native public boolean           isNetworkIPv6();     // xx_IP_USE_V6 (flags & 0x0010)
    native public Endpoint setNetworkIPv6(boolean torf);

    // IPv6 only:
    native public byte              getNetworkScope();      // flags & 0x000F

    native public boolean           isScopeInterface();      // flags & 0x000F
    native public Endpoint setScopeInterface(boolean torf);      // flags & 0x000F

    native public boolean           isScopeLink();           // flags & 0x000F
    native public Endpoint setScopeLink(boolean torf);      // flags & 0x000F

    native public boolean           isScopeRealm();          // flags & 0x000F
    native public Endpoint setScopeRealm(boolean torf);      // flags & 0x000F

    native public boolean           isScopeAdmin();
    native public Endpoint setScopeAdmin(boolean torf);

    native public boolean           isScopeSite();
    native public Endpoint setScopeSite(boolean torf);

    native public boolean           isScopeOrg();
    native public Endpoint setScopeOrg(boolean torf);

    native public boolean           isScopeGlobal();
    native public Endpoint setScopeGlobal(boolean torf);

    // Routing
    native public boolean           isRoutingMulticast();
    native public Endpoint setRoutingMulticast(boolean torf);

    native public int getIfIndex();

    native public static List getLocalEndpoints();
}
