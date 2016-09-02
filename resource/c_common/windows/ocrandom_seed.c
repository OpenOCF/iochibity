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

#ifndef _POSIX_TIMERS
#include <sys/time.h>
#elif  _POSIX_TIMERS <= 0
#include <sys/time.h>
#endif
/* #ifdef HAVE_WINDOWS_H */
/* #include <windows.h> */
/* #endif */

#include "../ocrandom.h"

#define NANO_SEC 1000000000

// this is only used once, in rsource/connectivity/src/caconnectivitymanager.c:CAInitialize()
int8_t OCSeedRandom()
{
#ifdef HAVE_ARC4RANDOM_STIR_FN
    arc4random_stir();
#else
    // Get current time to Seed.
    uint64_t currentTime = 0;
    LARGE_INTEGER count;
    if (QueryPerformanceCounter(&count)) {
        currentTime = count.QuadPart;
    }
    {
        srand(currentTime);
    }

    return 0;
#endif
}
