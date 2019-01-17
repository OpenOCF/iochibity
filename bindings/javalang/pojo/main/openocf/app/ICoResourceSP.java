package openocf.app;

import openocf.message.InboundResponse;
import openocf.Endpoint;

// client
// All methods EXCEPT observemessage implemented by CoResourceSP abstract class
public interface ICoResourceSP {

    public void               coReact(InboundResponse response);   // abstract, to be implemented (but never called) by user

    // the remaining methods are implemented (natively)by CoResourceSP
    // OCDoResource params
    public String             getUri();
    public ICoResourceSP      setUri(String theUri);

    // Method operations moved to messageRecord (i.e. messages)
    // public int                getMethod();
    // public ICoResourceSP      setMethod(int m);

    //////////////// NETWORKING params ////////////////
    // moved to Endpoint

    // public String            getIPAddress();
    // public String            getBLEAddress();
    // public int               getPort();

    // // Raw
    // public int                getNetworkAdapter();
    // public int                getNetworkFlags();

    // // networkFlags, broken out
    // public boolean            isTransportSecure();
    // public ICoResourceSP      setTransportSecure(boolean torf);

    // // Transport Protocol flags: mutually exclusive; setting one resets all the others.
    // public boolean            isTransportUDP();
    // public ICoResourceSP      setTransportUDP(boolean torf);

    // public boolean            isTransportTCP();
    // public ICoResourceSP      setTransportTCP(boolean torf);

    // public boolean            isTransportGATT();
    // public ICoResourceSP      setTransportGATT(boolean torf);

    // public boolean            isTransportRFCOMM();
    // public ICoResourceSP      setTransportRFCOMM(boolean torf);

    // public boolean            isTransportNFC();
    // public ICoResourceSP      setTransportNFC(boolean torf);

    // // IP Protocol flags: only needed to select version of IP protocol
    // public boolean            networkIsIP(); // == transportIsUDP
    // public ICoResourceSP setNetworkIsIP(boolean torf);

    // public boolean            networkIsIPv4();
    // public ICoResourceSP setNetworkIsIPv4(boolean torf);

    // public boolean            networkIsIPv6();
    // public ICoResourceSP setNetworkIsIPv6(boolean torf);

    // // IPv6 only:
    // public boolean            scopeIsInterface();
    // public ICoResourceSP setScopeIsInterface(boolean torf);

    // public boolean            scopeIsLink();
    // public ICoResourceSP setScopeIsLink(boolean torf);

    // public boolean            scopeIsRealm();
    // public ICoResourceSP setScopeIsRealm(boolean torf);

    // public boolean            scopeIsAdmin();
    // public ICoResourceSP setScopeIsAdmin(boolean torf);

    // public boolean            scopeIsSite();
    // public ICoResourceSP setScopeIsSite(boolean torf);

    // public boolean            scopeIsOrg();
    // public ICoResourceSP setScopeIsOrg(boolean torf);

    // public boolean            scopeIsGlobal();
    // public ICoResourceSP setScopeIsGlobal(boolean torf);

    // // Routing
    // public boolean            routingIsMulticast();
    // public ICoResourceSP setRoutingIsMulticast(boolean torf);

    // public int                getQualityOfService();
    // public ICoResourceSP setQualityOfService(int qos);

    // to add: fields from struct OCClientResponse? they will be
    // updated on receipt of ObservationRecordIn
    // public Endpoint      coAddress(); // native

    // what about struct ClientCB?

    // for discovery and presence requests:
    public void               deactivate();
}
