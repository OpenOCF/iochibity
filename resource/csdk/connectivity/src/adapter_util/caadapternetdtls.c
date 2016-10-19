/******************************************************************
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
#include "platform_features.h"
#include "caadapternetdtls.h"
#include "cacommon.h"
#include "caipinterface.h"
#include "dtls.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "global.h"
#include "timer.h"
#if defined(HAVE_WINSOCK2_H) && defined(HAVE_WS2TCPIP_H)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

/* tinyDTLS library error code */
#define TINY_DTLS_ERROR (-1)
/* tinyDTLS library success code */
#define TINY_DTLS_SUCCESS (0)

#ifdef __WITH_X509__
#include "pki.h"
#include "crl.h"
#include "cainterface.h"

/* lenght of ASN.1 header in DER format
 * for subject field in X.509 certificate */
#define DER_SUBJECT_HEADER_LEN  (9)

#undef VERIFY_SUCCESS
#define VERIFY_SUCCESS(op, successCode) { if ((op) != (successCode)) \
            {OIC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} }
#endif

/**
 * @def TAG
 * @brief Logging tag for module name
 */
#define TAG "OIC_CA_NET_DTLS"

/**
 * @var g_caDtlsContext
 * @brief global context which holds dtls context and cache list information.
 */
static stCADtlsContext_t *g_caDtlsContext = NULL;

/**
 * @var g_dtlsContextMutex
 * @brief Mutex to synchronize access to g_caDtlsContext.
 */
static ca_mutex g_dtlsContextMutex = NULL;

/**
 * @var g_getCredentialsCallback
 * @brief callback to get DTLS credentials
 */
static CAGetDTLSPskCredentialsHandler g_getCredentialsCallback = NULL;

/**
 * @var RETRANSMISSION_TIME
 * @brief Maximum timeout value (in seconds) to start DTLS retransmission.
 */
#define RETRANSMISSION_TIME 1

/**
 * @var g_dtlsHandshakeCallback
 * @brief callback to deliver the DTLS handshake result
 */
static CAErrorCallback g_dtlsHandshakeCallback = NULL;

#ifdef __WITH_X509__
/**
 * @var g_getX509CredentialsCallback
 * @brief callback to get DTLS certificate credentials
 */
static CAGetDTLSX509CredentialsHandler g_getX509CredentialsCallback = NULL;
/**
 * @var g_getCrlCallback
 * @brief callback to get CRL for DTLS
 */
static CAGetDTLSCrlHandler g_getCrlCallback = NULL;
#endif //__WITH_X509__


static CASecureEndpoint_t *GetPeerInfo(const CAEndpoint_t *peer)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    uint32_t list_index = 0;
    uint32_t list_length = 0;

    if(NULL == peer)
    {
        OIC_LOG(ERROR, TAG, "CAPeerInfoListContains invalid parameters");
        return NULL;
    }

    CASecureEndpoint_t *peerInfo = NULL;
    list_length = u_arraylist_length(g_caDtlsContext->peerInfoList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        peerInfo = (CASecureEndpoint_t *)u_arraylist_get(g_caDtlsContext->peerInfoList, list_index);
        if (NULL == peerInfo)
        {
            continue;
        }

        if((0 == strncmp(peer->addr, peerInfo->endpoint.addr, MAX_ADDR_STR_SIZE_CA)) &&
                (peer->port == peerInfo->endpoint.port))
        {
            return peerInfo;
        }
    }
    return NULL;
}

