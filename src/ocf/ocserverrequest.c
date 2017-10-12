/* ****************************************************************
 *
 * Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <string.h>

#include "ocstack.h"
#include "ocserverrequest.h"
#include "ocresourcehandler.h"
#include "ocobserve.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "logger.h"

#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
#include "routingutility.h"
#endif

#include "cacommon.h"
#include "cainterface.h"

#include <coap/pdu.h>

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define VERIFY_NON_NULL(arg) { if (!arg) {OIC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }

// Module Name
#define TAG "OIC_RI_SERVERREQUEST"

//-------------------------------------------------------------------------------------------------
// Local functions for RB tree
//-------------------------------------------------------------------------------------------------
static int RBRequestTokenCmp(OCServerRequest *target, OCServerRequest *treeNode)
{
    return memcmp(target->requestToken, treeNode->requestToken, target->tokenLength);
}

static int RBResponseTokenCmp(OCServerResponse *target, OCServerResponse *treeNode)
{
    return memcmp(((OCServerRequest*)target->requestHandle)->requestToken,
                  ((OCServerRequest*)treeNode->requestHandle)->requestToken,
                  ((OCServerRequest*)target->requestHandle)->tokenLength);
}

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
RB_HEAD(ServerRequestTree, OCServerRequest) g_serverRequestTree =
                                                            RB_INITIALIZER(&g_serverRequestTree);
RBL_GENERATE(ServerRequestTree, OCServerRequest, entry, RBRequestTokenCmp)

RB_HEAD(ServerResponseTree, OCServerResponse) g_serverResponseTree =
                                                            RB_INITIALIZER(&g_serverResponseTree);
RB_GENERATE(ServerResponseTree, OCServerResponse, entry, RBResponseTokenCmp)

//-------------------------------------------------------------------------------------------------
// Local functions
//-------------------------------------------------------------------------------------------------

/**
 * Add a server response to the server response list
 *
 * @param[in]  response         initialized server response that is created by this function
 * @param[in]  requestHandle    handle of the response
 *
 * @return
 *     OCStackResult
 */
static OCStackResult AddServerResponse (OCServerResponse ** response, OCRequestHandle requestHandle)
{
    assert(response);

    OCServerResponse * serverResponse = NULL;

    serverResponse = (OCServerResponse *) OICCalloc(1, sizeof(OCServerResponse));
    VERIFY_NON_NULL(serverResponse);

    serverResponse->payload = NULL;
    serverResponse->requestHandle = requestHandle;

    *response = serverResponse;

    RB_INSERT(ServerResponseTree, &g_serverResponseTree, serverResponse);
    OIC_LOG(INFO, TAG, "Server Response Added");
    return OC_STACK_OK;

exit:
    *response = NULL;
    return OC_STACK_NO_MEMORY;
}

/**
 * Get a server response from the server response list using the specified handle
 *
 * @param[in]  handle           handle of server response.
 *
 * @return
 *     OCServerResponse*
 */
static OCServerResponse * GetServerResponseUsingHandle (const OCServerRequest * handle)
{
    if (!handle)
    {
        OIC_LOG(ERROR, TAG, "Invalid Parameter handle");
        return NULL;
    }

    OCServerResponse tmpFind, *out = NULL;

    tmpFind.requestHandle = (OCRequestHandle)handle;
    out = RB_FIND(ServerResponseTree, &g_serverResponseTree, &tmpFind);

    if (!out)
    {
        OIC_LOG(INFO, TAG, "Server Response not found!!");
        return NULL;
    }

    OIC_LOG(INFO, TAG, "Found in server response list");
    return out;
}

/**
 * Delete a server response from the server response list
 *
 * @param[in] serverResponse    server response to delete
 */
static void DeleteServerResponse (OCServerResponse * serverResponse)
{
    if (serverResponse)
    {
        RB_REMOVE(ServerResponseTree, &g_serverResponseTree, serverResponse);
        OICFree(serverResponse);
        serverResponse = NULL;
        OIC_LOG(INFO, TAG, "Server Response Removed!!");
    }
}

