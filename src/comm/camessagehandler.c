/* *****************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include "camessagehandler.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#if INTERFACE
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#endif	/* INTERFACE */

#include "coap_config.h"
#include "coap/coap_list.h"
#include "coap/block.h"
#include "coap/pdu.h"

CAHistory_t requestHistory;  /**< filter IP family in requests */

// fn ptr typedefs from cacommon.h
#if INTERFACE
/**
 * Callback function type for request delivery.
 * @param[out]   object       Endpoint object from which the request is received.
 *                            It contains endpoint address based on the connectivity type.
 * @param[out]   requestInfo  Info for resource model to understand about the request.
 */
typedef void (*CARequestCallback)(const CAEndpoint_t *object,
                                  const struct CARequestInfo *requestInfo);

/**
 * Callback function type for response delivery.
 * @param[out]   object           Endpoint object from which the response is received.
 * @param[out]   responseInfo     Identifier which needs to be mapped with response.
 */
typedef void (*CAResponseCallback)(const CAEndpoint_t *object,
                                   const CAResponseInfo_t *responseInfo);

/**
 * Callback function type for error.
 * @param[out]   object           remote device information.
 * @param[out]   errorInfo        CA Error information.
 */
typedef void (*CAErrorCallback)(const CAEndpoint_t *object,
                                const CAErrorInfo_t *errorInfo);

/**
 * Callback function type for error.
 * @param[out]   object           remote device information.
 * @param[out]   result           error information.
 */
typedef CAResult_t (*CAHandshakeErrorCallback)(const CAEndpoint_t *object,
                                               const CAErrorInfo_t *errorInfo);

/**
 * Data type whether the data is Request Message or Response Message.
 * if there is some failure before send data on network.
 * Type will be set as error type for error callback.
 */
typedef enum                    /* => camessagehandler.c */
{
    CA_REQUEST_DATA = 1,                  /**< Request data */
    CA_RESPONSE_DATA,                     /**< Response data */
    CA_ERROR_DATA,                        /**< Error data */
    CA_RESPONSE_FOR_RES,                  /**< Response for resource */
#ifdef WITH_TCP
    CA_SIGNALING_DATA                     /**< Signaling data */
#endif
} CADataType_t;                 /* FIXME: CoAP_MSG_KIND? */
#endif	/* INTERFACE */

/* #define CA_MEMORY_ALLOC_CHECK(arg) { if (NULL == arg) {OIC_LOG(ERROR, TAG, "Out of memory"); \ */
/* goto memory_error_exit;} } */

#if INTERFACE
/** IP, EDR, LE. **/
#define DEFAULT_RETRANSMISSION_TYPE (CA_ADAPTER_IP | \
                                     CA_ADAPTER_RFCOMM_BTEDR | \
                                     CA_ADAPTER_GATT_BTLE)

/** default ACK time is 2 sec(CoAP). **/
#define DEFAULT_ACK_TIMEOUT_SEC     2

/** default max retransmission trying count is 4(CoAP). **/
#define DEFAULT_RETRANSMISSION_COUNT      4

/** check period is 1 sec. **/
#define RETRANSMISSION_CHECK_PERIOD_SEC     1

/** retransmission data send method type. **/
typedef CAResult_t (*CADataSendMethod_t)(const CAEndpoint_t *endpoint,
                                         const void *pdu,
                                         uint32_t size,
                                         CADataType_t dataType);

/** retransmission timeout callback type. **/
typedef void (*CATimeoutCallback_t)(const CAEndpoint_t *endpoint,
                                    const void *pdu,
                                    uint32_t size);

typedef enum
{
    MULTICAST = 0, // SEND_TYPE_MULTICAST = 0,
    UNICAST  // SEND_TYPE_UNICAST
} ROUTING_TYPE;  // CASendDataType_t;

typedef struct oocf_ocf_msg_generic
{
    ROUTING_TYPE /* CASendDataType_t type */ outbound_routing;            /**< routing type (ucast/mcast) */
    CAEndpoint_t *remoteEndpoint;     /**< remote endpoint; origin for inbound, dest for outbound */
    //union {
    struct CARequestInfo *requestInfo;     /**< request information */
    CAResponseInfo_t *responseInfo;   /**< response information */
    CAErrorInfo_t *errorInfo;         /**< error information */
#ifdef WITH_TCP
    CASignalingInfo_t *signalingInfo; /**< signaling information */
#endif
        //}
    CADataType_t dataType;            /**< data type */
} CAData_t;
#endif

#define SINGLE_HANDLE
#define MAX_THREAD_POOL_SIZE    20

// thread pool handle
static ca_thread_pool_t g_threadPoolHandle = NULL;

// message handler main thread
static CAQueueingThread_t g_sendThread;
static CAQueueingThread_t g_receiveThread;

//#define CA_MAX_RT_ARRAY_SIZE    3

#define BLOCKWISE_OPTION_BUFFER    (sizeof(unsigned int))

#define TAG "OIC_CA_MSG_HANDLE"

static CARetransmission_t g_retransmissionContext;

static CANetworkMonitorCallback g_nwMonitorHandler = NULL;

#ifdef WITH_BWT
void CAAddDataToSendThread(CAData_t *data) /* called by CAAddSendThreadQueue */
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL_VOID(data, TAG, "data");
    size_t payloadLen = 0;
    CAGetPayloadInfo(data, &payloadLen);
    OIC_LOG_V(DEBUG, TAG, "payloadLen=%" PRIuPTR, payloadLen);

    // add thread
    CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));
}

void CAAddDataToReceiveThread(CAData_t *data)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL_VOID(data, TAG, "data");

    // add thread
    CAQueueingThreadAddData(&g_receiveThread, data, sizeof(CAData_t));
}
#endif

// delete unneeded fn:
/* static bool CAIsSelectedNetworkAvailable(void) */
/* { */
/*     u_arraylist_t *list = CAGetSelectedNetworkList(); */
/*     if (!list || u_arraylist_length(list) == 0) */
/*     { */
/*         OIC_LOG(ERROR, TAG, "No selected network"); */
/*         return false; */
/*     } */

/*     return true; */
/* } */

/* convert incoming PDU to OCF data, handles both requests and
   responses (inbound only) and error (outbound); called by
   mh_CAReceivedPacketCallback and CAErrorHandler. result is passed to msg handler
*/
/* FIXME: @rename oocf_coap_pdu_to_msg */
static CAData_t* _oocf_coap_pdu_to_msg(const CAEndpoint_t *endpoint, /* @was CAGenerateHandlerData */
                                       const CARemoteId_t *identity,
                                       const coap_pdu_t /* void */ *pdu,
				       CADataType_t dataType /* request, respose, error, etc */
                                       )
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
#if defined(DEBUG_MSGS)
    CAInfo_t *info = NULL;
#endif
    CAData_t *cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (!cadata)
    {
        OIC_LOG(ERROR, TAG, "memory allocation failed");
        return NULL;
    }
    CAEndpoint_t* ep = CACloneEndpoint(endpoint);
    if (!ep)
    {
        OIC_LOG(ERROR, TAG, "endpoint clone failed");
        goto exit;
    }

    OIC_LOG_V(DEBUG, TAG, "origin address : %s", ep->addr);

    if (CA_RESPONSE_DATA == dataType) /* CLIENT mode */
    {
	OIC_LOG_V(DEBUG, TAG, "data type is CA_RESPONSE_DATA (inbound)");
        CAResponseInfo_t* inbound_response = (CAResponseInfo_t*)OICCalloc(1, sizeof(CAResponseInfo_t));
        if (!inbound_response)
        {
            OIC_LOG(ERROR, TAG, "memory allocation failed");
            goto exit;
        }

        CAResult_t result = CAGetResponseInfoFromPDU(pdu, inbound_response, endpoint);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CAGetResponseInfoFromPDU Failed");
            CADestroyResponseInfoInternal(inbound_response);
            goto exit;
        }
        cadata->responseInfo = inbound_response;
#if defined(DEBUG_MSGS)
        info = &inbound_response->info;
        if (identity)
        {
            info->identity = *identity;
        }
        else
        {
            OIC_LOG_V(INFO, TAG, "%s: No identity information provided", __func__);
        }
        OIC_LOG(DEBUG, TAG, "Response Info :");
        CALogPayloadInfo(info);
#endif
    }
    else if (CA_REQUEST_DATA == dataType) /* SERVER mode */
    {
	OIC_LOG_V(DEBUG, TAG, "data type is CA_REQUEST_DATA (inbound)");
        struct CARequestInfo *inbound_request = (struct CARequestInfo*)OICCalloc(1, sizeof(struct CARequestInfo));
        if (!inbound_request)
        {
            OIC_LOG(ERROR, TAG, "memory allocation failed");
            goto exit;
        }

        CAResult_t result = CAGetRequestInfoFromPDU(pdu, endpoint, inbound_request);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CAGetRequestInfoFromPDU failed");
            CADestroyRequestInfoInternal(inbound_request);
            goto exit;
        }

        if ((inbound_request->info.type != CA_MSG_CONFIRM) &&
            CADropSecondMessage(&requestHistory, endpoint, inbound_request->info.messageId,
                                inbound_request->info.token, inbound_request->info.tokenLength))
        {
            OIC_LOG(INFO, TAG, "Second Request with same Token, Drop it");
            CADestroyRequestInfoInternal(inbound_request);
            goto exit;
        }

        cadata->requestInfo = inbound_request;
#if defined(DEBUG_MSGS)
        info = &inbound_request->info;
        if (identity)
        {
            info->identity = *identity;
        }
        OIC_LOG(DEBUG, TAG, "Request Info :");
        CALogPayloadInfo(info);
#endif
    }
    else if (CA_ERROR_DATA == dataType)
    {
	OIC_LOG_V(DEBUG, TAG, "data type is CA_ERROR_DATA");
        CAErrorInfo_t *errorInfo = (CAErrorInfo_t *)OICCalloc(1, sizeof (CAErrorInfo_t));
        if (!errorInfo)
        {
            OIC_LOG(ERROR, TAG, "Memory allocation failed!");
            goto exit;
        }

        CAResult_t result = CAGetErrorInfoFromPDU(pdu, endpoint, errorInfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CAGetErrorInfoFromPDU failed");
            OICFree(errorInfo);
            goto exit;
        }

        cadata->errorInfo = errorInfo;
#if defined(DEBUG_MSGS)
        info = &errorInfo->info;
        if (identity)
        {
            info->identity = *identity;
        }
        OIC_LOG(DEBUG, TAG, "error Info :");
        CALogPayloadInfo(info);
#endif
    }
