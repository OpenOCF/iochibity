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

#ifndef OCPRESENCE_H_
#define OCPRESENCE_H_

#define WITH_PRESENCE

/* #include "occlientcb.h" */
/* #include "ocresource.h" */
#include "octypes.h"
#include "cacommon.h"

/* //src/logger */
#include "logger.h"
#include "trace.h"

/* #ifdef WITH_PRESENCE */

/**
 * The OCPresenceTrigger enum delineates the three spec-compliant modes for
 * "Trigger." These enum values are then mapped to  strings
 * "create", "change", "delete", respectively, before getting encoded into
 * the payload.
 */
typedef enum
{
    /** The creation of a resource is associated with this invocation. */
    OC_PRESENCE_TRIGGER_CREATE = 0,

    /** The change/update of a resource is associated this invocation. */
    OC_PRESENCE_TRIGGER_CHANGE = 1,

    /** The deletion of a resource is associated with this invocation.*/
    OC_PRESENCE_TRIGGER_DELETE = 2
} OCPresenceTrigger;
/* #endif */

/* #ifdef WITH_PRESENCE */
typedef struct			/* from octypes.h */
{
    OCPayload base;
    uint32_t sequenceNumber;
    uint32_t maxAge;
    OCPresenceTrigger trigger;
    char* resourceType;
} OCPresencePayload;
/* #endif */

/* #ifdef WITH_PRESENCE */
/** from occlientcb.h
 * Data structure For presence Discovery.
 * This is the TTL associated with presence.
 */
typedef struct OCPresence
{
    /** Time to Live. */
    uint32_t TTL;

    /** Time out. */
    uint32_t * timeOut;

    /** TTL Level. */
    uint32_t TTLlevel;
} OCPresence;
/* #endif // WITH_PRESENCE */

/** from ocresource.h
 *Virtual Resource Presence Attributes
 */
/* #ifdef WITH_PRESENCE */
typedef struct PRESENCERESOURCE{
    OCResourceHandle handle;
    uint32_t presenceTTL;
} PresenceResource;
/* #endif */

/* #ifdef WITH_PRESENCE */
typedef enum
{
    OC_PRESENCE_UNINITIALIZED = 0,
    OC_PRESENCE_INITIALIZED
} OCPresenceState;
/* #endif */

/* #ifdef WITH_PRESENCE */
static OCPresenceState presenceState = OC_PRESENCE_UNINITIALIZED;
static PresenceResource presenceResource = {0};
static uint8_t PresenceTimeOutSize = 0;
static uint32_t PresenceTimeOut[] = {50, 75, 85, 95, 100};
/* #endif */

/**
 * Forward declaration of resource type.
 */
typedef struct resourcetype_t OCResourceType;

OCStackResult SendPresenceNotification(OCResourceType *resourceType,
				       OCPresenceTrigger trigger);

OCStackResult SendStopNotification();

#endif
