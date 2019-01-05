#include "oocf_host_client.h"

#include <errno.h>

OCStackResult OC_CALL oocf_send_request(OCDoHandle *handle,
					OCMethod method,
					const char *requestUri,
					const OCDevAddr *destination,
					OCPayload* payload,
					OCConnectivityType connectivityType,
					OCQualityOfService qos,
					OCCallbackData *cbData,
					OCHeaderOption *options,
					uint8_t numOptions) EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    return OCDoRequest(handle, method, requestUri,destination, payload,
		       connectivityType, qos, cbData, options, numOptions);
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

/**
 * Map OCQualityOfService to CAMessageType (CaAP msg type).
 *
 * @param qos Input qos.
 *
 * @return CA message type for a given qos.
 */
CAMessageType_t qualityOfServiceToCoAPMessageType(OCQualityOfService qos)
{
    switch (qos)
    {
        case OC_HIGH_QOS:
            return CA_MSG_CONFIRM;
        case OC_LOW_QOS:
        case OC_MEDIUM_QOS:
        case OC_NA_QOS:
        default:
            return CA_MSG_NONCONFIRM;
    }
}

/**
 * Generate handle of OCDoResource invocation for callback management.
 *
 * @return Generated OCDoResource handle.
 */
LOCAL OCDoHandle GenerateInvocationHandle(void)
{
    OCDoHandle handle = NULL;
    // Generate token here, it will be deleted when the transaction is deleted
    handle = (OCDoHandle) OICMalloc(sizeof(uint8_t[CA_MAX_TOKEN_LEN]));
    if (handle)
    {
        if (!OCGetRandomBytes((uint8_t*)handle, sizeof(uint8_t[CA_MAX_TOKEN_LEN])))
        {
            OICFree(handle);
            return NULL;
        }
    } else {
	OIC_LOG_V(ERROR, TAG, "%s: OICMalloc error", __func__);
    }

    return handle;
}

void process_options(struct CARequestInfo requestInfo, uint8_t numOptions, OCHeaderOption *options)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}


/**
 * Discover or Perform requests on a specified resource
 */
