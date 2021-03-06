#include "oocf_server.h"

/**
 * Possible returned values from entity handler.
 */
#if INTERFACE
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
#endif

/**
 * This structure will be created in occoap and passed up the stack on the server side.
 */
#if INTERFACE
typedef struct
{
    /** Observe option field.*/
    uint32_t observationOption;

    /** The REST method retrieved from received request PDU.*/
    OCMethod method;

    /** the provided payload format. */
    OCPayloadFormat payloadFormat;

    /** the requested payload format. */
    OCPayloadFormat acceptFormat;

    /** the requested payload version. */
    uint16_t acceptVersion;

    /** resourceUrl will be filled in occoap using the path options in received request PDU.*/
    char resourceUrl[MAX_URI_LENGTH];

    /** resource query send by client.*/
    char query[MAX_QUERY_LENGTH];

    /** reqJSON is retrieved from the payload of the received request PDU.*/
    uint8_t *payload;

    /** qos is indicating if the request is CON or NON.*/
    OCQualityOfService qos;

    /** Number of the received vendor specific header options.*/
    uint8_t numRcvdVendorSpecificHeaderOptions;

    /** Array of received vendor specific header option .*/
    OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];

    /** Remote end-point address **/
    OCDevAddr devAddr;

    /** Token for the observe request.*/
    CAToken_t requestToken;

    /** token length.*/
    uint8_t tokenLength;

    /** The ID of CoAP PDU.*/
    uint16_t coapID;

    /** For delayed Response.*/
    uint8_t delayedResNeeded;

    /** For More packet.*/
    uint8_t reqMorePacket;

    /** The number of requested packet.*/
    uint32_t reqPacketNum;

    /** The size of requested packet.*/
    uint16_t reqPacketSize;

    /** The number of responded packet.*/
    uint32_t resPacketNum;

    /** Responded packet size.*/
    uint16_t resPacketSize;

    /** The total size of requested packet.*/
    size_t reqTotalSize;
} OCServerProtocolRequest; /* src: ocstackinternal.h */
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
void HandleCARequests(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
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
    OCStackResult ret = RMHandleRequest((CARequestInfo_t *)requestInfo, (CAEndpoint_t *)endPoint,
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

void OCHandleRequests(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
{
    OIC_TRACE_MARK(%s:OCHandleRequests:%s, TAG, requestInfo->info.resourceUri);
    OIC_LOG(DEBUG, TAG, "Enter OCHandleRequests");
    OIC_LOG_V(INFO, TAG, "Endpoint URI : %s", requestInfo->info.resourceUri);

    if (myStackMode == OC_CLIENT)
    {
        //TODO: should the client be responding to requests?
        return;
    }

    // If the request message is Confirmable,
    // then the response SHOULD be returned in an Acknowledgement message.
    CAMessageType_t directResponseType = requestInfo->info.type;
    directResponseType = (directResponseType == CA_MSG_CONFIRM)
            ? CA_MSG_ACKNOWLEDGE : CA_MSG_NONCONFIRM;

    char * uriWithoutQuery = NULL;
    char * query = NULL;
    OCStackResult requestResult = OC_STACK_ERROR;

    requestResult = getQueryFromUri(requestInfo->info.resourceUri, &query, &uriWithoutQuery);

    if (requestResult != OC_STACK_OK || !uriWithoutQuery)
    {
        OIC_LOG_V(ERROR, TAG, "getQueryFromUri() failed with OC error code %d\n", requestResult);
        return;
    }
    OIC_LOG_V(INFO, TAG, "URI without query: %s", uriWithoutQuery);
    OIC_LOG_V(INFO, TAG, "Query : %s", query);

    OCServerProtocolRequest serverRequest = { 0 };
    if (strlen(uriWithoutQuery) < MAX_URI_LENGTH)
    {
        OICStrcpy(serverRequest.resourceUrl, sizeof(serverRequest.resourceUrl), uriWithoutQuery);
        OICFree(uriWithoutQuery);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "URI length exceeds MAX_URI_LENGTH.");
        OICFree(uriWithoutQuery);
        OICFree(query);
        return;
    }

    if (query)
    {
        if (strlen(query) < MAX_QUERY_LENGTH)
        {
            OICStrcpy(serverRequest.query, sizeof(serverRequest.query), query);
            OICFree(query);
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Query length exceeds MAX_QUERY_LENGTH.");
            OICFree(query);
            return;
        }
    }

    if ((requestInfo->info.payload) && (0 < requestInfo->info.payloadSize))
    {
        serverRequest.payloadFormat = CAToOCPayloadFormat(requestInfo->info.payloadFormat);
        serverRequest.reqTotalSize = requestInfo->info.payloadSize;
        serverRequest.payload = (uint8_t *) OICMalloc(requestInfo->info.payloadSize);
        if (!serverRequest.payload)
        {
            OIC_LOG(ERROR, TAG, "Allocation for payload failed.");
            return;
        }
        memcpy (serverRequest.payload, requestInfo->info.payload,
                requestInfo->info.payloadSize);
    }
    else
    {
        serverRequest.reqTotalSize = 0;
    }

    switch (requestInfo->method)
    {
        case CA_GET:
            serverRequest.method = OC_REST_GET;
            break;
        case CA_PUT:
            serverRequest.method = OC_REST_PUT;
            break;
        case CA_POST:
            serverRequest.method = OC_REST_POST;
            break;
        case CA_DELETE:
            serverRequest.method = OC_REST_DELETE;
            break;
        default:
            OIC_LOG_V(ERROR, TAG, "Received CA method %d not supported", requestInfo->method);
            SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_BAD_REQ,
                                    directResponseType, requestInfo->info.numOptions,
                                    requestInfo->info.options, requestInfo->info.token,
                                    requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                    CA_RESPONSE_DATA);
            OICFree(serverRequest.payload);
            return;
    }

    OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)requestInfo->info.token,
                   requestInfo->info.tokenLength);

    serverRequest.tokenLength = requestInfo->info.tokenLength;
    if (serverRequest.tokenLength)
    {
        // Non empty token
        serverRequest.requestToken = (CAToken_t)OICMalloc(requestInfo->info.tokenLength);

        if (!serverRequest.requestToken)
        {
            OIC_LOG(FATAL, TAG, "Allocation for token failed.");
            SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_INTERNAL_SERVER_ERROR,
                                    directResponseType, requestInfo->info.numOptions,
                                    requestInfo->info.options, requestInfo->info.token,
                                    requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                    CA_RESPONSE_DATA);
            OICFree(serverRequest.payload);
            return;
        }
        memcpy(serverRequest.requestToken, requestInfo->info.token, requestInfo->info.tokenLength);
    }

    serverRequest.acceptFormat = CAToOCPayloadFormat(requestInfo->info.acceptFormat);
    if (OC_FORMAT_VND_OCF_CBOR == serverRequest.acceptFormat)
    {
        serverRequest.acceptVersion = requestInfo->info.acceptVersion;
    }

    if (requestInfo->info.type == CA_MSG_CONFIRM)
    {
        serverRequest.qos = OC_HIGH_QOS;
    }
    else
    {
        serverRequest.qos = OC_LOW_QOS;
    }
    // CA does not need the following field
    // Are we sure CA does not need them? how is it responding to multicast
    serverRequest.delayedResNeeded = 0;

    serverRequest.coapID = requestInfo->info.messageId;

    CopyEndpointToDevAddr(endPoint, &serverRequest.devAddr);

    // copy vendor specific header options
    uint8_t tempNum = (requestInfo->info.numOptions);

    // Assume no observation requested and it is a pure GET.
    // If obs registration/de-registration requested it'll be fetched from the
    // options in GetObserveHeaderOption()
    serverRequest.observationOption = OC_OBSERVE_NO_OPTION;

    GetObserveHeaderOption(&serverRequest.observationOption, requestInfo->info.options, &tempNum);
    if (requestInfo->info.numOptions > MAX_HEADER_OPTIONS)
    {
        OIC_LOG(ERROR, TAG,
                "The request info numOptions is greater than MAX_HEADER_OPTIONS");
        SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_BAD_OPT,
                                directResponseType, requestInfo->info.numOptions,
                                requestInfo->info.options, requestInfo->info.token,
                                requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                CA_RESPONSE_DATA);
        OICFree(serverRequest.payload);
        OICFree(serverRequest.requestToken);
        return;
    }
    serverRequest.numRcvdVendorSpecificHeaderOptions = tempNum;
    if (serverRequest.numRcvdVendorSpecificHeaderOptions && requestInfo->info.options)
    {
        memcpy(&(serverRequest.rcvdVendorSpecificHeaderOptions), requestInfo->info.options,
               sizeof(CAHeaderOption_t) * tempNum);
    }

    requestResult = HandleStackRequests (&serverRequest);

    if (requestResult == OC_STACK_SLOW_RESOURCE)
    {
        // Send ACK to client as precursor to slow response
        if (requestInfo->info.type == CA_MSG_CONFIRM)
        {
            SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_EMPTY,
                                    CA_MSG_ACKNOWLEDGE,0, NULL, NULL, 0, NULL,
                                    CA_RESPONSE_DATA);
        }
    }
    if (requestResult == OC_STACK_RESOURCE_ERROR
            && serverRequest.observationOption == OC_OBSERVE_REGISTER)
    {
        OIC_LOG(ERROR, TAG, "Observe Registration failed due to resource error");
    }
    else if (!OCResultToSuccess(requestResult))
    {
        OIC_LOG_V(ERROR, TAG, "HandleStackRequests failed. error: %d", requestResult);

        CAResponseResult_t stackResponse = OCToCAStackResult(requestResult, serverRequest.method);

        SendDirectStackResponse(endPoint, requestInfo->info.messageId, stackResponse,
                                directResponseType, requestInfo->info.numOptions,
                                requestInfo->info.options, requestInfo->info.token,
                                requestInfo->info.tokenLength, requestInfo->info.resourceUri,
                                CA_RESPONSE_DATA);
    }
    // requestToken is fed to HandleStackRequests, which then goes to AddServerRequest.
    // The token is copied in there, and is thus still owned by this function.
    OICFree(serverRequest.payload);
    OICFree(serverRequest.requestToken);
    OIC_LOG(INFO, TAG, "Exit OCHandleRequests");
}

