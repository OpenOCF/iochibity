/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

/* Legacy feature macro. For older versions of glibc < 2.19 */
#define _BSD_SOURCE
/* New feature macro that provides everything _BSD_SOURCE and more glibc >= 2.20 */
#define _DEFAULT_SOURCE

#include "iotivity_config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "ocstack.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "oic_time.h"
#include "logger.h"
#include "utlist.h"
#include "ocpayload.h"

#include "srmresourcestrings.h" //@note: SRM's internal header
#include "doxmresource.h"       //@note: SRM's internal header
#include "pstatresource.h"      //@note: SRM's internal header

#include "pmtypes.h"
#include "pmutility.h"
#include "pmutilityinternal.h"

#include "srmutility.h"

#define TAG ("OIC_PM_UTILITY")

typedef struct _DiscoveryInfo{
    OCProvisionDev_t    **ppDevicesList;
    OCProvisionDev_t    *pCandidateList;
    bool                isOwnedDiscovery;
    bool                isSingleDiscovery;
    bool                isFound;
    const OicUuid_t     *targetId;
} DiscoveryInfo;

/*
 * Function to discover secre port information through unicast
 *
 * @param[in] discoveryInfo The pointer of discovery information to matain result of discovery
 * @param[in] clientResponse  Response information(It will contain payload)
 *
 * @return OC_STACK_OK on success otherwise error.
 */
static OCStackResult SecurePortDiscovery(DiscoveryInfo* discoveryInfo,
                                         const OCClientResponse *clientResponse);

/**
 * Callback handler for PMDeviceDiscovery API.
 *
 * @param[in] ctx             User context
 * @param[in] handle          Handler for response
 * @param[in] clientResponse  Response information (It will contain payload)
 * @return OC_STACK_KEEP_TRANSACTION to keep transaction and
 *         OC_STACK_DELETE_TRANSACTION to delete it.
 */
static OCStackApplicationResult DeviceDiscoveryHandler(void *ctx, OCDoHandle UNUSED,
                                OCClientResponse *clientResponse);

/*
 * Since security version discovery does not used anymore, disable security version discovery.
 * Need to discussion to removing all version discovery related codes.
 */

/*
 * Function to discover security version information through unicast
 *
 * @param[in] discoveryInfo The pointer of discovery information to matain result of discovery
 * @param[in] clientResponse  Response information(It will contain payload)
 *
 * @return OC_STACK_OK on success otherwise error.
 */
static OCStackResult SpecVersionDiscovery(DiscoveryInfo* discoveryInfo,
                                              const OCClientResponse *clientResponse);
#if 0
/**
 * Callback handler for getting secure port information using /oic/res discovery.
 *
 * @param[in] ctx             user context
 * @param[in] handle          Handle for response
 * @param[in] clientResponse  Response information(It will contain payload)
 *
 * @return OC_STACK_KEEP_TRANSACTION to keep transaction and
 *         OC_STACK_DELETE_TRANSACTION to delete it.
 */
static OCStackApplicationResult SecurePortDiscoveryHandler(void *ctx, OCDoHandle UNUSED,
                                 OCClientResponse *clientResponse);
#endif

/*
 * Since security version discovery does not used anymore, disable security version discovery.
 * Need to discussion to removing all version discovery related codes.
 */
#if 0
/**
 * Callback handler for security version discovery.
 *
 * @param[in] ctx             User context
 * @param[in] handle          Handler for response
 * @param[in] clientResponse  Response information (It will contain payload)
 * @return OC_STACK_KEEP_TRANSACTION to keep transaction and
 *         OC_STACK_DELETE_TRANSACTION to delete it.
 */
static OCStackApplicationResult SecVersionDiscoveryHandler(void *ctx, OCDoHandle UNUSED,
                                OCClientResponse *clientResponse);
#endif

#ifdef MULTIPLE_OWNER
OicSecMom_t* CloneOicSecMom(const OicSecMom_t* src)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);

    if (!src)
    {
        OIC_LOG_V(ERROR, TAG, "%s : Invalid parameter", __func__);
        return NULL;
    }

    OicSecMom_t* newMom = (OicSecMom_t*)OICMalloc(sizeof(*newMom));
    if (newMom)
    {
        *newMom = *src;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s : Failed to allocate memory", __func__);
    }

    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);

    return newMom;
}

OicSecSubOwner_t* CloneOicSecSubOwner(const OicSecSubOwner_t* src)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);

    if (!src)
    {
        OIC_LOG_V(ERROR, TAG, "%s : Invalid parameter", __func__);
        return NULL;
    }

    OicSecSubOwner_t* newSubOwner = (OicSecSubOwner_t*)OICCalloc(1, sizeof(*newSubOwner));
    VERIFY_NOT_NULL(TAG, newSubOwner, ERROR);

    memcpy(newSubOwner, src, sizeof(OicSecSubOwner_t));

    if (src->next)
    {
        newSubOwner->next = CloneOicSecSubOwner(src->next);
        VERIFY_NOT_NULL(TAG, newSubOwner->next, ERROR);
    }

    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);

    return newSubOwner;

exit:
    OIC_LOG_V(ERROR, TAG, "%s : Failed to allocate memory", __func__);
    if (newSubOwner)
    {
        OICFree(newSubOwner);
    }
    return NULL;
}
#endif //MULTIPLE_OWNER

OicSecDoxm_t* CloneOicSecDoxm(const OicSecDoxm_t* src)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);

    if (!src)
    {
        OIC_LOG_V(ERROR, TAG, "%s : Invalid parameter", __func__);
        return NULL;
    }

    OicSecDoxm_t* newDoxm = (OicSecDoxm_t*)OICCalloc(1, sizeof(OicSecDoxm_t));
    VERIFY_NOT_NULL(TAG, newDoxm, ERROR);

    memcpy(newDoxm, src, sizeof(OicSecDoxm_t));

#ifdef MULTIPLE_OWNER
    newDoxm->subOwners = NULL;
    newDoxm->mom = NULL;

    if (src->subOwners)
    {
        newDoxm->subOwners = CloneOicSecSubOwner(src->subOwners);
        VERIFY_NOT_NULL(TAG, newDoxm->subOwners, ERROR);
    }

    if (src->mom)
    {
        newDoxm->mom = CloneOicSecMom(src->mom);
        VERIFY_NOT_NULL(TAG, newDoxm->mom, ERROR);
    }
#endif //MULTIPLE_OWNER

    // We have to assign NULL for not necessary information to prevent memory corruption.
    newDoxm->oxm = NULL;
    newDoxm->oxmLen = 0;

    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);

    return newDoxm;

exit:
    OIC_LOG_V(ERROR, TAG, "%s : Failed to allocate memory", __func__);
    DeleteDoxmBinData(newDoxm);

    return NULL;
}

/**
 * Function to search node in linked list that matches given IP and port.
 *
 * @param[in] pList         List of OCProvisionDev_t.
 * @param[in] addr          address of target device.
 * @param[in] port          port of remote server.
 *
 * @return pointer of OCProvisionDev_t if exist, otherwise NULL
 */
OCProvisionDev_t* GetDevice(OCProvisionDev_t **ppDevicesList, const char* addr, const uint16_t port)
{
    if(NULL == addr || NULL == *ppDevicesList)
    {
        OIC_LOG_V(ERROR, TAG, "Invalid Input parameters in [%s]\n", __FUNCTION__);
        return NULL;
    }

    OCProvisionDev_t *ptr = NULL;
    LL_FOREACH(*ppDevicesList, ptr)
    {
        if( strcmp(ptr->endpoint.addr, addr) == 0 && port == ptr->endpoint.port)
        {
            return ptr;
        }
    }

    return NULL;
}


