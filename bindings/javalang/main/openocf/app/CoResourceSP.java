package openocf.app;

import openocf.message.InboundResponse;
import openocf.utils.Endpoint;

import java.util.List;
import java.util.LinkedList;

/**
 * brief desc of this file
 *
 * It wraps the OCDoResource parameters, plus ...
 */

/* notes

   FIXME: revision: CoRSP is really just the stimulus info (remote EP
   etc.) and response handler (coReact); the outboundRequest and
   inboundResponse are separate.

   OUTDATED:
   this class encapsulates both the CoSP (i.e. client request) and the SP (i.e. server response).

   for example, the networking properties are one thing on the way
   out, but could be sth else in the response.  e.g. the response to a
   multicast will be a unicast.

   legacy stuff: OCClientResponse contains redundant networking info,
   in that OCDevAddr contains the same info as connType
   (OCConnectivityType).  The source is commented: connType is for
   backward compatibility.

   but note that OCDevAddr is misnamed; it contains not just address
   info, but also info about networking params, which is not really
   address info.

 */

public abstract class CoResourceSP
    implements ICoResourceSP
{
    public boolean isRetain;

    // ctor, initializes TLS vars
    // native private void ctorCoRSP();
    public CoResourceSP() {
	// ctorCoRSP();
    }

    // a CoRSP object may be associated with multiple transactions, so
    // keeping a handle member will not work...
    // private long _handle; // OCDoHandle from OCDoResource

    // Method is set as side-effect of OpenOCF.coExhibit
    // private int _method;
    public int _method;		// for testing

    // public int               getMethod() {return _method;}
    // public CoResourceSP      setMethod(int method) { _method = method; return this;}

    private String _uri;
    public String            getUri() { return _uri; }
    public CoResourceSP      setUri(String uriPath) { _uri = uriPath; return this; }

    // For INBOUND responses (InboundResponse):
   // implemented by user, called by engine
    abstract public void            coReact(InboundResponse response);


    // For OUTBOUND requests (OutboundRequest):
    // FIXME: the following are really part of the message. CoRSP only
    // needs coReact? Put this stuff in OutboundRequest

    // OR: addressing info is CoRSP state; networking info is message
    // state?

    // The protocol is: define a CoSP, use it to create
    // OutboundRequest.

    // networking params for OUTBOUND requests: ////////////////

    // addressing stuff moved to utils.Endpoint

    // // Address: char OCDevAddr.addr[MAX_ADDR_STR_SIZE];
    // // Meaning of addr field depends on nw (adapter) type
    // // address getters return null if type is inappropriate
    // native public String            getIPAddress();
    // native public String            getBLEAddress();
    // // etc. for other supported networks

    // native public int               getPort();		// uint16_t OCDevAddr.port

    // // comm params:
    // //  Transport Security
    // //  Transport protocol (UDP/TCP/etc.);
    // //  Network protocol (IPv4/6)
    // //  Scope (IPv6 only)
    // //  Routing (uni/multi)
    // //  QoS

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
    // native public ICoResourceSP setNetworkIsIP(boolean torf);

    // native public boolean           networkIsIPv4();     // xx_IP_USE_V4 (flags & 0x0010)
    // native public CoResourceSP setNetworkIsIPv4(boolean torf);

    // native public boolean           networkIsIPv6();     // xx_IP_USE_V6 (flags & 0x0010)
    // native public CoResourceSP setNetworkIsIPv6(boolean torf);

    // // IPv6 only:
    // native public boolean           scopeIsInterface();      // flags & 0x000F
    // native public CoResourceSP setScopeIsInterface(boolean torf);      // flags & 0x000F

    // native public boolean           scopeIsLink();           // flags & 0x000F
    // native public CoResourceSP setScopeIsLink(boolean torf);      // flags & 0x000F

    // native public boolean           scopeIsRealm();          // flags & 0x000F
    // native public CoResourceSP setScopeIsRealm(boolean torf);      // flags & 0x000F

    // native public boolean           scopeIsAdmin();
    // native public CoResourceSP setScopeIsAdmin(boolean torf);

    // native public boolean           scopeIsSite();
    // native public CoResourceSP setScopeIsSite(boolean torf);

    // native public boolean           scopeIsOrg();
    // native public CoResourceSP setScopeIsOrg(boolean torf);

    // native public boolean           scopeIsGlobal();
    // native public CoResourceSP setScopeIsGlobal(boolean torf);

    // // Routing
    // native public boolean           routingIsMulticast();
    // native public CoResourceSP setRoutingIsMulticast(boolean torf);

    native public int               getQualityOfService();
    native public CoResourceSP setQualityOfService(int qos);

    ////////////////////////////////////////////////////////////////
    // OCClientResponse data:

    // these data describe the corresponding SP:

    // FIXME: resolve clash between CoSP's descr of SP (which are
    // match patterns) and the SP description received in the
    // response.  The former is not really a description, or rather it
    // is an open description; the latter is a closed description.

    // for example, the CoSP might have type="foo.bar", and the
    // response might include that plus several other types.  We do
    // not want to confuse the two "descriptions".

    // one possible solution: leave the open ("query") description
    // (associated with the CoSP) at the top, and keep the closed
    // descriptions (associated with the SP) with the
    // ObservationRecords.  This will work for some payloads, which
    // contains uri, types[], and interfaces[] (e.g. OCRepPayload,
    // OCResourcePayload, etc.) but not others
    // (e.g. OCSecurityPayload).  Note that OCResource also contains
    // uri, types, and interfaces.


    // endpoints are stored in InboundResponse
    // native public Endpoint    getEndpoint();
    // native public int              getCoResult();

    // native public byte[]           getCoSecurityId();

    // // for observables, https://tools.ietf.org/html/rfc7641
    // native public int              getNotificationSerial();

    ////////////////////////////////////////////////////////////////
    // OCPayload wrapper
    // NB: ObservationRecords must be read-only!
    // native  List<ObservationRecord> observations();
    // private ObservationRecord      _observationRecord;
    // public  ObservationRecord      getObservationRecord() { return _observationRecord; }
    // public  void                   setObservationRecord(ObservationRecord o) { _observationRecord = o; }

    // // OCResource.rsrcChildResourcesHead (for collections)
    // private List<IServiceProvider> _children;
    // public  List<IServiceProvider> getChildren() { return _children; }

    // // SPs only?
    // private List<ActionSet>        _actionSet;
    // public  List<ActionSet>        getActionSet() { return _actionSet; }

    // for discovery and presence requests:
    native public void               deactivate();
}
