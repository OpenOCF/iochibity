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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

/* #include <pthread.h>  //GAR */

#include "caqueueingthread.h"
#include "oic_malloc.h"
#include "logger.h"

#define TAG PCF("OIC_CA_QING")

static void CAQueueingThreadBaseRoutine(void *threadValue)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY, thread %d", __func__, pthread_self());
    /* OIC_LOG_V(DEBUG, TAG, "message handler main thread %d start..", pthread_self()); */

    CAQueueingThread_t *thread = (CAQueueingThread_t *) threadValue;

    if (NULL == thread)
    {
        OIC_LOG(ERROR, TAG, "thread data passing error!!");
        return;
    }

    while (!thread->isStop)
    {
        // mutex lock
        ca_mutex_lock(thread->threadMutex);

        if (!thread->isStop && u_queue_get_size(thread->dataQueue) <= 0)
        {
	    // if queue is empty, thread will wait
            OIC_LOG_V(DEBUG, TAG, "%s: ca_cond_wait blocking, on thread %d", __func__, pthread_self());
            ca_cond_wait(thread->threadCond, thread->threadMutex);
            OIC_LOG_V(DEBUG, TAG, "%s: ca_cond_wait unblocked, on thread %d..", __func__, pthread_self());
        }

        // check stop flag
        if (thread->isStop)
        {
            // mutex unlock
            ca_mutex_unlock(thread->threadMutex);
            continue;
        }

        // get data
        u_queue_message_t *message = u_queue_get_element(thread->dataQueue);
        // mutex unlock
        ca_mutex_unlock(thread->threadMutex);
        if (NULL == message)
        {
            continue;
        }

        // process data
	OIC_LOG_V(DEBUG, TAG, "%s: process thread task", __func__);
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

    ca_mutex_lock(thread->threadMutex);
    ca_cond_signal(thread->threadCond);
    ca_mutex_unlock(thread->threadMutex);

    OIC_LOG(DEBUG, TAG, "message handler main thread ending");
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT", __func__);
}

CAResult_t CAQueueingThreadInitialize(CAQueueingThread_t *thread, ca_thread_pool_t handle,
                                      CAThreadTask task, CADataDestroyFunction destroy)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
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

    // set send thread data
    thread->threadPool = handle;
    thread->dataQueue = u_queue_create();
    thread->threadMutex = ca_mutex_new();
    thread->threadCond = ca_cond_new();
    thread->isStop = true;
    thread->threadTask = task;
    thread->destroy = destroy;
    if (NULL == thread->dataQueue || NULL == thread->threadMutex || NULL == thread->threadCond)
    {
        goto ERROR_MEM_FAILURE;
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT OK", __func__);
    return CA_STATUS_OK;

ERROR_MEM_FAILURE:
    if (thread->dataQueue)
    {
        u_queue_delete(thread->dataQueue);
        thread->dataQueue = NULL;
    }
    if (thread->threadMutex)
    {
        ca_mutex_free(thread->threadMutex);
        thread->threadMutex = NULL;
    }
    if (thread->threadCond)
    {
        ca_cond_free(thread->threadCond);
        thread->threadCond = NULL;
    }
    OIC_LOG_V(DEBUG, TAG, "%s: EXIT ALLOC FAILED", __func__);
    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CAQueueingThreadStart(CAQueueingThread_t *thread)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
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

    // mutex lock
    ca_mutex_lock(thread->threadMutex);
    thread->isStop = false;
    // mutex unlock
    ca_mutex_unlock(thread->threadMutex);

    CAResult_t res = ca_thread_pool_add_task(thread->threadPool, CAQueueingThreadBaseRoutine,
                                            thread);
    if (res != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "thread pool add task error(send thread).");
    }

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT returning %x", __func__, res);
    return res;
}

CAResult_t CAQueueingThreadAddData(CAQueueingThread_t *thread, void *data, uint32_t size)
{
    OIC_LOG_V(DEBUG, TAG, "%s: ENTRY", __func__);
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
    ca_mutex_lock(thread->threadMutex);

    // add thread data into list
    u_queue_add_element(thread->dataQueue, message);

    // notity the thread
    OIC_LOG_V(DEBUG, TAG, "%s: Signal waiting thread", __func__);
    ca_cond_signal(thread->threadCond);

    // mutex unlock
    ca_mutex_unlock(thread->threadMutex);

    OIC_LOG_V(DEBUG, TAG, "%s: EXIT returning OK", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAQueueingThreadDestroy(CAQueueingThread_t *thread)
{
    if (NULL == thread)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    OIC_LOG(DEBUG, TAG, "thread destroy..");

    ca_mutex_free(thread->threadMutex);
    thread->threadMutex = NULL;
    ca_cond_free(thread->threadCond);

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

    return CA_STATUS_OK;
}

CAResult_t CAQueueingThreadStop(CAQueueingThread_t *thread)
{
    if (NULL == thread)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    OIC_LOG_V(DEBUG, TAG, "thread stop request!! pthread_self: %d", pthread_self());

    if (!thread->isStop)
    {
        // mutex lock
        ca_mutex_lock(thread->threadMutex);

        // set stop flag
        thread->isStop = true;

        // notify the thread
        ca_cond_signal(thread->threadCond);

        ca_cond_wait(thread->threadCond, thread->threadMutex);

        // mutex unlock
        ca_mutex_unlock(thread->threadMutex);
    }

    return CA_STATUS_OK;
}