/*
 * This function sends out Direct Stack Responses. These are responses that are not coming
 * from the application entity handler. These responses have no payload and are usually ACKs,
 * RESETs or some error conditions that were caught by the stack.
 */
OCStackResult SendDirectStackResponse(const CAEndpoint_t* endPoint, const uint16_t coapID,
        const CAResponseResult_t responseResult, const CAMessageType_t type,
        const uint8_t numOptions, const CAHeaderOption_t *options,
        CAToken_t token, uint8_t tokenLength, const char *resourceUri,
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
        CARequestInfo_t reqInfo = {.method = CA_POST };
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

/**
 * Handler function to execute stack requests
 *
 * @param protocolRequest      Pointer to the protocol requests from server.
 *
 * @return
 *     ::OCStackResult
 */
OCStackResult HandleStackRequests(OCServerProtocolRequest * protocolRequest)
{
    OIC_LOG(INFO, TAG, "Entering HandleStackRequests (OCStack Layer)");
    OCStackResult result = OC_STACK_ERROR;
    if (!protocolRequest)
    {
        OIC_LOG(ERROR, TAG, "protocolRequest is NULL");
        return OC_STACK_INVALID_PARAM;
    }

    OCServerRequest * request = GetServerRequestUsingToken(protocolRequest->requestToken,
            protocolRequest->tokenLength);
    if (!request)
    {
        OIC_LOG(INFO, TAG, "This is a new Server Request");
        result = AddServerRequest(&request, protocolRequest->coapID,
                protocolRequest->delayedResNeeded, 0, protocolRequest->method,
                protocolRequest->numRcvdVendorSpecificHeaderOptions,
                protocolRequest->observationOption, protocolRequest->qos,
                protocolRequest->query, protocolRequest->rcvdVendorSpecificHeaderOptions,
                protocolRequest->payloadFormat, protocolRequest->payload,
                protocolRequest->requestToken, protocolRequest->tokenLength,
                protocolRequest->resourceUrl, protocolRequest->reqTotalSize,
                protocolRequest->acceptFormat, protocolRequest->acceptVersion,
                &protocolRequest->devAddr);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(ERROR, TAG, "Error adding server request");
            return result;
        }

        if(!request)
        {
            OIC_LOG(ERROR, TAG, "Out of Memory");
            return OC_STACK_NO_MEMORY;
        }

        if(!protocolRequest->reqMorePacket)
        {
            request->requestComplete = 1;
        }
    }
    else
    {
        OIC_LOG(INFO, TAG, "This is either a repeated or blocked Server Request");
    }

    if (request->requestComplete)
    {
        OIC_LOG(INFO, TAG, "This Server Request is complete");
        ResourceHandling resHandling = OC_RESOURCE_VIRTUAL;
        OCResource *resource = NULL;
        result = DetermineResourceHandling (request, &resHandling, &resource);
        if (result == OC_STACK_OK)
        {
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

OCStackResult ProcessRequest(ResourceHandling resHandling, OCResource *resource, OCServerRequest *request)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    OCStackResult ret = OC_STACK_OK;

    switch (resHandling)
    {
        case OC_RESOURCE_VIRTUAL:
        {
            ret = HandleVirtualResource (request, resource);
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
            ret = HandleResourceWithEntityHandler (request, resource);
            break;
        }
        case OC_RESOURCE_COLLECTION_WITH_ENTITYHANDLER:
        {
            ret = HandleResourceWithEntityHandler (request, resource);
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