/**
 * Ensure no accept header option is included when sending responses and add routing info to
 * outgoing response.
 *
 * @param[in]  object           CA remote endpoint.
 * @param[in]  responseInfo     CA response info.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult OCSendResponse (const CAEndpoint_t *object, CAResponseInfo_t *responseInfo)
{
#if defined (ROUTING_GATEWAY) || defined (ROUTING_EP)
    // Add route info in RM option.
    OCStackResult rmResult = RMAddInfo(object->routeData, responseInfo, false, NULL);
    if(OC_STACK_OK != rmResult)
    {
        OIC_LOG(ERROR, TAG, "Add option failed");
        return rmResult;
    }
#endif

    // Do not include the accept header option
    responseInfo->info.acceptFormat = CA_FORMAT_UNDEFINED;
    CAResult_t result = CASendResponse(object, responseInfo);
    if(CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TAG, "CASendResponse failed with CA error %u", result);
        return CAResultToOCResult(result);
    }
    return OC_STACK_OK;
}

static CAPayloadFormat_t OCToCAPayloadFormat (OCPayloadFormat ocFormat)
{
    switch (ocFormat)
    {
    case OC_FORMAT_UNDEFINED:
        return CA_FORMAT_UNDEFINED;
    case OC_FORMAT_CBOR:
        return CA_FORMAT_APPLICATION_CBOR;
    case OC_FORMAT_VND_OCF_CBOR:
        return CA_FORMAT_APPLICATION_VND_OCF_CBOR;
    default:
        return CA_FORMAT_UNSUPPORTED;
    }
}
static CAResponseResult_t ConvertEHResultToCAResult (OCEntityHandlerResult result, OCMethod method)
{
    CAResponseResult_t caResult = CA_BAD_REQ;

    switch (result)
    {
        // Successful Client Request
        case OC_EH_RESOURCE_CREATED: // 2.01
            if (method == OC_REST_POST || method == OC_REST_PUT)
            {
                caResult = CA_CREATED;
            }
            break;
        case OC_EH_RESOURCE_DELETED: // 2.02
            if (method == OC_REST_POST || method == OC_REST_DELETE)
            {
                caResult = CA_DELETED;
            }
            break;
        case OC_EH_SLOW: // 2.05
            caResult = CA_CONTENT;
            break;
        case OC_EH_OK:
        case OC_EH_CHANGED: // 2.04
        case OC_EH_CONTENT: // 2.05
            if (method == OC_REST_POST || method == OC_REST_PUT || method == OC_REST_DELETE)
            {
                caResult = CA_CHANGED;
            }
            else if (method == OC_REST_GET)
            {
                caResult = CA_CONTENT;
            }
            break;
        case OC_EH_VALID: // 2.03
            caResult = CA_VALID;
            break;
        // Unsuccessful Client Request
        case OC_EH_UNAUTHORIZED_REQ: // 4.01
            caResult = CA_UNAUTHORIZED_REQ;
            break;
        case OC_EH_BAD_OPT: // 4.02
            caResult = CA_BAD_OPT;
            break;
        case OC_EH_FORBIDDEN: // 4.03
            caResult = CA_FORBIDDEN_REQ;
            break;
        case OC_EH_RESOURCE_NOT_FOUND: // 4.04
            caResult = CA_NOT_FOUND;
            break;
        case OC_EH_METHOD_NOT_ALLOWED: // 4.05
            caResult = CA_METHOD_NOT_ALLOWED;
            break;
        case OC_EH_NOT_ACCEPTABLE: // 4.06
            caResult = CA_NOT_ACCEPTABLE;
            break;
        case OC_EH_INTERNAL_SERVER_ERROR: // 5.00
            caResult = CA_INTERNAL_SERVER_ERROR;
            break;
        case OC_EH_SERVICE_UNAVAILABLE: // 5.03
            caResult = CA_SERVICE_UNAVAILABLE;
            break;
        case OC_EH_RETRANSMIT_TIMEOUT: // 5.04
            caResult = CA_RETRANSMIT_TIMEOUT;
            break;
        default:
            caResult = CA_BAD_REQ;
            break;
    }
    return caResult;
}

//-------------------------------------------------------------------------------------------------
// Internal APIs
//-------------------------------------------------------------------------------------------------
OCStackResult AddServerRequest (OCServerRequest ** request,
                                uint16_t coapMessageID,
                                uint8_t delayedResNeeded,
                                uint8_t notificationFlag,
                                OCMethod method,
                                uint8_t numRcvdVendorSpecificHeaderOptions,
                                uint32_t observationOption,
                                OCQualityOfService qos,
                                char * query,
                                OCHeaderOption * rcvdVendorSpecificHeaderOptions,
                                OCPayloadFormat payloadFormat,
                                uint8_t * payload,
                                CAToken_t requestToken,
                                uint8_t tokenLength,
                                char * resourceUrl,
                                size_t payloadSize,
                                OCPayloadFormat acceptFormat,
                                uint16_t acceptVersion,
                                const OCDevAddr *devAddr)
{
    if (!request || !devAddr)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OIC_LOG_V(INFO, TAG, "AddServerRequest entry [%s:%u]", devAddr->addr, devAddr->port);

    OCServerRequest * serverRequest = (OCServerRequest *) OICCalloc(1, sizeof(OCServerRequest) +
                                                            (payloadSize ? payloadSize : 1) - 1);
    VERIFY_NON_NULL(serverRequest);

    serverRequest->coapID = coapMessageID;
    serverRequest->delayedResNeeded = delayedResNeeded;
    serverRequest->notificationFlag = notificationFlag;
    serverRequest->method = method;
    serverRequest->numRcvdVendorSpecificHeaderOptions = numRcvdVendorSpecificHeaderOptions;
    serverRequest->observationOption = observationOption;
    serverRequest->observeResult = OC_STACK_ERROR;
    serverRequest->qos = qos;
    serverRequest->acceptFormat = acceptFormat;
    serverRequest->acceptVersion = acceptVersion;
    serverRequest->ehResponseHandler = HandleSingleResponse;
    serverRequest->numResponses = 1;

    if (query)
    {
        OICStrcpy(serverRequest->query, sizeof(serverRequest->query), query);
    }
    if (rcvdVendorSpecificHeaderOptions)
    {
        memcpy(serverRequest->rcvdVendorSpecificHeaderOptions, rcvdVendorSpecificHeaderOptions,
            MAX_HEADER_OPTIONS * sizeof(OCHeaderOption));
    }
    if (payload && payloadSize)
    {
        // destination is at least 1 greater than the source, so a NULL always exists in the
        // last character
        memcpy(serverRequest->payload, payload, payloadSize);
        serverRequest->payloadSize = payloadSize;
        serverRequest->payloadFormat = payloadFormat;
    }

    serverRequest->requestComplete = 0;

    if (requestToken)
    {
        // If tokenLength is zero, the return value depends on the
        // particular library implementation (it may or may not be a null pointer).
        if (tokenLength)
        {
            serverRequest->requestToken = (CAToken_t) OICMalloc(tokenLength);
            VERIFY_NON_NULL(serverRequest->requestToken);
            memcpy(serverRequest->requestToken, requestToken, tokenLength);
        }
    }
    serverRequest->tokenLength = tokenLength;

    if (resourceUrl)
    {
        OICStrcpy(serverRequest->resourceUrl, sizeof(serverRequest->resourceUrl), resourceUrl);
    }

    serverRequest->devAddr = *devAddr;

    *request = serverRequest;

    RBL_INSERT(ServerRequestTree, &g_serverRequestTree, serverRequest);
    OIC_LOG(INFO, TAG, "Server Request Added");
    return OC_STACK_OK;

exit:
    if (serverRequest)
    {
        OICFree(serverRequest);
        serverRequest = NULL;
    }
    *request = NULL;
    return OC_STACK_NO_MEMORY;
}

OCServerRequest * GetServerRequestUsingToken (const CAToken_t token, uint8_t tokenLength)
{
    if (!token)
    {
        OIC_LOG(ERROR, TAG, "Invalid Parameter Token");
        return NULL;
    }

    OIC_LOG(INFO, TAG,"Get server request with token");
    OIC_LOG_BUFFER(INFO, TAG, (const uint8_t *)token, tokenLength);

    OCServerRequest tmpFind, *out = NULL;

    tmpFind.requestToken = token;
    tmpFind.tokenLength = tokenLength;
    out = RB_FIND(ServerRequestTree, &g_serverRequestTree, &tmpFind);

    if (!out)
    {
        OIC_LOG(INFO, TAG, "Server Request not found!!");
        return NULL;
    }

    OIC_LOG(INFO, TAG, "Found in server request list");
    return out;
}

void DeleteServerRequest(OCServerRequest * serverRequest)
{
    if (serverRequest)
    {
        RBL_REMOVE(ServerRequestTree, &g_serverRequestTree, serverRequest);
        OICFree(serverRequest->requestToken);
        OICFree(serverRequest);
        serverRequest = NULL;
        OIC_LOG(INFO, TAG, "Server Request Removed");
    }
}

OCStackResult FormOCEntityHandlerRequest(OCEntityHandlerRequest * entityHandlerRequest,
                                         OCRequestHandle request,
                                         OCMethod method,
                                         OCDevAddr *endpoint,
                                         OCResourceHandle resource,
                                         char * queryBuf,
                                         OCPayloadType payloadType,
                                         OCPayloadFormat payloadFormat,
                                         uint8_t * payload,
                                         size_t payloadSize,
                                         uint8_t numVendorOptions,
                                         OCHeaderOption * vendorOptions,
                                         OCObserveAction observeAction,
                                         OCObservationId observeID,
                                         uint16_t messageID)
{
    if (entityHandlerRequest)
    {
        entityHandlerRequest->resource = (OCResourceHandle) resource;
        entityHandlerRequest->requestHandle = request;
        entityHandlerRequest->method = method;
        entityHandlerRequest->devAddr = *endpoint;
        entityHandlerRequest->query = queryBuf;
        entityHandlerRequest->obsInfo.action = observeAction;
        entityHandlerRequest->obsInfo.obsId = observeID;
        entityHandlerRequest->messageID = messageID;

        if(payload && payloadSize)
        {
            if(OCParsePayload(&entityHandlerRequest->payload, payloadFormat, payloadType,
                        payload, payloadSize) != OC_STACK_OK)
            {
                return OC_STACK_ERROR;
            }
        }
        else
        {
            entityHandlerRequest->payload = NULL;
        }

        entityHandlerRequest->numRcvdVendorSpecificHeaderOptions = numVendorOptions;
        entityHandlerRequest->rcvdVendorSpecificHeaderOptions = vendorOptions;

        return OC_STACK_OK;
    }

    return OC_STACK_INVALID_PARAM;
}

/**
 * Handler function for sending a response from a single resource
 *
 * @param ehResponse - pointer to the response from the resource
 *
 * @return
 *     OCStackResult
 */
