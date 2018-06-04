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

#include "tcp_data_receiver_posix.h"

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

#define TAG "TCPRCVRPOSIX"

void tcp_handle_inbound_data()  // @was CAFindReadyMessage
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    fd_set readFds;
    //struct timeval timeout = { .tv_sec = caglobals.tcp.selectTimeout };
    struct timeval timeout = { .tv_sec = tcp_selectTimeout };

    FD_ZERO(&readFds);
    //CA_FD_SET(ipv4, &readFds);
    if (tcp_socket_ipv4.fd != OC_INVALID_SOCKET) FD_SET(tcp_socket_ipv4.fd, &readFds);

    //CA_FD_SET(ipv4s, &readFds);
    if (tcp_socket_ipv4s.fd != OC_INVALID_SOCKET) FD_SET(tcp_socket_ipv4s.fd, &readFds);

    //CA_FD_SET(ipv6, &readFds);
    if (tcp_socket_ipv6.fd != OC_INVALID_SOCKET) FD_SET(tcp_socket_ipv6.fd, &readFds);

    // CA_FD_SET(ipv6s, &readFds);
    if (tcp_socket_ipv6s.fd != OC_INVALID_SOCKET) FD_SET(tcp_socket_ipv6s.fd, &readFds);

    if (OC_INVALID_SOCKET != tcp_shutdownFds[0]) FD_SET(tcp_shutdownFds[0], &readFds);

    if (OC_INVALID_SOCKET != tcp_connectionFds[0]) FD_SET(tcp_connectionFds[0], &readFds);

    CATCPSessionInfo_t *session = NULL;
    LL_FOREACH(tcp_sessionList, session)
    {
        if (session && session->fd != OC_INVALID_SOCKET && session->state == CONNECTED)
        {
            FD_SET(session->fd, &readFds);
        }
    }

    int ret = select(caglobals.tcp.maxfd + 1, &readFds, NULL, NULL, &timeout);

    if (caglobals.tcp.terminate)
    {
        OIC_LOG_V(DEBUG, TAG, "Packet receiver Stop request received.");
        return;
    }

    if (0 == ret)
    {
        return;
    }
    else if (0 < ret)
    {
        CASelectReturned(&readFds);
    }
    else // if (0 > ret)
    {
        OIC_LOG_V(FATAL, TAG, "select error %s", strerror(errno));
        return;
    }
}

LOCAL void CASelectReturned(fd_set *readFds)
{
    VERIFY_NON_NULL_VOID(readFds, TAG, "readFds is NULL");

    if (caglobals.tcp.ipv4.fd != -1 && FD_ISSET(caglobals.tcp.ipv4.fd, readFds))
    {
        CAAcceptConnection(CA_IPV4, &caglobals.tcp.ipv4);
        return;
    }
    else if (caglobals.tcp.ipv4s.fd != -1 && FD_ISSET(caglobals.tcp.ipv4s.fd, readFds))
    {
        CAAcceptConnection(CA_IPV4 | CA_SECURE, &caglobals.tcp.ipv4s);
        return;
    }
    else if (caglobals.tcp.ipv6.fd != -1 && FD_ISSET(caglobals.tcp.ipv6.fd, readFds))
    {
        CAAcceptConnection(CA_IPV6, &caglobals.tcp.ipv6);
        return;
    }
    else if (caglobals.tcp.ipv6s.fd != -1 && FD_ISSET(caglobals.tcp.ipv6s.fd, readFds))
    {
        CAAcceptConnection(CA_IPV6 | CA_SECURE, &caglobals.tcp.ipv6s);
        return;
    }
    else if (-1 != caglobals.tcp.connectionFds[0] &&
            FD_ISSET(caglobals.tcp.connectionFds[0], readFds))
    {
        // new connection was created from remote device.
        // exit the function to update read file descriptor.
        char buf[MAX_ADDR_STR_SIZE_CA] = {0};
        ssize_t len = read(caglobals.tcp.connectionFds[0], buf, sizeof (buf));
        if (-1 == len)
        {
            return;
        }
        OIC_LOG_V(DEBUG, TAG, "Received new connection event with [%s]", buf);
        return;
    }
    else
    {
        oc_mutex_lock(tcp_mutexObjectList);
        CATCPSessionInfo_t *session = NULL;
        CATCPSessionInfo_t *tmp = NULL;
        LL_FOREACH_SAFE(tcp_sessionList, session, tmp)
        {
            if (session && session->fd != OC_INVALID_SOCKET)
            {
                if (FD_ISSET(session->fd, readFds))
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
                        LL_DELETE(tcp_sessionList, session);
                        CADisconnectTCPSession(session);
                        oc_mutex_unlock(tcp_mutexObjectList);
                        return;
                    }
                }
            }
        }
        oc_mutex_unlock(tcp_mutexObjectList);
    }
}
