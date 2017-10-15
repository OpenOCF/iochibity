/* ****************************************************************
 *
 * Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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
 * This file contains the definition and types for client's callback mode and functions.
 */


#ifndef OC_CLIENT_CB_METHODS_H_
#define OC_CLIENT_CB_METHODS_H_

#include "ocstack.h"
#include "ocresource.h"
#include "ocpresence.h"
#include "cacommon.h"
#include "occlientcb.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * This method is used to add a client callback method in cbList.
 *
 * @param[out] clientCB            The resulting node from making this call. Null if out of memory.
 * @param[in]  cbData              Address to client callback function.
 * @param[in]  type                Qos type.
 * @param[in]  token               Identifier for OTA CoAP comms.
 * @param[in]  tokenLength         Length for OTA CoAP comms.
 * @param[in]  options             The address of an array containing the vendor specific header
 *                                 options to be sent with the request.
 * @param[in]  numOptions          Number of header options to be included.
 * @param[in]  payload             Request payload.
 * @param[in]  payloadSize         Size of payload.
 * @param[in]  payloadFormat       Format of payload.
 * @param[in]  handle              masked in the public API as an 'invocation handle'
 *                                 Used for callback management.
 * @param[in]  method              A method via which this client callback is expected to operate
 * @param[in]  devAddr             The Device address.
 * @param[in]  requestUri          The resource uri of the request.
 * @param[in]  resourceTypeName    The resource type associated with a presence request.
 * @param[in]  ttl                 time to live in coap_ticks for the callback.
 *
 * @note If the handle you're looking for does not exist, the stack will reply with a RST message.
 *
 * @return OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult AddClientCB(ClientCB** clientCB,
                          OCCallbackData* cbData,
                          CAMessageType_t type,
                          CAToken_t token,
                          uint8_t tokenLength,
                          CAHeaderOption_t *options,
                          uint8_t numOptions,
                          CAPayload_t payload,
                          size_t payloadSize,
                          CAPayloadFormat_t payloadFormat,
                          OCDoHandle *handle,
                          OCMethod method,
                          OCDevAddr *devAddr,
                          char *requestUri,
                          char *resourceTypeName,
                          uint32_t ttl);

/**
 * This method is used to remove a callback node from cbList.
 *
 * @param[in]  cbNode               Address to client callback node.
 */
void DeleteClientCB(ClientCB *cbNode);

/**
 * This method is used to clear the cbList.
 */
void DeleteClientCBList();

/**
 * This method is used to search and retrieve a cb node in cbList using token.
 *
 * @param[in]  token                Token to search for.
 * @param[in]  tokenLength          The Length of the token.
 *
 * @return address of the node if found, otherwise NULL
 */
ClientCB* GetClientCBUsingToken(const CAToken_t token,
                                const uint8_t tokenLength);

/**
 * This method is used to search and retrieve a cb node in cbList using a handle.
 *
 * @param[in]  handle               Handle to search for.
 *
 * @return address of the node if found, otherwise NULL
 */
ClientCB* GetClientCBUsingHandle(const OCDoHandle handle);

/* #ifdef WITH_PRESENCE */
/*
 * This method is used to search and retrieve a cb node in cbList using a uri.
 *
 * @param[in]  requestUri           Uri to search for.
 *
 * @return address of the node if found, otherwise NULL
 */
ClientCB* GetClientCBUsingUri(const char *requestUri);
/* #endif // WITH_PRESENCE */

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // OC_CLIENT_CB_H_