#ifdef TCP_ADAPTER
    else if (CA_SIGNALING_DATA == dataType)
    {
        CASignalingInfo_t *signalingInfo =
                (CASignalingInfo_t *)OICCalloc(1, sizeof (CASignalingInfo_t));
        if (!signalingInfo)
        {
            OIC_LOG(ERROR, TAG, "Memory allocation failed!");
            goto exit;
        }

        CAResult_t result = CAGetSignalingInfoFromPDU(data, endpoint, signalingInfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CAGetSignalingInfoFromPDU failed");
            CADestroySignalingInfoInternal(signalingInfo);
            goto exit;
        }

        cadata->signalingInfo = signalingInfo;
#if defined(DEBUG_MSGS)
        info = &signalingInfo->info;
        if (identity)
        {
            info->identity = *identity;
        }
        OIC_LOG(DEBUG, TAG, "Signaling Info :");
        CALogPayloadInfo(info);
#endif
        // Get CA_OPTION_MAX_MESSAGE_SIZE from pdu.
        unsigned char optionData[CA_MAX_HEADER_OPTION_DATA_LENGTH];
        size_t optionDataSize = sizeof(optionData);
        uint16_t receivedDataLength = 0;

#if defined(DEBUG_MSGS)
        // TODO: We need to decide what options needs to be handled and which needs to be ignored.
        if(CAGetHeaderOption(info->options, info->numOptions, CA_OPTION_MAX_MESSAGE_SIZE,
                    optionData, optionDataSize, &receivedDataLength) == CA_STATUS_OK && receivedDataLength) {
            unsigned int maxMsgSize = (unsigned int) coap_decode_var_bytes(optionData, receivedDataLength);
            OIC_LOG_V(DEBUG, TAG, "received MAX_MESSAGE_SIZE option data: %u", maxMsgSize);
            if(maxMsgSize > 1152){
                //TODO: Change the TCP sockets parameters for maxMsgSize > 1152
            }
        }
#endif
    }
#endif

    cadata->remoteEndpoint = ep;
    cadata->dataType = dataType;

    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
    return cadata;

exit:
    OICFree(cadata);
    CAFreeEndpoint(ep);
    return NULL;
}

static void CATimeoutCallback(const CAEndpoint_t *endpoint, const void *pdu, uint32_t size)
{
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL_VOID(pdu, TAG, "pdu");
    CAEndpoint_t* ep = CACloneEndpoint(endpoint);
    if (!ep)
    {
        OIC_LOG(ERROR, TAG, "clone failed");
        return;
    }

    CAResponseInfo_t* resInfo = (CAResponseInfo_t*)OICCalloc(1, sizeof(CAResponseInfo_t));

    if (!resInfo)
    {
        OIC_LOG(ERROR, TAG, "calloc failed");
        CAFreeEndpoint(ep);
        return;
    }

    resInfo->result = CA_RETRANSMIT_TIMEOUT;
    resInfo->info.type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    resInfo->info.messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    CAResult_t res = CAGetTokenFromPDU((const coap_hdr_transport_t *) pdu, &(resInfo->info),
                                       endpoint);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "fail to get Token from retransmission list");
        CADestroyResponseInfoInternal(resInfo);
        CAFreeEndpoint(ep);
        return;
    }

    CAData_t *cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (NULL == cadata)
    {
        OIC_LOG(ERROR, TAG, "memory allocation failed !");
        CAFreeEndpoint(ep);
        CADestroyResponseInfoInternal(resInfo);
        return;
    }

    cadata->outbound_routing = UNICAST;
    cadata->remoteEndpoint = ep;
    cadata->requestInfo = NULL;
    cadata->responseInfo = resInfo;

#ifdef WITH_BWT
    if (CAIsSupportedBlockwiseTransfer(endpoint->adapter))
    {
        res = CARemoveBlockDataFromListWithSeed(resInfo->info.token, resInfo->info.tokenLength,
                                                endpoint->addr, endpoint->port);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CARemoveBlockDataFromListWithSeed failed");
        }
    }
#endif // WITH_BWT

    CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
}

static void CADestroyData(void *data, uint32_t size)
{
    OIC_LOG(DEBUG, TAG, "CADestroyData IN");
    if ((size_t)size < sizeof(CAData_t))
    {
        OIC_LOG_V(ERROR, TAG, "Destroy data too small %p %d", data, size);
    }
    CAData_t *cadata = (CAData_t *) data;

    if (NULL == cadata)
    {
        OIC_LOG(ERROR, TAG, "cadata is NULL");
        return;
    }

    if (NULL != cadata->remoteEndpoint)
    {
        CAFreeEndpoint(cadata->remoteEndpoint);
    }

    if (NULL != cadata->requestInfo)
    {
        CADestroyRequestInfoInternal((struct CARequestInfo *) cadata->requestInfo);
    }

    if (NULL != cadata->responseInfo)
    {
        CADestroyResponseInfoInternal((CAResponseInfo_t *) cadata->responseInfo);
    }

    if (NULL != cadata->errorInfo)
    {
        CADestroyErrorInfoInternal(cadata->errorInfo);
    }

#ifdef TCP_ADAPTER
    if (NULL != cadata->signalingInfo)
    {
        CADestroySignalingInfoInternal(cadata->signalingInfo);
    }
#endif

    OICFree(cadata);
    OIC_LOG(DEBUG, TAG, "CADestroyData OUT");
}

static void CAReceiveThreadProcess(void *threadData)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY (g_receiveThread)", __func__);
#ifndef SINGLE_HANDLE
    CAData_t *data = (CAData_t *) threadData;
    OIC_TRACE_BEGIN(%s:CAProcessReceivedData, TAG);
    CAProcessReceivedData(data);
    OIC_TRACE_END();
#else
    (void)threadData;
#endif
}

/* send outbound multicast msg */
static CAResult_t CAProcessMulticastData(const CAData_t *data)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    VERIFY_NON_NULL_MSG(data, TAG, "data");
    VERIFY_NON_NULL_MSG(data->remoteEndpoint, TAG, "remoteEndpoint");

    OIC_LOG_V(DEBUG, TAG, "%s remote ep: %s", __func__, data->remoteEndpoint->addr);

    coap_pdu_t *_outbound_coap_pdu = NULL;
    CAInfo_t *outbound_ocf_msg = NULL;
    coap_list_t *options = NULL;
    coap_transport_t transport = COAP_UDP;
    CAResult_t res = CA_SEND_FAILED;

    if (!data->requestInfo && !data->responseInfo)
    {
        OIC_LOG(ERROR, TAG, "data contain no request nor response info");
        return res;
    }

    if (data->requestInfo)
    {
        OIC_LOG(DEBUG, TAG, "msg is outbound requestInfo");

        outbound_ocf_msg = &data->requestInfo->info;
        _outbound_coap_pdu = CAGeneratePDU(CA_GET, outbound_ocf_msg, data->remoteEndpoint, &options, &transport);
    }
    else if (data->responseInfo)
    {
        OIC_LOG(DEBUG, TAG, "msg is outbound responseInfo");

        outbound_ocf_msg = &data->responseInfo->info;
        _outbound_coap_pdu = CAGeneratePDU(data->responseInfo->result, outbound_ocf_msg, data->remoteEndpoint,
                            &options, &transport);
    }

    if (!_outbound_coap_pdu)
    {
        OIC_LOG(ERROR,TAG,"Failed to generate multicast PDU");
        CASendErrorInfo(data->remoteEndpoint, outbound_ocf_msg, CA_SEND_FAILED);
        coap_delete_list(options);
        return res;
    }

#ifdef WITH_BWT
    if (CAIsSupportedBlockwiseTransfer(data->remoteEndpoint->adapter))
    {
        // Blockwise transfer
        res = CAAddBlockOption(&_outbound_coap_pdu, outbound_ocf_msg, data->remoteEndpoint, &options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(DEBUG, TAG, "CAAddBlockOption has failed");
            goto exit;
        }
    }
#endif // WITH_BWT

    CALogPDUInfo(data, _outbound_coap_pdu);

    res = CASendMulticastData(data->remoteEndpoint,
                              _outbound_coap_pdu->transport_hdr,
                              _outbound_coap_pdu->length,
                              data->dataType);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "send failed:%d", res);
        goto exit;
    }

    coap_delete_list(options);
    coap_delete_pdu(_outbound_coap_pdu);
    return res;

exit:
    CAErrorHandler(data->remoteEndpoint, _outbound_coap_pdu->transport_hdr, _outbound_coap_pdu->length, res);
    coap_delete_list(options);
    coap_delete_pdu(_outbound_coap_pdu);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return res;
}