OCStackResult OC_CALL OCDoRequest(OCDoHandle *handle,
                                  OCMethod method,
                                  const char *requestUri,
                                  const OCDevAddr *destination,
                                  OCPayload* payload,
                                  OCConnectivityType connectivityType,
                                  OCQualityOfService qos,
                                  OCCallbackData *cbData,
                                  OCHeaderOption *options,
                                  uint8_t numOptions) EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY, request uri: %s", __func__, requestUri);

    // Validate input parameters
    VERIFY_NON_NULL(cbData, FATAL, OC_STACK_INVALID_CALLBACK);
    VERIFY_NON_NULL(cbData->cb, FATAL, OC_STACK_INVALID_CALLBACK);

    OCStackResult result = OC_STACK_ERROR;
    CAResult_t caResult;
    uint8_t *token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    ClientCB *clientCB = NULL;
    OCDoHandle resHandle = NULL;
    CAEndpoint_t endpoint = {.adapter = CA_DEFAULT_ADAPTER};
    OCDevAddr tmpDevAddr = { OC_DEFAULT_ADAPTER };
    uint32_t ttl = 0;
    OCTransportAdapter adapter;
    OCTransportFlags flags;
    // the request contents are put here
    struct CARequestInfo requestInfo = {.method = CA_GET};
    // requestUri  will be parsed into the following three variables
    OCDevAddr *devAddr = NULL;
    char *resourceUri = NULL;
    char *resourceType = NULL;
    bool isProxyRequest = false;

    /*
     * Support original behavior with address on resourceUri argument.
     */
    adapter = (OCTransportAdapter)(connectivityType >> CT_ADAPTER_SHIFT);
    flags = (OCTransportFlags)(connectivityType & CT_MASK_FLAGS);
    OIC_LOG_V(DEBUG, TAG, "%s: adapter = %d, flags = %d", __func__, adapter, flags);

    if (requestUri)
    {
        result = ParseRequestUri(requestUri, adapter, flags, &devAddr, &resourceUri, &resourceType);
        if (result != OC_STACK_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "Unable to parse uri: %s", requestUri);
            goto exit;
        }
    }
    else
    {
        isProxyRequest = checkProxyUri(options, numOptions);
        if (!isProxyRequest)
        {
            OIC_LOG(ERROR, TAG, "Request doesn't contain RequestURI/Proxy URI");
            goto exit;
        }
    }

    switch (method)
    {
    case OC_REST_GET:
    case OC_REST_OBSERVE:
    case OC_REST_OBSERVE_ALL:
        requestInfo.method = CA_GET;
        break;
    case OC_REST_PUT:
        requestInfo.method = CA_PUT;
        break;
    case OC_REST_POST:
        requestInfo.method = CA_POST;
        break;
    case OC_REST_DELETE:
        requestInfo.method = CA_DELETE;
        break;
    case OC_REST_DISCOVER:
        // intentional fall through don't add break
#ifdef WITH_PRESENCE
    case OC_REST_PRESENCE:
#endif
        if (destination || devAddr)
        {
            requestInfo.isMulticast = false;
            endpoint.flags &= ~CA_MULTICAST;
	    OIC_LOG_V(DEBUG, TAG, "Request is UNICAST");
        }
        else
        {
            tmpDevAddr.adapter = adapter;
            tmpDevAddr.flags |= CA_MULTICAST;
            destination = &tmpDevAddr;
            requestInfo.isMulticast = true;
            endpoint.flags |= CA_MULTICAST;
            qos = OC_LOW_QOS;
	    OIC_LOG_V(DEBUG, TAG, "Request is MULTICAST");
        }
        // OC_REST_DISCOVER: CA_DISCOVER will become GET and isMulticast.
        // OC_REST_PRESENCE: Since "presence" is a stack layer only implementation.
        //                   replacing method type with GET.
        requestInfo.method = CA_GET;
        break;
    default:
	OIC_LOG_V(ERROR, TAG, "%s: INVALID METHOD: %d", __func__, method);
        result = OC_STACK_INVALID_METHOD;
        goto exit;
    }

    if (!devAddr && !destination)
    {
        OIC_LOG(DEBUG, TAG, "no devAddr and no destination");
        result = OC_STACK_INVALID_PARAM;
        goto exit;
    }

    /* If not original behavior, use destination argument */
    if (destination && !devAddr)
    {
        devAddr = (OCDevAddr *)OICMalloc(sizeof (OCDevAddr));
        if (!devAddr)
        {
	    OIC_LOG_V(ERROR, TAG, "%s: OICMalloc error", __func__);
            result = OC_STACK_NO_MEMORY;
            goto exit;
        }
        OIC_LOG(DEBUG, TAG, "new devAddr is set as destination");
        *devAddr = *destination;
    }

    if (devAddr)
    {
        OIC_LOG_V(DEBUG, TAG, "remoteId of devAddr : %s", devAddr->remoteId);

        if ((devAddr->remoteId[0] == 0)
            && destination
            && (destination->remoteId[0] != 0))
        {
            OIC_LOG_V(DEBUG, TAG, "Copying remoteId from destination parameter: %s", destination->remoteId);
            OICStrcpy(devAddr->remoteId, sizeof(devAddr->remoteId), destination->remoteId);
        }
    }

    resHandle = GenerateInvocationHandle();
    if (!resHandle)
    {
	OIC_LOG_V(ERROR, TAG, "%s: GenerateInvocationHandle error: %d", __func__, resHandle);
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    caResult = CAGenerateToken(&token, tokenLength);
    if (caResult != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "CAGenerateToken error");
        result= OC_STACK_ERROR;
        goto exit;
    }

    // fill in request data
    requestInfo.info.type = qualityOfServiceToCoAPMessageType(qos);
    requestInfo.info.token = token;
    requestInfo.info.tokenLength = tokenLength;

    /* set CoAP options */
    if ((method == OC_REST_OBSERVE) || (method == OC_REST_OBSERVE_ALL))
    {
        result = CreateObserveHeaderOption (&(requestInfo.info.options),
                                    options, numOptions, OC_OBSERVE_REGISTER);
        if (result != OC_STACK_OK)
        {
	    OIC_LOG_V(ERROR, TAG, "%s: CreateObserveHeaderOption error: %d", __func__, result);
            goto exit;
        }
        requestInfo.info.numOptions = numOptions + 1;
    }
    else
    {
        OIC_LOG_V(DEBUG, TAG, "%s: process options", __func__);
        process_options(requestInfo, numOptions, options);
        // FIXME: put in a fn

        // Check if accept format and accept version have been set.
        uint16_t acceptVersion = OCF_VERSION_1_0_0;
        uint16_t acceptFormat = COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR;
        bool IsAcceptVersionSet = false;
        bool IsAcceptFormatSet = false;
        // Check settings of version option and content format.
        if (numOptions > 0 && options)
        {
            for (uint8_t i = 0; i < numOptions; i++)
            {
                if (OCF_ACCEPT_CONTENT_FORMAT_VERSION == options[i].optionID
                        && sizeof(uint16_t) == options[i].optionLength)
                {
                    acceptVersion = (options[i].optionData[1] << 8) + options[i].optionData[0];
                    IsAcceptVersionSet = true;
                }
                else if (COAP_OPTION_ACCEPT == options[i].optionID)
                {
                    if (sizeof(uint8_t) == options[i].optionLength)
                    {
                        acceptFormat = CAConvertFormat(*(uint8_t*)options[i].optionData);
                        IsAcceptFormatSet = true;
                    }
                    else if (sizeof(uint16_t) == options[i].optionLength)
                    {
                        acceptFormat = CAConvertFormat(
                                (options[i].optionData[1] << 8) + options[i].optionData[0]);
                        IsAcceptFormatSet = true;
                    }
                    else
                    {
                        acceptFormat = CA_FORMAT_UNSUPPORTED;
                        IsAcceptFormatSet = true;
                        OIC_LOG_V(DEBUG, TAG, "option has an unsupported format");
                    }
                }
            }
        }

        if (!IsAcceptVersionSet && !IsAcceptFormatSet)
        {
            requestInfo.info.numOptions = numOptions + 2;
        }
        else if ((IsAcceptFormatSet &&
                CA_FORMAT_APPLICATION_VND_OCF_CBOR == acceptFormat &&
                !IsAcceptVersionSet) || (IsAcceptVersionSet && !IsAcceptFormatSet))
        {
            requestInfo.info.numOptions = numOptions + 1;
        }
        else
        {
            requestInfo.info.numOptions = numOptions;
        }

        requestInfo.info.options = (CAHeaderOption_t*) OICCalloc(requestInfo.info.numOptions,
                sizeof(CAHeaderOption_t));
        if (NULL == requestInfo.info.options)
        {
            OIC_LOG(ERROR, TAG, "Calloc failed");
            result = OC_STACK_NO_MEMORY;
            goto exit;
        }
        memcpy(requestInfo.info.options, (CAHeaderOption_t*) options,
               numOptions * sizeof(CAHeaderOption_t));

        if (!IsAcceptVersionSet && !IsAcceptFormatSet)
        {
            // Append accept format and accept version to the options.
            SetHeaderOption(requestInfo.info.options, numOptions, CA_OPTION_ACCEPT, &acceptFormat,
                    sizeof(uint16_t));
            SetHeaderOption(requestInfo.info.options, numOptions + 1, OCF_ACCEPT_CONTENT_FORMAT_VERSION,
                    &acceptVersion, sizeof(uint16_t));
        }
        else if (IsAcceptFormatSet && CA_FORMAT_APPLICATION_VND_OCF_CBOR == acceptFormat
                && !IsAcceptVersionSet)
        {
            // Append accept version to the options.
            SetHeaderOption(requestInfo.info.options, numOptions, OCF_ACCEPT_CONTENT_FORMAT_VERSION,
                    &acceptVersion, sizeof(uint16_t));
        }
        else if (IsAcceptVersionSet && OCF_VERSION_1_0_0 <= acceptVersion && !IsAcceptFormatSet)
        {
            // Append accept format to the options.
            SetHeaderOption(requestInfo.info.options, numOptions, CA_OPTION_ACCEPT, &acceptFormat,
                    sizeof(uint16_t));
        }
    }

    CopyDevAddrToEndpoint(devAddr, &endpoint);

    if (payload)
    {
        uint16_t payloadVersion = OCF_VERSION_1_0_0;
        CAPayloadFormat_t payloadFormat = CA_FORMAT_APPLICATION_VND_OCF_CBOR;
        // Check version option settings
        if (numOptions > 0 && options)
        {
            for (uint8_t i = 0; i < numOptions; i++)
            {
                if (OCF_CONTENT_FORMAT_VERSION == options[i].optionID
                        && sizeof(uint16_t) == options[i].optionLength)
                {
                    payloadVersion = (options[i].optionData[1] << 8) + options[i].optionData[0];
                }
                else if (COAP_OPTION_CONTENT_FORMAT == options[i].optionID)
                {
                    if (sizeof(uint8_t) == options[i].optionLength)
                    {
                        payloadFormat = CAConvertFormat(*(uint8_t*) options[i].optionData);
                    }
                    else if (sizeof(uint16_t) == options[i].optionLength)
                    {
                        payloadFormat = CAConvertFormat(
                                (options[i].optionData[1] << 8) + options[i].optionData[0]);
                    }
                    else
                    {
                        payloadFormat = CA_FORMAT_UNSUPPORTED;
                        OIC_LOG_V(DEBUG, TAG, "option has an unsupported format");
                    }
                }
            }
        }

        requestInfo.info.payloadFormat = payloadFormat;
        if (CA_FORMAT_APPLICATION_CBOR == payloadFormat && payloadVersion)
        {
            payloadVersion = 0;
        }
        requestInfo.info.payloadVersion = payloadVersion;

        if ((result =
            OCConvertPayload(payload, CAToOCPayloadFormat(requestInfo.info.payloadFormat),
                            &requestInfo.info.payload, &requestInfo.info.payloadSize))
                != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "Failed to create CBOR Payload");
	    OIC_LOG_V(ERROR, TAG, "%s: OCConvertPayload error: %d", __func__, result);
            goto exit;
        }
    }
    else
    {
        requestInfo.info.payload = NULL;
        requestInfo.info.payloadSize = 0;
        requestInfo.info.payloadFormat = CA_FORMAT_UNDEFINED;
    }

    // prepare for response
