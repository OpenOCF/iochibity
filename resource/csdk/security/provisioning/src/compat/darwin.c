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
/* #include <string.h> */

#include <sys/time.h>

#include "ocstack.h"
/* #include "oic_malloc.h" */
/* #include "oic_string.h" */

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
    static const struct timeval zeroTime = { .tv_sec = 0, .tv_usec = 0 };
    struct timeval startTime = { .tv_sec = 0, .tv_usec = 0 };
    struct timeval currTime = { .tv_sec = 0, .tv_usec = 0 };
    int clock_res = gettimeofday(&startTime, NULL);	//FIXME:  use mach_absolute_time?
    if (0 != clock_res)
    {
        return OC_STACK_ERROR;
    }
    OCStackResult res = OC_STACK_OK;
    while (OC_STACK_OK == res)
    {
        currTime = zeroTime;
        clock_res = gettimeofday(&currTime, NULL);
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
