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

#define CA_TCP_RESPONSE_CLASS(C) (((C) >> 5)*100)

#define CA_TCP_RESPONSE_CODE(C) \
    (CA_TCP_RESPONSE_CLASS(C) \
            + (C - COAP_RESPONSE_CODE(CA_TCP_RESPONSE_CLASS(C))))

/**
 * Store the connected TCP session information.
 */
u_arraylist_t *s_sessionList = NULL;

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
    CACSMExchangeState_t CSMState;      /**< Capability and Setting Message shared status */
    bool isClient;                      /**< Host Mode of Operation. */
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

void CADtorTCPSession(CATCPSessionInfo_t *removedData)
{
    OIC_LOG_V(DEBUG, TAG, "%s", __func__);

    if (!removedData) {
        return;
    }

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
    OICFree(removedData);

    OIC_LOG(DEBUG, TAG, "data is removed");
}

void CAAcceptConnection(CATransportFlags_t flag, CASocket_t *sock)
{
    VERIFY_NON_NULL_VOID(sock, TAG, "sock is NULL");

    struct sockaddr_storage clientaddr;
    socklen_t clientlen = sizeof (struct sockaddr_in);
    if (flag & CA_IPV6)
    {
        clientlen = sizeof(struct sockaddr_in6);
    }

    CASocketFd_t sockfd = accept(sock->fd, (struct sockaddr *)&clientaddr, &clientlen);
    if (OC_INVALID_SOCKET != sockfd)
    {
        CATCPSessionInfo_t *svritem =
                (CATCPSessionInfo_t *) OICCalloc(1, sizeof (*svritem));
        if (!svritem)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            OC_CLOSE_SOCKET(sockfd);
            return;
        }

        OICClearMemory(svritem, 0);
        svritem->fd = sockfd;
        svritem->sep.endpoint.flags = flag;
        svritem->sep.endpoint.adapter = CA_ADAPTER_TCP;
        svritem->state = CONNECTED;
        svritem->isClient = false;
        CAConvertAddrToName((struct sockaddr_storage *)&clientaddr, clientlen,
                            svritem->sep.endpoint.addr, &svritem->sep.endpoint.port);

        oc_refcounter ref = oc_refcounter_create(svritem, (oc_refcounter_dtor_data_func) CADtorTCPSession);
        if (!ref)
        {
            OICFree(svritem);
            OIC_LOG(ERROR, TAG, "Out of memory");
            OC_CLOSE_SOCKET(sockfd);
            return;
        }

        oc_mutex_lock(tcp_mutexObjectList);
        u_arraylist_add(s_sessionList, ref);
        oc_mutex_unlock(tcp_mutexObjectList);

        TCP_CHECKFD(sockfd);

        // pass the connection information to CA Common Layer.
        if (tcp_connectionCallback)
        {
            tcp_connectionCallback(&(svritem->sep.endpoint), true, svritem->isClient);
        }
    }
}

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

uint32_t CAGetCodeFromHeader(const unsigned char *recvBuffer)
{
    OIC_LOG(DEBUG, TAG, "IN - CAGetCodeFromHeader");

    coap_transport_t transport = coap_get_tcp_header_type_from_initbyte(
                ((unsigned char *)recvBuffer)[0] >> 4);
    size_t headerLen = coap_get_tcp_header_length_for_transport(transport);
    uint32_t code = CA_TCP_RESPONSE_CODE(recvBuffer[headerLen -1]);

    OIC_LOG_V(DEBUG, TAG, "header length [%zu]", headerLen);
    OIC_LOG_V(DEBUG, TAG, "code [%d]", code);

    OIC_LOG(DEBUG, TAG, "OUT - CAGetCodeFromHeader");
    return code;
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

    oc_refcounter ref = oc_refcounter_create(svritem, (oc_refcounter_dtor_data_func) CADtorTCPSession);
    if (!ref)
    {
        OICFree(svritem);
        OIC_LOG(ERROR, TAG, "Out of memory");
        return OC_INVALID_SOCKET;
    }

    // #2. add TCP connection info to list
    oc_mutex_lock(tcp_mutexObjectList);
    u_arraylist_add(s_sessionList, ref);
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

CATCPSessionInfo_t* session_list_get(u_arraylist_t* sessionList, size_t index)
{
    oc_refcounter ref = (oc_refcounter)u_arraylist_get(sessionList, index);
    return (CATCPSessionInfo_t*) oc_refcounter_get_data(ref);
}

void CARemoveSession(CATCPSessionInfo_t *session)
{
    oc_refcounter ref = NULL;
    oc_mutex_lock(tcp_mutexObjectList);
    size_t length = u_arraylist_length(s_sessionList);
    for (size_t i = 0; i < length; ++i)
    {
        oc_refcounter tmp = (oc_refcounter) u_arraylist_get(s_sessionList, i);
        if (oc_refcounter_get_data(tmp) == session) {
            //swap last element with current position and remove last element
            u_arraylist_swap(s_sessionList, i, length-1);
            ref = (oc_refcounter) u_arraylist_remove(s_sessionList, length-1);
            break;
        }
    }
    oc_mutex_unlock(tcp_mutexObjectList);
    if (ref)
    {
        oc_refcounter_dec(ref);
    }
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

CAResult_t CATCPDisconnectSession(const CAEndpoint_t *endpoint)
{
    CAResult_t res = CA_STATUS_OK;
#ifdef __WITH_TLS__
    res = CAcloseSslConnection(endpoint);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "failed to close TLS session");
    }
#endif

    res = CASearchAndDeleteTCPSession(endpoint);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "failed to close TCP session");
    }

    return res;
}

