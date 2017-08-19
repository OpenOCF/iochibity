/******************************************************************
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

// Defining _BSD_SOURCE or _DEFAULT_SOURCE causes header files to expose
// definitions that may otherwise be skipped. Skipping can cause implicit
// declaration warnings and/or bugs and subtle problems in code execution.
// For glibc information on feature test macros,
// Refer http://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
//
// For details on compatibility and glibc support,
// Refer http://www.gnu.org/software/libc/manual/html_node/BSD-Random.html
#define _DEFAULT_SOURCE

#include "iotivity_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "caprotocolmessage.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocrandom.h"
#include "cacommonutil.h"
#include "cablockwisetransfer.h"

#define TAG "OIC_CA_PRTCL_MSG"

#define CA_PDU_MIN_SIZE (4)
#define CA_ENCODE_BUFFER_SIZE (4)

static const char COAP_URI_HEADER[] = "coap://[::]/";

static char g_chproxyUri[CA_MAX_URI_LENGTH];

CAResult_t CASetProxyUri(const char *uri)
{
    VERIFY_NON_NULL(uri, TAG, "uri");
    OICStrcpy(g_chproxyUri, sizeof (g_chproxyUri), uri);
    return CA_STATUS_OK;
}

CAResult_t CAGetRequestInfoFromPDU(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                                   CARequestInfo_t *outReqInfo)
{
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(outReqInfo, TAG, "outReqInfo");

    uint32_t code = CA_NOT_FOUND;
    CAResult_t ret = CAGetInfoFromPDU(pdu, endpoint, &code, &(outReqInfo->info));
    outReqInfo->method = code;

    return ret;
}

CAResult_t CAGetResponseInfoFromPDU(const coap_pdu_t *pdu, CAResponseInfo_t *outResInfo,
                                    const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(outResInfo, TAG, "outResInfo");

    uint32_t code = CA_NOT_FOUND;
    CAResult_t ret = CAGetInfoFromPDU(pdu, endpoint, &code, &(outResInfo->info));
    outResInfo->result = code;

    return ret;
}

CAResult_t CAGetErrorInfoFromPDU(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                                 CAErrorInfo_t *errorInfo)
{
    VERIFY_NON_NULL(pdu, TAG, "pdu");

    uint32_t code = 0;
    CAResult_t ret = CAGetInfoFromPDU(pdu, endpoint, &code, &errorInfo->info);

    return ret;
}

coap_pdu_t *CAGeneratePDU(uint32_t code, const CAInfo_t *info, const CAEndpoint_t *endpoint,
                          coap_list_t **optlist, coap_transport_t *transport)
{
    VERIFY_NON_NULL_RET(info, TAG, "info", NULL);
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint", NULL);
    VERIFY_NON_NULL_RET(optlist, TAG, "optlist", NULL);

    OIC_LOG_V(DEBUG, TAG, "generate pdu for [%d]adapter, [%d]flags",
              endpoint->adapter, endpoint->flags);

    coap_pdu_t *pdu = NULL;

    // RESET have to use only 4byte (empty message)
    // and ACKNOWLEDGE can use empty message when code is empty.
    if (CA_MSG_RESET == info->type || (CA_EMPTY == code && CA_MSG_ACKNOWLEDGE == info->type))
    {
        if (CA_EMPTY != code)
        {
            OIC_LOG(ERROR, TAG, "reset is not empty message");
            return NULL;
        }

        if (info->payloadSize > 0 || info->payload || info->token || info->tokenLength > 0)
        {
            OIC_LOG(ERROR, TAG, "Empty message has unnecessary data after messageID");
            return NULL;
        }

        OIC_LOG(DEBUG, TAG, "code is empty");
        if (!(pdu = CAGeneratePDUImpl((code_t) code, info, endpoint, NULL, transport)))
        {
            OIC_LOG(ERROR, TAG, "pdu NULL");
            return NULL;
        }
    }
    else
    {
        if (info->resourceUri)
        {
            OIC_LOG_V(DEBUG, TAG, "uri : %s", info->resourceUri);

            size_t length = strlen(info->resourceUri);
            if (CA_MAX_URI_LENGTH < length)
            {
                OIC_LOG(ERROR, TAG, "URI len err");
                return NULL;
            }

            size_t uriLength = length + sizeof(COAP_URI_HEADER);
            char *coapUri = (char *) OICCalloc(1, uriLength);
            if (NULL == coapUri)
            {
                OIC_LOG(ERROR, TAG, "out of memory");
                return NULL;
            }
            OICStrcat(coapUri, uriLength, COAP_URI_HEADER);
            OICStrcat(coapUri, uriLength, info->resourceUri);

            // parsing options in URI
            CAResult_t res = CAParseURI(coapUri, optlist);
            if (CA_STATUS_OK != res)
            {
                OICFree(coapUri);
                return NULL;
            }

            OICFree(coapUri);
        }
        // parsing options in HeadOption
        CAResult_t ret = CAParseHeadOption(code, info, optlist);
        if (CA_STATUS_OK != ret)
        {
            return NULL;
        }

        pdu = CAGeneratePDUImpl((code_t) code, info, endpoint, *optlist, transport);
        if (NULL == pdu)
        {
            OIC_LOG(ERROR, TAG, "pdu NULL");
            return NULL;
        }
    }

    // pdu print method : coap_show_pdu(pdu);
    return pdu;
}

coap_pdu_t *CAParsePDU(const char *data, size_t length, uint32_t *outCode,
                       const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL_RET(data, TAG, "data", NULL);
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint", NULL);

    coap_transport_t transport = COAP_UDP;
#ifdef WITH_TCP
    if (CAIsSupportedCoAPOverTCP(endpoint->adapter))
    {
        transport = coap_get_tcp_header_type_from_initbyte(((unsigned char *)data)[0] >> 4);
    }
#endif

    coap_pdu_t *outpdu =
        coap_pdu_init2(0, 0, ntohs((unsigned short)COAP_INVALID_TID), length, transport);
    if (NULL == outpdu)
    {
        OIC_LOG(ERROR, TAG, "outpdu is null");
        return NULL;
    }

    OIC_LOG_V(DEBUG, TAG, "pdu parse-transport type : %d", transport);

    int ret = coap_pdu_parse2((unsigned char *) data, length, outpdu, transport);
    OIC_LOG_V(DEBUG, TAG, "pdu parse ret: %d", ret);
    if (0 >= ret)
    {
        OIC_LOG(ERROR, TAG, "pdu parse failed");
        goto exit;
    }

#ifdef WITH_TCP
    if (CAIsSupportedCoAPOverTCP(endpoint->adapter))
    {
        OIC_LOG(INFO, TAG, "there is no version info in coap header");
    }
    else
#endif
    {
        if (outpdu->transport_hdr->udp.version != COAP_DEFAULT_VERSION)
        {
            OIC_LOG_V(ERROR, TAG, "coap version is not available : %d",
                      outpdu->transport_hdr->udp.version);
            goto exit;
        }
        if (outpdu->transport_hdr->udp.token_length > CA_MAX_TOKEN_LEN)
        {
            OIC_LOG_V(ERROR, TAG, "token length has been exceed : %d",
                      outpdu->transport_hdr->udp.token_length);
            goto exit;
        }
    }

    if (outCode)
    {
        (*outCode) = (uint32_t) CA_RESPONSE_CODE(coap_get_code(outpdu, transport));
    }

    return outpdu;

exit:
    OIC_LOG(DEBUG, TAG, "data :");
    OIC_LOG_BUFFER(DEBUG, TAG,  (const uint8_t *)data, length);
    coap_delete_pdu(outpdu);
    return NULL;
}

coap_pdu_t *CAGeneratePDUImpl(code_t code, const CAInfo_t *info,
                              const CAEndpoint_t *endpoint, coap_list_t *options,
                              coap_transport_t *transport)
{
    VERIFY_NON_NULL_RET(info, TAG, "info", NULL);
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint", NULL);
    VERIFY_NON_NULL_RET(transport, TAG, "transport", NULL);
    VERIFY_TRUE_RET((info->payloadSize <= UINT_MAX), TAG,
                    "info->payloadSize", NULL);

    size_t length = COAP_MAX_PDU_SIZE;
#ifdef WITH_TCP
    size_t msgLength = 0;
    if (CAIsSupportedCoAPOverTCP(endpoint->adapter))
    {
        if (options)
        {
            unsigned short prevOptNumber = 0;
            for (coap_list_t *opt = options; opt; opt = opt->next)
            {
                unsigned short curOptNumber = COAP_OPTION_KEY(*(coap_option *) opt->data);
                if (prevOptNumber > curOptNumber)
                {
                    OIC_LOG(ERROR, TAG, "option list is wrong");
                    return NULL;
                }

                size_t optValueLen = COAP_OPTION_LENGTH(*(coap_option *) opt->data);
                size_t optLength = coap_get_opt_header_length(curOptNumber - prevOptNumber, optValueLen);
                if (0 == optLength)
                {
                    OIC_LOG(ERROR, TAG, "Reserved for the Payload marker for the option");
                    return NULL;
                }
                msgLength += optLength;
                prevOptNumber = curOptNumber;
                OIC_LOG_V(DEBUG, TAG, "curOptNumber[%d], prevOptNumber[%d], optValueLen[%" PRIuPTR "], "
                        "optLength[%" PRIuPTR "], msgLength[%" PRIuPTR "]",
                          curOptNumber, prevOptNumber, optValueLen, optLength, msgLength);
            }
        }

        if (info->payloadSize > 0)
        {
            msgLength = msgLength + info->payloadSize + PAYLOAD_MARKER;
        }

        *transport = coap_get_tcp_header_type_from_size((unsigned int)msgLength);
        length = msgLength + coap_get_tcp_header_length_for_transport(*transport)
                + info->tokenLength;
    }
    else
#endif
    {
        *transport = COAP_UDP;
    }

    coap_pdu_t *pdu = coap_pdu_init2(0, 0,
                                     ntohs((unsigned short)COAP_INVALID_TID),
                                     length, *transport);

    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "malloc failed");
        return NULL;
    }

    OIC_LOG_V(DEBUG, TAG, "transport type: %d, payload size: %" PRIuPTR,
              *transport, info->payloadSize);

#ifdef WITH_TCP
    if (CAIsSupportedCoAPOverTCP(endpoint->adapter))
    {
        coap_add_length(pdu, *transport, (unsigned int)msgLength);
    }
    else
#endif
    {
        OIC_LOG_V(DEBUG, TAG, "msgID is %d", info->messageId);
        uint16_t message_id = 0;
        if (0 == info->messageId)
        {
            /* initialize message id */
            prng((uint8_t * ) &message_id, sizeof(message_id));

            OIC_LOG_V(DEBUG, TAG, "gen msg id=%d", message_id);
        }
        else
        {
            /* use saved message id */
            message_id = info->messageId;
        }
        pdu->transport_hdr->udp.id = message_id;
        OIC_LOG_V(DEBUG, TAG, "messageId in pdu is %d, %d", message_id, pdu->transport_hdr->udp.id);

        pdu->transport_hdr->udp.type = info->type;
    }

    coap_add_code(pdu, *transport, code);

    if (info->token && CA_EMPTY != code)
    {
        uint32_t tokenLength = info->tokenLength;
        OIC_LOG_V(DEBUG, TAG, "token info token length: %d, token :", tokenLength);
        OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *)info->token, tokenLength);

        int32_t ret = coap_add_token2(pdu, tokenLength, (unsigned char *)info->token, *transport);
        if (0 == ret)
        {
            OIC_LOG(ERROR, TAG, "can't add token");
        }
    }