static CAResult_t CAProcessSendData(const CAData_t *data)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    VERIFY_NON_NULL_MSG(data, TAG, "data");
    VERIFY_NON_NULL_MSG(data->remoteEndpoint, TAG, "remoteEndpoint");

    if ((CAData_t*)data->responseInfo) {
        size_t payloadLen = 0;
        CAGetPayloadInfo(data, &payloadLen);
        OIC_LOG_V(DEBUG, TAG, "payloadLen=%" PRIuPTR, payloadLen);
        OIC_LOG_V(ERROR, TAG, "payload size: %u", ((CAResponseInfo_t *)data->responseInfo)->info.payloadSize);
        OIC_LOG_V(ERROR, TAG, "payload: %p", ((CAResponseInfo_t *)data->responseInfo)->info.payload);
    }

    CAResult_t res = CA_STATUS_FAILED;

    ROUTING_TYPE /* CASendDataType_t */ type = data->outbound_routing;

    coap_pdu_t *pdu = NULL;
    CAInfo_t *info = NULL;
    coap_list_t *options = NULL;
    coap_transport_t transport = COAP_UDP;

    /* OIC_LOG(DEBUG,TAG,"logging remoteEndpoint:"); */
    /* LogEndpoint((CAEndpoint_t*)&data->remoteEndpoint); */

    /* OIC_LOG_V(DEBUG, TAG, */
    /*           "%s sending %s %s %s", __func__, */
    /*           (data->remoteEndpoint->flags & CA_IPV4) ? "IPV4" : (data->remoteEndpoint->flags & CA_IPV6)? "IPV6" : "ERR", */
    /*           (data->remoteEndpoint->flags & CA_SECURE) ? "secure " : "insecure ", */
    /*           (data->remoteEndpoint->flags & CA_MULTICAST) ? "multicast" : "unicast"); */

    if (UNICAST == type)
    {
        OIC_LOG(DEBUG,TAG,"Unicast message");

#ifdef ROUTING_GATEWAY
        /*
         * When forwarding a packet, do not attempt retransmission as its the responsibility of
         * packet originator node
         */
        bool skipRetransmission = false;
#endif

        if (NULL != data->requestInfo)
        {
            OIC_LOG(DEBUG, TAG, "msg is OUTBOUND REQUEST");

            info = &data->requestInfo->info;
#ifdef ROUTING_GATEWAY
            skipRetransmission = data->requestInfo->info.skipRetransmission;
#endif
            pdu = CAGeneratePDU(data->requestInfo->method, info, data->remoteEndpoint,
                                &options, &transport);
        }
        else if (NULL != data->responseInfo)
        {
            OIC_LOG(DEBUG, TAG, "msg is OUTBOUND RESPONSE");
            OIC_LOG_V(DEBUG, TAG, "response result: 0x%04x", data->responseInfo->result);

            info = &data->responseInfo->info;
#ifdef ROUTING_GATEWAY
            skipRetransmission = data->responseInfo->info.skipRetransmission;
#endif
            pdu = CAGeneratePDU(data->responseInfo->result, info, data->remoteEndpoint,
                                &options, &transport);
        }
#ifdef TCP_ADAPTER
        else if (NULL != data->signalingInfo)
        {
            OIC_LOG(DEBUG, TAG, "signalingInfo is available..");
            info = &data->signalingInfo->info;
#ifdef ROUTING_GATEWAY
            skipRetransmission = data->signalingInfo->info.skipRetransmission;
#endif
            pdu = CAGeneratePDU(data->signalingInfo->code, info, data->remoteEndpoint,
                                &options, &transport);
        }
#endif
        else
        {
            OIC_LOG(DEBUG, TAG, "request info, response info is empty");
            return CA_STATUS_INVALID_PARAM;
        }

        // interface controller function call.
        if (NULL != pdu)
        {
#ifdef WITH_BWT
            if (CAIsSupportedBlockwiseTransfer(data->remoteEndpoint->adapter))
            {
                // Blockwise transfer
                if (NULL != info)
                {
                    res = CAAddBlockOption(&pdu, info, data->remoteEndpoint, &options);
                    if (CA_STATUS_OK != res)
                    {
                        OIC_LOG(INFO, TAG, "to write block option has failed");
                        CAErrorHandler(data->remoteEndpoint, pdu->transport_hdr, pdu->length, res);
                        coap_delete_list(options);
                        coap_delete_pdu(pdu);
                        return res;
                    }
                }
            }
#endif // WITH_BWT
            OIC_LOG_V(INFO, TAG, "%s logging pdu:", __func__);
            CALogPDUInfo(data, pdu);

            OIC_LOG_V(INFO, TAG, "CASendUnicastData type : %d",
                      (data->dataType == CA_REQUEST_DATA)? "REQUEST"
                      :(data->dataType == CA_RESPONSE_DATA)? "RESPONSE"
                      : "OTHER");
            res = CASendUnicastData(data->remoteEndpoint, pdu->transport_hdr, pdu->length, data->dataType);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG_V(ERROR, TAG, "send failed:%d", res);
                CAErrorHandler(data->remoteEndpoint, pdu->transport_hdr, pdu->length, res);
                coap_delete_list(options);
                coap_delete_pdu(pdu);
                return res;
            }

#ifdef WITH_TCP
            if (CAIsSupportedCoAPOverTCP(data->remoteEndpoint->adapter))
            {
                OIC_LOG(INFO, TAG, "retransmission will be not worked");
            }
            else
#endif
#ifdef ROUTING_GATEWAY
            if (!skipRetransmission)
#endif
            {
                // for retransmission
                res = CARetransmissionSentData(&g_retransmissionContext,
                                               data->remoteEndpoint,
                                               data->dataType,
                                               pdu->transport_hdr, pdu->length);
                if ((CA_STATUS_OK != res) && (CA_NOT_SUPPORTED != res))
                {
                    //when retransmission not supported this will return CA_NOT_SUPPORTED, ignore
                    OIC_LOG_V(INFO, TAG, "retransmission is not enabled due to error, res : %d", res);
                    coap_delete_list(options);
                    coap_delete_pdu(pdu);
                    return res;
                }
            }

            coap_delete_list(options);
            coap_delete_pdu(pdu);
        }
        else
        {
            OIC_LOG(ERROR,TAG,"Failed to generate unicast PDU");
            CASendErrorInfo(data->remoteEndpoint, info, CA_SEND_FAILED);
            return CA_SEND_FAILED;
        }
    }
    else if (MULTICAST == type)
    {
        OIC_LOG(DEBUG,TAG,"Multicast message");
        data->remoteEndpoint->flags = data->remoteEndpoint->flags | CA_MULTICAST;
        OIC_LOG_V(DEBUG, TAG,
                  "%s sending %s %s %s", __func__,
                  (data->remoteEndpoint->flags & CA_IPV4) ? "IPV4" : (data->remoteEndpoint->flags & CA_IPV6)? "IPV6" : "ERR",
                  (data->remoteEndpoint->flags & CA_SECURE) ? "secure " : "insecure ",
                  (data->remoteEndpoint->flags & CA_MULTICAST) ? "multicast" : "unicast");

#ifdef WITH_TCP
        /*
         * If CoAP over TCP is enabled, the CoAP pdu wont be same for IP and other adapters.
         * That's why we need to generate two pdu's, one for IP and second for other transports.
         * Two possible cases we might have to split: a) when adapter is CA_DEFAULT_ADAPTER
         * b) when one of the adapter is IP adapter(ex: CA_ADAPTER_IP | CA_ADAPTER_GATT_BTLE)
         */
        if (data->remoteEndpoint->adapter == CA_DEFAULT_ADAPTER ||
                (CA_ADAPTER_IP & data->remoteEndpoint->adapter &&
                    CA_ADAPTER_IP != data->remoteEndpoint->adapter))
        {
            if (data->remoteEndpoint->adapter == CA_DEFAULT_ADAPTER)
            {
                data->remoteEndpoint->adapter = CA_ALL_ADAPTERS ^ CA_ADAPTER_IP;
            }
            else
            {
                data->remoteEndpoint->adapter = data->remoteEndpoint->adapter ^ CA_ADAPTER_IP;
            }
            CAProcessMulticastData(data); /* send to all except IP (UDP?) */
            data->remoteEndpoint->adapter = CA_ADAPTER_IP;
            CAProcessMulticastData(data); /* send to IP (UDP?) */
        }
        else
        {
            CAProcessMulticastData(data);
        }
#else
        CAProcessMulticastData(data);
#endif
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

static void CASendThreadProcess(void *threadData)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY (g_sendThread task)", __func__);
    CAData_t *data = (CAData_t *) threadData;
    OIC_TRACE_BEGIN(%s:CAProcessSendData, TAG);
    CAProcessSendData(data);
    OIC_TRACE_END();
}

/*
 * If a second message arrives with the same message ID, token and the other address
 * family, drop it.  Typically, IPv6 beats IPv4, so the IPv4 message is dropped.
 */
LOCAL bool CADropSecondMessage(CAHistory_t *history, const CAEndpoint_t *ep, uint16_t id,
                               uint8_t *token, uint8_t tokenLength)
{
    if (!ep)
    {
        return true;
    }
    if (ep->adapter != CA_ADAPTER_IP)
    {
        return false;
    }

    if (tokenLength > CA_MAX_TOKEN_LEN)
    {
        /*
         * If token length is more than CA_MAX_TOKEN_LEN,
         * we compare the first CA_MAX_TOKEN_LEN bytes only.
         */
        tokenLength = CA_MAX_TOKEN_LEN;
    }

    bool ret = false;
    CATransportFlags_t familyFlags = ep->flags & CA_IPFAMILY_MASK;

    for (size_t i = 0; i < sizeof(history->items) / sizeof(history->items[0]); i++)
    {
        CAHistoryItem_t *item = &(history->items[i]);
        if (id == item->messageId && tokenLength == item->tokenLength
            /* && ep->ifindex == item->ifindex */
            && memcmp(item->token, token, tokenLength) == 0)
        {
            OIC_LOG_V(INFO, TAG, "IPv%c duplicate message ignored",
                      familyFlags & CA_IPV6 ? '6' : '4');
            ret = true;
            break;
        }
    }

    history->items[history->nextIndex].flags = familyFlags;
    history->items[history->nextIndex].messageId = id;
    /* history->items[history->nextIndex].ifindex = ep->ifindex; */
    if (token && tokenLength)
    {
        memcpy(history->items[history->nextIndex].token, token, tokenLength);
        history->items[history->nextIndex].tokenLength = tokenLength;
    }

    if (++history->nextIndex >= HISTORYSIZE)
    {
        history->nextIndex = 0;
    }

    return ret;
}

