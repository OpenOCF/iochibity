//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

/*
 * Expose POSIX.1-2008 base specification,
 * Refer http://pubs.opengroup.org/onlinepubs/9699919799/
 * For this specific file, see use of clock_gettime,
 * Refer to http://pubs.opengroup.org/stage7tc1/functions/clock_gettime.html
 * and to http://man7.org/linux/man-pages/man2/clock_gettime.2.html
 */
#define _POSIX_C_SOURCE 200809L

/* #include "iotivity_config.h" */
/* #include "logger.h" */
#include "ocrandom.h"

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif
#if defined(__ANDROID__)
#include <ctype.h>
#endif
#if EXPORT_INTERFACE
#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#include <windows.h>
#include <Bcrypt.h>
/* #endif */

/* /\* http://kirkshoop.blogspot.com/2011/09/ntstatus.html *\/ */
/* #define WIN32_NO_STATUS */
/* #include <windows.h> */
/* #undef WIN32_NO_STATUS */
/* #include <winternl.h> */
/* #include <ntstatus.h> */
#endif	/* HAVE_WINDOWS_H */
#endif	/* EXPORT_INTERFACE */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

#define OC_MIN(A,B) ((A)<(B)?(A):(B))


#if EXPORT_INTERFACE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

 /* Number of bytes in a UUID. */
#define UUID_SIZE (16)

/*
 * Size of a UUID string.
 * IoTivity formats UUIDs as strings following RFC 4122, Section 3.
 * For example, "f81d4fae-7dec-11d0-a765-00a0c91e6bf6".
 * This requires 36 characters, plus one for the null terminator.
 */
#define UUID_STRING_SIZE (37)
#endif

#define OC_UUID_HYPHEN_1 9
#define OC_UUID_HYPHEN_2 14
#define OC_UUID_HYPHEN_3 19
#define OC_UUID_HYPHEN_4 24
#define OC_UUID_HYPHEN_COUNT 4

/**
* @def OCRANDOM_TAG
* @brief Logging tag for module name
*/
#define OCRANDOM_TAG "OIC_OCRANDOM"


bool OCGetRandomBytes(uint8_t * output, size_t len)
{
    if ( (output == NULL) || (len == 0) )
    {
        return false;
    }

#if defined(__unix__) || defined(__APPLE__)
    FILE* urandom = fopen("/dev/urandom", "r");
    if (urandom == NULL)
    {
        OIC_LOG(FATAL, OCRANDOM_TAG, "Failed open /dev/urandom!");
        assert(false);
        return false;
    }

    if (fread(output, sizeof(uint8_t), len, urandom) != len)
    {
        OIC_LOG(FATAL, OCRANDOM_TAG, "Failed while reading /dev/urandom!");
        assert(false);
        fclose(urandom);
        return false;
    }
    fclose(urandom);

#elif defined(_WIN32)
    /*
     * size_t may be 64 bits, but ULONG is always 32.
     * If len is larger than the maximum for ULONG, just fail.
     * It's unlikely anything ever will want to ask for this much randomness.
     */
    if (len > 0xFFFFFFFFULL)
    {
        OIC_LOG(FATAL, OCRANDOM_TAG, "Requested number of bytes too large for ULONG");
        assert(false);
        return false;
    }

    NTSTATUS status = BCryptGenRandom(NULL, output, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!BCRYPT_SUCCESS(status))
    {
        OIC_LOG_V(FATAL, OCRANDOM_TAG, "BCryptGenRandom failed (%X)!", status);
        assert(false);
        return false;
    }

#elif defined(ARDUINO)
    if (!g_isSeeded)
    {
        OCSeedRandom();
    }

    size_t i;
    for (i = 0; i < len; i++)
    {
        output[i] = OC_arduino_random_function() & 0x00ff;
    }

#else
    #error Unrecognized platform
#endif

    return true;
}

uint32_t OCGetRandom()
{
    uint32_t result = 0;
    if (!OCGetRandomBytes((uint8_t*)&result, sizeof(result)))
    {
        OIC_LOG(FATAL, OCRANDOM_TAG, "OCGetRandom failed!");
        assert(false);
    }
    return result;
}

/* Return the number of leading zeroes in x.
 * Binary search algorithm from Section 5-3 of:
 *     H.S. Warren Jr. Hacker's Delight. Addison-Wesley. 2003.
 */
static int nlz(uint32_t x)
{
    if (x == 0)
    {
        return 32;
    }

    int n = 0;
    if (x <= 0x0000FFFF) { n = n + 16; x = x << 16;}
    if (x <= 0x00FFFFFF) { n = n + 8;  x = x << 8; }
    if (x <= 0x0FFFFFFF) { n = n + 4;  x = x << 4; }
    if (x <= 0x3FFFFFFF) { n = n + 2;  x = x << 2; }
    if (x <= 0x7FFFFFFF) { n = n + 1;}

    return n;
}

