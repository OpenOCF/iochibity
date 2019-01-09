/* oocf_host_server */

#include "oocf_host_server.h"

/* Inbound Request Handling
 *
 * CoAP msg data are passed up the stack and converted to various
 * structs, from CAInfo_t to OCServerProtocolRequest to
 * OCServerRequest.
 *
 * HandleCARequests
 * => OCHandleRequests (convert struct CARequestInfo to OCServerProtocolRequest)
 * ==> HandleStackRequests (convert OCServerProtocolRequest to OCServerRequest)
 * ===> ProcessRequest
 * ====> handler, depending on resource type.
 *         for user-defined, convert OCServerRequest to OCEntityHandlerRequest and invoke user CB
 *
 * TODO: eliminate intermediate structs.
 */

static RB_HEAD(_oocf_request_cache_tree, CARequestInfo) g_request_cache_tree =
                                                            RB_INITIALIZER(&g_request_cache_tree);

static RBL_GENERATE(_oocf_request_cache_tree, CARequestInfo, entry, RBRequestInfoEntryCmp);

LOCAL int RBRequestInfoEntryCmp(CARequestInfo *target, CARequestInfo *treeNode)
{
    return memcmp(target->info.token, treeNode->info.token, target->info.tokenLength);
}

struct CARequestInfo *_oocf_request_cache_put(CARequestInfo *request)
{
    RBL_INSERT(_oocf_request_cache_tree, &g_request_cache_tree, request);
    return (struct CARequestInfo*) request;
}

//void DeleteServerRequest(OCServerRequest * serverRequest)
void _oocf_request_cache_delete(CARequestInfo *request)
{
    if (request)
    {
        if (!RBL_FIND(_oocf_request_cache_tree, &g_request_cache_tree, request))
        {
            return;
        }

        RBL_REMOVE(_oocf_request_cache_tree, &g_request_cache_tree, request);
        /* OICFree(request->info.resourceUri); */
        /* OICFree(request->info.token); */
        /* OICFree(request); */
        /* request = NULL; */
        OIC_LOG(INFO, TAG, "Request removed from cache");
    }
}

//OCServerRequest * GetServerRequestUsingToken (const uint8_t *token, uint8_t tokenLength)
struct CARequestInfo * GetServerRequestUsingToken (const uint8_t *token, uint8_t tokenLength)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    if (!token)
    {
        OIC_LOG(ERROR, TAG, "Invalid Parameter Token");
        return NULL;
    }

    OIC_LOG(INFO, TAG,"Get server request with token");
    OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)token, tokenLength);

    //OCServerRequest tmpFind, *out = NULL;
    struct CARequestInfo tmpFind, *out = NULL;

    tmpFind.info.token = (uint8_t*)token;
    tmpFind.info.tokenLength = tokenLength;
    //out = RB_FIND(ServerRequestTree, &g_serverRequestTree, &tmpFind);
    out = RB_FIND(_oocf_request_cache_tree, &g_request_cache_tree, &tmpFind);

    if (!out)
    {
        OIC_LOG(INFO, TAG, "Server Request not found!!");
        return NULL;
    }

    OIC_LOG(INFO, TAG, "Found in server request list");
    return out;
}

/**
 * This structure will be created in occoap and passed up the stack on the server side.
 * A CoAP msg.
 */
/* FIXME: not needed? purpose is to use OC names instead of CA names */
// typedef/*  struct OCServerProtocolRequest /\* FIXME: @rename dgram? *\/ */
/* { */
/*     /\** Observe option field.*\/ */
/*     uint32_t observationOption; */

/*     /\** The REST method retrieved from received request PDU.*\/ */
/*     OCMethod method;            /\* from inbound requestInfo->method *\/ */

/*     /\** the provided payload format. *\/ */
/*     OCPayloadFormat payloadFormat; /\* in CAInfo_t (struct CARequestInfo.info) *\/ */

/*     /\** the requested payload format. *\/ */
/*     OCPayloadFormat acceptFormat; /\* in CAInfo_t (struct CARequestInfo.info) *\/ */

/*     /\** the requested payload version. *\/ */
/*     uint16_t acceptVersion;   /\* in CAInfo_t (struct CARequestInfo.info) *\/ */

/*     /\** extracted from resourceUri of inboud PDU (CAInfo_t (struct CARequestInfo.info) *\/ */
/*     char resourceUrl[MAX_URI_LENGTH]; /\* path without query string *\/ */

/*     /\** resource query send by client.*\/ */
/*     char query[MAX_QUERY_LENGTH]; /\* extracted from resourceUri of inbound pdu *\/ */

/*     /\** reqJSON is retrieved from the payload of the received request PDU.*\/ */
/*     uint8_t *payload;   /\* in CAInfo_t (struct CARequestInfo.info) *\/ */

/*     /\** qos is indicating if the request is CON or NON.*\/ */
/*     OCQualityOfService qos;     /\* FIXME: @rename coap_msg_type (NON, CON, ACK, RESET) *\/ */

/*     /\** Number of the received vendor specific header options.*\/ */
/*     uint8_t numRcvdVendorSpecificHeaderOptions; */

/*     /\** Array of received vendor specific header option .*\/ */
/*     OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS]; */

/*     /\** Remote end-point address **\/ */
/*     OCDevAddr devAddr;          /\* ADDED to CARequestInfo by OCHandleRequests;  FIXME: @rename origin_ep *\/ */
/*     // CAEndpoint origin_ep; */

/*     /\** Token for the observe request.*\/ */
/*     char *requestToken; */

/*     /\** token length.*\/ */
/*     uint8_t tokenLength; */

/*     /\** The ID of CoAP PDU.*\/ */
/*     uint16_t coapID; */

/*     /\** For delayed Response.*\/ */
/*     uint8_t delayedResNeeded; */

/*     /\** For More packet.*\/ */
/*     uint8_t reqMorePacket;      /\* pdu is not a complete msg? *\/ */

/*     /\** The number of requested packet.*\/ */
/*     uint32_t reqPacketNum;      /\* @UNUSED *\/ */

/*     /\** The size of requested packet.*\/ */
/*     uint16_t reqPacketSize;     /\* @UNUSED *\/ */

/*     /\** The number of responded packet.*\/ */
/*     uint32_t resPacketNum;      /\* @UNUSED *\/ */

/*     /\** Responded packet size.*\/ */
/*     uint16_t resPacketSize;     /\* @UNUSED *\/ */

/*     /\** The total size of requested packet.*\/ */
/*     size_t reqTotalSize; */
/* } OCServerProtocolRequest; /\* src: ocstackinternal.h *\/ */

/**
 * following structure will be created in occoap and passed up the stack on the server side.
 */
// FIXME: rename _oocf_inbound_request
/* typedef struct OCServerRequest  /\* FIXME: essentially same as OCServerProtocolRequest *\/ */
/* { */
/*     OCMethod method;            /\* get_method(_oocf_coap_msg_unpacked) *\/ */

/*     /\** Accept format retrieved from the received request PDU. *\/ */
/*     OCPayloadFormat acceptFormat; */

/*     /\** Accept version retrieved from the received request PDU. *\/ */
/*     uint16_t acceptVersion; */

/*     /\** resourceUrl will be filled in occoap using the path options in received request PDU.*\/ */
/*     char resourceUrl[MAX_URI_LENGTH]; /\* url path without query string *\/ */

/*     /\** resource query send by client.*\/ */
/*     char query[MAX_QUERY_LENGTH]; */

/*     /\** qos is indicating if the request is CON or NON.*\/ */
/*     OCQualityOfService qos; */

/*     /\** Observe option field.*\/ */

/*     uint32_t observationOption; */

/*     /\** Observe Result field.*\/ */
/*     OCStackResult observeResult; */

/*     /\** number of Responses.*\/ */
/*     uint8_t numResponses; */

/*     /\** Response Entity Handler .*\/ */
/*     OCEHResponseHandler ehResponseHandler; */

/*     /\** Remote endpoint address **\/ */
/*     OCDevAddr devAddr; */
/*     // CAEndpoint_t origin_ep; */

