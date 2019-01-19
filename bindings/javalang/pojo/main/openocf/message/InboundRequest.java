package openocf.message;

import java.util.List;

import openocf.app.IResourceSP;
import openocf.observation.ObservationRecords;
import openocf.message.CoAPOption;
import openocf.Endpoint;

/**
 * Wrapper on args to:
 *
 * typedef OCEntityHandlerResult (*OCEntityHandler)
 *                (OCEntityHandlerFlag flag,
 *                 OCEntityHandlerRequest * entityHandlerRequest,
 *                 void* callbackParam);
 *
 * InboundRequest is passed to application-defined react routine
 */

// NB: client request arrives at server as OCServerRequest

public class InboundRequest extends InboundMessage
{
    // For internal (JNI) use only:
    private IResourceSP resourceSP; // JNI only

    // OCEntityHandlerFlag: enum OC_REQUEST_FLAG || OC_OBSERVE_FLAG
    // public int watchAction;  // REGISTER, DEREGISTER, NO_OPTION, MQ_SUBSCRIBER, MQ_UNSUBSCRIBER
    // public int watchId;
    // public boolean isWatch = false; //  false => OC_REQUEST_FLAG
    native public boolean isWatch(); // true -> OC_OBSERVE_FLAG

    // OCEntityHandlerRequest:
    // {
    // Handles for for internal (JNI) use only:
    // OCResourceHandle resource; => resourceHandle
    native public long getResourceHandle(); // void *

    // OCRequestHandle requestHandle - i.e. handle of request at ORIGIN (i.e. client)?;
    native public long getRequestHandle(); // void *

    // OCMethod method; // the REST method retrieved from received request PDU.
    // private int _method;			// for OCEntityHandlerRequest, not OCClientResponse
    // public int getMethod() { return _method; }; // IMessage
    native public int getMethod();

    // description of endpoint that sent the request.
    // OCDevAddr devAddr;
    // EXPOSED IN Observable.java
    native public Endpoint getEndpoint();

    // resource query sent by client.
    // char * query;
    // private String _query;
    // public String getQueryString() { return _query; }
    native public String getQueryString();

    // Information associated with observation - valid only when OCEntityHandler flag includes
    // * ::OC_OBSERVE_FLAG.
    // (i.e. the flag passed to the resource provider service routine)
    // NOTE: - on the client side this is encoded as a header option
    //       - on server side, the header option is converted to the OCEntityHandler flag
    //         and the OCObservationInfo struct is filled out
    // NOTE: this is only for used register/deregister of watchers (observers)
    // typedef struct
    // {
    // Action associated with observation request.*/
    // OCObserveAction action;  /* enum */
    // Identifier for observation being registered/deregistered.*/
    // OCObservationId obsId;   /* uint8_t */
    // } OCObservationInfo;
    // NOTE: C API uses "observe"; we use "watch"
    // OCObservationInfo obsInfo;
    // NOTE: we encapsulate all this in methods:
    //     Method.WATCH = GET, plus OC_OBSERVE_FLAG, + REGISTER (what about obsId?)
    //     Method.UNWATCH = GET, plus OC_OBSERVE_FLAG, + DEREGISTER
    // and we ignore MQTT for now

    // the payload from the request PDU.
    // OCPayload *payload;
    native public ObservationRecords getObservationRecords();

    // uint8_t numRcvdVendorSpecificHeaderOptions;
    // OCHeaderOption * rcvdVendorSpecificHeaderOptions;
    native public List<CoAPOption> getCoAPOptions();

    // Message id.
    // uint16_t messageID;
    private int _messageId;
    public  int getMessageId() { return _messageId; }

    // } OCEntityHandlerRequest;

}