#ifdef WITH_BWT
    if (CA_ADAPTER_GATT_BTLE != endpoint->adapter
#ifdef WITH_TCP
            && !CAIsSupportedCoAPOverTCP(endpoint->adapter)
#endif
            )
    {
        // option list will be added in blockwise-transfer
        return pdu;
    }
#endif

    if (options)
    {
        for (coap_list_t *opt = options; opt; opt = opt->next)
        {
            OIC_LOG_V(DEBUG, TAG, "[%s] opt will be added.",
                      COAP_OPTION_DATA(*(coap_option *) opt->data));

            OIC_LOG_V(DEBUG, TAG, "[%d] pdu length", pdu->length);
            if (0 == coap_add_option2(pdu, COAP_OPTION_KEY(*(coap_option *) opt->data),
                                      COAP_OPTION_LENGTH(*(coap_option *) opt->data),
                                      COAP_OPTION_DATA(*(coap_option *) opt->data), *transport))
            {
                OIC_LOG(ERROR, TAG, "coap_add_option2 has failed");
                coap_delete_pdu(pdu);
                return NULL;
            }
        }
    }

    OIC_LOG_V(DEBUG, TAG, "[%d] pdu length after option", pdu->length);

    if ((NULL != info->payload) && (0 < info->payloadSize))
    {
        OIC_LOG(DEBUG, TAG, "payload is added");
        coap_add_data(pdu, (unsigned int)info->payloadSize,
                      (const unsigned char*)info->payload);
    }

    return pdu;
}

