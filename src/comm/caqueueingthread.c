/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "caqueueingthread.h"

/* #include "iotivity_config.h" */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

/* #include "caqueueingthread.h" */
/* #include "oic_malloc.h" */
/* #include "logger.h" */

#define TAG "OIC_CA_QING"

#if EXPORT_INTERFACE
#include <stdint.h>
#endif

/* #include "cathreadpool.h" */
/* #include "octhread.h" */
/* #include "uqueue.h" */
/* #include "cacommon.h" */

#if EXPORT_INTERFACE
/** Thread function to be invoked. **/
typedef void (*CAThreadTask)(void *threadData);

/** Data destroy function. **/
typedef void (*CADataDestroyFunction)(void *data, uint32_t size);

typedef struct
{
    /** Thread pool of the thread started. **/
    ca_thread_pool_t threadPool;
    /** mutex for synchronization. **/
    oc_mutex threadMutex;
    /** conditional mutex for synchronization. **/
    oc_cond threadCond;
    /** Thread function to be invoked. **/
    CAThreadTask threadTask;
    /** Data destroy function. **/
    CADataDestroyFunction destroy;
    /** Variable to inform the thread to stop. **/
    bool isStop;
    /** Que on which the thread is operating. **/
    u_queue_t *dataQueue;
#ifdef DEBUG_THREADS
    char *name;
#endif
} CAQueueingThread_t;
#endif

static void CAQueueingThreadBaseRoutine(void *threadValue)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    CAQueueingThread_t *thread = (CAQueueingThread_t *) threadValue;

    OIC_LOG_THREADS_V(DEBUG, TAG, "%s thread: %s", __func__, thread->name);

    if (NULL == thread)
    {
        OIC_LOG(ERROR, TAG, "thread data passing error!!");
        return;
    }

    while (!thread->isStop)
    {
        // mutex lock
        oc_mutex_lock(thread->threadMutex);

        // if queue is empty, thread will wait
        if (!thread->isStop && u_queue_get_size(thread->dataQueue) <= 0)
        {
            OIC_LOG_THREADS_V(DEBUG, TAG, "%s %s waiting on queue..", __func__, thread->name);

            // wait
            oc_cond_wait(thread->threadCond, thread->threadMutex);

            OIC_LOG_THREADS_V(DEBUG, TAG, "%s %s waking up...", __func__, thread->name);
        }

        // check stop flag
        if (thread->isStop)
        {
            // mutex unlock
            oc_mutex_unlock(thread->threadMutex);
            continue;
        }

        // get data
        u_queue_message_t *message = u_queue_get_element(thread->dataQueue);
        // mutex unlock
        oc_mutex_unlock(thread->threadMutex);
        if (NULL == message)
        {
            continue;
        }

        // process data
        thread->threadTask(message->msg);

        // free
        if (NULL != thread->destroy)
        {
            thread->destroy(message->msg, message->size);
        }
        else
        {
            OICFree(message->msg);
        }

        OICFree(message);
    }

    oc_mutex_lock(thread->threadMutex);
    oc_cond_signal(thread->threadCond);
    oc_mutex_unlock(thread->threadMutex);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}


