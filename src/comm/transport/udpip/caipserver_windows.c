/* ****************************************************************
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

/* #ifndef __APPLE_USE_RFC_3542
 * #define __APPLE_USE_RFC_3542 // for PKTINFO
 * #endif */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for in6_pktinfo
#endif

#include "iotivity_config.h"
#include "iotivity_debug.h"

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
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
#ifdef __linux__
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

/* #include <coap/pdu.h> */
#include <inttypes.h>

/* #define USE_IP_MREQN
 * #if defined(_WIN32)
 * #undef USE_IP_MREQN
 * #endif */

/*
 * Logging tag for module name
 */
#define TAG "OIC_CA_IP_SERVER"

#define IFF_UP_RUNNING_FLAGS  (IFF_UP)

char* caips_get_error(){
    static char buffer[32];
    snprintf(buffer, 32, "%i", WSAGetLastError());
    return buffer;
}
#define CAIPS_GET_ERROR \
    caips_get_error()

#define PUSH_HANDLE(HANDLE, ARRAY, INDEX) \
{ \
    ARRAY[INDEX] = HANDLE; \
    INDEX++; \
}

// Create WSAEvent for SOCKET and push the new event into ARRAY
#define PUSH_SOCKET(SOCKET, ARRAY, INDEX) \
    if (SOCKET != OC_INVALID_SOCKET) \
    { \
        WSAEVENT NewEvent = WSACreateEvent(); \
        if (WSA_INVALID_EVENT != NewEvent) \
        { \
            if (0 != WSAEventSelect(SOCKET, NewEvent, FD_READ)) \
            { \
                OIC_LOG_V(ERROR, TAG, "WSAEventSelect failed %d", WSAGetLastError()); \
                BOOL closed = WSACloseEvent(NewEvent); \
                assert(closed); \
                if (!closed) \
                { \
                    OIC_LOG_V(ERROR, TAG, "WSACloseEvent(NewEvent) failed %d", WSAGetLastError()); \
                } \
            } \
            else \
            { \
                PUSH_HANDLE(NewEvent, ARRAY, INDEX); \
            } \
        } \
        else \
        { \
            OIC_LOG_V(ERROR, TAG, "WSACreateEvent failed %d", WSAGetLastError()); \
        }\
    }

#define INSERT_SOCKET(FD, ARRAY, INDEX) \
    { \
        if (OC_INVALID_SOCKET != FD) \
        { \
            ARRAY[INDEX] = FD; \
        } \
    }


// Inserts the socket into the SOCKET_ARRAY and pushes the socket event into EVENT_ARRAY
#define PUSH_IP_SOCKET(TYPE, EVENT_ARRAY, SOCKET_ARRAY, INDEX) \
    { \
        if (OC_INVALID_SOCKET != caglobals.ip.TYPE.fd) \
        { \
            INSERT_SOCKET(caglobals.ip.TYPE.fd, SOCKET_ARRAY, INDEX); \
            PUSH_SOCKET(caglobals.ip.TYPE.fd, EVENT_ARRAY, INDEX); \
        } \
    }

#define IS_MATCHING_IP_SOCKET(TYPE, SOCKET, FLAGS) \
    if ((caglobals.ip.TYPE.fd != OC_INVALID_SOCKET) && (caglobals.ip.TYPE.fd == SOCKET)) \
    { \
        fd = caglobals.ip.TYPE.fd; \
        flags = FLAGS; \
    }

#define EVENT_ARRAY_SIZE  10