/**
 * Add device information to list.
 *
 * @param[in] pList         List of OCProvisionDev_t.
 * @param[in] endpoint      target device endpoint.
 * @param[in] connType      connectivity type of endpoint
 * @param[in] doxm          pointer to doxm instance.
 *
 * @return OC_STACK_OK for success and error code otherwise.
 */
OCStackResult AddDevice(OCProvisionDev_t **ppDevicesList, OCDevAddr* endpoint,
                        OCConnectivityType connType, OicSecDoxm_t *doxm)
{
    if (NULL == endpoint)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t *ptr = GetDevice(ppDevicesList, endpoint->addr, endpoint->port);
    if(!ptr)
    {
        ptr = (OCProvisionDev_t *)OICCalloc(1, sizeof (OCProvisionDev_t));
        if (NULL == ptr)
        {
            OIC_LOG(ERROR, TAG, "Error while allocating memory for linkedlist node !!");
            return OC_STACK_NO_MEMORY;
        }

        ptr->endpoint = *endpoint;
        ptr->doxm = doxm;
        ptr->securePort = (CT_ADAPTER_GATT_BTLE == connType) ?
                          endpoint->port : DEFAULT_SECURE_PORT;
        ptr->next = NULL;
        ptr->connType = connType;
        ptr->devStatus = DEV_STATUS_ON; //AddDevice is called when discovery(=alive)
        OICStrcpy(ptr->specVer, SPEC_MAX_VER_LEN, DEFAULT_SPEC_VERSION); // version initialization
        ptr->handle = NULL;

        LL_PREPEND(*ppDevicesList, ptr);
    }

    return OC_STACK_OK;
}

/**
 * Move device object between two device lists.
 *
 * @param[in] ppDstDevicesList         Destination list of OCProvisionDev_t.
 * @param[in] ppSrcDevicesList         Source list of OCProvisionDev_t.
 * @param[in] endpoint      target device endpoint.
 *
 * @return OC_STACK_OK for success and error code otherwise.
 */
OCStackResult MoveDeviceList(OCProvisionDev_t **ppDstDevicesList,
                        OCProvisionDev_t **ppSrcDevicesList, OCDevAddr* endpoint)
{
    if (NULL == ppSrcDevicesList || NULL == endpoint)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t *ptr = GetDevice(ppSrcDevicesList, endpoint->addr, endpoint->port);
    if(ptr)
    {
        LL_DELETE(*ppSrcDevicesList, ptr);
        LL_PREPEND(*ppDstDevicesList, ptr);
        OIC_LOG_V(DEBUG, TAG, "MoveDeviceList success : %s(%d)", endpoint->addr, endpoint->port);
        return OC_STACK_OK;
    }

    return OC_STACK_ERROR;
}

/**
 * Function to set secure port information from the given list of devices.
 *
 * @param[in] pList         List of OCProvisionDev_t.
 * @param[in] addr          address of target device.
 * @param[in] port          port of remote server.
 * @param[in] secureport    secure port information.
 *
 * @return OC_STACK_OK for success and errorcode otherwise.
 */
static OCStackResult UpdateSecurePortOfDevice(OCProvisionDev_t **ppDevicesList, const char *addr,
                                       uint16_t port, uint16_t securePort
#ifdef __WITH_TLS__
                                       ,uint16_t tcpPort
                                       ,uint16_t tcpSecurePort
#endif
                                       )
{
    OCProvisionDev_t *ptr = GetDevice(ppDevicesList, addr, port);

    if(!ptr)
    {
        OIC_LOG(ERROR, TAG, "Can not find device information in the discovery device list");
        return OC_STACK_ERROR;
    }

    ptr->securePort = (OC_ADAPTER_GATT_BTLE == ptr->endpoint.adapter) ?
                      ptr->endpoint.port : securePort;

#ifdef __WITH_TLS__
    ptr->tcpPort = tcpPort;
    ptr->tcpSecurePort = tcpSecurePort;
#endif

    return OC_STACK_OK;
}

/**
 * Function to set security version information from the given list of devices.
 *
 * @param[in] pList         List of OCProvisionDev_t.
 * @param[in] addr          address of target device.
 * @param[in] port          port of remote server.
 * @param[in] specVer    security version information.
 *
 * @return OC_STACK_OK for success and errorcode otherwise.
 */
static OCStackResult UpdateSpecVersionOfDevice(OCProvisionDev_t **ppDevicesList, const char *addr,
                                       uint16_t port, const char* specVer)
{
    if (NULL == specVer)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t *ptr = GetDevice(ppDevicesList, addr, port);

    if(!ptr)
    {
        OIC_LOG(ERROR, TAG, "Can not find device information in the discovery device list");
        return OC_STACK_ERROR;
    }

    OICStrcpy(ptr->specVer, SPEC_MAX_VER_LEN, specVer);

    return OC_STACK_OK;
}

/**
 * This function deletes list of provision target devices
 *
 * @param[in] pDevicesList         List of OCProvisionDev_t.
 */
void PMDeleteDeviceList(OCProvisionDev_t *pDevicesList)
{
    if(pDevicesList)
    {
        OCProvisionDev_t *del = NULL, *tmp = NULL;
        LL_FOREACH_SAFE(pDevicesList, del, tmp)
        {
            LL_DELETE(pDevicesList, del);

            DeleteDoxmBinData(del->doxm);
            del->doxm = NULL;
            DeletePstatBinData(del->pstat);
            del->pstat = NULL;
            OICFree(del);
            del = NULL;
        }
    }
}

OCProvisionDev_t* PMCloneOCProvisionDev(const OCProvisionDev_t* src)
{
    OIC_LOG(DEBUG, TAG, "IN PMCloneOCProvisionDev");

    if (!src)
    {
        OIC_LOG(ERROR, TAG, "PMCloneOCProvisionDev : Invalid parameter");
        return NULL;
    }

    // TODO: Consider use VERIFY_NON_NULL instead of if ( null check ) { goto exit; }
    OCProvisionDev_t* newDev = (OCProvisionDev_t*)OICCalloc(1, sizeof(OCProvisionDev_t));
    VERIFY_NOT_NULL(TAG, newDev, ERROR);

    memcpy(&newDev->endpoint, &src->endpoint, sizeof(OCDevAddr));

    if (src->pstat)
    {
        newDev->pstat= (OicSecPstat_t*)OICCalloc(1, sizeof(OicSecPstat_t));
        VERIFY_NOT_NULL(TAG, newDev->pstat, ERROR);

        memcpy(newDev->pstat, src->pstat, sizeof(OicSecPstat_t));
        // We have to assign NULL for not necessary information to prevent memory corruption.
        newDev->pstat->sm = NULL;
    }

    if (src->doxm)
    {
        newDev->doxm = CloneOicSecDoxm(src->doxm);
        VERIFY_NOT_NULL(TAG, newDev->doxm, ERROR);
    }

    if (0 == strlen(src->specVer))
    {
        OICStrcpy(newDev->specVer, SPEC_MAX_VER_LEN, DEFAULT_SPEC_VERSION);
    }
    else
    {
        OICStrcpy(newDev->specVer, SPEC_MAX_VER_LEN, src->specVer);
    }

    newDev->securePort = src->securePort;
    newDev->devStatus = src->devStatus;
    newDev->connType = src->connType;
    newDev->next = NULL;

    OIC_LOG(DEBUG, TAG, "OUT PMCloneOCProvisionDev");

    return newDev;

exit:
    OIC_LOG(ERROR, TAG, "PMCloneOCProvisionDev : Failed to allocate memory");
    PMDeleteDeviceList(newDev);
    return NULL;
}

OCProvisionDev_t* PMCloneOCProvisionDevList(const OCProvisionDev_t* src)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);

    if (!src)
    {
        OIC_LOG_V(ERROR, TAG, "%s : Invalid parameter", __func__);
        return NULL;
    }

    OCProvisionDev_t* newDev = PMCloneOCProvisionDev(src);
    VERIFY_NOT_NULL(TAG, newDev, ERROR);

    OCProvisionDev_t* current = newDev;
    for (OCProvisionDev_t* next = src->next; NULL != next; next = next->next)
    {
        current->next = PMCloneOCProvisionDev(next);
        VERIFY_NOT_NULL(TAG, current->next, ERROR);

        current = current->next;
    }

    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);

    return newDev;