/* called by both UDP and TCP; converts data and puts on recv queue */
/* called in ca_adapter_net_ssl.c via g_caSslContext->adapterCallbacks[adapterIndex].recvCallback(&peer->sep, decryptBuffer, ret); */
/* called in udp_data_receiver.c via g_packetReceivedCallback(&sep, recvBuffer, recvLen); */
/* called in tcp_data_receiver.c via tcp_networkPacketCallback(sep, svritem->data, svritem->totalLen); */
/* client-sde, lower-level inbound request/response handler */
void mh_CAReceivedPacketCallback(const CASecureEndpoint_t *origin_sep, // @was CAReceivedPacketCallback
				 const void *dgram_payload,
				 size_t dataLen) EXPORT
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY >>>>>>>>>>>>>>>>", __func__);
    VERIFY_NON_NULL_VOID(origin_sep, TAG, "remoteEndpoint");
    VERIFY_NON_NULL_VOID(dgram_payload, TAG, "dgram_payload");
    OIC_TRACE_BEGIN(%s:mh_CAReceivedPacketCallback, TAG);

    OIC_LOG_V(DEBUG, TAG, "logging secure ep:");
    LogSecureEndpoint(origin_sep);

    if (0 == dataLen)
    {
        OIC_LOG(ERROR, TAG, "dataLen is zero");
        OIC_TRACE_END();
        return;
    }

    /* code is CoAP code field, for Method (request) or Response (response) (c.dd */
    /* see section 12.1 of RFC 7252 */
    /* Iotivity uses its own 32-bit fld */
    uint32_t code = CA_NOT_FOUND;
    CAData_t *cadata = NULL;

#ifdef ENABLE_TCP
    OIC_LOG_V(DEBUG, TAG, "msg length: %d", coap_get_total_message_length((const unsigned char*)dgram_payload, dataLen));
#endif
    OIC_LOG(ERROR, TAG, "parse inbound dgram as coap pdu:");
    coap_pdu_t *pdu = (coap_pdu_t *) _oocf_dgram_payload_to_coap_pdu((const char *) dgram_payload, dataLen, &code, &(origin_sep->endpoint));

    log_coap_msg_code(pdu, origin_sep->endpoint.adapter);

    if (NULL == pdu) {
        OIC_LOG(ERROR, TAG, "Parse PDU failed");
        goto exit;
    }
    OIC_LOG_V(DEBUG, TAG, "coap msg code = %d", code);

    if (pdu->transport_hdr->udp.code == 0) {
        OIC_LOG(ERROR, TAG, "Ignoring EMPTY response msg");
        goto exit;
    }

#ifdef TCP_ADAPTER
    if (CA_ADAPTER_TCP == origin_sep->endpoint.adapter && CA_CSM != code)
    {
        CACSMExchangeState_t csmState = CAGetCSMState(&origin_sep->endpoint);
        if (csmState != RECEIVED && csmState != SENT_RECEIVED)
        {
            CATCPDisconnectSession(&origin_sep->endpoint);
            OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, first message is not a CSM!!");
        }
    }
#endif

    log_coap_pdu_hdr(pdu, origin_sep->endpoint.adapter);
    /* CoAP code field determines which type of msg */
    //if (CA_GET == code || CA_POST == code || CA_PUT == code || CA_DELETE == code)
    if (COAP_MESSAGE_IS_REQUEST(&pdu->transport_hdr->udp)) {
        /*  SERVER mode */
        OIC_LOG_V(DEBUG, TAG, "msg is INBOUND REQUEST", __func__);
        cadata = _oocf_coap_pdu_to_msg(&(origin_sep->endpoint), &(origin_sep->identity), pdu, CA_REQUEST_DATA);
        if (!cadata)
            {
                // FIXME: use errno to indicate dropped duplicat msg
                OIC_LOG(ERROR, TAG, "mh_CAReceivedPacketCallback, CAGenerateHandlerData failed!");
                coap_delete_pdu(pdu);
                goto exit;
            }
        //} else if (CA_RESPONSE_CLASS(pdu->transport_hdr->udp.code)) {
    } else if (COAP_MESSAGE_IS_RESPONSE(&pdu->transport_hdr->udp)) {
        cadata = _oocf_coap_pdu_to_msg(&(origin_sep->endpoint), &(origin_sep->identity), pdu, CA_RESPONSE_DATA);
        if (!cadata)
            {
                OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, CAGenerateHandlerData failed!");
                coap_delete_pdu(pdu);
                goto exit;
            }

#ifdef WITH_TCP
        if (CAIsSupportedCoAPOverTCP(origin_sep->endpoint.adapter))
            {
                OIC_LOG(INFO, TAG, "CoAP/TCP retransmission is not supported");
            }
        else
#endif
            {
                // for retransmission
                void *retransmissionPdu = NULL;
                CARetransmissionReceivedData(&g_retransmissionContext, cadata->remoteEndpoint, pdu->transport_hdr,
                                             pdu->length, &retransmissionPdu);

                // get token from saved data in retransmission list
                if (retransmissionPdu && CA_EMPTY == code)
                    {
                        if (cadata->responseInfo)
                            {
                                CAInfo_t *info = &cadata->responseInfo->info;
                                CAResult_t res = CAGetTokenFromPDU((const coap_hdr_transport_t *)retransmissionPdu,
                                                                   info, &(origin_sep->endpoint));
                                if (CA_STATUS_OK != res)
                                    {
                                        OIC_LOG(ERROR, TAG, "fail to get Token from retransmission list");
                                        OICFree(info->token);
                                        info->tokenLength = 0;
                                    }
                            }
                    }
                OICFree(retransmissionPdu);
            }
    }
#ifdef TCP_ADAPTER
    // This is signaling message (inbound?).
    else if (CA_ADAPTER_TCP == origin_sep->endpoint.adapter &&
             (CA_CSM == code || CA_PING == code || CA_PONG == code
              || CA_RELEASE == code || CA_ABORT == code)) {

            OIC_LOG_V(DEBUG, TAG, "%s msg is INBOUND SIGNAL MSG, rc: %s", __func__,
                      coap_response_phrase(pdu->transport_hdr->udp.code));

            cadata = _oocf_coap_pdu_to_msg(&(origin_sep->endpoint), &(origin_sep->identity),
                                           pdu, CA_SIGNALING_DATA);
       if (!cadata)
                {
                    OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, CAGenerateHandlerData failed!");
                    coap_delete_pdu(pdu);
                    return;
                }

            // Received signaling message is CSM.
            if (CA_CSM == code)
                {
                    OIC_LOG_V(DEBUG, TAG, "CAReceivedPacketCallback, CSM received");
                    // update session info of tcp_adapter.
                    // TODO check if it is a valid CSM, if it is not valid message disconnect the session.
                    CACSMExchangeState_t csmState = CAGetCSMState(&origin_sep->endpoint);
                    if (csmState == NONE)
                        {
                            CAUpdateCSMState(&origin_sep->endpoint,RECEIVED);
                        }
                    else if (csmState == SENT)
                        {
                            CAUpdateCSMState(&origin_sep->endpoint,SENT_RECEIVED);
                        }
                }
            else if (CA_PING == code)
                {
                    CASendPongMessage(cadata->remoteEndpoint, false, cadata->signalingInfo->info.token,
                                      cadata->signalingInfo->info.tokenLength);
                }
            else if(CA_PONG == code)
                {
                    CAPongReceivedCallback(cadata->remoteEndpoint, cadata->signalingInfo->info.token,
                                           cadata->signalingInfo->info.tokenLength);
                }
            return;
    }
#endif
    cadata->outbound_routing = UNICAST;

#if defined(DEBUG_MSGS)
    CALogPDUInfo(cadata, pdu);
#endif

#ifdef WITH_BWT
    if (CAIsSupportedBlockwiseTransfer(origin_sep->endpoint.adapter))
    {
        CAResult_t res = CAReceiveBlockWiseData(pdu, &(origin_sep->endpoint), cadata, dataLen);
        if (CA_NOT_SUPPORTED == res || CA_REQUEST_TIMEOUT == res)
        {
	    if (CA_NOT_SUPPORTED == res)
		OIC_LOG(DEBUG, TAG, "this message does not have block option");
	    else
		OIC_LOG(DEBUG, TAG, "BlockWise receive TIMEOUT");
            CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
        }
        else
        {
            CADestroyData(cadata, sizeof(CAData_t));
        }
    }
    else
#endif
    {
        CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
    }

    coap_delete_pdu(pdu);

exit:
    // OIC_LOG(DEBUG, TAG, "received pdu dgram_payload :");
    OIC_LOG_PAYLOAD_BUFFER(DEBUG, TAG,  dgram_payload, dataLen);

    OIC_TRACE_END();
    OIC_LOG_V(DEBUG, TAG, "%s EXIT <<<<<<<<<<<<<<<<", __func__);
}

