//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 * @file
 *
 */

#ifndef OCPRESENCE_METHODS_H_
#define OCPRESENCE_METHODS_H_

#define WITH_PRESENCE

#include "occlientcb.h"
#include "ocresource.h"
#include "octypes.h"
#include "ocpayload.h"
#include "ocpresence.h"
#include "cacommon.h"

/* //src/logger */
#include "logger.h"
#include "trace.h"

OCStackResult OCProcessPresence();

/**
 * Handle response from presence request.
 *
 * @param endPoint CA remote endpoint.
 * @param responseInfo CA response info.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult HandlePresenceResponse(const CAEndpoint_t *endPoint,
        const CAResponseInfo_t *responseInfo);

/**
 * Reset presence TTL for a ClientCB struct. ttlLevel will be set to 0.
 * TTL will be set to maxAge.
 *
 * @param cbNode Callback Node for which presence ttl is to be reset.
 * @param maxAge New value of ttl in seconds.

 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult ResetPresenceTTL(ClientCB *cbNode, uint32_t maxAgeSeconds);

OCStackResult OCPreparePresence(CAEndpoint_t *endpoint,
                                       char **requestUri,
                                       bool isMulticast);

/* #ifdef WITH_PRESENCE */
/** from ocstackinternal.h
 * Notify Presence subscribers that a resource has been modified.
 *
 * @param resourceType    Handle to the resourceType linked list of resource that was modified.
 * @param trigger         The simplified reason this API was invoked.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult SendPresenceNotification(OCResourceType *resourceType,
        OCPresenceTrigger trigger);

/**
 * Send Stop Notification to Presence subscribers.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult SendStopNotification();
/* #endif // WITH_PRESENCE */

/* #ifdef WITH_PRESENCE */
/** from ocstackinternal.h
 * Enable/disable a resource property.
 *
 * @param inputProperty             Pointer to resource property.
 * @param resourceProperties        Property to be enabled/disabled.
 * @param enable                    0:disable, 1:enable.
 *
 * @return OCStackResult that was converted from the input CAResult_t value.
 */
//TODO: should the following function be public?
OCStackResult OCChangeResourceProperty(OCResourceProperty * inputProperty,
        OCResourceProperty resourceProperties, uint8_t enable);
/* #endif */

/* from ocstackinternal.h */
const char *OC_CALL convertTriggerEnumToString(OCPresenceTrigger trigger);

OCPresenceTrigger OC_CALL convertTriggerStringToEnum(const char * triggerStr);

/* from ocpayload.h: */
// Presence Payload
OCPresencePayload* OC_CALL OCPresencePayloadCreate(uint32_t seqNum, uint32_t maxAge,
        OCPresenceTrigger trigger, const char* resourceType);

void OC_CALL OCPresencePayloadDestroy(OCPresencePayload* payload);

/* #ifdef WITH_PRESENCE */
/* FIXME: do not overload this fn! rename it to
   SendAllObserverNotificationWithPresence or some such */
/** from ocobserve.h
 * Create an observe response and send to all observers in the observe list.
 *
 * @param method          RESTful method.
 * @param resPtr          Observed resource.
 * @param maxAge          Time To Live (in seconds) of observation.
 * @param resourceType    Resource type.  Allows resource type name to be added to response.
 * @param qos             Quality of service of resource.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult SendAllObserverNotificationWithPresence (OCMethod method,
						       OCResource *resPtr,
						       uint32_t maxAge,
						       OCPresenceTrigger trigger,
						       OCResourceType *resourceType,
						       OCQualityOfService qos);
/* #else */
/* #endif */

#endif