exit:
    OIC_LOG_V(ERROR, TAG, "%s : Failed to allocate memory", __func__);
    PMDeleteDeviceList(newDev);
    return NULL;
}

/**
 * Timeout implementation for secure discovery. When performing secure discovery,
 * we should wait a certain period of time for getting response of each devices.
 *
 * @param[in]  waittime  Timeout in seconds.
 * @param[in]  waitForStackResponse if true timeout function will call OCProcess while waiting.
 * @return OC_STACK_OK on success otherwise error.
 */
OCStackResult PMTimeout(unsigned short waittime, bool waitForStackResponse)
{
    OCStackResult res = OC_STACK_OK;

    uint64_t startTime = OICGetCurrentTime(TIME_IN_MS);
    while (OC_STACK_OK == res)
    {
        uint64_t currTime = OICGetCurrentTime(TIME_IN_MS);

        long elapsed = (long)((currTime - startTime) / MS_PER_SEC);
        if (elapsed > waittime)
        {
            return OC_STACK_OK;
        }
        if (waitForStackResponse)
        {
            res = OCProcess();
        }
    }
    return res;
}

bool OC_CALL PMGenerateQuery(bool isSecure,
                             const char* address, uint16_t port,
                             OCConnectivityType connType,
                             char* buffer, size_t bufferSize, const char* uri)
{
    if(!address || !buffer || !uri)
    {
        OIC_LOG(ERROR, TAG, "PMGenerateQuery : Invalid parameters.");
        return false;
    }

    int snRet = 0;
    char* prefix = (isSecure == true) ? COAPS_PREFIX : COAP_PREFIX;

    switch(connType & CT_MASK_ADAPTER)
    {
        case CT_ADAPTER_TCP:
            prefix = (isSecure == true) ? COAPS_TCP_PREFIX : COAP_TCP_PREFIX;
        case CT_ADAPTER_IP:
            switch(connType & CT_MASK_FLAGS & ~CT_FLAG_SECURE)
            {
                case CT_IP_USE_V4:
                    snRet = snprintf(buffer, bufferSize, "%s%s:%d%s",
                                     prefix, address, port, uri);
                    break;
                case CT_IP_USE_V6:
                {
                    char addressEncoded[128] = {0};

                    OCStackResult result = OCEncodeAddressForRFC6874(addressEncoded,
                                                                     sizeof(addressEncoded),
                                                                     address);
                    if (OC_STACK_OK != result)
                    {
                        OIC_LOG_V(ERROR, TAG, "PMGenerateQuery : encoding error %d\n", result);
                        return false;
                    }

                    snRet = snprintf(buffer, bufferSize, "%s[%s]:%d%s",
                                     prefix, addressEncoded, port, uri);
                    break;
                }
                default:
                    OIC_LOG(ERROR, TAG, "Unknown address format.");
                    return false;
            }
            break;
        case CT_ADAPTER_GATT_BTLE:
            snRet = snprintf(buffer, bufferSize, "%s%s%s",
                             prefix, address, uri);
            break;
        case CT_ADAPTER_RFCOMM_BTEDR:
            OIC_LOG(ERROR, TAG, "Not supported connectivity adapter.");
            return false;
        default:
            OIC_LOG(ERROR, TAG, "Unknown connectivity adapter.");
            return false;
    }

    // snprintf return value check
    if (snRet < 0)
    {
        OIC_LOG_V(ERROR, TAG, "PMGenerateQuery : Error (snprintf) %d\n", snRet);
        return false;
    }
    else if ((size_t)snRet >= bufferSize)
    {
        OIC_LOG_V(ERROR, TAG, "PMGenerateQuery : Truncated (snprintf) %d\n", snRet);
        return false;
    }

    return true;
}

/*
 * Since security version discovery does not used anymore, disable security version discovery.
 * Need to discussion to removing all version discovery related codes.
 */
static OCStackApplicationResult SpecVersionDiscoveryHandler(void *ctx, OCDoHandle UNUSED,
                                OCClientResponse *clientResponse)
{
    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG, "Lost List of device information");
        return OC_STACK_KEEP_TRANSACTION;
    }
    (void)UNUSED;
    if (clientResponse)
    {
        if  (NULL == clientResponse->payload)
        {
            OIC_LOG(INFO, TAG, "Skiping Null payload");
            return OC_STACK_KEEP_TRANSACTION;
        }
        if (OC_STACK_OK != clientResponse->result)
        {
            OIC_LOG(INFO, TAG, "Error in response");
            return OC_STACK_KEEP_TRANSACTION;
        }
        else
        {
            if (PAYLOAD_TYPE_REPRESENTATION != clientResponse->payload->type)
            {
                OIC_LOG(INFO, TAG, "Unknown payload type");
                return OC_STACK_KEEP_TRANSACTION;
            }
            OCRepPayloadValue* val = ((OCRepPayload*) clientResponse->payload)->values;

            char specVer[SPEC_MAX_VER_LEN + 1] = {0};
            OICStrcpy(specVer, SPEC_MAX_VER_LEN, DEFAULT_SPEC_VERSION);
            while (val)
            {
                if (val->type == OCREP_PROP_STRING)
                {
                    OIC_LOG_V(DEBUG, TAG, "\t\t%s:%s", val->name, val->str);
                    if (0 == strcmp(val->name, OC_RSRVD_SPEC_VERSION))
                    {
                        OICStrcpy(specVer, SPEC_MAX_VER_LEN, val->str);
                        break;
                    }
                }
                val = val -> next;
            }
            //If this is owend device discovery we have to filter out the responses.
            DiscoveryInfo* pDInfo = (DiscoveryInfo*)ctx;
            OCStackResult res = UpdateSpecVersionOfDevice(pDInfo->ppDevicesList, clientResponse->devAddr.addr,
                                                     clientResponse->devAddr.port, specVer);
            if (OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, TAG, "Error while getting security version.");
                return OC_STACK_KEEP_TRANSACTION;
            }

            OIC_LOG(INFO, TAG, "= Discovered security version =");
            OIC_LOG_V(DEBUG, TAG, "IP %s", clientResponse->devAddr.addr);
            OIC_LOG_V(DEBUG, TAG, "PORT %d", clientResponse->devAddr.port);
            OIC_LOG_V(DEBUG, TAG, "VERSION %s", specVer);
        }
    }
    else
    {
        OIC_LOG(INFO, TAG, "Skiping Null response");
        return OC_STACK_KEEP_TRANSACTION;
    }

    return  OC_STACK_DELETE_TRANSACTION;
}

