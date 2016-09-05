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
//
//
//*********************************************************************

/**
 * @file
 * This file provides APIs related to mutex and semaphores.
 */

// Defining _POSIX_C_SOURCE macro with 199309L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1b, Real-time extensions
// (IEEE Std 1003.1b-1993) specification
//
// For this specific file, see use of clock_gettime and PTHREAD_MUTEX_DEFAULT
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "oic_malloc.h"
#include "platform_features.h"
#include "camutex.h"
#include "oic_time.h"
#include "logger.h"

/**
 * TAG
 * Logging tag for module name
 */
#define TAG PCF("UMUTEX")

static const uint64_t USECS_PER_SEC         = 1000000;
static const uint64_t NANOSECS_PER_USECS    = 1000;
static const uint64_t NANOSECS_PER_SEC      = 1000000000L;

typedef struct _tagMutexInfo_t
{
#if defined(_WIN32)
    CRITICAL_SECTION mutex;
#else
    pthread_mutex_t mutex;
#endif
} ca_mutex_internal;

typedef struct _tagEventInfo_t
{
#if defined(_WIN32)
    CONDITION_VARIABLE cond;
#else
    pthread_cond_t cond;
    pthread_condattr_t condattr;
#endif
} ca_cond_internal;

ca_mutex ca_mutex_new(void)
{
    ca_mutex retVal = NULL;
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) OICMalloc(sizeof(ca_mutex_internal));
    if (NULL != mutexInfo)
    {
#if defined(_WIN32)
        InitializeCriticalSection(&mutexInfo->mutex);
        retVal = (ca_mutex)mutexInfo;
#else
        // create the mutex with the attributes set
        int ret=pthread_mutex_init(&(mutexInfo->mutex), PTHREAD_MUTEX_DEFAULT);
        if (0 == ret)
        {
            retVal = (ca_mutex) mutexInfo;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s Failed to initialize mutex !", __func__);
            OICFree(mutexInfo);
        }
#endif
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Failed to allocate mutex!", __func__);
    }

    return retVal;
}

bool ca_mutex_free(ca_mutex mutex)
{
    bool bRet=false;

    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;
    if (mutexInfo)
    {
#if defined(_WIN32)
        DeleteCriticalSection(&mutexInfo->mutex);
        OICFree(mutexInfo);
        bRet=true;
#else
        int ret = pthread_mutex_destroy(&mutexInfo->mutex);
        if (0 == ret)
        {
            OICFree(mutexInfo);
            bRet=true;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s Failed to free mutex !", __func__);
        }
#endif
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Invalid mutex !", __func__);
    }

    return bRet;
}

void ca_mutex_lock(ca_mutex mutex)
{
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;
    if (mutexInfo)
    {
#if defined(_WIN32)
        EnterCriticalSection(&mutexInfo->mutex);
#else
        int ret = pthread_mutex_lock(&mutexInfo->mutex);
        if(ret != 0)
        {
            OIC_LOG_V(ERROR, TAG, "Pthread Mutex lock failed: %d", ret);
            exit(ret);
        }
#endif
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Invalid mutex !", __func__);
        return;
    }
}

void ca_mutex_unlock(ca_mutex mutex)
{
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;
    if (mutexInfo)
    {
#if defined(_WIN32)
        LeaveCriticalSection(&mutexInfo->mutex);
#else
        int ret = pthread_mutex_unlock(&mutexInfo->mutex);
        if(ret != 0)
        {
            OIC_LOG_V(ERROR, TAG, "Pthread Mutex unlock failed: %d", ret);
            exit(ret);
        }
        (void)ret;
#endif
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid mutex !", __func__);
        return;
    }
}

ca_cond ca_cond_new(void)
{
    ca_cond retVal = NULL;
    ca_cond_internal *eventInfo = (ca_cond_internal*) OICMalloc(sizeof(ca_cond_internal));
    if (NULL != eventInfo)
    {
#if defined(_WIN32)
        InitializeConditionVariable(&eventInfo->cond);
        retVal = (ca_cond) eventInfo;
#else
        int ret = pthread_condattr_init(&(eventInfo->condattr));
        if(0 != ret)
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed to initialize condition variable attribute %d!",
                    __func__, ret);
            OICFree(eventInfo);
            return retVal;
        }

#ifdef HAVE_PTHREAD_CONDATTR_SETCLOCK
        {
            ret = pthread_condattr_setclock(&(eventInfo->condattr), CLOCK_MONOTONIC);
            if(0 != ret)
            {
                OIC_LOG_V(ERROR, TAG, "%s: Failed to set condition variable clock %d!",
                        __func__, ret);
                pthread_condattr_destroy(&(eventInfo->condattr));
                OICFree(eventInfo);
                return retVal;
            }
        }
#else
	/*GAR: android < 5.0?  os x? */
	/*GAR: android < 5.0:  pthread_cond_timedwait_relative */
	/*GAR: osx: pthread_cond_timedwait_relative_np ?? */
#endif
        ret = pthread_cond_init(&(eventInfo->cond), &(eventInfo->condattr));
        if (0 == ret)
        {
            retVal = (ca_cond) eventInfo;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed to initialize condition variable %d!", __func__, ret);
            pthread_condattr_destroy(&(eventInfo->condattr));
            OICFree(eventInfo);
        }
#endif /* _WIN32 */
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Failed to allocate condition variable!", __func__);
    }

    return retVal;
}