#ifdef WITH_PRESENCE
    if (method == OC_REST_PRESENCE)
    {
        char *presenceUri = NULL;
        result = OCPreparePresence(&endpoint, &presenceUri,
                                   requestInfo.isMulticast);
        if (OC_STACK_OK != result)
        {
	    OIC_LOG_V(ERROR, TAG, "%s: OCPreparePresence error: %d", __func__, result);
            goto exit;
        }

        // Assign full presence uri as coap://ip:port/oic/ad to add to callback list.
        // Presence notification will form a canonical uri to
        // look for callbacks into the application.
        if (resourceUri)
        {
            OICFree(resourceUri);
        }
        resourceUri = presenceUri;
    }
#endif

    // update resourceUri onto requestInfo after check presence uri
    requestInfo.info.resourceUri = resourceUri;

    ttl = GetTicks(MAX_CB_TIMEOUT_SECONDS * MILLISECONDS_PER_SECOND);
    result = AddClientCB(&clientCB, cbData, requestInfo.info.type, token, tokenLength,
                         requestInfo.info.options, requestInfo.info.numOptions,
                         requestInfo.info.payload, requestInfo.info.payloadSize,
                         requestInfo.info.payloadFormat, &resHandle, method,
                         devAddr, resourceUri, resourceType, ttl);

    if (OC_STACK_OK != result)
    {
	OIC_LOG_V(ERROR, TAG, "%s: AddClientCB error: %d", __func__, result);
        goto exit;
    }

    devAddr = NULL;       // Client CB list entry now owns it
    resourceUri = NULL;   // Client CB list entry now owns it
    resourceType = NULL;  // Client CB list entry now owns it

#ifdef WITH_PRESENCE
    if (method == OC_REST_PRESENCE)
    {
        OIC_LOG(ERROR, TAG, "AddClientCB for presence done.");

        if (handle)
        {
            *handle = resHandle;
        }

        goto exit;
    }
#endif

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
    /* Check whether we should assert role certificates before making this request. */
    bool isEndpointSecure = ((endpoint.flags & CA_SECURE) != 0);
    bool requestContainsRolesUri = (strcmp(requestInfo.info.resourceUri, OIC_RSRC_ROLES_URI) == 0);
    bool requestContainsDoxmResource = (strcmp(requestInfo.info.resourceUri, OIC_RSRC_DOXM_URI) == 0);
    bool isTcpConnectivityType = (CT_ADAPTER_TCP == connectivityType);
    bool requestContainsKeepaliveUri = (strcmp(requestInfo.info.resourceUri, OC_RSRVD_KEEPALIVE_URI) == 0);

    if (isEndpointSecure && (isProxyRequest ||
        (!requestContainsRolesUri && !requestContainsDoxmResource &&
         isTcpConnectivityType && !requestContainsKeepaliveUri)))
    {
        CASecureEndpoint_t sep;
        CAResult_t caRes = CAGetSecureEndpointData(&endpoint, &sep);
        if (caRes != CA_STATUS_OK)
        {
            /*
             * This is a secure request but we do not have a secure connection with
             * this peer, try to assert roles. There's no way to tell if the peer
             * uses certificates without asking, so just try to assert roles.  If
             * it fails, that's OK, roles will get asserted "automatically" when PSK
             * credentials are used.
             */
            if (!isProxyRequest)
            {
                OIC_LOG_V(DEBUG, TAG, "%s: going to try to assert roles before doing request to %s ",
                          __func__, requestInfo.info.resourceUri);
            }

            OCDevAddr da;
            CopyEndpointToDevAddr(&endpoint, &da);
            OCStackResult assertResult = OCAssertRoles((void*)ASSERT_ROLES_CTX, &da,
                                                       &assertRolesCB);
            if (assertResult == OC_STACK_OK)
            {
                OIC_LOG_V(DEBUG, TAG, "%s: Call to OCAssertRoles succeeded", __func__);
            }
            else if (assertResult == OC_STACK_INCONSISTENT_DB)
            {
                OIC_LOG_V(DEBUG, TAG, "%s: No role certificates to assert", __func__);
            }
            else
            {
                OIC_LOG_V(DEBUG, TAG, "%s: Call to OCAssertRoles failed", __func__);
            }

            /*
             * We don't block waiting for OCAssertRoles to complete.  Because the roles assertion
             * request is queued before the actual request, it will happen first.  If it fails, we
             * log the error, but don't retry; the actually request made to OCDorequest may or may
             * not fail (with permission denied), the caller can decide whether to retry.
             */
        }

    }
#endif // __WITH_DTLS__ || __WITH_TLS__


    // send request
    result = OCSendRequest(&endpoint, &requestInfo);
    if (OC_STACK_OK != result)
    {
	OIC_LOG_V(ERROR, TAG, "%s: OCSendRequest error: %d", __func__, result);
	goto exit;
    }

    if (handle)
    {
        *handle = resHandle;
    }

