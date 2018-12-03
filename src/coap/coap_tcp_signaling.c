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

#include "coap_tcp_signaling.h"

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

#define TAG "TCPCOAP"

#if INTERFACE
/**
 * TCP Capability and Settings message (CSM) exchange state.
 * Capability and Settings message must be sent
 * as the first message for both server/client.
 */
typedef enum
{
    NONE = 0,      /**< none state */
    SENT,          /**< sent state */
    RECEIVED,      /**< received state */
    SENT_RECEIVED  /**< sent and received state */
} CACSMExchangeState_t;
#endif

/* from catcpserver.c */
CACSMExchangeState_t CAGetCSMState(const CAEndpoint_t *endpoint)
{
    CACSMExchangeState_t csmState = NONE;
    oc_refcounter ref = CAGetTCPSessionInfoRefCountedFromEndpoint(endpoint);
    CATCPSessionInfo_t *svritem =  (CATCPSessionInfo_t *) oc_refcounter_get_data(ref);
    if (svritem)
    {
        csmState = svritem->CSMState;
    }
    oc_refcounter_dec(ref);

    return csmState;
}

/* from catcpserver.c */
void CAUpdateCSMState(const CAEndpoint_t *endpoint, CACSMExchangeState_t state)
{
    oc_refcounter ref = CAGetTCPSessionInfoRefCountedFromEndpoint(endpoint);
    CATCPSessionInfo_t *svritem =  (CATCPSessionInfo_t *) oc_refcounter_get_data(ref);
    if (svritem)
    {
        svritem->CSMState = state;
    }
    oc_refcounter_dec(ref);

    return;
}