/* ocf (upper) runloop handler */
void oocf_handle_inbound_messages() // @was CAHandleRequestResponseCallbacks
{
#ifdef SINGLE_HANDLE
    // parse the data and call the callbacks.
    // #1 parse the data
    // #2 get endpoint

    OIC_LOG_V(INFO, TAG, "%s ENTRY (main runloop iteration)", __func__);

    oc_mutex_lock(g_receiveThread.threadMutex);

    u_queue_message_t *item = u_queue_get_element(g_receiveThread.dataQueue);

    oc_mutex_unlock(g_receiveThread.threadMutex);

    if (NULL == item || NULL == item->msg)
    {
        OIC_LOG_V(INFO, TAG, "%s EXIT (no msg ready)", __func__);
        return;
    }
    OIC_LOG_V(INFO, TAG, "%s pulled inbound msg from g_receiveThread queue", __func__);

    // get endpoint
    CAData_t *td = (CAData_t *) item->msg;

    OIC_LOG_V(DEBUG, TAG, "inbound msg CA_DATA type: %d: %s", td->dataType,
	      (CA_REQUEST_DATA == td->dataType)? "CA REQUEST"
	      :(CA_RESPONSE_DATA == td->dataType)? "CA RESPONSE"
	      :(CA_ERROR_DATA == td->dataType)? "CA ERROR"
	      :(CA_RESPONSE_FOR_RES == td->dataType)? "CA RESPONSE_FOR_RES"
	      : "UNKNOWN");

    if (CA_REQUEST_DATA == td->dataType) {
        OIC_LOG_V(DEBUG, TAG, "inbound msg origin EP: [%s]:%d ",
                  td->remoteEndpoint->addr, td->remoteEndpoint->port);
        OIC_LOG_V(DEBUG, TAG, "inbound msg dest EP: [%s]:%d ",
                  td->requestInfo->dest_ep.addr, td->requestInfo->dest_ep.port);
    }
    if (CA_RESPONSE_DATA == td->dataType) {
        OIC_LOG_V(DEBUG, TAG, "inbound msg origin EP: [%s]:%d ",
                  td->remoteEndpoint->addr, td->remoteEndpoint->port);
        OIC_LOG_V(DEBUG, TAG, "inbound msg dest EP: [%s]",
                  td->responseInfo->info.resourceUri);
    }
    if (CA_ERROR_DATA == td->dataType) {
        /* outbound? */
    }
    if (CA_RESPONSE_FOR_RES == td->dataType) {
    }

    if (td->requestInfo) // && g_requestHandler)
    {			 /* inbound requests: SERVER mode? */
        OIC_LOG_V(DEBUG, TAG, "inbound REQUEST callback option count: %d", td->requestInfo->info.numOptions);
	OIC_LOG_V(DEBUG, TAG, "inbound REQUEST method: %d", td->requestInfo->method); // get=1,post,put,delete
	OIC_LOG_V(DEBUG, TAG, "inbound REQUEST isMcast? %d", td->requestInfo->isMulticast);
	OIC_LOG_V(DEBUG, TAG, "inbound REQUEST subject id: %s", td->requestInfo->info.identity.id);
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
        /* g_requestHandler(td->remoteEndpoint, td->requestInfo); */
	SRMRequestHandler(td->remoteEndpoint, td->requestInfo);
#else
#endif
    }
    else if (td->responseInfo) // && g_responseHandler)
    {			       /* inbound response, CLIENT mode */
        OIC_LOG_V(DEBUG, TAG, "inbound RESPONSE callback option count: %d", td->responseInfo->info.numOptions);
	OIC_LOG_V(DEBUG, TAG, "inbound RESPONSE result: %d", td->responseInfo->result);
	OIC_LOG_V(DEBUG, TAG, "inbound RESPONSE isMcast? %d", td->responseInfo->isMulticast);
	OIC_LOG_V(DEBUG, TAG, "inbound RESPONSE result: %u", td->responseInfo->result);
	OIC_LOG_V(DEBUG, TAG, "inbound RESPONSE payload len: %u", td->responseInfo->info.payloadSize);
	OIC_LOG_V(DEBUG, TAG, "inbound RESPONSE payload ptr: %p", td->responseInfo->info.payload);
        //g_responseHandler(td->remoteEndpoint, td->responseInfo);
        HandleCAResponses(td->remoteEndpoint, td->responseInfo);
    }
    else if (td->errorInfo) // && g_errorHandler)
    {
        OIC_LOG_V(DEBUG, TAG, "ERROR callback error: %d", td->errorInfo->result);
        /* g_errorHandler(td->remoteEndpoint, td->errorInfo); */
	// FIXME: rename to e.g. receiver_error_handler
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
	// FIXME: SRMErrorHandler=>gErrorHandler == HandleCAErrorResponse
	// SRMErrorHandler(td->remoteEndpoint, td->errorInfo);
	HandleCAErrorResponse(td->remoteEndpoint, td->errorInfo);
#else
	HandleCAErrorResponse(td->remoteEndpoint, td->errorInfo);
#endif
    }

    CADestroyData(item->msg, sizeof(CAData_t));
    OICFree(item);

#endif // SINGLE_HANDLE
    /* OIC_LOG_V(DEBUG, TAG, "%s <<<<<<<<<<<<<<<< EXIT <<<<<<<<<<<<<<<<", __func__); */
}

CAData_t* CAPrepareSendData(const CAEndpoint_t *endpoint,
                            const void *sendData, /* may be requestInfo, responseInfo, signalingInfo */
                            CADataType_t dataType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_LOG_V(ERROR, TAG, "payload size: %u", ((struct CARequestInfo *)sendData)->info.payloadSize);

    CAData_t *cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (!cadata)
    {
        OIC_LOG(ERROR, TAG, "memory allocation failed");
        return NULL;
    }

    if (CA_REQUEST_DATA == dataType)
    {
        OIC_LOG_V(DEBUG, TAG, "%s type == REQUEST", __func__);
        // clone request info
        struct CARequestInfo *request = CACloneRequestInfo((struct CARequestInfo *)sendData);
        if (!request)
        {
            OIC_LOG(ERROR, TAG, "CACloneRequestInfo failed");
            goto exit;
        }
        cadata->outbound_routing = request->isMulticast ? MULTICAST : UNICAST;
        cadata->requestInfo =  request;
    }
    else if (CA_RESPONSE_DATA == dataType || CA_RESPONSE_FOR_RES == dataType)
    {
        OIC_LOG_V(DEBUG, TAG, "%s type == RESPONSE", __func__);

        // clone response info
        CAResponseInfo_t *response = CACloneResponseInfo((CAResponseInfo_t *)sendData);
        if(!response)
        {
            OIC_LOG(ERROR, TAG, "CACloneResponseInfo failed");
            goto exit;
        }
        cadata->outbound_routing = response->isMulticast ? MULTICAST : UNICAST;
        cadata->responseInfo = response;
        OIC_LOG_V(DEBUG, TAG, "result: %u", response->result);

    }
#ifdef TCP_ADAPTER
    else if (CA_SIGNALING_DATA == dataType)
    {
        // clone signaling info
        CASignalingInfo_t *signaling = CACloneSignalingInfo((CASignalingInfo_t *) sendData);
        if (!signaling)
        {
            OIC_LOG(ERROR, TAG, "CACloneSignalingInfo failed");
            goto exit;
        }
        cadata->outbound_routing = UNICAST;
        cadata->signalingInfo = signaling;
    }
#endif
    else
    {
        OIC_LOG(ERROR, TAG, "CAPrepareSendData unknown data type");
        goto exit;
    }

    CAEndpoint_t* ep = CACloneEndpoint(endpoint);
    if (!ep)
    {
        OIC_LOG(ERROR, TAG, "endpoint clone failed");
        goto exit;
    }
    cadata->remoteEndpoint = ep;
    cadata->dataType = dataType;
    OIC_LOG_V(DEBUG, TAG, "%s EXIT OK", __func__);
    return cadata;

exit:
    OIC_LOG_V(DEBUG, TAG, "%s EXIT ERR", __func__);
    CADestroyData(cadata, sizeof(CAData_t));
    return NULL;
}

CAResult_t CADetachSendMessage(const CAEndpoint_t *dest_ep,
                               const void *sendMsg, /* either requestInfo or responseInfo */
                               CADataType_t dataType)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL_MSG(dest_ep, TAG, "dest_ep");
    VERIFY_NON_NULL_MSG(sendMsg, TAG, "sendMsg");

    OIC_LOG_V(INFO, TAG, "%s logging dest_ep:");
    LogEndpoint(dest_ep);

    // we always have at least one transport available!
    /* if (false == CAIsSelectedNetworkAvailable()) */
    /* { */
    /*     return CA_STATUS_FAILED; */
    /* } */

    CAData_t *data = NULL;
#ifdef TCP_ADAPTER
    if (CA_ADAPTER_TCP == dest_ep->adapter)
    {
        // #1. Try to find session info from tcp_adapter.
        CACSMExchangeState_t CSMstate = CAGetCSMState(dest_ep);
        if (CSMstate != SENT && CSMstate != SENT_RECEIVED)
        {
            // #2. Generate CSM message
            OIC_LOG_V(DEBUG, TAG, "Generate CSM message for [%s:%d]",
                      dest_ep->addr, dest_ep->port);

            // TODO: We need to decide what options needs to be sent Initially?
            //       Right now we're sending CA_OPTION_MAX_MESSAGE_SIZE as 1152,
            //       which is default according to the RFC.
            uint8_t numOptions = 0;
            CAHeaderOption_t *csmOpts = NULL;
            unsigned int maxMsgSize = 1152;
            unsigned char optionData[CA_MAX_HEADER_OPTION_DATA_LENGTH] = { 0 };
            size_t optionDataLength = coap_encode_var_bytes(optionData, maxMsgSize);
            CAAddHeaderOption(&csmOpts, &numOptions, CA_OPTION_MAX_MESSAGE_SIZE,
                              optionData, optionDataLength);

            data = CAGenerateSignalingMessage(dest_ep, CA_CSM, csmOpts, numOptions);
            if (!data)
            {
                OIC_LOG(ERROR, TAG, "GenerateSignalingMessage failed");
                return CA_STATUS_FAILED;
            }
            OICFree(csmOpts);

            // #3. Add pdu to send queue.
            CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));
        }
    }
#endif

    data = CAPrepareSendData(dest_ep, sendMsg, dataType);
    if(!data)
    {
        OIC_LOG(ERROR, TAG, "CAPrepareSendData failed");
        return CA_MEMORY_ALLOC_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "device ID of dest_ep of this message is: \"%s\"", dest_ep->remoteId);

#if defined(TCP_ADAPTER) && defined(WITH_CLOUD)
    CAResult_t ret = CACMGetMessageData(data);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(DEBUG, TAG, "Ignore ConnectionManager");
    }
#endif

    if (UNICAST == data->outbound_routing && CAIsLocalEndpoint(data->remoteEndpoint))
    {
        OIC_LOG(DEBUG, TAG,
                "This is a loopback message. Transfer it to the receive queue directly");
        CAQueueingThreadAddData(&g_receiveThread, data, sizeof(CAData_t));
        return CA_STATUS_OK;
    }
#ifdef WITH_BWT
    if (CAIsSupportedBlockwiseTransfer(dest_ep->adapter))
    {
        // send block data
        CAResult_t res = CASendBlockWiseData(data);
        if (CA_NOT_SUPPORTED == res)
        {
            OIC_LOG(DEBUG, TAG, "normal msg will be sent");
            CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));
            return CA_STATUS_OK;
        }
        else
        {
            CADestroyData(data, sizeof(CAData_t));
        }
        return res;
    }
    else
#endif // WITH_BWT
    {
        CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));
    }

    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