/*     /\** The ID of server request*\/ */
/*     uint32_t requestId; */

/*     /\** Token for the request.*\/ */
/*     char *requestToken; */

/*     /\** token length the request.*\/ */
/*     uint8_t tokenLength; */

/*     /\** The ID of CoAP pdu (Kept in CoAp).*\/ */
/*     uint16_t coapID; */

/*     /\** For Delayed Response.*\/ */
/*     uint8_t delayedResNeeded; */

/*     /\** Number of vendor specific header options.*\/ */
/*     uint8_t numRcvdVendorSpecificHeaderOptions; */

/*     /\** An Array  of received vendor specific header options.*\/ */
/*     OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS]; */

/*     /\** Request to complete.*\/ */
/*     uint8_t requestComplete; */

/*     /\** Node entry in red-black tree of linked lists.*\/ */
/*     RBL_ENTRY(OCServerRequest) entry; */

/*     /\** Flag indicating slow response.*\/ */
/*     uint8_t slowFlag; */

/*     /\** Flag indicating notification.*\/ */
/*     uint8_t notificationFlag; */

/*     /\** Payload format retrieved from the received request PDU. *\/ */
/*     OCPayloadFormat payloadFormat; */

/*     /\** Payload Size.*\/ */
/*     size_t payloadSize; */

/*     /\** payload is retrieved from the payload of the received request PDU.*\/ */
/*     uint8_t payload[1]; */

/*     // WARNING: Do NOT add attributes after payload as they get overwritten */
/*     // when payload content gets copied over! */

/* } OCServerRequest; */

/**
 * Incoming requests handled by the server. Requests are passed in as a parameter to the
 * OCEntityHandler callback API.
 * The OCEntityHandler callback API must be implemented in the application in order
 * to receive these requests.
 */
#if EXPORT_INTERFACE
#include <stdint.h>
struct oocf_inbound_request     /* @was OCEntityHandlerRequest (typedef) */
{
    OCResourceHandle   resource; /* ptr to struct OCResource  FIXME: name resourceHandle*/
    OCRequestHandle    requestHandle; /* ptr to struct CARequestInfo */
    //OCMethod           method;  /* REST method, inbound request PDU.*/
    struct oocf_endpoint  origin_ep;  //OCDevAddr          devAddr; /* origin_ep */
    //char              *query;   /* derived from *requestHandle->info.resourceUri */
    OCObservationInfo  obsInfo; /* valid only when OCEntityHandler flag includes ::OC_OBSERVE_FLAG.*/
    /* uint8_t            numRcvdVendorSpecificHeaderOptions; /\* CoAP Options *\/ */
    /* OCHeaderOption    *rcvdVendorSpecificHeaderOptions; */
    /* uint16_t           messageID; */
    /* OCPayload         *payload; */
};
typedef struct oocf_inbound_request OCEntityHandlerRequest;

/**
 * Request handle is passed to server via the entity handler for each incoming request.
 * Stack assigns when request is received, server sets to indicate what request response is for.
 */
struct oocf_outbound_response
{
    /** Request handle.*/
    OCRequestHandle requestHandle;

    /** Resource handle. (@deprecated: This parameter is not used.) */
    OCResourceHandle resourceHandle;

    /** Allow the entity handler to pass a result with the response.*/
    OCEntityHandlerResult  ehResult;

    /** This is the pointer to server payload data to be transferred.*/
    OCPayload* payload;

    /** number of the vendor specific header options .*/
    uint8_t numSendVendorSpecificHeaderOptions;

    /** An array of the vendor specific header options the entity handler wishes to use in response.*/
    OCHeaderOption sendVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];

    /** Resource path of new resource that entity handler might create.*/
    char resourceUri[MAX_URI_LENGTH];

    /** Server sets to true for persistent response buffer,false for non-persistent response buffer*/
    uint8_t persistentBufferFlag;
}; // OCEntityHandlerResponse;
typedef struct oocf_outbound_response OCEntityHandlerResponse;

/**
 * Possible returned values from entity handler.
 */
typedef enum
{
    OC_EH_OK = 0,
    OC_EH_ERROR,
    OC_EH_SLOW,
    OC_EH_RESOURCE_CREATED = 201,
    OC_EH_RESOURCE_DELETED = 202,
    OC_EH_VALID = 203,
    OC_EH_CHANGED = 204,
    OC_EH_CONTENT = 205,
    OC_EH_BAD_REQ = 400,
    OC_EH_UNAUTHORIZED_REQ = 401,
    OC_EH_BAD_OPT = 402,
    OC_EH_FORBIDDEN = 403,
    OC_EH_RESOURCE_NOT_FOUND = 404,
    OC_EH_METHOD_NOT_ALLOWED = 405,
    OC_EH_NOT_ACCEPTABLE = 406,
    OC_EH_TOO_LARGE = 413,
    OC_EH_UNSUPPORTED_MEDIA_TYPE = 415,
    OC_EH_INTERNAL_SERVER_ERROR = 500,
    OC_EH_BAD_GATEWAY = 502,
    OC_EH_SERVICE_UNAVAILABLE = 503,
    OC_EH_RETRANSMIT_TIMEOUT = 504
} OCEntityHandlerResult;        // FIXME: rename COAP_RESPONSE_CODE

#endif	/* INTERFACE */

/**
 * Entity's state
 */
#if EXPORT_INTERFACE
typedef enum
{
    /** Request state.*/
    OC_REQUEST_FLAG = (1 << 1),
    /** Observe state.*/
    OC_OBSERVE_FLAG = (1 << 2)
} OCEntityHandlerFlag;

/**
 * Device Entity handler need to use this call back instead of OCEntityHandler.
 *
 * When you set specific return value like OC_EH_CHANGED, OC_EH_CONTENT,
 * OC_EH_SLOW and etc in entity handler callback,
 * ocstack will be not send response automatically to client
 * except for error return value like OC_EH_ERROR.
 *
 * If you want to send response to client with specific result,
 * OCDoResponse API should be called with the result value.
 *
 * e.g)
 *
 * OCEntityHandlerResponse response;
 *
 * ..
 *
 * response.ehResult = OC_EH_CHANGED;
 *
 * ..
 *
 * OCDoResponse(&response)
 *
 * ..
 *
 * return OC_EH_OK;
 */
typedef OCEntityHandlerResult (*OCDeviceEntityHandler)
(OCEntityHandlerFlag flag, struct oocf_inbound_request *entityHandlerRequest, char* uri, void* callbackParam);
// (OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest, char* uri, void* callbackParam);

/**
 * The signature of the internal call back functions to handle responses from entity handler
 */
typedef OCStackResult (* OCEHResponseHandler)(OCEntityHandlerResponse * ehResponse);

#endif	/* EXPORT_INTERFACE */

/**
 * Following structure will be created in ocstack to aggregate responses
 * (in future: for block transfer).
 */
#if INTERFACE
typedef struct OCServerResponse
{
    /** Node entry in red-black tree.*/
    RB_ENTRY(OCServerResponse) entry;

    /** this is the pointer to server payload data to be transferred.*/
    OCPayload* payload;

    /** Remaining size of the payload data to be transferred.*/
    uint16_t remainingPayloadSize;

    /** Requests to handle.*/
    OCRequestHandle requestHandle;
} OCServerResponse;
#endif

/**
 * This typedef is to represent our Server Instance identification.
 */
typedef uint8_t ServerID[16]; /* src: ocstackinternal.h */

/**
 * This function will be called back by CA layer when a request is received.
 *
 * @param endPoint CA remote endpoint.
 * @param requestInfo CA request info.
 */
