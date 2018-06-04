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

#include "tcp_data_receiver_windows.h"

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

#define TAG "TCPRCVRWIN"

#define TCP_CHECKFD(FD)

/**
 * Push an exiting socket event to listen on
 *
 * @param[in] s              Socket to push
 * @param[in] socketArray    Array in which to add socket
 * @param[in] event          Event to push
 * @param[in] eventArray     Array in which to add event
 * @param[in/out] eventIndex Current length of arrays
 * @param[in] arraySize      Maximum length of arrays
 * @return true on success, false on failure
 */
static bool CAPushEvent(CASocketFd_t s, CASocketFd_t* socketArray,
                        HANDLE event, HANDLE* eventArray, int* eventIndex, int arraySize)
{
    if (*eventIndex == arraySize)
    {
        return false;
    }

    assert(*eventIndex >= 0);
    socketArray[*eventIndex] = s;
    eventArray[(*eventIndex)++] = event;
    return true;
}

/**
 * Push a new socket event to listen on
 *
 * @param[in] s              Socket to push
 * @param[in] socketArray    Array in which to add socket
 * @param[in] eventArray     Array in which to add event
 * @param[in/out] eventIndex Current length of arrays
 * @param[in] arraySize      Maximum length of arrays
 * @return true on success, false on failure
 */
static bool CAPushSocket(CASocketFd_t s, CASocketFd_t* socketArray,
                         HANDLE *eventArray, int *eventIndex, int arraySize)
{
    if (s == OC_INVALID_SOCKET)
    {
        // Nothing to push.
        return true;
    }

    WSAEVENT newEvent = WSACreateEvent();
    if (WSA_INVALID_EVENT == newEvent)
    {
        OIC_LOG_V(ERROR, TAG, "WSACreateEvent(NewEvent) failed %u", WSAGetLastError());
        return false;
    }

    if (0 != WSAEventSelect(s, newEvent, FD_READ | FD_ACCEPT))
    {
        OIC_LOG_V(ERROR, TAG, "WSAEventSelect failed %u", WSAGetLastError());
        OC_VERIFY(WSACloseEvent(newEvent));
        return false;
    }

    if (!CAPushEvent(s, socketArray, newEvent, eventArray, eventIndex, arraySize))
    {
        OIC_LOG_V(ERROR, TAG, "CAPushEvent failed");
        OC_VERIFY(WSACloseEvent(newEvent));
        return false;
    }

    return true;
}

#define EVENT_ARRAY_SIZE 64

/**
 * Process any message that is ready
 */