static OCStackApplicationResult SecurePortDiscoveryHandler(void *ctx, OCDoHandle UNUSED,
                                 OCClientResponse *clientResponse)
{
    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG, "Lost List of device information");
        return OC_STACK_DELETE_TRANSACTION;
    }
    (void)UNUSED;
    if (clientResponse)
    {
        if  (NULL == clientResponse->payload)
        {
            OIC_LOG(INFO, TAG, "Skiping Null payload");
        }
        else
        {
            if (PAYLOAD_TYPE_DISCOVERY != clientResponse->payload->type)
            {
                OIC_LOG(INFO, TAG, "Wrong payload type");
                return OC_STACK_DELETE_TRANSACTION;
            }

            uint16_t securePort = 0;
#ifdef __WITH_TLS__
            uint16_t tcpPort = 0;
            uint16_t tcpSecurePort = 0;
#endif
            OCResourcePayload* resPayload = ((OCDiscoveryPayload*)clientResponse->payload)->resources;

            // Use seure port of doxm for OTM and Provision.
            while (resPayload)
            {
                if (0 == strncmp(resPayload->uri, OIC_RSRC_DOXM_URI, strlen(OIC_RSRC_DOXM_URI)))
                {
                    OIC_LOG_V(INFO,TAG,"resPaylod->uri:%s",resPayload->uri);
                    OIC_LOG(INFO, TAG, "Found doxm resource.");
                    break;
                }
                else
                {
                    resPayload = resPayload->next;
                }
            }
            if (NULL == resPayload)
            {
                OIC_LOG(ERROR, TAG, "Can not find doxm resource.");
                return OC_STACK_DELETE_TRANSACTION;
            }
            if (resPayload && resPayload->secure)
            {
                securePort = resPayload->port;
            }
            else if (resPayload && resPayload->eps)
            {
                OCEndpointPayload* eps = resPayload->eps;
                while (eps != NULL)
                {
                    if ((eps->family & OC_FLAG_SECURE) &&
                        ((OC_IP_USE_V6 == clientResponse->devAddr.flags &&
                          strchr(eps->addr, ':')) ||
                         (OC_IP_USE_V4 == clientResponse->devAddr.flags &&
                          strchr(eps->addr, '.'))))
                    {
                            securePort = eps->port;
                            break;
                    }
                    eps = eps->next;
                }
#ifdef __WITH_TLS__
                eps = resPayload->eps;
                while (eps != NULL)
                {
                    if ((eps->family & OC_FLAG_SECURE) &&
                        ((OC_IP_USE_V6 == clientResponse->devAddr.flags &&
                          strchr(eps->addr, ':')) ||
                         (OC_IP_USE_V4 == clientResponse->devAddr.flags &&
                          strchr(eps->addr, '.'))) &&
                        0 == strncmp(eps->tps, COAPS_TCP_PREFIX, strlen(COAPS_TCP_PREFIX)-3))
                    {
                            tcpSecurePort = eps->port;
                            break;
                    }
                    eps = eps->next;
                }
                eps = resPayload->eps;
                while (eps != NULL)
                {
                    if(((OC_IP_USE_V6 == clientResponse->devAddr.flags && strchr(eps->addr, ':')) ||
                        (OC_IP_USE_V4 == clientResponse->devAddr.flags && strchr(eps->addr, '.'))) &&
                        0 == strncmp(eps->tps, COAP_TCP_PREFIX, strlen(COAP_TCP_PREFIX)-3)
                      )
                    {
                        tcpPort =  eps->port;
                        break;
                    }
                    eps = eps->next;
                }
#endif
                if (!securePort)
                {
                    OIC_LOG(INFO, TAG, "Can not find secure port information.");
                    return OC_STACK_DELETE_TRANSACTION;
                }
                else
                {
                    OIC_LOG_V(INFO, TAG, "%s: secure port: %d", __func__, securePort);
                }
            }
            else
            {
                OIC_LOG(INFO, TAG, "Can not find secure port information.");
                return OC_STACK_DELETE_TRANSACTION;
            }
#ifdef __WITH_TLS__
            OIC_LOG_V(DEBUG, TAG, "%s: TCP port from discovery = %d", __func__, resPayload->tcpPort);
#endif
            DiscoveryInfo* pDInfo = (DiscoveryInfo*)ctx;
            OCProvisionDev_t *ptr = GetDevice(&pDInfo->pCandidateList,
                                              clientResponse->devAddr.addr,
                                              clientResponse->devAddr.port);
            if(!ptr)
            {
                OIC_LOG(ERROR, TAG, "Can not find device information in the discovery candidate device list");
                return OC_STACK_DELETE_TRANSACTION;
            }

            OCStackResult res = UpdateSecurePortOfDevice(&pDInfo->pCandidateList,
                                                         clientResponse->devAddr.addr,
                                                         clientResponse->devAddr.port,
                                                         securePort
#ifdef __WITH_TLS__
                                                         ,tcpPort
                                                         ,tcpSecurePort
#endif
                                                         );
            if (OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, TAG, "Error while getting secure port.");
                return OC_STACK_DELETE_TRANSACTION;
            }

            res = MoveDeviceList(pDInfo->ppDevicesList, &pDInfo->pCandidateList, &clientResponse->devAddr);
            if(OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, TAG, "Error while move the discovered device to list.");
                return OC_STACK_DELETE_TRANSACTION;
            }

            if(pDInfo->isSingleDiscovery)
            {
                pDInfo->isFound = true;
            }
            else
            {
                res = SpecVersionDiscovery(pDInfo, clientResponse);
                if(OC_STACK_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "Failed to SpecVersionDiscovery");
                    return OC_STACK_DELETE_TRANSACTION;
                }
            }

            OIC_LOG(INFO, TAG, "Exiting SecurePortDiscoveryHandler.");
        }

        return  OC_STACK_DELETE_TRANSACTION;
    }
    else
    {
        OIC_LOG(INFO, TAG, "Skiping Null response");
    }

    return  OC_STACK_DELETE_TRANSACTION;
}

static OCStackApplicationResult DeviceDiscoveryHandler(void *ctx, OCDoHandle UNUSED,
                                OCClientResponse *clientResponse)
{
    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG, "Lost List of device information");
        return OC_STACK_KEEP_TRANSACTION;
    }
    (void)UNUSED;

    if (!clientResponse)
    {
        OIC_LOG(INFO, TAG, "Skiping Null response");
        return OC_STACK_KEEP_TRANSACTION;
    }

    if  (NULL == clientResponse->payload)
    {
        OIC_LOG(INFO, TAG, "Skiping Null payload");
        return OC_STACK_KEEP_TRANSACTION;
    }
    if (OC_STACK_OK != clientResponse->result)
    {
        OIC_LOG(INFO, TAG, "Error in response");
        return OC_STACK_KEEP_TRANSACTION;
    }

    if (PAYLOAD_TYPE_SECURITY != clientResponse->payload->type)
    {
        OIC_LOG(INFO, TAG, "Unknown payload type");
        return OC_STACK_KEEP_TRANSACTION;
    }

    OicSecDoxm_t *ptrDoxm = NULL;
    uint8_t *payload = ((OCSecurityPayload*)clientResponse->payload)->securityData;
    size_t size = ((OCSecurityPayload*)clientResponse->payload)->payloadSize;

    OCStackResult res = CBORPayloadToDoxm(payload, size, &ptrDoxm);
    if ((NULL == ptrDoxm) || (OC_STACK_OK != res))
    {
        OIC_LOG(INFO, TAG, "Ignoring malformed CBOR");
        return OC_STACK_KEEP_TRANSACTION;
    }

    OIC_LOG(DEBUG, TAG, "Successfully converted doxm cbor to bin.");

    //If this is owend device discovery we have to filter out the responses.
    DiscoveryInfo* pDInfo = (DiscoveryInfo*)ctx;
    OCProvisionDev_t **ppDevicesList = &pDInfo->pCandidateList;

    // Get my device ID from doxm resource
    OicUuid_t myId;
    memset(&myId, 0, sizeof(myId));
    res = GetDoxmDeviceID(&myId);
    if(OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Error while getting my device ID.");
        DeleteDoxmBinData(ptrDoxm);
        return OC_STACK_KEEP_TRANSACTION;
    }

    // If this is owned discovery response but owner is not me then discard it.
    if( (pDInfo->isOwnedDiscovery) &&
        (0 != memcmp(&ptrDoxm->owner.id, &myId.id, sizeof(myId.id))) )
    {
        OIC_LOG(DEBUG, TAG, "Discovered device is not owend by me");
        DeleteDoxmBinData(ptrDoxm);
        return OC_STACK_KEEP_TRANSACTION;
    }

    //if targetId and discovered deviceID are different, discard it
    if ((pDInfo->isSingleDiscovery) &&
        (0 != memcmp(&ptrDoxm->deviceID.id, &pDInfo->targetId->id, sizeof(pDInfo->targetId->id))) )
    {
        OIC_LOG(DEBUG, TAG, "Discovered device is not target device");
        DeleteDoxmBinData(ptrDoxm);
        return OC_STACK_KEEP_TRANSACTION;
    }
    //If self reply, discard it
    if (0 == memcmp(&ptrDoxm->deviceID.id, &myId.id, sizeof(myId.id)))
    {
        OIC_LOG(DEBUG, TAG, "discarding provision tool's reply");
        DeleteDoxmBinData(ptrDoxm);
        return OC_STACK_KEEP_TRANSACTION;
    }

    res = AddDevice(ppDevicesList, &clientResponse->devAddr,
            clientResponse->connType, ptrDoxm);
    if (OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Error while adding data to linkedlist.");
        DeleteDoxmBinData(ptrDoxm);
        return OC_STACK_KEEP_TRANSACTION;
    }

    res = SecurePortDiscovery(pDInfo, clientResponse);
    if(OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to SecurePortDiscovery");
        DeleteDoxmBinData(ptrDoxm);
        return OC_STACK_KEEP_TRANSACTION;
    }

    OIC_LOG(INFO, TAG, "Exiting ProvisionDiscoveryHandler.");

    return  OC_STACK_KEEP_TRANSACTION;
}

