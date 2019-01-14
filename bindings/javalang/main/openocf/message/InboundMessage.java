package openocf.message;

import openocf.Message;
import openocf.app.ICoResourceSP;
import openocf.app.CoResourceSP;
import openocf.utils.Endpoint;

import java.util.ArrayList;
import java.util.LinkedList;

// Wraps common elements of InboundRequest, InboundResponse: OCDevAddr, VendorSpecificHeaderOptions; Uri?

// Server side: InboundRequest, wrapper on:
//     typedef OCEntityHandlerResult (*OCEntityHandler)
//                      (OCEntityHandlerFlag flag,
//                       OCEntityHandlerRequest * entityHandlerRequest,
//                       void* callbackParam);

// Client side: InboundResponse, wrapper on:
//     typedef OCStackApplicationResult (* OCClientResponseHandler)
//                       (void *context,
//                        OCDoHandle handle, // GAR: misnamed, s/b txn_id
//                        OCClientResponse * clientResponse);

abstract public class InboundMessage extends Message
{

    native public String getUri(); // { return _uri; }

    abstract public int getMethod();

    // addressing stuffed moved to utils.Endpoint
    // // Address: char OCDevAddr.addr[MAX_ADDR_STR_SIZE];
    // // Meaning of addr field depends on nw (adapter) type
    // // address getters return null if type is inappropriate
    // native public String            getIPAddress(); // excluding port
    // native public String            getBLEAddress();
    // // etc. for other supported networks

    // native public int               getPort();		// uint16_t OCDevAddr.port

    // // Raw bitfields
    // native public int               getNetworkAdapter(); // OCTransportAdapter OCDevAddr.adapter (enum)
    // native public int               getNetworkFlags();    // OCTransportFlags OCDevAddr.flags (enum)

    // // networkFlags (devAddr.flags), broken out
    // // CT_FLAG_SECURE = (1 << 4) = flags & 0x0008
    // native public boolean           isTransportSecure();
    // native public CoResourceSP      setTransportSecure(boolean torf);

    // // Transport Protocol flags: mutually exclusive, setting one resets all the others.
    // // (comments: xx_ is for CT_ or OC_)
    // native public boolean           isTransportUDP();    // xx_ADAPTER_IP (misnamed) (1 << 16)
    // native public CoResourceSP      setTransportUDP(boolean torf);

    // native public boolean           isTransportTCP();    // xx_ADAPTER_TCP (1 << 20)
    // native public CoResourceSP      setTransportTCP(boolean torf);

    // native public boolean           isTransportGATT();   // xx_ADAPTER_GATT_BTLE (1 << 17)
    // native public CoResourceSP      setTransportGATT(boolean torf);

    // native public boolean           isTransportRFCOMM(); // xx_ADAPTER_RFCOMM_BTEDR (1 << 18)
    // native public CoResourceSP      setTransportRFCOMM(boolean torf);

    // native public boolean           isTransportNFC();    // xx_ADAPTER_NFC (1 << 21)
    // native public CoResourceSP      setTransportNFC(boolean torf);

    // // IP flags: only needed to select version of IP protocol
    // native public boolean            networkIsIP(); // == transportIsUDP
    // native public ICoResourceSP networkIsIP(boolean torf);

    // native public boolean           networkIsIPv4();     // xx_IP_USE_V4 (flags & 0x0010)
    // native public CoResourceSP networkIsIPv4(boolean torf);

    // native public boolean           networkIsIPv6();     // xx_IP_USE_V6 (flags & 0x0010)
    // native public CoResourceSP networkIsIPv6(boolean torf);

    // // IPv6 only:
    // native public boolean           scopeIsInterface();      // flags & 0x000F
    // native public CoResourceSP scopeIsInterface(boolean torf);      // flags & 0x000F

    // native public boolean           scopeIsLink();           // flags & 0x000F
    // native public CoResourceSP scopeIsLink(boolean torf);      // flags & 0x000F

    // native public boolean           scopeIsRealm();          // flags & 0x000F
    // native public CoResourceSP scopeIsRealm(boolean torf);      // flags & 0x000F

    // native public boolean           scopeIsAdmin();
    // native public CoResourceSP scopeIsAdmin(boolean torf);

    // native public boolean           scopeIsSite();
    // native public CoResourceSP scopeIsSite(boolean torf);

    // native public boolean           scopeIsOrg();
    // native public CoResourceSP scopeIsOrg(boolean torf);

    // native public boolean           scopeIsGlobal();
    // native public CoResourceSP scopeIsGlobal(boolean torf);

    // // Routing
    // native public boolean           routingIsMulticast();
    // native public CoResourceSP routingIsMulticast(boolean torf);

    // QoS is only for OutboundRequest (client request)
    // native public int               qualityOfService();
    // native public CoResourceSP qualityOfService(int qos);

    // @Override
    // abstract public Endpoint getEndpoint();

    // FIXME: payload stuff. make these abstract?
    // put this in Observedmessage:
    // the payload from the request PDU.
    // native public long getObservationRecordHandle();

   // native public ObservationList getObservationList();

}
