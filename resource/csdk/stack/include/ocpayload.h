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

#ifndef OCPAYLOAD_H_
#define OCPAYLOAD_H_

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdbool.h>
#include <inttypes.h>
#include "octypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Macro to verify the validity of cbor operation.
 */
#define VERIFY_CBOR_SUCCESS(log_tag, err, log_message) \
    if ((CborNoError != (err)) && (CborErrorOutOfMemory != (err))) \
    { \
        if ((log_tag) && (log_message)) \
        { \
            OIC_LOG_V(ERROR, (log_tag), "%s with cbor error: \'%s\'.", \
                    (log_message), (cbor_error_string(err))); \
        } \
        goto exit; \
    } \

#define VERIFY_PARAM_NON_NULL(log_tag, err, log_message) \
    if (NULL == (err)) \
    { \
        OIC_LOG_V(FATAL, (log_tag), "%s", (log_message)); \
        goto exit;\
    } \


typedef struct OCResource OCResource;

OC_EXPORT void OCPayloadDestroy(OCPayload* payload);

// Representation Payload
OC_EXPORT OCRepPayload* OCRepPayloadCreate();

OC_EXPORT size_t calcDimTotal(const size_t dimensions[MAX_REP_ARRAY_DEPTH]);

OC_EXPORT OCRepPayload* OCRepPayloadClone(const OCRepPayload* payload);

OC_EXPORT void OCRepPayloadAppend(OCRepPayload* parent, OCRepPayload* child);

OC_EXPORT bool OCRepPayloadSetUri(OCRepPayload* payload, const char* uri);

OC_EXPORT bool OCRepPayloadAddResourceType(OCRepPayload* payload, const char* resourceType);
OC_EXPORT bool OCRepPayloadAddInterface(OCRepPayload* payload, const char* iface);
OC_EXPORT bool OCRepPayloadAddModelVersion(OCRepPayload* payload, const char* dmv);

OC_EXPORT bool OCRepPayloadAddResourceTypeAsOwner(OCRepPayload* payload, char* resourceType);
OC_EXPORT bool OCRepPayloadAddInterfaceAsOwner(OCRepPayload* payload, char* iface);

OC_EXPORT bool OCRepPayloadIsNull(const OCRepPayload* payload, const char* name);
OC_EXPORT bool OCRepPayloadSetNull(OCRepPayload* payload, const char* name);

OC_EXPORT bool OCRepPayloadSetPropInt(OCRepPayload* payload, const char* name, int64_t value);
OC_EXPORT bool OCRepPayloadGetPropInt(const OCRepPayload* payload, const char* name, int64_t* value);

OC_EXPORT bool OCRepPayloadSetPropDouble(OCRepPayload* payload, const char* name, double value);
OC_EXPORT bool OCRepPayloadGetPropDouble(const OCRepPayload* payload, const char* name, double* value);

/**
 * This function allocates memory for the byte string and sets it in the payload.
 *
 * @param payload      Pointer to the payload to which byte string needs to be added.
 * @param name         Name of the byte string.
 * @param value        Byte string and it's length.
 *
 * @return true on success, false upon failure.
 */
OC_EXPORT bool OCRepPayloadSetPropByteString(OCRepPayload* payload, const char* name, OCByteString value);

/**
 * This function sets the byte string in the payload.
 *
 * @param payload      Pointer to the payload to which byte string needs to be added.
 * @param name         Name of the byte string.
 * @param value        Byte string and it's length.
 *
 * @return true on success, false upon failure.
 */
OC_EXPORT bool OCRepPayloadSetPropByteStringAsOwner(OCRepPayload* payload, const char* name,
        OCByteString* value);

/**
 * This function gets the byte string from the payload.
 *
 * @param payload      Pointer to the payload from which byte string needs to be retrieved.
 * @param name         Name of the byte string.
 * @param value        Byte string and it's length.
 *
 * @note: Caller needs to invoke OCFree on value.bytes after it is finished using the byte string.
 *
 * @return true on success, false upon failure.
 */