CAResult_t CAParseURI(const char *uriInfo, coap_list_t **optlist)
{
    VERIFY_NON_NULL(uriInfo, TAG, "uriInfo");
    VERIFY_NON_NULL(optlist, TAG, "optlist");

    /* split arg into Uri-* options */
    coap_uri_t uri;
    coap_split_uri((unsigned char *) uriInfo, strlen(uriInfo), &uri);

    if (uri.port != COAP_DEFAULT_PORT)
    {
        unsigned char portbuf[CA_ENCODE_BUFFER_SIZE] = { 0 };
        int ret = coap_insert(optlist,
                              CACreateNewOptionNode(COAP_OPTION_URI_PORT,
                                                    coap_encode_var_bytes(portbuf, uri.port),
                                                    (char *)portbuf),
                              CAOrderOpts);
        if (ret <= 0)
        {
            return CA_STATUS_INVALID_PARAM;
        }
    }

    if (uri.path.s && uri.path.length)
    {
        CAResult_t ret = CAParseUriPartial(uri.path.s, uri.path.length,
                                           COAP_OPTION_URI_PATH, optlist);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG(ERROR, TAG, "CAParseUriPartial failed(uri path)");
            return ret;
        }
    }

    if (uri.query.s && uri.query.length)
    {
        CAResult_t ret = CAParseUriPartial(uri.query.s, uri.query.length, COAP_OPTION_URI_QUERY,
                                           optlist);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG(ERROR, TAG, "CAParseUriPartial failed(uri query)");
            return ret;
        }
    }

    return CA_STATUS_OK;
}