exit:
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCDoResource error");
        DeleteClientCB(clientCB);
        CADestroyToken(token);
        if (handle)
        {
            *handle = NULL;
        }
        OICFree(resHandle);
    }

    OICFree(requestInfo.info.payload);
    OICFree(devAddr);
    OICFree(resourceUri);
    OICFree(resourceType);
    OICFree(requestInfo.info.options);
    return result;
}

/**
 * Discover or Perform requests on a specified resource
 * Deprecated: use OCDoRequest instead
 */
OCStackResult OC_CALL OCDoResource(OCDoHandle *handle,
                                   OCMethod method,
                                   const char *requestUri,
                                   const OCDevAddr *destination,
                                   OCPayload* payload,
                                   OCConnectivityType connectivityType,
                                   OCQualityOfService qos,
                                   OCCallbackData *cbData,
                                   OCHeaderOption *options,
                                   uint8_t numOptions)
/* EXPORT (deprecated, do not expose) */
{
    OIC_TRACE_BEGIN(%s:OCDoResource, TAG);
    OCStackResult ret = OCDoRequest(handle, method, requestUri,destination, payload,
                connectivityType, qos, cbData, options, numOptions);
    OIC_TRACE_END();

    // This is the owner of the payload object, so we free it
    OCPayloadDestroy(payload);
    return ret;
}

/**
 * Ensure the accept header option is set appropriatly before sending the requests and routing
 * header option is updated with destination.
 *
 * @param object CA remote endpoint.
 * @param requestInfo CA request info.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 * src: ocstack.c
 */
OCStackResult OCSendRequest(const CAEndpoint_t *dest_ep, struct CARequestInfo *requestInfo)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL(dest_ep, FATAL, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(requestInfo, FATAL, OC_STACK_INVALID_PARAM);
    OIC_TRACE_BEGIN(%s:OCSendRequest, TAG);

    OIC_LOG_V(DEBUG, TAG,
              "%s sending %s %s %s %s", __func__,
              (dest_ep->flags & CA_IPV6) ? "IPV6" : "",
              (dest_ep->flags & CA_IPV4) ? "IPV4" : "",
              (dest_ep->flags & CA_SECURE) ? "secure " : "insecure ",
              (dest_ep->flags & CA_MULTICAST) ? "multicast" : "unicast");

#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
    OCStackResult rmResult = RMAddInfo(dest_ep->routeData, requestInfo, true, NULL);
    if (OC_STACK_OK != rmResult)
    {
        OIC_LOG(ERROR, TAG, "Add destination option failed");
        OIC_TRACE_END();
        return rmResult;
    }
#endif

    // TODO: We might need to remove acceptFormat and acceptVersion fields in requestinfo->info
    // at a later stage to avoid duplication.
    uint16_t acceptVersion = OCF_VERSION_1_0_0;
    CAPayloadFormat_t acceptFormat = CA_FORMAT_APPLICATION_VND_OCF_CBOR;
    // Check settings of version option and content format.
    if (requestInfo->info.numOptions > 0 && requestInfo->info.options)
    {
        for (uint8_t i = 0; i < requestInfo->info.numOptions; i++)
        {
            if (OCF_ACCEPT_CONTENT_FORMAT_VERSION == requestInfo->info.options[i].optionID
                    && sizeof(uint16_t) == requestInfo->info.options[i].optionLength)
            {
                acceptVersion = (requestInfo->info.options[i].optionData[1] << 8)
                        + requestInfo->info.options[i].optionData[0];
            }
            else if (COAP_OPTION_ACCEPT == requestInfo->info.options[i].optionID)
            {
                if (sizeof(uint8_t) == requestInfo->info.options[i].optionLength)
                {
                    acceptFormat = CAConvertFormat(
                            *(uint8_t*) requestInfo->info.options[i].optionData);
                }
                else if (sizeof(uint16_t) == requestInfo->info.options[i].optionLength)
                {
                    acceptFormat = CAConvertFormat(
                            (requestInfo->info.options[i].optionData[1] << 8)
                                    + requestInfo->info.options[i].optionData[0]);
                }
                else
                {
                    acceptFormat = CA_FORMAT_UNSUPPORTED;
                    OIC_LOG_V(DEBUG, TAG, "option has an unsupported format");
                }
            }
        }
    }

    if (CA_FORMAT_UNDEFINED == requestInfo->info.acceptFormat)
    {
        requestInfo->info.acceptFormat = acceptFormat;
    }
    if (CA_FORMAT_APPLICATION_CBOR == requestInfo->info.acceptFormat && acceptVersion)
    {
        acceptVersion = 0;
    }
    requestInfo->info.acceptVersion = acceptVersion;

    CAResult_t result = CASendRequest(dest_ep, requestInfo);
    if (CA_STATUS_OK != result)
    {
	/* 13 = uninitialized */
        OIC_LOG_V(ERROR, TAG, "CASendRequest failed with CA error %u", result);
        OIC_TRACE_END();
        return CAResultToOCResult(result);
    }
    OIC_TRACE_END();
    return OC_STACK_OK;
}

