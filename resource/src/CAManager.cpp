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

/**
 * @file
 *
 * This file contains the implementation of classes and its members related
 * to CAManager. Network changes status will be passed from CA to application.
 */

#include "OCApi.h"
#include "CAManager.h"
#include "cautilinterface.h"

using namespace OC;

namespace
{
        CAManager::AdapterChangedCallback g_adapterHandler = nullptr;
        CAManager::ConnectionChangedCallback g_connectionHandler = nullptr;
}

OCStackResult convertCAResultToOCResult(CAResult_t caResult)
{
    switch (caResult)
    {
        case CA_STATUS_OK:
            return OC_STACK_OK;
        case CA_STATUS_INVALID_PARAM:
            return OC_STACK_INVALID_PARAM;
        case CA_STATUS_FAILED:
            return OC_STACK_ERROR;
        case CA_NOT_SUPPORTED:
            return OC_STACK_NOTIMPL;
        default:
            return OC_STACK_ERROR;
    }
}

void DefaultAdapterStateChangedHandler(CATransportAdapter_t adapter, bool enabled)
{
    if (g_adapterHandler)
    {
        g_adapterHandler((OCTransportAdapter) adapter, enabled);
    }
}

void DefaultConnectionStateChangedHandler(CATransportAdapter_t adapter,
                                          const char *remote_address, bool connected)
{
    if (g_connectionHandler)
    {
        g_connectionHandler((OCTransportAdapter) adapter, remote_address, connected);
    }
}

OCStackResult CAManager::setNetworkMonitorHandler(AdapterChangedCallback adapterHandler,
                                                  ConnectionChangedCallback connectionHandler)
{
    g_adapterHandler = adapterHandler;
    g_connectionHandler = connectionHandler;

    CAResult_t ret = CARegisterNetworkMonitorHandler(DefaultAdapterStateChangedHandler,
                                                     DefaultConnectionStateChangedHandler);

    return convertCAResultToOCResult(ret);
}

OCStackResult CAManager::setPortNumberToAssign(OCTransportAdapter adapter,
                                               OCTransportFlags flag, uint16_t port)
{
    CAResult_t ret = CASetPortNumberToAssign((CATransportAdapter_t) adapter,
                                             (CATransportFlags_t) flag, port);

    return convertCAResultToOCResult(ret);
}

uint16_t CAManager::getAssignedPortNumber(OCTransportAdapter adapter, OCTransportFlags flag)
{
    return CAGetAssignedPortNumber((CATransportAdapter_t) adapter, (CATransportFlags_t) flag);
}
