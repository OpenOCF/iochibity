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

#include "tcp_data_receiver.h"

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

#define TAG "TCPRCVR"

/**
 * TLS header size
 */
#define TLS_HEADER_SIZE 5

//CANetworkPacketReceivedCallback tcp_networkPacketCallback = NULL;
void (*tcp_networkPacketCallback)(const CASecureEndpoint_t *sep,
					 const void *data,
					 size_t dataLen) = NULL;

void CATCPPacketReceivedCB(const CASecureEndpoint_t *sep,
			   const void *data,
                           size_t dataLength)
{
    VERIFY_NON_NULL_VOID(sep, TAG, "sep is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    OIC_LOG_V(DEBUG, TAG, "Address: %s, port:%d", sep->endpoint.addr, sep->endpoint.port);

#ifdef SINGLE_THREAD
    if (tcp_networkPacketCallback)
    {
        tcp_networkPacketCallback(sep, data, dataLength);
    }
#else
    unsigned char *buffer = (unsigned char*)data;
    size_t bufferLen = dataLength;

    //get remote device information from file descriptor.
    CATCPSessionInfo_t *svritem = CAGetTCPSessionInfoFromEndpoint(&sep->endpoint);
    if (!svritem)
    {
        OIC_LOG(ERROR, TAG, "there is no connection information in list");
        return;
    }
    if (UNKNOWN == svritem->protocol)
    {
        OIC_LOG(ERROR, TAG, "invalid protocol type");
        return;
    }

    //totalLen filled only when header fully read and parsed
    while (0 != bufferLen)
    {
        CAResult_t res = CAConstructCoAP(svritem, &buffer, &bufferLen);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG_V(ERROR, TAG, "CAConstructCoAP return error : %d", res);
            return;
        }

        //when successfully read all required data - pass them to upper layer.
        if (svritem->len == svritem->totalLen)
        {
	    // tcp_networkPacketCallback always set to ifc_CAReceivedPacketCallback
	    // ifc_CAReceivedPacketCallback calls g_networkPacketReceivedCallback (and does nothing else)
	    // g_networkPacketReceivedCallback always set to mh_CAReceivedPacketCallback
	    // 
            /* if (tcp_networkPacketCallback) */
            /* { */
            /*     tcp_networkPacketCallback(sep, svritem->data, svritem->totalLen); */
            /* } */
	    mh_CAReceivedPacketCallback(sep, svritem->data, svritem->totalLen);
            CACleanData(svritem);
        }
        else
        {
            OIC_LOG_V(DEBUG, TAG, "%" PRIuPTR " bytes required for complete CoAP",
                                svritem->totalLen - svritem->len);
        }
    }
#endif
}

// static void CAReceiveHandler(void *data)
void tcp_data_receiver_runloop(void *data) // @was CAReceiveHandler
{
    (void)data;
    OIC_LOG(DEBUG, TAG, "IN - CAReceiveHandler");

    //while (!caglobals.tcp.terminate)
    while (!tcp_is_terminating)
    {
        udp_handle_inbound_data();  // @was CAFindReadyMessage();
    }

    oc_mutex_lock(tcp_mutexObjectList);
    oc_cond_signal(tcp_condObjectList);
    oc_mutex_unlock(tcp_mutexObjectList);

    OIC_LOG(DEBUG, TAG, "OUT - CAReceiveHandler");
}