static CAResult_t CAAddIdToPeerInfoList(const char *peerAddr, uint32_t port,
					const unsigned char *id, uint16_t id_length)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY\n\taddr: %s\n\tport: %d\n\tlen: %d",
	      __func__, peerAddr, port, id_length);
    /*GAR DEBUG*/
    printf("\tid: ");
    for (int idx=0; idx<id_length; idx++) {
	printf("%02X", id[idx]);
    }
    printf("\n");

    if(NULL == peerAddr
       || NULL == id
       || 0 == port
       || 0 == id_length
       || CA_MAX_ENDPOINT_IDENTITY_LEN < id_length)
    {
        OIC_LOG(ERROR, TAG, "CAAddIdToPeerInfoList invalid parameters");
        return CA_STATUS_INVALID_PARAM;
    }

    CASecureEndpoint_t *peer = (CASecureEndpoint_t *)OICCalloc(1, sizeof (CASecureEndpoint_t));
    if (NULL == peer)
    {
        OIC_LOG(ERROR, TAG, "peerInfo malloc failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    OICStrcpy(peer->endpoint.addr, sizeof(peer->endpoint.addr), peerAddr);
    peer->endpoint.port = port;

    memcpy(peer->identity.id, id, id_length);
    peer->identity.id_length = id_length;

    if (NULL != GetPeerInfo(&peer->endpoint))
    {
        OIC_LOG(ERROR, TAG, "CAAddIdToPeerInfoList peer already exist");
        OICFree(peer);
        return CA_STATUS_FAILED;
    }

    bool result = u_arraylist_add(g_caDtlsContext->peerInfoList, (void *)peer);
    if (!result)
    {
        OIC_LOG(ERROR, TAG, "u_arraylist_add failed!");
        OICFree(peer);
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

static void CAFreePeerInfoList()
{
    uint32_t list_length = u_arraylist_length(g_caDtlsContext->peerInfoList);
    for (uint32_t list_index = 0; list_index < list_length; list_index++)
    {
        CAEndpoint_t *peerInfo = (CAEndpoint_t *)u_arraylist_get(
                                     g_caDtlsContext->peerInfoList, list_index);
        OICFree(peerInfo);
    }
    u_arraylist_free(&(g_caDtlsContext->peerInfoList));
    g_caDtlsContext->peerInfoList = NULL;
}

static void CARemovePeerFromPeerInfoList(const char * addr, uint16_t port)
{
    if (NULL == addr || 0 >= port)
    {
        OIC_LOG(ERROR, TAG, "CADTLSGetPeerPSKId invalid parameters");
        return;
    }

    uint32_t list_length = u_arraylist_length(g_caDtlsContext->peerInfoList);
    for (uint32_t list_index = 0; list_index < list_length; list_index++)
    {
        CAEndpoint_t *peerInfo = (CAEndpoint_t *)u_arraylist_get(
                                g_caDtlsContext->peerInfoList,list_index);
        if (NULL == peerInfo)
        {
            continue;
        }
        if((0 == strncmp(addr, peerInfo->addr, MAX_ADDR_STR_SIZE_CA)) &&
                (port == peerInfo->port))
        {
            OICFree(u_arraylist_remove(g_caDtlsContext->peerInfoList, list_index));
            return;
        }
    }
}

static int CASizeOfAddrInfo(stCADtlsAddrInfo_t *addrInfo)
{
    VERIFY_NON_NULL_RET(addrInfo, TAG, "addrInfo is NULL" , DTLS_FAIL);

    switch (addrInfo->addr.st.ss_family)
    {
    case AF_INET:
        {
            return sizeof (struct sockaddr_in);
        }
    case AF_INET6:
        {
            return sizeof (struct sockaddr_in6);
        }
    default:
        {
            break;
        }
    }
    return sizeof (struct sockaddr_storage);
}

static eDtlsRet_t CAAdapterNetDtlsEncryptInternal(const stCADtlsAddrInfo_t *dstSession,
        uint8_t *data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    VERIFY_NON_NULL_RET(dstSession, TAG, "Param dstSession is NULL" , DTLS_FAIL);
    VERIFY_NON_NULL_RET(data, TAG, "Param data is NULL" , DTLS_FAIL);

    if (0 == dataLen)
    {
        OIC_LOG(ERROR, TAG, "Given Packet length is equal to zero.");
        return DTLS_FAIL;
    }

    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Context is NULL");
        return DTLS_FAIL;
    }

    // @todo: Remove need to typecast stCADtlsAddrInfo_t --> session_t below.
    // Until then, apply a static assert.
    OC_STATIC_ASSERT((sizeof(session_t) == sizeof(stCADtlsAddrInfo_t)),
         "BUG: session_t size must exactly match stCADtlsAddrInfo_t!");

    int retLen = dtls_write(g_caDtlsContext->dtlsContext, (session_t *)dstSession, data,
                                dataLen);
    OIC_LOG_V(DEBUG, TAG, "%s: dtls_write return len: %d", __func__, retLen);
    if (retLen < 0)
    {
	OIC_LOG_V(DEBUG, TAG, "%s: EXIT, DTLS failure I", __func__);
        return DTLS_FAIL;
    }
    if (0 == retLen)
    {
        // A new DTLS session was initiated by tinyDTLS library and wait for callback.
	OIC_LOG_V(DEBUG, TAG, "%s: EXIT, DTLS session initiated", __func__);
       return DTLS_SESSION_INITIATED;
    }
    else if (dataLen != (uint32_t)retLen)
    {
	OIC_LOG_V(DEBUG, TAG, "%s: EXIT, DTLS failure II", __func__);
        return DTLS_FAIL;
    }
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return DTLS_OK;
}

static eDtlsRet_t CAAdapterNetDtlsDecryptInternal(const stCADtlsAddrInfo_t *srcSession,
        uint8_t *buf, uint32_t bufLen)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    VERIFY_NON_NULL_RET(srcSession, TAG, "Param srcSession is NULL", DTLS_FAIL);
    VERIFY_NON_NULL_RET(buf, TAG, "Param buf is NULL", DTLS_FAIL);

    if (0 == bufLen)
    {
        OIC_LOG(ERROR, TAG, "Given Packet length is equal to zero.");
        return DTLS_FAIL;
    }

    eDtlsRet_t ret = DTLS_FAIL;

    // @todo: Remove need to typecast stCADtlsAddrInfo_t --> session_t below.
    if (dtls_handle_message(g_caDtlsContext->dtlsContext, (session_t *)srcSession, buf, bufLen) == 0)
    {
        OIC_LOG_V(DEBUG, TAG, "%s: EXIT, dtls_handle_message succeeded", __func__);
        ret = DTLS_OK;
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return ret;
}

static void CAFreeCacheMsg(stCACacheMessage_t *msg)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    VERIFY_NON_NULL_VOID(msg, TAG, "msg");

    OICFree(msg->data);
    OICFree(msg);

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
}

static void CAClearCacheList()
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    uint32_t list_index = 0;
    uint32_t list_length = 0;
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Dtls Context is NULL");
        return;
    }
    list_length = u_arraylist_length(g_caDtlsContext->cacheList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        stCACacheMessage_t *msg = (stCACacheMessage_t *)u_arraylist_get(g_caDtlsContext->cacheList,
                                  list_index);
        if (msg != NULL)
        {
            CAFreeCacheMsg(msg);
        }
    }
    u_arraylist_free(&g_caDtlsContext->cacheList);
    g_caDtlsContext->cacheList = NULL;
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
}

