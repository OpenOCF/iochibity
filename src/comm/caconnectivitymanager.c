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

#include "caconnectivitymanager.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

/* IPv4 and IPv6 both forced on in OCInitializeInternal, so we initialize statically instead */
// @rewrite: get rid of caglobals
CAGlobals_t caglobals = { .clientFlags = CA_IPV4|CA_IPV6,
                          .serverFlags = CA_IPV4|CA_IPV6 };

CATransportFlags_t ocf_clientFlags = CA_IPV4|CA_IPV6; /**< flag for client */
CATransportFlags_t ocf_serverFlags = CA_IPV4|CA_IPV6; /**< flag for server */

/* set by ocstack::OCInitializeInternal */
bool ocf_client; /**< client mode */
bool ocf_server; /**< server mode */


#define TAG "OIC_CA_CONN_MGR"

bool g_isInitialized = false;

CAResult_t CAInitialize(CATransportAdapter_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, transport type: %s (%d)", __func__,
	      CA_TRANSPORT_ADAPTER_STRING(transportType), transportType);

    if (!g_isInitialized)
    {
        CAResult_t res = CAInitializeMessageHandler(transportType);
        if (res != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "CAInitialize has failed");
            CATerminateMessageHandler();
            return res;
        }
        g_isInitialized = true;
    }

    return CA_STATUS_OK;
}

void CATerminate(void)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);

    if (g_isInitialized)
    {
        CATerminateMessageHandler();
        /* CATerminateNetworkType(); */

        g_isInitialized = false;
    }
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

CAResult_t CAStartListeningServer(void)
{
    OIC_LOG(DEBUG, TAG, "CAStartListeningServer");

    if (!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CAStartListeningServerAdapters();
}

/* NOTE: this is not called from anywhere in the stack. Only for testing? */
CAResult_t CAStopListeningServer()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    if (!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CAStopListeningServerAdapters();
}

// FIXME: rename _oocf_start_udp?
CAResult_t CAStartDiscoveryServer(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    if (!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CAStartDiscoveryServerAdapters();
}

void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler,
                       CAErrorCallback ErrorHandler)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    // GAR: unnecessary
    /* if (!g_isInitialized) */
    /* { */
    /*     OIC_LOG(DEBUG, TAG, "CA is not initialized"); */
    /*     return; */
    /* } */

    CASetInterfaceCallbacks(ReqHandler, RespHandler, ErrorHandler);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)

CAResult_t CAGetSecureEndpointData(const CAEndpoint_t *peer, CASecureEndpoint_t *sep)
{
    OIC_LOG(DEBUG, TAG, "IN CAGetSecurePeerInfo");

    if (!g_isInitialized)
    {
        OIC_LOG(DEBUG, TAG, "CA is not initialized");
        return CA_STATUS_NOT_INITIALIZED;
    }

    OIC_LOG(DEBUG, TAG, "OUT CAGetSecurePeerInfo");
    return GetCASecureEndpointData(peer, sep);
}

bool CASetSecureEndpointAttribute(const CAEndpoint_t* peer, uint32_t attribute)
{
    bool success = false;
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    if (!g_isInitialized)
    {
        OIC_LOG(DEBUG, TAG, "CA is not initialized");
    }
    else
    {
        success = SetCASecureEndpointAttribute(peer, attribute);
    }

    OIC_LOG_V(DEBUG, TAG, "Out %s -> %u", __func__, (uint32_t)success);
    return success;
}

bool CAGetSecureEndpointAttributes(const CAEndpoint_t* peer, uint32_t* attributes)
{
    bool success = false;
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    if (!g_isInitialized)
    {
        OIC_LOG(DEBUG, TAG, "CA is not initialized");
    }
    else
    {
        success = GetCASecureEndpointAttributes(peer, attributes);
    }

    OIC_LOG_V(DEBUG, TAG, "Out %s -> %u", __func__, (uint32_t)success);
    return success;
}