void CASetInterfaceCallbacks(CARequestCallback ReqHandler, CAResponseCallback RespHandler,
                             CAErrorCallback errorHandler)
{
    //g_requestHandler = ReqHandler;
    // g_responseHandler = RespHandler;
    // g_errorHandler = errorHandler;
}

void CASetNetworkMonitorCallback(CANetworkMonitorCallback nwMonitorHandler)
{
    g_nwMonitorHandler = nwMonitorHandler;
}

CAResult_t CAInitializeMessageHandler(CATransportAdapter_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    //@rewrite remove CASetPacketReceivedCallback(mh_CAReceivedPacketCallback);
    CASetErrorHandleCallback(CAErrorHandler);

    // create thread pool
    CAResult_t res = ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &g_threadPoolHandle);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread pool initialize error.");
        return res;
    }

    // send thread initialize
#ifdef DEBUG_THREADS
    g_sendThread.name = "g_sendThread";
#endif
    res = CAQueueingThreadInitialize(&g_sendThread,
				     g_threadPoolHandle,
                                     CASendThreadProcess,
				     CADestroyData);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        return res;
    }

    // start send thread
    res = CAQueueingThreadStart(&g_sendThread);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread start error(send thread).");
        return res;
    }

    // receive thread initialize
#ifdef DEBUG_THREADS
    g_receiveThread.name = "g_receiveThread";
#endif
    res = CAQueueingThreadInitialize(&g_receiveThread,
				     g_threadPoolHandle,
                                     CAReceiveThreadProcess,
				     CADestroyData);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize receive queue thread");
        return res;
    }

#ifndef SINGLE_HANDLE // This will be enabled when RI supports multi threading
    // start receive thread
    res = CAQueueingThreadStart(&g_receiveThread);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread start error(receive thread).");
        return res;
    }
#endif // SINGLE_HANDLE

    // retransmission initialize
    res = CARetransmissionInitialize(&g_retransmissionContext, g_threadPoolHandle,
                                     CASendUnicastData, CATimeoutCallback, NULL);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize Retransmission.");
        return res;
    }

#ifdef WITH_BWT
    // block-wise transfer initialize
    res = CAInitializeBlockWiseTransfer(CAAddDataToSendThread, CAAddDataToReceiveThread);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize BlockWiseTransfer.");
        return res;
    }
#endif

    // start retransmission
    res = CARetransmissionStart(&g_retransmissionContext);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread start error(retransmission thread).");
        return res;
    }

    // initialize interface adapters by controller
    res = CAInitializeAdapters(g_threadPoolHandle, transportType);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize Adapters.");
        return res;
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

void CATerminateMessageHandler(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    // stop adapters
    CAStopAdapters();

    // stop retransmission
    if (NULL != g_retransmissionContext.threadMutex)
    {
        CARetransmissionStop(&g_retransmissionContext);
    }

    // stop thread
    // delete thread data
    if (NULL != g_sendThread.threadMutex)
    {
        CAQueueingThreadStop(&g_sendThread);
    }

    // stop thread
    // delete thread data
    if (NULL != g_receiveThread.threadMutex)
    {
      //#ifndef SINGLE_HANDLE // This will be enabled when RI supports multi threading
        CAQueueingThreadStop(&g_receiveThread);
	//#endif
    }

    // destroy thread pool
    if (NULL != g_threadPoolHandle)
    {
        ca_thread_pool_free(g_threadPoolHandle);
        g_threadPoolHandle = NULL;
    }

#ifdef WITH_BWT
    CATerminateBlockWiseTransfer();
#endif
    CARetransmissionDestroy(&g_retransmissionContext);
    CAQueueingThreadDestroy(&g_sendThread);
    CAQueueingThreadDestroy(&g_receiveThread);

    // terminate interface adapters by controller
    CATerminateAdapters();
}

#if defined(DEBUG_MSGS)
LOCAL void CALogPayloadInfo(CAInfo_t *info)
{
    if (info)
    {
        if (info->options)
        {
            for (uint32_t i = 0; i < info->numOptions; i++)
            {
                OIC_LOG_V(DEBUG, TAG, "optionID: %u", info->options[i].optionID);

                OIC_LOG_V(DEBUG, TAG, "list: %s", info->options[i].optionData);
            }
        }

        if (info->payload)
        {
            OIC_LOG_V(DEBUG, TAG, "payload: %p(%" PRIuPTR ")", info->payload,
                      info->payloadSize);
        }

        if (info->token)
        {
            OIC_LOG(DEBUG, TAG, "token:");
            OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *) info->token,
                           info->tokenLength);
        }
        OIC_LOG_V(DEBUG, TAG, "msgID: %u", info->messageId);
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "info is NULL, cannot output log data");
    }
}
#endif

void CAErrorHandler(const CAEndpoint_t *endpoint,
                    const void *dgram_payload,
                    size_t dataLen,
                    CAResult_t result)
{
    OIC_LOG(DEBUG, TAG, "CAErrorHandler IN");
    VERIFY_NON_NULL_VOID(endpoint, TAG, "remoteEndpoint");
    VERIFY_NON_NULL_VOID(dgram_payload, TAG, "dgram_payload");

    if (0 == dataLen)
    {
        OIC_LOG(ERROR, TAG, "dataLen is zero");
        return;
    }

    uint32_t code = CA_NOT_FOUND;
    //Do not free remoteEndpoint and dgram_payload. Currently they will be freed in data thread
    //Get PDU from dgram_payload
    coap_pdu_t *pdu = (coap_pdu_t *)_oocf_dgram_payload_to_coap_pdu((const char *)dgram_payload, dataLen, &code, endpoint);
    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "Parse PDU failed");
        return;
    }

    CAData_t *cadata = _oocf_coap_pdu_to_msg(endpoint, NULL, pdu, CA_ERROR_DATA);
    if (!cadata)
    {
        OIC_LOG(ERROR, TAG, "CAErrorHandler, CAGenerateHandlerData failed!");
        coap_delete_pdu(pdu);
        return;
    }

#ifdef WITH_TCP
    if (CAIsSupportedCoAPOverTCP(endpoint->adapter))
    {
        OIC_LOG(INFO, TAG, "retransmission is not supported");
    }
    else
#endif
    {
        //Fix up CoAP message to adjust it to current retransmission implementation
        coap_hdr_t *hdr = (coap_hdr_t *)(pdu->transport_hdr);
        hdr->type = CA_MSG_RESET;
        hdr->code = CA_EMPTY;

        // for retransmission
        void *retransmissionPdu = NULL;
        CARetransmissionReceivedData(&g_retransmissionContext, cadata->remoteEndpoint,
                                     pdu->transport_hdr, pdu->length, &retransmissionPdu);

        // get token from saved data in retransmission list
        if (retransmissionPdu && cadata->errorInfo)
        {
            CAInfo_t *info = &cadata->errorInfo->info;
            CAResult_t res = CAGetTokenFromPDU((const coap_hdr_transport_t *)retransmissionPdu,
                                               info, endpoint);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "fail to get Token from retransmission list");
                OICFree(info->token);
                info->tokenLength = 0;
            }
        }
        OICFree(retransmissionPdu);
    }

    cadata->errorInfo->result = result;

    CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
    coap_delete_pdu(pdu);

    OIC_LOG(DEBUG, TAG, "CAErrorHandler OUT");
    return;
}

LOCAL void CASendErrorInfo(const CAEndpoint_t *endpoint, const CAInfo_t *info, CAResult_t result)
{
    OIC_LOG(DEBUG, TAG, "CASendErrorInfo IN");
    CAData_t *cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (!cadata)
    {
        OIC_LOG(ERROR, TAG, "cadata memory allocation failed");
        return;
    }

    CAEndpoint_t* ep = CACloneEndpoint(endpoint);
    if (!ep)
    {
        OIC_LOG(ERROR, TAG, "endpoint clone failed");
        OICFree(cadata);
        return;
    }

    CAErrorInfo_t *errorInfo = (CAErrorInfo_t *)OICCalloc(1, sizeof (CAErrorInfo_t));
    if (!errorInfo)
    {
        OIC_LOG(ERROR, TAG, "errorInfo memory allocation failed");
        OICFree(cadata);
        CAFreeEndpoint(ep);
        return;
    }

    CAResult_t res = CACloneInfo(info, &errorInfo->info);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "info clone failed");
        OICFree(cadata);
        OICFree(errorInfo);
        CAFreeEndpoint(ep);
        return;
    }

    errorInfo->result = result;
    cadata->remoteEndpoint = ep;
    cadata->errorInfo = errorInfo;
    cadata->dataType = CA_ERROR_DATA;

    CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
    OIC_LOG(DEBUG, TAG, "CASendErrorInfo OUT");
}


/**
 * print send / receive message of CoAP.
 * @param[in] data      CA information which has send/receive message and endpoint.
 * @param[in] pdu       CoAP pdu low data.
 */