static CAResult_t CADtlsCacheMsg(stCACacheMessage_t *msg)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Dtls Context is NULL");
        return CA_STATUS_FAILED;
    }

    bool result = u_arraylist_add(g_caDtlsContext->cacheList, (void *)msg);
    if (!result)
    {
        OIC_LOG(ERROR, TAG, "u_arraylist_add failed!");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return CA_STATUS_OK;
}


static bool CAIsAddressMatching(const stCADtlsAddrInfo_t *a,  const stCADtlsAddrInfo_t *b)
{
    if (a->size != b->size)
    {
        return false;
    }
    if (memcmp(&a->addr, &b->addr, a->size))
    {
        return false;
    }
    return true;
}

static void CASendCachedMsg(const stCADtlsAddrInfo_t *dstSession)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    VERIFY_NON_NULL_VOID(dstSession, TAG, "Param dstSession is NULL");

    uint32_t list_index = 0;
    uint32_t list_length = 0;
    list_length = u_arraylist_length(g_caDtlsContext->cacheList);
    for (list_index = 0; list_index < list_length;)
    {
        stCACacheMessage_t *msg = (stCACacheMessage_t *)u_arraylist_get(g_caDtlsContext->cacheList,
                                  list_index);
        if ((NULL != msg) && (true == CAIsAddressMatching(&(msg->destSession), dstSession)))
        {
            eDtlsRet_t ret = CAAdapterNetDtlsEncryptInternal(&(msg->destSession),
                             msg->data, msg->dataLen);
            if (ret == DTLS_OK)
            {
                OIC_LOG(DEBUG, TAG, "CAAdapterNetDtlsEncryptInternal success");
            }
            else
            {
                OIC_LOG(ERROR, TAG, "CAAdapterNetDtlsEncryptInternal failed.");
            }

            if (u_arraylist_remove(g_caDtlsContext->cacheList, list_index))
            {
                CAFreeCacheMsg(msg);
                // Reduce list length by 1 as we removed one element.
                list_length--;
            }
            else
            {
                OIC_LOG(ERROR, TAG, "u_arraylist_remove failed.");
                break;
            }
        }
        else
        {
            // Move to the next element
            ++list_index;
        }
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
}

static int32_t CAReadDecryptedPayload(dtls_context_t *context,
                                      session_t *session,
                                      uint8_t *buf,
                                      size_t bufLen )
{
    (void)context;
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    VERIFY_NON_NULL_RET(session, TAG, "Param Session is NULL", 0);
    OIC_LOG_V(DEBUG, TAG, "Decrypted buf len [%zu]", bufLen);

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)session;

    CASecureEndpoint_t sep =
    { .endpoint =
    { .adapter = CA_ADAPTER_IP, .flags =
            ((addrInfo->addr.st.ss_family == AF_INET) ? CA_IPV4 : CA_IPV6) | CA_SECURE, .port = 0 },
            .identity =
            { 0 } };
    CAConvertAddrToName(&(addrInfo->addr.st), addrInfo->size, sep.endpoint.addr, &sep.endpoint.port);

    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Context is NULL");
        return TINY_DTLS_ERROR;
    }

    int type = 0;
    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type) &&
        (NULL != g_caDtlsContext->adapterCallbacks[type].recvCallback))
    {
        // Get identity of the source of packet
        CASecureEndpoint_t *peerInfo = GetPeerInfo(&sep.endpoint);
        if (peerInfo)
        {
            sep.identity = peerInfo->identity;
        }

        g_caDtlsContext->adapterCallbacks[type].recvCallback(&sep, buf, bufLen);
    }
    else
    {
        OIC_LOG_V(DEBUG, TAG, "recvCallback Callback or adapter type is wrong [%d]", type);
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return TINY_DTLS_SUCCESS;
}

static int32_t CASendSecureData(dtls_context_t *context,
                                session_t *session,
                                uint8_t *buf,
                                size_t bufLen)
{
    (void)context;
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    VERIFY_NON_NULL_RET(session, TAG, "Param Session is NULL", -1);
    VERIFY_NON_NULL_RET(buf, TAG, "Param buf is NULL", -1);

    if (0 == bufLen)
    {
        OIC_LOG(ERROR, TAG, "Encrypted Buffer length is equal to zero");
        return 0;
    }

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)session;

    CAEndpoint_t endpoint = {.adapter = CA_DEFAULT_ADAPTER};

    CAConvertAddrToName(&(addrInfo->addr.st), addrInfo->size, endpoint.addr, &endpoint.port);
    endpoint.flags = addrInfo->addr.st.ss_family == AF_INET ? CA_IPV4 : CA_IPV6;
    endpoint.flags |= CA_SECURE;
    endpoint.adapter = CA_ADAPTER_IP;
    endpoint.ifindex = session->ifindex;
    int type = 0;

    //Mutex is not required for g_caDtlsContext. It will be called in same thread.
    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type) &&
        (NULL != g_caDtlsContext->adapterCallbacks[type].sendCallback))
    {
        g_caDtlsContext->adapterCallbacks[type].sendCallback(&endpoint, buf, bufLen);
    }
    else
    {
        OIC_LOG_V(DEBUG, TAG, "send Callback or adapter type is wrong [%d]", type );
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return bufLen;
}

