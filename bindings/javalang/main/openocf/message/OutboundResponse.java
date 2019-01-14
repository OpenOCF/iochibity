package openocf.message;

import java.util.List;

import openocf.observation.ObservationRecords;
import openocf.utils.CoAPOption;
import openocf.utils.Endpoint;

// wraps OCEntityHandlerResponse which contains handle of
//    corresponding OCServerRequest (that is, the request record
//    (InboundRequest) from the client)

// NOTES: This struct is for creating the responses - it's fields are
// populated by the service routine. The incoming request data is
// encoded in the requestHandle of the OCEntityHandlerRequest, which
// is OCServerRequest*.

// typedef struct
// {
//     OCRequestHandle requestHandle;  (to be set to OCEntityHandlerRequest.requestHandle)
//     /** Resource handle. (@deprecated: This parameter is not used.) */
//     OCResourceHandle resourceHandle;
//     OCEntityHandlerResult  ehResult;
//     OCPayload* payload;
//     uint8_t numSendVendorSpecificHeaderOptions;
//     OCHeaderOption sendVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];
//     char resourceUri[MAX_URI_LENGTH];
//     uint8_t persistentBufferFlag;
// } OCEntityHandlerResponse;

// NOTE: OCDevAddr etc. is in incoming OCEntityHandlerRequest

// OutboundResponse objects are created by server's service routine (react)
// will be passed to OpenOCFServer.exhibit (which calls OCDoResponse)
public class OutboundResponse extends OutboundMessage
{
    // _handle is inherited from messageRecord
    // in this case, a ptr to OCEntityHandlerResponse (this is not requestHandle)

    public  InboundRequest _requestIn;

    // CTORS
    public OutboundResponse(InboundRequest rqst)
    {
    	System.out.println("OutboundResponse CTOR 2");
    	_requestIn = rqst;
    }

    // private native long createOutboundResponse(InboundRequest r, ObservationList<Observation> pll);
    // NB: observation = payload
    public OutboundResponse(InboundRequest rqst, ObservationRecords observations)
    {
    	System.out.println("OutboundResponse CTOR 2");
    	_requestIn = rqst;
    	_observations = observations;
    }

    // OCEntityHandlerResponse response = { .requestHandle = NULL };

    // typedef struct  /* OCEntityHandlerResponse */
    // {

    //     OCRequestHandle requestHandle; set this to
    //     entityHandlerRequest->requestHandle, which is a ptr to
    //     OCServerRequest?

    //     OCResourceHandle resourceHandle; deprecated; correspondes to OCEntityHandlerRequest.resource?

    private Endpoint _ep;
    public Endpoint getEndpoint() { return _ep; }

    //     OCEntityHandlerResult  ehResult;  set by service routine (RSP.react)
    private int _serviceResult;
    public  int getServiceResult() { return _serviceResult; }
    public  OutboundResponse setServiceResult(int result) { _serviceResult = result; return this;}

    //     OCPayload *payload;   outgoing payload, created by service routine
    //    private Payload _payload;
    private ObservationRecords _observations;

    public ObservationRecords getObservationRecords() { return _observations; }
    public OutboundResponse setObservationRecords(ObservationRecords observations) {
	_observations = observations;
	return this;
    }



    // uint8_t numSendVendorSpecificHeaderOptions;
    // OCHeaderOption sendVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];
    private List<CoAPOption> _options;

    //     /** URI of new resource that entity handler might create.*/
    //     char resourceUri[MAX_URI_LENGTH];
    private String _newResourceUri;
    public  String getNewResourceUri() { return _newResourceUri; }
    public  OutboundResponse setNewResourceUri(String uri) { _newResourceUri = uri; return this;}

    //     /** Server sets to true for persistent response buffer,false for non-persistent response buffer*/
    //     uint8_t persistentBufferFlag;
    public int persistentBufferFlag;
    // } OCEntityHandlerResponse;
}