OCStackResult HandleSingleResponse(OCEntityHandlerResponse * ehResponse)
{
    OCStackResult result = OC_STACK_ERROR;
    CAEndpoint_t responseEndpoint = {.adapter = CA_DEFAULT_ADAPTER};
    CAResponseInfo_t responseInfo = {.result = CA_EMPTY};
    CAHeaderOption_t* optionsPointer = NULL;

    if(!ehResponse || !ehResponse->requestHandle)
    {
        OIC_LOG(ERROR, TAG, "ehResponse/requestHandle is NULL");
        return OC_STACK_ERROR;
    }

    OCServerRequest *serverRequest = (OCServerRequest *)ehResponse->requestHandle;

    CopyDevAddrToEndpoint(&serverRequest->devAddr, &responseEndpoint);

    responseInfo.info.messageId = serverRequest->coapID;
    responseInfo.info.resourceUri = serverRequest->resourceUrl;
    responseInfo.result = ConvertEHResultToCAResult(ehResponse->ehResult, serverRequest->method);
    responseInfo.info.dataType = CA_RESPONSE_DATA;

    if(serverRequest->notificationFlag && serverRequest->qos == OC_HIGH_QOS)
    {
        responseInfo.info.type = CA_MSG_CONFIRM;
    }
    else if(serverRequest->notificationFlag && serverRequest->qos != OC_HIGH_QOS)
    {
        responseInfo.info.type = CA_MSG_NONCONFIRM;
    }
    else if(!serverRequest->notificationFlag && !serverRequest->slowFlag &&
            serverRequest->qos == OC_HIGH_QOS)
    {
        responseInfo.info.type = CA_MSG_ACKNOWLEDGE;
    }
    else if(!serverRequest->notificationFlag && serverRequest->slowFlag &&
            serverRequest->qos == OC_HIGH_QOS)
    {
        // To assign new messageId in CA.
        responseInfo.info.messageId = 0;
        responseInfo.info.type = CA_MSG_CONFIRM;
    }
    else if(!serverRequest->notificationFlag)
    {
        responseInfo.info.type = CA_MSG_NONCONFIRM;
    }
    else
    {
        OIC_LOG(ERROR, TAG, "default responseInfo type is NON");
        responseInfo.info.type = CA_MSG_NONCONFIRM;
    }

    char rspToken[CA_MAX_TOKEN_LEN + 1] = {0};
    responseInfo.info.messageId = serverRequest->coapID;
    responseInfo.info.token = (CAToken_t)rspToken;

    memcpy(responseInfo.info.token, serverRequest->requestToken, serverRequest->tokenLength);
    responseInfo.info.tokenLength = serverRequest->tokenLength;

    if((serverRequest->observeResult == OC_STACK_OK)&&
       (serverRequest->observationOption != MAX_SEQUENCE_NUMBER + 1))
    {
        responseInfo.info.numOptions = ehResponse->numSendVendorSpecificHeaderOptions + 1;
    }
    else
    {
        responseInfo.info.numOptions = ehResponse->numSendVendorSpecificHeaderOptions;
    }

    // Path of new resource is returned in options as location-path.
    if (ehResponse->resourceUri[0] != '\0')
    {
        responseInfo.info.numOptions++;
    }

    // Check if version and format option exist.
    uint16_t payloadVersion = OC_SPEC_VERSION_VALUE;
    uint16_t payloadFormat = COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR;
    bool IsPayloadVersionSet = false;
    bool IsPayloadFormatSet = false;
    if (ehResponse->payload)
    {
        for (uint8_t i = 0; i < responseInfo.info.numOptions; i++)
        {
            if (COAP_OPTION_CONTENT_VERSION == ehResponse->sendVendorSpecificHeaderOptions[i].optionID)
            {
                payloadVersion =
                        (ehResponse->sendVendorSpecificHeaderOptions[i].optionData[1] << 8)
                        + ehResponse->sendVendorSpecificHeaderOptions[i].optionData[0];
                IsPayloadVersionSet = true;
            }
            else if (COAP_OPTION_CONTENT_TYPE == ehResponse->sendVendorSpecificHeaderOptions[i].optionID)
            {
                if (1 == ehResponse->sendVendorSpecificHeaderOptions[i].optionLength)
                {
                    payloadFormat = ehResponse->sendVendorSpecificHeaderOptions[i].optionData[0];
                    IsPayloadFormatSet = true;
                }
                else if (2 == ehResponse->sendVendorSpecificHeaderOptions[i].optionLength)
                {
                    payloadFormat =
                            (ehResponse->sendVendorSpecificHeaderOptions[i].optionData[1] << 8)
                            + ehResponse->sendVendorSpecificHeaderOptions[i].optionData[0];

                    IsPayloadFormatSet = true;
                }
                else
                {
                    payloadFormat = CA_FORMAT_UNSUPPORTED;
                    IsPayloadFormatSet = false;
                    OIC_LOG_V(DEBUG, TAG, "option has an unsupported format");
                }
            }
        }
        if (!IsPayloadVersionSet && !IsPayloadFormatSet)
        {
            responseInfo.info.numOptions = responseInfo.info.numOptions + 2;
        }
        else if ((IsPayloadFormatSet && CA_FORMAT_APPLICATION_VND_OCF_CBOR == payloadFormat
                && !IsPayloadVersionSet) || (IsPayloadVersionSet && !IsPayloadFormatSet))
        {
            responseInfo.info.numOptions++;
        }
    }

    if (responseInfo.info.numOptions > 0)
    {
        responseInfo.info.options = (CAHeaderOption_t *)
                                      OICCalloc(responseInfo.info.numOptions,
                                              sizeof(CAHeaderOption_t));

        if(!responseInfo.info.options)
        {
            OIC_LOG(FATAL, TAG, "Memory alloc for options failed");
            return OC_STACK_NO_MEMORY;
        }

        optionsPointer = responseInfo.info.options;

        // TODO: This exposes CoAP specific details.  At some point, this should be
        // re-factored and handled in the CA layer.
        // Stack sets MAX_SEQUENCE_NUMBER+1 to sequence number on observe cancel request.
        // And observe option should not be part of CoAP response for observe cancel request.
        if(serverRequest->observeResult == OC_STACK_OK
           && serverRequest->observationOption != MAX_SEQUENCE_NUMBER + 1)
        {
            responseInfo.info.options[0].protocolID = CA_COAP_ID;
            responseInfo.info.options[0].optionID = COAP_OPTION_OBSERVE;
            responseInfo.info.options[0].optionLength = sizeof(uint32_t);
            uint8_t* observationData = (uint8_t*)responseInfo.info.options[0].optionData;
            uint32_t observationOption= serverRequest->observationOption;

            for (size_t i=sizeof(uint32_t); i; --i)
            {
                observationData[i-1] = observationOption & 0xFF;
                observationOption >>=8;
            }

            // Point to the next header option before copying vender specific header options
            optionsPointer += 1;
        }

        if (ehResponse->numSendVendorSpecificHeaderOptions)
        {
            memcpy(optionsPointer, ehResponse->sendVendorSpecificHeaderOptions,
                            sizeof(OCHeaderOption) *
                            ehResponse->numSendVendorSpecificHeaderOptions);

            // Advance the optionPointer by the number of vendor options.
            optionsPointer += ehResponse->numSendVendorSpecificHeaderOptions;
        }

        // Return new resource as location-path option.
        // https://tools.ietf.org/html/rfc7252#section-5.8.2.
        if (ehResponse->resourceUri[0] != '\0')
        {
            if ((strlen(ehResponse->resourceUri) + 1) > CA_MAX_HEADER_OPTION_DATA_LENGTH)
            {
                OIC_LOG(ERROR, TAG,
                    "New resource path must be less than CA_MAX_HEADER_OPTION_DATA_LENGTH");
                OICFree(responseInfo.info.options);
                return OC_STACK_INVALID_URI;
            }

            optionsPointer->protocolID = CA_COAP_ID;
            optionsPointer->optionID = CA_HEADER_OPTION_ID_LOCATION_PATH;
            OICStrcpy(
                optionsPointer->optionData,
                sizeof(optionsPointer->optionData),
                ehResponse->resourceUri);
            optionsPointer->optionLength = (uint16_t)strlen(optionsPointer->optionData) + 1;
            optionsPointer += 1;
        }

        if (ehResponse->payload)
        {
            if (!IsPayloadVersionSet && !IsPayloadFormatSet)
            {
                optionsPointer->protocolID = CA_COAP_ID;
                optionsPointer->optionID = CA_OPTION_CONTENT_VERSION;
                memcpy(optionsPointer->optionData, &payloadVersion,
                        sizeof(uint16_t));
                optionsPointer->optionLength = sizeof(uint16_t);
                optionsPointer += 1;

                optionsPointer->protocolID = CA_COAP_ID;
                optionsPointer->optionID = COAP_OPTION_CONTENT_FORMAT;
                memcpy(optionsPointer->optionData, &payloadFormat,
                        sizeof(uint16_t));
                optionsPointer->optionLength = sizeof(uint16_t);
            }
            else if (IsPayloadFormatSet && CA_FORMAT_APPLICATION_VND_OCF_CBOR == payloadFormat
                            && !IsPayloadVersionSet)
            {
                optionsPointer->protocolID = CA_COAP_ID;
                optionsPointer->optionID = CA_OPTION_CONTENT_VERSION;
                memcpy(optionsPointer->optionData, &payloadVersion,
                        sizeof(uint16_t));
                optionsPointer->optionLength = sizeof(uint16_t);
            }
            else if (IsPayloadVersionSet && OC_SPEC_VERSION_VALUE <= payloadVersion && !IsPayloadFormatSet)
            {
                optionsPointer->protocolID = CA_COAP_ID;
                optionsPointer->optionID = COAP_OPTION_CONTENT_TYPE;
                memcpy(optionsPointer->optionData, &payloadFormat,
                        sizeof(uint16_t));
                optionsPointer->optionLength = sizeof(uint16_t);
            }
        }
    }
    else
    {
        responseInfo.info.options = NULL;
    }

    responseInfo.isMulticast = false;
    responseInfo.info.payload = NULL;
    responseInfo.info.payloadSize = 0;
    responseInfo.info.payloadFormat = CA_FORMAT_UNDEFINED;

    // Put the JSON prefix and suffix around the payload
    if(ehResponse->payload)
    {
        if (ehResponse->payload->type == PAYLOAD_TYPE_PRESENCE)
        {
            responseInfo.isMulticast = true;
        }
        else
        {
            responseInfo.isMulticast = false;
        }

        switch(serverRequest->acceptFormat)
        {
            case OC_FORMAT_UNDEFINED:
                // No preference set by the client, so default to CBOR then
            case OC_FORMAT_CBOR:
            case OC_FORMAT_VND_OCF_CBOR:
                if((result = OCConvertPayload(ehResponse->payload, serverRequest->acceptFormat,
                                &responseInfo.info.payload, &responseInfo.info.payloadSize))
                        != OC_STACK_OK)
                {
                    OIC_LOG(ERROR, TAG, "Error converting payload");
                    OICFree(responseInfo.info.options);
                    return result;
                }
                // Add CONTENT_FORMAT OPT if payload exist
                if (ehResponse->payload->type != PAYLOAD_TYPE_DIAGNOSTIC &&
                        responseInfo.info.payloadSize > 0)
                {
                    responseInfo.info.payloadFormat = OCToCAPayloadFormat(
                            serverRequest->acceptFormat);
                    if (CA_FORMAT_UNDEFINED == responseInfo.info.payloadFormat)
                    {
                        responseInfo.info.payloadFormat = CA_FORMAT_APPLICATION_CBOR;
                    }
                    if ((OC_FORMAT_VND_OCF_CBOR == serverRequest->acceptFormat))
                    {
                        // Add versioning information for this format
                        responseInfo.info.payloadVersion = serverRequest->acceptVersion;
                        if (!responseInfo.info.payloadVersion)
                        {
                            responseInfo.info.payloadVersion = DEFAULT_VERSION_VALUE;
                        }

                    }
                }
                break;
            default:
                responseInfo.result = CA_NOT_ACCEPTABLE;
        }
    }

#ifdef WITH_PRESENCE
    CATransportAdapter_t CAConnTypes[] = {
                            CA_ADAPTER_IP,
                            CA_ADAPTER_GATT_BTLE,
                            CA_ADAPTER_RFCOMM_BTEDR,
                            CA_ADAPTER_NFC
#ifdef RA_ADAPTER
                            , CA_ADAPTER_REMOTE_ACCESS
#endif
                            , CA_ADAPTER_TCP
                        };

    size_t size = sizeof(CAConnTypes)/ sizeof(CATransportAdapter_t);

    CATransportAdapter_t adapter = responseEndpoint.adapter;
    // Default adapter, try to send response out on all adapters.
    if (adapter == CA_DEFAULT_ADAPTER)
    {
        adapter =
            (CATransportAdapter_t)(
                CA_ADAPTER_IP           |
                CA_ADAPTER_GATT_BTLE    |
                CA_ADAPTER_RFCOMM_BTEDR |
                CA_ADAPTER_NFC
#ifdef RA_ADAP
                | CA_ADAPTER_REMOTE_ACCESS
#endif
                | CA_ADAPTER_TCP
            );
    }

    result = OC_STACK_OK;
    OCStackResult tempResult = OC_STACK_OK;

    for(size_t i = 0; i < size; i++ )
    {
        responseEndpoint.adapter = (CATransportAdapter_t)(adapter & CAConnTypes[i]);
        if(responseEndpoint.adapter)
        {
            //The result is set to OC_STACK_OK only if OCSendResponse succeeds in sending the
            //response on all the n/w interfaces else it is set to OC_STACK_ERROR
            tempResult = OCSendResponse(&responseEndpoint, &responseInfo);
        }
        if(OC_STACK_OK != tempResult)
        {
            result = tempResult;
        }
    }
#else

    OIC_LOG(INFO, TAG, "Calling OCSendResponse with:");
    OIC_LOG_V(INFO, TAG, "\tEndpoint address: %s", responseEndpoint.addr);
    OIC_LOG_V(INFO, TAG, "\tEndpoint adapter: %s", responseEndpoint.adapter);
    OIC_LOG_V(INFO, TAG, "\tResponse result : %s", responseInfo.result);
    OIC_LOG_V(INFO, TAG, "\tResponse for uri: %s", responseInfo.info.resourceUri);

    result = OCSendResponse(&responseEndpoint, &responseInfo);
#endif

    OICFree(responseInfo.info.payload);
    OICFree(responseInfo.info.options);
    //Delete the request
    DeleteServerRequest(serverRequest);
    return result;
}