static int32_t CAHandleSecureEvent(dtls_context_t *context,
                                   session_t *session,
                                   dtls_alert_level_t level,
                                   unsigned short code)
{
    (void)context;
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    VERIFY_NON_NULL_RET(session, TAG, "Param Session is NULL", 0);

    OIC_LOG_V(DEBUG, TAG, "%s:\n\tlevel: %d = '%s'\n\tcode: %u = '%s'", __func__,
	      level,
	      (level == DTLS_ALERT_LEVEL_WARNING)? "DTLS_ALERT_LEVEL_WARNING"
	      :(level == DTLS_ALERT_LEVEL_FATAL)? "DTLS_ALERT_LEVEL_FATAL"
	      : "UNKNOWN",
	      code,
	      (code == DTLS_EVENT_CONNECT)? "DTLS_EVENT_CONNECT"
	      :(code == DTLS_EVENT_CONNECTED)? "DTLS_EVENT_CONNECTED"
	      :(code == DTLS_EVENT_RENEGOTIATE)? "DTLS_EVENT_RENEGOTIATE"
	      : "UNKNOWN");


    CAEndpoint_t endpoint = {.adapter=CA_DEFAULT_ADAPTER};
    CAErrorInfo_t errorInfo = {.result=CA_STATUS_OK};

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)session;
    char peerAddr[MAX_ADDR_STR_SIZE_CA] = { 0 };
    uint16_t port = 0;
    CAConvertAddrToName(&(addrInfo->addr.st), addrInfo->size, peerAddr, &port);

    if (!level && (DTLS_EVENT_CONNECTED == code))
    {
        OIC_LOG(DEBUG, TAG, "Received DTLS_EVENT_CONNECTED. Sending Cached data");

        if(g_dtlsHandshakeCallback)
        {
            OICStrcpy(endpoint.addr, MAX_ADDR_STR_SIZE_CA, peerAddr);
            endpoint.port = port;
            errorInfo.result = CA_STATUS_OK;
            g_dtlsHandshakeCallback(&endpoint, &errorInfo);
        }

        CASendCachedMsg((stCADtlsAddrInfo_t *)session);
    }
    else if(DTLS_ALERT_LEVEL_FATAL == level && DTLS_ALERT_DECRYPT_ERROR == code)
    {
        if(g_dtlsHandshakeCallback)
        {
            OICStrcpy(endpoint.addr, MAX_ADDR_STR_SIZE_CA, peerAddr);
            endpoint.addr[MAX_ADDR_STR_SIZE_CA - 1] = '\0';
            endpoint.port = port;
            errorInfo.result = CA_DTLS_AUTHENTICATION_FAILURE;
            g_dtlsHandshakeCallback(&endpoint, &errorInfo);
        }
    }
    else if(DTLS_ALERT_LEVEL_FATAL == level && DTLS_ALERT_HANDSHAKE_FAILURE == code)
    {
        OIC_LOG_V(INFO, TAG, "%s: Failed to DTLS handshake, the peer will be removed.", __func__);
        CARemovePeerFromPeerInfoList(peerAddr, port);
    }
    else if(DTLS_ALERT_LEVEL_FATAL == level || DTLS_ALERT_CLOSE_NOTIFY == code)
    {
        OIC_LOG(INFO, TAG, "Peer closing connection");
        CARemovePeerFromPeerInfoList(peerAddr, port);
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return TINY_DTLS_SUCCESS;
}


static int32_t CAGetPskCredentials(dtls_context_t *ctx,
                                   const session_t *session,
                                   dtls_credentials_type_t type,
                                   const unsigned char *desc, size_t descLen,
                                   unsigned char *result, size_t resultLen)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY, cred type %d = '%s', desc: '%s', desc len: %d", __func__,
	      type,
	      (type == DTLS_PSK_HINT)? "DTLS_PSK_HINT"
	      :(type == DTLS_PSK_IDENTITY)? "DTLS_PSK_IDENTITY"
	      :(type == DTLS_PSK_KEY)? "DTLS_PSK_KEY"
	      : "UNKNOWN",
	      desc, descLen);

    int32_t ret = TINY_DTLS_ERROR;
    if(NULL == ctx || NULL == session || NULL == result)
    {
        OIC_LOG(ERROR, TAG, "CAGetPskCredentials invalid parameters");
        return ret;
    }

    VERIFY_NON_NULL_RET(g_getCredentialsCallback, TAG, "GetCredential callback", -1);

    // Retrieve the credentials blob from security module
    ret =  g_getCredentialsCallback( (CADtlsPskCredType_t)type, desc, descLen, result, resultLen);

    if (ret > 0)
    {
        // TODO SRM needs identity of the remote end-point with every data packet to
        // perform access control management. tinyDTLS 'frees' the handshake parameters
        // data structure when handshake completes. Therefore, currently this is a
        // workaround to cache remote end-point identity when tinyDTLS asks for PSK.
        stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)session;
        char peerAddr[MAX_ADDR_STR_SIZE_CA] = { 0 };
        uint16_t port = 0;
        CAConvertAddrToName(&(addrInfo->addr.st), addrInfo->size, peerAddr, &port);

        if(CA_STATUS_OK != CAAddIdToPeerInfoList(peerAddr, port, desc, descLen) )
        {
            OIC_LOG_V(ERROR, TAG, "%s: Fail to add peer id to gDtlsPeerInfoList", __func__);
        }
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT returnting %x", __func__, ret);
    return ret;
}

