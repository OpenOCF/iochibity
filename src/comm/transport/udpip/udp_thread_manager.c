/** @file udp_thread_manager.c
 *
 * Provides thread management services to udp service components.
 *
 */

#include "udp_thread_manager.h"

#define TAG "UDPTHREADMGR"

#if INTERFACE
#ifdef DEBUG_THREADING
#define THREAD_DBUG(...) __VA_ARGS__ ,
#else
#define THREAD_DBUG(...)
#endif
#endif

#ifndef SINGLE_THREAD

/**
 * Queue handle for Send Data.
 */
CAQueueingThread_t udp_sendQueueHandle;

/* ONLY called by udp_initialize_send_thread */
LOCAL CAResult_t udp_initialize_send_msg_queue() // @was CAIPInitializeQueueHandles
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    // Check if the message queue is already initialized
    /* if (udp_sendQueueHandle) */
    /* { */
    /*     OIC_LOG(DEBUG, TAG, "send queue handle is already initialized!"); */
    /*     return CA_STATUS_OK; */
    /* } */

    g_local_endpoint_cache = u_arraylist_create();
    if (!g_local_endpoint_cache)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed! (g_local_endpoint_cache)");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Create send message queue
    /* udp_sendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t)); */
    /* if (!udp_sendQueueHandle) */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "Memory allocation failed! (udp_sendQueueHandle)"); */
    /*     u_arraylist_free(&g_local_endpoint_cache); */
    /*     g_local_endpoint_cache = NULL; */
    /*     return CA_MEMORY_ALLOC_FAILED; */
    /* } */

#ifdef DEBUG_THREADS
    udp_sendQueueHandle.name = "udp_sendQueueHandle";
#endif
    if (CA_STATUS_OK != CAQueueingThreadInitialize(&udp_sendQueueHandle,
						   (const ca_thread_pool_t)udp_threadpool,
						   CAIPSendDataThread,
						   CADataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
	/* OICFree(udp_sendQueueHandle); */
        /* udp_sendQueueHandle = NULL; */
        u_arraylist_free(&g_local_endpoint_cache);
        g_local_endpoint_cache = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

/* ONLY called by CATerminateIP */
void udp_stop_send_msg_queue() // @was CAIPDeinitializeQueueHandles()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    CAQueueingThreadDestroy(&udp_sendQueueHandle);
    /* OICFree(udp_sendQueueHandle); */
    /* udp_sendQueueHandle = NULL; */

    // Since the items in g_local_endpoint_cache are allocated once in a big chunk, we only need to
    // free the first item. Another location this is done is in the CA_INTERFACE_DOWN handler
    // in CAUpdateStoredIPAddressInfo().
    OICFree(u_arraylist_get(g_local_endpoint_cache, 0));
    u_arraylist_free(&g_local_endpoint_cache);
    g_local_endpoint_cache = NULL;
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

CAResult_t udp_start_send_msg_queue()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    if (CA_STATUS_OK != udp_initialize_send_msg_queue())  // @was CAIPInitializeQueueHandles
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize Queue Handle");
        return CA_STATUS_FAILED;
    }

    // Start send queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(&udp_sendQueueHandle))
    {
        OIC_LOG(ERROR, TAG, "Failed to Start Send Data Thread");
        return CA_STATUS_FAILED;
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

#endif