CAResult_t CAParseUriPartial(const unsigned char *str, size_t length, uint16_t target,
                             coap_list_t **optlist)
{
    VERIFY_NON_NULL(optlist, TAG, "optlist");

    if ((target != COAP_OPTION_URI_PATH) && (target != COAP_OPTION_URI_QUERY))
    {
        // should never occur. Log just in case.
        OIC_LOG(DEBUG, TAG, "Unexpected URI component.");
        return CA_NOT_SUPPORTED;
    }
    else if (str && length)
    {
        unsigned char uriBuffer[CA_MAX_URI_LENGTH] = { 0 };
        unsigned char *pBuf = uriBuffer;
        size_t unusedBufferSize = sizeof(uriBuffer);
        int res = (target == COAP_OPTION_URI_PATH) ? coap_split_path(str, length, pBuf, &unusedBufferSize) :
                                                     coap_split_query(str, length, pBuf, &unusedBufferSize);

        if (res > 0)
        {
            assert(unusedBufferSize < sizeof(uriBuffer));
            size_t usedBufferSize = sizeof(uriBuffer) - unusedBufferSize;
            size_t prevIdx = 0;
            while (res--)
            {
                int ret = coap_insert(optlist,
                                      CACreateNewOptionNode(target, COAP_OPT_LENGTH(pBuf),
                                                            (char *)COAP_OPT_VALUE(pBuf)),
                                      CAOrderOpts);
                if (ret <= 0)
                {
                    return CA_STATUS_INVALID_PARAM;
                }

                size_t optSize = COAP_OPT_SIZE(pBuf);
                if (prevIdx + optSize > usedBufferSize)
                {
                    assert(false);
                    return CA_STATUS_INVALID_PARAM;
                }
                pBuf += optSize;
                prevIdx += optSize;
            }
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "Problem parsing URI : %d for %d", res, target);
            return CA_STATUS_FAILED;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "str or length is not available");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CAParseHeadOption(uint32_t code, const CAInfo_t *info, coap_list_t **optlist)
{
    (void)code;
    VERIFY_NON_NULL_RET(info, TAG, "info", CA_STATUS_INVALID_PARAM);

    OIC_LOG_V(DEBUG, TAG, "parse Head Opt: %d", info->numOptions);

    if (!optlist)
    {
        OIC_LOG(ERROR, TAG, "optlist is null");
        return CA_STATUS_INVALID_PARAM;
    }

    for (uint32_t i = 0; i < info->numOptions; i++)
    {
        if(!(info->options + i))
        {
            OIC_LOG(ERROR, TAG, "options is not available");
            return CA_STATUS_FAILED;
        }

        uint16_t id = (info->options + i)->optionID;
        switch (id)
        {
            case COAP_OPTION_URI_PATH:
            case COAP_OPTION_URI_QUERY:
                OIC_LOG_V(DEBUG, TAG, "not Header Opt: %d", id);
                break;
            case COAP_OPTION_ACCEPT:
            case CA_OPTION_ACCEPT_VERSION:
            case COAP_OPTION_CONTENT_FORMAT:
            case CA_OPTION_CONTENT_VERSION:
                // this is handled below via CAParsePayloadFormatHeadOption
                break;
            default:
                OIC_LOG_V(DEBUG, TAG, "Head opt ID[%d], length[%d]", id,
                    (info->options + i)->optionLength);
                int ret = coap_insert(optlist,
                                      CACreateNewOptionNode(id, (info->options + i)->optionLength,
                                                            (info->options + i)->optionData),
                                      CAOrderOpts);
                if (ret <= 0)
                {
                    return CA_STATUS_INVALID_PARAM;
                }
        }
    }

    // insert one extra header with the payload format if applicable.
    if (CA_FORMAT_UNDEFINED != info->payloadFormat)
    {
        CAParsePayloadFormatHeadOption(COAP_OPTION_CONTENT_FORMAT, info->payloadFormat, CA_OPTION_CONTENT_VERSION, info->payloadVersion, optlist);
    }

    if (CA_FORMAT_UNDEFINED != info->acceptFormat)
    {
        CAParsePayloadFormatHeadOption(COAP_OPTION_ACCEPT, info->acceptFormat, CA_OPTION_ACCEPT_VERSION, info->acceptVersion, optlist);
    }

    return CA_STATUS_OK;
}

CAResult_t CAParsePayloadFormatHeadOption(uint16_t formatOption, CAPayloadFormat_t format,
        uint16_t versionOption, uint16_t version, coap_list_t **optlist)
{
    coap_list_t* encodeNode = NULL;
    coap_list_t* versionNode = NULL;
    uint8_t encodeBuf[CA_ENCODE_BUFFER_SIZE] = { 0 };
    uint8_t versionBuf[CA_ENCODE_BUFFER_SIZE] = { 0 };

    switch (format)
    {
        case CA_FORMAT_APPLICATION_CBOR:
            encodeNode = CACreateNewOptionNode(formatOption,
                    coap_encode_var_bytes(encodeBuf,
                            (unsigned short) COAP_MEDIATYPE_APPLICATION_CBOR), (char *) encodeBuf);
            break;
        case CA_FORMAT_APPLICATION_VND_OCF_CBOR:
            encodeNode = CACreateNewOptionNode(formatOption,
                    coap_encode_var_bytes(encodeBuf,
                            (unsigned short) COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR),
                    (char *) encodeBuf);
            break;
        default:
            OIC_LOG_V(ERROR, TAG, "Format option:[%d] not supported", format);
    }
    if (!encodeNode)
    {
        OIC_LOG(ERROR, TAG, "Format option not created");
        return CA_STATUS_INVALID_PARAM;
    }
    int ret = coap_insert(optlist, encodeNode, CAOrderOpts);
    if (0 >= ret)
    {
        coap_delete(encodeNode);
        OIC_LOG(ERROR, TAG, "Format option not inserted in header");
        return CA_STATUS_INVALID_PARAM;
    }

    if ((CA_OPTION_ACCEPT_VERSION == versionOption ||
         CA_OPTION_CONTENT_VERSION == versionOption) &&
        CA_FORMAT_APPLICATION_VND_OCF_CBOR == format)
    {
        versionNode = CACreateNewOptionNode(versionOption,
                coap_encode_var_bytes(versionBuf, version), (char *) versionBuf);

        if (!versionNode)
        {
            OIC_LOG(ERROR, TAG, "Version option not created");
            coap_delete(encodeNode);
            return CA_STATUS_INVALID_PARAM;
        }
        ret = coap_insert(optlist, versionNode, CAOrderOpts);
        if (0 >= ret)
        {
            coap_delete(versionNode);
            coap_delete(encodeNode);
            OIC_LOG(ERROR, TAG, "Content version option not inserted in header");
            return CA_STATUS_INVALID_PARAM;
        }
    }
    return CA_STATUS_OK;
}

coap_list_t *CACreateNewOptionNode(uint16_t key, uint32_t length, const char *data)
{
    VERIFY_NON_NULL_RET(data, TAG, "data", NULL);

    coap_option *option = coap_malloc(sizeof(coap_option) + length + 1);
    if (!option)
    {
        OIC_LOG(ERROR, TAG, "Out of memory");
        return NULL;
    }
    memset(option, 0, sizeof(coap_option) + length + 1);

    COAP_OPTION_KEY(*option) = key;

    coap_option_def_t* def = coap_opt_def(key);
    if (NULL != def && coap_is_var_bytes(def))
    {
       if (length > def->max)
        {
            // make sure we shrink the value so it fits the coap option definition
            // by truncating the value, disregard the leading bytes.
            OIC_LOG_V(DEBUG, TAG, "Option [%d] data size [%d] shrunk to [%d]",
                    def->key, length, def->max);
            data = &(data[length-def->max]);
            length = def->max;
        }
        // Shrink the encoding length to a minimum size for coap
        // options that support variable length encoding.
         COAP_OPTION_LENGTH(*option) = coap_encode_var_bytes(
                COAP_OPTION_DATA(*option),
                coap_decode_var_bytes((unsigned char *)data, length));
    }
    else
    {
        COAP_OPTION_LENGTH(*option) = length;
        memcpy(COAP_OPTION_DATA(*option), data, length);
    }

    /* we can pass NULL here as delete function since option is released automatically  */
    coap_list_t *node = coap_new_listnode(option, NULL);

    if (!node)
    {
        OIC_LOG(ERROR, TAG, "node is NULL");
        coap_free(option);
        return NULL;
    }

    return node;
}

int CAOrderOpts(void *a, void *b)
{
    if (!a || !b)
    {
        return a < b ? -1 : 1;
    }

    if (COAP_OPTION_KEY(*(coap_option *) a) < COAP_OPTION_KEY(*(coap_option * ) b))
    {
        return -1;
    }

    return COAP_OPTION_KEY(*(coap_option *) a) == COAP_OPTION_KEY(*(coap_option * ) b);
}

CAResult_t CAGetOptionCount(coap_opt_iterator_t opt_iter, uint8_t *optionCount)
{
    CAResult_t result = CA_STATUS_OK;
    coap_opt_t *option = NULL;
    *optionCount = 0;

    while ((option = coap_option_next(&opt_iter)))
    {
        if (COAP_OPTION_URI_PATH != opt_iter.type && COAP_OPTION_URI_QUERY != opt_iter.type
            && COAP_OPTION_BLOCK1 != opt_iter.type && COAP_OPTION_BLOCK2 != opt_iter.type
            && COAP_OPTION_SIZE1 != opt_iter.type && COAP_OPTION_SIZE2 != opt_iter.type
            && COAP_OPTION_CONTENT_FORMAT != opt_iter.type
            && CA_OPTION_CONTENT_VERSION != opt_iter.type
            && COAP_OPTION_URI_HOST != opt_iter.type && COAP_OPTION_URI_PORT != opt_iter.type
            && COAP_OPTION_ETAG != opt_iter.type && COAP_OPTION_MAXAGE != opt_iter.type
            && COAP_OPTION_PROXY_SCHEME != opt_iter.type)
        {
            if (*optionCount < UINT8_MAX)
            {
                (*optionCount)++;
            }
            else
            {
                // Overflow. Return an error to the caller.
                assert(false);
                OIC_LOG_V(ERROR, TAG, "Overflow detected in %s", __func__);
                *optionCount = 0;
                result = CA_STATUS_FAILED;
                break;
            }
        }
    }

    return result;
}

CAResult_t CAGetInfoFromPDU(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                            uint32_t *outCode, CAInfo_t *outInfo)
{
    OIC_LOG(INFO, TAG, "IN - CAGetInfoFromPDU");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL(outCode, TAG, "outCode");
    VERIFY_NON_NULL(outInfo, TAG, "outInfo");

    coap_transport_t transport;
#ifdef WITH_TCP
    if (CAIsSupportedCoAPOverTCP(endpoint->adapter))
    {
        transport = coap_get_tcp_header_type_from_initbyte(((unsigned char *)pdu->transport_hdr)[0] >> 4);
    }
    else
#endif
    {
        transport = COAP_UDP;
    }

    coap_opt_iterator_t opt_iter;
    coap_option_iterator_init2((coap_pdu_t *) pdu, &opt_iter, COAP_OPT_ALL, transport);

    if (outCode)
    {
        (*outCode) = (uint32_t) CA_RESPONSE_CODE(coap_get_code(pdu, transport));
    }

    // init HeaderOption list
    uint8_t count = 0;
    CAResult_t countResult = CAGetOptionCount(opt_iter, &count);
    if (CA_STATUS_OK != countResult)
    {
        OIC_LOG_V(ERROR, TAG, "CAGetOptionCount failed with error: %d!", countResult);
        return countResult;
    }

    memset(outInfo, 0, sizeof(*outInfo));

    outInfo->numOptions = count;

#ifdef WITH_TCP
    if (CAIsSupportedCoAPOverTCP(endpoint->adapter))
    {
        // set type
        outInfo->type = CA_MSG_NONCONFIRM;
        outInfo->payloadFormat = CA_FORMAT_UNDEFINED;
    }
    else
#else
    (void) endpoint;
#endif
    {
        // set type
        outInfo->type = pdu->transport_hdr->udp.type;

        // set message id
        outInfo->messageId = pdu->transport_hdr->udp.id;
        outInfo->payloadFormat = CA_FORMAT_UNDEFINED;
        outInfo->acceptFormat = CA_FORMAT_UNDEFINED;
    }

    if (count > 0)
    {
        outInfo->options = (CAHeaderOption_t *) OICCalloc(count, sizeof(CAHeaderOption_t));
        if (NULL == outInfo->options)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            return CA_MEMORY_ALLOC_FAILED;
        }
    }

    coap_opt_t *option = NULL;
    char *optionResult = (char *)OICCalloc(1, CA_MAX_URI_LENGTH * sizeof(char));
    if (NULL == optionResult)
    {
        goto exit;
    }

    uint32_t idx = 0;
    uint32_t optionLength = 0;
    bool isfirstsetflag = false;
    bool isQueryBeingProcessed = false;
    bool isProxyRequest = false;

    while ((option = coap_option_next(&opt_iter)))
    {
        char *buf = (char *)OICCalloc(1, COAP_MAX_PDU_SIZE * sizeof(char));
        if (NULL == buf)
        {
            goto exit;
        }

        uint32_t bufLength =
            CAGetOptionData(opt_iter.type, (uint8_t *)(COAP_OPT_VALUE(option)),
                    COAP_OPT_LENGTH(option), (uint8_t *)buf, COAP_MAX_PDU_SIZE);
        if (bufLength)
        {
            OIC_LOG_V(DEBUG, TAG, "COAP URI element : %s", buf);
            if (COAP_OPTION_URI_PATH == opt_iter.type || COAP_OPTION_URI_QUERY == opt_iter.type)
            {
                if (false == isfirstsetflag)
                {
                    isfirstsetflag = true;
                    optionResult[optionLength] = '/';
                    optionLength++;
                    // Make sure there is enough room in the optionResult buffer
                    if ((optionLength + bufLength) < CA_MAX_URI_LENGTH)
                    {
                        memcpy(&optionResult[optionLength], buf, bufLength);
                        optionLength += bufLength;
                    }
                    else
                    {
                        OICFree(buf);
                        goto exit;
                    }
                }
                else
                {
                    if (COAP_OPTION_URI_PATH == opt_iter.type)
                    {
                        // Make sure there is enough room in the optionResult buffer
                        if (optionLength < CA_MAX_URI_LENGTH)
                        {
                            optionResult[optionLength] = '/';
                            optionLength++;
                        }
                        else
                        {
                            OICFree(buf);
                            goto exit;
                        }
                    }
                    else if (COAP_OPTION_URI_QUERY == opt_iter.type)
                    {
                        if (false == isQueryBeingProcessed)
                        {
                            // Make sure there is enough room in the optionResult buffer
                            if (optionLength < CA_MAX_URI_LENGTH)
                            {
                                optionResult[optionLength] = '?';
                                optionLength++;
                                isQueryBeingProcessed = true;
                            }
                            else
                            {
                                OICFree(buf);
                                goto exit;
                            }
                        }
                        else
                        {
                            // Make sure there is enough room in the optionResult buffer
                            if (optionLength < CA_MAX_URI_LENGTH)
                            {
                                optionResult[optionLength] = ';';
                                optionLength++;
                            }
                            else
                            {
                                OICFree(buf);
                                goto exit;
                            }
                        }
                    }
                    // Make sure there is enough room in the optionResult buffer
                    if ((optionLength + bufLength) < CA_MAX_URI_LENGTH)
                    {
                        memcpy(&optionResult[optionLength], buf, bufLength);
                        optionLength += bufLength;
                    }
                    else
                    {
                        OICFree(buf);
                        goto exit;
                    }
                }
            }
            else if (COAP_OPTION_BLOCK1 == opt_iter.type || COAP_OPTION_BLOCK2 == opt_iter.type
                    || COAP_OPTION_SIZE1 == opt_iter.type || COAP_OPTION_SIZE2 == opt_iter.type)
            {
                OIC_LOG_V(DEBUG, TAG, "option[%d] will be filtering", opt_iter.type);
            }
            else if (COAP_OPTION_CONTENT_FORMAT == opt_iter.type)
            {
                if (1 == COAP_OPT_LENGTH(option))
                {
                    outInfo->payloadFormat = CAConvertFormat((uint8_t)buf[0]);
                }
                else if (2 == COAP_OPT_LENGTH(option))
                {
                    unsigned int decodedFormat = coap_decode_var_bytes(COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option));
                    assert(decodedFormat <= UINT16_MAX);
                    outInfo->payloadFormat = CAConvertFormat((uint16_t)decodedFormat);
                }
                else
                {
                    outInfo->payloadFormat = CA_FORMAT_UNSUPPORTED;
                    OIC_LOG(DEBUG, TAG, "option has an unsupported format");
                }
            }
            else if (CA_OPTION_CONTENT_VERSION == opt_iter.type)
            {
                if (2 == COAP_OPT_LENGTH(option))
                {
                    unsigned int decodedVersion = coap_decode_var_bytes(COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option));
                    assert(decodedVersion <= UINT16_MAX);
                    outInfo->payloadVersion = (uint16_t)decodedVersion;
                }
                else
                {
                    OIC_LOG(DEBUG, TAG, "unsupported content version");
                    outInfo->payloadVersion = DEFAULT_VERSION_VALUE;

                }
            }
            else if (COAP_OPTION_URI_PORT == opt_iter.type ||
                    COAP_OPTION_URI_HOST == opt_iter.type ||
                    COAP_OPTION_ETAG == opt_iter.type ||
                    COAP_OPTION_MAXAGE == opt_iter.type ||
                    COAP_OPTION_PROXY_SCHEME== opt_iter.type)
            {
                OIC_LOG_V(INFO, TAG, "option[%d] has an unsupported format [%d]",
                          opt_iter.type, (uint8_t)buf[0]);
            }
            else
            {
                if (COAP_OPTION_PROXY_URI == opt_iter.type)
                {
                    isProxyRequest = true;
                }
                else if (CA_OPTION_ACCEPT_VERSION == opt_iter.type)
                {
                    if (2 == COAP_OPT_LENGTH(option))
                    {
                        unsigned int decodedVersion = coap_decode_var_bytes(COAP_OPT_VALUE(option),
                                COAP_OPT_LENGTH(option));
                        assert(decodedVersion <= UINT16_MAX);
                        outInfo->acceptVersion = (uint16_t) decodedVersion;
                    }
                    else
                    {
                        OIC_LOG(DEBUG, TAG, "unsupported accept version");
                        outInfo->acceptVersion = DEFAULT_VERSION_VALUE;
                    }
                }
                else if (COAP_OPTION_ACCEPT == opt_iter.type)
                {
                    if (1 == COAP_OPT_LENGTH(option))
                    {
                        outInfo->acceptFormat = CAConvertFormat((uint8_t) buf[0]);
                    }
                    else if (2 == COAP_OPT_LENGTH(option))
                    {
                        unsigned int decodedFormat = coap_decode_var_bytes(COAP_OPT_VALUE(option),
                                COAP_OPT_LENGTH(option));
                        assert(decodedFormat <= UINT16_MAX);
                        outInfo->acceptFormat = CAConvertFormat((uint16_t) decodedFormat);
                    }
                    else
                    {
                        outInfo->acceptFormat = CA_FORMAT_UNSUPPORTED;
                        OIC_LOG(DEBUG, TAG, "option has an unsupported accept format");
                    }
                }
                if (idx < count)
                {
                    if (bufLength <= sizeof(outInfo->options[0].optionData))
                    {
                        outInfo->options[idx].optionID = opt_iter.type;
                        assert(bufLength <= UINT16_MAX);
                        outInfo->options[idx].optionLength = (uint16_t)bufLength;
                        outInfo->options[idx].protocolID = CA_COAP_ID;
                        memcpy(outInfo->options[idx].optionData, buf, bufLength);
                        idx++;
                    }
                }
            }
        }
        OICFree(buf);
    } // while

    unsigned char* token = NULL;
    unsigned int token_length = 0;
    coap_get_token2(pdu->transport_hdr, transport, &token, &token_length);

    // set token data
    if (token_length > 0)
    {
        OIC_LOG_V(DEBUG, TAG, "inside token length : %d", token_length);
        outInfo->token = (char *) OICMalloc(token_length);
        if (NULL == outInfo->token)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            OICFree(outInfo->options);
            OICFree(optionResult);
            return CA_MEMORY_ALLOC_FAILED;
        }
        memcpy(outInfo->token, token, token_length);
    }

    assert(token_length <= UINT8_MAX);
    outInfo->tokenLength = (uint8_t)token_length;

    // set payload data
    size_t dataSize;
    uint8_t *data;
    if (coap_get_data(pdu, &dataSize, &data))
    {
        OIC_LOG(DEBUG, TAG, "inside pdu->data");
        outInfo->payload = (uint8_t *) OICMalloc(dataSize);
        if (NULL == outInfo->payload)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            OICFree(outInfo->options);
            OICFree(outInfo->token);
            OICFree(optionResult);
            return CA_MEMORY_ALLOC_FAILED;
        }
        memcpy(outInfo->payload, pdu->data, dataSize);
        outInfo->payloadSize = dataSize;
    }

    if (optionResult[0] != '\0')
    {
        optionResult[optionLength] = '\0';
        OIC_LOG_V(DEBUG, TAG, "URL length:%" PRIuPTR, strlen(optionResult));
        outInfo->resourceUri = OICStrdup(optionResult);
        if (!outInfo->resourceUri)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            OICFree(outInfo->options);
            OICFree(outInfo->token);
            OICFree(optionResult);
            return CA_MEMORY_ALLOC_FAILED;
        }
    }
    else if(isProxyRequest && g_chproxyUri[0] != '\0')
    {
       /*
        *   A request for Proxy will not have any uri element as per CoAP specs
        *   and only COAP_OPTION_PROXY_URI will be present. Use preset proxy uri
        *   for such requests.
        */
        outInfo->resourceUri = OICStrdup(g_chproxyUri);
        if (!outInfo->resourceUri)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            OICFree(outInfo->options);
            OICFree(outInfo->token);
            OICFree(optionResult);
            return CA_MEMORY_ALLOC_FAILED;
        }
    }
    OICFree(optionResult);
    OIC_LOG(INFO, TAG, "OUT - CAGetInfoFromPDU");
    return CA_STATUS_OK;