static void DeviceDiscoveryDeleteHandler(void *ctx)
{
    OIC_LOG(DEBUG, TAG, "IN DeviceDiscoveryDeleteHandler");
    if (NULL == ctx)
    {
        OIC_LOG(WARNING, TAG, "Not found context in DeviceDiscoveryDeleteHandler");
        return;
    }

    DiscoveryInfo* pDInfo = (DiscoveryInfo*)ctx;
    if (NULL != pDInfo->pCandidateList)
    {
        OCProvisionDev_t *pDev = NULL;
        LL_FOREACH(pDInfo->pCandidateList, pDev)
        {
            OIC_LOG_V(DEBUG, TAG, "OCCancel - %s : %d",
                            pDev->endpoint.addr, pDev->endpoint.port);
            if(OC_STACK_OK !=  OCCancel(pDev->handle,OC_HIGH_QOS,NULL,0))
            {
                OIC_LOG(ERROR, TAG, "Failed to remove registered callback");
            }
        }
        PMDeleteDeviceList(pDInfo->pCandidateList);
    }
    OIC_LOG(DEBUG, TAG, "OUT DeviceDiscoveryDeleteHandler");
}

/**
 * Discover owned/unowned device in the specified endpoint/deviceID.
 * It will return the found device even though timeout is not exceeded.
 *
 * @param[in] waittime           Timeout in seconds
 * @param[in] deviceID           deviceID of target device.
 * @param[out] ppFoundDevice     OCProvisionDev_t of found device
 *
 * @return OC_STACK_OK on success otherwise error.\n
 *         OC_STACK_INVALID_PARAM when deviceID is NULL or ppFoundDevice is not initailized.
 */
OCStackResult PMSingleDeviceDiscovery(unsigned short waittime, const OicUuid_t* deviceID,
                                 OCProvisionDev_t **ppFoundDevice)
{
    OIC_LOG(DEBUG, TAG, "IN PMSingleDeviceDiscovery");

    if (NULL != *ppFoundDevice)
    {
        OIC_LOG(ERROR, TAG, "List is not null can cause memory leak");
        return OC_STACK_INVALID_PARAM;
    }

    if (NULL == deviceID)
    {
        OIC_LOG(ERROR, TAG, "Invalid device ID");
        return OC_STACK_INVALID_PARAM;
    }


    DiscoveryInfo *pDInfo = OICCalloc(1, sizeof(DiscoveryInfo));
    if(NULL == pDInfo)
    {
        OIC_LOG(ERROR, TAG, "PMSingleDeviceDiscovery : Memory allocation failed.");
        return OC_STACK_NO_MEMORY;
    }

    pDInfo->ppDevicesList = ppFoundDevice;
    pDInfo->pCandidateList = NULL;
    pDInfo->isOwnedDiscovery = false;
    pDInfo->isSingleDiscovery = true;
    pDInfo->isFound = false;
    pDInfo->targetId = deviceID;

    OCCallbackData cbData;
    cbData.cb = &DeviceDiscoveryHandler;
    cbData.context = (void *)pDInfo;
    cbData.cd = &DeviceDiscoveryDeleteHandler;

    OCStackResult res = OC_STACK_ERROR;

    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH + 1] = { '\0' };
    snprintf(query, MAX_URI_LENGTH + MAX_QUERY_LENGTH + 1, "/oic/sec/doxm");

    OCDoHandle handle = NULL;
    res = OCDoResource(&handle, OC_REST_DISCOVER, query, 0, 0,
                                     CT_DEFAULT, OC_HIGH_QOS, &cbData, NULL, 0);
    if (res != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
        OICFree(pDInfo);
        return res;
    }

    //Waiting for each response.
    res = OC_STACK_OK;
    uint64_t startTime = OICGetCurrentTime(TIME_IN_MS);
    while (OC_STACK_OK == res && !pDInfo->isFound)
    {
        uint64_t currTime = OICGetCurrentTime(TIME_IN_MS);

        long elapsed = (long)((currTime - startTime) / MS_PER_SEC);
        if (elapsed > waittime)
        {
            break;
        }
        res = OCProcess();
    }

    if(OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to wait response for secure discovery.");
        OICFree(pDInfo);
        OCStackResult resCancel = OCCancel(handle, OC_HIGH_QOS, NULL, 0);
        if(OC_STACK_OK !=  resCancel)
        {
            OIC_LOG(ERROR, TAG, "Failed to remove registered callback");
        }
        return res;
    }

    res = OCCancel(handle,OC_HIGH_QOS,NULL,0);
    if (OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to remove registered callback");
        OICFree(pDInfo);
        return res;
    }
    OIC_LOG(DEBUG, TAG, "OUT PMSingleDeviceDiscovery");
    OICFree(pDInfo);
    return res;
}


/**
 * Discover owned/unowned devices in the same IP subnet. .
 *
 * @param[in] waittime      Timeout in seconds.
 * @param[in] isOwned       bool flag for owned / unowned discovery
 * @param[in] ppDevicesList        List of OCProvisionDev_t.
 *
 * @return OC_STACK_OK on success otherwise error.
 */
OCStackResult PMDeviceDiscovery(unsigned short waittime, bool isOwned, OCProvisionDev_t **ppDevicesList)
{
    OIC_LOG(DEBUG, TAG, "IN PMDeviceDiscovery");

    if (NULL != *ppDevicesList)
    {
        OIC_LOG(ERROR, TAG, "List is not null can cause memory leak");
        return OC_STACK_INVALID_PARAM;
    }

    const char DOXM_OWNED_FALSE_MULTICAST_QUERY[] = "/oic/sec/doxm?Owned=FALSE";
    const char DOXM_OWNED_TRUE_MULTICAST_QUERY[] = "/oic/sec/doxm?Owned=TRUE";

    DiscoveryInfo *pDInfo = OICCalloc(1, sizeof(DiscoveryInfo));
    if(NULL == pDInfo)
    {
        OIC_LOG(ERROR, TAG, "PMDeviceDiscovery : Memory allocation failed.");
        return OC_STACK_NO_MEMORY;
    }

    pDInfo->ppDevicesList = ppDevicesList;
    pDInfo->pCandidateList = NULL;
    pDInfo->isOwnedDiscovery = isOwned;
    pDInfo->isSingleDiscovery = false;
    pDInfo->targetId = NULL;

    OCCallbackData cbData;
    cbData.cb = &DeviceDiscoveryHandler;
    cbData.context = (void *)pDInfo;
    cbData.cd = &DeviceDiscoveryDeleteHandler;
    OCStackResult res = OC_STACK_ERROR;

    const char* query = isOwned ? DOXM_OWNED_TRUE_MULTICAST_QUERY :
                                  DOXM_OWNED_FALSE_MULTICAST_QUERY;

    OCDoHandle handle = NULL;
    res = OCDoResource(&handle, OC_REST_DISCOVER, query, 0, 0,
                                     CT_DEFAULT, OC_HIGH_QOS, &cbData, NULL, 0);
    if (res != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
        OICFree(pDInfo);
        return res;
    }

    //Waiting for each response.
    res = PMTimeout(waittime, true);
    if(OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to wait response for secure discovery.");
        OICFree(pDInfo);
        OCStackResult resCancel = OCCancel(handle, OC_HIGH_QOS, NULL, 0);
        if(OC_STACK_OK !=  resCancel)
        {
            OIC_LOG(ERROR, TAG, "Failed to remove registered callback");
        }
        return res;
    }
    res = OCCancel(handle,OC_HIGH_QOS,NULL,0);
    if (OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to remove registered callback");
        OICFree(pDInfo);
        return res;
    }
    OIC_LOG(DEBUG, TAG, "OUT PMDeviceDiscovery");
    OICFree(pDInfo);
    return res;
}

