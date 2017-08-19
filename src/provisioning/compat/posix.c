/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <unistd.h>
#include <string.h>
#include <time.h>

#include "ocstack.h"

/**
 * Timeout implementation for secure discovery. When performing secure discovery,
 * we should wait a certain period of time for getting response of each devices.
 *
 * @param[in]  waittime  Timeout in seconds.
 * @param[in]  waitForStackResponse if true timeout function will call OCProcess while waiting.
 * @return OC_STACK_OK on success otherwise error.
 */
OCStackResult PMTimeout(unsigned short waittime, bool waitForStackResponse)
{
    OCStackResult res = OC_STACK_OK;

    struct timespec startTime = {.tv_sec=0, .tv_nsec=0};
    struct timespec currTime  = {.tv_sec=0, .tv_nsec=0};
# if defined(_POSIX_MONOTONIC_CLOCK)
    int clock_res = clock_gettime(CLOCK_MONOTONIC, &startTime);
# else
    int clock_res = clock_gettime(CLOCK_REALTIME, &startTime);
# endif

    if (0 != clock_res)
    {
        return OC_STACK_ERROR;
    }

    OCStackResult res = OC_STACK_OK;
    while (OC_STACK_OK == res)
    {
        currTime = zeroTime;
#if (_POSIX_TIMERS > 0)
# if defined(_POSIX_MONOTONIC_CLOCK)
        clock_res = clock_gettime(CLOCK_MONOTONIC, &currTime);
# else
        clock_res = clock_gettime(CLOCK_REALTIME, &currTime);
# endif
        if (0 != clock_res)
        {
            return OC_STACK_TIMEOUT;
        }
        long elapsed = (currTime.tv_sec - startTime.tv_sec);
        if (elapsed > waittime)
        {
            return OC_STACK_OK;
        }
        if (waitForStackResponse)
        {
            res = OCProcess();
        }
    }
    return res;
}