void OC_CALL OCHandleResponse(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_TRACE_MARK(%s:OCHandleResponse:%s, TAG, responseInfo->info.resourceUri);

    OCStackApplicationResult appFeedback = OC_STACK_DELETE_TRANSACTION;

#ifdef WITH_PRESENCE
    if(responseInfo->info.resourceUri &&
        strcmp(responseInfo->info.resourceUri, OC_RSRVD_PRESENCE_URI) == 0)
    {
        HandlePresenceResponse(endPoint, responseInfo);
        return;
    }
#endif

    ClientCB *cbNode = GetClientCBUsingToken(responseInfo->info.token,
                                             responseInfo->info.tokenLength);
    if(cbNode)
    {
        OIC_LOG(INFO, TAG, "There is a cbNode associated with the response token");

        // check obs header option
        bool obsHeaderOpt = false;
        CAHeaderOption_t *options = responseInfo->info.options;
        for (uint8_t i = 0; i < responseInfo->info.numOptions; i++)
        {
            if (options && (options[i].optionID == COAP_OPTION_OBSERVE))
            {
                obsHeaderOpt = true;
                break;
            }
        }

        if(responseInfo->result == CA_EMPTY)
        {
            OIC_LOG(INFO, TAG, "Receiving A ACK/RESET for this token");
            // We do not have a case for the client to receive a RESET
            if(responseInfo->info.type == CA_MSG_ACKNOWLEDGE)
            {
                //This is the case of receiving an ACK on a request to a slow resource!
                OIC_LOG(INFO, TAG, "This is a pure ACK");
                //TODO: should we inform the client
                //      app that at least the request was received at the server?
            }
        }
        else if (CA_REQUEST_ENTITY_INCOMPLETE == responseInfo->result)
        {
            OIC_LOG(INFO, TAG, "Wait for block transfer finishing.");
            return;
        }
        else if (CA_RETRANSMIT_TIMEOUT == responseInfo->result
                || CA_NOT_ACCEPTABLE == responseInfo->result)
        {
            if (CA_RETRANSMIT_TIMEOUT == responseInfo->result)
            {
                OIC_LOG(INFO, TAG, "Receiving A Timeout for this token");
                OIC_LOG(INFO, TAG, "Calling into application address space");
            }
            else
            {
                OIC_LOG(INFO, TAG, "Server doesn't support the requested payload format");
                OIC_LOG(INFO, TAG, "Calling into application address space");
            }

            // Check if it is the case that a OCF client connects to a OIC server.
            // If so, reissue request using OIC format
            if (CA_NOT_ACCEPTABLE == responseInfo->result &&
                CA_FORMAT_UNDEFINED == responseInfo->info.payloadFormat)
            {
		OIC_LOG(DEBUG, TAG, "Response result NOT_ACCEPTABLE, format UNDEFINED");
                struct CARequestInfo requestInfo = { .method = CA_GET };

                switch (cbNode->method)
                {
                    case OC_REST_GET:
                    case OC_REST_OBSERVE:
                    case OC_REST_OBSERVE_ALL:
                        requestInfo.method = CA_GET;
                        break;
                    case OC_REST_PUT:
                        requestInfo.method = CA_PUT;
                        break;
                    case OC_REST_POST:
                        requestInfo.method = CA_POST;
                        break;
                    case OC_REST_DELETE:
                        requestInfo.method = CA_DELETE;
                        break;
                    case OC_REST_DISCOVER:
#ifdef WITH_PRESENCE
                    case OC_REST_PRESENCE:
#endif
                        // OC_REST_DISCOVER: CA_DISCOVER will become GET
                        // OC_REST_PRESENCE: Since "presence" is a stack layer only implementation.
                        //                   replacing method type with GET.
                        requestInfo.method = CA_GET;
                        break;
                    default:
			/* GAR: OC_REST_NOMETHOD???  */
                        goto proceed;
                }

                CAInfo_t requestData = {.type = cbNode->type};
                requestData.tokenLength = cbNode->tokenLength;
                requestData.token = (uint8_t*) OICMalloc(requestData.tokenLength);
                if (!requestData.token)
                {
                    OIC_LOG(ERROR, TAG, "Out of memory");
                    goto proceed;
                }
                memcpy(requestData.token, cbNode->token, requestData.tokenLength);

                if (!cbNode->options || !cbNode->numOptions)
                {
                    OIC_LOG (INFO, TAG, "No options present in cbNode");
                    requestData.options = NULL;
                    requestData.numOptions = 0;
                }
                else
                {
                    requestData.options = (CAHeaderOption_t *) OICCalloc(cbNode->numOptions,
                            sizeof(CAHeaderOption_t));
                    if (!requestData.options)
                    {
                        OIC_LOG(ERROR, TAG, "Out of memory");
                        OICFree(requestData.token);
                        goto proceed;
                    }
                    memcpy(requestData.options, cbNode->options,
                            sizeof(CAHeaderOption_t) * cbNode->numOptions);

                    requestData.numOptions = cbNode->numOptions;
                }
                if (!cbNode->payload || !cbNode->payloadSize)
                {
                    OIC_LOG (INFO, TAG, "No payload present in cbNode");
                    requestData.payload = NULL;
                    requestData.payloadSize = 0;
                }
                else
                {
                    requestData.payload = (CAPayload_t) OICCalloc(1, cbNode->payloadSize);
                    if (!requestData.payload)
                    {
                        OIC_LOG(ERROR, TAG, "out of memory");
                        OICFree(requestData.token);
                        OICFree(requestData.options);
                        goto proceed;
                    }
                    memcpy(requestData.payload, cbNode->payload, cbNode->payloadSize);
                    requestData.payloadSize = cbNode->payloadSize;
                }
                requestData.payloadFormat = CA_FORMAT_APPLICATION_CBOR;
                requestData.acceptFormat = CA_FORMAT_APPLICATION_CBOR; //?
                requestData.resourceUri = OICStrdup(cbNode->requestUri);

                requestInfo.info = requestData;

                // send request
                OCStackResult result = OCSendRequest(endPoint, &requestInfo);
                if (OC_STACK_OK == result)
                {
		    OIC_LOG_V(DEBUG, TAG, "Resending request with Accept=CBOR (OIC)");
                    return;
                }
                else
                {
                    OIC_LOG(ERROR, TAG, "Re-send request failed");
                    OICFree(requestData.token);
                    OICFree(requestData.options);
                    OICFree(requestData.payload);
                }
            }

        proceed:;
            OCClientResponse *response = (OCClientResponse *) OICCalloc(1, sizeof(*response));
            if (!response)
            {
                OIC_LOG_V(ERROR, TAG, "[%d] %s: Allocating memory for OCClientResponse failed",
			__LINE__, __func__);
                return;
            }

            response->devAddr.adapter = OC_DEFAULT_ADAPTER;
            CopyEndpointToDevAddr(endPoint, &response->devAddr);
            FixUpClientResponse(response);
            response->resourceUri = responseInfo->info.resourceUri;
            memcpy(response->identity.id, responseInfo->info.identity.id,
                    sizeof(response->identity.id));
            response->identity.id_length = responseInfo->info.identity.id_length;

            response->result = CAResponseToOCStackResult(responseInfo->result);
            cbNode->callBack(cbNode->context,
                    cbNode->handle, response);
            DeleteClientCB(cbNode);
            OICFree(response);
        } /* result != EMPTY, ENTITY_INCOMPLETE, RETRANSMIT_TIMEOUT, NOT_ACCEPTABLE */
        else if ((cbNode->method == OC_REST_OBSERVE || cbNode->method == OC_REST_OBSERVE_ALL)
                && (responseInfo->result == CA_CONTENT) && !obsHeaderOpt)
	    /* response = CONTENT means HTTP 200 "OK" but only used in response to GET requests. */
        {
            OCClientResponse *response = NULL;

            response = (OCClientResponse *)OICCalloc(1, sizeof(*response));
            if (!response)
            {
                OIC_LOG_V(ERROR, TAG, "[%d] %s: Allocating memory for OCClientResponse failed",
			__LINE__, __func__);
                return;
            }

            response->devAddr.adapter = OC_DEFAULT_ADAPTER;
            response->sequenceNumber = MAX_SEQUENCE_NUMBER + 1;
            CopyEndpointToDevAddr(endPoint, &response->devAddr);
            FixUpClientResponse(response);
            response->resourceUri = responseInfo->info.resourceUri;
            memcpy(response->identity.id, responseInfo->info.identity.id,
                                    sizeof (response->identity.id));
            response->identity.id_length = responseInfo->info.identity.id_length;
            response->result = OC_STACK_OK;

            OIC_LOG(DEBUG, TAG, "This is response of observer cancel or observer request fail");

            cbNode->callBack(cbNode->context,
                             cbNode->handle,
                             response);
            DeleteClientCB(cbNode);
            OICFree(response);
        } /* OBSERVE or OBSERVE_ALL */
        else
        {
            OIC_LOG(INFO, TAG, "This is a regular response, A client call back is found");
            OIC_LOG(INFO, TAG, "Calling into application address space");

            OCClientResponse *response = NULL;
            OCPayloadType type = PAYLOAD_TYPE_INVALID;

            response = (OCClientResponse *)OICCalloc(1, sizeof(*response));
            if (!response)
            {
                OIC_LOG_V(ERROR, TAG, "[%d] %s: Allocating memory for response failed",
			__LINE__, __func__);
                return;
            }

            response->devAddr.adapter = OC_DEFAULT_ADAPTER;
            response->sequenceNumber = MAX_SEQUENCE_NUMBER + 1;
            CopyEndpointToDevAddr(endPoint, &response->devAddr);
            FixUpClientResponse(response);
	    response->resourceUri = OICStrdup(responseInfo->info.resourceUri);
            memcpy(response->identity.id, responseInfo->info.identity.id,
                                                sizeof (response->identity.id));
            response->identity.id_length = responseInfo->info.identity.id_length;

            response->result = CAResponseToOCStackResult(responseInfo->result);

            if(responseInfo->info.payload &&
               responseInfo->info.payloadSize)
            {   /* infer payload type from uri */
                if (SRMIsSecurityResourceURI(cbNode->requestUri))
                {
                    type = PAYLOAD_TYPE_SECURITY;
                }
                else if (cbNode->method == OC_REST_DISCOVER)
                {
                    if (strncmp(OC_RSRVD_WELL_KNOWN_URI,cbNode->requestUri,
                                sizeof(OC_RSRVD_WELL_KNOWN_URI) - 1) == 0)
                    {
                        type = PAYLOAD_TYPE_DISCOVERY;
                    }
#ifdef WITH_MQ
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_WELL_KNOWN_MQ_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_DISCOVERY;
                    }
#endif
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_DEVICE_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_PLATFORM_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
#ifdef ROUTING_GATEWAY
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_GATEWAY_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
#endif
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_RD_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
#ifdef TCP_ADAPTER
                    else if (strcmp(cbNode->requestUri, OC_RSRVD_KEEPALIVE_URI) == 0)
                    {
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
#endif
                    else
                    {
                        OIC_LOG_V(ERROR, TAG, "Unknown Payload type in Discovery: %d %s",
                                cbNode->method, cbNode->requestUri);
                        OICFree(response);
                        return;
                    }
                }
                else if (cbNode->method == OC_REST_GET ||
                         cbNode->method == OC_REST_PUT ||
                         cbNode->method == OC_REST_POST ||
                         cbNode->method == OC_REST_OBSERVE ||
                         cbNode->method == OC_REST_OBSERVE_ALL ||
                         cbNode->method == OC_REST_DELETE)
                {
                    if (cbNode->requestUri)
                    {
                        if (0 == strcmp(OC_RSRVD_PLATFORM_URI, cbNode->requestUri))
                        {
                            type = PAYLOAD_TYPE_REPRESENTATION;
                        }
                        else if (0 == strcmp(OC_RSRVD_DEVICE_URI, cbNode->requestUri))
                        {
                            type = PAYLOAD_TYPE_REPRESENTATION;
                        }
                        if (type == PAYLOAD_TYPE_INVALID)
                        {
                            if (responseInfo->info.payloadFormat == CA_FORMAT_UNDEFINED)
                            {
                                OIC_LOG_V(INFO, TAG, "Invalid payload type; assuming PAYLOAD_TYPE_DIAGNOSTIC: %d %s",
                                        cbNode->method, cbNode->requestUri);
                                type = PAYLOAD_TYPE_DIAGNOSTIC;
                            }
                            else
                            {
                                OIC_LOG_V(INFO, TAG, "Invalid payload type; assuming PAYLOAD_TYPE_REPRESENTATION: %d %s",
                                        cbNode->method, cbNode->requestUri);
                                type = PAYLOAD_TYPE_REPRESENTATION;
                            }
                        }
                    }
                    else
                    {
                        OIC_LOG(INFO, TAG, "No Request URI, PROXY URI");
                        type = PAYLOAD_TYPE_REPRESENTATION;
                    }
                }
                else
                {
                    OIC_LOG_V(ERROR, TAG, "Unknown method: %d %s",
                            cbNode->method, cbNode->requestUri);
                    OICFree(response);
                    return;
                }

                // In case of error, still want application to receive the error message.
                if (OCResultToSuccess(response->result) || PAYLOAD_TYPE_REPRESENTATION == type ||
                        PAYLOAD_TYPE_DIAGNOSTIC == type)
                {
                    if (OC_STACK_OK != OCParsePayload(&response->payload,
                            CAToOCPayloadFormat(responseInfo->info.payloadFormat),
                            type,
                            responseInfo->info.payload,
                            responseInfo->info.payloadSize))
                    {
                        OIC_LOG(ERROR, TAG, "Error: OCParsePayload");
                        OCPayloadDestroy(response->payload);
                        OICFree(response);
                        return;
                    }

                    // Check endpoints has link-local ipv6 address.
                    // if there is, map zone-id which parsed from ifindex
#if defined (IP_ADAPTER)
                    if (PAYLOAD_TYPE_DISCOVERY == response->payload->type)
                    {
                        OCDiscoveryPayload *disPayload = (OCDiscoveryPayload*)(response->payload);
                        if (OC_STACK_OK !=
                            OCMapZoneIdToLinkLocalEndpoint(disPayload, response->devAddr.ifindex))
                        {
                            OIC_LOG(ERROR, TAG, "failed at map zone-id for link-local address");
                            OCPayloadDestroy(response->payload);
                            OICFree(response);
                            return;
                        }
                    }
#endif
                }
                else
                {
                    response->resourceUri = OICStrdup(cbNode->requestUri);
                }
            }

            response->numRcvdVendorSpecificHeaderOptions = 0;
            if((responseInfo->info.numOptions > 0) && (responseInfo->info.options != NULL))
            {
                uint8_t start = 0;
                //First option always with option ID is COAP_OPTION_OBSERVE if it is available.
                if(responseInfo->info.options[0].optionID == COAP_OPTION_OBSERVE)
                {
                    size_t i;
                    uint32_t observationOption;
                    uint8_t* optionData = (uint8_t*)responseInfo->info.options[0].optionData;
                    for (observationOption=0, i=0;
                            i<sizeof(uint32_t) && i<responseInfo->info.options[0].optionLength;
                            i++)
                    {
                        observationOption =
                            (observationOption << 8) | optionData[i];
                    }
                    response->sequenceNumber = observationOption;
                    response->numRcvdVendorSpecificHeaderOptions = responseInfo->info.numOptions - 1;
                    start = 1;
                }
                else
                {
                    response->numRcvdVendorSpecificHeaderOptions = responseInfo->info.numOptions;
                }

                if(response->numRcvdVendorSpecificHeaderOptions > MAX_HEADER_OPTIONS)
                {
                    OIC_LOG(ERROR, TAG, "#header options are more than MAX_HEADER_OPTIONS");
                    OCPayloadDestroy(response->payload);
                    OICFree(response);
                    return;
                }

                for (uint8_t i = start; i < responseInfo->info.numOptions; i++)
                {
                    memcpy (&(response->rcvdVendorSpecificHeaderOptions[i - start]),
                            &(responseInfo->info.options[i]), sizeof(OCHeaderOption));
                }
            }

            if (cbNode->method == OC_REST_OBSERVE &&
                response->sequenceNumber > OC_OFFSET_SEQUENCE_NUMBER &&
                cbNode->sequenceNumber <=  MAX_SEQUENCE_NUMBER &&
                response->sequenceNumber <= cbNode->sequenceNumber)
            {
                OIC_LOG_V(INFO, TAG, "Received stale notification. Number :%d",
                                                 response->sequenceNumber);
            }
            else
            {
#ifdef RD_CLIENT
                if (cbNode->requestUri)
                {
                    // if request uri is '/oic/rd', update ins value of resource.
                    char *targetUri = strstr(cbNode->requestUri, OC_RSRVD_RD_URI);
                    if (targetUri)
                    {
                        OCUpdateResourceInsWithResponse(cbNode->requestUri, response);
                    }
                }
#endif
                // set remoteID(device ID) into OCClientResponse callback parameter
                if (OC_REST_DISCOVER == cbNode->method && PAYLOAD_TYPE_DISCOVERY == type)
                {
                    OCDiscoveryPayload *payload = (OCDiscoveryPayload*) response->payload;
                    // Payload can be empty in case of error message.
                    if (payload && payload->sid)
                    {
                        OICStrcpy(response->devAddr.remoteId, sizeof(response->devAddr.remoteId),
                                  payload->sid);
                        OIC_LOG_V(INFO, TAG, "[%d] %s: Device ID of response : %s",
				  __LINE__, __func__,
                                  response->devAddr.remoteId);

#if defined(TCP_ADAPTER) && defined(WITH_CLOUD)
                        CAConnectUserPref_t connPrefer = CA_USER_PREF_CLOUD;
                        CAResult_t ret = CAUtilCMGetConnectionUserConfig(&connPrefer);
                        if (ret == CA_STATUS_OK && connPrefer != CA_USER_PREF_CLOUD)
                        {
                            OCCMDiscoveryResource(response);
                        }
#endif
                    }
                } /* FIXME: else error? */
                if (response->payload && response->payload->type == PAYLOAD_TYPE_REPRESENTATION)
                {
                    HandleBatchResponse(cbNode->requestUri, (OCRepPayload **)&response->payload);
                }

		OIC_LOG_V(INFO, TAG, "%s: calling user CB", __func__);
		errno = 0;
		appFeedback = cbNode->callBack(cbNode->context,
                                                                        cbNode->handle,
                                                                        response);
		/* FIXME: check errno */

		/* now either retain or discard msg saved to co_service_provider_manager */

                cbNode->sequenceNumber = response->sequenceNumber;

                if (appFeedback & OC_STACK_KEEP_TRANSACTION) /* GAR */
                {
		    OIC_LOG(INFO, TAG, "retaining user CB");
                    // To keep discovery callbacks active.
                    cbNode->TTL = GetTicks(MAX_CB_TIMEOUT_SECONDS *
                                            MILLISECONDS_PER_SECOND);
                }
		else
                {
		    OIC_LOG(INFO, TAG, "removing user CB");
                    DeleteClientCB(cbNode);
                }
            }

            //Need to send ACK when the response is CON
            if(responseInfo->info.type == CA_MSG_CONFIRM)
            {
                SendDirectStackResponse(endPoint, responseInfo->info.messageId, CA_EMPTY,
                        CA_MSG_ACKNOWLEDGE, 0, NULL, NULL, 0, NULL, CA_RESPONSE_FOR_RES);
            }

	    if (appFeedback & OC_STACK_KEEP_RESPONSE) { /* GAR */
		OIC_LOG(INFO, TAG, "retaining OCClientResponse");
		/* Client app is responsible for retaining
		   (cosp_mgr_register) and freeing
		   (cosp_mgr_unregister) the message. */
	    } else {
		OIC_LOG(INFO, TAG, "removing OCClientResponse");
		OCPayloadDestroy(response->payload);
		OICFree((void*)response->resourceUri);
		OICFree(response);
	    }
        }
	OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
        return;
    } /* cbNode */
    OIC_LOG_V(DEBUG, TAG, "no callback found for this response message");

    OCResource *resource = NULL;
    ResourceObserver *observer = NULL;
    if (true == GetObserverFromResourceList(&resource, &observer,
                                            responseInfo->info.token,
                                            responseInfo->info.tokenLength))
	{
        OIC_LOG(INFO, TAG, "There is an observer associated with the response token");
        if(responseInfo->result == CA_EMPTY)
        {
            OIC_LOG(INFO, TAG, "Receiving A ACK/RESET for this token");
            if(responseInfo->info.type == CA_MSG_RESET)
            {
                OIC_LOG(INFO, TAG, "This is a RESET");
                OCStackFeedBack(responseInfo->info.token, responseInfo->info.tokenLength,
                        OC_OBSERVER_NOT_INTERESTED);
            }
            else if(responseInfo->info.type == CA_MSG_ACKNOWLEDGE)
            {
                OIC_LOG(INFO, TAG, "This is a pure ACK");
                OCStackFeedBack(responseInfo->info.token, responseInfo->info.tokenLength,
                        OC_OBSERVER_STILL_INTERESTED);
            }
        }
        else if(responseInfo->result == CA_RETRANSMIT_TIMEOUT)
        {
            OIC_LOG(INFO, TAG, "Receiving Time Out for an observer");
            OCStackFeedBack(responseInfo->info.token, responseInfo->info.tokenLength,
                    OC_OBSERVER_FAILED_COMM);
        }
        return;
    }

    if(!cbNode && !observer)
    {
        if(myStackMode == OC_CLIENT || myStackMode == OC_CLIENT_SERVER
           || myStackMode == OC_GATEWAY)
        {
            OIC_LOG(INFO, TAG, "This is a client, but no cbNode was found for token");
            if(responseInfo->result == CA_EMPTY)
            {
                OIC_LOG(INFO, TAG, "Receiving CA_EMPTY in the ocstack");
            }
            else
            {
                OIC_LOG(INFO, TAG, "Received a message without callbacks. Sending RESET");
                SendDirectStackResponse(endPoint, responseInfo->info.messageId, CA_EMPTY,
                                        CA_MSG_RESET, 0, NULL, NULL, 0, NULL, CA_RESPONSE_FOR_RES);
            }
        }

        if(myStackMode == OC_SERVER || myStackMode == OC_CLIENT_SERVER
           || myStackMode == OC_GATEWAY)
        {
            OIC_LOG(INFO, TAG, "This is a server, but no observer was found for token");
            if (responseInfo->info.type == CA_MSG_ACKNOWLEDGE)
            {
                OIC_LOG_V(INFO, TAG, "Received ACK at server for messageId : %d",
                                            responseInfo->info.messageId);
            }
            if (responseInfo->info.type == CA_MSG_RESET)
            {
                OIC_LOG_V(INFO, TAG, "Received RESET at server for messageId : %d",
                                            responseInfo->info.messageId);
            }
        }

        return;
    }
}