OCStackResult PMSingleDeviceDiscoveryInUnicast(unsigned short waittime, const OicUuid_t* deviceID,
                                 const char* hostAddress, OCConnectivityType connType,
                                 OCProvisionDev_t **ppFoundDevice)
{
    OIC_LOG(DEBUG, TAG, "IN PMSingleDeviceDiscoveryInUnicast");

    if (NULL != *ppFoundDevice)
    {
        OIC_LOG(ERROR, TAG, "List is not null can cause memory leak");
        return OC_STACK_INVALID_PARAM;
    }

    if (NULL == deviceID)
    {
        OIC_LOG(ERROR, TAG, "Invalid device ID");
        return OC_STACK_INVALID_PARAM;
    }

    DiscoveryInfo *pDInfo = (DiscoveryInfo*)OICCalloc(1, sizeof(DiscoveryInfo));
    if (NULL == pDInfo)
    {
        OIC_LOG(ERROR, TAG, "PMSingleDeviceDiscoveryInUnicast : Memory allocation failed.");
        return OC_STACK_NO_MEMORY;
    }

    pDInfo->ppDevicesList = ppFoundDevice;
    pDInfo->pCandidateList = NULL;
    pDInfo->isOwnedDiscovery = false;
    pDInfo->isSingleDiscovery = true;
    pDInfo->isFound = false;
    pDInfo->targetId = deviceID;

    OCCallbackData cbData;
    cbData.cb = &DeviceDiscoveryHandler;
    cbData.context = (void *)pDInfo;
    cbData.cd = &DeviceDiscoveryDeleteHandler;

    OCStackResult res = OC_STACK_ERROR;

    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH + 1] = { '\0' };
    if (hostAddress == NULL)
    {
        hostAddress = "";
    }
    snprintf(query, MAX_URI_LENGTH + MAX_QUERY_LENGTH + 1, "%s/oic/sec/doxm", hostAddress);
    connType = connType & CT_MASK_ADAPTER;

    OCDoHandle handle = NULL;
    res = OCDoResource(&handle, OC_REST_DISCOVER, query, 0, 0,
            connType, OC_HIGH_QOS, &cbData, NULL, 0);

    if (res != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
        OICFree(pDInfo);
        pDInfo = NULL;
        return res;
    }

    res = OC_STACK_OK;
    uint64_t startTime = OICGetCurrentTime(TIME_IN_MS);
    while (OC_STACK_OK == res && !pDInfo->isFound)
    {
        uint64_t currTime = OICGetCurrentTime(TIME_IN_MS);

        long elapsed = (long)((currTime - startTime) / MS_PER_SEC);
        if (elapsed > waittime)
        {
            break;
        }
        res = OCProcess();
    }

    if (OC_STACK_OK != res)
    {
        OIC_LOG (ERROR, TAG, "Failed to wait response for secure discovery.");
        OICFree(pDInfo);
        pDInfo = NULL;
        OCStackResult resCancel = OCCancel(handle, OC_HIGH_QOS, NULL, 0);
        if (OC_STACK_OK !=  resCancel)
        {
            OIC_LOG(ERROR, TAG, "Failed to remove registered callback");
        }
        return res;
    }

    res = OCCancel(handle, OC_HIGH_QOS, NULL, 0);
    if (OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to remove registered callback");
        OICFree(pDInfo);
        pDInfo = NULL;
        return res;
    }
    OIC_LOG(DEBUG, TAG, "OUT PMSingleDeviceDiscoveryInUnicast");
    OICFree(pDInfo);
    pDInfo = NULL;
    return res;
}

#ifdef MULTIPLE_OWNER

static const unsigned int IOTIVITY_USECS_PER_MSEC = 1000;
extern int MOTIsSupportedOnboardingType(OicSecDoxm_t *ptrDoxm);