void CATCPDisconnectAll()
{
    oc_mutex_lock(tcp_mutexObjectList);
    u_arraylist_t* sessionList = s_sessionList;
    s_sessionList = NULL;
    oc_mutex_unlock(tcp_mutexObjectList);
    for (size_t i = 0; i < u_arraylist_length(sessionList); ++i)
    {
        oc_refcounter_dec((oc_refcounter)u_arraylist_get(sessionList, i));
    }

#ifdef __WITH_TLS__
    CAcloseSslConnectionAll(CA_ADAPTER_TCP);
#endif

}

oc_refcounter CAGetTCPSessionInfoRefCountedFromEndpoint(const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint is NULL", NULL);

    OIC_LOG_V(DEBUG, TAG, "Looking for [%s:%d]", endpoint->addr, endpoint->port);

    oc_mutex_lock(tcp_mutexObjectList);

    // get connection info from list
    for (size_t i = 0; i < u_arraylist_length(s_sessionList); ++i)
    {
        oc_refcounter ref = (oc_refcounter) u_arraylist_get(s_sessionList, i);
        CATCPSessionInfo_t *session = (CATCPSessionInfo_t *) oc_refcounter_get_data(ref);
        if (!strncmp(session->sep.endpoint.addr, endpoint->addr,
                     sizeof(session->sep.endpoint.addr))
                && (session->sep.endpoint.port == endpoint->port)
                && (session->sep.endpoint.flags & endpoint->flags))
        {
            OIC_LOG(DEBUG, TAG, "Found in session list");
            oc_refcounter_inc(ref);
            oc_mutex_unlock(tcp_mutexObjectList);
            return ref;
        }
    }
    oc_mutex_unlock(tcp_mutexObjectList);

    OIC_LOG(DEBUG, TAG, "Session not found");
    return NULL;
}

CAResult_t CASearchAndDeleteTCPSession(const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL_MSG(endpoint, TAG, "endpoint is NULL");

    OIC_LOG_V(DEBUG, TAG, "Looking for [%s:%d]", endpoint->addr, endpoint->port);

    // get connection info from list
    oc_refcounter ref = NULL;

    oc_mutex_lock(tcp_mutexObjectList);
    size_t length = u_arraylist_length(s_sessionList);
    for (size_t i = 0; i < length; ++i)
    {
        CATCPSessionInfo_t *s = session_list_get(s_sessionList, i);
        if (!strncmp(s->sep.endpoint.addr, endpoint->addr,
                     sizeof(s->sep.endpoint.addr))
                && (s->sep.endpoint.port == endpoint->port)
                && (s->sep.endpoint.flags & endpoint->flags))
        {
            u_arraylist_swap(s_sessionList, i, length-1);
            ref = (oc_refcounter)u_arraylist_remove(s_sessionList, length-1);
            break;
        }
    }
    oc_mutex_unlock(tcp_mutexObjectList);

    if (ref)
    {
       OIC_LOG(DEBUG, TAG, "Found in session list");
       oc_refcounter_dec(ref);
       return CA_STATUS_OK;
    }

    OIC_LOG(DEBUG, TAG, "Session not found");
    return CA_STATUS_OK;
}

void CATCPCloseInProgressConnections()
{
    OIC_LOG(INFO, TAG, "IN - CATCPCloseInProgressConnections");

#ifndef WSA_WAIT_EVENT_0
    oc_mutex_lock(tcp_mutexObjectList);

    for (size_t i = 0; i < u_arraylist_length(s_sessionList); ++i)
    {
        CATCPSessionInfo_t *session = session_list_get(s_sessionList, i);
        if (session && session->fd >= 0 && session->state == CONNECTING)
        {
            shutdown(session->fd, SHUT_RDWR);
            close(session->fd);
            session->fd = -1;
            session->state = DISCONNECTED;
        }
    }

    oc_mutex_unlock(tcp_mutexObjectList);
#endif
    OIC_LOG(INFO, TAG, "OUT - CATCPCloseInProgressConnections");
}
