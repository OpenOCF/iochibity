//FIXME: just call this Payload?  "ObservationRecord" too obscure.

package openocf.observation;

import openocf.utils.PropertyMap;

import java.util.List;

// NB: are these only for response messages? what about request payloads?
// Payload types in C API:
// PAYLOAD_TYPE_DISCOVERY: OCDiscoveryPayload
//   OCResourcePayload - used inside DiscoveryPayload = coResourceLink datum; compare struct OCResource
//     OCEndpointPayload - used inside a ResourcePayload
// PAYLOAD_TYPE_DEVICE: OCDevicePayload
// PAYLOAD_TYPE_PLATFORM: OCPlatformPayload
// PAYLOAD_TYPE_REPRESENTATION: OCRepPayload - resource representation including properties
// PAYLOAD_TYPE_SECURITY: OCSecurityPayload
// PAYLOAD_TYPE_PRESENCE: OCPresencePayload
// PAYLOAD_TYPE_DIAGNOSTIC: OCDiagnosticPayload
// PAYLOAD_TYPE_INTROSPECTION: OCIntrospectionPayload
// PAYLOAD_TYPE_INVALID

// obsolete as of 1.3.1:  (?)
// OCRDDiscoveryPayload - Resource Directory discovery
// OCRDPayload          - Resource Directory
// OCResourceCollectionPayload - OCRepPayload is used for thise
// OCTagsPayload  - substruct used in OCResourceCollectionPayload
// OCLinksPayload - substruct used in OCResourceCollectionPayload

public interface IObservationRecord
{
    public long getHandle();	// ptr to c struct

    public int getType();	// OCPayloadType

    // ****************************************************************
    // uriPath included in: OCRepPayload, OCResourcePayload,
    // OCLinksPayload (href), OCDiscoveryPayload, OCPlatformPayload
    // NOT included in: OCTagsPayload, OCResourceCollectionPayload,
    // OCRDDiscoveryPayload, OCRDPayload, OCDevicePayload,
    // OCSecurityPayload, OCPresencePayload
    public String getUriPath();
    public void   setUriPath(String theUri);

    // ****************************************************************
    // rtypes included in: OCRepPayload, OCResourcePayload,
    // OCLinksPayload, OCDiscoveryPayload, OCDevicePayload,
    // OCPlatformPayload

    // NOT included in: OCTagsPayload, OCResourceCollectionPayload,
    // OCRDDiscoveryPayload, OCRDPayload, OCSecurityPayload,
    // OCPresencePayload
    public List<String> getResourceTypes();
    // do we need setTypes if we have addType:
    public void setResourceTypes(List<String> rts);
    // public void addResourceType(String rt);

    // ****************************************************************
    // interfaces included in: OCRepPayload, OCResourcePayload,
    // OCLinksPayload, OCDiscoveryPayload, OCDevicePayload,
    // OCPlatformPayload

    // NOT included in: OCTagsPayload, OCResourceCollectionPayload,
    // OCRDDiscoveryPayload, OCRDPayload, OCSecurityPayload,
    // OCPresencePayload
    public List<String> getInterfaces();
    public void setInterfaces(List<String> ifs);
    // public void addInterface(String iface);

    // ****************************************************************
    // properties included in: OCRepPayload
    // indirectly included in: OCDevicePayload, OCPlatformPayload,
    // etc. - we create iotivity-specific properties
    public PropertyMap<String, Object> getProperties();
    // protected void setProperties(PropertyForResource ps);
    // public void putProperty(String name, Object value) { _properties.put(name, value); }

    // ****************************************************************
    // policies (DISCOVERABLE, OBSERVABLE, etc.)
    public long getPolicies();

    // ****************************************************************
    public List<IObservationRecord> getChildren();
}