static OCStackApplicationResult MOTDeviceDiscoveryHandler(void *ctx, OCDoHandle UNUSED,
                                OCClientResponse *clientResponse)
{
    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG, "Lost List of device information");
        return OC_STACK_KEEP_TRANSACTION;
    }
    (void)UNUSED;
    if (clientResponse)
    {
        if  (NULL == clientResponse->payload)
        {
            OIC_LOG(INFO, TAG, "Skipping Null payload");
            return OC_STACK_KEEP_TRANSACTION;
        }
        if (OC_STACK_OK != clientResponse->result)
        {
            OIC_LOG(INFO, TAG, "Error in response");
            return OC_STACK_KEEP_TRANSACTION;
        }
        else
        {
            if (PAYLOAD_TYPE_SECURITY != clientResponse->payload->type)
            {
                OIC_LOG(INFO, TAG, "Unknown payload type");
                return OC_STACK_KEEP_TRANSACTION;
            }

            OicSecDoxm_t *ptrDoxm = NULL;
            uint8_t *payload = ((OCSecurityPayload*)clientResponse->payload)->securityData;
            size_t size = ((OCSecurityPayload*)clientResponse->payload)->payloadSize;

            OCStackResult res = CBORPayloadToDoxm(payload, size, &ptrDoxm);
            if ((NULL == ptrDoxm) || (OC_STACK_OK != res))
            {
                OIC_LOG(INFO, TAG, "Ignoring malformed CBOR");
                return OC_STACK_KEEP_TRANSACTION;
            }
            else
            {
                OIC_LOG(DEBUG, TAG, "Successfully converted doxm cbor to bin.");

                //If this is owend device discovery we have to filter out the responses.
                DiscoveryInfo* pDInfo = (DiscoveryInfo*)ctx;
                OCProvisionDev_t **ppDevicesList = &pDInfo->pCandidateList;

                // Get my device ID from doxm resource
                OicUuid_t myId;
                memset(&myId, 0, sizeof(myId));

                res = GetDoxmDeviceID(&myId);
                if(OC_STACK_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "Error while getting my device ID.");
                    DeleteDoxmBinData(ptrDoxm);
                    return OC_STACK_KEEP_TRANSACTION;
                }

                //if targetId and discovered deviceID are different, discard it
                if ((pDInfo->isSingleDiscovery) &&
                    (0 != memcmp(&ptrDoxm->deviceID.id, &pDInfo->targetId->id, sizeof(pDInfo->targetId->id))))
                {
                    OIC_LOG(DEBUG, TAG, "Discovered device is not target device");
                    DeleteDoxmBinData(ptrDoxm);
                    return OC_STACK_KEEP_TRANSACTION;
                }

                //if this is owned discovery and this is PT's reply, discard it
                if (((pDInfo->isSingleDiscovery) || (pDInfo->isOwnedDiscovery)) &&
                    (0 == memcmp(&ptrDoxm->deviceID.id, &myId.id, sizeof(myId.id))))
                {
                    OIC_LOG(DEBUG, TAG, "discarding provision tool's reply");
                    DeleteDoxmBinData(ptrDoxm);
                    return OC_STACK_KEEP_TRANSACTION;
                }

                if(pDInfo->isOwnedDiscovery)
                {
                    OicSecSubOwner_t* subOwner = NULL;
                    LL_FOREACH(ptrDoxm->subOwners, subOwner)
                    {
                        if(memcmp(myId.id, subOwner->uuid.id, sizeof(myId.id)) == 0)
                        {
                            break;
                        }
                    }

                    if(subOwner)
                    {
                        res = AddDevice(ppDevicesList, &clientResponse->devAddr,
                                clientResponse->connType, ptrDoxm);
                        if (OC_STACK_OK != res)
                        {
                            OIC_LOG(ERROR, TAG, "Error while adding data to linkedlist.");
                            DeleteDoxmBinData(ptrDoxm);
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        res = SecurePortDiscovery(pDInfo, clientResponse);
                        if(OC_STACK_OK != res)
                        {
                            OIC_LOG(ERROR, TAG, "Failed to SecurePortDiscovery");
                            DeleteDoxmBinData(ptrDoxm);
                            return OC_STACK_KEEP_TRANSACTION;
                        }
                    }
                    else
                    {
                        OIC_LOG(ERROR, TAG, "discarding device's reply, because not a SubOwner.");
                        DeleteDoxmBinData(ptrDoxm);
                        return OC_STACK_KEEP_TRANSACTION;
                    }
                }
                else
                {
                    if(ptrDoxm->mom && OIC_MULTIPLE_OWNER_DISABLE != ptrDoxm->mom->mode)
                    {
                        res = AddDevice(ppDevicesList, &clientResponse->devAddr,
                                clientResponse->connType, ptrDoxm);
                        if (OC_STACK_OK != res)
                        {
                            OIC_LOG(ERROR, TAG, "Error while adding data to linkedlist.");
                            DeleteDoxmBinData(ptrDoxm);
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        res = SecurePortDiscovery(pDInfo, clientResponse);
                        if(OC_STACK_OK != res)
                        {
                            OIC_LOG(ERROR, TAG, "Failed to SecurePortDiscovery");
                            DeleteDoxmBinData(ptrDoxm);
                            return OC_STACK_KEEP_TRANSACTION;
                        }
                    }
                    else
                    {
                        OIC_LOG(ERROR, TAG, "discarding mom disabled device's reply");
                        DeleteDoxmBinData(ptrDoxm);
                        return OC_STACK_KEEP_TRANSACTION;
                    }
                }

                OIC_LOG(INFO, TAG, "Exiting ProvisionDiscoveryHandler.");
            }

            return  OC_STACK_KEEP_TRANSACTION;
        }
    }
    else
    {
        OIC_LOG(INFO, TAG, "Skiping Null response");
        return OC_STACK_KEEP_TRANSACTION;
    }
}

/**
 * The function is responsible for the discovery of an MOT-enabled device with the specified deviceID.
 * The function will return when security information for device with deviceID has been obtained or the
 * timeout has been exceeded.
 *
 * @param[in]  timeoutSeconds  Maximum time, in seconds, this function will listen for responses from
 *                             servers before returning.
 * @param[in]  deviceID        deviceID of target device.
 * @param[out] ppFoundDevice   OCProvisionDev_t of found device. Caller should use PMDeleteDeviceList 
 *                             to delete the device.
 *
 * @return OC_STACK_OK on success otherwise error.
 *         OC_STACK_INVALID_PARAM when deviceID is NULL or ppFoundDevice is not initailized.
 */
OCStackResult PMMultipleOwnerSingleDeviceDiscovery(unsigned short timeoutSeconds,
                                                   const OicUuid_t* deviceID,
                                                   OCProvisionDev_t **ppFoundDevice)
{
    OIC_LOG(DEBUG, TAG, "IN PMMultipleOwnerSingleDeviceDiscovery");

    if ((NULL == ppFoundDevice) || (NULL == deviceID))
    {
        return OC_STACK_INVALID_PARAM;
    }

    DiscoveryInfo discoveryInfo;
    discoveryInfo.ppDevicesList = ppFoundDevice;
    discoveryInfo.pCandidateList = NULL;
    discoveryInfo.isOwnedDiscovery = false;
    discoveryInfo.isSingleDiscovery = true;
    discoveryInfo.isFound = false;
    discoveryInfo.targetId = deviceID;

    OCCallbackData cbData;
    cbData.cb = &MOTDeviceDiscoveryHandler;
    cbData.context = (void *)&discoveryInfo;
    cbData.cd = NULL;

    OCStackResult res = OC_STACK_ERROR;
    const char query[] = "/oic/sec/doxm?mom!=0&owned=TRUE";

    OCDoHandle handle = NULL;
    res = OCDoResource(&handle, OC_REST_DISCOVER, query, 0, 0, CT_DEFAULT, OC_HIGH_QOS, &cbData, NULL, 0);
    if (res != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
        return res;
    }

    //Waiting for each response.
    uint64_t startTime = OICGetCurrentTime(TIME_IN_MS);
    while ((OC_STACK_OK == res) && !discoveryInfo.isFound)
    {
        uint64_t currTime = OICGetCurrentTime(TIME_IN_MS);
        if (currTime >= startTime)
        {
            long elapsed = (long)((currTime - startTime) / MS_PER_SEC);
            if (elapsed > timeoutSeconds)
            {
                break;
            }

            // Sleep for 100 ms to free up the CPU
            usleep(100 * IOTIVITY_USECS_PER_MSEC);

            res = OCProcess();
        }
        else
        {
            // System time has changed so we cannot reliably continue processing.
            // Function returns with no device discovered.
            break;
        }
    }

    if (OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed while waiting for a secure discovery response.");
        OCStackResult resCancel = OCCancel(handle, OC_HIGH_QOS, NULL, 0);
        if (OC_STACK_OK != resCancel)
        {
            OIC_LOG(ERROR, TAG, "Failed to remove registered callback");
        }
        return res;
    }

    res = OCCancel(handle, OC_HIGH_QOS, NULL, 0);
    if (OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to remove the registered callback");
        return res;
    }
    OIC_LOG(DEBUG, TAG, "OUT PMMultipleOwnerSingleDeviceDiscovery");
    return res;
}

/**
 * Discover multiple OTM enabled devices in the same IP subnet.
 *
 * @param[in] waittime      Timeout in seconds.
 * @param[in] ppDevicesList        List of OCProvisionDev_t.
 *
 * @return OC_STACK_OK on success otherwise error.
 */