void HandleCARequests(const CAEndpoint_t* endPoint, struct CARequestInfo* requestInfo)
{
    OIC_LOG(INFO, TAG, "Enter HandleCARequests");
    OIC_TRACE_BEGIN(%s:HandleCARequests, TAG);
    if (!endPoint)
    {
        OIC_LOG(ERROR, TAG, "endPoint is NULL");
        OIC_TRACE_END();
        return;
    }

    if (!requestInfo)
    {
        OIC_LOG(ERROR, TAG, "requestInfo is NULL");
        OIC_TRACE_END();
        return;
    }

    memcpy(&requestInfo->dest_ep, endPoint, sizeof(CAEndpoint_t));
    // FIXME
    requestInfo->requestComplete = 1; /* FIXME */

#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
#ifdef ROUTING_GATEWAY
    bool needRIHandling = false;
    bool isEmptyMsg = false;
    /*
     * Routing manager is going to update either of endpoint or request or both.
     * This typecasting is done to avoid unnecessary duplication of Endpoint and requestInfo
     * RM can update "routeData" option in endPoint so that future RI requests can be sent to proper
     * destination. It can also remove "RM" coap header option before passing request / response to
     * RI as this option will make no sense to either RI or application.
     */
    OCStackResult ret = RMHandleRequest((struct CARequestInfo *)requestInfo, (CAEndpoint_t *)endPoint,
                                        &needRIHandling, &isEmptyMsg);
    if (OC_STACK_OK != ret || !needRIHandling)
    {
        OIC_LOG_V(INFO, TAG, "Routing status![%d]. Not forwarding to RI", ret);
        OIC_TRACE_END();
        return;
    }
#endif

    /*
     * Put source in sender endpoint so that the next packet from application can be routed to
     * proper destination and remove RM header option.
     */
    RMUpdateInfo((CAHeaderOption_t **) &(requestInfo->info.options),
        (uint8_t *) &(requestInfo->info.numOptions),
        (CAEndpoint_t *)endPoint);

#ifdef ROUTING_GATEWAY
    if (isEmptyMsg)
    {
        /*
         * In Gateways, the MSGType in route option is used to check if the actual
         * response is EMPTY message(4 bytes CoAP Header).  In case of Client, the
         * EMPTY response is sent in the form of POST request which need to be changed
         * to a EMPTY response by RM.  This translation is done in this part of the code.
         */
        OIC_LOG(INFO, TAG, "This is a Empty response from the Client");
        CAResponseInfo_t respInfo = {.result = CA_EMPTY,
                                     .info.messageId = requestInfo->info.messageId,
                                     .info.type = CA_MSG_ACKNOWLEDGE};
        OCHandleResponse(endPoint, &respInfo);
    }
    else
#endif
#endif
    {
        // Normal handling of the packet
        OCHandleRequests(endPoint, requestInfo);
    }
    OIC_LOG(INFO, TAG, "Exit HandleCARequests");
    OIC_TRACE_END();
}

/** convert inbound request msg from CA structs to OC structs, then pass to handler  */
// FIXME: eliminate this routine! unify CA and OC layers
void OCHandleRequests(const CAEndpoint_t* origin_ep, struct CARequestInfo* requestInfo)
{
    OIC_TRACE_MARK(%s:OCHandleRequests:%s, TAG, requestInfo->info.resourceUri);
    OIC_LOG(DEBUG, TAG, "Enter OCHandleRequests");
    OIC_LOG_V(INFO, TAG, "Endpoint URI : %s", requestInfo->info.resourceUri);

    LogEndpoint(origin_ep);

    if (myStackMode == OC_CLIENT)
    {
        //TODO: should the client be responding to requests?
        OIC_LOG(DEBUG, TAG, "Mode == client; discarding request");
        return;
    }

    // If the request message is Confirmable,
    // then the response SHOULD be returned in an Acknowledgement message.
    CAMessageType_t directResponseType = requestInfo->info.type;
    directResponseType = (directResponseType == CA_MSG_CONFIRM)
            ? CA_MSG_ACKNOWLEDGE : CA_MSG_NONCONFIRM;

    char * uriWithoutQuery = NULL;
    char * query = NULL;
    OCStackResult result = OC_STACK_ERROR;

    result = getQueryFromUri(requestInfo->info.resourceUri, &query, &uriWithoutQuery);

    if (result != OC_STACK_OK || !uriWithoutQuery)
    {
        OIC_LOG_V(ERROR, TAG, "getQueryFromUri() failed with OC error code %d\n", result);
        return;
    }
    OIC_LOG_V(INFO, TAG, "URI without query: %s", uriWithoutQuery);
    OIC_LOG_V(INFO, TAG, "Query : %s", query);

    /* OCServerProtocolRequest serverRequest = { 0 }; */
    /* if (strlen(uriWithoutQuery) < MAX_URI_LENGTH) */
    /* { */
    /*     OICStrcpy(serverRequest.resourceUrl, sizeof(serverRequest.resourceUrl), uriWithoutQuery); */
    /*     OICFree(uriWithoutQuery); */
    /* } */
    /* else */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "URI length exceeds MAX_URI_LENGTH."); */
    /*     OICFree(uriWithoutQuery); */
    /*     OICFree(query); */
    /*     return; */
    /* } */

    /* if (query) */
    /* { */
    /*     if (strlen(query) < MAX_QUERY_LENGTH) */
    /*     { */
    /*         OICStrcpy(serverRequest.query, sizeof(serverRequest.query), query); */
    /*         OICFree(query); */
    /*     } */
    /*     else */
    /*     { */
    /*         OIC_LOG(ERROR, TAG, "Query length exceeds MAX_QUERY_LENGTH."); */
    /*         OICFree(query); */
    /*         return; */
    /*     } */
    /* } */

    /* if ((requestInfo->info.payload) && (0 < requestInfo->info.payloadSize)) */
    /* { */
    /*     serverRequest.payloadFormat = CAToOCPayloadFormat(requestInfo->info.payloadFormat); */
    /*     serverRequest.reqTotalSize = requestInfo->info.payloadSize; */
    /*     serverRequest.payload = (uint8_t *) OICMalloc(requestInfo->info.payloadSize); */
    /*     if (!serverRequest.payload) */
    /*     { */
    /*         OIC_LOG(ERROR, TAG, "Allocation for payload failed."); */
    /*         return; */
    /*     } */
    /*     memcpy (serverRequest.payload, requestInfo->info.payload, */
    /*             requestInfo->info.payloadSize); */
    /* } */
    /* else */
    /* { */
    /*     serverRequest.reqTotalSize = 0; */
    /* } */

    /* switch (requestInfo->method) */
    /* { */
    /*     case CA_GET: */
    /*         serverRequest.method = OC_REST_GET; */
    /*         break; */
    /*     case CA_PUT: */
    /*         serverRequest.method = OC_REST_PUT; */
    /*         break; */
    /*     case CA_POST: */
    /*         serverRequest.method = OC_REST_POST; */
    /*         break; */
    /*     case CA_DELETE: */
    /*         serverRequest.method = OC_REST_DELETE; */
    /*         break; */
    /*     default: */
    /*         OIC_LOG_V(ERROR, TAG, "Received CA method %d not supported", requestInfo->method); */
    /*         SendDirectStackResponse(origin_ep, requestInfo->info.messageId, CA_BAD_REQ, */
    /*                                 directResponseType, requestInfo->info.numOptions, */
    /*                                 requestInfo->info.options, requestInfo->info.token, */
    /*                                 requestInfo->info.tokenLength, requestInfo->info.resourceUri, */
    /*                                 CA_RESPONSE_DATA); */
    /*         OICFree(serverRequest.payload); */
    /*         return; */
    /* } */

    /* OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)requestInfo->info.token, */
    /*                requestInfo->info.tokenLength); */

    /* serverRequest.tokenLength = requestInfo->info.tokenLength; */
    /* if (serverRequest.tokenLength) */
    /* { */
    /*     // Non empty token */
    /*     serverRequest.requestToken = (uint8_t*)OICMalloc(requestInfo->info.tokenLength); */

    /*     if (!serverRequest.requestToken) */
    /*     { */
    /*         OIC_LOG(FATAL, TAG, "Allocation for token failed."); */
    /*         SendDirectStackResponse(origin_ep, requestInfo->info.messageId, CA_INTERNAL_SERVER_ERROR, */
    /*                                 directResponseType, requestInfo->info.numOptions, */
    /*                                 requestInfo->info.options, requestInfo->info.token, */
    /*                                 requestInfo->info.tokenLength, requestInfo->info.resourceUri, */
    /*                                 CA_RESPONSE_DATA); */
    /*         OICFree(serverRequest.payload); */
    /*         return; */
    /*     } */
    /*     memcpy(serverRequest.requestToken, requestInfo->info.token, requestInfo->info.tokenLength); */
    /* } */

    /* serverRequest.acceptFormat = CAToOCPayloadFormat(requestInfo->info.acceptFormat); */
    /* if (OC_FORMAT_VND_OCF_CBOR == serverRequest.acceptFormat) */
    /* { */
    /*     serverRequest.acceptVersion = requestInfo->info.acceptVersion; */
    /* } */

    /* if (requestInfo->info.type == CA_MSG_CONFIRM) */
    /* { */
    /*     serverRequest.qos = OC_HIGH_QOS; */
    /* } */
    /* else */
    /* { */
    /*     serverRequest.qos = OC_LOW_QOS; */
    /* } */
    // CA does not need the following field
    // Are we sure CA does not need them? how is it responding to multicast
    /* serverRequest.delayedResNeeded = 0; */

    /* serverRequest.coapID = requestInfo->info.messageId; */

    /* CopyEndpointToDevAddr(origin_ep, &serverRequest.devAddr); */

    // copy vendor specific header options
    /* uint8_t tempNum = (requestInfo->info.numOptions); */

    // Assume no observation requested and it is a pure GET.
    // If obs registration/de-registration requested it'll be fetched from the
    // options in GetObserveHeaderOption()
    /* serverRequest.observationOption = OC_OBSERVE_NO_OPTION; */

    /* GetObserveHeaderOption(&serverRequest.observationOption, requestInfo->info.options, &tempNum); */
    /* if (requestInfo->info.numOptions > MAX_HEADER_OPTIONS) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, */
    /*             "The request info numOptions is greater than MAX_HEADER_OPTIONS"); */
    /*     SendDirectStackResponse(origin_ep, requestInfo->info.messageId, CA_BAD_OPT, */
    /*                             directResponseType, requestInfo->info.numOptions, */
    /*                             requestInfo->info.options, requestInfo->info.token, */
    /*                             requestInfo->info.tokenLength, requestInfo->info.resourceUri, */
    /*                             CA_RESPONSE_DATA); */
    /*     OICFree(serverRequest.payload); */
    /*     OICFree(serverRequest.requestToken); */
    /*     return; */
    /* } */
    /* serverRequest.numRcvdVendorSpecificHeaderOptions = tempNum; */
    /* if (serverRequest.numRcvdVendorSpecificHeaderOptions && requestInfo->info.options) */
    /* { */
    /*     memcpy(&(serverRequest.rcvdVendorSpecificHeaderOptions), requestInfo->info.options, */
    /*            sizeof(CAHeaderOption_t) * tempNum); */
    /* } */

    // requestResult = HandleStackRequests(&serverRequest);
    result = HandleStackRequests(origin_ep, requestInfo);

    if (result == OC_STACK_SLOW_RESOURCE)
    {
        // Send ACK to client as precursor to slow response
        if (requestInfo->info.type == CA_MSG_CONFIRM)
        {
            SendDirectStackResponse(origin_ep, requestInfo->info.messageId, CA_EMPTY,
                                    CA_MSG_ACKNOWLEDGE,0, NULL, NULL, 0, NULL,
                                    CA_RESPONSE_DATA);
        }
    }
    /* if (result == OC_STACK_RESOURCE_ERROR */
    /*         && serverRequest.observationOption == OC_OBSERVE_REGISTER) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "Observe Registration failed due to resource error"); */
    /* } */
    /* else */
    if (!OCResultToSuccess(result))
    {
        OIC_LOG_V(ERROR, TAG, "HandleStackRequests failed. error: %d", result);

        CAResponseResult_t stackResponse = OCToCAStackResult(result, requestInfo->method);

        SendDirectStackResponse(origin_ep, requestInfo->info.messageId, stackResponse,
                                directResponseType, requestInfo->info.numOptions,
                                requestInfo->info.options, requestInfo->info.token,
                                requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                CA_RESPONSE_DATA);
    }
    // requestToken is fed to HandleStackRequests, which then goes to AddServerRequest.
    // The token is copied in there, and is thus still owned by this function.
    /* OICFree(serverRequest.payload); */
    /* OICFree(serverRequest.requestToken); */
    OIC_LOG(INFO, TAG, "Exit OCHandleRequests");
}