/* handle inbound response */
/**
 * This function will be called back by CA layer when a response is received.
 *
 * @param endPoint CA remote endpoint.
 * @param responseInfo CA response info.
 */
void HandleCAResponses(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
{
    VERIFY_NON_NULL_NR(endPoint, FATAL);
    VERIFY_NON_NULL_NR(responseInfo, FATAL);

    OIC_LOG(INFO, TAG, "Enter HandleCAResponses");
    OIC_TRACE_BEGIN(%s:HandleCAResponses, TAG);
#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
#ifdef ROUTING_GATEWAY
    bool needRIHandling = false;
    /*
     * Routing manager is going to update either of endpoint or response or both.
     * This typecasting is done to avoid unnecessary duplication of Endpoint and responseInfo
     * RM can update "routeData" option in endPoint so that future RI requests can be sent to proper
     * destination.
     */
    OCStackResult ret = RMHandleResponse((CAResponseInfo_t *)responseInfo, (CAEndpoint_t *)endPoint,
                                         &needRIHandling);
    if(ret != OC_STACK_OK || !needRIHandling)
    {
        OIC_LOG_V(INFO, TAG, "Routing status![%d]. Not forwarding to RI", ret);
        OIC_TRACE_END();
        return;
    }
#endif

    /*
     * Put source in sender endpoint so that the next packet from application can be routed to
     * proper destination and remove "RM" coap header option before passing request / response to
     * RI as this option will make no sense to either RI or application.
     */
    RMUpdateInfo((CAHeaderOption_t **) &(responseInfo->info.options),
        (uint8_t *) &(responseInfo->info.numOptions),
        (CAEndpoint_t *)endPoint);
#endif

    OCHandleResponse(endPoint, responseInfo);

    OIC_LOG(INFO, TAG, "Exit HandleCAResponses");
    OIC_TRACE_END();
}

/*
 * This function handles error response from CA
 * code shall be added to handle the errors
 */
void HandleCAErrorResponse(const CAEndpoint_t *endPoint, const CAErrorInfo_t *errorInfo)
{
    VERIFY_NON_NULL_NR(endPoint, FATAL);
    VERIFY_NON_NULL_NR(errorInfo, FATAL);

    OIC_LOG(INFO, TAG, "Enter HandleCAErrorResponse");
    OIC_TRACE_BEGIN(%s:HandleCAErrorResponse, TAG);

    ClientCB *cbNode = GetClientCBUsingToken(errorInfo->info.token,
                                             errorInfo->info.tokenLength);
    if (cbNode)
    {
        OCClientResponse *response = NULL;

        response = (OCClientResponse *)OICCalloc(1, sizeof(*response));
        if (!response)
        {
            OIC_LOG(ERROR, TAG, "Allocating memory for response failed");
            OIC_TRACE_END();
            return;
        }

        response->devAddr.adapter = OC_DEFAULT_ADAPTER;
        CopyEndpointToDevAddr(endPoint, &response->devAddr);
        FixUpClientResponse(response);
        response->resourceUri = errorInfo->info.resourceUri;
        memcpy(response->identity.id, errorInfo->info.identity.id,
               sizeof (response->identity.id));
        response->identity.id_length = errorInfo->info.identity.id_length;
        response->result = CAResultToOCResult(errorInfo->result);

        OCStackApplicationResult cbResult = cbNode->callBack(cbNode->context, cbNode->handle, response);
        if (cbResult == OC_STACK_DELETE_TRANSACTION)
        {
            DeleteClientCB(cbNode);
        }
        OICFree(response);
    }

    OCResource *resource = NULL;
    ResourceObserver *observer = NULL;
    if (true == GetObserverFromResourceList(&resource, &observer,
                                            errorInfo->info.token, errorInfo->info.tokenLength))
    {
        OIC_LOG(INFO, TAG, "Receiving communication error for an observer");
        OCStackResult result = CAResultToOCResult(errorInfo->result);
        if (OC_STACK_COMM_ERROR == result)
        {
            OCStackFeedBack(errorInfo->info.token, errorInfo->info.tokenLength,
                            OC_OBSERVER_FAILED_COMM);
        }
    }

    OIC_LOG(INFO, TAG, "Exit HandleCAErrorResponse");
    OIC_TRACE_END();
}


