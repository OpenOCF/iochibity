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

#include "ocrandom.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#if INTERFACE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define UUID_SIZE (16)		/* in bytes */

/*
 * IoTivity formats UUIDs as strings following RFC 4122, Section 3.
 * For example, "f81d4fae-7dec-11d0-a765-00a0c91e6bf6".
 * This requires 36 characters, plus one for the null terminator.
 */
#define UUID_STRING_SIZE (37)

#define OC_UUID_HYPHEN_1 9
#define OC_UUID_HYPHEN_2 14
#define OC_UUID_HYPHEN_3 19
#define OC_UUID_HYPHEN_4 24
#define OC_UUID_HYPHEN_COUNT 4
#endif

#define OC_MIN(A,B) ((A)<(B)?(A):(B))

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
