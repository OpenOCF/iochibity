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

#include "tcp_data_sender.h"

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

#include <limits.h>
#include <string.h>		/* strerror */

#define TAG "TCPSENDER"

/**
 * Queue handle for Send Data.
 */
CAQueueingThread_t *tcp_sendQueueHandle = NULL;

#ifdef __WITH_TLS__
ssize_t CATCPPacketSendCB(CAEndpoint_t *endpoint, const void *data, size_t dataLength)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint is NULL", -1);
    VERIFY_NON_NULL_RET(data, TAG, "data is NULL", -1);

    OIC_LOG_V(DEBUG, TAG, "Address: %s, port:%d", endpoint->addr, endpoint->port);
    OIC_LOG_BUFFER(DEBUG, TAG, data, dataLength);

    ssize_t ret = CATCPSendData(endpoint, data, dataLength);

    OIC_LOG_V(DEBUG, TAG, "Out %s : %" PRIdPTR " bytes sent", __func__, ret);
    return ret;
}
#endif

// @rewrite tcp_send_data @was sendData
static ssize_t tcp_send_data(const CAEndpoint_t *endpoint,
			const void *data,
                        size_t dlen, const char *fam)
{
    OIC_LOG_V(INFO, TAG, "The length of data that needs to be sent is %" PRIuPTR " bytes", dlen);

    // #1. find a session info from list.
    CASocketFd_t sockFd = CAGetSocketFDFromEndpoint(endpoint);
    if (OC_INVALID_SOCKET == sockFd)
    {
        // if there is no connection info, connect to remote device.
        sockFd = CAConnectTCPSession(endpoint);
        if (OC_INVALID_SOCKET == sockFd)
        {
            OIC_LOG(ERROR, TAG, "Failed to create tcp session object");
            return -1;
        }
    }

    // #2. send data to remote device.
    ssize_t remainLen = dlen;
    do
    {
        int dataToSend = (remainLen > INT_MAX) ? INT_MAX : (int)remainLen;
        ssize_t len = send(sockFd, data, dataToSend, 0);
        if (-1 == len)
        {
            if (EWOULDBLOCK != errno)
            {
                OIC_LOG_V(ERROR, TAG, "unicast ipv4tcp sendTo failed: %s", strerror(errno));
                CALogSendStateInfo(endpoint->adapter, endpoint->addr, endpoint->port,
                                   len, false, strerror(errno));
                return len;
            }
            continue;
        }
        data = ((char*)data) + len;
        remainLen -= len;
    } while (remainLen > 0);

#ifndef TB_LOG
    (void)fam;
#endif
    OIC_LOG_V(INFO, TAG, "unicast %stcp sendTo is successful: %" PRIuPTR " bytes", fam, dlen);
    CALogSendStateInfo(endpoint->adapter, endpoint->addr, endpoint->port,
                       dlen, true, NULL);
    return dlen;
}

ssize_t CATCPSendData(CAEndpoint_t *endpoint, const void *data, size_t datalen)
{
    OIC_LOG_V(DEBUG, TAG, "%s", __func__);
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint is NULL", -1);
    VERIFY_NON_NULL_RET(data, TAG, "data is NULL", -1);

    if (caglobals.tcp.ipv6tcpenabled && (endpoint->flags & CA_IPV6))
    {
        return tcp_send_data(endpoint, data, datalen, "ipv6"); /* @was sendData */
    }
    if (caglobals.tcp.ipv4tcpenabled && (endpoint->flags & CA_IPV4))
    {
        return tcp_send_data(endpoint, data, datalen, "ipv4"); /* @was sendData */
    }

    OIC_LOG(ERROR, TAG, "Not supported transport flags");
    return -1;
}

CATCPData *CACreateTCPData(const CAEndpoint_t *remoteEndpoint, const void *data,
                           size_t dataLength, bool isMulticast, bool encryptedData)
{
    VERIFY_NON_NULL_RET(remoteEndpoint, TAG, "remoteEndpoint is NULL", NULL);
    VERIFY_NON_NULL_RET(data, TAG, "data is NULL", NULL);

    CATCPData *tcpData = (CATCPData *) OICCalloc(1, sizeof(*tcpData));
    if (!tcpData)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return NULL;
    }

    tcpData->remoteEndpoint = CACloneEndpoint(remoteEndpoint);
    tcpData->data = (void *) OICMalloc(dataLength);
    if (!tcpData->data)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        CAFreeTCPData(tcpData);
        return NULL;
    }

    memcpy(tcpData->data, data, dataLength);
    tcpData->dataLen = dataLength;

    tcpData->isMulticast = isMulticast;
    tcpData->encryptedData = encryptedData;

    return tcpData;
}

void CAFreeTCPData(CATCPData *tcpData)
{
    VERIFY_NON_NULL_VOID(tcpData, TAG, "tcpData is NULL");

    CAFreeEndpoint(tcpData->remoteEndpoint);
    OICFree(tcpData->data);
    OICFree(tcpData);
}

void CATCPDataDestroyer(void *data, uint32_t size)
{
    if (size < sizeof(CATCPData))
    {
        OIC_LOG_V(ERROR, TAG, "Destroy data too small %p %" PRIu32, data, size);
    }
    CATCPData *TCPData = (CATCPData *) data;

    CAFreeTCPData(TCPData);
}

