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

#include "../platform_features.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "../ocrandom.h"

#define NANO_SEC 1000000000

#include "Arduino.h"

// ARM GCC compiler doesnt define srandom function.
#if defined(ARDUINO) && !defined(ARDUINO_ARCH_SAM)
#define HAVE_SRANDOM 1
#endif

uint8_t GetRandomBitRaw()
{
    return analogRead((uint8_t)ANALOG_IN) & 0x1;
}

uint8_t GetRandomBitRaw2()
{
    int a = 0;
    for (;;)
    {
        a = GetRandomBitRaw() | (GetRandomBitRaw()<<1);
        if (a==1)
        {
            return 0; // 1 to 0 transition: log a zero bit
        }
        if (a==2)
        {
            return 1;// 0 to 1 transition: log a one bit
        }
        // For other cases, try again.
    }
}

uint8_t GetRandomBit()
{
    int a = 0;
    for (;;)
    {
        a = GetRandomBitRaw2() | (GetRandomBitRaw2()<<1);
        if (a==1)
        {
            return 0; // 1 to 0 transition: log a zero bit
        }
        if (a==2)
        {
            return 1;// 0 to 1 transition: log a one bit
        }
        // For other cases, try again.
    }
}

// this is only used once, in rsource/connectivity/src/caconnectivitymanager.c:CAInitialize()
int8_t OCSeedRandom()
{
#ifdef HAVE_ARC4RANDOM_STIR_FN
    arc4random_stir();
#else
    uint32_t result =0;
    uint8_t i;
    for (i=32; i--;)
    {
        result += result + GetRandomBit();
    }
#if HAVE_SRANDOM
    srandom(result);
#else
    srand(result);
#endif
    return 0;
#endif
}

