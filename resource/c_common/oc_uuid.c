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

// Defining _POSIX_C_SOURCE macro with 199309L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1b, Real-time extensions
// (IEEE Std 1003.1b-1993) specification
//
// For this specific file, see use of clock_gettime,
// Refer to http://pubs.opengroup.org/stage7tc1/functions/clock_gettime.html
// and to http://man7.org/linux/man-pages/man2/clock_gettime.2.html
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "platform_features.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "logger.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#ifdef HAVE_UUID_UUID_H
#include <uuid/uuid.h>
#endif

#include "oc_uuid.h"

#define NANO_SEC 1000000000

OCRandomUuidResult OCGenerateUuid(uint8_t uuid[UUID_SIZE])
{
    /* printf("GAR: %s ENTRY", __func__); */
    if (!uuid)
    {
        return RAND_UUID_INVALID_PARAM;
    }
#if defined(HAVE_UUID_UUID_H)
    // note: uuid_t is typedefed as unsigned char[16] on linux/apple
    uuid_generate(uuid);
    return RAND_UUID_OK;
#else
    // Fallback for all platforms is filling the array with random data
    OCFillRandomMem(uuid, UUID_SIZE);
    return RAND_UUID_OK;
#endif
}

OCRandomUuidResult OCGenerateUuidString(char uuidString[UUID_STRING_SIZE])
{
    if (!uuidString)
    {
        return RAND_UUID_INVALID_PARAM;
    }
    /* GAR FIXME: this should be feature tested since any linux has it but not e.g. freebsd */
#if defined(__ANDROID__)	/* GAR: test for __linux__ instead?  */
    int32_t fd = open("/proc/sys/kernel/random/uuid", O_RDONLY);
    if (fd > 0)
    {
        ssize_t readResult = read(fd, uuidString, UUID_STRING_SIZE - 1);
        close(fd);
        if (readResult < 0)
        {
            return RAND_UUID_READ_ERROR;
        }
        else if (readResult < UUID_STRING_SIZE - 1)
        {
            uuidString[0] = '\0';
            return RAND_UUID_READ_ERROR;
        }

        uuidString[UUID_STRING_SIZE - 1] = '\0';
        for (char* p = uuidString; *p; ++p)
        {
            *p = tolower(*p);
        }
        return RAND_UUID_OK;
    }
    else
    {
        close(fd);
        return RAND_UUID_READ_ERROR;
    }
#elif defined(HAVE_UUID_UUID_H)
    uint8_t uuid[UUID_SIZE];
    int8_t ret = OCGenerateUuid(uuid);

    if (ret != 0)
    {
        return ret;
    }

    uuid_unparse_lower(uuid, uuidString);
    return RAND_UUID_OK;

#else
    uint8_t uuid[UUID_SIZE];
    OCGenerateUuid(uuid);

    return OCConvertUuidToString(uuid, uuidString);
#endif
}

OCRandomUuidResult OCConvertUuidToString(const uint8_t uuid[UUID_SIZE],
                                         char uuidString[UUID_STRING_SIZE])
{
    if (uuid == NULL || uuidString == NULL)
    {
        return RAND_UUID_INVALID_PARAM;
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
        return RAND_UUID_CONVERT_ERROR;
    }

    return RAND_UUID_OK;
}