CAResult_t CATCPInitializeQueueHandles()
{
    // Check if the message queue is already initialized
    if (tcp_sendQueueHandle)
    {
        OIC_LOG(DEBUG, TAG, "send queue handle is already initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    tcp_sendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!tcp_sendQueueHandle)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

#ifdef DEBUG_THREADS
    tcp_sendQueueHandle.name = "tcp_sendQueueHandle";
#endif
    if (CA_STATUS_OK != CAQueueingThreadInitialize(tcp_sendQueueHandle,
                                (const ca_thread_pool_t)caglobals.tcp.threadpool,
                                CATCPSendDataThread, CATCPDataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        OICFree(tcp_sendQueueHandle);
        tcp_sendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

void CATCPDeinitializeQueueHandles()
{
    CAQueueingThreadDestroy(tcp_sendQueueHandle);
    OICFree(tcp_sendQueueHandle);
    tcp_sendQueueHandle = NULL;
}

static int32_t CAQueueTCPData(bool isMulticast, const CAEndpoint_t *endpoint,
                             const void *data, size_t dataLength, bool encryptedData)
{
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint", -1);
    VERIFY_NON_NULL_RET(data, TAG, "data", -1);
    VERIFY_TRUE_RET((dataLength <= INT_MAX) && (dataLength > 0),
                    TAG,
                    "Invalid Data Length",
                    -1);
    VERIFY_NON_NULL_RET(tcp_sendQueueHandle, TAG, "sendQueueHandle", -1);

    // Create TCPData to add to queue
    CATCPData *tcpData = CACreateTCPData(endpoint, data, dataLength, isMulticast, encryptedData);
    if (!tcpData)
    {
        OIC_LOG(ERROR, TAG, "Failed to create ipData!");
        return -1;
    }
    // Add message to send queue
    CAQueueingThreadAddData(tcp_sendQueueHandle, tcpData, sizeof(CATCPData));

    return (int32_t)dataLength;
}

int32_t CASendTCPUnicastData(const CAEndpoint_t *endpoint,
                             const void *data, uint32_t dataLength,
                             CADataType_t dataType)
{
    OIC_LOG(DEBUG, TAG, "IN");
    (void)dataType;
    if ((0 == dataLength) || (dataLength > INT32_MAX))
    {
        OIC_LOG(ERROR, TAG, "Invalid Data Length");
        return -1;
    }

#ifndef SINGLE_THREAD
    return CAQueueTCPData(false, endpoint, data, dataLength, false);
#else
    return (int32_t)CATCPSendData(endpoint, data, dataLength);
#endif
}

int32_t CASendTCPMulticastData(const CAEndpoint_t *endpoint,
                               const void *data, uint32_t dataLength,
                               CADataType_t dataType)
{
    (void)dataType;
    return CAQueueTCPData(true, endpoint, data, dataLength, false);
}

void CATCPSendDataThread(void *threadData)
{
    CATCPData *tcpData = (CATCPData *) threadData;
    if (!tcpData)
    {
        OIC_LOG(DEBUG, TAG, "Invalid TCP data!");
        return;
    }

    if (caglobals.tcp.terminate)
    {
        OIC_LOG(DEBUG, TAG, "Adapter is not enabled");
        CATCPErrorHandler(tcpData->remoteEndpoint, tcpData->data, tcpData->dataLen,
                          CA_SEND_FAILED);
        return;
    }

    if (tcpData->isMulticast)
    {
        //Processing for sending multicast
        OIC_LOG(DEBUG, TAG, "Send Multicast Data is called, not supported");
        return;
    }
    else
    {
        if (!tcpData->encryptedData)
        {
            // Check payload length from CoAP over TCP format header.
            size_t payloadLen = CACheckPayloadLengthFromHeader(tcpData->data, tcpData->dataLen);
            if (!payloadLen)
            {
                // if payload length is zero, disconnect from remote device.
                OIC_LOG(DEBUG, TAG, "payload length is zero, disconnect from remote device");
#ifdef __WITH_TLS__
                if (CA_STATUS_OK != CAcloseSslConnection(tcpData->remoteEndpoint))
                {
                    OIC_LOG(ERROR, TAG, "Failed to close TLS session");
                }
#endif
                CASearchAndDeleteTCPSession(tcpData->remoteEndpoint);
                return;
            }

#ifdef __WITH_TLS__
            CAResult_t result = CA_STATUS_OK;
            if (tcpData->remoteEndpoint && tcpData->remoteEndpoint->flags & CA_SECURE)
            {
                OIC_LOG(DEBUG, TAG, "CAencryptSsl called!");
                result = CAencryptSsl(tcpData->remoteEndpoint, tcpData->data, tcpData->dataLen);

                if (CA_STATUS_OK != result)
                {
                    OIC_LOG(ERROR, TAG, "CAAdapterNetDtlsEncrypt failed!");
                    CASearchAndDeleteTCPSession(tcpData->remoteEndpoint);
                    CATCPErrorHandler(tcpData->remoteEndpoint, tcpData->data, tcpData->dataLen,
                                      CA_SEND_FAILED);
                }
                OIC_LOG_V(DEBUG, TAG,
                          "CAAdapterNetDtlsEncrypt returned with result[%d]", result);
               return;
            }
#endif
        }

        //Processing for sending unicast
        ssize_t dlen = CATCPSendData(tcpData->remoteEndpoint, tcpData->data, tcpData->dataLen);
        if (-1 == dlen)
        {
            OIC_LOG(ERROR, TAG, "CATCPSendData failed");
            CASearchAndDeleteTCPSession(tcpData->remoteEndpoint);
            CATCPErrorHandler(tcpData->remoteEndpoint, tcpData->data, tcpData->dataLen,
                              CA_SEND_FAILED);
        }
    }
}
