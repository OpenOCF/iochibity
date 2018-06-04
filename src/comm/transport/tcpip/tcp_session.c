/* ****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "tcp_session.h"

#include "utlist.h"

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_POLL_H
#include <sys/poll.h>
#endif
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#include <errno.h>
#include <assert.h>

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#include <coap/pdu.h>
#include <inttypes.h>

#define TAG "TCPSESSION"

/**
 * TCP Session Information for IPv4/IPv6 TCP transport
 */
#if INTERFACE
typedef struct CATCPSessionInfo_t
{
    CASecureEndpoint_t sep;             /**< secure endpoint information */
    CASocketFd_t fd;                    /**< file descriptor info */
    unsigned char* data;                /**< received data from remote device */
    size_t len;                         /**< received data length */
    size_t totalLen;                    /**< total coap data length required to receive */
    unsigned char tlsdata[18437];       /**< tls data(rfc5246: TLSCiphertext max (2^14+2048+5)) */
    size_t tlsLen;                      /**< received tls data length */
    CAProtocol_t protocol;              /**< application-level protocol */
    CATCPConnectionState_t state;       /**< current tcp session state */
    bool isClient;                      /**< Host Mode of Operation. */
    struct CATCPSessionInfo_t *next;    /**< Linked list; for multiple session list. */
} CATCPSessionInfo_t;

/**
 * TCP Connection State.
 */
typedef enum
{
    CONNECTING = 0,
    CONNECTED,
    DISCONNECTED
} CATCPConnectionState_t;
#endif

/**
 * Store the connected TCP session information.
 */
CATCPSessionInfo_t *tcp_sessionList = NULL;

/**
 * Clean socket state data
 *
 * @param[in/out] item - socket state data
 */
void CACleanData(CATCPSessionInfo_t *svritem)
{
    if (svritem)
    {
        OICFree(svritem->data);
        svritem->data = NULL;
        svritem->len = 0;
        svritem->tlsLen = 0;
        svritem->totalLen = 0;
        svritem->protocol = UNKNOWN;
    }
}

/**
 * Maximum CoAP over TCP header length
 * to know the total data length.
 */
#define COAP_MAX_HEADER_SIZE  6

/**
 * Construct CoAP header and payload from buffer
 *
 * @param[in] svritem - used socket, buffer, current received message length and protocol
 * @param[in/out]  data  - data buffer, this value is updated as data is copied to svritem
 * @param[in/out]  dataLength  - length of data, this value decreased as data is copied to svritem
 * @return             - CA_STATUS_OK or appropriate error code
 */
CAResult_t CAConstructCoAP(CATCPSessionInfo_t *svritem, unsigned char **data,
                          size_t *dataLength)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    if (NULL == svritem || NULL == data || NULL == dataLength)
    {
        OIC_LOG(ERROR, TAG, "Invalid input parameter(NULL)");
        return CA_STATUS_INVALID_PARAM;
    }

    unsigned char *inBuffer = *data;
    size_t inLen = *dataLength;
    OIC_LOG_V(DEBUG, TAG, "before-datalength : %" PRIuPTR, *dataLength);

    if (NULL == svritem->data && inLen > 0)
    {
        // allocate memory for message header (CoAP header size because it is bigger)
        svritem->data = (unsigned char *) OICCalloc(1, COAP_MAX_HEADER_SIZE);
        if (NULL == svritem->data)
        {
            OIC_LOG(ERROR, TAG, "OICCalloc - out of memory");
            return CA_MEMORY_ALLOC_FAILED;
        }

        // copy 1 byte to parse coap header length
        memcpy(svritem->data, inBuffer, 1);
        svritem->len = 1;
        inBuffer++;
        inLen--;
    }

    //if not enough data received - read them on next CAFillHeader() call
    if (0 == inLen)
    {
        return CA_STATUS_OK;
    }

    //if enough data received - parse header
    svritem->protocol = COAP;

    //seems CoAP data received. read full coap header.
    coap_transport_t transport = coap_get_tcp_header_type_from_initbyte(svritem->data[0] >> 4);
    size_t headerLen = coap_get_tcp_header_length_for_transport(transport);
    size_t copyLen = 0;

    // HEADER
    if (svritem->len < headerLen)
    {
        copyLen = headerLen - svritem->len;
        if (inLen < copyLen)
        {
            copyLen = inLen;
        }

        //read required bytes to have full CoAP header
        memcpy(svritem->data + svritem->len, inBuffer, copyLen);
        svritem->len += copyLen;
        inBuffer += copyLen;
        inLen -= copyLen;

        //if not enough data received - read them on next CAFillHeader() call
        if (svritem->len < headerLen)
        {
            *data = inBuffer;
            *dataLength = inLen;
            OIC_LOG(DEBUG, TAG, "CoAP header received partially. Wait for rest header data");
            return CA_STATUS_OK;
        }

        //calculate CoAP message length
        svritem->totalLen = CAGetTotalLengthFromHeader(svritem->data);

        // allocate required memory
        unsigned char *buffer = OICRealloc(svritem->data, svritem->totalLen);
        if (NULL == buffer)
        {
            OIC_LOG(ERROR, TAG, "OICRealloc - out of memory");
            return CA_MEMORY_ALLOC_FAILED;
        }
        svritem->data = buffer;
    }

    // PAYLOAD
    if (inLen > 0)
    {
        // read required bytes to have full CoAP payload
        copyLen = svritem->totalLen - svritem->len;
        if (inLen < copyLen)
        {
            copyLen = inLen;
        }

        //read required bytes to have full CoAP header
        memcpy(svritem->data + svritem->len, inBuffer, copyLen);
        svritem->len += copyLen;
        inBuffer += copyLen;
        inLen -= copyLen;
    }

    *data = inBuffer;
    *dataLength = inLen;

    OIC_LOG_V(DEBUG, TAG, "after-datalength : %" PRIuPTR, *dataLength);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}