OCStackResult PMMultipleOwnerDeviceDiscovery(unsigned short waittime, bool isMultipleOwned, OCProvisionDev_t **ppDevicesList)
{
    OIC_LOG(DEBUG, TAG, "IN PMMultipleOwnerEnabledDeviceDiscovery");

    if (NULL != *ppDevicesList)
    {
        OIC_LOG(ERROR, TAG, "List is not null can cause memory leak");
        return OC_STACK_INVALID_PARAM;
    }

    const char *DOXM_MOM_ENABLE_MULTICAST_QUERY = "/oic/sec/doxm?mom!=0&owned=TRUE";
    const char *DOXM_MULTIPLE_OWNED_MULTICAST_QUERY = "/oic/sec/doxm?owned=TRUE";

    DiscoveryInfo *pDInfo = OICCalloc(1, sizeof(DiscoveryInfo));
    if(NULL == pDInfo)
    {
        OIC_LOG(ERROR, TAG, "PMDeviceDiscovery : Memory allocation failed.");
        return OC_STACK_NO_MEMORY;
    }

    pDInfo->ppDevicesList = ppDevicesList;
    pDInfo->pCandidateList = NULL;
    pDInfo->isOwnedDiscovery = isMultipleOwned;

    OCCallbackData cbData;
    cbData.cb = &MOTDeviceDiscoveryHandler;
    cbData.context = (void *)pDInfo;
    cbData.cd = NULL;
    OCStackResult res = OC_STACK_ERROR;

    const char* query = isMultipleOwned ? DOXM_MULTIPLE_OWNED_MULTICAST_QUERY :
                                          DOXM_MOM_ENABLE_MULTICAST_QUERY;

    OCDoHandle handle = NULL;
    res = OCDoResource(&handle, OC_REST_DISCOVER, query, 0, 0,
                                     CT_DEFAULT, OC_HIGH_QOS, &cbData, NULL, 0);
    if (res != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
        OICFree(pDInfo);
        return res;
    }

    //Waiting for each response.
    res = PMTimeout(waittime, true);
    if(OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to wait response for secure discovery.");
        OICFree(pDInfo);
        OCStackResult resCancel = OCCancel(handle, OC_HIGH_QOS, NULL, 0);
        if(OC_STACK_OK !=  resCancel)
        {
            OIC_LOG(ERROR, TAG, "Failed to remove registered callback");
        }
        return res;
    }
    res = OCCancel(handle,OC_HIGH_QOS,NULL,0);
    if (OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to remove registered callback");
        OICFree(pDInfo);
        return res;
    }
    OIC_LOG(DEBUG, TAG, "OUT PMMultipleOwnerEnabledDeviceDiscovery");
    OICFree(pDInfo);
    return res;
}

/**
 * The function is responsible for determining if the caller is a subowner of the specified device.
 *
 * @param[in] device       MOT enabled device that contains a list of subowners
 * @param[out] isSubowner  Bool indicating whether the caller is a subowner of device
 *
 * @return OC_STACK_OK in case of success and other value otherwise.
 */
OCStackResult PMIsSubownerOfDevice(OCProvisionDev_t *device, bool *isSubowner)
{  
    if ((NULL == device) || (NULL == isSubowner))
    {
        return OC_STACK_INVALID_PARAM;
    }

    OicUuid_t myId;
    memset(&myId, 0, sizeof(myId));
    OicSecSubOwner_t* subOwner = NULL;

    *isSubowner = false;

    OCStackResult result = GetDoxmDeviceID(&myId);
    if (OC_STACK_OK == result)
    {
        LL_FOREACH(device->doxm->subOwners, subOwner)
        {
            if (memcmp(myId.id, subOwner->uuid.id, sizeof(myId.id)) == 0)
            {
                *isSubowner = true;
                break;
            }
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Error while getting my UUID.");
    }

    return result;
}
#endif //MULTIPLE_OWNER

static OCStackResult SecurePortDiscovery(DiscoveryInfo* discoveryInfo,
                                         const OCClientResponse *clientResponse)
{
    OIC_LOG(DEBUG, TAG, "IN SecurePortDiscovery");

    if(NULL == discoveryInfo || NULL == clientResponse)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t *pDev = GetDevice(&discoveryInfo->pCandidateList,
                        clientResponse->devAddr.addr, clientResponse->devAddr.port);
    if(NULL == pDev)
    {
        OIC_LOG(ERROR, TAG, "SecurePortDiscovery : Failed to get device");
        return OC_STACK_ERROR;
    }

    //Try to the unicast discovery to getting secure port
    char query[MAX_URI_LENGTH+MAX_QUERY_LENGTH+1] = {0};
    if(!PMGenerateQuery(false,
                        pDev->endpoint.addr, pDev->endpoint.port,
                        pDev->connType,
                        query, sizeof(query), OC_RSRVD_WELL_KNOWN_URI))
    {
        OIC_LOG(ERROR, TAG, "SecurePortDiscovery : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(DEBUG, TAG, "Query=%s", query);

    // Set filter query with rt=oic.r.doxm
    const char RES_DOXM_QUERY_FMT[] = "%s?%s=%s";
    char uri[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};
    snprintf(uri, sizeof(uri), RES_DOXM_QUERY_FMT, query,
            OC_RSRVD_RESOURCE_TYPE, OIC_RSRC_TYPE_SEC_DOXM);

    OIC_LOG_V(DEBUG, TAG, "URI=%s", uri);

    OCCallbackData cbData;
    cbData.cb = &SecurePortDiscoveryHandler;
    cbData.context = (void*)discoveryInfo;
    cbData.cd = NULL;
    OCStackResult ret = OCDoResource(&pDev->handle, OC_REST_DISCOVER, uri, 0, 0,
            pDev->connType, OC_HIGH_QOS, &cbData, NULL, 0);
    if(OC_STACK_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "Failed to Secure Port Discovery");
        return ret;
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "OCDoResource with [%s] Success", query);
    }

    OIC_LOG(DEBUG, TAG, "OUT SecurePortDiscovery");

    return ret;
}


static OCStackResult SpecVersionDiscovery(DiscoveryInfo* discoveryInfo,
                                              const OCClientResponse *clientResponse)
{
    OIC_LOG(DEBUG, TAG, "IN SpecVersionDiscovery");

    if(NULL == discoveryInfo || NULL == clientResponse)
    {
        return OC_STACK_INVALID_PARAM;
    }

    //Try to the unicast discovery to getting security version
    char query[MAX_URI_LENGTH+MAX_QUERY_LENGTH+1] = {0};
    if(!PMGenerateQuery(false,
                        clientResponse->devAddr.addr, clientResponse->devAddr.port,
                        clientResponse->connType,
                        query, sizeof(query), OC_RSRVD_DEVICE_URI))
    {
        OIC_LOG(ERROR, TAG, "SpecVersionDiscovery : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(DEBUG, TAG, "Query=%s", query);

    OCCallbackData cbData;
    cbData.cb = &SpecVersionDiscoveryHandler;
    cbData.context = (void*)discoveryInfo;
    cbData.cd = NULL;
    OCStackResult ret = OCDoResource(NULL, OC_REST_DISCOVER, query, 0, 0,
            clientResponse->connType, OC_HIGH_QOS, &cbData, NULL, 0);
    if(OC_STACK_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "Failed to Security Version Discovery");
        return ret;
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "OCDoResource with [%s] Success", query);
    }

    OIC_LOG(DEBUG, TAG, "OUT SpecVersionDiscovery");

    return ret;
}

/**
 * Function to print OCProvisionDev_t for debug purpose.
 *
 * @param[in] pDev Pointer to OCProvisionDev_t. It's information will be printed by OIC_LOG_XX
 *
 */
void PMPrintOCProvisionDev(const OCProvisionDev_t* pDev)
{
    if (pDev)
    {
        OIC_LOG(DEBUG, TAG, "+++++ OCProvisionDev_t Information +++++");
        OIC_LOG_V(DEBUG, TAG, "IP %s", pDev->endpoint.addr);
        OIC_LOG_V(DEBUG, TAG, "PORT %d", pDev->endpoint.port);
        OIC_LOG_V(DEBUG, TAG, "S-PORT %d", pDev->securePort);
        OIC_LOG(DEBUG, TAG, "++++++++++++++++++++++++++++++++++++++++");
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "+++++ OCProvisionDev_t is NULL +++++");
    }
}

bool PMDeleteFromUUIDList(OCUuidList_t **pUuidList, OicUuid_t *targetId)
{
    if(*pUuidList == NULL || targetId == NULL)
    {
        return false;
    }
    OCUuidList_t *tmp1 = NULL,*tmp2=NULL;
    LL_FOREACH_SAFE(*pUuidList, tmp1, tmp2)
    {
        if(0 == memcmp(tmp1->dev.id, targetId->id, sizeof(targetId->id)))
        {
            LL_DELETE(*pUuidList, tmp1);
            OICFree(tmp1);
            return true;
        }
    }
    return false;
}