void CADTLSSetAdapterCallbacks(CAPacketReceivedCallback recvCallback,
                               CAPacketSendCallback sendCallback,
                               CATransportAdapter_t type)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return;
    }

    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type))
    {
        // TODO: change the zeros to better values.
        g_caDtlsContext->adapterCallbacks[0].recvCallback = recvCallback;
        g_caDtlsContext->adapterCallbacks[0].sendCallback = sendCallback;
    }

    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
}

void CADTLSSetHandshakeCallback(CAErrorCallback dtlsHandshakeCallback)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    g_dtlsHandshakeCallback = dtlsHandshakeCallback;
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
}

void CADTLSSetCredentialsCallback(CAGetDTLSPskCredentialsHandler credCallback)
{
    // TODO Does this method needs protection of DtlsContextMutex ?
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    g_getCredentialsCallback = credCallback;
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT", __func__);
}

#ifdef __WITH_X509__
void CADTLSSetX509CredentialsCallback(CAGetDTLSX509CredentialsHandler credCallback)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    g_getX509CredentialsCallback = credCallback;
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
}
void CADTLSSetCrlCallback(CAGetDTLSCrlHandler crlCallback)
{
    // TODO Does this method needs protection of DtlsContextMutex ?
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    g_getCrlCallback = crlCallback;
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
}
#endif // __WITH_X509__

CAResult_t CADtlsSelectCipherSuite(const dtls_cipher_t cipher)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }
    dtls_select_cipher(g_caDtlsContext->dtlsContext, cipher);
    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG_V(DEBUG, TAG, "Selected cipher suite is 0x%02X%02X\n",
        ((uint8_t*)(&cipher))[1], ((uint8_t*)(&cipher))[0]);

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return CA_STATUS_OK ;
}

CAResult_t CADtlsEnableAnonECDHCipherSuite(const bool enable)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }
    dtls_enables_anon_ecdh(g_caDtlsContext->dtlsContext,
        enable == true ? DTLS_CIPHER_ENABLE : DTLS_CIPHER_DISABLE);
    ca_mutex_unlock(g_dtlsContextMutex);
    OIC_LOG_V(DEBUG, TAG, "TLS_ECDH_anon_WITH_AES_128_CBC_SHA_256  is %s",
        enable ? "enabled" : "disabled");

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return CA_STATUS_OK ;
}

CAResult_t CADtlsInitiateHandshake(const CAEndpoint_t *endpoint)
{
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);

    stCADtlsAddrInfo_t dst = { 0 };

    if(!endpoint)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    CAConvertNameToAddr(endpoint->addr, endpoint->port, &(dst.addr.st));
    dst.ifIndex = 0;
    dst.size = CASizeOfAddrInfo(&dst);

    ca_mutex_lock(g_dtlsContextMutex);
    if(NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    if(0 > dtls_connect(g_caDtlsContext->dtlsContext, (session_t*)(&dst)))
    {
        OIC_LOG(ERROR, TAG, "Failed to connect");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return CA_STATUS_OK;
}

CAResult_t CADtlsClose(const CAEndpoint_t *endpoint)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    stCADtlsAddrInfo_t dst = { 0 };

    if(!endpoint)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    CAConvertNameToAddr(endpoint->addr, endpoint->port, &(dst.addr.st));
    dst.ifIndex = 0;
    dst.size = CASizeOfAddrInfo(&dst);

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    if (0 > dtls_close(g_caDtlsContext->dtlsContext, (session_t*)(&dst)))
    {
        OIC_LOG(ERROR, TAG, "Failed to close the session");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return CA_STATUS_OK;
}

CAResult_t CADtlsGenerateOwnerPSK(const CAEndpoint_t *endpoint,
                    const uint8_t* label, const size_t labelLen,
                    const uint8_t* rsrcServerDeviceID, const size_t rsrcServerDeviceIDLen,
                    const uint8_t* provServerDeviceID, const size_t provServerDeviceIDLen,
                    uint8_t* ownerPSK, const size_t ownerPSKSize)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    if(!endpoint || !label || 0 == labelLen || !ownerPSK || 0 == ownerPSKSize)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    stCADtlsAddrInfo_t dst = { 0 };

    CAConvertNameToAddr(endpoint->addr, endpoint->port, &(dst.addr.st));
    dst.ifIndex = 0;
    dst.size = CASizeOfAddrInfo(&dst);

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    if( 0 == dtls_prf_with_current_keyblock(g_caDtlsContext->dtlsContext, (session_t*)(&dst),
                 label, labelLen, rsrcServerDeviceID, rsrcServerDeviceIDLen,
                 provServerDeviceID, provServerDeviceIDLen, ownerPSK, ownerPSKSize))
    {
        OIC_LOG(ERROR, TAG, "Failed to DTLS PRF");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }
    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return CA_STATUS_OK;
}