OC_EXPORT bool OCRepPayloadGetPropByteString(const OCRepPayload* payload, const char* name,
        OCByteString* value);

OC_EXPORT bool OCRepPayloadSetPropString(OCRepPayload* payload, const char* name, const char* value);
OC_EXPORT bool OCRepPayloadSetPropStringAsOwner(OCRepPayload* payload, const char* name, char* value);
OC_EXPORT bool OCRepPayloadGetPropString(const OCRepPayload* payload, const char* name, char** value);

OC_EXPORT bool OCRepPayloadSetPropBool(OCRepPayload* payload, const char* name, bool value);
OC_EXPORT bool OCRepPayloadGetPropBool(const OCRepPayload* payload, const char* name, bool* value);

OC_EXPORT bool OCRepPayloadSetPropObject(OCRepPayload* payload, const char* name, const OCRepPayload* value);
OC_EXPORT bool OCRepPayloadSetPropObjectAsOwner(OCRepPayload* payload, const char* name, OCRepPayload* value);
OC_EXPORT bool OCRepPayloadGetPropObject(const OCRepPayload* payload, const char* name, OCRepPayload** value);

/**
 * This function allocates memory for the byte string array and sets it in the payload.
 *
 * @param payload      Pointer to the payload to which byte string array needs to be added.
 * @param name         Name of the byte string.
 * @param array        Byte string array.
 * @param dimensions   Number of byte strings in above array.
 *
 * @return true on success, false upon failure.
 */
