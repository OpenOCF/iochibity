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
typedef enum
{
    /* Signaling code - START HERE */
    CA_CSM = 701,                           /**< Capability and Settings messages */
    CA_PING = 702,                          /**< Ping messages */
    CA_PONG = 703,                          /**< Pong messages */
    CA_RELEASE = 704,                       /**< Release messages */
    CA_ABORT = 705,                         /**< Abort messages */
    /* Signaling code - END HERE */
} CASignalingCode_t;

/**
 * Signaling information received.
 *
 * This structure is used to hold signaling information.
 */
typedef struct                  /* FIXME: move to e.g. caop/signaling.c */
{
    CASignalingCode_t code;     /**< signaling code */
    CAInfo_t info;              /**< Information of the signaling */
} CASignalingInfo_t;

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

/* src: tcpadapter.c */
bool CATCPIsSignalingData(const CATCPData *tcpData)
{
    // Check if the tcp data is a COAP signaling message as per RFC 8323
    // by checking the code in the header
    uint32_t code = CAGetCodeFromHeader(tcpData->data);
    return code == CA_CSM || code == CA_PING ||
           code == CA_PONG || code == CA_RELEASE || code == CA_ABORT;
}

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

/* src: caremotehandler.c  */
CASignalingInfo_t *CACloneSignalingInfo(const CASignalingInfo_t *sig)
{
    if (NULL == sig)
    {
        OIC_LOG(ERROR, TAG, "Singnaling pointer is NULL");
        return NULL;
    }

    // check the result value of signaling info.
    // Keep this check in sync with CASignalingCode_t.
    switch (sig->code)
    {
        case CA_CSM:
        case CA_PING:
        case CA_PONG:
        case CA_RELEASE:
        case CA_ABORT:
            break;
        default:
            OIC_LOG_V(ERROR, TAG, "Signaling code  %u is invalid", sig->code);
            return NULL;
    }

    // allocate the signaling info structure.
    CASignalingInfo_t *clone = (CASignalingInfo_t *) OICCalloc(1, sizeof(CASignalingInfo_t));
    if (NULL == clone)
    {
        OIC_LOG(ERROR, TAG, "CACloneSignalingInfo Out of memory");
        return NULL;
    }

    CAResult_t result = CACloneInfo(&sig->info, &clone->info);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CACloneResponseInfo error in CACloneInfo");
        CADestroySignalingInfoInternal(clone);
        return NULL;
    }

    clone->code = sig->code;
    return clone;
}

/* src: caremotehandler.c  */
void CADestroySignalingInfoInternal(CASignalingInfo_t *sig)
{
    if (NULL == sig)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return;
    }

    CADestroyInfoInternal(&sig->info);
    OICFree(sig);
}

/* src: camessagehandler.c */
CAData_t *CAGenerateSignalingMessage(const CAEndpoint_t *endpoint, CASignalingCode_t code,
                                     CAHeaderOption_t *headerOpt, uint8_t numOptions)
{
    OIC_LOG(DEBUG, TAG, "GenerateSignalingMessage - IN");

    // create token for signaling message.
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    if (CA_STATUS_OK != CAGenerateTokenInternal(&token, tokenLength))
    {
        OIC_LOG(ERROR, TAG, "CAGenerateTokenInternal failed");
        return NULL;
    }

    CAInfo_t signalingData = { .type = CA_MSG_NONCONFIRM,
                               .token = token,
                               .tokenLength = tokenLength,
                               .numOptions = numOptions,
                               .options = headerOpt };

    CASignalingInfo_t sigMsg = { .code = code,
                                 .info = signalingData };

    return CAPrepareSendData(endpoint, &sigMsg, CA_SIGNALING_DATA);
}

CAData_t *CAGenerateSignalingMessageUsingToken(const CAEndpoint_t *endpoint, CASignalingCode_t code,
                                               CAHeaderOption_t *headerOpt, uint8_t numOptions,
                                               const CAToken_t pingToken, uint8_t pingTokenLength)
{
    OIC_LOG(DEBUG, TAG, "GenerateSignalingMessage - IN");

    // create token for signaling message.
    CAToken_t token = (char *)OICCalloc(pingTokenLength, sizeof(char));
    memcpy(token, pingToken, pingTokenLength);

    CAInfo_t signalingData = { .type = CA_MSG_NONCONFIRM,
                               .token = token,
                               .tokenLength = pingTokenLength,
                               .numOptions = numOptions,
                               .options = headerOpt };

    CASignalingInfo_t sigMsg = { .code = code,
                                 .info = signalingData };

    return CAPrepareSendData(endpoint, &sigMsg, CA_SIGNALING_DATA);
}

/* src: caprotocolmessage.c */
CAResult_t CAGetSignalingInfoFromPDU(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                                     CASignalingInfo_t *outSigInfo)
{
    VERIFY_NON_NULL_MSG(pdu, TAG, "pdu");
    VERIFY_NON_NULL_MSG(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL_MSG(outSigInfo, TAG, "outSigInfo");

    uint32_t code = CA_NOT_FOUND;
    CAResult_t ret = CAGetInfoFromPDU(pdu, endpoint, &code, &(outSigInfo->info));
    outSigInfo->code = code;

    return ret;
}
