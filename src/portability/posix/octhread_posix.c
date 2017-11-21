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

/*
 * Expose POSIX.1-2008 base specification,
 * Refer http://pubs.opengroup.org/onlinepubs/9699919799/
 * For this specific file, see use of clock_gettime and PTHREAD_MUTEX_DEFAULT
 */
#define _POSIX_C_SOURCE 200809L

/* #include "iotivity_config.h" */
/* #include "iotivity_debug.h" */
#include "octhread_posix.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

/* #include "../oic_malloc.h" */
/* #include "logger.h" */

typedef struct oc_event_t *oc_event;

/**
 * Value used for the owner field of an oc_mutex that doesn't have an owner.
 */
#define OC_INVALID_THREAD_ID    0

/**
 * TAG
 * Logging tag for module name
 */
#define TAG PCF("OIC_UMUTEX")

static const uint64_t USECS_PER_SEC         = 1000000;
static const uint64_t NANOSECS_PER_USECS    = 1000;
static const uint64_t NANOSECS_PER_SEC      = 1000000000L;

#if EXPORT_INTERFACE
/**
 * Value used for the owner field of an oc_mutex that doesn't have an owner.
 */
#define OC_INVALID_THREAD_ID    0

typedef enum
{
    OC_THREAD_SUCCESS = 0,
    OC_THREAD_ALLOCATION_FAILURE = 1,
    OC_THREAD_CREATE_FAILURE=2,
    OC_THREAD_INVALID=3,
    OC_THREAD_WAIT_FAILURE=4,
    OC_THREAD_INVALID_PARAMETER=5
} OCThreadResult_t;
#endif

typedef struct _tagMutexInfo_t
{
    pthread_mutex_t mutex;

    /**
     * Catch some of the incorrect mutex usage, by tracking the mutex owner,
     * on Debug builds.
     */
#ifndef NDEBUG
    pthread_t owner;
    uint32_t recursionCount;
#endif
} oc_mutex_internal;

typedef struct _tagEventInfo_t
{
    pthread_cond_t cond;
    pthread_condattr_t condattr;
} oc_cond_internal;

typedef struct _tagThreadInfo_t
{
    pthread_t thread;
    pthread_attr_t  threadattr;
} oc_thread_internal;

#if EXPORT_INTERFACE
#include <stdint.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
typedef struct oc_mutex_internal *oc_mutex;
typedef struct oc_cond_internal *oc_cond;
typedef struct oc_thread_internal *oc_thread;
#endif
#ifndef NDEBUG
static pthread_t oc_get_current_thread_id()
{
    pthread_t id = pthread_self();
    assert(OC_INVALID_THREAD_ID != id);
    return id;
}
#endif

OCThreadResult_t oc_thread_new(oc_thread *t, void *(*start_routine)(void *), void *arg)
{
    OCThreadResult_t res = OC_THREAD_SUCCESS;
    oc_thread_internal *threadInfo = (oc_thread_internal*)OICMalloc(sizeof(oc_thread_internal));
    if (NULL != threadInfo)
    {
        int result = pthread_create(&threadInfo->thread, NULL, start_routine, arg);
        if (result != 0)
        {
            res = OC_THREAD_CREATE_FAILURE;
            *t = NULL;
            OICFree(threadInfo);
            OIC_LOG_V(ERROR, TAG, "%s: pthread_create failed", __func__);
        }
        else
        {
            *t = (oc_thread)threadInfo;
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Failed to allocate thread!", __func__);
        *t = NULL;
        res = OC_THREAD_ALLOCATION_FAILURE;
    }
    return res;
}

OCThreadResult_t oc_thread_free(oc_thread t)
{
    OCThreadResult_t res = OC_THREAD_SUCCESS;
    oc_thread_internal *threadInfo = (oc_thread_internal*) t;
    if (threadInfo)
    {
        OICFree(threadInfo);
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Invalid thread !", __func__);
        res = OC_THREAD_INVALID;
    }
    return res;
}

OCThreadResult_t oc_thread_wait(oc_thread t)
{
    OCThreadResult_t res = OC_THREAD_SUCCESS;
    oc_thread_internal *threadInfo = (oc_thread_internal*) t;
    int joinres = pthread_join(threadInfo->thread, NULL);
    if (0 != joinres)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to join thread with error %d", joinres);
        res = OC_THREAD_WAIT_FAILURE;
    }

    return res;
}

