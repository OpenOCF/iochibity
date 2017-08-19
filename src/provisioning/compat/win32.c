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
#if (_POSIX_TIMERS > 0)
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "ocstack.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "logger.h"
#include "cJSON.h"
#include "utlist.h"
#include "ocpayload.h"

#include "securevirtualresourcetypes.h"
#include "srmresourcestrings.h" //@note: SRM's internal header
#include "doxmresource.h"       //@note: SRM's internal header
#include "pstatresource.h"      //@note: SRM's internal header
#include "verresource.h"      //@note: SRM's internal header

#include "pmtypes.h"
#include "pmutility.h"

#include "srmutility.h"

#define TAG ("PM-UTILITY")

#define HNS_TO_S(VAL)  ((VAL)/(10*1000*1000))

/**
 * Windows Timeout implementation for secure discovery. When performing secure discovery,
 * we should wait a certain period of time for getting response of each devices.
 *
 * @param[in]  waittime  Timeout in seconds.
 * @param[in]  waitForStackResponse if true timeout function will call OCProcess while waiting.
 * @return OC_STACK_OK on success otherwise error.
 */
OCStackResult PMTimeout(unsigned short waittime, bool waitForStackResponse)
{
    OCStackResult res = OC_STACK_OK;
    FILETIME startTime = {0};
    FILETIME currTime = {0};
    GetSystemTimeAsFileTime(&startTime);
    if (0 != clock_res)
    {
        return OC_STACK_ERROR;
    }

    OCStackResult res = OC_STACK_OK;
    while (OC_STACK_OK == res)
    {
        currTime = zeroTime;
        GetSystemTimeAsFileTime(&currTime);
        if (0 != clock_res)
        {
            return OC_STACK_TIMEOUT;
        }
        ULARGE_INTEGER currTimeInt;
        ULARGE_INTEGER startTimeInt;

        currTimeInt.LowPart  = currTime.dwLowDateTime;
        currTimeInt.HighPart = currTime.dwHighDateTime;

        startTimeInt.LowPart  = startTime.dwLowDateTime;
        startTimeInt.HighPart = startTime.dwHighDateTime;

        long elapsed = (long)HNS_TO_S(currTimeInt.QuadPart - startTimeInt.QuadPart);
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
