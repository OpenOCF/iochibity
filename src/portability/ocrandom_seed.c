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

#ifndef _POSIX_TIMERS
#include <sys/time.h>
#elif  _POSIX_TIMERS <= 0
#include <sys/time.h>
#endif
/* #ifdef HAVE_TIME_H */
/* #include <time.h> */
/* #endif */
/* #if defined(__ANDROID__) */
/* #include <ctype.h> */
/* #include <linux/time.h> */
/* #endif */

#include "ocrandom.h"

#define NANO_SEC 1000000000

// this is only used once, in rsource/connectivity/src/caconnectivitymanager.c:CAInitialize()
// NB: this is too trivial to merit platform-specific files; select on a macro:
#ifdef WINDOWS			/* FIXME: HAVE_WINDOWS_H? */
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
#else
int8_t OCSeedRandom()
{
#ifdef HAVE_ARC4RANDOM_STIR_FN
    arc4random_stir();
#else
    // Get current time to Seed.
    uint64_t currentTime = 0;
#if  _POSIX_TIMERS > 0	/* this takes care of android */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    currentTime = (ts.tv_sec * (uint64_t)NANO_SEC + ts.tv_nsec)/ 1000;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    currentTime = tv.tv_sec * (uint64_t)1000000 + tv.tv_usec;

    int32_t fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0)
    {
        uint32_t randomSeed = 0;
        uint32_t totalRead = 0; //how many integers were read
        int32_t currentRead = 0;
        while (totalRead < sizeof(randomSeed))
        {
            currentRead = read(fd, (uint8_t*) &randomSeed + totalRead,
                    sizeof(randomSeed) - totalRead);
            if (currentRead > 0)
            {
                totalRead += currentRead;
            }
        }
        close(fd);
        srand(randomSeed | currentTime);
    }
    else
    {
        // Do time based seed when problem in accessing "/dev/urandom"
        srand(currentTime);
    }

    return 0;
#endif // _POSIX_TIMERS <= 0
#endif // arc4random
}
#endif