static void CAFindReadyMessage()
{
    CASocketFd_t socketArray[EVENT_ARRAY_SIZE];
    HANDLE eventArray[EVENT_ARRAY_SIZE];
    DWORD arraySize = 0;
    DWORD eventIndex;

    // socketArray and eventArray should have same number of elements
    OC_STATIC_ASSERT(_countof(socketArray) == _countof(eventArray), "Arrays should have same number of elements");
    OC_STATIC_ASSERT(_countof(eventArray) <= WSA_MAXIMUM_WAIT_EVENTS, "Too many events for a single Wait");

    PUSH_IP_SOCKET(u6,  eventArray, socketArray, arraySize);
    PUSH_IP_SOCKET(u6s, eventArray, socketArray, arraySize);
    PUSH_IP_SOCKET(u4,  eventArray, socketArray, arraySize);
    PUSH_IP_SOCKET(u4s, eventArray, socketArray, arraySize);
    PUSH_IP_SOCKET(m6,  eventArray, socketArray, arraySize);
    PUSH_IP_SOCKET(m6s, eventArray, socketArray, arraySize);
    PUSH_IP_SOCKET(m4,  eventArray, socketArray, arraySize);
    PUSH_IP_SOCKET(m4s, eventArray, socketArray, arraySize);

    if (WSA_INVALID_EVENT != caglobals.ip.shutdownEvent)
    {
        INSERT_SOCKET(OC_INVALID_SOCKET, socketArray, arraySize);
        PUSH_HANDLE(caglobals.ip.shutdownEvent, eventArray, arraySize);
    }

    if (WSA_INVALID_EVENT != caglobals.ip.addressChangeEvent)
    {
        INSERT_SOCKET(OC_INVALID_SOCKET, socketArray, arraySize);
        PUSH_HANDLE(caglobals.ip.addressChangeEvent, eventArray, arraySize);
    }

    // Should not have overflowed buffer
    assert(arraySize <= (_countof(socketArray)));

    // Timeout is unnecessary on Windows
    assert(-1 == caglobals.ip.selectTimeout);

    while (!caglobals.ip.terminate)
    {
        DWORD ret = WSAWaitForMultipleEvents(arraySize, eventArray, FALSE, WSA_INFINITE, FALSE);
        assert(ret >= WSA_WAIT_EVENT_0);
        assert(ret < (WSA_WAIT_EVENT_0 + arraySize));

        switch (ret)
        {
            case WSA_WAIT_FAILED:
                OIC_LOG_V(ERROR, TAG, "WSAWaitForMultipleEvents returned WSA_WAIT_FAILED %d", WSAGetLastError());
                break;
            case WSA_WAIT_IO_COMPLETION:
                OIC_LOG_V(ERROR, TAG, "WSAWaitForMultipleEvents returned WSA_WAIT_IO_COMPLETION %d", WSAGetLastError());
                break;
            case WSA_WAIT_TIMEOUT:
                OIC_LOG_V(ERROR, TAG, "WSAWaitForMultipleEvents returned WSA_WAIT_TIMEOUT %d", WSAGetLastError());
                break;
            default:
                eventIndex = ret - WSA_WAIT_EVENT_0;
                if ((eventIndex >= 0) && (eventIndex < arraySize))
                {
                    if (false == WSAResetEvent(eventArray[eventIndex]))
                    {
                        OIC_LOG_V(ERROR, TAG, "WSAResetEvent failed %d", WSAGetLastError());
                    }

                    // Handle address changes if addressChangeEvent is triggered.
                    if ((caglobals.ip.addressChangeEvent != WSA_INVALID_EVENT) &&
                        (caglobals.ip.addressChangeEvent == eventArray[eventIndex]))
                    {
                        u_arraylist_t *iflist = CAFindInterfaceChange();
                        if (iflist)
                        {
                            size_t listLength = u_arraylist_length(iflist);
                            for (size_t i = 0; i < listLength; i++)
                            {
                                CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
                                if (ifitem)
                                {
                                    CAProcessNewInterface(ifitem);
                                }
                            }
                            u_arraylist_destroy(iflist);
                        }
                        break;
                    }

                    // Break out if shutdownEvent is triggered.
                    if ((caglobals.ip.shutdownEvent != WSA_INVALID_EVENT) &&
                        (caglobals.ip.shutdownEvent == eventArray[eventIndex]))
                    {
                        break;
                    }
                    CAEventReturned(socketArray[eventIndex]);
                }
                else
                {
                    OIC_LOG_V(ERROR, TAG, "WSAWaitForMultipleEvents failed %d", WSAGetLastError());
                }
                break;
        }

    }

    for (size_t i = 0; i < arraySize; i++)
    {
        HANDLE h = eventArray[i];
        if (h != caglobals.ip.addressChangeEvent)
        {
            BOOL closed = WSACloseEvent(h);
            assert(closed);
            if (!closed)
            {
                OIC_LOG_V(ERROR, TAG, "WSACloseEvent (Index %i) failed %d", i, WSAGetLastError());
            }
        }
    }

    if (caglobals.ip.terminate)
    {
        caglobals.ip.shutdownEvent = WSA_INVALID_EVENT;
    }
}