/**
 * Handler function to execute stack requests
 *
 * @param protocolRequest      Pointer to the protocol requests from server.
 *
 * @return
 *     ::OCStackResult
 */
// FIXME: this routine just converts OCServerProtocolRequest to OCServerRequest
//OCStackResult HandleStackRequests(OCServerProtocolRequest * protocolRequest)
/* FIXME: HandleInboundRequests */
OCStackResult HandleStackRequests(const CAEndpoint_t* origin_ep, struct CARequestInfo* requestInfo)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    OCStackResult result = OC_STACK_ERROR;

    /* if (!protocolRequest) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "protocolRequest is NULL"); */
    /*     return OC_STACK_INVALID_PARAM; */
    /* } */
    if (!origin_ep)
    {
        OIC_LOG(ERROR, TAG, "origin_ep is NULL");
        OIC_TRACE_END();
        return OC_STACK_ERROR;
    }

    if (!requestInfo)
    {
        OIC_LOG(ERROR, TAG, "requestInfo is NULL");
        OIC_TRACE_END();
        return OC_STACK_ERROR;
    }

    /* convert OCServerProtocolRequest to OCServerRequest. Check to
       see if it is cached, if not, create and cache it */

    char *uriWithoutQuery = NULL;
    char *query = NULL;

    result = getQueryFromUri(requestInfo->info.resourceUri, &query, &uriWithoutQuery);

    if (result != OC_STACK_OK || !uriWithoutQuery)
    {
        OIC_LOG_V(ERROR, TAG, "getQueryFromUri() failed with OC error code %d\n", result);
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(INFO, TAG, "URI without query: %s", uriWithoutQuery);
    OIC_LOG_V(INFO, TAG, "Query : %s", query);

    if ( !(strlen(uriWithoutQuery) < MAX_URI_LENGTH))
    /* { */
    /*     OICStrcpy(serverRequest.resourceUrl, sizeof(serverRequest.resourceUrl), uriWithoutQuery); */
    /*     OICFree(uriWithoutQuery); */
    /* } */
    /* else */
    {
        OIC_LOG(ERROR, TAG, "URI length exceeds MAX_URI_LENGTH.");
        OICFree(uriWithoutQuery);
        OICFree(query);
        return OC_STACK_ERROR;
    }

    if (query)
    {
        if ( !(strlen(query) < MAX_QUERY_LENGTH) )
        /* { */
        /*     OICStrcpy(serverRequest.query, sizeof(serverRequest.query), query); */
        /*     OICFree(query); */
        /* } */
        /* else */
        {
            OIC_LOG(ERROR, TAG, "Query length exceeds MAX_QUERY_LENGTH.");
            OICFree(uriWithoutQuery);
            OICFree(query);
            return OC_STACK_ERROR;
        }
    }

    CAMessageType_t directResponseType = requestInfo->info.type;
    directResponseType = (directResponseType == CA_MSG_CONFIRM)
            ? CA_MSG_ACKNOWLEDGE : CA_MSG_NONCONFIRM;

    if (requestInfo->info.numOptions > MAX_HEADER_OPTIONS)
    {
        OIC_LOG(ERROR, TAG,
                "The request info numOptions is greater than MAX_HEADER_OPTIONS");
        SendDirectStackResponse(origin_ep, requestInfo->info.messageId, CA_BAD_OPT,
                                directResponseType, requestInfo->info.numOptions,
                                requestInfo->info.options, requestInfo->info.token,
                                requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                CA_RESPONSE_DATA);
        /* OICFree(serverRequest.payload); */
        /* OICFree(serverRequest.requestToken); */
        return OC_STACK_ERROR;
    }

    /* initialize for response */
    requestInfo->ehResponseHandler = HandleSingleResponse;
    requestInfo->numResponses = 1;

    // FIXME: move all this into observationOption = GetObserveHeaderOption(requestInfo)
    // we need local vars, GetObserveHeaderOption is destructive
    requestInfo->observationOption = OC_OBSERVE_NO_OPTION;
    uint8_t option_count = (requestInfo->info.numOptions);
    GetObserveHeaderOption(&requestInfo->observationOption, requestInfo->info.options, &option_count);

    if (requestInfo->info.numOptions > MAX_HEADER_OPTIONS)
    {
        OIC_LOG(ERROR, TAG,
                "The request info numOptions is greater than MAX_HEADER_OPTIONS");
        SendDirectStackResponse(origin_ep, requestInfo->info.messageId, CA_BAD_OPT,
                                directResponseType, requestInfo->info.numOptions,
                                requestInfo->info.options, requestInfo->info.token,
                                requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                CA_RESPONSE_DATA);
        /* OICFree(serverRequest.payload); */
        /* OICFree(serverRequest.requestToken); */
        return OC_STACK_ERROR;
    }

    /* check inbound request cache for this request  */
    /* OCServerRequest * request = GetServerRequestUsingToken(protocolRequest->requestToken, */
    /*                                                        protocolRequest->tokenLength); */

    /* OCServerRequest * request = GetServerRequestUsingToken(requestInfo->info.token, */
    /*                                                        requestInfo->info.tokenLength); */

    /* struct CARequestInfo * request = GetServerRequestUsingToken(requestInfo->info.token, */
    /*                                                             requestInfo->info.tokenLength); */
    struct CARequestInfo tmpFind, *request = NULL;
    tmpFind.info.token = requestInfo->info.token;
    tmpFind.info.tokenLength = requestInfo->info.tokenLength;
    /* out = RB_FIND(ServerRequestTree, &g_serverRequestTree, &tmpFind); */
    request = RB_FIND(_oocf_request_cache_tree, &g_request_cache_tree, &tmpFind);

    if (!request)
    {
        OIC_LOG(INFO, TAG, "This is a new (uncached) inbound Request");
        request = _oocf_request_cache_put(requestInfo);

        /* result = AddServerRequest(&request, */
        /*                           requestInfo->info.messageId,  // protocolRequest->coapID, */
        /*                           0; // protocolRequest->delayedResNeeded, */
        /*                           0, /\* uint8_t notificationFlag, *\/ */
        /*                           requestInfo->method, //protocolRequest->method, */
        /*                           requestInfo->info.numOptions;//protocolRequest->numRcvdVendorSpecificHeaderOptions, */
        /*                           observationOption, // protocolRequest->observationOption, */
        /*                           // CAMessageType_t: CA_MSG_CONFIRM = 0, CA_MSG_NONCONFIRM, CA_MSG_ACKNOWLEDGE, CA_MSG_RESET */
        /*                           // OCQualityOfService: OC_LOW_QOS = 0, OC_MEDIUM_QOS, OC_HIGH_QOS, OC_NA_QOS */

        /*                           // FIXME: mapping from coap msg types to qos */
        /*                           //requestInfo->info.type, // protocolRequest->qos, */
        /*                           OC_LOW_QOS, // for testing hardcode low qos */
        /*                           query,  // protocolRequest->query, */
        /*                           requestInfo->info.options,  // protocolRequest->rcvdVendorSpecificHeaderOptions, */
        /*                           requestInfo->info.payloadFormat, // protocolRequest->payloadFormat, */
        /*                           requestInfo->info.payload, // protocolRequest->payload, */
        /*                           requestInfo->info.token, // protocolRequest->requestToken, */
        /*                           requestInfo->info.tokenLength, // protocolRequest->tokenLength, */
        /*                           uriWithoutQuery, // protocolRequest->resourceUrl, */
        /*                           requestInfo->info.payloadSize, // protocolRequest->reqTotalSize, */
        /*                           requestInfo->info.acceptFormat, // protocolRequest->acceptFormat, */
        /*                           requestInfo->info.acceptVersion, // protocolRequest->acceptVersion, */
        /*                           &endpoint);  // &protocolRequest->devAddr); */
        /* if (OC_STACK_OK != result) */
        /* { */
        /*     OIC_LOG(ERROR, TAG, "Error adding server request"); */
        /*     return result; */
        /* } */

        /* if(!request) */
        /* { */
        /*     OIC_LOG(ERROR, TAG, "Out of Memory"); */
        /*     return OC_STACK_NO_MEMORY; */
        /* } */

        /* if(!protocolRequest->reqMorePacket) */
        /* { */
        /*     request->requestComplete = 1; */
        /* } */
    }
    else
    {
        OIC_LOG(INFO, TAG, "This is a cached (either repeated or blocked) inbound Request");
    }

    if (request->requestComplete)
    {
        OIC_LOG(INFO, TAG, "This Server Request is complete");
        ResourceHandling resHandling = OC_RESOURCE_VIRTUAL;
        OCResource *resource = NULL;
        // result = DetermineResourceHandling (request, &resHandling, &resource);
        result = DetermineResourceHandling(request, &resHandling, &resource);
        if (result == OC_STACK_OK)
        {
            /* result = ProcessRequest(resHandling, resource, request); */
            result = ProcessRequest(resHandling, resource, request);
        } else {
	    OIC_LOG_V(DEBUG, TAG, "DetermineResourceHandling failure: %d", result);
	}
    }
    else
    {
        OIC_LOG(INFO, TAG, "This Server Request is incomplete");
        result = OC_STACK_CONTINUE;
    }
    return result;
}

