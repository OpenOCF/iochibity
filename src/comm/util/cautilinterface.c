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

#include "camanagerleinterface.h"
/* #include "cabtpairinginterface.h" */
#include "cautilinterface.h"
#include "cainterfacecontroller.h"
#include "cacommon.h"
#include "logger.h"

#define TAG "OIC_CA_COMMON_UTILS"

/*GAR CAResult_t CARegisterNetworkMonitorHandler(CAAdapterStateChangedCB adapterStateCB, */
CAResult_t CARegisterNetworkMonitorHandler(CAAdapterStateChangedCB adapterStateCB,
                                           CAConnectionStateChangedCB connStateCB)
{
    OIC_LOG(DEBUG, TAG, "CARegisterNetworkMonitorHandler");

    CASetNetworkMonitorCallbacks(adapterStateCB, connStateCB);
    return CA_STATUS_OK;
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
        if (CA_IPV6 & flag)
        {
            targetPort = &caglobals.ports.tcp.u6;
        }
        else if (CA_IPV4 & flag)
        {
            targetPort = &caglobals.ports.tcp.u4;
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
        if (CA_IPV6 & flag)
        {
            return caglobals.tcp.ipv6.port;
        }
        else if (CA_IPV4 & flag)
        {
            return caglobals.tcp.ipv4.port;
        }
    }
#endif
    return 0;
}