static void CAEventReturned(CASocketFd_t socket)
{
    CASocketFd_t fd = OC_INVALID_SOCKET;
    CATransportFlags_t flags = CA_DEFAULT_FLAGS;

    while (!caglobals.ip.terminate)
    {
        IS_MATCHING_IP_SOCKET(u6,  socket, CA_IPV6)
        else IS_MATCHING_IP_SOCKET(u6s, socket, CA_IPV6 | CA_SECURE)
        else IS_MATCHING_IP_SOCKET(u4,  socket, CA_IPV4)
        else IS_MATCHING_IP_SOCKET(u4s, socket, CA_IPV4 | CA_SECURE)
        else IS_MATCHING_IP_SOCKET(m6,  socket, CA_MULTICAST | CA_IPV6)
        else IS_MATCHING_IP_SOCKET(m6s, socket, CA_MULTICAST | CA_IPV6 | CA_SECURE)
        else IS_MATCHING_IP_SOCKET(m4,  socket, CA_MULTICAST | CA_IPV4)
        else IS_MATCHING_IP_SOCKET(m4s, socket, CA_MULTICAST | CA_IPV4 | CA_SECURE)
        else
        {
            break;
        }
        (void)CAReceiveMessage(socket, flags);
        // We will never get more than one match per socket, so always break.
        break;
    }
}

void CADeInitializeMonitorGlobals()
{
    if (caglobals.ip.addressChangeEvent != WSA_INVALID_EVENT)
	{
	    OC_VERIFY(WSACloseEvent(caglobals.ip.addressChangeEvent));
	    caglobals.ip.addressChangeEvent = WSA_INVALID_EVENT;
	}
}

static CAResult_t CAReceiveMessage(CASocketFd_t fd, CATransportFlags_t flags)
{
    char recvBuffer[RECV_MSG_BUF_LEN] = {0};
    int level = 0;
    int type = 0;
    int namelen = 0;
    struct sockaddr_storage srcAddr = { .ss_family = 0 };
    unsigned char *pktinfo = NULL;
    union control
    {
        WSACMSGHDR cmsg;
        uint8_t data[WSA_CMSG_SPACE(sizeof (IN6_PKTINFO))];
    } cmsg;
    memset(&cmsg, 0, sizeof(cmsg));

    if (flags & CA_IPV6)
    {
        namelen  = sizeof (struct sockaddr_in6);
        level = IPPROTO_IPV6;
        type = IPV6_PKTINFO;
    }
    else
    {
        namelen = sizeof (struct sockaddr_in);
        level = IPPROTO_IP;
        type = IP_PKTINFO;
    }

    WSABUF iov = {.len = sizeof (recvBuffer), .buf = recvBuffer};
    WSAMSG msg = {.name = (PSOCKADDR)&srcAddr,
                  .namelen = namelen,
                  .lpBuffers = &iov,
                  .dwBufferCount = 1,
                  .Control = {.buf = (char*)cmsg.data, .len = sizeof (cmsg)}
                 };

    uint32_t recvLen = 0;
    uint32_t ret = caglobals.ip.wsaRecvMsg(fd, &msg, (LPDWORD)&recvLen, 0,0);
    if (OC_SOCKET_ERROR == ret)
    {
        OIC_LOG_V(ERROR, TAG, "WSARecvMsg failed %i", WSAGetLastError());
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "WSARecvMsg recvd %u bytes", recvLen);

    for (WSACMSGHDR *cmp = WSA_CMSG_FIRSTHDR(&msg); cmp != NULL;
         cmp = WSA_CMSG_NXTHDR(&msg, cmp))
    {
        if (cmp->cmsg_level == level && cmp->cmsg_type == type)
        {
            pktinfo = WSA_CMSG_DATA(cmp);
        }
    }
/* #endif // !defined(WSA_CMSG_DATA) */
    if (!pktinfo)
    {
        OIC_LOG(ERROR, TAG, "pktinfo is null");
        return CA_STATUS_FAILED;
    }

    CASecureEndpoint_t sep = {.endpoint = {.adapter = CA_ADAPTER_IP, .flags = flags}};

    if (flags & CA_IPV6)
    {
        sep.endpoint.ifindex = ((struct in6_pktinfo *)pktinfo)->ipi6_ifindex;

        if (flags & CA_MULTICAST)
        {
            struct in6_addr *addr = &(((struct in6_pktinfo *)pktinfo)->ipi6_addr);
            unsigned char topbits = ((unsigned char *)addr)[0];
            if (topbits != 0xff)
            {
                sep.endpoint.flags &= ~CA_MULTICAST;
            }
        }
    }
    else
    {
        sep.endpoint.ifindex = ((struct in_pktinfo *)pktinfo)->ipi_ifindex;

        if (flags & CA_MULTICAST)
        {
            struct in_addr *addr = &((struct in_pktinfo *)pktinfo)->ipi_addr;
            uint32_t host = ntohl(addr->s_addr);
            unsigned char topbits = ((unsigned char *)&host)[3];
            if (topbits < 224 || topbits > 239)
            {
                sep.endpoint.flags &= ~CA_MULTICAST;
            }
        }
    }

    CAConvertAddrToName(&srcAddr, namelen, sep.endpoint.addr, &sep.endpoint.port);

    if (flags & CA_SECURE)
    {
#ifdef __WITH_DTLS__
#ifdef TB_LOG
        int decryptResult =
#endif
        CAdecryptSsl(&sep, (uint8_t *)recvBuffer, recvLen);
        OIC_LOG_V(DEBUG, TAG, "CAdecryptSsl returns [%d]", decryptResult);
#else
        OIC_LOG(ERROR, TAG, "Encrypted message but no DTLS");
#endif // __WITH_DTLS__
    }
    else
    {
        if (g_packetReceivedCallback)
        {
            g_packetReceivedCallback(&sep, recvBuffer, recvLen);
        }
    }

    return CA_STATUS_OK;
}