#ifdef __WITH_X509__
static CADtlsX509Creds_t g_X509Cred = {{0}, 0, 0, {0}, {0}, {0}};

int CAInitX509()
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    VERIFY_NON_NULL_RET(g_getX509CredentialsCallback, TAG, "GetX509Credential callback", -1);
    int isX509Init = (0 == g_getX509CredentialsCallback(&g_X509Cred));

    if (isX509Init)
    {
        uint8_t crlData[CRL_MAX_LEN] = {0};
        ByteArray crlArray = {crlData, CRL_MAX_LEN};
        g_getCrlCallback(&crlArray);
        if (crlArray.len > 0)
        {
            uint8_t keyData[PUBLIC_KEY_SIZE] = {0};
            CertificateList crl = CRL_INITIALIZER;
            ByteArray rootPubKey = {keyData, PUBLIC_KEY_SIZE};
            memcpy(keyData, g_X509Cred.rootPublicKeyX, PUBLIC_KEY_SIZE / 2);
            memcpy(keyData + PUBLIC_KEY_SIZE / 2, g_X509Cred.rootPublicKeyY, PUBLIC_KEY_SIZE / 2);
            DecodeCertificateList(crlArray, &crl, rootPubKey);
        }
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    if (isX509Init)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


static int CAIsX509Active(struct dtls_context_t *ctx)
{
    (void)ctx;
    return TINY_DTLS_SUCCESS;
}

static int CAGetDeviceKey(struct dtls_context_t *ctx,
                       const session_t *session,
                       const dtls_ecc_key_t **result)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    static dtls_ecc_key_t ecdsa_key = {DTLS_ECDH_CURVE_SECP256R1, NULL, NULL, NULL};

    int ret = TINY_DTLS_ERROR;
    VERIFY_SUCCESS(CAInitX509(), 0);

    ecdsa_key.priv_key = g_X509Cred.devicePrivateKey;
    *result = &ecdsa_key;

    ret = TINY_DTLS_SUCCESS;
exit:
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT returning %x", __func__, ret);
    return ret;
}

static int
CAGetDeviceCertificate(struct dtls_context_t *ctx,
                    const session_t *session,
                    const unsigned char **cert,
                    size_t *cert_size)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    int ret = TINY_DTLS_ERROR;

    VERIFY_SUCCESS(CAInitX509(), 0);

    *cert = g_X509Cred.certificateChain;
    *cert_size = g_X509Cred.certificateChainLen;
#ifdef X509_DEBUG
    ByteArray ownCert = {g_X509Cred.certificateChain, g_X509Cred.certificateChainLen};
    PRINT_BYTE_ARRAY("OWN CERT: \n", ownCert);
#endif

    ret = TINY_DTLS_SUCCESS;
exit:
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT returning", __func__, ret);
    return ret;
}
/**
 * @fn  CAGetRootKey
 * @brief  Gets x and y components of Root Certificate Autority public key
 *
 * @return  0 on success otherwise a positive error value.
 *
 */
static int CAGetRootKey(const unsigned char **ca_pub_x, const unsigned char **ca_pub_y)
{
    OIC_LOG(DEBUG, TAG, "CAGetRootKey");
    int ret = 1;

    VERIFY_SUCCESS(CAInitX509(), 0);

    *ca_pub_x = g_X509Cred.rootPublicKeyX;
    *ca_pub_y = g_X509Cred.rootPublicKeyY;

    ret = 0;
exit:
    return ret;
}


static int CAVerifyCertificate(struct dtls_context_t *ctx, const session_t *session,
                               const unsigned char *cert, size_t certLen,
                               const unsigned char *x, size_t xLen,
                               const unsigned char *y, size_t yLen)
{
    OIC_LOG(DEBUG, TAG, "Verify Certificate");

    ByteArray crtChainDer[MAX_CHAIN_LEN];
    CertificateX509 crtChain[MAX_CHAIN_LEN];

    uint8_t chainLength;

    int ret = TINY_DTLS_ERROR;
    const unsigned char *ca_pub_x;
    const unsigned char *ca_pub_y;
    ByteArray certDerCode = BYTE_ARRAY_INITIALIZER;
    ByteArray caPubKey = BYTE_ARRAY_INITIALIZER;
    unsigned char ca_pub_key[PUBLIC_KEY_SIZE];

    if ( !ctx ||  !session ||  !cert || !x || !y)
    {
        return TINY_DTLS_ERROR;
    }

    CAGetRootKey (&ca_pub_x, &ca_pub_y);

    certDerCode.data = (uint8_t *)cert;
    certDerCode.len = certLen;

#ifdef X509_DEBUG
    PRINT_BYTE_ARRAY("CERT :\n", certDerCode);
#endif


    caPubKey.len = PUBLIC_KEY_SIZE;
    caPubKey.data = ca_pub_key;

    memcpy(caPubKey.data, ca_pub_x, PUBLIC_KEY_SIZE / 2);
    memcpy(caPubKey.data + PUBLIC_KEY_SIZE / 2, ca_pub_y, PUBLIC_KEY_SIZE / 2);

    ret = (int)  LoadCertificateChain (certDerCode, crtChainDer, &chainLength);
    VERIFY_SUCCESS(ret, PKI_SUCCESS);
    ret = (int)  ParseCertificateChain (crtChainDer, crtChain, chainLength );
    VERIFY_SUCCESS(ret, PKI_SUCCESS);
    ret = (int)  CheckCertificateChain (crtChain, chainLength, caPubKey);
    VERIFY_SUCCESS(ret, PKI_SUCCESS);

    INC_BYTE_ARRAY(crtChain[0].pubKey, 2);

    memcpy(x, crtChain[0].pubKey.data, xLen);
    memcpy(y, crtChain[0].pubKey.data + PUBLIC_KEY_SIZE / 2, yLen);

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)session;
    char peerAddr[MAX_ADDR_STR_SIZE_CA] = { 0 };
    uint16_t port = 0;
    CAConvertAddrToName(&(addrInfo->addr.st), addrInfo->size, peerAddr, &port);

    CAResult_t result = CAAddIdToPeerInfoList(peerAddr, port,
            crtChain[0].subject.data + DER_SUBJECT_HEADER_LEN + 2, crtChain[0].subject.data[DER_SUBJECT_HEADER_LEN + 1]);
    if (CA_STATUS_OK != result )
    {
        OIC_LOG_V(ERROR, TAG, "%s: Fail to add peer id to gDtlsPeerInfoList", __func__);
    }