LOCAL void CALogPDUInfo(const CAData_t *data, const coap_pdu_t *pdu)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>", __func__);

    VERIFY_NON_NULL_VOID(data, TAG, "data");
    VERIFY_NON_NULL_VOID(pdu, TAG, "pdu");
    OIC_TRACE_BEGIN(%s:CALogPDUInfo, TAG);

    OIC_LOG_V(INFO, TAG, "%s type: %s", __func__,
	      (MULTICAST == data->outbound_routing)
	      ? "MULTICAST"
	      :(UNICAST == data->outbound_routing)
	      ? "UNICAST"
	      : "ERROR-UKNOWN TYPE");

    if (NULL != data->remoteEndpoint)
    {
        CALogAdapterTypeInfo(data->remoteEndpoint->adapter);
        OIC_LOG_V(DEBUG, ANALYZER_TAG, "Address = [%s]:[%d]", data->remoteEndpoint->addr,
                  data->remoteEndpoint->port);
    }

    switch(data->dataType)
    {
        case CA_REQUEST_DATA:
            OIC_LOG(DEBUG, ANALYZER_TAG, "Data Type = [CA_REQUEST_DATA]");
            OIC_LOG_V(DEBUG, ANALYZER_TAG, "identity = ", data->requestInfo->info.identity);
            break;
        case CA_RESPONSE_DATA:
            OIC_LOG(DEBUG, ANALYZER_TAG, "Data Type = [CA_RESPONSE_DATA]");
            OIC_LOG_V(DEBUG, ANALYZER_TAG, "identity = ", data->responseInfo->info.identity);
            break;
        case CA_ERROR_DATA:
            OIC_LOG(DEBUG, ANALYZER_TAG, "Data Type = [CA_ERROR_DATA]");
            break;
        case CA_RESPONSE_FOR_RES:
            OIC_LOG(DEBUG, ANALYZER_TAG, "Data Type = [CA_RESPONSE_FOR_RES]");
            break;
        default:
            OIC_LOG_V(DEBUG, ANALYZER_TAG, "Data Type = [%d]", data->dataType);
            break;
    }
    OIC_LOG(DEBUG, ANALYZER_TAG, "-------------------------------------------------");

    const CAInfo_t *info = NULL;
    if (NULL != data->requestInfo)
    {
        switch(data->requestInfo->method)
        {
            case CA_GET:
                OIC_LOG(DEBUG, ANALYZER_TAG, "Method = [GET]");
                break;
            case CA_POST:
                OIC_LOG(DEBUG, ANALYZER_TAG, "Method = [POST]");
                break;
            case CA_PUT:
                OIC_LOG(DEBUG, ANALYZER_TAG, "Method = [PUT]");
                break;
            case CA_DELETE:
                OIC_LOG(DEBUG, ANALYZER_TAG, "Method = [DELETE]");
                break;
            default:
                OIC_LOG_V(DEBUG, ANALYZER_TAG, "Method = [%d]", data->requestInfo->method);
                break;
        }
        info = &data->requestInfo->info;
    }

    if (NULL != data->responseInfo)
    {
        OIC_LOG_V(DEBUG, ANALYZER_TAG, "result code = [%d]", data->responseInfo->result);
        info = &data->responseInfo->info;
    }

    if (pdu->transport_hdr)
    {
        OIC_LOG_V(DEBUG, ANALYZER_TAG, "Msg ID = [%u]",
            (uint32_t)ntohs(pdu->transport_hdr->udp.id));
    }

    if (info)
    {
        OIC_LOG(DEBUG, ANALYZER_TAG, "Coap Token");
        OIC_LOG_BUFFER(DEBUG, ANALYZER_TAG, (const uint8_t *) info->token, info->tokenLength);
        OIC_TRACE_BUFFER("OIC_CA_MSG_HANDLE:CALogPDUInfo:token:", (const uint8_t *) info->token,
                         info->tokenLength);
        OIC_LOG_V(DEBUG, ANALYZER_TAG, "Res URI = [%s]", info->resourceUri);
        OIC_TRACE_MARK(%s:CALogPDUInfo:uri:%s, TAG, info->resourceUri);

        if (CA_FORMAT_APPLICATION_CBOR == info->payloadFormat)
        {
            OIC_LOG(DEBUG, ANALYZER_TAG, "Payload Format = [CA_FORMAT_APPLICATION_CBOR]");
        }
        else if (CA_FORMAT_APPLICATION_VND_OCF_CBOR == info->payloadFormat)
        {
            OIC_LOG(DEBUG, ANALYZER_TAG, "Payload Format = [CA_FORMAT_APPLICATION_VND_OCF_CBOR]");
        }
        else
        {
            OIC_LOG_V(DEBUG, ANALYZER_TAG, "Payload Format = [%d]", info->payloadFormat);
        }
    }

#ifdef TB_LOG
    size_t payloadLen = (pdu->data) ? (unsigned char *)pdu->hdr + pdu->length - pdu->data : 0;
#endif
    OIC_LOG_V(DEBUG, ANALYZER_TAG, "CoAP Message Full Size = [%u]", pdu->length);
    OIC_LOG(DEBUG, ANALYZER_TAG, "CoAP Header (+ 0xFF)");
    OIC_LOG_BUFFER(DEBUG, ANALYZER_TAG,  (const uint8_t *) pdu->transport_hdr,
                   pdu->length - payloadLen);
    OIC_LOG_V(DEBUG, ANALYZER_TAG, "CoAP Header size = [%lu]", pdu->length - payloadLen);

    OIC_LOG_V(DEBUG, ANALYZER_TAG, "CoAP Payload Size = [%lu]", payloadLen);
    OIC_LOG_V(DEBUG, ANALYZER_TAG, "CoAP Payload:");
    OIC_LOG_PAYLOAD_BUFFER(DEBUG, ANALYZER_TAG, pdu->data, payloadLen);
    OIC_TRACE_END();
    OIC_LOG_V(DEBUG, TAG, "%s EXIT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<", __func__);
}

#ifdef TCP_ADAPTER
CAResult_t CAAddHeaderOption(CAHeaderOption_t **hdrOpt, uint8_t *numOptions,
                             uint16_t optionID, void *optionData, size_t optionDataLength)
{
    OIC_LOG_V(DEBUG, TAG, "Entering CAAddHeaderOption with optionID: %d", optionID);

    VERIFY_NON_NULL_MSG(hdrOpt, TAG, "hdrOpt");
    VERIFY_NON_NULL_MSG(numOptions, TAG, "numOptions");
    VERIFY_NON_NULL_MSG(optionData, TAG, "optionData");

    CAHeaderOption_t *tmpOpt = OICRealloc(*hdrOpt, (*numOptions + 1) * sizeof(CAHeaderOption_t));
    if (!tmpOpt)
    {
        OIC_LOG(ERROR, TAG, "out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    //tmpOpt[*numOptions].protocolID = CA_COAP_ID;
    tmpOpt[*numOptions].optionID = optionID;
    tmpOpt[*numOptions].optionLength = (uint16_t)optionDataLength;

    if (optionData)
    {
        memcpy(tmpOpt[*numOptions].optionData, optionData,
               sizeof(tmpOpt[*numOptions].optionData));
    }

    // increase the number of options.
    *numOptions += 1;
    *hdrOpt = tmpOpt;

    OIC_LOG(DEBUG, TAG, "Added option successfully");
    return CA_STATUS_OK;
}

CAResult_t CAGetHeaderOption(CAHeaderOption_t *hdrOpt, size_t numOptions, uint16_t optionID,
                             void *optionData, size_t optionDataLength, uint16_t *receivedDataLen)
{
    OIC_LOG_V(DEBUG, TAG, "Entering CAGetHeaderOption with optionID: %d", optionID);

    VERIFY_NON_NULL_MSG(hdrOpt, TAG, "hdrOpt");
    VERIFY_NON_NULL_MSG(optionData, TAG, "optionData");
    VERIFY_NON_NULL_MSG(receivedDataLen, TAG, "receivedDataLen");

    for (size_t i = 0; i < numOptions; i++)
    {
        if (hdrOpt[i].optionID == optionID)
        {
            if (optionDataLength >= hdrOpt[i].optionLength)
            {
                memcpy(optionData, hdrOpt[i].optionData, hdrOpt[i].optionLength);
                *receivedDataLen = hdrOpt[i].optionLength;
                return CA_STATUS_OK;
            }
        }
    }

    return CA_STATUS_NOT_FOUND;
}

#endif //TCP_ADAPTER

CAResult_t CAAddBlockSizeOption(coap_pdu_t *pdu, uint16_t sizeType, size_t dataLength,
                                coap_list_t **options)
{
    OIC_LOG(DEBUG, TAG, "IN-CAAddBlockSizeOption");
    VERIFY_NON_NULL_MSG(pdu, TAG, "pdu");
    VERIFY_NON_NULL_MSG(options, TAG, "options");
    VERIFY_TRUE((dataLength <= UINT_MAX), TAG, "dataLength");

    if (sizeType != COAP_OPTION_SIZE1 && sizeType != COAP_OPTION_SIZE2)
    {
        OIC_LOG(ERROR, TAG, "unknown option type");
        return CA_STATUS_FAILED;
    }

    unsigned char value[BLOCKWISE_OPTION_BUFFER] = { 0 };
    unsigned int optionLength = coap_encode_var_bytes(value,
                                                      (unsigned int)dataLength);

    int ret = coap_insert(options,
                          CACreateNewOptionNode(sizeType, optionLength, (char *) value),
                          CAOrderOpts);
    if (ret <= 0)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    OIC_LOG(DEBUG, TAG, "OUT-CAAddBlockSizeOption");

    return CA_STATUS_OK;
}

static CAResult_t CAAddBlockOptionImpl(coap_block_t *block, uint8_t blockType,
                                coap_list_t **options)
{
    OIC_LOG(DEBUG, TAG, "IN-AddBlockOptionImpl");
    VERIFY_NON_NULL_MSG(block, TAG, "block");
    VERIFY_NON_NULL_MSG(options, TAG, "options");

    unsigned char buf[BLOCKWISE_OPTION_BUFFER] = { 0 };
    unsigned int optionLength = coap_encode_var_bytes(buf,
                                                      ((block->num << BLOCK_NUMBER_IDX)
                                                       | (block->m << BLOCK_M_BIT_IDX)
                                                       | block->szx));

    int ret = coap_insert(options,
                          CACreateNewOptionNode(blockType, optionLength, (char *) buf),
                          CAOrderOpts);
    if (ret <= 0)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    OIC_LOG(DEBUG, TAG, "OUT-AddBlockOptionImpl");
    return CA_STATUS_OK;
}

static CAResult_t CAAddBlockOption1(coap_pdu_t **pdu, const CAInfo_t *info, size_t dataLength,
                             const CABlockDataID_t *blockID, coap_list_t **options)
{
    OIC_LOG(DEBUG, TAG, "IN-AddBlockOption1");
    VERIFY_NON_NULL_MSG(pdu, TAG, "pdu");
    VERIFY_NON_NULL_MSG((*pdu), TAG, "(*pdu)");
    VERIFY_NON_NULL_MSG((*pdu)->transport_hdr, TAG, "(*pdu)->transport_hdr");
    VERIFY_NON_NULL_MSG(info, TAG, "info");
    VERIFY_NON_NULL_MSG(blockID, TAG, "blockID");
    VERIFY_NON_NULL_MSG(options, TAG, "options");
    VERIFY_TRUE((dataLength <= UINT_MAX), TAG, "dataLength");

    // get set block data from CABlock list-set.
    coap_block_t *block1 = CAGetBlockOption(blockID, COAP_OPTION_BLOCK1);
    if (!block1)
    {
        OIC_LOG(ERROR, TAG, "getting has failed");
        return CA_STATUS_FAILED;
    }
    bool blockRemoved = false;

    CAResult_t res = CA_STATUS_OK;
    uint32_t code = (*pdu)->transport_hdr->udp.code;
    if (CA_GET == code || CA_POST == code || CA_PUT == code || CA_DELETE == code)
    {
        CASetMoreBitFromBlock(dataLength, block1);

        // if block number is 0, add size1 option
        if (0 == block1->num)
        {
            res = CAAddBlockSizeOption(*pdu, COAP_OPTION_SIZE1, dataLength, options);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                goto exit;
            }
        }

        // add block option to option list.
        res = CAAddBlockOptionImpl(block1, COAP_OPTION_BLOCK1, options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            goto exit;
        }

        // add option list to pdu.
        res = CAAddOptionToPDU(*pdu, options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            goto exit;
        }

        // add the payload data as the block size.
        assert(block1->szx <= UINT8_MAX);
        if (!coap_add_block(*pdu, (unsigned int)dataLength,
                            (const unsigned char *) info->payload, block1->num,
                            (unsigned char)block1->szx))
        {
            OIC_LOG(ERROR, TAG, "Data length is smaller than the start index");
            return CA_STATUS_FAILED;
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "received response message with block option1");

        // add block option to option list.
        res = CAAddBlockOptionImpl(block1, COAP_OPTION_BLOCK1, options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            goto exit;
        }

        // add option list to pdu.
        res = CAAddOptionToPDU(*pdu, options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            goto exit;
        }

        // add the payload data as the block size.
        if (!coap_add_data(*pdu, (unsigned int)dataLength,
                           (const unsigned char*)info->payload))
        {
            OIC_LOG(ERROR, TAG, "failed to add payload");
            return CA_STATUS_FAILED;
        }

        // if it is last block message, remove block data from list.
        if (0 == block1->m)
        {
            // remove data from list
            res = CARemoveBlockDataFromList(blockID);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "remove has failed");
                return res;
            }
            blockRemoved = true;
        }
    }

    if (!blockRemoved)
    {
        CALogBlockInfo(block1);
    }

    OIC_LOG(DEBUG, TAG, "OUT-AddBlockOption1");

    return CA_STATUS_OK;