oc_mutex oc_mutex_new(void)
{
    oc_mutex retVal = NULL;
    oc_mutex_internal *mutexInfo = (oc_mutex_internal*) OICMalloc(sizeof(oc_mutex_internal));
    if (NULL != mutexInfo)
    {
        // create the mutex with the attributes set
        int ret=pthread_mutex_init(&(mutexInfo->mutex), PTHREAD_MUTEX_DEFAULT);
        if (0 == ret)
        {
#ifndef NDEBUG
            mutexInfo->owner = OC_INVALID_THREAD_ID;
            mutexInfo->recursionCount = 0;
#endif
            retVal = (oc_mutex) mutexInfo;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s Failed to initialize mutex !", __func__);
            OICFree(mutexInfo);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Failed to allocate mutex!", __func__);
    }

    return retVal;
}

oc_mutex oc_mutex_new_recursive(void)
{
    oc_mutex retVal = NULL;
    int ret = -1;

    // Allocate new mutex.
    oc_mutex_internal *mutexInfo = (oc_mutex_internal*) OICMalloc(sizeof(oc_mutex_internal));
    if (NULL == mutexInfo)
    {
        OIC_LOG_V(ERROR, TAG, "%s Failed to allocate mutex!", __func__);
        goto exit;
    }

    // Set up the mutex attributes.
    pthread_mutexattr_t ma;
    ret = pthread_mutexattr_init(&ma);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "%s Failed in pthread_mutexattr_init - error %d!",
            __func__, ret);
        goto exit;
    }

    ret = pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "%s Failed in pthread_mutexattr_settype - error %d!",
            __func__, ret);
        pthread_mutexattr_destroy(&ma);
        goto exit;
    }

    // Initialize the mutex and destroy the attributes.
    ret = pthread_mutex_init(&(mutexInfo->mutex), &ma);
    OC_VERIFY(0 == pthread_mutexattr_destroy(&ma));
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "%s Failed in pthread_mutex_init - error %d!",
            __func__, ret);
        goto exit;
    }

#ifndef NDEBUG
    mutexInfo->owner = OC_INVALID_THREAD_ID;
    mutexInfo->recursionCount = 0;
#endif

exit:
    if (0 == ret)
    {
        retVal = (oc_mutex) mutexInfo;
    }
    else
    {
        OICFree(mutexInfo);
    }

    return retVal;
}

bool oc_mutex_free(oc_mutex mutex)
{
    bool bRet=false;
    oc_mutex_internal *mutexInfo = (oc_mutex_internal*) mutex;
    if (mutexInfo)
    {
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
    }

    return bRet;
}

void oc_mutex_lock(oc_mutex mutex)
{
    oc_mutex_internal *mutexInfo = (oc_mutex_internal*) mutex;
    if (mutexInfo)
    {
        int ret = pthread_mutex_lock(&mutexInfo->mutex);
        if(ret != 0)
        {
            OIC_LOG_V(ERROR, TAG, "Pthread Mutex lock failed: %d", ret);
            exit(ret);
        }

#ifndef NDEBUG
        /**
         * Updating the recursionCount and owner fields must be performed
         * while owning the lock, to solve race conditions with other
         * threads using the same lock.
         */
        if (mutexInfo->recursionCount != 0)
        {
            oc_mutex_assert_owner(mutex, true);
        }
        else
        {
            mutexInfo->owner = oc_get_current_thread_id();
        }

        mutexInfo->recursionCount++;
#endif
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Invalid mutex !", __func__);
    }
}

