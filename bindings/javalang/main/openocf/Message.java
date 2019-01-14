package openocf;

import java.util.List;

import openocf.observation.ObservationRecords;
import openocf.utils.CoAPOption;
import openocf.utils.CoAPOption;
import openocf.utils.Endpoint;

// was: Message.java

// messages are exhibited by routines:

// server:  OCStackResult OC_CALL OCDoResponse(OCEntityHandlerResponse *ehResponse)
// OCEntityHandlerResponse:
//    OCRequestHandle requestHandle; ->  OCServerRequest* - contains method, URL, endpoint, etc.
//    OCResourceHandle resourceHandle;
//    OCEntityHandlerResult  ehResult;
//    OCPayload* payload;
//    uint8_t numSendVendorSpecificHeaderOptions;
//    OCHeaderOption sendVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];
//    char resourceUri[MAX_URI_LENGTH];
//    uint8_t persistentBufferFlag;

// client:  OCStackResult OC_CALL OCDoRequest(
//    OCDoHandle *handle,  - Txn ID
//    OCMethod method,
//    const char *requestUri,
//    const OCDevAddr *destination,
//    OCPayload* payload,
//    OCConnectivityType connectivityType,
//    OCQualityOfService qos,
//    OCCallbackData *cbData,
//    OCHeaderOption *options,
//    uint8_t numOptions)

// messages are observed by routines:

//  server: OCEntityHandler (OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest, void* callbackParam)
//  OCEntityHandlerRequest:
//    OCResourceHandle resource;
//    OCRequestHandle requestHandle; - contains URL, etc.
//    OCMethod method;
//    OCDevAddr devAddr;
//    char * query;
//    OCObservationInfo obsInfo;
//    uint8_t numRcvdVendorSpecificHeaderOptions;
//    OCHeaderOption * rcvdVendorSpecificHeaderOptions;
//    uint16_t messageID;
//    OCPayload *payload;

//  client: OCClientResponseHandler (void *context, OCDoHandle handle, OCClientResponse * clientResponse)
//  OCClientResponse:
//    OCDevAddr devAddr;
//    OCDevAddr *addr;
//    OCConnectivityType connType; /* corresponds to OCTransportAdapter, OCTransportFlags: adapter type, sec flag, IP ver., IPv6 scope */
//    OCIdentity identity;	/* GAR: not used for discovery responses? */
//    OCStackResult result;
//    uint32_t sequenceNumber;
//    const char * resourceUri;
//    OCPayload *payload;
//    uint8_t numRcvdVendorSpecificHeaderOptions;
//    OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];

// what they have in common: OCDevAddr (remote Endpoint); handle
// (OCDoHandle, entityHandlerRequest.requestHandle; vendor-specific
// header options; payload

// i.e. base Message (OCEntityHandlerRequest, OCClientResponse)
public abstract class Message implements IMessage
{
    protected long _handle;	// pointer to underlying c struct

    // Method not needed for e.g. OutboundResponse
    // abstract int getMethod();

    // private Endpoint _remoteEP;
    // private long _endpoint_handle;	  // c ptr to EP
    abstract public Endpoint getEndpoint(); // { return _remoteEP; } // IMessage

    // clientresponse:
    // uint8_t numRcvdVendorSpecificHeaderOptions;
    // OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];
    // server (inbound stimulus):
    // uint8_t numRcvdVendorSpecificHeaderOptions;
    // OCHeaderOption * rcvdVendorSpecificHeaderOptions;

    // local handle for c struct underlying this object: OCClientResponse, etc.
    // private long _message;	// was: _localHandle


    // FIXME: uri should go in SP rather than message?
    // OCEntityHandlerRequest.resource.uri
    // OCClientResponse.resourceUri
    // OCDiscoveryPayload.uri, OCRepPayload.uri, OCPlatformPayload.uri,
    protected String _uri;
    abstract public  String getUri(); // { return _uri; }
    // setUri is only for Exhibitables (Outbound messages)

    // network stuff? OCClientResponse.connType, OCEntityHandlerRequest.OCDevAddr.adapter/.flags

    // Payload. Every message potentially has a payload, so it belongs here (abstractly).
    // A payload is a resource (state) observation record.

    //FIXME: just call it Payload instead of Observation?
    // public native PayloadList<Payload> getPayloadList(); // IMessage
    // private long _observationRecordHandle;
    // public  long getObservationRecordHandle() { return _observationRecordHandle; } // IMessage
    // abstract public long getObservationRecordHandle();
    native public ObservationRecords getObservationRecords(); // IMessage

    // public native int getObservationType(); // IMessage

    // Number of vendor specific header options sent or recd.
    // InboundRequest:   uint8_t numRcvdVendorSpecificHeaderOptions;
    //              OCHeaderOption * rcvdVendorSpecificHeaderOptions;
    // ObservationOut: uint8_t numSendVendorSpecificHeaderOptions;
    //              OCHeaderOption sendVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];
    // public  int  optionCount;
    // private long ptr_Options;	// OCHeaderOption*


    private List<CoAPOption> _options;
    public native List<CoAPOption> getOptions(); // IMessage
}