CAResult_t CAregisterSslHandshakeCallback(CAHandshakeErrorCallback tlsHandshakeCallback)
{
    OIC_LOG(DEBUG, TAG, "CAregisterSslHandshakeCallback");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    CAsetSslHandshakeCallback(tlsHandshakeCallback);
    return CA_STATUS_OK;
}

CAResult_t CAregisterPskCredentialsHandler(CAgetPskCredentialsHandler getTlsCredentialsHandler)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    if (!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }
    CAsetPskCredentialsCallback(getTlsCredentialsHandler);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAregisterPkixInfoHandler(CAgetPkixInfoHandler getPkixInfoHandler)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    if (!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }
    CAsetPkixInfoCallback(getPkixInfoHandler);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAregisterGetCredentialTypesHandler(CAgetCredentialTypesHandler getCredTypesHandler)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    if (!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }
    CAsetCredentialTypesCallback(getCredTypesHandler);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAregisterIdentityHandler(CAgetIdentityHandler getIdentityHandler)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    if (!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }
    CAsetIdentityCallback(getIdentityHandler);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}
#endif // __WITH_DTLS__ or __WITH_TLS__

CAResult_t CACreateEndpoint(CATransportFlags_t flags,
                            CATransportAdapter_t adapter,
                            const char *addr,
                            uint16_t port,
                            CAEndpoint_t **object)
{
    if (!object)
    {
        OIC_LOG(ERROR, TAG, "Invalid Parameter");
        return CA_STATUS_INVALID_PARAM;
    }

    CAEndpoint_t *endpoint = CACreateEndpointObject(flags, adapter, addr, port);
    if (!endpoint)
    {
        return CA_STATUS_FAILED;
    }
    *object = endpoint;
    return CA_STATUS_OK;
}

void CADestroyEndpoint(CAEndpoint_t *rep)
{
    OIC_LOG(DEBUG, TAG, "CADestroyEndpoint");

    CAFreeEndpoint(rep);
}

CAResult_t CAGenerateToken(uint8_t **token, uint8_t tokenLength)
{
    OIC_LOG(DEBUG, TAG, "CAGenerateToken");

    return CAGenerateTokenInternal(token, tokenLength);
}

