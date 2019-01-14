package openocf.message;

import openocf.Message;

import java.util.ArrayList;
import java.util.LinkedList;

// OCFServerService observes InboundRequest
// dispatches it to ResourceServiceProvider.coReact
// ResourceServiceProvider.coReact constructs OutboundResponse,
// tells OCFServerService to exhibit OutboundResponse
//
// InboundRequest observed by OCFServerService matches OutboundRequest exhibited by OCFClientService

// OutboundResponse exhibited by OCFServerService matches InboundResponse observed by OCFClientService

// All four are messages associated with an OCFService

// OutboundRequest: wraps OCDoResource (client, i.e. send request)
// OutboundResponse: wraps OCEntityHandlerResponse* (server, i.e. send response)

// base for OutboundRequest (client) and OutboundResponse (server)

    // data necessary to support OCDoResource (client's OutboundRequest):
    // OCStackResult OCDoResource(OCDoHandle *handle,
    //                             OCMethod method,
    //                             const char *requestUri,
    //                             const OCDevAddr *destination,
    // Exhibitedmessage:          OCPayload* payload,
    //                             OCConnectivityType connectivityType,
    //                             OCQualityOfService qos,
    //                             OCCallbackData *cbData,
    //                             OCHeaderOption *options,
    //                             uint8_t numOptions)

// NB: intuitively the payload is the exhibitable message; the rest
// is metadata about the message/transport. The latter could be
// thought of as channel params.

// So when app calls OCFClientSP.exhibit(OutboundRequest so) it instructs
// the engine to exhibit the payload in the channel (mode?) specified
// by the metadata. That's a messaging metaphor, but it fits mere
// exhibition of a message as well - Pavlov's stimulus was conveyed
// over a channel as well - air at a certain density, temp, etc. If
// not "channel" at least medium. IOW you cannot "exhibit" a message
// without a medium of exhibition.


// client's OutboundRequest needs address info (OCDevAddr)
// server's OutboundReponse does not

abstract public class OutboundMessage extends Message
{
    // Request remote handle - request handle at ORIGIN (i.e. client)? */

    private long                     _remoteRequestHandle; // OCRequestHandle requestHandle = OCRequest*
    // public  long                    getRemoteRequestHandle() { return _remoteHandle; }

    private long                    _resourceHandle; // OCResource*
    // public  long                    getResourceHandle() { return _resourceHandle; }

    // OutboundResponse does not need method?
    // protected int _method;
    // abstract public int getMethod(); // { return _method; }
    // abstract public Outbound setMethod(int method); // { _method = method; return this; }

    // private _uri, abstract getUri inherited from messageRecord
    // for outbound messages, these can be implemented in java; for inbound, in native code
    public  String      getUri() { return _uri; }
    public  OutboundMessage setUri(String uri) { _uri = uri; return this; }

    // common to OCDoResource and OCEntityHandlerResponse:
    // OCPayload *payload

    // public  native IServiceProvider getServiceProvider();   // getResource();

    //     /** Pointer of ActionSet which to support group action.*/
    //     OCActionSet *actionsetHead;

    //     /** The instance identifier for this web link in an array of web links - used in links. */
    //     union
    //     {
    //         /** An ordinal number that is not repeated - must be unique in the collection context. */
    //         uint8_t ins;
    //         /** Any unique string including a URI. */
    //         char *uniqueStr;
    //         /** Use UUID for universal uniqueness - used in /oic/res to identify the device. */
    //         OCIdentity uniqueUUID;
    //     };
    // } OCResource;
}