void oc_mutex_unlock(oc_mutex mutex)
{
    oc_mutex_internal *mutexInfo = (oc_mutex_internal*) mutex;
    if (mutexInfo)
    {
#ifndef NDEBUG
        oc_mutex_assert_owner(mutex, true);

        /**
         * Updating the recursionCount and owner fields must be performed
         * while owning the lock, to solve race conditions with other
         * threads using the same lock.
         */
        mutexInfo->recursionCount--;

        if (mutexInfo->recursionCount == 0)
        {
            mutexInfo->owner = OC_INVALID_THREAD_ID;
        }
#endif

        int ret = pthread_mutex_unlock(&mutexInfo->mutex);
        if(ret != 0)
        {
            OIC_LOG_V(ERROR, TAG, "Pthread Mutex unlock failed: %d", ret);
            exit(ret);
        }
        (void)ret;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid mutex !", __func__);
    }
}

void oc_mutex_assert_owner(const oc_mutex mutex, bool currentThreadIsOwner)
{
#ifdef NDEBUG
    (void)mutex;
    (void)currentThreadIsOwner;
#else
    assert(NULL != mutex);
    const oc_mutex_internal *mutexInfo = (const oc_mutex_internal*) mutex;

    pthread_t currentThreadID = oc_get_current_thread_id();
    if (currentThreadIsOwner)
    {
        assert(pthread_equal(mutexInfo->owner, currentThreadID));
        assert(mutexInfo->recursionCount != 0);
    }
    else
    {
        assert(!pthread_equal(mutexInfo->owner, currentThreadID));
    }
#endif
}

oc_cond oc_cond_new(void)
{
    oc_cond retVal = NULL;
    oc_cond_internal *eventInfo = (oc_cond_internal*) OICMalloc(sizeof(oc_cond_internal));
    if (NULL != eventInfo)
    {
        int ret = pthread_condattr_init(&(eventInfo->condattr));
        if(0 != ret)
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed to initialize condition variable attribute %d!",
                    __func__, ret);
            OICFree(eventInfo);
            return retVal;
        }

#if _POSIX_TIMERS > 0
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
#endif /* _POSIX_TIMERS > 0 */
        ret = pthread_cond_init(&(eventInfo->cond), &(eventInfo->condattr));
        if (0 == ret)
        {
            retVal = (oc_cond) eventInfo;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed to initialize condition variable %d!", __func__, ret);
            pthread_condattr_destroy(&(eventInfo->condattr));
            OICFree(eventInfo);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Failed to allocate condition variable!", __func__);
    }

    return retVal;
}

void oc_cond_free(oc_cond cond)
{
    oc_cond_internal *eventInfo = (oc_cond_internal*) cond;
    if (eventInfo != NULL)
    {
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
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid parameter", __func__);
    }
}

void oc_cond_signal(oc_cond cond)
{
    oc_cond_internal *eventInfo = (oc_cond_internal*) cond;
    if (eventInfo != NULL)
    {
        int ret = pthread_cond_signal(&(eventInfo->cond));
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed to signal condition variable", __func__);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid parameter", __func__);
    }
}

void oc_cond_broadcast(oc_cond cond)
{
    oc_cond_internal* eventInfo = (oc_cond_internal*) cond;
    if (eventInfo != NULL)
    {
        int ret = pthread_cond_broadcast(&(eventInfo->cond));
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG, "%s: failed to signal condition variable", __func__);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid parameter", __func__);
    }
}

void oc_cond_wait(oc_cond cond, oc_mutex mutex)
{
    oc_cond_wait_for(cond, mutex, 0L);
}

#ifndef TIMEVAL_TO_TIMESPEC
#define TIMEVAL_TO_TIMESPEC(tv, ts) {               \
    (ts)->tv_sec = (tv)->tv_sec;                    \
    (ts)->tv_nsec = (tv)->tv_usec * 1000;           \
}
#endif