uint32_t OCGetRandomRange(uint32_t firstBound, uint32_t secondBound)
{
    if (firstBound == secondBound)
    {
        return secondBound;
    }

    uint32_t rangeBase = OC_MIN(firstBound, secondBound);
    uint32_t rangeWidth = (firstBound > secondBound) ? (firstBound - secondBound) : (secondBound - firstBound);

    /*
     * Compute a random number between 0 and rangeWidth. Avoid using floating
     * point types to avoid overflow when rangeWidth is large. The condition
     * in the while loop will be false with probability at least 1/2.
     */
    uint32_t rangeMask = 0xFFFFFFFF >> nlz(rangeWidth);
    uint32_t offset = 0;
    do
    {
        if(!OCGetRandomBytes((uint8_t*)&offset, sizeof(offset)))
        {
            OIC_LOG(FATAL, OCRANDOM_TAG, "OCGetRandomBytes failed");
            assert(false);
            return rangeBase;
        }
        offset = offset & rangeMask;
    }
    while (offset > rangeWidth);

    return rangeBase + offset;
}

bool OCGenerateUuid(uint8_t uuid[UUID_SIZE])
{
    if (!uuid)
    {
        OIC_LOG(ERROR, OCRANDOM_TAG, "Invalid parameter");
        return false;
    }

    return OCGetRandomBytes(uuid, UUID_SIZE);
}

bool OCConvertUuidToString(const uint8_t uuid[UUID_SIZE],
                                         char uuidString[UUID_STRING_SIZE])
{
    if (uuid == NULL || uuidString == NULL)
    {
        OIC_LOG(ERROR, OCRANDOM_TAG, "Invalid parameter");
        return false;
    }


    int ret = snprintf(uuidString, UUID_STRING_SIZE,
            "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid[0], uuid[1], uuid[2], uuid[3],
            uuid[4], uuid[5], uuid[6], uuid[7],
            uuid[8], uuid[9], uuid[10], uuid[11],
            uuid[12], uuid[13], uuid[14], uuid[15]
            );

    if (ret != UUID_STRING_SIZE - 1)
    {
        OIC_LOG(ERROR, OCRANDOM_TAG, "snprintf failed");
        return false;
    }

    return true;
}

bool OCConvertStringToUuid(const char uuidString[UUID_STRING_SIZE],
                                         uint8_t uuid[UUID_SIZE])
{
    if(NULL == uuidString || NULL == uuid)
    {
        OIC_LOG(ERROR, OCRANDOM_TAG, "Invalid parameter");
        return false;
    }

    size_t urnIdx = 0;
    size_t uuidIdx = 0;
    size_t strUuidLen = 0;
    char convertedUuid[UUID_SIZE * 2] = {0};

    strUuidLen = strlen(uuidString);
    if((UUID_STRING_SIZE - 1) == strUuidLen)
    {
        for(uuidIdx=0, urnIdx=0; uuidIdx < UUID_SIZE ; uuidIdx++, urnIdx+=2)
        {
            if(*(uuidString + urnIdx) == '-')
            {
                urnIdx++;
            }
            sscanf(uuidString + urnIdx, "%2hhx", &convertedUuid[uuidIdx]);
        }
    }
    else
    {
        OIC_LOG(ERROR, OCRANDOM_TAG, "unexpected string length");
        return false;
    }

    memcpy(uuid, convertedUuid, UUID_SIZE);

    return true;
}

bool OCIsUUID(const char *uuid)
{
    size_t hyphens[OC_UUID_HYPHEN_COUNT] = {OC_UUID_HYPHEN_1, OC_UUID_HYPHEN_2,
                                            OC_UUID_HYPHEN_3, OC_UUID_HYPHEN_4};
    if (strlen(uuid) != UUID_STRING_SIZE -1)
    {
        //The length doesn't match
        return false;
    }

    for (size_t i = 0, counter = 0; i < (UUID_STRING_SIZE - 1); i++)
    {
        char var = uuid[i];

        //Check if a hyphen is expected here.
        if (i == (hyphens[counter] - 1))
        {
            //We need a hyphen here.
            if ('-' != var)
            {
                //The character is not a hyphen.
                return false;
            }
            else
            {
                //Move on to the next expected hyphen position.
                counter++;
            }
        }
        else
        {
            //The character here should be a simple xdigit
            if (0 == isxdigit(var))
            {
                //The current character is not a xdigit.
                return false;
            }
        }
    }

    return true;
}