exit:
    OIC_LOG(ERROR, TAG, "buffer too small");
    OICFree(outInfo->options);
    OICFree(optionResult);
    return CA_STATUS_FAILED;
}

CAResult_t CAGetTokenFromPDU(const coap_hdr_transport_t *pdu_hdr,
                             CAInfo_t *outInfo,
                             const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL(pdu_hdr, TAG, "pdu_hdr");
    VERIFY_NON_NULL(outInfo, TAG, "outInfo");
    VERIFY_NON_NULL(endpoint, TAG, "endpoint");

    coap_transport_t transport = COAP_UDP;
#ifdef WITH_TCP
    if (CAIsSupportedCoAPOverTCP(endpoint->adapter))
    {
        transport = coap_get_tcp_header_type_from_initbyte(((unsigned char *)pdu_hdr)[0] >> 4);
    }
#endif

    unsigned char* token = NULL;
    unsigned int token_length = 0;
    coap_get_token2(pdu_hdr, transport, &token, &token_length);

    // set token data
    if (token_length > 0)
    {
        OIC_LOG_V(DEBUG, TAG, "token len:%d", token_length);
        outInfo->token = (char *) OICMalloc(token_length);
        if (NULL == outInfo->token)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            return CA_MEMORY_ALLOC_FAILED;
        }
        memcpy(outInfo->token, token, token_length);
    }

    assert(token_length <= UINT8_MAX);
    outInfo->tokenLength = (uint8_t)token_length;

    return CA_STATUS_OK;
}