size_t CACheckPayloadLengthFromHeader(const void *data, size_t dlen)
{
    VERIFY_NON_NULL_RET(data, TAG, "data", 0);

    coap_transport_t transport = coap_get_tcp_header_type_from_initbyte(
            ((unsigned char *)data)[0] >> 4);

    coap_pdu_t *pdu = coap_pdu_init2(0, 0,
                                     ntohs((unsigned short)COAP_INVALID_TID),
                                     dlen, transport);
    if (!pdu)
    {
        OIC_LOG(ERROR, TAG, "outpdu is null");
        OIC_LOG_V(DEBUG, TAG, "data length: %" PRIuPTR, dlen);
        return 0;
    }

    int ret = coap_pdu_parse2((unsigned char *) data, dlen, pdu, transport);
    if (0 >= ret)
    {
        OIC_LOG(ERROR, TAG, "pdu parse failed");
        coap_delete_pdu(pdu);
        return 0;
    }

    size_t payloadLen = 0;
    size_t headerSize = coap_get_tcp_header_length_for_transport(transport);
    OIC_LOG_V(DEBUG, TAG, "headerSize : %" PRIuPTR ", pdu length : %d",
              headerSize, pdu->length);
    if (pdu->length > headerSize)
    {
        payloadLen = (unsigned char *) pdu->hdr + pdu->length - pdu->data;
    }

    OICFree(pdu);

    return payloadLen;
}

size_t CAGetTotalLengthFromHeader(const unsigned char *recvBuffer)
{
    OIC_LOG(DEBUG, TAG, "IN - CAGetTotalLengthFromHeader");

    coap_transport_t transport = coap_get_tcp_header_type_from_initbyte(
            ((unsigned char *)recvBuffer)[0] >> 4);
    size_t optPaylaodLen = coap_get_length_from_header((unsigned char *)recvBuffer,
                                                        transport);
    size_t headerLen = coap_get_tcp_header_length((unsigned char *)recvBuffer);

    OIC_LOG_V(DEBUG, TAG, "option/paylaod length [%" PRIuPTR "]", optPaylaodLen);
    OIC_LOG_V(DEBUG, TAG, "header length [%" PRIuPTR "]", headerLen);
    OIC_LOG_V(DEBUG, TAG, "total data length [%" PRIuPTR "]", headerLen + optPaylaodLen);

    OIC_LOG(DEBUG, TAG, "OUT - CAGetTotalLengthFromHeader");
    return headerLen + optPaylaodLen;
}

CASocketFd_t CAConnectTCPSession(const CAEndpoint_t *endpoint)
{
    OIC_LOG_V(DEBUG, TAG, "%s", __func__);
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint is NULL", OC_INVALID_SOCKET);

    // #1. create TCP server object
    CATCPSessionInfo_t *svritem = (CATCPSessionInfo_t *) OICCalloc(1, sizeof (*svritem));
    if (!svritem)
    {
        OIC_LOG(ERROR, TAG, "Out of memory");
        return OC_INVALID_SOCKET;
    }
    svritem->sep.endpoint = *endpoint;
    svritem->state = CONNECTING;
    svritem->isClient = true;

    // #2. add TCP connection info to list
    oc_mutex_lock(tcp_mutexObjectList);
    LL_APPEND(tcp_sessionList, svritem);
    oc_mutex_unlock(tcp_mutexObjectList);

    // #3. create the socket and connect to TCP server
    int family = (svritem->sep.endpoint.flags & CA_IPV6) ? AF_INET6 : AF_INET;
    if (CA_STATUS_OK != CATCPCreateSocket(family, svritem))
    {
        return OC_INVALID_SOCKET;
    }

    // #4. pass the connection information to CA Common Layer.
    if (tcp_connectionCallback)
    {
        tcp_connectionCallback(&(svritem->sep.endpoint), true, svritem->isClient);
    }

    return svritem->fd;
}

