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
#include <stddef.h>        /* For NULL */
#ifdef HAVE_UNISTD_H
#include <unistd.h>	   /* for _POSIX_TIMERS */
#endif

#if EXPORT_INTERFACE
#ifdef HAVE_TIME_H
#  include <time.h>        // For clock_gettime()
#endif
#ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>    // For gettimeofday()
#endif
#endif

/* #define TAG "OIC_TIME" */

/**
 * @name
 * Useful constants for time unit conversions.
 *
 * @{
 */
/* FIXME: eliminate redundancies */
#include <stdint.h>
#define MS_PER_SEC  (1000)
#define MSECS_PER_SEC MS_PER_SEC
#define US_PER_SEC  (1000000)
#define USECS_PER_SEC US_PER_SEC
#define US_PER_MS   (1000)
#define USECS_PER_MSEC US_PER_MS
#define NS_PER_US   (1000)
#define NANOSECS_PER_USECS NS_PER_US
#define NS_PER_MS   (1000000)
#define NANOSECS_PER_SEC (1000000000L)
#define HNS_PER_US  (10)
/** @} */

/* const uint64_t USECS_PER_SEC      = 1000000; */
/* const uint64_t MSECS_PER_SEC      = 1000; */
/* const uint64_t NANOSECS_PER_SEC   = 1000000000L; */
/* const uint64_t USECS_PER_MSEC     = 1000; */
/* const uint64_t NANOSECS_PER_USECS = 1000; */

#if EXPORT_INTERFACE
#include <stdint.h>
typedef enum
{
    TIME_IN_MS = 0,     //!< milliseconds
    TIME_IN_US,         //!< microseconds
} OICTimePrecision;
#endif	/* EXPORT_INTERFACE */