CAResult_t CAGenerateTokenInternal(CAToken_t *token, uint8_t tokenLength)
{
    VERIFY_NON_NULL(token, TAG, "token");

    if ((tokenLength > CA_MAX_TOKEN_LEN) || (0 == tokenLength))
    {
        OIC_LOG(ERROR, TAG, "invalid token length");
        return CA_STATUS_INVALID_PARAM;
    }

    // memory allocation, token is stored as a Pascal-style string
    char *temp = (char *) OICCalloc(tokenLength + 1, sizeof(char));
    if (NULL == temp)
    {
        OIC_LOG(ERROR, TAG, "Out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    *temp++ = tokenLength;
    if (!OCGetRandomBytes((uint8_t *)temp, tokenLength))
    {
        OIC_LOG(ERROR, TAG, "Failed to generate random token");
        return CA_STATUS_FAILED;
    }

    // save token
    *token = temp;

    OIC_LOG_V(DEBUG, TAG, "token len:%d, token:", tokenLength);
    OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *)(*token), tokenLength);

    return CA_STATUS_OK;
}

void CADestroyTokenInternal(CAToken_t token)
{
    if (token)
    {
        char *temp = token - 1;
#ifdef WITH_BWT
        CARemoveBlockMulticastDataFromListWithSeed(token, *temp);
#endif
        OICFree(temp);
    }
}

