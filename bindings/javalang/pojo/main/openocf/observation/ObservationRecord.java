package openocf.observation;

import java.util.List;

import openocf.utils.PropertyMap;

// Observation corresponsds to OCPayload.
// An OCPayload struct conveys an observation on the ServiceProvider
// qua black box.  So that's what we call it.

public class ObservationRecord implements IObservationRecord
{
    // OCPayloadType enum
    public static final int INVALID        = 0;
    public static final int DISCOVERY      = 1;
    public static final int DEVICE         = 2;
    public static final int PLATFORM       = 3;
    public static final int REPRESENTATION = 4;
    public static final int SECURITY       = 5;
    public static final int PRESENCE       = 6;
    public static final int RD             = 7;
    public static final int NOTIFICATION   = 8;

    // private InboundRequest _stimulusIn;
    // ObservationRecord(InboundRequest observation)
    // {
    // 	_stimulusIn = observation;
    // }


    private long _handle;      // OCPayload*
    public  long getHandle() { return _handle; }

    // FIXME: make type private with getter
    private int _type;	// OCPayloadType
    public  int getType() { return _type; }

    // ****************************************************************
    // uriPath included in: OCRepPayload, OCResourcePayload,
    // OCLinksPayload (href), OCDiscoveryPayload, OCPlatformPayload
    // NOT included in: OCTagsPayload, OCResourceCollectionPayload,
    // OCRDDiscoveryPayload, OCRDPayload, OCDevicePayload,
    // OCSecurityPayload, OCPresencePayload
    private String _uriPath;
    public  String getUriPath() { return _uriPath; }
    public  void   setUriPath(String theUri) { _uriPath = theUri; }

    // ****************************************************************
    // rtypes included in: OCRepPayload, OCResourcePayload,
    // OCLinksPayload, OCDiscoveryPayload, OCDevicePayload,
    // OCPlatformPayload

    // NOT included in: OCTagsPayload, OCResourceCollectionPayload,
    // OCRDDiscoveryPayload, OCRDPayload, OCSecurityPayload,
    // OCPresencePayload
    private List<String> _rtypes = null;  // new LinkedList<String>();
    public  List<String> getResourceTypes() { return _rtypes; }
    public  void setResourceTypes(List<String> rts) { _rtypes = rts; }
    // do we need setTypes if we have addType?
    public void addResourceType(String rt) { _rtypes.add(rt); }
    // to add/rem a type: get the list, add/remove, then setTypes

    // ****************************************************************
    // interfaces included in: OCRepPayload, OCResourcePayload,
    // OCLinksPayload, OCDiscoveryPayload, OCDevicePayload,
    // OCPlatformPayload

    // NOT included in: OCTagsPayload, OCResourceCollectionPayload,
    // OCRDDiscoveryPayload, OCRDPayload, OCSecurityPayload,
    // OCPresencePayload
    private List<String> _interfaces = null;
    public  List<String> getInterfaces() { return _interfaces; }
    public  void setInterfaces(List<String> ifs) { _interfaces = ifs; }
    public  void addInterface(String iface) { _interfaces.add(iface); }

    // ****************************************************************
    // properties included in: OCRepPayload
    // indirectly included in: OCDevicePayload, OCPlatformPayload,
    // etc. - we create iotivity-specific properties
    private PropertyMap<String, Object> _properties = null;
    public  PropertyMap<String, Object> getProperties() { return _properties; }
    // protected native void setProperties(PropertyForResource ps);
    // public void putProperty(String name, Object value) { _properties.put(name, value); }

    // ****************************************************************

    // policies (DISCOVERABLE, OBSERVABLE, etc.)

    // CAVEAT: these are resource "policy properties", not network
    // policies; they are encoded in OCResourceProperty.  they come
    // from
    // OCEntityHandlerRequest.OCResourceHandle->resourceProperties and
    // OCEntityHandlerResponse.OCResourceHandle->resourceProperties
    private long _policies;
    public  long getPolicies() { return _policies; }

    // ****************************************************************
    private List<IObservationRecord> _children;
    public  List<IObservationRecord> getChildren() { return _children; }
}
