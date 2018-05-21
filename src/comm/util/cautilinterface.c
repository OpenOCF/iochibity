/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

/* #include "caadapterutils.h" */

/*GAR FIXME: conditionalize on --with-ble etc. */
/* #include "camanagerleinterface.h" */
/* #include "cabtpairinginterface.h" */

#include "cautilinterface.h"

// GAR needed for fn prototypes: #include "cainterfacecontroller.h"
/* #include "cacommon.h" */
/* #include "logger.h" */

/* #if defined(TCP_ADAPTER) && defined(WITH_CLOUD) */
/* #include "caconnectionmanager.h" */
/* #endif */

#define TAG "OIC_CA_COMMON_UTILS"

/* GAR: definition is in comm/cainterfacecontroller.c/AddNetworkStateChangedCallback */
CAResult_t AddNetworkStateChangedCallback(CAAdapterStateChangedCB adapterCB,
					  CAConnectionStateChangedCB connCB);

/* GAR: inline this */
static CAResult_t CASetNetworkMonitorCallbacks(CAAdapterStateChangedCB adapterCB,
					       CAConnectionStateChangedCB connCB)
{
    OIC_LOG(DEBUG, TAG, "Set network monitoring callback");
    CAResult_t res = AddNetworkStateChangedCallback(adapterCB, connCB);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "AddNetworkStateChangedCallback has failed");
        return CA_STATUS_FAILED;
    }
    return CA_STATUS_OK;
}

CAResult_t CARegisterNetworkMonitorHandler(CAAdapterStateChangedCB adapterStateCB,
                                           CAConnectionStateChangedCB connStateCB)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    return CASetNetworkMonitorCallbacks(adapterStateCB, connStateCB);
}

/* GAR: definition is in comm/cainterfacecontroller.c/AddNetworkStateChangedCallback */
CAResult_t RemoveNetworkStateChangedCallback(CAAdapterStateChangedCB adapterCB,
					     CAConnectionStateChangedCB connCB);

/* GAR: inline this */
static CAResult_t CAUnsetNetworkMonitorCallbacks(CAAdapterStateChangedCB adapterCB,
						 CAConnectionStateChangedCB connCB)
{
    OIC_LOG(DEBUG, TAG, "Unset network monitoring callback");
    CAResult_t res = RemoveNetworkStateChangedCallback(adapterCB, connCB);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "RemoveNetworkStateChangedCallback has failed");
        return CA_STATUS_FAILED;
    }
    return CA_STATUS_OK;
}

CAResult_t CAUnregisterNetworkMonitorHandler(CAAdapterStateChangedCB adapterStateCB,
                                             CAConnectionStateChangedCB connStateCB)
{
    OIC_LOG(DEBUG, TAG, "CAUnregisterNetworkMonitorHandler");

    return CAUnsetNetworkMonitorCallbacks(adapterStateCB, connStateCB);
}

CAResult_t CASetAutoConnectionDeviceInfo(const char *address)
{
    OIC_LOG(DEBUG, TAG, "CASetAutoConnectionDeviceInfo");

#if defined(__ANDROID__) && defined(LE_ADAPTER)
    return CASetLEClientAutoConnectionDeviceInfo(address);
#else
    (void)address;
    return CA_NOT_SUPPORTED;
#endif
}

CAResult_t CAUnsetAutoConnectionDeviceInfo(const char *address)
{
    OIC_LOG(DEBUG, TAG, "CAUnsetAutoConnectionDeviceInfo");

#if defined(__ANDROID__) && defined(LE_ADAPTER)
    return CAUnsetLEClientAutoConnectionDeviceInfo(address);
#else
    (void)address;
    return CA_NOT_SUPPORTED;
#endif
}

