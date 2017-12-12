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


/*
 * Expose POSIX.1-2008 base specification,
 * Refer http://pubs.opengroup.org/onlinepubs/9699919799/
 * For this specific file, see use of clock_gettime,
 * Refer to http://pubs.opengroup.org/stage7tc1/functions/clock_gettime.html
 * and to http://man7.org/linux/man-pages/man2/clock_gettime.2.html
 */
#define _POSIX_C_SOURCE 200809L

#include "oic_time_windows.h"

#include <stddef.h>        /* For NULL */
#ifdef HAVE_UNISTD_H
#include <unistd.h>	   /* for _POSIX_TIMERS */
#endif

#if EXPORT_INTERFACE
#ifdef HAVE_WINDOWS_H
# include <winsock2.h>
# include <windows.h>
# define HAVE_QUERYPERFORMANCEFREQUENCY
#elif _POSIX_TIMERS > 0
#  include <time.h>        // For clock_gettime()
#else
#  include <sys/time.h>    // For gettimeofday()
#endif
#endif

#define TAG "OIC_TIME_WINDOWS"

/* /\** */
/*  * @name */
/*  * Useful constants for time unit conversions. */
/*  * */
/*  * @{ */
/*  *\/ */
/* #if EXPORT_INTERFACE */
/* #define MS_PER_SEC  (1000) */
/* #define US_PER_SEC  (1000000) */
/* #define US_PER_MS   (1000) */
/* #define NS_PER_US   (1000) */
/* #define NS_PER_MS   (1000000) */
/* #define HNS_PER_US  (10) */
/* #endif	/\* INTERFACE *\/ */
/* /\** @} *\/ */


/* #if EXPORT_INTERFACE */
/* #include <stdint.h> */
/* typedef enum */
/* { */
/*     TIME_IN_MS = 0,     //!< milliseconds */
/*     TIME_IN_US,         //!< microseconds */
/* } OICTimePrecision; */
/* #endif	/\* EXPORT_INTERFACE *\/ */

uint64_t OICGetCurrentTime(OICTimePrecision precision)
{
    uint64_t currentTime = 0;

#ifdef _WIN32
    static LARGE_INTEGER frequency = {0};

    if (!frequency.QuadPart)
    {
        QueryPerformanceFrequency(&frequency);
    }

    LARGE_INTEGER count = {0};
    QueryPerformanceCounter(&count);

    currentTime =
    (TIME_IN_MS == precision)
        ? count.QuadPart / (frequency.QuadPart / MS_PER_SEC)
        : count.QuadPart / (frequency.QuadPart / US_PER_SEC);
#else
# if _POSIX_TIMERS > 0
#   if defined(CLOCK_MONOTONIC_COARSE)
    static const clockid_t clockId = CLOCK_MONOTONIC_COARSE;
#   elif _POSIX_MONOTONIC_CLOCK >= 0
    // Option _POSIX_MONOTONIC_CLOCK == 0 indicates that the option is
    // available at compile time but may not be supported at run
    // time.  Check if option _POSIX_MONOTONIC_CLOCK is supported at
    // run time.
#     if _POSIX_MONOTONIC_CLOCK == 0
    static const clockid_t clockId =
        sysconf(_SC_MONOTONIC_CLOCK) > 0 ? CLOCK_MONOTONIC : CLOCK_REALTIME;
#     else
    static const clockid_t clockId = CLOCK_MONOTONIC;
#     endif  // _POSIX_MONOTONIC_CLOCK == 0
#   else
    static const clockid_t clockId = CLOCK_REALTIME;
#   endif  // CLOCK_MONOTONIC_COARSE

    struct timespec current = { .tv_sec = 0, .tv_nsec = 0 };
    if (clock_gettime(clockId, &current) == 0)
    {
        currentTime =
            (TIME_IN_MS == precision)
            ? (((uint64_t) current.tv_sec * MS_PER_SEC) + (current.tv_nsec / NS_PER_MS))
            : (((uint64_t) current.tv_sec * US_PER_SEC) + (current.tv_nsec / NS_PER_US));
    }
# else
    struct timeval current = { .tv_sec = 0, .tv_usec = 0 };
    if (gettimeofday(&current, NULL) == 0)
    {
        currentTime =
            (TIME_IN_MS == precision)
            ? (((uint64_t) current.tv_sec * MS_PER_SEC) + (current.tv_usec / US_PER_MS))
            : (((uint64_t) current.tv_sec * US_PER_SEC) + (current.tv_usec));
    }
# endif  // _POSIX_TIMERS > 0
#endif  // _WIN32

    return currentTime;
}