struct timespec oc_get_current_time()
{
#if _POSIX_TIMERS > 0
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct timespec ts;
    TIMEVAL_TO_TIMESPEC(&tv, &ts);
    return ts;
#endif
}

#include <time.h>
void oc_add_microseconds_to_timespec(struct timespec* ts, uint64_t microseconds)
{
    time_t secPart = microseconds/USECS_PER_SEC;
    uint64_t nsecPart = (microseconds % USECS_PER_SEC) * NANOSECS_PER_USECS;
    uint64_t totalNs = ts->tv_nsec + nsecPart;
    time_t secOfNs = totalNs/NANOSECS_PER_SEC;

    ts->tv_nsec = (totalNs)% NANOSECS_PER_SEC;
    ts->tv_sec += secPart + secOfNs;
}

OCWaitResult_t oc_cond_wait_for(oc_cond cond, oc_mutex mutex, uint64_t microseconds)
{
    OCWaitResult_t retVal = OC_WAIT_INVAL;

    oc_cond_internal *eventInfo = (oc_cond_internal*) cond;
    oc_mutex_internal *mutexInfo = (oc_mutex_internal*) mutex;

    if (NULL == mutexInfo)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid mutex", __func__);
        return OC_WAIT_INVAL;
    }

    if (NULL == eventInfo)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid condition", __func__);
        return OC_WAIT_INVAL;
    }

    if (microseconds > 0)
    {
        int ret = 0;
        struct timespec abstime = { .tv_sec = 0 };

        {
             abstime = oc_get_current_time();
            oc_add_microseconds_to_timespec(&abstime, microseconds);

#ifndef NDEBUG
            // Recursively-acquired locks are not supported for use with condition variables.
            oc_mutex_assert_owner(mutex, true);
            assert(mutexInfo->recursionCount == 1);
            mutexInfo->recursionCount = 0;
            mutexInfo->owner = OC_INVALID_THREAD_ID;
#endif

            // Wait for the given time
            ret = pthread_cond_timedwait(&(eventInfo->cond), &(mutexInfo->mutex), &abstime);

#ifndef NDEBUG
            oc_mutex_assert_owner(mutex, false);
            assert(mutexInfo->recursionCount == 0);
            mutexInfo->recursionCount = 1;
            mutexInfo->owner = oc_get_current_thread_id();
#endif
        }

        switch (ret)
        {
            case 0:
                // Success
                retVal = OC_WAIT_SUCCESS;
                break;
            case ETIMEDOUT:
                retVal = OC_WAIT_TIMEDOUT;
                break;
            case EINVAL:
                OIC_LOG_V(ERROR, TAG, "%s: condition, mutex, or abstime is Invalid", __func__);
                retVal = OC_WAIT_INVAL;
                break;
            default:
                OIC_LOG_V(ERROR, TAG, "%s: pthread_cond_timedwait returned %d", __func__, retVal);
                retVal = OC_WAIT_INVAL;
                break;
        }
    }
    else
    {
#ifndef NDEBUG
        // Recursively-acquired locks are not supported for use with condition variables.
        oc_mutex_assert_owner(mutex, true);
        assert(mutexInfo->recursionCount == 1);
        mutexInfo->recursionCount = 0;
        mutexInfo->owner = OC_INVALID_THREAD_ID;
#endif

        // Wait forever
#ifndef NDEBUG
        // The conditional variable wait API used will atomically release the mutex, but the
        // best we can do here is to just clear the owner info before the API is called.
        mutexInfo->owner = OC_INVALID_THREAD_ID;
#endif
        int ret = pthread_cond_wait(&eventInfo->cond, &mutexInfo->mutex);
        retVal = (ret == 0) ? OC_WAIT_SUCCESS : OC_WAIT_INVAL;

#ifndef NDEBUG
        oc_mutex_assert_owner(mutex, false);
        assert(mutexInfo->recursionCount == 0);
        mutexInfo->recursionCount = 1;
        mutexInfo->owner = oc_get_current_thread_id();
#endif
    }
    return retVal;
}
