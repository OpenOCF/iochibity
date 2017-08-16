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

#ifndef CA_UTILS_INTERFACE_H_
#define CA_UTILS_INTERFACE_H_

#include "cacommon.h"
//GAR FIXME: remove
/* #ifdef __ANDROID__ */
/* #include "jni.h" */
/* #endif */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Callback function type for connection status changes delivery.
 * @param[out]   info           Remote endpoint information.
 * @param[out]   isConnected    Current connection status info.
 */
typedef void (*CAConnectionStateChangedCB)(const CAEndpoint_t *info, bool isConnected);

/**
 * Callback function type for adapter status changes delivery.
 * @param[out]   adapter    Transport type information.
 * @param[out]   enabled    Current adapter status info.
 */
typedef void (*CAAdapterStateChangedCB)(CATransportAdapter_t adapter, bool enabled);

/**
 * Register network monitoring callback.
 * Network status changes are delivered these callback.
 * @param[in]   adapterStateCB  Adapter state monitoring callback.
 * @param[in]   connStateCB     Connection state monitoring callback.
 *
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED or ::CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CARegisterNetworkMonitorHandler(CAAdapterStateChangedCB adapterStateCB,
                                           CAConnectionStateChangedCB connStateCB);

/**
 * Set device to handle for auto connection.
 * @param[in]   address         LE address to set.
 *
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED.
 */
CAResult_t CASetAutoConnectionDeviceInfo(const char* address);

/**
 * Unset device to handle for auto connection.
 * @param[in]   address         LE address to unset.
 *
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED.
 */
CAResult_t CAUnsetAutoConnectionDeviceInfo(const char* address);

/**
 * Set the port number to assign .
 * @param[in]   adapter     Transport adapter information.
 * @param[in]   flag        Transport flag information.
 * @param[in]   port        The port number to use.
 *
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED.
 */
CAResult_t CASetPortNumberToAssign(CATransportAdapter_t adapter,
                                   CATransportFlags_t flag, uint16_t port);

/**
 * Get the assigned port number currently.
 * @param[in]   adapter     Transport adapter information.
 * @param[in]   flag        Transport flag information.
 *
 * @return  assigned port number information.
 */
uint16_t CAGetAssignedPortNumber(CATransportAdapter_t adapter, CATransportFlags_t flag);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_UTILS_INTERFACE_H_ */