OCStackResult ProcessRequest(ResourceHandling resHandling, OCResource *resource, struct CARequestInfo *request)
// OCStackResult ProcessRequest(ResourceHandling resHandling, OCResource *resource, OCServerRequest *request)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    OCStackResult ret = OC_STACK_OK;

    switch (resHandling)
    {
        case OC_RESOURCE_VIRTUAL:
        {
            ret = HandleVirtualResource(request, resource);
            break;
        }
        case OC_RESOURCE_DEFAULT_DEVICE_ENTITYHANDLER:
        {
            ret = HandleDefaultDeviceEntityHandler(request);
            break;
        }
        case OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER:
        {
            OIC_LOG(ERROR, TAG, "OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER");
            return OC_STACK_ERROR;
        }
        case OC_RESOURCE_NOT_COLLECTION_WITH_ENTITYHANDLER:
        {
            ret = HandleResourceWithEntityHandler(request, resource);
            break;
        }
        case OC_RESOURCE_COLLECTION_WITH_ENTITYHANDLER:
        {
            ret = HandleResourceWithEntityHandler(request, resource);
            break;
        }
        case OC_RESOURCE_COLLECTION_DEFAULT_ENTITYHANDLER:
        {
            ret = HandleCollectionResourceDefaultEntityHandler (request, resource);
            break;
        }
        case OC_RESOURCE_NOT_SPECIFIED:
        {
            ret = OC_STACK_NO_RESOURCE;
            break;
        }
        default:
        {
            OIC_LOG(INFO, TAG, "Invalid Resource Determination");
            return OC_STACK_ERROR;
        }
    }
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
    return ret;
}