CAResult_t CASetPortNumberToAssign(CATransportAdapter_t adapter,
                                   CATransportFlags_t flag, uint16_t port)
{
    uint16_t *targetPort = 0;

    if (CA_ADAPTER_IP & adapter)
    {
        if (CA_SECURE & flag)
        {
            if (CA_IPV6 & flag)
            {
                targetPort = &caglobals.ports.udp.u6s;
            }
            else if (CA_IPV4 & flag)
            {
                targetPort = &caglobals.ports.udp.u4s;
            }
        }
        else
        {
            if (CA_IPV6 & flag)
            {
                targetPort = &caglobals.ports.udp.u6;
            }
            else if (CA_IPV4 & flag)
            {
                targetPort = &caglobals.ports.udp.u4;
            }
        }
    }
#ifdef TCP_ADAPTER
    if (CA_ADAPTER_TCP & adapter)
    {
        if (CA_SECURE & flag)
        {
            if (CA_IPV6 & flag)
            {
                targetPort = &caglobals.ports.tcp.u6s;
            }
            else if (CA_IPV4 & flag)
            {
                targetPort = &caglobals.ports.tcp.u4s;
            }
        }
        else
        {
            if (CA_IPV6 & flag)
            {
                targetPort = &caglobals.ports.tcp.u6;
            }
            else if (CA_IPV4 & flag)
            {
                targetPort = &caglobals.ports.tcp.u4;
            }
        }
    }
#endif

    if (targetPort)
    {
        *targetPort = port;
        return CA_STATUS_OK;
    }

    return CA_NOT_SUPPORTED;
}

uint16_t CAGetAssignedPortNumber(CATransportAdapter_t adapter, CATransportFlags_t flag)
{
    OIC_LOG(DEBUG, TAG, "CAGetAssignedPortNumber");

    if (CA_ADAPTER_IP & adapter)
    {
        if (CA_SECURE & flag)
        {
            if (CA_IPV6 & flag)
            {
                return caglobals.ip.u6s.port;
            }
            else if (CA_IPV4 & flag)
            {
                return caglobals.ip.u4s.port;
            }
        }
        else
        {
            if (CA_IPV6 & flag)
            {
                return caglobals.ip.u6.port;
            }
            else if (CA_IPV4 & flag)
            {
                return caglobals.ip.u4.port;
            }
        }
    }
#ifdef TCP_ADAPTER
    if (CA_ADAPTER_TCP & adapter)
    {
        if (CA_SECURE & flag)
        {
            if (CA_IPV6 & flag)
            {
                return caglobals.tcp.ipv6s.port;
            }
            else if (CA_IPV4 & flag)
            {
                return caglobals.tcp.ipv4s.port;
            }
        }
        else
        {
            if (CA_IPV6 & flag)
            {
                return caglobals.tcp.ipv6.port;
            }
            else if (CA_IPV4 & flag)
            {
                return caglobals.tcp.ipv4.port;
            }
        }
    }
#endif
    return 0;
}

#if defined(TCP_ADAPTER) && defined(WITH_CLOUD)
CAResult_t CAUtilCMInitailize()
{
    return CACMInitialize();
}

CAResult_t CAUtilCMTerminate()
{
    return CACMTerminate();
}

CAResult_t CAUtilCMUpdateRemoteDeviceInfo(const CAEndpoint_t *endpoint, bool isCloud)
{
    return CACMUpdateRemoteDeviceInfo(endpoint, isCloud);
}

CAResult_t CAUtilCMResetRemoteDeviceInfo()
{
    return CACMResetRemoteDeviceInfo();
}

CAResult_t CAUtilCMSetConnectionUserConfig(CAConnectUserPref_t connPrefer)
{
    return CACMSetConnUserConfig(connPrefer);
}

CAResult_t CAUtilCMGetConnectionUserConfig(CAConnectUserPref_t *connPrefer)
{
    return CACMGetConnUserConfig(connPrefer);
}
#endif

CAResult_t CAGetIpv6AddrScope(const char *addr, CATransportFlags_t *scopeLevel)
{
    return CAGetIpv6AddrScopeInternal(addr, scopeLevel);
}

void CAUtilSetLogLevel(CAUtilLogLevel_t level, bool hidePrivateLogEntries)
{
    OIC_LOG(DEBUG, TAG, "CAUtilSetLogLevel");
    LogLevel logLevel = DEBUG;
    switch(level)
    {
        case CA_LOG_LEVEL_INFO:
            logLevel = INFO;
            break;
        case CA_LOG_LEVEL_ALL:
        default:
            logLevel = DEBUG;
            break;
    }

    OCSetLogLevel(logLevel, hidePrivateLogEntries);
}
