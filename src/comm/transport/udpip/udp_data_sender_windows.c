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

#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542 // for PKTINFO
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for in6_pktinfo
#endif

#include "udp_data_sender_windows.h"

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if EXPORT_INTERFACE
#include <winsock2.h>
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
/* #ifdef __linux__ */
/* #include <linux/netlink.h> */
/* #include <linux/rtnetlink.h> */
/* #endif */

/* #include <coap/pdu.h> */

#if EXPORT_INTERFACE
#include <inttypes.h>
#include <Ws2tcpip.h>
#endif

/* #define USE_IP_MREQN
 * #if defined(_WIN32)
 * #undef USE_IP_MREQN
 * #endif */

void PORTABLE_sendto(CASocketFd_t fd,
                     const void *data,
		     size_t dlen,
		     int flags,
		     //struct sockaddr_storage * sockaddrptr,
		     struct sockaddr * sockaddrptr,
		     socklen_t socklen,
		     const CAEndpoint_t *endpoint)
		     /* const char *cast, const char *fam) */
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
        len = sendto(fd, ((char*)data) + sent, dataToSend, 0, (struct sockaddr *)sockaddrptr, socklen);
        if (OC_SOCKET_ERROR == len)
        {
            err = WSAGetLastError();
            if ((WSAEWOULDBLOCK != err) && (WSAENOBUFS != err))
            {
                 // If logging is not defined/enabled.
		// @rewrite: g_ipErrorHandler removed, call CAIPErrorHandler directly
		/* if (g_ipErrorHandler) */
		/* { */
		/*     g_ipErrorHandler(endpoint, data, dlen, CA_SEND_FAILED); */
		/* } */
		// FIXME: short-circuit to CAAdapterErrorHandleCallback
		CAErrorHandler(endpoint, data, dlen, (CAResult_t)CA_STATUS_INVALID_PARAM);
#ifdef TB_LOG
                OIC_LOG_V(ERROR, TAG, "%s%s %s sendTo failed: %i", secure, cast, fam, err);
#endif
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
