/** @file udp_data_receiver.c
 *
 */

#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542 // for PKTINFO
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for in6_pktinfo
#endif

#include "udp_data_receiver_windows.h"

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif

#if INTERFACE
#include <inttypes.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
// mingw:
#include <mswsock.h>
#endif

#include <errno.h>


LPFN_WSARECVMSG udp_wsaRecvMsg; /**< Win32 function pointer to WSARecvMsg() */

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

LOCAL void CAEventReturned(CASocketFd_t socket)
{
    CASocketFd_t fd = OC_INVALID_SOCKET;
    CATransportFlags_t flags = CA_DEFAULT_FLAGS;

    while (!udp_is_terminating)
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
        (void)udp_recvmsg_on_socket(socket, flags);
        // We will never get more than one match per socket, so always break.
        break;
    }
}

// FIXME: if there is no inbound data, this will cause hang upon termination?
// @rewrite udp_handle_inboud_data  @was CAFindReadyMessage()
/* called by udp_data_receiver_runloop */
void udp_handle_inbound_data() // @was CAFindReadyMessage
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

    if (WSA_INVALID_EVENT != udp_shutdownEvent)
    {
        INSERT_SOCKET(OC_INVALID_SOCKET, socketArray, arraySize);
        PUSH_HANDLE(udp_shutdownEvent, eventArray, arraySize);
    }

    if (WSA_INVALID_EVENT != udp_addressChangeEvent)
    {
        INSERT_SOCKET(OC_INVALID_SOCKET, socketArray, arraySize);
        PUSH_HANDLE(udp_addressChangeEvent, eventArray, arraySize);
    }

    // Should not have overflowed buffer
    assert(arraySize <= (_countof(socketArray)));

    // Timeout is unnecessary on Windows
    assert(-1 == udp_selectTimeout);

    while (!udp_is_terminating)
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
                    if ((udp_addressChangeEvent != WSA_INVALID_EVENT) &&
                        (udp_addressChangeEvent == eventArray[eventIndex]))
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
                                    udp_add_if_to_multicast_groups(ifitem); // @was CAProcessNewInterface
                                }
                            }
                            u_arraylist_destroy(iflist);
                        }
                        break;
                    }

                    // Break out if shutdownEvent is triggered.
                    if ((udp_shutdownEvent != WSA_INVALID_EVENT) &&
                        (udp_shutdownEvent == eventArray[eventIndex]))
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
        if (h != udp_addressChangeEvent)
        {
            BOOL closed = WSACloseEvent(h);
            assert(closed);
            if (!closed)
            {
                OIC_LOG_V(ERROR, TAG, "WSACloseEvent (Index %i) failed %d", i, WSAGetLastError());
            }
        }
    }

    if (udp_is_terminating)
    {
        udp_shutdownEvent = WSA_INVALID_EVENT;
    }
}

void CAIPStopServer()
{
    udp_is_terminating = true;

    // receive thread will stop immediately.
    if (!WSASetEvent(udp_shutdownEvent))
    {
        OIC_LOG_V(DEBUG, TAG, "set shutdown event failed: %d", WSAGetLastError());
    }

    if (!udp_is_started)
    { // Close fd's since receive handler was not started
	// CADeInitializeIPGlobals();
	udp_cleanup();  // @rewrite @was CACloseFDs();
    }
    udp_is_started = false;
}

void CAWakeUpForChange()
{
    if (!WSASetEvent(udp_shutdownEvent))
    {
        OIC_LOG_V(DEBUG, TAG, "set shutdown event failed: %d", WSAGetLastError());
    }
}

LOCAL CAResult_t udp_recvmsg_on_socket(CASocketFd_t fd, CATransportFlags_t flags)
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
        /* if (g_udpPacketRecdCB) */
        /* { */
        /*     g_udpPacketRecdCB(&sep, recvBuffer, recvLen); */
        /* } */
	mh_CAReceivedPacketCallback(&sep, recvBuffer, recvLen);
    }

    return CA_STATUS_OK;
}