OCStackResult HandleAggregateResponse(OCEntityHandlerResponse * ehResponse)
{
    if(!ehResponse || !ehResponse->payload)
    {
        OIC_LOG(ERROR, TAG, "HandleAggregateResponse invalid parameters");
        return OC_STACK_INVALID_PARAM;
    }

    OIC_LOG(INFO, TAG, "Inside HandleAggregateResponse");

    OCServerRequest *serverRequest = (OCServerRequest *)ehResponse->requestHandle;
    OCServerResponse *serverResponse = GetServerResponseUsingHandle((OCServerRequest *)
                                                                    ehResponse->requestHandle);

    OCStackResult stackRet = OC_STACK_ERROR;
    if(serverRequest)
    {
        if(!serverResponse)
        {
            OIC_LOG(INFO, TAG, "This is the first response fragment");
            stackRet = AddServerResponse(&serverResponse, ehResponse->requestHandle);
            if (OC_STACK_OK != stackRet)
            {
                OIC_LOG(ERROR, TAG, "Error adding server response");
                return stackRet;
            }
            VERIFY_NON_NULL(serverResponse);
        }

        if(ehResponse->payload->type != PAYLOAD_TYPE_REPRESENTATION)
        {
            stackRet = OC_STACK_ERROR;
            OIC_LOG(ERROR, TAG, "Error adding payload, as it was the incorrect type");
            goto exit;
        }

        OCRepPayload *newPayload = OCRepPayloadBatchClone((OCRepPayload *)ehResponse->payload);

        if(!serverResponse->payload)
        {
            serverResponse->payload = (OCPayload *)newPayload;
        }
        else
        {
            OCRepPayloadAppend((OCRepPayload*)serverResponse->payload,
                    (OCRepPayload*)newPayload);
        }

        (serverRequest->numResponses)--;

        if(serverRequest->numResponses == 0)
        {
            OIC_LOG(INFO, TAG, "This is the last response fragment");
            ehResponse->payload = serverResponse->payload;
            ehResponse->ehResult = OC_EH_OK;
            stackRet = HandleSingleResponse(ehResponse);
            //Delete the request and response
            DeleteServerRequest(serverRequest);
            DeleteServerResponse(serverResponse);
        }
        else
        {
            OIC_LOG(INFO, TAG, "More response fragments to come");
            stackRet = OC_STACK_OK;
        }
    }
exit:

    return stackRet;
}