CAResult_t CAQueueingThreadInitialize(CAQueueingThread_t *thread,
				      ca_thread_pool_t handle,
                                      CAThreadTask task,
				      CADataDestroyFunction destroy)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_LOG_THREADS_V(DEBUG, TAG, "%s thread: %s", __func__, thread->name);
    if (NULL == thread)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    if (NULL == handle)
    {
        OIC_LOG(ERROR, TAG, "thread pool handle is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    // OIC_LOG(DEBUG, TAG, "initializing thread struct...");

    // set send thread data
    thread->threadPool = handle;
    thread->dataQueue = u_queue_create();
    thread->threadMutex = oc_mutex_new();
    thread->threadCond = oc_cond_new();
    thread->isStop = true;
    thread->threadTask = task;
    thread->destroy = destroy;
    if (NULL == thread->dataQueue || NULL == thread->threadMutex || NULL == thread->threadCond)
    {
        goto ERROR_MEM_FAILURE;
    }

    return CA_STATUS_OK;

ERROR_MEM_FAILURE:
    if (thread->dataQueue)
    {
        u_queue_delete(thread->dataQueue);
        thread->dataQueue = NULL;
    }
    if (thread->threadMutex)
    {
        oc_mutex_free(thread->threadMutex);
        thread->threadMutex = NULL;
    }
    if (thread->threadCond)
    {
        oc_cond_free(thread->threadCond);
        thread->threadCond = NULL;
    }
    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CAQueueingThreadStart(CAQueueingThread_t *thread)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_LOG_THREADS_V(DEBUG, TAG, "%s thread: %s", __func__, thread->name);
    if (NULL == thread)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    if (NULL == thread->threadPool)
    {
        OIC_LOG(ERROR, TAG, "thread pool handle is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    if (false == thread->isStop) //Queueing thread already running
    {
        OIC_LOG(DEBUG, TAG, "queueing thread already running..");
        return CA_STATUS_OK;
    }

    oc_mutex_lock(thread->threadMutex);
    thread->isStop = false;
    oc_mutex_unlock(thread->threadMutex);

    OIC_LOG_THREADS_V(DEBUG, TAG, "Adding thread %s to queueing pool", thread->name);
    CAResult_t res = ca_thread_pool_add_task(thread->threadPool, CAQueueingThreadBaseRoutine, thread);

    if (res != CA_STATUS_OK)
    {
        // update thread status.
        oc_mutex_lock(thread->threadMutex);
        thread->isStop = true;
        oc_mutex_unlock(thread->threadMutex);

        OIC_LOG(ERROR, TAG, "thread pool add task error(send thread).");
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return res;
}

CAResult_t CAQueueingThreadAddData(CAQueueingThread_t *thread, void *data, uint32_t size)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_LOG_THREADS_V(DEBUG, TAG, "%s thread: %s", __func__, thread->name);
    if (NULL == thread)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    if (NULL == data || 0 == size)
    {
        OIC_LOG(ERROR, TAG, "data is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    // create thread data
    u_queue_message_t *message = (u_queue_message_t *) OICMalloc(sizeof(u_queue_message_t));

    if (NULL == message)
    {
        OIC_LOG(ERROR, TAG, "memory error!!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    message->msg = data;
    message->size = size;

    // mutex lock
    oc_mutex_lock(thread->threadMutex);

    // add thread data into list
    u_queue_add_element(thread->dataQueue, message);

    // notity the thread
    oc_cond_signal(thread->threadCond);

    // mutex unlock
    oc_mutex_unlock(thread->threadMutex);

    return CA_STATUS_OK;
}

CAResult_t CAQueueingThreadDestroy(CAQueueingThread_t *thread)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_LOG_THREADS_V(DEBUG, TAG, "\tthread: %s", thread->name);
    if (NULL == thread)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    OIC_LOG(DEBUG, TAG, "thread destroy..");

    // mutex lock
    oc_mutex_lock(thread->threadMutex);

    // remove all remained list data.
    while (u_queue_get_size(thread->dataQueue) > 0)
    {
        // get data
        u_queue_message_t *message = u_queue_get_element(thread->dataQueue);

        // free
        if (NULL != message)
        {
            if (NULL != thread->destroy)
            {
                thread->destroy(message->msg, message->size);
            }
            else
            {
                OICFree(message->msg);
            }

            OICFree(message);
        }
    }

    u_queue_delete(thread->dataQueue);
    thread->dataQueue = NULL;

    // mutex unlock
    oc_mutex_unlock(thread->threadMutex);

    oc_mutex_free(thread->threadMutex);
    thread->threadMutex = NULL;
    oc_cond_free(thread->threadCond);

    return CA_STATUS_OK;
}

CAResult_t CAQueueingThreadStop(CAQueueingThread_t *thread)
{
#ifdef DEBUG_THREADS
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY: %s", __func__, thread->name);
#else
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
#endif
    if (NULL == thread)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    if (!thread->isStop)
    {
        // mutex lock
        oc_mutex_lock(thread->threadMutex);

        // set stop flag
        thread->isStop = true;

        // notify the thread
        oc_cond_signal(thread->threadCond);

        oc_cond_wait(thread->threadCond, thread->threadMutex);

        // mutex unlock
        oc_mutex_unlock(thread->threadMutex);
    }

    return CA_STATUS_OK;
}