exit:
    if (ret != 0)
    {
        OIC_LOG(ERROR, TAG, "Certificate verification FAILED\n");
        return TINY_DTLS_ERROR;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "Certificate verification SUCCESS\n");
        return TINY_DTLS_SUCCESS;
    }
}

#endif

static void CAStartRetransmit()
{
    static int timerId = -1;
    if (timerId != -1)
    {
        //clear previous timer
        unregisterTimer(timerId);

        ca_mutex_lock(g_dtlsContextMutex);

        //stop retransmission if context is invalid
        if(NULL == g_caDtlsContext)
        {
            OIC_LOG(ERROR, TAG, "Context is NULL. Stop retransmission");
            ca_mutex_unlock(g_dtlsContextMutex);
            return;
        }
        dtls_check_retransmit(g_caDtlsContext->dtlsContext, NULL);
        ca_mutex_unlock(g_dtlsContextMutex);
    }
    //start new timer
    registerTimer(RETRANSMISSION_TIME, &timerId, CAStartRetransmit);
}

CAResult_t CAAdapterNetDtlsInit()
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    // Initialize mutex for DtlsContext
    if (NULL == g_dtlsContextMutex)
    {
        g_dtlsContextMutex = ca_mutex_new();
        VERIFY_NON_NULL_RET(g_dtlsContextMutex, TAG, "malloc failed",
            CA_MEMORY_ALLOC_FAILED);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "CAAdapterNetDtlsInit done already!");
        return CA_STATUS_OK;
    }

    // Lock DtlsContext mutex and create DtlsContext
    ca_mutex_lock(g_dtlsContextMutex);
    g_caDtlsContext = (stCADtlsContext_t *)OICCalloc(1, sizeof(stCADtlsContext_t));

    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Context malloc failed");
        ca_mutex_unlock(g_dtlsContextMutex);
        ca_mutex_free(g_dtlsContextMutex);
        return CA_MEMORY_ALLOC_FAILED;
    }


    // Create PeerInfoList and CacheList
    g_caDtlsContext->peerInfoList = u_arraylist_create();
    g_caDtlsContext->cacheList = u_arraylist_create();

    if( (NULL == g_caDtlsContext->peerInfoList) ||
        (NULL == g_caDtlsContext->cacheList))
    {
    OIC_LOG(ERROR, TAG, "peerInfoList or cacheList initialization failed!");
        CAClearCacheList();
        CAFreePeerInfoList();
        OICFree(g_caDtlsContext);
        g_caDtlsContext = NULL;
        ca_mutex_unlock(g_dtlsContextMutex);
        ca_mutex_free(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    // Initialize clock, crypto and other global vars in tinyDTLS library
    dtls_init();

    // Create tinydtls Context
    g_caDtlsContext->dtlsContext = dtls_new_context(g_caDtlsContext);

    if (NULL ==  g_caDtlsContext->dtlsContext)
    {
        OIC_LOG(ERROR, TAG, "dtls_new_context failed");
        ca_mutex_unlock(g_dtlsContextMutex);
        CAAdapterNetDtlsDeInit();
        return CA_STATUS_FAILED;
    }

    g_caDtlsContext->callbacks.write = CASendSecureData;
    g_caDtlsContext->callbacks.read  = CAReadDecryptedPayload;
    g_caDtlsContext->callbacks.event = CAHandleSecureEvent;

    g_caDtlsContext->callbacks.get_psk_info = CAGetPskCredentials;
#ifdef __WITH_X509__
    g_caDtlsContext->callbacks.get_x509_key = CAGetDeviceKey;
    g_caDtlsContext->callbacks.verify_x509_cert = CAVerifyCertificate;
    g_caDtlsContext->callbacks.get_x509_cert = CAGetDeviceCertificate;
    g_caDtlsContext->callbacks.is_x509_active = CAIsX509Active;
#endif //__WITH_X509__*
    dtls_set_handler(g_caDtlsContext->dtlsContext, &(g_caDtlsContext->callbacks));
    ca_mutex_unlock(g_dtlsContextMutex);

    CAStartRetransmit();

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return CA_STATUS_OK;
}

void CAAdapterNetDtlsDeInit()
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    VERIFY_NON_NULL_VOID(g_caDtlsContext, TAG, "context is NULL");
    VERIFY_NON_NULL_VOID(g_dtlsContextMutex, TAG, "context mutex is NULL");

    //Lock DtlsContext mutex
    ca_mutex_lock(g_dtlsContextMutex);

    // Clear all lists
    CAFreePeerInfoList();
    CAClearCacheList();

    // De-initialize tinydtls context
    dtls_free_context(g_caDtlsContext->dtlsContext);
    g_caDtlsContext->dtlsContext = NULL;

    // De-initialize DtlsContext
    OICFree(g_caDtlsContext);
    g_caDtlsContext = NULL;

    // Unlock DtlsContext mutex and de-initialize it
    ca_mutex_unlock(g_dtlsContextMutex);
    ca_mutex_free(g_dtlsContextMutex);
    g_dtlsContextMutex = NULL;

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
}