void CADestroyToken(uint8_t *token)
{
    OIC_LOG(DEBUG, TAG, "CADestroyToken");

    CADestroyTokenInternal(token);

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAGetNetworkInformation(CAEndpoint_t **info, size_t *size)
{
    OIC_LOG(DEBUG, TAG, "CAGetNetworkInformation");

    if (!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    if (NULL == info || NULL == size)
    {
        OIC_LOG(ERROR, TAG, "Input parameter is invalid value");

        return CA_STATUS_INVALID_PARAM;
    }

    return CAGetNetworkInfo(info, size);
    // return CAGetNetworkInformationInternal(info, size);
}

static CAResult_t CASendMessageMultiAdapter(const CAEndpoint_t *dest_ep, const void *sendMsg,
                                            CADataType_t dataType)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_LOG_V(ERROR, TAG, "payload size: %u", ((struct CARequestInfo *)sendMsg)->info.payloadSize);

    CATransportAdapter_t connTypes[] = {
            CA_ADAPTER_IP
#ifdef LE_ADAPTER
            ,CA_ADAPTER_GATT_BTLE
#endif
#ifdef EDR_ADAPTER
            ,CA_ADAPTER_RFCOMM_BTEDR
#endif
#ifdef NFC_ADAPTER
            ,CA_ADAPTER_NFC
#endif
#ifdef RA_ADAPTER
            ,CA_ADAPTER_REMOTE_ACCESS
#endif
#ifdef TCP_ADAPTER
            ,CA_ADAPTER_TCP
#endif
        };

    CAEndpoint_t *cloneEp = CACloneEndpoint(dest_ep);
    if (!cloneEp)
    {
        OIC_LOG(ERROR, TAG, "Failed to clone CAEndpoint");
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAResult_t ret = CA_STATUS_OK;
    size_t numConnTypes = sizeof(connTypes) / sizeof(connTypes[0]);

    for (size_t i = 0; i < numConnTypes && ret == CA_STATUS_OK; i++)
    {
        cloneEp->adapter = connTypes[i];
        ret = CADetachSendMessage(cloneEp, sendMsg, dataType);
    }
    CAFreeEndpoint(cloneEp);
    return ret;
}

CAResult_t CASendRequest(const CAEndpoint_t *dest_ep, const struct CARequestInfo *requestInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendRequest");

    if (!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    if (requestInfo && requestInfo->isMulticast &&
            (dest_ep->adapter == CA_DEFAULT_ADAPTER || dest_ep->adapter == CA_ALL_ADAPTERS))
    {
        return CASendMessageMultiAdapter(dest_ep, requestInfo, CA_REQUEST_DATA);
    }
    else
    {
        return CADetachSendMessage(dest_ep, requestInfo, CA_REQUEST_DATA);
    }
}

CAResult_t CASendResponse(const CAEndpoint_t *dest_ep, const CAResponseInfo_t *responseInfo)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_LOG_V(ERROR, TAG, "payload size: %u", ((CARequestInfo *)responseInfo)->info.payloadSize);

    if (!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    if (!responseInfo || !dest_ep)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    if (responseInfo->isMulticast &&
            (dest_ep->adapter == CA_DEFAULT_ADAPTER || dest_ep->adapter == CA_ALL_ADAPTERS))
    {
        return CASendMessageMultiAdapter(dest_ep, responseInfo, responseInfo->info.dataType);
    }
    else
    {
        return CADetachSendMessage(dest_ep, responseInfo, responseInfo->info.dataType);
    }
}

// called by ocstack/OCSelectNetwork, once per transport, pointlessly
/* CAResult_t CASelectNetwork(CATransportAdapter_t transport) // @was transport <- interestedNetwork) */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); */

/*     if (!g_isInitialized) */
/*     { */
/* 	OIC_LOG_V(ERROR, TAG, "%s !g_isInitialized", __func__); */
/*         return CA_STATUS_NOT_INITIALIZED; */
/*     } */

/*     CAResult_t res = CA_STATUS_OK; */

/*     res = CAAddNetworkType(transport); */

/*     // @rewrite: init always uses OC_DEFAULT_FLAGS, so we already know */
/*     // this stuff at build time otoh, init2 is parameterized by */
/*     // transport type. seems a bad idea, this should be a build-time */
/*     // config option, not a runtime choice.  iow all this can be done */
/*     // via #ifdef <TRANSPORT>_ADAPTER - which is what */
/*     // e.g. cainterfacecontroller does. */
/* /\*     if (interestedNetwork & CA_ADAPTER_IP) *\/ */
/* /\*     { *\/ */
/* /\*         res = CAAddNetworkType(CA_ADAPTER_IP); *\/ */
/* /\*         OIC_LOG_V(DEBUG, TAG, "CAAddNetworkType(CA_IP_ADAPTER) function returns result: %d", res); *\/ */
/* /\*     } *\/ */
/* /\*     else if (interestedNetwork & CA_ADAPTER_RFCOMM_BTEDR) *\/ */
/* /\*     { *\/ */
/* /\*         res = CAAddNetworkType(CA_ADAPTER_RFCOMM_BTEDR); *\/ */
/* /\*         OIC_LOG_V(DEBUG, TAG, "CAAddNetworkType(CA_RFCOMM_ADAPTER) function returns result : %d", res); *\/ */
/* /\*     } *\/ */
/* /\*     else if (interestedNetwork & CA_ADAPTER_GATT_BTLE) *\/ */
/* /\*     { *\/ */
/* /\*         res = CAAddNetworkType(CA_ADAPTER_GATT_BTLE); *\/ */
/* /\*         OIC_LOG_V(DEBUG, TAG, "CAAddNetworkType(CA_GATT_ADAPTER) function returns result : %d", res); *\/ */
/* /\*     } *\/ */

/* /\* #ifdef RA_ADAPTER *\/ */
/* /\*     else if (interestedNetwork & CA_ADAPTER_REMOTE_ACCESS) *\/ */
/* /\*     { *\/ */
/* /\*         res = CAAddNetworkType(CA_ADAPTER_REMOTE_ACCESS); *\/ */
/* /\*         OIC_LOG_V(DEBUG, TAG, *\/ */
/* /\*                   "CAAddNetworkType(CA_ADAPTER_REMOTE_ACCESS) function returns result : %d", res); *\/ */
/* /\*     } *\/ */
/* /\* #endif *\/ */

/* /\* #ifdef TCP_ADAPTER *\/ */
/* /\*     else if (interestedNetwork & CA_ADAPTER_TCP) *\/ */
/* /\*     { *\/ */
/* /\*         res = CAAddNetworkType(CA_ADAPTER_TCP); *\/ */
/* /\*         OIC_LOG_V(DEBUG, TAG, *\/ */
/* /\*                   "CAAddNetworkType(CA_ADAPTER_TCP) function returns result : %d", res); *\/ */
/* /\*     } *\/ */
/* /\* #endif *\/ */
/* /\*     else if (interestedNetwork & CA_ADAPTER_NFC) *\/ */
/* /\*     { *\/ */
/* /\*         res = CAAddNetworkType(CA_ADAPTER_NFC); *\/ */
/* /\*         OIC_LOG_V(DEBUG, TAG, "CAAddNetworkType(CA_ADAPTER_NFC) function returns result : %d", res); *\/ */
/* /\*     } *\/ */
/* /\*     else *\/ */
/* /\*     { *\/ */
/* /\*         res = CA_NOT_SUPPORTED; *\/ */
/* /\*     } *\/ */
/*     OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__); */

/*     return res; */
/* } */

/* CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork) */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "unselected network : %d", nonInterestedNetwork); */

/*     if (!g_isInitialized) */
/*     { */
/*         return CA_STATUS_NOT_INITIALIZED; */
/*     } */

/*     CAResult_t res = CA_STATUS_OK; */

/*     if (nonInterestedNetwork & CA_ADAPTER_IP) */
/*     { */
/*         res = CARemoveNetworkType(CA_ADAPTER_IP); */
/*         OIC_LOG_V(DEBUG, TAG, "CARemoveNetworkType(CA_IP_ADAPTER) function returns result : %d", res); */
/*     } */
/*     else if (nonInterestedNetwork & CA_ADAPTER_RFCOMM_BTEDR) */
/*     { */
/*         res = CARemoveNetworkType(CA_ADAPTER_RFCOMM_BTEDR); */
/*         OIC_LOG_V(DEBUG, TAG, "CARemoveNetworkType(CA_RFCOMM_ADAPTER) function returns result : %d", res); */
/*     } */
/*     else if (nonInterestedNetwork & CA_ADAPTER_GATT_BTLE) */
/*     { */
/*         res = CARemoveNetworkType(CA_ADAPTER_GATT_BTLE); */
/*         OIC_LOG_V(DEBUG, TAG, "CARemoveNetworkType(CA_GATT_ADAPTER) function returns result : %d", res); */
/*     } */
/* #ifdef RA_ADAPTER */
/*     else if (nonInterestedNetwork & CA_ADAPTER_REMOTE_ACCESS) */
/*     { */
/*         res = CARemoveNetworkType(CA_ADAPTER_REMOTE_ACCESS); */
/*         OIC_LOG_V(DEBUG, TAG, "CARemoveNetworkType(CA_ADAPTER_REMOTE_ACCESS) function returns result : %d", */
/*                   res); */
/*     } */
/* #endif */


/* #ifdef TCP_ADAPTER */
/*     else if (nonInterestedNetwork & CA_ADAPTER_TCP) */
/*     { */
/*         res = CARemoveNetworkType(CA_ADAPTER_TCP); */
/*         OIC_LOG_V(DEBUG, TAG, "CARemoveNetworkType(CA_ADAPTER_TCP) function returns result : %d", */
/*                   res); */
/*     } */
/* #endif */

/*     else */
/*     { */
/*         res = CA_STATUS_FAILED; */
/*     } */
/*     return res; */
/* } */

/* CAResult_t CAHandleRequestResponse() */
/* { */
/*     if (!g_isInitialized) */
/*     { */
/*         OIC_LOG(ERROR, TAG, "not initialized"); */
/*         return CA_STATUS_NOT_INITIALIZED; */
/*     } */

/*     oocf_handle_inbound_messages(); // @was CAHandleRequestResponseCallbacks */

/*     return CA_STATUS_OK; */
/* } */

#if INTERFACE
#include <inttypes.h>
#endif
/* CASelectCipherSuite => src/sec/oocf_cipher_suites.c */

/* CAEnableAnonECDHCipherSuite => src/sec/oocf_cipher_suites.c */

CAResult_t CAGenerateOwnerPSK(const CAEndpoint_t* endpoint,
                    const uint8_t* label, const size_t labelLen,
                    const uint8_t* rsrcServerDeviceID, const size_t rsrcServerDeviceIDLen,
                    const uint8_t* provServerDeviceID, const size_t provServerDeviceIDLen,
                    uint8_t* ownerPSK, const size_t ownerPskSize)
{
    OIC_LOG(DEBUG, TAG, "IN : CAGenerateOwnerPSK");
    CAResult_t res = CA_STATUS_FAILED;
#if defined (__WITH_DTLS__) || defined(__WITH_TLS__)
    //newOwnerLabel and prevOwnerLabe can be NULL
    if (!endpoint || !label || 0 == labelLen || !ownerPSK || 0 == ownerPskSize)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    res = CAsslGenerateOwnerPsk(endpoint, label, labelLen,
                                      rsrcServerDeviceID, rsrcServerDeviceIDLen,
                                      provServerDeviceID, provServerDeviceIDLen,
                                      ownerPSK, ownerPskSize);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to CAGenerateOwnerPSK : %d", res);
    }
#else
    (void)(endpoint); // prevent unused-parameter compiler warnings
    (void)(label);
    (void)(labelLen);
    (void)(rsrcServerDeviceID);
    (void)(rsrcServerDeviceIDLen);
    (void)(provServerDeviceID);
    (void)(provServerDeviceIDLen);
    (void)(ownerPSK);
    (void)(ownerPskSize);
    OIC_LOG(ERROR, TAG, "Method not supported");
#endif
    OIC_LOG(DEBUG, TAG, "OUT : CAGenerateOwnerPSK");
    return res;
}