#if INTERFACE
#define CHECKFD(FD)
#endif

void CARegisterForAddressChanges()
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
/* #ifdef _WIN32 */
    caglobals.ip.addressChangeEvent = WSACreateEvent();
    if (WSA_INVALID_EVENT == caglobals.ip.addressChangeEvent)
    {
        OIC_LOG(ERROR, TAG, "WSACreateEvent failed");
    }
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}

static void CAInitializeFastShutdownMechanism()
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    caglobals.ip.selectTimeout = -1; // don't poll for shutdown
    int ret = -1;
    caglobals.ip.shutdownEvent = WSACreateEvent();
    if (WSA_INVALID_EVENT != caglobals.ip.shutdownEvent)
    {
        ret = 0;
    }
    if (-1 == ret)
    {
        OIC_LOG_V(ERROR, TAG, "fast shutdown mechanism init failed: %s", CAIPS_GET_ERROR);
        caglobals.ip.selectTimeout = SELECT_TIMEOUT; //poll needed for shutdown
    }
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}

void CAIPStopServer()
{
    caglobals.ip.terminate = true;

    // receive thread will stop immediately.
    if (!WSASetEvent(caglobals.ip.shutdownEvent))
    {
        OIC_LOG_V(DEBUG, TAG, "set shutdown event failed: %d", WSAGetLastError());
    }

    if (!caglobals.ip.started)
    { // Close fd's since receive handler was not started
        CACloseFDs();
    }
    caglobals.ip.started = false;
}

void CAWakeUpForChange()
{
    if (!WSASetEvent(caglobals.ip.shutdownEvent))
    {
        OIC_LOG_V(DEBUG, TAG, "set shutdown event failed: %d", WSAGetLastError());
    }
}

INLINE_API PORTABLE_check_setsockopt_err() { return WSAEINVAL != WSAGetLastError(); }

INLINE_API bool PORTABLE_check_setsockopt_m4s_err()
{
    if (WSAEINVAL == WSAGetLastError())
        {
            // We're trying to join the multicast group again.
            // If the interface has gone down and come back up, the socket might be in
            // an inconsistent state where it still thinks we're joined when the interface
            // doesn't think we're joined.  So try to leave and rejoin the group just in case.
            setsockopt(caglobals.ip.m4s.fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, OPTVAL_T(&mreq),
		       sizeof(mreq));
            ret = setsockopt(caglobals.ip.m4s.fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, OPTVAL_T(&mreq),
                             sizeof(mreq));
        }
    return (OC_SOCKET_ERROR == ret);
}

