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

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#elif defined(HAVE_TIME_H)
#include <time.h>
#endif
/*GAR #if defined(__ANDROID__) */
/* #include <ctype.h> */
/* #include <linux/time.h> */
/* #endif */
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#include "ocrandom.h"

#define NANO_SEC 1000000000

void OCFillRandomMem(uint8_t * location, uint16_t len)
{
#ifdef HAVE_ARC4RANDOM_BUF_FN
    arc4random_buf((void*)location, (size_t)len);
#else
    if (!location)
    {
        return;
    }
    for (; len--;)
    {
        *location++ = OCGetRandomByte();
    }
#endif
}

uint32_t OCGetRandom()
{
#ifdef HAVE_ARC4RANDOM_FN
    return arc4random();
#else
    uint32_t result = 0;
    OCFillRandomMem((uint8_t*) &result, 4);
    return result;
#endif
}

uint8_t OCGetRandomByte(void)
{
#ifdef HAVE_ARC4RANDOM_FN
    return arc4random() & 0x00FF;
#elif HAVE_SRANDOM
    return random() & 0x00FF;
#else
    return rand() & 0x00FF;
#endif
}

uint32_t OCGetRandomRange(uint32_t firstBound, uint32_t secondBound)
{
    uint32_t base;
    uint32_t diff;
    uint32_t result;
    if (firstBound > secondBound)
    {
        base = secondBound;
        diff = firstBound - secondBound;
    }
    else if (firstBound < secondBound)
    {
        base = firstBound;
        diff = secondBound - firstBound;
    }
    else
    {
        return secondBound;
    }
    result = ((float)OCGetRandom()/((float)(0xFFFFFFFF))*(float)diff) + (float) base;
    return result;
}