/* src: ocresource.c */
// FIXME: @rename _oocf_pass_request_to_user_handler
OCStackResult HandleResourceWithEntityHandler(struct CARequestInfo *request,
                                              OCResource *resource)
{
    if(!request || ! resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_ERROR;
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerFlag ehFlag = OC_REQUEST_FLAG;
    ResourceObserver *resObs = NULL;

    //OCEntityHandlerRequest ehRequest = {0};
    struct oocf_inbound_request ehRequest = {0};

    OIC_LOG(INFO, TAG, "Entering HandleResourceWithEntityHandler");
    OCPayloadType type = PAYLOAD_TYPE_REPRESENTATION;
    // check the security resource
    if (request /* GAR: this is always true: && request->resourceUrl */
	&& SRMIsSecurityResourceURI(getPathFromRequestURL(request->info.resourceUri)))
    {
        type = PAYLOAD_TYPE_SECURITY;
    }

    result = EHRequest(&ehRequest, type, request, resource);
    VERIFY_SUCCESS_1(result);

    if(ehRequest.obsInfo.action == OC_OBSERVE_NO_OPTION)
    {
        OIC_LOG(INFO, TAG, "No observation requested");
        ehFlag = OC_REQUEST_FLAG;
    }
    else if(ehRequest.obsInfo.action == OC_OBSERVE_REGISTER)
    {
        OIC_LOG(INFO, TAG, "Observation registration requested");

        ResourceObserver *obs = GetObserverUsingToken(resource,
                                                      request->info.token,
                                                      request->info.tokenLength);

        if (obs)
        {
            OIC_LOG (INFO, TAG, "Observer with this token already present");
            OIC_LOG (INFO, TAG, "Possibly re-transmitted CON OBS request");
            OIC_LOG (INFO, TAG, "Not adding observer. Not responding to client");
            OIC_LOG (INFO, TAG, "The first request for this token is already ACKED.");

            // server requests are usually free'd when the response is sent out
            // for the request in ocserverrequest.c : HandleSingleResponse()
            // Since we are making an early return and not responding, the server request
            // needs to be deleted.
            // DeleteServerRequest (request);
            _oocf_request_cache_delete(request);
            return OC_STACK_OK;
        }

        result = GenerateObserverId(&ehRequest.obsInfo.obsId);
        VERIFY_SUCCESS_1(result);

        result = AddObserver ((const char*)getPathFromRequestURL(request->info.resourceUri),
                              (const char*)getQueryFromRequestURL(request->info.resourceUri),
                              ehRequest.obsInfo.obsId,
                              request->info.token,
                              request->info.tokenLength,
                              resource,
                              request->info.type, /* coap msg type */
                              request->info.acceptFormat,
                              request->info.acceptVersion,
                              &request->dest_ep); // devAddr);
        \
        if(result == OC_STACK_OK)
        {
            OIC_LOG(INFO, TAG, "Added observer successfully");
            request->observeResult = OC_STACK_OK;
            ehFlag = (OCEntityHandlerFlag)(OC_REQUEST_FLAG | OC_OBSERVE_FLAG);
        }
        else if (result == OC_STACK_RESOURCE_ERROR)
        {
            OIC_LOG(INFO, TAG, "The Resource is not active, discoverable or observable");
            request->observeResult = OC_STACK_ERROR;
            ehFlag = OC_REQUEST_FLAG;
        }
        else
        {
            // The error in observeResult for the request will be used when responding to this
            // request by omitting the observation option/sequence number.
            request->observeResult = OC_STACK_ERROR;
            OIC_LOG(ERROR, TAG, "Observer Addition failed");
            ehFlag = OC_REQUEST_FLAG;
            // DeleteServerRequest(request);
            _oocf_request_cache_delete(request);
            goto exit;
        }

    }
    else if(ehRequest.obsInfo.action == OC_OBSERVE_DEREGISTER)
    {
        OIC_LOG(INFO, TAG, "Deregistering observation requested");

        resObs = GetObserverUsingToken (resource,
                                        request->info.token,
                                        request->info.tokenLength);

        if (NULL == resObs)
        {
            // Stack does not contain this observation request
            // Either token is incorrect or observation list is corrupted
            result = OC_STACK_ERROR;
            goto exit;
        }
        ehRequest.obsInfo.obsId = resObs->observeId;
        ehFlag = (OCEntityHandlerFlag)(ehFlag | OC_OBSERVE_FLAG);

        result = DeleteObserverUsingToken (resource,
                                           request->info.token,
                                           request->info.tokenLength);

        if(result == OC_STACK_OK)
        {
            OIC_LOG(INFO, TAG, "Removed observer successfully");
            request->observeResult = OC_STACK_OK;
            // There should be no observe option header for de-registration response.
            // Set as an invalid value here so we can detect it later and remove the field in response.
            request->observationOption = MAX_SEQUENCE_NUMBER + 1;
        }
        else
        {
            request->observeResult = OC_STACK_ERROR;
            OIC_LOG(ERROR, TAG, "Observer Removal failed");
            // DeleteServerRequest(request);
            _oocf_request_cache_delete(request);
            goto exit;
        }
    }
    else
    {
        result = OC_STACK_ERROR;
        goto exit;
    }

    ehResult = resource->entityHandler(ehFlag, &ehRequest, resource->entityHandlerCallbackParam);
    if(ehResult == OC_EH_SLOW)
    {
        OIC_LOG(INFO, TAG, "This is a slow resource");
        request->slowFlag = 1;
    }
    else if(ehResult == OC_EH_ERROR)
    {
        // DeleteServerRequest(request);
        _oocf_request_cache_delete(request);
    }
    result = EntityHandlerCodeToOCStackCode(ehResult);
exit:
    //OCPayloadDestroy(ehRequest.payload);
    return result;
}

/* OCStackResult XHandleResourceWithEntityHandler(OCServerRequest *request, */
/*                                               OCResource *resource) */
/* { */
/*     if(!request || ! resource) */
/*     { */
/*         return OC_STACK_INVALID_PARAM; */
/*     } */

/*     OCStackResult result = OC_STACK_ERROR; */
/*     OCEntityHandlerResult ehResult = OC_EH_ERROR; */
/*     OCEntityHandlerFlag ehFlag = OC_REQUEST_FLAG; */
/*     ResourceObserver *resObs = NULL; */

/*     OCEntityHandlerRequest ehRequest = {0}; */

/*     OIC_LOG(INFO, TAG, "Entering HandleResourceWithEntityHandler"); */
/*     OCPayloadType type = PAYLOAD_TYPE_REPRESENTATION; */
/*     // check the security resource */
/*     if (request /\* GAR: this is always true: && request->resourceUrl *\/ */
/* 	&& SRMIsSecurityResourceURI(request->resourceUrl)) */
/*     { */
/*         type = PAYLOAD_TYPE_SECURITY; */
/*     } */

/*     result = EHRequest(&ehRequest, type, request, resource); */
/*     VERIFY_SUCCESS_1(result); */

/*     if(ehRequest.obsInfo.action == OC_OBSERVE_NO_OPTION) */
/*     { */
/*         OIC_LOG(INFO, TAG, "No observation requested"); */
/*         ehFlag = OC_REQUEST_FLAG; */
/*     } */
/*     else if(ehRequest.obsInfo.action == OC_OBSERVE_REGISTER) */
/*     { */
/*         OIC_LOG(INFO, TAG, "Observation registration requested"); */

/*         ResourceObserver *obs = GetObserverUsingToken(resource, */
/*                                                       request->requestToken, request->tokenLength); */

/*         if (obs) */
/*         { */
/*             OIC_LOG (INFO, TAG, "Observer with this token already present"); */
/*             OIC_LOG (INFO, TAG, "Possibly re-transmitted CON OBS request"); */
/*             OIC_LOG (INFO, TAG, "Not adding observer. Not responding to client"); */
/*             OIC_LOG (INFO, TAG, "The first request for this token is already ACKED."); */

/*             // server requests are usually free'd when the response is sent out */
/*             // for the request in ocserverrequest.c : HandleSingleResponse() */
/*             // Since we are making an early return and not responding, the server request */
/*             // needs to be deleted. */
/*             DeleteServerRequest (request); */
/*             return OC_STACK_OK; */
/*         } */

/*         result = GenerateObserverId(&ehRequest.obsInfo.obsId); */
/*         VERIFY_SUCCESS_1(result); */

/*         result = AddObserver ((const char*)(request->resourceUrl), */
/*                 (const char *)(request->query), */
/*                 ehRequest.obsInfo.obsId, request->requestToken, request->tokenLength, */
/*                 resource, request->qos, request->acceptFormat, */
/*                 request->acceptVersion, &request->devAddr); */

/*         if(result == OC_STACK_OK) */
/*         { */
/*             OIC_LOG(INFO, TAG, "Added observer successfully"); */
/*             request->observeResult = OC_STACK_OK; */
/*             ehFlag = (OCEntityHandlerFlag)(OC_REQUEST_FLAG | OC_OBSERVE_FLAG); */
/*         } */
/*         else if (result == OC_STACK_RESOURCE_ERROR) */
/*         { */
/*             OIC_LOG(INFO, TAG, "The Resource is not active, discoverable or observable"); */
/*             request->observeResult = OC_STACK_ERROR; */
/*             ehFlag = OC_REQUEST_FLAG; */
/*         } */
/*         else */
/*         { */
/*             // The error in observeResult for the request will be used when responding to this */
/*             // request by omitting the observation option/sequence number. */
/*             request->observeResult = OC_STACK_ERROR; */
/*             OIC_LOG(ERROR, TAG, "Observer Addition failed"); */
/*             ehFlag = OC_REQUEST_FLAG; */
/*             DeleteServerRequest(request); */
/*             goto exit; */
/*         } */

/*     } */
/*     else if(ehRequest.obsInfo.action == OC_OBSERVE_DEREGISTER) */
/*     { */
/*         OIC_LOG(INFO, TAG, "Deregistering observation requested"); */

/*         resObs = GetObserverUsingToken (resource, */
/*                                         request->requestToken, request->tokenLength); */

/*         if (NULL == resObs) */
/*         { */
/*             // Stack does not contain this observation request */
/*             // Either token is incorrect or observation list is corrupted */
/*             result = OC_STACK_ERROR; */
/*             goto exit; */
/*         } */
/*         ehRequest.obsInfo.obsId = resObs->observeId; */
/*         ehFlag = (OCEntityHandlerFlag)(ehFlag | OC_OBSERVE_FLAG); */

/*         result = DeleteObserverUsingToken (resource, */
/*                                            request->requestToken, request->tokenLength); */

/*         if(result == OC_STACK_OK) */
/*         { */
/*             OIC_LOG(INFO, TAG, "Removed observer successfully"); */
/*             request->observeResult = OC_STACK_OK; */
/*             // There should be no observe option header for de-registration response. */
/*             // Set as an invalid value here so we can detect it later and remove the field in response. */
/*             request->observationOption = MAX_SEQUENCE_NUMBER + 1; */
/*         } */
/*         else */
/*         { */
/*             request->observeResult = OC_STACK_ERROR; */
/*             OIC_LOG(ERROR, TAG, "Observer Removal failed"); */
/*             DeleteServerRequest(request); */
/*             goto exit; */
/*         } */
/*     } */
/*     else */
/*     { */
/*         result = OC_STACK_ERROR; */
/*         goto exit; */
/*     } */

/*     ehResult = resource->entityHandler(ehFlag, &ehRequest, resource->entityHandlerCallbackParam); */
/*     if(ehResult == OC_EH_SLOW) */
/*     { */
/*         OIC_LOG(INFO, TAG, "This is a slow resource"); */
/*         request->slowFlag = 1; */
/*     } */
/*     else if(ehResult == OC_EH_ERROR) */
/*     { */
/*         // DeleteServerRequest(request); */
/*     } */
/*     result = EntityHandlerCodeToOCStackCode(ehResult); */
/* exit: */
/*     OCPayloadDestroy(ehRequest.payload); */
/*     return result; */
/* } */

/* src: ocresource.c */
OCStackResult EHRequest(struct oocf_inbound_request /* OCEntityHandlerRequest */ *ehRequest,
                        OCPayloadType type,
                        struct CARequestInfo *request,
                        struct OCResource *resource)
{
    return FormOCEntityHandlerRequest(ehRequest,
                                      (OCRequestHandle)request,
                                      request->method,
                                      &request->dest_ep, // &request->devAddr,
                                     (OCResourceHandle)resource,
                                      getQueryFromRequestURL(request->info.resourceUri), //request->query,
                                      type,
                                      request->info.payloadFormat, // request->payloadFormat,
                                      request->info.payload, // request->payload,
                                      request->info.payloadSize, // request->payloadSize,
                                      request->info.numOptions, // request->numRcvdVendorSpecificHeaderOptions,
                                      request->info.options, // request->rcvdVendorSpecificHeaderOptions,
                                      (OCObserveAction)(request->notificationFlag ? OC_OBSERVE_NO_OPTION :
                                                        request->observationOption),
                                      (OCObservationId)0,
                                      request->info.messageId //request->coapID
                                      );
}

/* OCStackResult XEHRequest(OCEntityHandlerRequest *ehRequest, */
/*                         OCPayloadType type, */
/*                          OCServerRequest *request, */
/*                         struct OCResource *resource) */
/* { */
/*     return FormOCEntityHandlerRequest(ehRequest, */
/*                                      (OCRequestHandle)request, */
/*                                      request->method, */
/*                                      &request->devAddr, */
/*                                      (OCResourceHandle)resource, */
/*                                      request->query, */
/*                                      type, */
/*                                      request->payloadFormat, */
/*                                      request->payload, */
/*                                      request->payloadSize, */
/*                                      request->numRcvdVendorSpecificHeaderOptions, */
/*                                      request->rcvdVendorSpecificHeaderOptions, */
/*                                      (OCObserveAction)(request->notificationFlag ? OC_OBSERVE_NO_OPTION : */
/*                                                        request->observationOption), */
/*                                      (OCObservationId)0, */
/*                                      request->coapID); */
/* } */

/* src: ocserverrequest.c */
OCStackResult FormOCEntityHandlerRequest(struct oocf_inbound_request /* OCEntityHandlerRequest */ *inbound_request,
                                         OCRequestHandle request,
                                         OCMethod method,
                                         CAEndpoint_t * /* OCDevAddr */ endpoint,
                                         OCResourceHandle resource,
                                         char * queryBuf,
                                         OCPayloadType payloadType,
                                         OCPayloadFormat payloadFormat,
                                         struct OCPayload /* uint8_t */ * payload,
                                         size_t payloadSize,
                                         uint8_t numVendorOptions,
                                         CAHeaderOption_t * /* OCHeaderOption */ vendorOptions,
                                         OCObserveAction observeAction,
                                         OCObservationId observeID,
                                         uint16_t messageID)
{
    if (inbound_request)
    {
        inbound_request->resource = (OCResourceHandle) resource;
        inbound_request->requestHandle = request;
        /* inbound_request->method = method; */
        inbound_request->origin_ep = *endpoint;
        /* inbound_request->query = queryBuf; */
        /* inbound_request->obsInfo.action = observeAction; */
        /* inbound_request->obsInfo.obsId = observeID; */
        /* inbound_request->messageID = messageID; */

        /* if(payload && payloadSize) */
        /* { */
        /*     if(OCParsePayload(&inbound_request->payload, payloadFormat, payloadType, */
        /*                 payload, payloadSize) != OC_STACK_OK) */
        /*     { */
        /*         return OC_STACK_ERROR; */
        /*     } */
        /* } */
        /* else */
        /* { */
        /*     inbound_request->payload = NULL; */
        /* } */

        /* inbound_request->numRcvdVendorSpecificHeaderOptions = numVendorOptions; */
        /* inbound_request->rcvdVendorSpecificHeaderOptions = vendorOptions; */

        return OC_STACK_OK;
    }

    return OC_STACK_INVALID_PARAM;
}

/* OCStackResult XFormOCEntityHandlerRequest(OCEntityHandlerRequest * entityHandlerRequest, */
/*                                          OCRequestHandle request, */
/*                                          OCMethod method, */
/*                                          OCDevAddr *endpoint, */
/*                                          OCResourceHandle resource, */
/*                                          char * queryBuf, */
/*                                          OCPayloadType payloadType, */
/*                                          OCPayloadFormat payloadFormat, */
/*                                          uint8_t * payload, */
/*                                          size_t payloadSize, */
/*                                          uint8_t numVendorOptions, */
/*                                          OCHeaderOption * vendorOptions, */
/*                                          OCObserveAction observeAction, */
/*                                          OCObservationId observeID, */
/*                                          uint16_t messageID) */
/* { */
/*     if (entityHandlerRequest) */
/*     { */
/*         entityHandlerRequest->resource = (OCResourceHandle) resource; */
/*         entityHandlerRequest->requestHandle = request; */
/*         entityHandlerRequest->method = method; */
/*         entityHandlerRequest->devAddr = *endpoint; */
/*         entityHandlerRequest->query = queryBuf; */
/*         entityHandlerRequest->obsInfo.action = observeAction; */
/*         entityHandlerRequest->obsInfo.obsId = observeID; */
/*         entityHandlerRequest->messageID = messageID; */

/*         if(payload && payloadSize) */
/*         { */
/*             if(OCParsePayload(&entityHandlerRequest->payload, payloadFormat, payloadType, */
/*                         payload, payloadSize) != OC_STACK_OK) */
/*             { */
/*                 return OC_STACK_ERROR; */
/*             } */
/*         } */
/*         else */
/*         { */
/*             entityHandlerRequest->payload = NULL; */
/*         } */

/*         entityHandlerRequest->numRcvdVendorSpecificHeaderOptions = numVendorOptions; */
/*         entityHandlerRequest->rcvdVendorSpecificHeaderOptions = vendorOptions; */

/*         return OC_STACK_OK; */
/*     } */

/*     return OC_STACK_INVALID_PARAM; */
/* } */

/*
 * This function sends out Direct Stack Responses. These are responses that are not coming
 * from the application entity handler. These responses have no payload and are usually ACKs,
 * RESETs or some error conditions that were caught by the bstack.
 */
OCStackResult SendDirectStackResponse(const CAEndpoint_t* endPoint, const uint16_t coapID,
                                      const CAResponseResult_t responseResult, const CAMessageType_t type,
                                      const uint8_t numOptions, const CAHeaderOption_t *options,
                                      uint8_t *token, uint8_t tokenLength, const char *resourceUri,
                                      CADataType_t dataType)
{
    OIC_LOG(DEBUG, TAG, "Entering SendDirectStackResponse");
    CAResponseInfo_t respInfo = {
        .result = responseResult
    };
    respInfo.info.messageId = coapID;
    respInfo.info.numOptions = numOptions;

    if (respInfo.info.numOptions)
    {
        respInfo.info.options =
            (CAHeaderOption_t *)OICCalloc(respInfo.info.numOptions, sizeof(CAHeaderOption_t));
        if (NULL == respInfo.info.options)
        {
            OIC_LOG(ERROR, TAG, "Calloc failed");
            return OC_STACK_NO_MEMORY;
        }
        memcpy (respInfo.info.options, options,
                sizeof(CAHeaderOption_t) * respInfo.info.numOptions);

    }

    respInfo.info.payload = NULL;
    respInfo.info.token = token;
    respInfo.info.tokenLength = tokenLength;
    respInfo.info.type = type;
    respInfo.info.resourceUri = OICStrdup (resourceUri);
    respInfo.info.acceptFormat = CA_FORMAT_UNDEFINED;
    respInfo.info.dataType = dataType;

#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
    // Add the destination to route option from the endpoint->routeData.
    bool doPost = false;
    OCStackResult result = RMAddInfo(endPoint->routeData, &respInfo, false, &doPost);
    if(OC_STACK_OK != result)
    {
        OIC_LOG_V(ERROR, TAG, "Add routing option failed [%d]", result);
        OICFree (respInfo.info.resourceUri);
        OICFree (respInfo.info.options);
        return result;
    }
    if (doPost)
    {
        OIC_LOG(DEBUG, TAG, "Sending a POST message for EMPTY ACK in Client Mode");
        struct CARequestInfo reqInfo = {.method = CA_POST };
        /* The following initialization is not done in a single initializer block as in
         * arduino, .c file is compiled as .cpp and moves it from C99 to C++11.  The latter
         * does not have designated initalizers. This is a work-around for now.
         */
        reqInfo.info.type = CA_MSG_NONCONFIRM;
        reqInfo.info.messageId = coapID;
        reqInfo.info.tokenLength = tokenLength;
        reqInfo.info.token = token;
        reqInfo.info.numOptions = respInfo.info.numOptions;
        reqInfo.info.payload = NULL;
        reqInfo.info.resourceUri = OICStrdup (OC_RSRVD_GATEWAY_URI);
        if (reqInfo.info.numOptions)
        {
            reqInfo.info.options =
                (CAHeaderOption_t *)OICCalloc(reqInfo.info.numOptions, sizeof(CAHeaderOption_t));
            if (NULL == reqInfo.info.options)
            {
                OIC_LOG(ERROR, TAG, "Calloc failed");
                OICFree (reqInfo.info.resourceUri);
                OICFree (respInfo.info.resourceUri);
                OICFree (respInfo.info.options);
                return OC_STACK_NO_MEMORY;
            }
            memcpy (reqInfo.info.options, respInfo.info.options,
                    sizeof(CAHeaderOption_t) * reqInfo.info.numOptions);

        }
        CAResult_t caResult = CASendRequest(endPoint, &reqInfo);
        OICFree (reqInfo.info.resourceUri);
        OICFree (reqInfo.info.options);
        OICFree (respInfo.info.resourceUri);
        OICFree (respInfo.info.options);
        if (CA_STATUS_OK != caResult)
        {
            OIC_LOG(ERROR, TAG, "CASendRequest error");
            return CAResultToOCResult(caResult);
        }
    }
    else
#endif
    {
        CAResult_t caResult = CASendResponse(endPoint, &respInfo);

        // resourceUri in the info field is cloned in the CA layer and
        // thus ownership is still here.
        OICFree (respInfo.info.resourceUri);
        OICFree (respInfo.info.options);
        if(CA_STATUS_OK != caResult)
        {
            OIC_LOG(ERROR, TAG, "CASendResponse error");
            return CAResultToOCResult(caResult);
        }
    }
    OIC_LOG(DEBUG, TAG, "Exit SendDirectStackResponse");
    return OC_STACK_OK;
}

/* oocf_send_response */
OCStackResult OC_CALL OCDoResponse(OCEntityHandlerResponse *ehResponse) EXPORT
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_TRACE_BEGIN(%s:OCDoResponse, TAG);
    OCStackResult result = OC_STACK_ERROR;
    // OCServerRequest *serverRequest = NULL;
    struct CARequestInfo *request = NULL;

    OIC_LOG_V(DEBUG, TAG, "%s result: 0x%04x", __func__, ehResponse->ehResult);

    // Validate input parameters
    VERIFY_NON_NULL(ehResponse, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(ehResponse->requestHandle, ERROR, OC_STACK_INVALID_PARAM);

    // Normal response
    // Get pointer to request info
    // serverRequest = (OCServerRequest *)ehResponse->requestHandle;
    request = (struct CARequestInfo *)ehResponse->requestHandle;
    VERIFY_NON_NULL(request->ehResponseHandler, ERROR, OC_STACK_INVALID_PARAM);
    if(request)
    {
        // response handler in ocserverrequest.c. Usually HandleSingleResponse.
        // ehResponseHandler will be either HandleSingleResponse or HandleAggregateResponse
        result = request->ehResponseHandler(ehResponse);
    }

    OIC_TRACE_END();
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return result;
}

char  *get_query_from_inbound_request(const struct oocf_inbound_request * request) EXPORT
{
    if(!request)
    {
        return NULL;
    }

    char *url = ((struct CARequestInfo*)request)->info.resourceUri;

    char *uriWithoutQuery = NULL;
    char *query = NULL;
    OCStackResult requestResult = OC_STACK_ERROR;

    requestResult = getQueryFromUri(url, &query, &uriWithoutQuery);

    if (requestResult != OC_STACK_OK || !uriWithoutQuery)
    {
        OIC_LOG_V(ERROR, TAG, "get_query_inbound_request() failed with OC error code %d\n", requestResult);
        return NULL;
    }
    return query;
}

struct OCPayload *get_payload_from_inbound_request(const struct oocf_inbound_request * request) EXPORT
{
    /* if(!request) */
    /* { */
    /*     return NULL; */
    /* } */
    return ((struct CARequestInfo*)request->requestHandle)->info.payload;
}

struct oocf_endpoint *get_origin_ep_from_inbound_request(const struct oocf_inbound_request *request) EXPORT
{
    return &(request->origin_ep);
}