void udp_handle_inbound_data()  // @was CAFindReadyMessage
{
    CASocketFd_t socketArray[EVENT_ARRAY_SIZE] = {0};
    HANDLE eventArray[_countof(socketArray)];
    int arraySize = 0;

    if (OC_INVALID_SOCKET != caglobals.tcp.ipv4.fd)
    {
        CAPushSocket(caglobals.tcp.ipv4.fd, socketArray, eventArray, &arraySize, _countof(socketArray));
    }
    if (OC_INVALID_SOCKET != caglobals.tcp.ipv6.fd)
    {
        CAPushSocket(caglobals.tcp.ipv6.fd, socketArray, eventArray, &arraySize, _countof(socketArray));
    }
    if (WSA_INVALID_EVENT != caglobals.tcp.updateEvent)
    {
        CAPushEvent(OC_INVALID_SOCKET, socketArray,
                    caglobals.tcp.updateEvent, eventArray, &arraySize, _countof(socketArray));
    }

    int svrlistBeginIndex = arraySize;

    while (!caglobals.tcp.terminate)
    {
        CATCPSessionInfo_t *session = NULL;
        LL_FOREACH(g_sessionList, session)
        {
            if (session && OC_INVALID_SOCKET != session->fd && (arraySize < EVENT_ARRAY_SIZE))
            {
                 CAPushSocket(session->fd, socketArray, eventArray, &arraySize, _countof(socketArray));
            }
        }

        // Should not have overflowed buffer
        assert(arraySize <= (_countof(socketArray)));

        DWORD ret = WSAWaitForMultipleEvents(arraySize, eventArray, FALSE, WSA_INFINITE, FALSE);
        assert(ret < (WSA_WAIT_EVENT_0 + arraySize));
        DWORD eventIndex = ret - WSA_WAIT_EVENT_0;

        if (caglobals.tcp.updateEvent == eventArray[eventIndex])
        {
            OC_VERIFY(WSAResetEvent(caglobals.tcp.updateEvent));
        }
        else
        {
            // WSAEnumNetworkEvents also resets the event
            WSANETWORKEVENTS networkEvents;
            int enumResult = WSAEnumNetworkEvents(socketArray[eventIndex], eventArray[eventIndex], &networkEvents);
            if (SOCKET_ERROR != enumResult)
            {
                CASocketEventReturned(socketArray[eventIndex], networkEvents.lNetworkEvents);
            }
            else
            {
                OIC_LOG_V(ERROR, TAG, "WSAEnumNetworkEvents failed %u", WSAGetLastError());
                break;
            }
        }

        // Close events associated with svrlist
        while (arraySize > svrlistBeginIndex)
        {
            arraySize--;
            OC_VERIFY(WSACloseEvent(eventArray[arraySize]));
            eventArray[arraySize] = NULL;
        }
    }

    // Close events
    while (arraySize > 0)
    {
        arraySize--;
        OC_VERIFY(WSACloseEvent(eventArray[arraySize]));
    }

    if (caglobals.tcp.terminate)
    {
        caglobals.tcp.updateEvent = WSA_INVALID_EVENT;
    }
}

/**
 * Process an event (accept or receive) that is ready on a socket
 *
 * @param[in] s Socket to process
 */
static void CASocketEventReturned(CASocketFd_t s, long networkEvents)
{
    if (caglobals.tcp.terminate)
    {
        return;
    }

    assert(s != OC_INVALID_SOCKET);

    if (FD_ACCEPT & networkEvents)
    {
        if ((caglobals.tcp.ipv4.fd != OC_INVALID_SOCKET) && (caglobals.tcp.ipv4.fd == s))
        {
            CAAcceptConnection(CA_IPV4, &caglobals.tcp.ipv4);
        }
        else if ((caglobals.tcp.ipv6.fd != OC_INVALID_SOCKET) && (caglobals.tcp.ipv6.fd == s))
        {
            CAAcceptConnection(CA_IPV6, &caglobals.tcp.ipv6);
        }
    }

    if (FD_READ & networkEvents)
    {
        oc_mutex_lock(g_mutexObjectList);
        CATCPSessionInfo_t *session = NULL;
        CATCPSessionInfo_t *tmp = NULL;
        LL_FOREACH_SAFE(g_sessionList, session, tmp)
        {
            if (session && (session->fd == s))
            {
                CAResult_t res = CAReceiveMessage(session);
                //disconnect session and clean-up data if any error occurs
                if (res != CA_STATUS_OK)
                {
#ifdef __WITH_TLS__
                    if (CA_STATUS_OK != CAcloseSslConnection(&session->sep.endpoint))
                    {
                        OIC_LOG(ERROR, TAG, "Failed to close TLS session");
                    }
#endif
                    LL_DELETE(g_sessionList, session);
                    CADisconnectTCPSession(session);
                    oc_mutex_unlock(g_mutexObjectList);
                    return;
                }
            }
        }
        oc_mutex_unlock(g_mutexObjectList);
    }
}

static void CAWakeUpForReadFdsUpdate()
{
    if (WSA_INVALID_EVENT != caglobals.tcp.updateEvent)
    {
        if (!WSASetEvent(caglobals.tcp.updateEvent))
        {
            OIC_LOG_V(DEBUG, TAG, "CAWakeUpForReadFdsUpdate: set shutdown event failed: %u", GetLastError());
        }
    }
}