CAResult_t CAInitiateHandshake(const CAEndpoint_t *endpoint)
{
    OIC_LOG(DEBUG, TAG, "IN : CAInitiateHandshake");
    CAResult_t res = CA_STATUS_FAILED;
#if defined (__WITH_DTLS__) || defined(__WITH_TLS__)
    if (!endpoint)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    res = CAinitiateSslHandshake(endpoint);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to CAinitiateSslHandshake : %d", res);
    }
#else
    (void)(endpoint); // prevent unused-parameter compiler warning
    OIC_LOG(ERROR, TAG, "Method not supported");
#endif
    OIC_LOG(DEBUG, TAG, "OUT : CAInitiateHandshake");
    return res;
}

CAResult_t CAcloseSslSession(const CAEndpoint_t *endpoint)
{
    OIC_LOG(DEBUG, TAG, "IN : CAcloseSslSession");
    CAResult_t res = CA_STATUS_FAILED;
#if defined (__WITH_DTLS__) || defined(__WITH_TLS__)
    if (!endpoint)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    res = CAcloseSslConnection(endpoint);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to CAsslClose : %d", res);
    }
#else
    (void)(endpoint); // prevent unused-parameter compiler warning
    OIC_LOG(ERROR, TAG, "Method not supported");
#endif
    OIC_LOG(DEBUG, TAG, "OUT : CAcloseSslSession");
    return res;
}