void ca_cond_free(ca_cond cond)
{
    ca_cond_internal *eventInfo = (ca_cond_internal*) cond;
    if (eventInfo != NULL)
    {
#if defined(_WIN32)
        OICFree(cond);
#else
        int ret = pthread_cond_destroy(&(eventInfo->cond));
        int ret2 = pthread_condattr_destroy(&(eventInfo->condattr));
        if (0 == ret && 0 == ret2)
        {
            OICFree(cond);
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed to destroy condition variable %d, %d",
                    __func__, ret, ret2);
        }
#endif
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid parameter", __func__);
    }
}

void ca_cond_signal(ca_cond cond)
{
    ca_cond_internal *eventInfo = (ca_cond_internal*) cond;
    if (eventInfo != NULL)
    {
#if defined(_WIN32)
        WakeConditionVariable(&eventInfo->cond);
#else
        int ret = pthread_cond_signal(&(eventInfo->cond));
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed to signal condition variable", __func__);
        }
#endif
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid parameter", __func__);
    }
}

void ca_cond_broadcast(ca_cond cond)
{
    ca_cond_internal* eventInfo = (ca_cond_internal*) cond;
    if (eventInfo != NULL)
    {
#if defined(_WIN32)
        WakeAllConditionVariable(&eventInfo->cond);
#else
        int ret = pthread_cond_broadcast(&(eventInfo->cond));
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG, "%s: failed to signal condition variable", __func__);
        }
#endif
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid parameter", __func__);
    }
}

void ca_cond_wait(ca_cond cond, ca_mutex mutex)
{
    ca_cond_wait_for(cond, mutex, 0L);
}

#ifndef TIMEVAL_TO_TIMESPEC
#define TIMEVAL_TO_TIMESPEC(tv, ts) {               \
    (ts)->tv_sec = (tv)->tv_sec;                    \
    (ts)->tv_nsec = (tv)->tv_usec * 1000;           \
}
#endif

CAWaitResult_t ca_cond_wait_for(ca_cond cond, ca_mutex mutex, uint64_t microseconds)
{
  /* OIC_LOG_V(ERROR, TAG, "GAR: %ld: %s %d microseconds", pthread_self(), __func__, microseconds); */

    CAWaitResult_t retVal = CA_WAIT_INVAL;

    ca_cond_internal *eventInfo = (ca_cond_internal*) cond;
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;

    if (NULL == mutexInfo)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid mutex", __func__);
        return CA_WAIT_INVAL;
    }

    if (NULL == eventInfo)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid condition", __func__);
        return CA_WAIT_INVAL;
    }

    if (microseconds > 0)
    {
#if defined(_WIN32)		/*GAR FIXME: put this in adapter layer c_common/oic_time */
        // Wait for the given time
        DWORD milli = (DWORD)(microseconds / 1000);
        if (!SleepConditionVariableCS(&eventInfo->cond, &mutexInfo->mutex, milli))
        {
            if (GetLastError() == ERROR_TIMEOUT)
            {
                retVal = CA_WAIT_TIMEDOUT;
            }
            else
            {
                OIC_LOG_V(ERROR, TAG, "SleepConditionVariableCS() with Timeout failed %i", GetLastError());
                retVal = CA_WAIT_INVAL;
            }
        }else
        {
            retVal = CA_WAIT_SUCCESS;
        }
#else
	uint64_t abstime            = OICGetCurrentTime(TIME_IN_US) + microseconds;
        struct timespec abstimespec = { .tv_sec  = abstime/USECS_PER_SEC,
					.tv_nsec = (abstime % USECS_PER_SEC) * NANOSECS_PER_USECS };

        //Wait for the given time
        int ret = 0;
        ret = pthread_cond_timedwait(&(eventInfo->cond), &(mutexInfo->mutex), &abstimespec);

        switch (ret)
        {
            case 0:
                // Success
	/* OIC_LOG_V(ERROR, TAG, "GAR: CA_WAIT_SUCCESS\n", ret); */
                retVal = CA_WAIT_SUCCESS;
                break;
            case ETIMEDOUT:
                retVal = CA_WAIT_TIMEDOUT;
	/* OIC_LOG_V(ERROR, TAG, "GAR: CA_WAIT_TIMEDOUT\n", ret); */
                break;
            case EINVAL:
                OIC_LOG_V(ERROR, TAG, "%s: condition, mutex, or abstime is Invalid", __func__);
                retVal = CA_WAIT_INVAL;
                break;
            default:
                OIC_LOG_V(ERROR, TAG, "%s: pthread_cond_timedwait returned %d", __func__, retVal);
                retVal = CA_WAIT_INVAL;
                break;
        }
#endif
    }
    else
    {
#if defined(_WIN32)
        // Wait forever
        if (!SleepConditionVariableCS(&eventInfo->cond, &mutexInfo->mutex, INFINITE))
        {
            OIC_LOG_V(ERROR, TAG, "SleepConditionVariableCS() w/o Timeout failed %i", GetLastError());
            retVal = CA_WAIT_INVAL;
        }else
        {
            retVal = CA_WAIT_SUCCESS;
        }
#else
        // Wait forever
        int ret = pthread_cond_wait(&eventInfo->cond, &mutexInfo->mutex);
        retVal = ret == 0 ? CA_WAIT_SUCCESS : CA_WAIT_INVAL;
#endif
    }
    return retVal;
}