uint32_t CAGetOptionData(uint16_t key, const uint8_t *data, uint32_t len,
        uint8_t *option, uint32_t buflen)
{
    if (0 == buflen)
    {
        OIC_LOG(ERROR, TAG, "buflen 0");
        return 0;
    }

    if (buflen <= len)
    {
        OIC_LOG(ERROR, TAG, "option buffer too small");
        return 0;
    }

    coap_option_def_t* def = coap_opt_def(key);
    if (NULL != def && coap_is_var_bytes(def) && 0 == len)
    {
        // A 0 length option is permitted in CoAP but the
        // rest or the stack is unaware of variable byte encoding
        // should remain that way so a 0 byte of length 1 is inserted.
        len = 1;
        option[0]=0;
    }
    else
    {
        memcpy(option, data, len);
        option[len] = '\0';
    }

    return len;
}

CAMessageType_t CAGetMessageTypeFromPduBinaryData(const void *pdu, uint32_t size)
{
    VERIFY_NON_NULL_RET(pdu, TAG, "pdu", CA_MSG_NONCONFIRM);

    // pdu minimum size is 4 byte.
    if (size < CA_PDU_MIN_SIZE)
    {
        OIC_LOG(ERROR, TAG, "min size");
        return CA_MSG_NONCONFIRM;
    }

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;

    return (CAMessageType_t) hdr->type;
}