CAResult_t CAAdapterNetDtlsEncrypt(const CAEndpoint_t *endpoint,
                                   void *data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);

    VERIFY_NON_NULL_RET(endpoint, TAG,"Param remoteAddress is NULL",
                        CA_STATUS_INVALID_PARAM);
    VERIFY_NON_NULL_RET(data, TAG, "Param data is NULL" ,
                        CA_STATUS_INVALID_PARAM);

    if (0 == dataLen)
    {
        OIC_LOG_V(ERROR, TAG, "dataLen is less than or equal zero [%d]", dataLen);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "Data to be encrypted dataLen: %d", dataLen);

    stCADtlsAddrInfo_t addrInfo = { 0 };

    CAConvertNameToAddr(endpoint->addr, endpoint->port, &(addrInfo.addr.st));
    addrInfo.ifIndex = 0;
    addrInfo.size = CASizeOfAddrInfo(&addrInfo);

    ca_mutex_lock(g_dtlsContextMutex);
    if(NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    eDtlsRet_t ret = CAAdapterNetDtlsEncryptInternal(&addrInfo, data, dataLen);
    if (ret == DTLS_SESSION_INITIATED)
    {
        stCACacheMessage_t *message = (stCACacheMessage_t *)OICCalloc(1, sizeof(stCACacheMessage_t));
        if (NULL == message)
        {
            OIC_LOG(ERROR, TAG, "calloc failed!");
            ca_mutex_unlock(g_dtlsContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }

        message->data = (uint8_t *)OICCalloc(dataLen + 1, sizeof(uint8_t));
        if (NULL == message->data)
        {
            OIC_LOG(ERROR, TAG, "calloc failed!");
            OICFree(message);
            ca_mutex_unlock(g_dtlsContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }
        memcpy(message->data, data, dataLen);
        message->dataLen = dataLen;
        message->destSession = addrInfo;

        CAResult_t result = CADtlsCacheMsg(message);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(DEBUG, TAG, "CADtlsCacheMsg failed!");
            CAFreeCacheMsg(message);
        }
        ca_mutex_unlock(g_dtlsContextMutex);
	OIC_LOG_V(DEBUG, TAG, "%s: EXIT returning 0x%X", __func__, result);
        return result;
    }

    ca_mutex_unlock(g_dtlsContextMutex);

    if (ret != DTLS_OK)
    {
        OIC_LOG(ERROR, TAG, "OUT FAILURE");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAAdapterNetDtlsDecrypt(const CASecureEndpoint_t *sep,
                                   uint8_t *data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
    VERIFY_NON_NULL_RET(sep, TAG, "endpoint is NULL" , CA_STATUS_INVALID_PARAM);

    stCADtlsAddrInfo_t addrInfo = { 0 };

    CAConvertNameToAddr(sep->endpoint.addr, sep->endpoint.port, &(addrInfo.addr.st));
    addrInfo.ifIndex = 0;
    addrInfo.size = CASizeOfAddrInfo(&addrInfo);

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        ca_mutex_unlock(g_dtlsContextMutex);
	OIC_LOG_V(DEBUG, TAG, "%s: EXIT with CA_STATUS_FAILED (Context is NULL)", __func__);
        return CA_STATUS_FAILED;
    }

    eDtlsRet_t ret = CAAdapterNetDtlsDecryptInternal(&addrInfo, data, dataLen);
    ca_mutex_unlock(g_dtlsContextMutex);

    switch (ret) {
    case DTLS_OK:
        OIC_LOG_V(DEBUG, TAG, "%s: EXIT on successfully decryption [%d]", __func__, ret);
	return CA_STATUS_OK;
	break;
    case DTLS_HS_MSG:
        OIC_LOG_V(DEBUG, TAG, "%s: EXIT on handshake msg recvd [%d]", __func__, ret);
	return CA_STATUS_OK;
	break;
    default:
	OIC_LOG_V(DEBUG, TAG, "%s: EXIT on decryption error: %d", __func__, ret);
	return CA_STATUS_FAILED;
    }
}