exit:
    if (!blockRemoved)
    {
        CARemoveBlockDataFromList(blockID);
    }

    return res;
}

static CAResult_t CAAddBlockOption2(coap_pdu_t **pdu, const CAInfo_t *info, size_t dataLength,
                             const CABlockDataID_t *blockID, coap_list_t **options)
{
    OIC_LOG(DEBUG, TAG, "IN-AddBlockOption2");
    VERIFY_NON_NULL_MSG(pdu, TAG, "pdu");
    VERIFY_NON_NULL_MSG((*pdu), TAG, "(*pdu)");
    VERIFY_NON_NULL_MSG((*pdu)->transport_hdr, TAG, "(*pdu)->transport_hdr");
    VERIFY_NON_NULL_MSG(info, TAG, "info");
    VERIFY_NON_NULL_MSG(blockID, TAG, "blockID");
    VERIFY_NON_NULL_MSG(options, TAG, "options");
    VERIFY_TRUE((dataLength <= UINT_MAX), TAG, "dataLength");

    // get set block data from CABlock list-set.
    coap_block_t *block1 = CAGetBlockOption(blockID, COAP_OPTION_BLOCK1);
    coap_block_t *block2 = CAGetBlockOption(blockID, COAP_OPTION_BLOCK2);
    if (!block1 || !block2)
    {
        OIC_LOG(ERROR, TAG, "getting has failed");
        return CA_STATUS_FAILED;
    }

    CAResult_t res = CA_STATUS_OK;
    uint32_t code = (*pdu)->transport_hdr->udp.code;
    if (CA_GET != code && CA_POST != code && CA_PUT != code && CA_DELETE != code)
    {
        CASetMoreBitFromBlock(dataLength, block2);

        // if block number is 0, add size2 option
        if (0 == block2->num)
        {
            res = CAAddBlockSizeOption(*pdu, COAP_OPTION_SIZE2, dataLength, options);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                goto exit;
            }
        }

        res = CAAddBlockOptionImpl(block2, COAP_OPTION_BLOCK2, options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            goto exit;
        }

        if (block1->num)
        {
            OIC_LOG(DEBUG, TAG, "combining block1 and block2");
            res = CAAddBlockOptionImpl(block1, COAP_OPTION_BLOCK1, options);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                goto exit;
            }
            // initialize block number
            block1->num = 0;
        }

        res = CAAddOptionToPDU(*pdu, options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            goto exit;
        }

        assert(block2->szx <= UINT8_MAX);
        if (!coap_add_block(*pdu, (unsigned int)dataLength,
                            (const unsigned char *) info->payload,
                            block2->num, (unsigned char)block2->szx))
        {
            OIC_LOG(ERROR, TAG, "Data length is smaller than the start index");
            return CA_STATUS_FAILED;
        }

        CALogBlockInfo(block2);

        if (!block2->m)
        {
            // if sent message is last response block message, remove data
            CARemoveBlockDataFromList(blockID);
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "option2, not response msg");
        res = CAAddBlockOptionImpl(block2, COAP_OPTION_BLOCK2, options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            goto exit;
        }

        res = CAAddOptionToPDU(*pdu, options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            goto exit;
        }
        CALogBlockInfo(block2);
    }

    return CA_STATUS_OK;

exit:
    CARemoveBlockDataFromList(blockID);
    return res;
}

LOCAL CAResult_t CAAddBlockOption(coap_pdu_t **pdu, const CAInfo_t *info,
                            const CAEndpoint_t *endpoint, coap_list_t **options)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL_MSG(pdu, TAG, "pdu");
    VERIFY_NON_NULL_MSG((*pdu), TAG, "(*pdu)");
    VERIFY_NON_NULL_MSG((*pdu)->transport_hdr, TAG, "(*pdu)->transport_hdr");
    VERIFY_NON_NULL_MSG(info, TAG, "info");
    VERIFY_NON_NULL_MSG(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL_MSG(options, TAG, "options");
    VERIFY_TRUE(((*pdu)->transport_hdr->udp.token_length <= UINT8_MAX), TAG,
                "pdu->transport_hdr->udp.token_length");
    VERIFY_TRUE((info->payloadSize <= UINT_MAX), TAG, "info->payloadSize");

    CAResult_t res = CA_STATUS_OK;
    unsigned int dataLength = 0;
    if (info->payload)
    {
        dataLength = (unsigned int)info->payloadSize;
        OIC_LOG_V(DEBUG, TAG, "dataLength - %u", dataLength);
    }

    CABlockDataID_t* blockDataID = CACreateBlockDatablockId(
            (uint8_t*)(*pdu)->transport_hdr->udp.token,
            (uint8_t)(*pdu)->transport_hdr->udp.token_length,
            endpoint->addr, endpoint->port);
    if (NULL == blockDataID || blockDataID->idLength < 1)
    {
        OIC_LOG(ERROR, TAG, "blockId is null");
        res = CA_STATUS_FAILED;
        goto exit;
    }

    uint32_t repCode = CA_RESPONSE_CODE((*pdu)->transport_hdr->udp.code);
    if (CA_REQUEST_ENTITY_INCOMPLETE == repCode)
    {
        OIC_LOG(INFO, TAG, "don't use option");
        res = CA_STATUS_OK;
        goto exit;
    }

    uint16_t blockType = CAGetBlockOptionType(blockDataID);
    if (COAP_OPTION_BLOCK2 == blockType) /* response payload */
    {
        res = CAAddBlockOption2(pdu, info, dataLength, blockDataID, options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            goto exit;
        }
    }
    else if (COAP_OPTION_BLOCK1 == blockType) /* request payload */
    {
        res = CAAddBlockOption1(pdu, info, dataLength, blockDataID, options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            goto exit;
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "no BLOCK option");

        // in case it is not large data, add option list to pdu.
        if (*options)
        {
            for (coap_list_t *opt = *options; opt; opt = opt->next)
            {
                OIC_LOG_V(DEBUG, TAG, "[%s] opt will be added.",
                          COAP_OPTION_DATA(*(coap_option *) opt->data));
                OIC_LOG_V(DEBUG, TAG, "[%d] pdu length", (*pdu)->length);

                if (0 == coap_add_option(*pdu, COAP_OPTION_KEY(*(coap_option *) opt->data),
                                         COAP_OPTION_LENGTH(*(coap_option *) opt->data),
                                         COAP_OPTION_DATA(*(coap_option *) opt->data)))
                {
                    OIC_LOG(ERROR, TAG, "coap_add_option has failed");
                    res = CA_STATUS_FAILED;
                    goto exit;
                }
            }
        }
        OIC_LOG_V(DEBUG, TAG, "[%d] pdu length after option", (*pdu)->length);

        // if response data is so large. it have to send as block transfer
        if (!coap_add_data(*pdu, dataLength, (const unsigned char*)info->payload))
        {
            OIC_LOG(INFO, TAG, "it has to use block");
            res = CA_STATUS_FAILED;
            goto exit;
        }
        else
        {
            OIC_LOG(INFO, TAG, "not Blockwise Transfer");
        }
    }

    uint32_t code = (*pdu)->transport_hdr->udp.code;
    if (CA_GET == code || CA_POST == code || CA_PUT == code || CA_DELETE == code)
    {
        // if received message type is RESET from remote device,
        // we have to use the updated message id of request message to find token.
        CABlockData_t *blockData = CAGetBlockDataFromBlockDataList(blockDataID);
        if (blockData)
        {
            res = CAUpdateMessageId(*pdu, blockDataID);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "fail to update message id");
                goto exit;
            }
        }
    }

exit:
    CADestroyBlockID(blockDataID);
    OIC_LOG(DEBUG, TAG, "OUT-AddBlockOption");
    return res;
}