uint16_t CAGetMessageIdFromPduBinaryData(const void *pdu, uint32_t size)
{
    VERIFY_NON_NULL_RET(pdu, TAG, "pdu", 0);

    // pdu minimum size is 4 byte.
    if (size < CA_PDU_MIN_SIZE)
    {
        OIC_LOG(ERROR, TAG, "min size");
        return 0;
    }

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;

    return hdr->id;
}

CAResponseResult_t CAGetCodeFromPduBinaryData(const void *pdu, uint32_t size)
{
    VERIFY_NON_NULL_RET(pdu, TAG, "pdu", CA_NOT_FOUND);

    // pdu minimum size is 4 byte.
    if (size < CA_PDU_MIN_SIZE)
    {
        OIC_LOG(ERROR, TAG, "min size");
        return CA_NOT_FOUND;
    }

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;

    return (CAResponseResult_t) CA_RESPONSE_CODE(hdr->code);
}

CAPayloadFormat_t CAConvertFormat(uint16_t format)
{
    switch (format)
    {
        case COAP_MEDIATYPE_TEXT_PLAIN:
            return CA_FORMAT_TEXT_PLAIN;
        case COAP_MEDIATYPE_APPLICATION_LINK_FORMAT:
            return CA_FORMAT_APPLICATION_LINK_FORMAT;
        case COAP_MEDIATYPE_APPLICATION_XML:
            return CA_FORMAT_APPLICATION_XML;
        case COAP_MEDIATYPE_APPLICATION_OCTET_STREAM:
            return CA_FORMAT_APPLICATION_OCTET_STREAM;
        case COAP_MEDIATYPE_APPLICATION_RDF_XML:
            return CA_FORMAT_APPLICATION_RDF_XML;
        case COAP_MEDIATYPE_APPLICATION_EXI:
            return CA_FORMAT_APPLICATION_EXI;
        case COAP_MEDIATYPE_APPLICATION_JSON:
            return CA_FORMAT_APPLICATION_JSON;
        case COAP_MEDIATYPE_APPLICATION_CBOR:
            return CA_FORMAT_APPLICATION_CBOR;
        case COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR:
            return CA_FORMAT_APPLICATION_VND_OCF_CBOR;
        default:
            return CA_FORMAT_UNSUPPORTED;
    }
}

#ifdef WITH_BWT
bool CAIsSupportedBlockwiseTransfer(CATransportAdapter_t adapter)
{
    if (CA_ADAPTER_IP & adapter || CA_ADAPTER_NFC & adapter
            || CA_DEFAULT_ADAPTER == adapter)
    {
        return true;
    }
    OIC_LOG_V(INFO, TAG, "adapter value of BWT is %d", adapter);
    return false;
}
#endif

#ifdef WITH_TCP
bool CAIsSupportedCoAPOverTCP(CATransportAdapter_t adapter)
{
    if (CA_ADAPTER_GATT_BTLE & adapter || CA_ADAPTER_RFCOMM_BTEDR & adapter
            || CA_ADAPTER_TCP & adapter || CA_DEFAULT_ADAPTER == adapter)
    {
        return true;
    }
    OIC_LOG_V(INFO, TAG, "adapter value of CoAP/TCP is %d", adapter);
    return false;
}
#endif