INLINE_API bool PORTABLE_check_setsockopt_m6_err()
{
        if (WSAEINVAL == WSAGetLastError())
        {
            // We're trying to join the multicast group again.
            // If the interface has gone down and come back up, the socket might be in
            // an inconsistent state where it still thinks we're joined when the interface
            // doesn't think we're joined.  So try to leave and rejoin the group just in case.
            setsockopt(fd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, OPTVAL_T(&mreq), sizeof(mreq));
            ret = setsockopt(fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, OPTVAL_T(&mreq), sizeof(mreq));
        }
	return (OC_SOCKET_ERROR == ret);
}

void PORTABLE_sendto(CASocketFd_t fd,
                     const void *data,
		     size_t dlen,
		     int flags,
		     struct sockaddr * sockaddrptr,
		     socklen_t socklen,
		     const CAEndpoint_t *endpoint,
		     const char *cast, const char *fam)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
#ifdef TB_LOG
    const char *secure = (endpoint->flags & CA_SECURE) ? "secure " : "insecure ";
#endif
    int err = 0;
    int len = 0;
    size_t sent = 0;
    do {
        int dataToSend = ((dlen - sent) > INT_MAX) ? INT_MAX : (int)(dlen - sent);
        len = sendto(fd, ((char*)data) + sent, dataToSend, 0, sockaddrptr, socklen);
        if (OC_SOCKET_ERROR == len)
        {
            err = WSAGetLastError();
            if ((WSAEWOULDBLOCK != err) && (WSAENOBUFS != err))
            {
                 // If logging is not defined/enabled.
                if (g_ipErrorHandler)
                {
                    g_ipErrorHandler(endpoint, data, dlen, CA_SEND_FAILED);
                }

                OIC_LOG_V(ERROR, TAG, "%s%s %s sendTo failed: %i", secure, cast, fam, err);
            }
        }
        else
        {
            sent += len;
            if (sent != (size_t)len)
            {
                OIC_LOG_V(DEBUG, TAG, "%s%s %s sendTo (Partial Send) is successful: "
                                      "currently sent: %ld bytes, "
                                      "total sent: %" PRIuPTR " bytes, "
                                      "remaining: %" PRIuPTR " bytes",
                                      secure, cast, fam, len, sent, (dlen - sent));
            }
            else
            {
                OIC_LOG_V(INFO, TAG, "%s%s %s sendTo is successful: %ld bytes",
                                     secure, cast, fam, len);
            }
        }
    } while ((OC_SOCKET_ERROR == len) && ((WSAEWOULDBLOCK == err) || (WSAENOBUFS == err)) || (sent < dlen));
}

void CAIPSetErrorHandler(CAIPErrorHandleCallback errorHandleCallback)
{
    g_ipErrorHandler = errorHandleCallback;
}

CAResult_t CAGetLinkLocalZoneIdInternal(uint32_t ifindex, char **zoneId)
{
    if (!zoneId || (*zoneId != NULL))
    {
        return CA_STATUS_INVALID_PARAM;
    }

    PIP_ADAPTER_ADDRESSES pAdapters = GetAdapters();

    if (!pAdapters)
    {
        OICFree(pAdapters);
        return CA_STATUS_FAILED;
    }

    PIP_ADAPTER_ADDRESSES pCurrAdapter = NULL;
    pCurrAdapter = pAdapters;

    while (pCurrAdapter)
    {
        if (ifindex == pCurrAdapter->IfIndex)
        {
            OIC_LOG_V(DEBUG, TAG, "Given ifindex is %d parsed zoneId is %d",
                      ifindex, pCurrAdapter->ZoneIndices[ScopeLevelLink]);
            *zoneId = (char *)OICCalloc(IF_NAMESIZE, sizeof(char));
            _ultoa(pCurrAdapter->ZoneIndices[ScopeLevelLink], *zoneId, 10);
            break;
        }
        pCurrAdapter = pCurrAdapter->Next;
    }

    OICFree(pAdapters);

    if (!*zoneId)
    {
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}