LOCAL CAResult_t CAReceiveMessage(CATCPSessionInfo_t *svritem)
{
    VERIFY_NON_NULL_MSG(svritem, TAG, "svritem is NULL");

    // read data
    int len = 0;
    CAResult_t res = CA_STATUS_OK;
    if (svritem->sep.endpoint.flags & CA_SECURE)
    {
        svritem->protocol = TLS;

#ifdef __WITH_TLS__
        size_t nbRead = 0;
        size_t tlsLength = 0;

        if (TLS_HEADER_SIZE > svritem->tlsLen)
        {
            nbRead = TLS_HEADER_SIZE - svritem->tlsLen;
        }
        else
        {
            //[3][4] bytes in tls header are tls payload length
            tlsLength = TLS_HEADER_SIZE +
                            (size_t)((svritem->tlsdata[3] << 8) | svritem->tlsdata[4]);
            OIC_LOG_V(DEBUG, TAG, "total tls length = %" PRIuPTR, tlsLength);
            if (tlsLength > sizeof(svritem->tlsdata))
            {
                OIC_LOG_V(ERROR, TAG, "total tls length is too big (buffer size : %" PRIuPTR ")",
                                    sizeof(svritem->tlsdata));
                if (CA_STATUS_OK != CAcloseSslConnection(&svritem->sep.endpoint))
                {
                    OIC_LOG(ERROR, TAG, "Failed to close TLS session");
                }
                CASearchAndDeleteTCPSession(&(svritem->sep.endpoint));
                return CA_RECEIVE_FAILED;
            }
            nbRead = tlsLength - svritem->tlsLen;
        }

        len = recv(svritem->fd, (char*)svritem->tlsdata + svritem->tlsLen, (int)nbRead, 0);
        if (len < 0)
        {
            OIC_LOG_V(ERROR, TAG, "recv failed %s", strerror(errno));
            res = CA_RECEIVE_FAILED;
        }
        else if (0 == len)
        {
            OIC_LOG(INFO, TAG, "Received disconnect from peer. Close connection");
            res = CA_DESTINATION_DISCONNECTED;
        }
        else
        {
            svritem->tlsLen += len;
            OIC_LOG_V(DEBUG, TAG, "nb_read : %" PRIuPTR " bytes , recv() : %d bytes, svritem->tlsLen : %" PRIuPTR " bytes",
                                nbRead, len, svritem->tlsLen);
            if (tlsLength > 0 && tlsLength == svritem->tlsLen)
            {
                //when successfully read data - pass them to callback.
                res = CAdecryptSsl(&svritem->sep, (uint8_t *)svritem->tlsdata, (int)svritem->tlsLen);
                svritem->tlsLen = 0;
                OIC_LOG_V(DEBUG, TAG, "%s: CAdecryptSsl returned %d", __func__, res);
            }
        }
#endif

    }
    else
    {
        svritem->protocol = COAP;

        // svritem->tlsdata can also be used as receiving buffer in case of raw tcp
        len = recv(svritem->fd, (char*)svritem->tlsdata, sizeof(svritem->tlsdata), 0);
        if (len < 0)
        {
            OIC_LOG_V(ERROR, TAG, "recv failed %s", strerror(errno));
            res = CA_RECEIVE_FAILED;
        }
        else if (0 == len)
        {
            OIC_LOG(INFO, TAG, "Received disconnect from peer. Close connection");
            res = CA_DESTINATION_DISCONNECTED;
        }
        else
        {
            OIC_LOG_V(DEBUG, TAG, "recv() : %d bytes", len);
            //when successfully read data - pass them to callback.
            /* if (tcp_packetReceivedCallback) */
            /* { */
            /*     tcp_packetReceivedCallback(&svritem->sep, svritem->tlsdata, len); */
            /* } */
	    CATCPPacketReceivedCB(&svritem->sep, svritem->tlsdata, len);
        }
    }

    return res;
}

CAResult_t CAReadTCPData()
{
    OIC_LOG(DEBUG, TAG, "IN");
#ifdef SINGLE_THREAD
    CATCPPullData();
#endif
    return CA_STATUS_OK;
}

#ifdef SINGLE_THREAD
size_t CAGetTotalLengthFromPacketHeader(const unsigned char *recvBuffer, size_t size)
{
    OIC_LOG(DEBUG, TAG, "IN - CAGetTotalLengthFromHeader");

    if (NULL == recvBuffer || !size)
    {
        OIC_LOG(ERROR, TAG, "recvBuffer is NULL");
        return 0;
    }

    coap_transport_t transport = coap_get_tcp_header_type_from_initbyte(
            ((unsigned char *)recvBuffer)[0] >> 4);
    size_t optPaylaodLen = coap_get_length_from_header((unsigned char *)recvBuffer,
                                                        transport);
    size_t headerLen = coap_get_tcp_header_length((unsigned char *)recvBuffer);

    OIC_LOG_V(DEBUG, TAG, "option/paylaod length [%d]", optPaylaodLen);
    OIC_LOG_V(DEBUG, TAG, "header length [%d]", headerLen);
    OIC_LOG_V(DEBUG, TAG, "total data length [%d]", headerLen + optPaylaodLen);

    OIC_LOG(DEBUG, TAG, "OUT - CAGetTotalLengthFromHeader");
    return headerLen + optPaylaodLen;
}

void CAGetTCPHeaderDetails(unsigned char* recvBuffer, coap_transport_t *transport,
                           size_t *headerlen)
{
    if (NULL == recvBuffer)
    {
        OIC_LOG(ERROR, TAG, "recvBuffer is NULL");
        return;
    }

    if (NULL == transport)
    {
        OIC_LOG(ERROR, TAG, "transport is NULL");
        return;
    }

    if (NULL == headerlen)
    {
        OIC_LOG(ERROR, TAG, "headerlen is NULL");
        return;
    }

    *transport = coap_get_tcp_header_type_from_initbyte(
        ((unsigned char *)recvBuffer)[0] >> 4);
    *headerlen = coap_get_tcp_header_length_for_transport(*transport);
}
#endif