OC_EXPORT bool OCRepPayloadSetByteStringArrayAsOwner(OCRepPayload* payload, const char* name,
        OCByteString* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

/**
 * This function sets the byte string array in the payload.
 *
 * @param payload      Pointer to the payload to which byte string array needs to be added.
 * @param name         Name of the byte string.
 * @param array        Byte string array.
 * @param dimensions   Number of byte strings in above array.
 *
 * @return true on success, false upon failure.
 */
OC_EXPORT bool OCRepPayloadSetByteStringArray(OCRepPayload* payload, const char* name,
        const OCByteString* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

/**
 * This function gets the byte string array from the payload.
 *
 * @param payload      Pointer to the payload from which byte string array needs to be retrieved.
 * @param name         Name of the byte string array.
 * @param value        Byte string array.
 * @param dimensions   Number of byte strings in above array.
 *
 * @note: Caller needs to invoke OICFree on 'bytes' field of all array elements after it is
 *        finished using the byte string array.
 *
 * @return true on success, false upon failure.
 */
OC_EXPORT bool OCRepPayloadGetByteStringArray(const OCRepPayload* payload, const char* name,
        OCByteString** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

OC_EXPORT bool OCRepPayloadSetIntArrayAsOwner(OCRepPayload* payload, const char* name,
        int64_t* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
OC_EXPORT bool OCRepPayloadSetIntArray(OCRepPayload* payload, const char* name,
        const int64_t* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
OC_EXPORT bool OCRepPayloadGetIntArray(const OCRepPayload* payload, const char* name,
        int64_t** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

OC_EXPORT bool OCRepPayloadSetDoubleArrayAsOwner(OCRepPayload* payload, const char* name,
        double* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
OC_EXPORT bool OCRepPayloadSetDoubleArray(OCRepPayload* payload, const char* name,
        const double* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
OC_EXPORT bool OCRepPayloadGetDoubleArray(const OCRepPayload* payload, const char* name,
        double** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

OC_EXPORT bool OCRepPayloadSetStringArrayAsOwner(OCRepPayload* payload, const char* name,
        char** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
OC_EXPORT bool OCRepPayloadSetStringArray(OCRepPayload* payload, const char* name,
        const char** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
OC_EXPORT bool OCRepPayloadGetStringArray(const OCRepPayload* payload, const char* name,
        char*** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

OC_EXPORT bool OCRepPayloadSetBoolArrayAsOwner(OCRepPayload* payload, const char* name,
        bool* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
OC_EXPORT bool OCRepPayloadSetBoolArray(OCRepPayload* payload, const char* name,
        const bool* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
OC_EXPORT bool OCRepPayloadGetBoolArray(const OCRepPayload* payload, const char* name,
        bool** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

OC_EXPORT bool OCRepPayloadSetPropObjectArrayAsOwner(OCRepPayload* payload, const char* name,
        OCRepPayload** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
OC_EXPORT bool OCRepPayloadSetPropObjectArray(OCRepPayload* payload, const char* name,
        const OCRepPayload** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
OC_EXPORT bool OCRepPayloadGetPropObjectArray(const OCRepPayload* payload, const char* name,
        OCRepPayload*** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

OC_EXPORT void OCRepPayloadDestroy(OCRepPayload* payload);

// Discovery Payload
OC_EXPORT OCDiscoveryPayload* OCDiscoveryPayloadCreate();

OC_EXPORT OCSecurityPayload* OCSecurityPayloadCreate(const uint8_t* securityData, size_t size);
OC_EXPORT void OCSecurityPayloadDestroy(OCSecurityPayload* payload);

#ifndef TCP_ADAPTER
OC_EXPORT void OCDiscoveryPayloadAddResource(OCDiscoveryPayload* payload, const OCResource* res,
                                             uint16_t securePort);
#else
OC_EXPORT void OCDiscoveryPayloadAddResource(OCDiscoveryPayload* payload, const OCResource* res,
                                             uint16_t securePort, uint16_t tcpPort);
#endif
OC_EXPORT void OCDiscoveryPayloadAddNewResource(OCDiscoveryPayload* payload, OCResourcePayload* res);
OC_EXPORT bool OCResourcePayloadAddStringLL(OCStringLL **payload, const char* type);

OC_EXPORT size_t OCDiscoveryPayloadGetResourceCount(OCDiscoveryPayload* payload);
OC_EXPORT OCResourcePayload* OCDiscoveryPayloadGetResource(OCDiscoveryPayload* payload, size_t index);

OC_EXPORT void OCDiscoveryResourceDestroy(OCResourcePayload* payload);
OC_EXPORT void OCDiscoveryPayloadDestroy(OCDiscoveryPayload* payload);

// Device Payload
OC_EXPORT OCDevicePayload* OCDevicePayloadCreate(const char* sid, const char* dname,
        const OCStringLL *types, const char* specVer, const char* dmVer);
OC_EXPORT void OCDevicePayloadDestroy(OCDevicePayload* payload);

// Platform Payload
OC_EXPORT OCPlatformPayload* OCPlatformPayloadCreate(const OCPlatformInfo* platformInfo);
OC_EXPORT OCPlatformPayload* OCPlatformPayloadCreateAsOwner(OCPlatformInfo* platformInfo);
OC_EXPORT void OCPlatformInfoDestroy(OCPlatformInfo *info);
OC_EXPORT void OCPlatformPayloadDestroy(OCPlatformPayload* payload);

// Presence Payload
OC_EXPORT OCPresencePayload* OCPresencePayloadCreate(uint32_t seqNum, uint32_t maxAge,
        OCPresenceTrigger trigger, const char* resourceType);
OC_EXPORT void OCPresencePayloadDestroy(OCPresencePayload* payload);

// Helper API
OC_EXPORT OCStringLL* CloneOCStringLL (OCStringLL* ll);
OC_EXPORT void OCFreeOCStringLL(OCStringLL* ll);

/**
 * This function creates a list from a string (with separated contents if several)
 * @param text         single string or CSV text fields
 * @return newly allocated linked list
 * @note separator is ',' (according to rfc4180, ';' is not valid)
 **/
OC_EXPORT OCStringLL* OCCreateOCStringLL(const char* text);

/**
 * This function creates a string from a list (with separated contents if several)
 * @param ll           Pointer to list
 * @return newly allocated string
 * @note separator is ',' (according to rfc4180)
 **/
OC_EXPORT char* OCCreateString(const OCStringLL* ll);

#ifdef __cplusplus
}
#endif

#endif