CAResult_t CADisconnectTCPSession(CATCPSessionInfo_t *removedData)
{
    OIC_LOG_V(DEBUG, TAG, "%s", __func__);

    VERIFY_NON_NULL_MSG(removedData, TAG, "removedData is NULL");

    // close the socket and remove session info in list.
    if (removedData->fd != OC_INVALID_SOCKET)
    {
        shutdown(removedData->fd, SHUT_RDWR);
        OC_CLOSE_SOCKET(removedData->fd);
        removedData->fd = OC_INVALID_SOCKET;
        OIC_LOG(DEBUG, TAG, "close socket");
        removedData->state = (CONNECTED == removedData->state) ?
                                    DISCONNECTED : removedData->state;

        // pass the connection information to CA Common Layer.
        if (tcp_connectionCallback && DISCONNECTED == removedData->state)
        {
            tcp_connectionCallback(&(removedData->sep.endpoint), false, removedData->isClient);
        }
    }
    OICFree(removedData->data);
    removedData->data = NULL;

    OICFree(removedData);

    OIC_LOG(DEBUG, TAG, "data is removed from session list");
    return CA_STATUS_OK;
}

void CATCPDisconnectAll()
{
    oc_mutex_lock(tcp_mutexObjectList);
    CATCPSessionInfo_t *session = NULL;
    CATCPSessionInfo_t *tmp = NULL;
    LL_FOREACH_SAFE(tcp_sessionList, session, tmp)
    {
        if (session)
        {
            LL_DELETE(tcp_sessionList, session);
            // disconnect session from remote device.
            CADisconnectTCPSession(session);
        }
    }

    tcp_sessionList = NULL;
    oc_mutex_unlock(tcp_mutexObjectList);

#ifdef __WITH_TLS__
    CAcloseSslConnectionAll(CA_ADAPTER_TCP);
#endif

}

CATCPSessionInfo_t *CAGetTCPSessionInfoFromEndpoint(const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint is NULL", NULL);

    OIC_LOG_V(DEBUG, TAG, "Looking for [%s:%d]", endpoint->addr, endpoint->port);

    // get connection info from list
    CATCPSessionInfo_t *session = NULL;
    LL_FOREACH(tcp_sessionList, session)
    {
        if (!strncmp(session->sep.endpoint.addr, endpoint->addr,
                     sizeof(session->sep.endpoint.addr))
                && (session->sep.endpoint.port == endpoint->port)
                && (session->sep.endpoint.flags & endpoint->flags))
        {
            OIC_LOG(DEBUG, TAG, "Found in session list");
            return session;
        }
    }

    OIC_LOG(DEBUG, TAG, "Session not found");
    return NULL;
}

CAResult_t CASearchAndDeleteTCPSession(const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL_MSG(endpoint, TAG, "endpoint is NULL");

    OIC_LOG_V(DEBUG, TAG, "Looking for [%s:%d]", endpoint->addr, endpoint->port);

    // get connection info from list
    CATCPSessionInfo_t *session = NULL;
    CATCPSessionInfo_t *tmp = NULL;

    oc_mutex_lock(tcp_mutexObjectList);
    LL_FOREACH_SAFE(tcp_sessionList, session, tmp)
    {
        if (!strncmp(session->sep.endpoint.addr, endpoint->addr,
                     sizeof(session->sep.endpoint.addr))
                && (session->sep.endpoint.port == endpoint->port)
                && (session->sep.endpoint.flags & endpoint->flags))
        {
            OIC_LOG(DEBUG, TAG, "Found in session list");
            LL_DELETE(tcp_sessionList, session);
            CADisconnectTCPSession(session);
            oc_mutex_unlock(tcp_mutexObjectList);
            return CA_STATUS_OK;
        }
    }
    oc_mutex_unlock(tcp_mutexObjectList);

    OIC_LOG(DEBUG, TAG, "Session not found");
    return CA_STATUS_OK;
}
