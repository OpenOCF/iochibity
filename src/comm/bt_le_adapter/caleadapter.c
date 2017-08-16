/* ****************************************************************
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
#include "caleadapter.h"

#include <stdio.h>
#include <stdlib.h>

#include "caleinterface.h"
#include "cacommon.h"
#include "camutex.h"
#include "caadapterutils.h"
#ifndef SINGLE_THREAD
#include "caqueueingthread.h"
#endif
#if defined(__TIZEN__) || defined(__ANDROID__)
#include "caleserver.h"
#include "caleclient.h"
#endif
#include "oic_malloc.h"
#include "oic_string.h"
#include "caremotehandler.h"
#include "pdu.h"

/**
 * Logging tag for module name.
 */
#define TAG "OIC_CA_LE_ADAP"

/**
 * The MTU supported for BLE adapter
 */
#define CA_SUPPORTED_BLE_MTU_SIZE  20

/**
 * Stores information of all the senders.
 *
 * This structure will be used to track and defragment all incoming
 * data packet.
 */
typedef struct
{
    uint32_t recvDataLen;
    uint32_t totalDataLen;
    uint8_t *defragData;
    CAEndpoint_t *remoteEndpoint;
 } CABLESenderInfo_t;

typedef enum
{
    ADAPTER_EMPTY = 1,
    ADAPTER_BOTH_CLIENT_SERVER,
    ADAPTER_CLIENT,
    ADAPTER_SERVER
} CABLEAdapter_t;

/**
 * Callback to provide the status of the network change to CA layer.
 */
static CAAdapterChangeCallback g_networkCallback = NULL;

/**
 * Callback to provide the status of the connection change to CA layer.
 */
static CAConnectionChangeCallback g_connectionCallback = NULL;

/**
 * bleAddress of the local adapter. Value will be initialized to zero,
 * and will be updated later.
 */
static char g_localBLEAddress[18] = { 0 };

/**
 * Variable to differentiate btw GattServer and GattClient.
 */
static CABLEAdapter_t g_adapterType = ADAPTER_EMPTY;

/**
 * Mutex to synchronize the task to be executed on the GattServer
 * function calls.
 */
static ca_mutex g_bleIsServerMutex = NULL;

/**
 * Mutex to synchronize the callback to be called for the network
 * changes.
 */
static ca_mutex g_bleNetworkCbMutex = NULL;

/**
 * Mutex to synchronize the updates of the local LE address of the
 * adapter.
 */
static ca_mutex g_bleLocalAddressMutex = NULL;

/**
 * Reference to thread pool.
 */
static ca_thread_pool_t g_bleAdapterThreadPool = NULL;

/**
 * Mutex to synchronize the task to be pushed to thread pool.
 */
static ca_mutex g_bleAdapterThreadPoolMutex = NULL;

/**
 * Mutex to synchronize the queing of the data from SenderQueue.
 */
static ca_mutex g_bleClientSendDataMutex = NULL;

/**
 * Mutex to synchronize the queing of the data from ReceiverQueue.
 */
static ca_mutex g_bleReceiveDataMutex = NULL;

/**
 * Mutex to synchronize the queing of the data from SenderQueue.
 */
static ca_mutex g_bleServerSendDataMutex = NULL;

/**
 * Mutex to synchronize the callback to be called for the
 * adapterReqResponse.
 */
static ca_mutex g_bleAdapterReqRespCbMutex = NULL;

/**
 * Callback to be called when network packet received from either
 * GattServer or GattClient.
 */
static CANetworkPacketReceivedCallback g_networkPacketReceivedCallback = NULL;

/**
 * Callback to notify error from the BLE adapter.
 */
static CAErrorHandleCallback g_errorHandler = NULL;

/**
 * Register network change notification callback.
 *
 * @param[in]  netCallback  CAAdapterChangeCallback callback which will
 *                          be set for the change in adapter.
 * @param[in]  connCallback CAConnectionChangeCallback callback which will
 *                          be set for the change in connection.
 *
 * @return  0 on success otherwise a positive error value.
 * @retval  ::CA_STATUS_OK  Successful.
 * @retval  ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval  ::CA_STATUS_FAILED Operation failed.
 *
 */
static CAResult_t CALERegisterNetworkNotifications(CAAdapterChangeCallback netCallback,
                                                   CAConnectionChangeCallback connCallback);

/**
 * Set the thread pool handle which is required for spawning new
 * thread.
 *
 * @param[in] handle Thread pool handle which is given by above layer
 *                   for using thread creation task.
 *
 */
static void CASetLEAdapterThreadPoolHandle(ca_thread_pool_t handle);

/**
 * Call the callback to the upper layer when the adapter state gets
 * changed.
 *
 * @param[in] adapter_state New state of the adapter to be notified to
 *                          the upper layer.
 */
static void CALEDeviceStateChangedCb(CAAdapterState_t adapter_state);

/**
 * Call the callback to the upper layer when the device connection state gets
 * changed.
 *
 * @param[in] address      LE address of the device to be notified to the upper layer.
 * @param[in] isConnected  whether connection state is connected or not.
 */
static void CALEConnectionStateChangedCb(CATransportAdapter_t adapter, const char* address,
                                         bool isConnected);

/**
 * Used to initialize all required mutex variable for LE Adapter
 * implementation.
 *
 * @return  0 on success otherwise a positive error value.
 * @retval  ::CA_STATUS_OK  Successful.
 * @retval  ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval  ::CA_STATUS_FAILED Operation failed.
 *
 */
static CAResult_t CAInitLEAdapterMutex();

/**
 * Terminate all required mutex variables for LE adapter
 * implementation.
 */
static void CATerminateLEAdapterMutex();

/**
 * Prepares and notify error through error callback.
 */
static void CALEErrorHandler(const char *remoteAddress,
                             const uint8_t *data,
                             uint32_t dataLen,
                             CAResult_t result);

#ifndef SINGLE_THREAD
/**
 * Stop condition of recvhandler.
 */
static bool g_dataBleReceiverHandlerState = false;

/**
 * Sender information.
 */
static u_arraylist_t *g_bleServerSenderInfo = NULL;

static u_arraylist_t *g_bleClientSenderInfo = NULL;

/**
 * Queue to process the outgoing packets from GATTClient.
 */
static CAQueueingThread_t *g_bleClientSendQueueHandle = NULL;

/**
 * Queue to process the incoming packets.
 */
static CAQueueingThread_t *g_bleReceiverQueue = NULL;

/**
 * Queue to process the outgoing packets from GATTServer.
 */
static CAQueueingThread_t *g_bleServerSendQueueHandle = NULL;

/**
 * This function will be associated with the sender queue for
 * GattServer.
 *
 * This function will fragment the data to the MTU of the transport
 * and send the data in fragments to the adapters. The function will
 * be blocked until all data is sent out from the adapter.
 *
 * @param[in] threadData Data pushed to the queue which contains the
 *                       info about RemoteEndpoint and Data.
 */
static void CALEServerSendDataThread(void *threadData);

/**
 * This function will be associated with the sender queue for
 * GattClient.
 *
 * This function will fragment the data to the MTU of the transport
 * and send the data in fragments to the adapters. The function will
 * be blocked until all data is sent out from the adapter.
 *
 * @param[in] threadData Data pushed to the queue which contains the
 *                       info about RemoteEndpoint and Data.
 */
static void CALEClientSendDataThread(void *threadData);

/**
 * This function will be associated with the receiver queue.
 *
 * This function will defragment the received data from each sender
 * respectively and will send it up to CA layer.  Respective sender's
 * header will provide the length of the data sent.
 *
 * @param[in] threadData Data pushed to the queue which contains the
 *                       info about RemoteEndpoint and Data.
 */
static void CALEDataReceiverHandler(void *threadData);

/**
 * This function will stop all queues created for GattServer and
 * GattClient. All four queues will be be stopped with this function
 * invocations.
 */
static void CAStopLEQueues();

/**
 * This function will terminate all queues created for GattServer and
 * GattClient. All four queues will be be terminated with this
 * function invocations.
 */
static void CATerminateLEQueues();

/**
 * This function will initalize the Receiver and Sender queues for
 * GattServer. This function will in turn call the functions
 * CAInitBleServerReceiverQueue() and CAInitBleServerSenderQueue() to
 * initialize the queues.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CAInitLEServerQueues();

/**
 * This function will initalize the Receiver and Sender queues for
 * GattClient. This function will inturn call the functions
 * CAInitBleClientReceiverQueue() and CAInitBleClientSenderQueue() to
 * initialize the queues.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 *
 */
static CAResult_t CAInitLEClientQueues();

/**
 * This function will initalize the Receiver queue for
 * GattServer. This will initialize the queue to process the function
 * CABLEServerSendDataThread() when ever the task is added to this
 * queue.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CAInitLEServerSenderQueue();

/**
 * This function will initalize the Receiver queue for
 * GattClient. This will initialize the queue to process the function
 * CABLEClientSendDataThread() when ever the task is added to this
 * queue.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CAInitLEClientSenderQueue();

/**
 * This function will initialize the Receiver queue for
 * LEAdapter. This will initialize the queue to process the function
 * CABLEDataReceiverHandler() when ever the task is added to this
 * queue.
 *
 * @return ::CA_STATUS_OK or Appropriate error code
 * @retval ::CA_STATUS_OK  Successful
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments
 * @retval ::CA_STATUS_FAILED Operation failed
 *
 */
static CAResult_t CAInitLEReceiverQueue();

/**
 * This function will create the Data required to send it in the
 * queue.
 *
 * @param[in] remoteEndpoint Remote endpoint information of the
 *                           server.
 * @param[in] data           Data to be transmitted from LE.
 * @param[in] dataLength     Length of the Data being transmitted.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CALEData_t *CACreateLEData(const CAEndpoint_t *remoteEndpoint,
                                  const uint8_t *data,
                                  uint32_t dataLength,
                                  u_arraylist_t *senderInfo);

/**
 * Used to free the BLE information stored in the sender/receiver
 * queues.
 *
 * @param[in] bleData Information for a particular data segment.
 */
static void CAFreeLEData(CALEData_t *bleData);

/**
 * Free data.
 */
static void CALEDataDestroyer(void *data, uint32_t size);

#ifndef SINGLE_THREAD
/**
 * remove request or response data of send queue.
 *
 * @param[in] queueHandle    queue to process the outgoing packets.
 * @param[in] mutex          mutex related to sender for client / server.
 * @param[in] address        target address to remove data in queue.
 */
static void CALERemoveSendQueueData(CAQueueingThread_t *queueHandle,
                                    ca_mutex mutex,
                                    const char* address);

/**
 * remove all received data of data list from receive queue.
 *
 * @param[in] dataInfoList   received data list to remove for client / server.
 * @param[in] address        target address to remove data in queue.
 */
static void CALERemoveReceiveQueueData(u_arraylist_t *dataInfoList,
                                       const char* address);
#endif

static CAResult_t CAInitLEServerQueues()
{
    OIC_LOG(DEBUG, TAG, "IN");

    ca_mutex_lock(g_bleAdapterThreadPoolMutex);

    CAResult_t result = CAInitLEServerSenderQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAInitBleServerSenderQueue failed");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    g_bleServerSenderInfo = u_arraylist_create();
    if (!g_bleServerSenderInfo)
    {
        OIC_LOG(ERROR, TAG, "memory allocation failed!");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_MEMORY_ALLOC_FAILED;
    }

    result = CAInitLEReceiverQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAInitLEReceiverQueue failed");
        u_arraylist_free(&g_bleServerSenderInfo);
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    g_dataBleReceiverHandlerState = true;

    ca_mutex_unlock(g_bleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CAInitLEClientQueues()
{
    OIC_LOG(DEBUG, TAG, "IN");

    ca_mutex_lock(g_bleAdapterThreadPoolMutex);

    CAResult_t result = CAInitLEClientSenderQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAInitBleClientSenderQueue failed");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    g_bleClientSenderInfo = u_arraylist_create();
    if (!g_bleClientSenderInfo)
    {
        OIC_LOG(ERROR, TAG, "memory allocation failed!");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_MEMORY_ALLOC_FAILED;
    }

    result = CAInitLEReceiverQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAInitLEReceiverQueue failed");
        u_arraylist_free(&g_bleClientSenderInfo);
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    g_dataBleReceiverHandlerState = true;

    ca_mutex_unlock(g_bleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CAInitLEReceiverQueue()
{
    OIC_LOG(DEBUG, TAG, "IN - CAInitLEReceiverQueue");
    // Check if the message queue is already initialized
    if (g_bleReceiverQueue)
    {
        OIC_LOG(DEBUG, TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    // Create recv message queue
    g_bleReceiverQueue = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_bleReceiverQueue)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_bleReceiverQueue,
                                                   g_bleAdapterThreadPool,
                                                   CALEDataReceiverHandler,
                                                   CALEDataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        OICFree(g_bleReceiverQueue);
        g_bleReceiverQueue = NULL;
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadStart(g_bleReceiverQueue))
    {
        OIC_LOG(ERROR, TAG, "ca_thread_pool_add_task failed ");
        OICFree(g_bleReceiverQueue);
        g_bleReceiverQueue = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CAInitLEServerSenderQueue()
{
    OIC_LOG(DEBUG, TAG, "IN - CAInitLEServerSenderQueue");
    // Check if the message queue is already initialized
    if (g_bleServerSendQueueHandle)
    {
        OIC_LOG(DEBUG, TAG, "Queue is already initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_bleServerSendQueueHandle = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_bleServerSendQueueHandle)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_bleServerSendQueueHandle,
                                                   g_bleAdapterThreadPool,
                                                   CALEServerSendDataThread,
                                                   CALEDataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        OICFree(g_bleServerSendQueueHandle);
        g_bleServerSendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static void CALEClearSenderInfoImpl(u_arraylist_t ** list)
{
    const size_t length = u_arraylist_length(*list);
    for (size_t i = 0; i < length; ++i)
    {
        CABLESenderInfo_t * const info =
                (CABLESenderInfo_t *) u_arraylist_get(*list, i);
        if (info)
         {
             OICFree(info->defragData);
             CAFreeEndpoint(info->remoteEndpoint);
             OICFree(info);
         }
    }
    u_arraylist_free(list);
}

static void CALEClearSenderInfo()
{
    CALEClearSenderInfoImpl(&g_bleServerSenderInfo);
    CALEClearSenderInfoImpl(&g_bleClientSenderInfo);
}

static CAResult_t CAInitLEClientSenderQueue()
{
    OIC_LOG(DEBUG, TAG, "IN - CAInitLEClientSenderQueue");

    if (g_bleClientSendQueueHandle)
    {
        OIC_LOG(DEBUG, TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_bleClientSendQueueHandle = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_bleClientSendQueueHandle)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_bleClientSendQueueHandle,
                                                   g_bleAdapterThreadPool,
                                                   CALEClientSendDataThread, CALEDataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        OICFree(g_bleClientSendQueueHandle);
        g_bleClientSendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT - CAInitLEClientSenderQueue");
    return CA_STATUS_OK;
}

static void CAStopLEQueues()
{
    OIC_LOG(DEBUG, TAG, "IN - CAStopLEQueues");

    ca_mutex_lock(g_bleReceiveDataMutex);
    if (NULL != g_bleReceiverQueue)
    {
        CAQueueingThreadStop(g_bleReceiverQueue);
    }
    ca_mutex_unlock(g_bleReceiveDataMutex);

    OIC_LOG(DEBUG, TAG, "OUT - CAStopLEQueues");
}

static void CATerminateLEQueues()
{
    OIC_LOG(DEBUG, TAG, "IN");

    CAQueueingThreadDestroy(g_bleClientSendQueueHandle);
    OICFree(g_bleClientSendQueueHandle);
    g_bleClientSendQueueHandle = NULL;

    CAQueueingThreadDestroy(g_bleServerSendQueueHandle);
    OICFree(g_bleServerSendQueueHandle);
    g_bleServerSendQueueHandle = NULL;

    CAQueueingThreadDestroy(g_bleReceiverQueue);
    OICFree(g_bleReceiverQueue);
    g_bleReceiverQueue = NULL;

    CALEClearSenderInfo();

    OIC_LOG(DEBUG, TAG, "OUT");
}

static CAResult_t CALEGetSenderInfo(const char *leAddress,
                                    u_arraylist_t *senderInfoList,
                                    CABLESenderInfo_t **senderInfo,
                                    uint32_t *senderIndex)
{
    VERIFY_NON_NULL_RET(leAddress,
                        TAG,
                        "NULL BLE address argument",
                        CA_STATUS_INVALID_PARAM);
    VERIFY_NON_NULL_RET(senderIndex,
                        TAG,
                        "NULL index argument",
                        CA_STATUS_INVALID_PARAM);

    const uint32_t listLength = u_arraylist_length(senderInfoList);
    const uint32_t addrLength = strlen(leAddress);
    for (uint32_t index = 0; index < listLength; index++)
    {
        CABLESenderInfo_t *info = (CABLESenderInfo_t *) u_arraylist_get(senderInfoList, index);
        if(!info || !(info->remoteEndpoint))
        {
            continue;
        }

        if(!strncmp(info->remoteEndpoint->addr, leAddress, addrLength))
        {
            *senderIndex = index;
            if(senderInfo)
            {
                *senderInfo = info;
            }
            return CA_STATUS_OK;
        }
    }

    return CA_STATUS_FAILED;
}

static void CALEDataReceiverHandler(void *threadData)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEDataReceiverHandler");

    ca_mutex_lock(g_bleReceiveDataMutex);

    if (g_dataBleReceiverHandlerState)
    {
        OIC_LOG(DEBUG, TAG, "checking for DE Fragmentation");

        CALEData_t *bleData = (CALEData_t *) threadData;
        if (!bleData)
        {
            OIC_LOG(DEBUG, TAG, "Invalid bleData!");
            ca_mutex_unlock(g_bleReceiveDataMutex);
            return;
        }

        if (!(bleData->senderInfo))
        {
            OIC_LOG(ERROR, TAG, "sender info is not available");
            ca_mutex_unlock(g_bleReceiveDataMutex);
            return;
        }

        if (!(bleData->remoteEndpoint))
        {
            OIC_LOG(ERROR, TAG, "Client RemoteEndPoint NULL!!");
            ca_mutex_unlock(g_bleReceiveDataMutex);
            return;
        }

        CABLESenderInfo_t *senderInfo = NULL;
        uint32_t senderIndex = 0;

        if(CA_STATUS_OK != CALEGetSenderInfo(bleData->remoteEndpoint->addr,
                                             bleData->senderInfo,
                                             &senderInfo, &senderIndex))
        {
            OIC_LOG_V(DEBUG, TAG, "This is a new client [%s]",
                      bleData->remoteEndpoint->addr);
        }

        if(!senderInfo)
        {
            CABLESenderInfo_t *newSender = OICMalloc(sizeof(CABLESenderInfo_t));
            if(!newSender)
            {
                OIC_LOG(ERROR, TAG, "Memory allocation failed for new sender");
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }
            newSender->recvDataLen = 0;
            newSender->totalDataLen = 0;
            newSender->defragData = NULL;
            newSender->remoteEndpoint = NULL;

            OIC_LOG(DEBUG, TAG, "Parsing the header");

            newSender->totalDataLen = coap_get_total_message_length(bleData->data, bleData->dataLen);

            if(!(newSender->totalDataLen))
            {
                OIC_LOG(ERROR, TAG, "Total Data Length is parsed as 0!!!");
                OICFree(newSender);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }

            OIC_LOG_V(DEBUG, TAG, "Total data to be accumulated [%u] bytes",
                      newSender->totalDataLen);
            OIC_LOG_V(DEBUG, TAG, "data received in the first packet [%u] bytes",
                      bleData->dataLen);

            newSender->defragData = OICCalloc(newSender->totalDataLen + 1,
                                              sizeof(*newSender->defragData));

            if (NULL == newSender->defragData)
            {
                OIC_LOG(ERROR, TAG, "defragData is NULL!");
                OICFree(newSender);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }

            const char *remoteAddress = bleData->remoteEndpoint->addr;
            newSender->remoteEndpoint = CACreateEndpointObject(CA_DEFAULT_FLAGS,
                                                               CA_ADAPTER_GATT_BTLE,
                                                               remoteAddress,
                                                               0);
            if (NULL == newSender->remoteEndpoint)
            {
                OIC_LOG(ERROR, TAG, "remoteEndpoint is NULL!");
                OICFree(newSender->defragData);
                OICFree(newSender);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }

            if (newSender->recvDataLen + bleData->dataLen > newSender->totalDataLen)
            {
                OIC_LOG(ERROR, TAG, "buffer is smaller than received data");
                OICFree(newSender->defragData);
                CAFreeEndpoint(newSender->remoteEndpoint);
                OICFree(newSender);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }
            memcpy(newSender->defragData, bleData->data, bleData->dataLen);
            newSender->recvDataLen += bleData->dataLen;

            u_arraylist_add(bleData->senderInfo,(void *)newSender);

            //Getting newSender index position in bleSenderInfo array list
            if(CA_STATUS_OK !=
                CALEGetSenderInfo(newSender->remoteEndpoint->addr, bleData->senderInfo,
                                  NULL, &senderIndex))
            {
                OIC_LOG(ERROR, TAG, "Existing sender index not found!!");
                OICFree(newSender->defragData);
                CAFreeEndpoint(newSender->remoteEndpoint);
                OICFree(newSender);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }
            senderInfo = newSender;
        }
        else
        {
            if(senderInfo->recvDataLen + bleData->dataLen > senderInfo->totalDataLen)
            {
                OIC_LOG_V(ERROR, TAG,
                          "Data Length exceeding error!! Receiving [%d] total length [%d]",
                          senderInfo->recvDataLen + bleData->dataLen, senderInfo->totalDataLen);
                u_arraylist_remove(bleData->senderInfo, senderIndex);
                OICFree(senderInfo->defragData);
                OICFree(senderInfo);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "Copying the data of length [%d]",
                      bleData->dataLen);
            memcpy(senderInfo->defragData + senderInfo->recvDataLen, bleData->data,
                   bleData->dataLen);
            senderInfo->recvDataLen += bleData->dataLen ;
            OIC_LOG_V(DEBUG, TAG, "totalDatalength  [%d] received Datalen [%d]",
                                                senderInfo->totalDataLen, senderInfo->recvDataLen);
        }

        if (senderInfo->totalDataLen == senderInfo->recvDataLen)
        {
            ca_mutex_lock(g_bleAdapterReqRespCbMutex);
            if (NULL == g_networkPacketReceivedCallback)
            {
                OIC_LOG(ERROR, TAG, "gReqRespCallback is NULL!");

                u_arraylist_remove(bleData->senderInfo, senderIndex);
                OICFree(senderInfo->defragData);
                OICFree(senderInfo);
                ca_mutex_unlock(g_bleAdapterReqRespCbMutex);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }

            OIC_LOG(DEBUG, TAG, "[CALEDataReceiverHandler] Sending data up !");

            const CASecureEndpoint_t tmp =
                {
                    .endpoint = *senderInfo->remoteEndpoint
                };

            g_networkPacketReceivedCallback(&tmp,
                                            senderInfo->defragData,
                                            senderInfo->recvDataLen);
            ca_mutex_unlock(g_bleAdapterReqRespCbMutex);
            u_arraylist_remove(bleData->senderInfo, senderIndex);
            senderInfo->remoteEndpoint = NULL;
            senderInfo->defragData = NULL;
            OICFree(senderInfo);
        }
    }
    ca_mutex_unlock(g_bleReceiveDataMutex);
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CALEServerSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEServerSendDataThread");

    CALEData_t * const bleData = (CALEData_t *) threadData;
    if (!bleData)
    {
        OIC_LOG(DEBUG, TAG, "Invalid bledata!");
        return;
    }

    const uint32_t totalLength = bleData->dataLen;

    OIC_LOG_V(DEBUG,
              TAG,
              "Server total Data length with header is [%u]",
              totalLength);

    uint8_t * const dataSegment = OICCalloc(totalLength, 1);

    if (NULL == dataSegment)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed");
        return;
    }

    uint32_t length = 0;
    if (CA_SUPPORTED_BLE_MTU_SIZE > totalLength)
    {
        length = totalLength;
        memcpy(dataSegment, bleData->data, bleData->dataLen);
    }
    else
    {
        length =  CA_SUPPORTED_BLE_MTU_SIZE;
        memcpy(dataSegment, bleData->data, CA_SUPPORTED_BLE_MTU_SIZE);
    }

    uint32_t iter = totalLength / CA_SUPPORTED_BLE_MTU_SIZE;
    uint32_t index = 0;
    CAResult_t result = CA_STATUS_FAILED;

    // Send the first segment with the header.
    if (NULL != bleData->remoteEndpoint) // Sending Unicast Data
    {
        OIC_LOG(DEBUG, TAG, "Server Sending Unicast Data");

        result = CAUpdateCharacteristicsToGattClient(
                    bleData->remoteEndpoint->addr, dataSegment, length);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR,
                      TAG,
                      "Update characteristics failed, result [%d]",
                      result);

            g_errorHandler(bleData->remoteEndpoint,
                           bleData->data,
                           bleData->dataLen,
                           result);
            OICFree(dataSegment);
            return;
        }

        OIC_LOG_V(DEBUG,
                  TAG,
                  "Server Sent data length [%u]",
                  length);
        for (index = 1; index < iter; index++)
        {
            // Send the remaining header.
            OIC_LOG_V(DEBUG,
                      TAG,
                      "Sending the chunk number [%u]",
                      index);

            result =
                CAUpdateCharacteristicsToGattClient(
                    bleData->remoteEndpoint->addr,
                    bleData->data + ((index * CA_SUPPORTED_BLE_MTU_SIZE)),
                    CA_SUPPORTED_BLE_MTU_SIZE);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, TAG,
                            "Update characteristics failed, result [%d]", result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "Server Sent data length [%d]",
                                               CA_SUPPORTED_BLE_MTU_SIZE);
        }

        const uint32_t remainingLen =
            totalLength % CA_SUPPORTED_BLE_MTU_SIZE;

        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data (Ex: 22 bytes of 622
            // bytes of data when MTU is 200)
            OIC_LOG(DEBUG, TAG, "Sending the last chunk");

            result = CAUpdateCharacteristicsToGattClient(
                         bleData->remoteEndpoint->addr,
                         bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE),
                         remainingLen);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR,
                          TAG,
                          "Update characteristics failed, result [%d]",
                          result);
                g_errorHandler(bleData->remoteEndpoint,
                               bleData->data,
                               bleData->dataLen,
                               result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "Server Sent data length [%d]", remainingLen);
        }
     }
    else
    {
        OIC_LOG(DEBUG, TAG, "Server Sending Multicast data");
        result = CAUpdateCharacteristicsToAllGattClients(dataSegment, length);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, TAG, "Update characteristics failed, result [%d]",
                      result);
            CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
            OICFree(dataSegment);
            return;
        }
        OIC_LOG_V(DEBUG, TAG, "Server Sent data length [%d]", length);
        for (index = 1; index < iter; index++)
        {
            // Send the remaining header.
            OIC_LOG_V(DEBUG, TAG, "Sending the chunk number [%d]", index);

            result = CAUpdateCharacteristicsToAllGattClients(
                         bleData->data + ((index * CA_SUPPORTED_BLE_MTU_SIZE)),
                         CA_SUPPORTED_BLE_MTU_SIZE);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, TAG, "Update characteristics failed, result [%d]",
                          result);
                CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "Server Sent data length [%u]",
                      CA_SUPPORTED_BLE_MTU_SIZE);
        }

        const uint32_t remainingLen = totalLength % CA_SUPPORTED_BLE_MTU_SIZE;
        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data
            OIC_LOG(DEBUG, TAG, "Sending the last chunk");

            result = CAUpdateCharacteristicsToAllGattClients(
                         bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE),
                         remainingLen);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, TAG, "Update characteristics failed, result [%d]",
                          result);
                CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "Server Sent data length [%d]", remainingLen);
        }
    }
    OICFree(dataSegment);

    OIC_LOG(DEBUG, TAG, "OUT - CALEServerSendDataThread");
}

static void CALEClientSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEClientSendDataThread");

    CALEData_t *bleData = (CALEData_t *) threadData;
    if (!bleData)
    {
        OIC_LOG(DEBUG, TAG, "Invalid bledata!");
        return;
    }

    const uint32_t totalLength = bleData->dataLen;

    uint8_t *dataSegment = OICCalloc(totalLength, 1);
    if (NULL == dataSegment)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed");
        return;
    }

    uint32_t length = 0;
    if (CA_SUPPORTED_BLE_MTU_SIZE > totalLength)
    {
        length = totalLength;
        memcpy(dataSegment,
               bleData->data,
               bleData->dataLen);
    }
    else
    {
        length = CA_SUPPORTED_BLE_MTU_SIZE;
        memcpy(dataSegment,
               bleData->data,
               CA_SUPPORTED_BLE_MTU_SIZE);
    }

    CAResult_t result = CA_STATUS_FAILED;
    const uint32_t iter = totalLength / CA_SUPPORTED_BLE_MTU_SIZE;
    uint32_t index = 0;
    if (NULL != bleData->remoteEndpoint) //Sending Unicast Data
    {
        OIC_LOG(DEBUG, TAG, "Sending Unicast Data");
        // Send the first segment with the header.
        result =
            CAUpdateCharacteristicsToGattServer(
                bleData->remoteEndpoint->addr,
                dataSegment,
                length,
                LE_UNICAST,
                0);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR,
                      TAG,
                      "Update characteristics failed, result [%d]",
                      result);
            g_errorHandler(bleData->remoteEndpoint,
                           bleData->data,
                           bleData->dataLen,
                           result);
            OICFree(dataSegment);
            return;
        }

        OIC_LOG_V(DEBUG,
                  TAG,
                  "Client Sent Data length  is [%u]",
                  length);

        for (index = 1; index < iter; index++)
        {
            // Send the remaining header.
            result = CAUpdateCharacteristicsToGattServer(
                     bleData->remoteEndpoint->addr,
                     bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE),
                     CA_SUPPORTED_BLE_MTU_SIZE,
                     LE_UNICAST, 0);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR,
                          TAG,
                          "Update characteristics failed, result [%d]",
                          result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "Client Sent Data length  is [%d]",
                                               CA_SUPPORTED_BLE_MTU_SIZE);
        }

        const uint32_t remainingLen = totalLength % CA_SUPPORTED_BLE_MTU_SIZE;
        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data (Ex: 22 bytes of 622
            // bytes of data when MTU is 200)
            OIC_LOG(DEBUG, TAG, "Sending the last chunk");

            result = CAUpdateCharacteristicsToGattServer(
                     bleData->remoteEndpoint->addr,
                     bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE),
                     remainingLen,
                     LE_UNICAST, 0);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, TAG, "Update characteristics failed, result [%d]",
                                                   result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "Client Sent Data length  is [%d]", remainingLen);
        }
    }
    else
    {
        //Sending Mulitcast Data
        // Send the first segment with the header.
        OIC_LOG(DEBUG, TAG, "Sending Multicast Data");
        result = CAUpdateCharacteristicsToAllGattServers(dataSegment, length);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, TAG,
                      "Update characteristics (all) failed, result [%d]", result);
            CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
            OICFree(dataSegment);
            return ;
        }
        OIC_LOG_V(DEBUG, TAG, "Client Sent Data length  is [%d]", length);
        // Send the remaining header.
        for (index = 1; index < iter; index++)
        {
            result = CAUpdateCharacteristicsToAllGattServers(
                         bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE),
                         CA_SUPPORTED_BLE_MTU_SIZE);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, TAG, "Update characteristics failed, result [%d]",
                          result);
                CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "Client Sent Data length  is [%d]",
                      CA_SUPPORTED_BLE_MTU_SIZE);
        }

        uint32_t remainingLen = totalLength % CA_SUPPORTED_BLE_MTU_SIZE;
        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data (Ex: 22 bytes of 622
            // bytes of data when MTU is 200)
            OIC_LOG(DEBUG, TAG, "Sending the last chunk");
            result =
                CAUpdateCharacteristicsToAllGattServers(
                    bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE),
                    remainingLen);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, TAG,
                          "Update characteristics (all) failed, result [%d]", result);
                CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "Client Sent Data length  is [%d]", remainingLen);
        }

    }

    OICFree(dataSegment);

    OIC_LOG(DEBUG, TAG, "OUT - CABLEClientSendDataThread");
}

static CALEData_t *CACreateLEData(const CAEndpoint_t *remoteEndpoint,
                                  const uint8_t *data,
                                  uint32_t dataLength,
                                  u_arraylist_t *senderInfo)
{
    CALEData_t * const bleData = OICMalloc(sizeof(CALEData_t));

    if (!bleData)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return NULL;
    }

    bleData->remoteEndpoint = CACloneEndpoint(remoteEndpoint);
    bleData->data = OICCalloc(dataLength + 1, 1);

    if (NULL == bleData->data)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        CAFreeLEData(bleData);
        return NULL;
    }

    memcpy(bleData->data, data, dataLength);
    bleData->dataLen = dataLength;
    if (senderInfo)
    {
        bleData->senderInfo = senderInfo;
    }

    return bleData;
}

static void CAFreeLEData(CALEData_t *bleData)
{
    VERIFY_NON_NULL_VOID(bleData, TAG, "Param bleData is NULL");

    CAFreeEndpoint(bleData->remoteEndpoint);
    OICFree(bleData->data);
    OICFree(bleData);
}

static void CALEDataDestroyer(void *data, uint32_t size)
{
    if ((size_t)size < sizeof(CALEData_t *))
    {
        OIC_LOG_V(ERROR, TAG,
                  "Destroy data too small %p %d", data, size);
    }
    CALEData_t *ledata = (CALEData_t *) data;

    CAFreeLEData(ledata);
}
#endif

static CAResult_t CAInitLEAdapterMutex()
{
    OIC_LOG(DEBUG, TAG, "IN - CAInitLEAdapterMutex");

    if (NULL == g_bleIsServerMutex)
    {
        g_bleIsServerMutex = ca_mutex_new();
        if (NULL == g_bleIsServerMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleNetworkCbMutex)
    {
        g_bleNetworkCbMutex = ca_mutex_new();
        if (NULL == g_bleNetworkCbMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleLocalAddressMutex)
    {
        g_bleLocalAddressMutex = ca_mutex_new();
        if (NULL == g_bleLocalAddressMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleAdapterThreadPoolMutex)
    {
        g_bleAdapterThreadPoolMutex = ca_mutex_new();
        if (NULL == g_bleAdapterThreadPoolMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleClientSendDataMutex)
    {
        g_bleClientSendDataMutex = ca_mutex_new();
        if (NULL == g_bleClientSendDataMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleServerSendDataMutex)
    {
        g_bleServerSendDataMutex = ca_mutex_new();
        if (NULL == g_bleServerSendDataMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleAdapterReqRespCbMutex)
    {
        g_bleAdapterReqRespCbMutex = ca_mutex_new();
        if (NULL == g_bleAdapterReqRespCbMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleReceiveDataMutex)
    {
        g_bleReceiveDataMutex = ca_mutex_new();
        if (NULL == g_bleReceiveDataMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static void CATerminateLEAdapterMutex()
{
    OIC_LOG(DEBUG, TAG, "IN - CATerminateLEAdapterMutex");

    ca_mutex_free(g_bleIsServerMutex);
    g_bleIsServerMutex = NULL;

    ca_mutex_free(g_bleNetworkCbMutex);
    g_bleNetworkCbMutex = NULL;

    ca_mutex_free(g_bleLocalAddressMutex);
    g_bleLocalAddressMutex = NULL;

    ca_mutex_free(g_bleAdapterThreadPoolMutex);
    g_bleAdapterThreadPoolMutex = NULL;

    ca_mutex_free(g_bleClientSendDataMutex);
    g_bleClientSendDataMutex = NULL;

    ca_mutex_free(g_bleServerSendDataMutex);
    g_bleServerSendDataMutex = NULL;

    ca_mutex_free(g_bleAdapterReqRespCbMutex);
    g_bleAdapterReqRespCbMutex = NULL;

    ca_mutex_free(g_bleReceiveDataMutex);
    g_bleReceiveDataMutex = NULL;

    OIC_LOG(DEBUG, TAG, "OUT");
}

/**
 * Starting LE connectivity adapters.
 *
 * As its peer to peer it does not require to start any servers.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAStartLE();

/**
 * Start listening server for receiving multicast search requests.
 *
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics
 *   per OIC Specification.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAStartLEListeningServer();

/**
 * Stops listening server from receiving multicast search requests.
 *
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics
 *   per OIC Specification.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAStopLEListeningServer();

/**
 * Sarting discovery of servers for receiving multicast
 * advertisements.
 *
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics
 *   per OIC Specification.
 *
 * @return ::CA_STATUS_OK or Appropriate error code
 */
static CAResult_t CAStartLEDiscoveryServer();

/**
 * Send data to the endpoint using the adapter connectivity.
 *
 * @param[in] endpoint Remote Endpoint information (like MAC address,
 *                     reference URI and connectivity type) to which
 *                     the unicast data has to be sent.
 * @param[in] data     Data which required to be sent.
 * @param[in] dataLen  Size of data to be sent.
 *
 * @note  dataLen must be > 0.
 *
 * @return The number of bytes sent on the network, or -1 on error.
 */
static int32_t CASendLEUnicastData(const CAEndpoint_t *endpoint,
                                   const void *data,
                                   uint32_t dataLen);

/**
 * Send multicast data to the endpoint using the LE connectivity.
 *
 * @param[in] endpoint Remote Endpoint information to which the
 *                     multicast data has to be sent.
 * @param[in] data     Data which required to be sent.
 * @param[in] dataLen  Size of data to be sent.
 *
 * @note  dataLen must be > 0.
 *
 * @return The number of bytes sent on the network, or -1 on error.
 */
static int32_t CASendLEMulticastData(const CAEndpoint_t *endpoint,
                                     const void *data,
                                     uint32_t dataLen);

/**
 * Get LE Connectivity network information.
 *
 * @param[out] info Local connectivity information structures.
 * @param[out] size Number of local connectivity structures.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAGetLEInterfaceInformation(CAEndpoint_t **info,
                                              uint32_t *size);

/**
 * Read Synchronous API callback.
 *
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAReadLEData();

/**
 * Stopping the adapters and close socket connections.
 *
 * LE Stops all GATT servers and GATT Clients.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAStopLE();

/**
 * Terminate the LE connectivity adapter.
 *
 * Configuration information will be deleted from further use.
 */
static void CATerminateLE();

/**
 * This function will receive the data from the GattServer and add the
 * data to the Server receiver queue.
 *
 * @param[in] remoteAddress Remote address of the device from where
 *                          data is received.
 * @param[in] data          Actual data received from the remote
 *                          device.
 * @param[in] dataLength    Length of the data received from the
 *                          remote device.
 * @param[in] sentLength    Length of the data sent from the remote
 *                          device.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 *
 */
static CAResult_t CALEAdapterServerReceivedData(const char *remoteAddress,
                                                const uint8_t *data,
                                                uint32_t dataLength,
                                                uint32_t *sentLength);

/**
 * This function will receive the data from the GattClient and add the
 * data into the Client receiver queue.
 *
 * @param[in] remoteAddress Remote address of the device from where
 *                          data is received.
 * @param[in] data          Actual data recevied from the remote
 *                          device.
 * @param[in] dataLength    Length of the data received from the
 *                          remote device.
 * @param[in] sentLength    Length of the data sent from the remote
 *                          device.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CALEAdapterClientReceivedData(const char *remoteAddress,
                                                const uint8_t *data,
                                                uint32_t dataLength,
                                                uint32_t *sentLength);

/**
 * Set the NetworkPacket received callback to CA layer from adapter
 * layer.
 *
 * @param[in] callback Callback handle sent from the upper layer.
 */
static void CASetLEReqRespAdapterCallback(CANetworkPacketReceivedCallback callback);

/**
 * Push the data from CA layer to the Sender processor queue.
 *
 * @param[in] remoteEndpoint Remote endpoint information of the
 *                           server.
 * @param[in] data           Data to be transmitted from LE.
 * @param[in] dataLen        Length of the Data being transmitted.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CALEAdapterServerSendData(const CAEndpoint_t *remoteEndpoint,
                                            const uint8_t *data,
                                            uint32_t dataLen);

/**
 * Push the data from CA layer to the Sender processor queue.
 *
 * @param[in] remoteEndpoint Remote endpoint information of the
 *                           server.
 * @param[in] data           Data to be transmitted from LE.
 * @param[in] dataLen        Length of the Data being transmitted.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CALEAdapterClientSendData(const CAEndpoint_t *remoteEndpoint,
                                            const uint8_t *data,
                                            uint32_t dataLen);

static CAResult_t CALEAdapterGattServerStart()
{
    OIC_LOG(DEBUG, TAG, "Before CAStartLEGattServer");

    CAResult_t result = CAStartLEGattServer();

#ifndef SINGLE_THREAD
    /*
      Don't start the server side sending queue thread until the
      server itself has actually started.
    */
    if (CA_STATUS_OK == result)
    {
        ca_mutex_lock(g_bleServerSendDataMutex);
        result = CAQueueingThreadStart(g_bleServerSendQueueHandle);
        ca_mutex_unlock(g_bleServerSendDataMutex);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR,
                      TAG,
                      "Unable to start server queuing thread (%d)",
                      result);
        }
    }
#endif

    return result;
}

static CAResult_t CALEAdapterGattServerStop()
{
#ifndef SINGLE_THREAD
    OIC_LOG(DEBUG, TAG, "CALEAdapterGattServerStop");

    CAResult_t result = CAStopLEGattServer();
    ca_mutex_lock(g_bleServerSendDataMutex);
    if (CA_STATUS_OK == result)
    {
        result = CAQueueingThreadStop(g_bleServerSendQueueHandle);
    }
    ca_mutex_unlock(g_bleServerSendDataMutex);

    return result;
#else
    return CAStopLEGattServer();
#endif
}

static CAResult_t CALEAdapterGattClientStart()
{
    OIC_LOG(DEBUG, TAG, "Before CAStartLEGattClient");

    CAResult_t result = CAStartLEGattClient();

#ifndef SINGLE_THREAD
    /*
      Don't start the client side sending queue thread until the
      client itself has actually started.
    */
    if (CA_STATUS_OK == result)
    {
        ca_mutex_lock(g_bleClientSendDataMutex);
        result = CAQueueingThreadStart(g_bleClientSendQueueHandle);
        ca_mutex_unlock(g_bleClientSendDataMutex);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR,
                    TAG,
                    "Unable to start client queuing thread");
        }
    }
#endif

    return result;
}

static CAResult_t CALEAdapterGattClientStop()
{
#ifndef SINGLE_THREAD
    OIC_LOG(DEBUG, TAG, "CALEAdapterGattClientStop");
    CAStopLEGattClient();

    ca_mutex_lock(g_bleClientSendDataMutex);
    CAResult_t result = CAQueueingThreadStop(g_bleClientSendQueueHandle);
    ca_mutex_unlock(g_bleClientSendDataMutex);

    return result;
#else
    CAStopLEGattClient();

    return CA_STATUS_OK;
#endif
}

CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CAAdapterChangeCallback netCallback,
                          CAConnectionChangeCallback connCallback,
                          CAErrorHandleCallback errorCallback,
                          ca_thread_pool_t handle)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    OIC_LOG(DEBUG, TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(registerCallback, TAG, "RegisterConnectivity callback is null");
    VERIFY_NON_NULL(reqRespCallback, TAG, "PacketReceived Callback is null");
    VERIFY_NON_NULL(netCallback, TAG, "NetworkChange Callback is null");
    VERIFY_NON_NULL(connCallback, TAG, "ConnectionChange Callback is null");

    CAResult_t result = CA_STATUS_OK;
    result = CAInitLEAdapterMutex();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAInitBleAdapterMutex failed!");
        return CA_STATUS_FAILED;
    }

    result = CAInitializeLENetworkMonitor();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAInitializeLENetworkMonitor() failed");
        return CA_STATUS_FAILED;
    }
    CAInitializeLEAdapter();

    CASetLEClientThreadPoolHandle(handle);

    result = CAInitializeLEGattClient();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAInitializeLEGattClient() failed");
        return CA_STATUS_FAILED;
    }

    CASetLEReqRespClientCallback(CALEAdapterClientReceivedData);
    CASetLEServerThreadPoolHandle(handle);
    result = CAInitializeLEGattServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAInitializeLEGattServer() failed");
        return CA_STATUS_FAILED;
    }

    CASetLEAdapterThreadPoolHandle(handle);
    CASetLEReqRespServerCallback(CALEAdapterServerReceivedData);
    CASetLEReqRespAdapterCallback(reqRespCallback);

    CASetBLEClientErrorHandleCallback(CALEErrorHandler);
    CASetBLEServerErrorHandleCallback(CALEErrorHandler);
    CALERegisterNetworkNotifications(netCallback, connCallback);

    g_errorHandler = errorCallback;

    static const CAConnectivityHandler_t connHandler =
        {
            .startAdapter = CAStartLE,
            .stopAdapter = CAStopLE,
            .startListenServer = CAStartLEListeningServer,
            .stopListenServer = CAStopLEListeningServer,
            .startDiscoveryServer = CAStartLEDiscoveryServer,
            .sendData = CASendLEUnicastData,
            .sendDataToAll = CASendLEMulticastData,
            .GetnetInfo = CAGetLEInterfaceInformation,
            .readData = CAReadLEData,
            .terminate = CATerminateLE,
            .cType = CA_ADAPTER_GATT_BTLE
        };

    registerCallback(connHandler);

    OIC_LOG(DEBUG, TAG, "OUT");

    return CA_STATUS_OK;
}

static CAResult_t CAStartLE()
{
    OIC_LOG(DEBUG, TAG, "CAStartLE");

    return CAStartLEAdapter();
}

static CAResult_t CAStopLE()
{
    OIC_LOG(DEBUG, TAG, "IN");
#ifndef SINGLE_THREAD
    CAStopLEQueues();
#endif

    ca_mutex_lock(g_bleIsServerMutex);
    switch (g_adapterType)
    {
        case ADAPTER_SERVER:
            CALEAdapterGattServerStop();
            break;
        case ADAPTER_CLIENT:
            CALEAdapterGattClientStop();
            break;
        case ADAPTER_BOTH_CLIENT_SERVER:
            CALEAdapterGattServerStop();
            CALEAdapterGattClientStop();
            break;
        default:
            break;
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    OIC_LOG(DEBUG, TAG, "OUT");

    return CAStopLEAdapter();
}

static void CATerminateLE()
{
    OIC_LOG(DEBUG, TAG, "IN");

    CASetLEReqRespServerCallback(NULL);
    CASetLEReqRespClientCallback(NULL);
    CALERegisterNetworkNotifications(NULL, NULL);
    CASetLEReqRespAdapterCallback(NULL);
    CATerminateLENetworkMonitor();

    ca_mutex_lock(g_bleIsServerMutex);
    switch (g_adapterType)
    {
        case ADAPTER_SERVER:
            CATerminateLEGattServer();
            break;
        case ADAPTER_CLIENT:
            CATerminateLEGattClient();
            break;
        case ADAPTER_BOTH_CLIENT_SERVER:
            CATerminateLEGattServer();
            CATerminateLEGattClient();
            break;
        default:
            break;
    }
    g_adapterType = ADAPTER_EMPTY;
    ca_mutex_unlock(g_bleIsServerMutex);

#ifndef SINGLE_THREAD
    CATerminateLEQueues();
#endif
    CATerminateLEAdapterMutex();

    OIC_LOG(DEBUG, TAG, "OUT");
}

static CAResult_t CAStartLEListeningServer()
{
    OIC_LOG(DEBUG, TAG, "IN - CAStartLEListeningServer");
#ifndef ROUTING_GATEWAY
    CAResult_t result = CA_STATUS_OK;
#ifndef SINGLE_THREAD
    result = CAInitLEServerQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAInitLEServerQueues failed");
        return result;
    }
#endif

    ca_mutex_lock(g_bleIsServerMutex);
    switch (g_adapterType)
    {
        case ADAPTER_CLIENT:
            g_adapterType = ADAPTER_BOTH_CLIENT_SERVER;
            break;
        case ADAPTER_BOTH_CLIENT_SERVER:
            break;
        default:
            g_adapterType = ADAPTER_SERVER;
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    result = CAGetLEAdapterState();
    if (CA_STATUS_OK != result)
    {
        if (CA_ADAPTER_NOT_ENABLED == result)
        {
            OIC_LOG(DEBUG,
                    TAG,
                    "Listen Server will be started once BT Adapter is enabled");
            result = CA_STATUS_OK;
        }
    }
    else
    {
        result = CALEAdapterGattServerStart();
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return result;
#else
    // Routing Gateway only supports BLE client mode.
    OIC_LOG(ERROR, TAG, "LE server not supported in Routing Gateway");
    return CA_NOT_SUPPORTED;
#endif
}

static CAResult_t CAStopLEListeningServer()
{
    OIC_LOG(ERROR, TAG, "Listen server stop not supported.");
    return CA_NOT_SUPPORTED;
}

static CAResult_t CAStartLEDiscoveryServer()
{
    OIC_LOG(DEBUG, TAG, "IN - CAStartLEDiscoveryServer");
    CAResult_t result = CA_STATUS_OK;
#ifndef SINGLE_THREAD
    result = CAInitLEClientQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAInitLEClientQueues failed");
        return result;
    }
#endif

    ca_mutex_lock(g_bleIsServerMutex);
    switch (g_adapterType)
    {
        case ADAPTER_SERVER:
            g_adapterType = ADAPTER_BOTH_CLIENT_SERVER;
            break;
        case ADAPTER_BOTH_CLIENT_SERVER:
            break;
        default:
            g_adapterType = ADAPTER_CLIENT;
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    result = CAGetLEAdapterState();
    if (CA_STATUS_OK != result)
    {
        if (CA_ADAPTER_NOT_ENABLED == result)
        {
            OIC_LOG(DEBUG,
                    TAG,
                    "Discovery Server will be started once BT Adapter is enabled");
            result = CA_STATUS_OK;
        }
    }
    else
    {
        result = CALEAdapterGattClientStart();
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return result;
}

static CAResult_t CAReadLEData()
{
    OIC_LOG(DEBUG, TAG, "IN");
#ifdef SINGLE_THREAD
    CACheckLEData();
#endif
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static int32_t CASendLEUnicastData(const CAEndpoint_t *endpoint,
                                   const void *data,
                                   uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN - CASendLEUnicastData");

    //Input validation
    VERIFY_NON_NULL_RET(endpoint, TAG, "Remote endpoint is null", -1);
    VERIFY_NON_NULL_RET(data, TAG, "Data is null", -1);

    CAResult_t result = CA_STATUS_FAILED;

    OIC_LOG_V(DEBUG, TAG, "g_adapterType: %d", g_adapterType);
    if (ADAPTER_EMPTY == g_adapterType)
    {
        OIC_LOG(ERROR, TAG, "g_adapterType is Empty");
    }

    ca_mutex_lock(g_bleIsServerMutex);
    if (ADAPTER_SERVER == g_adapterType || ADAPTER_BOTH_CLIENT_SERVER == g_adapterType)
    {
        result = CALEAdapterServerSendData(endpoint, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            ca_mutex_unlock(g_bleIsServerMutex);
            OIC_LOG(ERROR, TAG, "Send unicast data for server failed");
            if (g_errorHandler)
            {
                g_errorHandler(endpoint, data, dataLen, result);
            }

            return -1;
        }
    }

    if (ADAPTER_CLIENT == g_adapterType || ADAPTER_BOTH_CLIENT_SERVER == g_adapterType)
    {
        result = CALEAdapterClientSendData(endpoint, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            ca_mutex_unlock(g_bleIsServerMutex);
            OIC_LOG(ERROR, TAG, "Send unicast data for client failed" );

             if (g_errorHandler)
             {
                 g_errorHandler(endpoint, data, dataLen, result);
             }
            return -1;
        }
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
    return dataLen;
}

static int32_t CASendLEMulticastData(const CAEndpoint_t *endpoint,
                                     const void *data,
                                     uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN - CASendLEMulticastData");

    //Input validation
    VERIFY_NON_NULL_RET(data, TAG, "Data is null", -1);

    if (0 >= dataLen)
    {
        OIC_LOG(ERROR, TAG, "Invalid Parameter");
        return -1;
    }

    CAResult_t result = CA_STATUS_FAILED;

    OIC_LOG_V(DEBUG, TAG, "g_adapterType: %d", g_adapterType);
    if (ADAPTER_EMPTY == g_adapterType)
    {
        OIC_LOG(ERROR, TAG, "g_adapterType is Empty");
    }

    ca_mutex_lock(g_bleIsServerMutex);
    if (ADAPTER_SERVER == g_adapterType || ADAPTER_BOTH_CLIENT_SERVER == g_adapterType)
    {
        result = CALEAdapterServerSendData(NULL, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            ca_mutex_unlock(g_bleIsServerMutex);

            OIC_LOG(ERROR, TAG, "Send multicast data for server failed" );

            if (g_errorHandler)
            {
                g_errorHandler(endpoint, data, dataLen, result);
            }
            return -1;
        }
    }

    if (ADAPTER_CLIENT == g_adapterType || ADAPTER_BOTH_CLIENT_SERVER == g_adapterType)
    {
        result = CALEAdapterClientSendData(NULL, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            ca_mutex_unlock(g_bleIsServerMutex);

            OIC_LOG(ERROR, TAG, "Send Multicast data for client failed" );

            if (g_errorHandler)
            {
                g_errorHandler(endpoint, data, dataLen, result);
            }
            return -1;
        }
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    OIC_LOG(DEBUG, TAG, "OUT - CASendLEMulticastData");
    return dataLen;
}

static CAResult_t CAGetLEInterfaceInformation(CAEndpoint_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(info, TAG, "CALocalConnectivity info is null");

    char *local_address = NULL;

    CAResult_t res = CAGetLEAddress(&local_address);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "CAGetLEAddress has failed");
        return res;
    }

    if (NULL == local_address)
    {
        OIC_LOG(ERROR, TAG, "local_address is NULL");
        return CA_STATUS_FAILED;
    }

    *size = 0;
    (*info) = (CAEndpoint_t *) OICCalloc(1, sizeof(CAEndpoint_t));
    if (NULL == (*info))
    {
        OIC_LOG(ERROR, TAG, "Malloc failure!");
        OICFree(local_address);
        return CA_STATUS_FAILED;
    }

    size_t local_address_len = strlen(local_address);

    if(local_address_len >= sizeof(g_localBLEAddress) ||
            local_address_len >= MAX_ADDR_STR_SIZE_CA - 1)
    {
        OIC_LOG(ERROR, TAG, "local_address is too long");
        OICFree(*info);
        OICFree(local_address);
        return CA_STATUS_FAILED;
    }

    OICStrcpy((*info)->addr, sizeof((*info)->addr), local_address);
    ca_mutex_lock(g_bleLocalAddressMutex);
    OICStrcpy(g_localBLEAddress, sizeof(g_localBLEAddress), local_address);
    ca_mutex_unlock(g_bleLocalAddressMutex);

    (*info)->adapter = CA_ADAPTER_GATT_BTLE;
    *size = 1;
    OICFree(local_address);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CALERegisterNetworkNotifications(CAAdapterChangeCallback netCallback,
                                                   CAConnectionChangeCallback connCallback)
{
    OIC_LOG(DEBUG, TAG, "IN");

    ca_mutex_lock(g_bleNetworkCbMutex);
    g_networkCallback = netCallback;
    g_connectionCallback = connCallback;
    ca_mutex_unlock(g_bleNetworkCbMutex);
    CAResult_t res = CA_STATUS_OK;
    if (netCallback)
    {
        res = CASetLEAdapterStateChangedCb(CALEDeviceStateChangedCb);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CASetLEAdapterStateChangedCb failed!");
        }
    }
    else
    {
        res = CAUnSetLEAdapterStateChangedCb();
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CASetLEAdapterStateChangedCb failed!");
        }
    }

    if (g_connectionCallback)
    {
        res = CASetLENWConnectionStateChangedCb(CALEConnectionStateChangedCb);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CASetLENWConnectionStateChangedCb failed!");
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return res;
}

static void CALEConnectionStateChangedCb(CATransportAdapter_t adapter, const char* address,
                                         bool isConnected)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEConnectionStateChangedCb");

    VERIFY_NON_NULL_VOID(address, TAG, "address");
    (void)adapter;

#ifdef __TIZEN__
    ca_mutex_lock(g_bleIsServerMutex);
    switch (g_adapterType)
    {
        case ADAPTER_SERVER:
            CALEGattServerConnectionStateChanged(isConnected, address);
            break;
        case ADAPTER_CLIENT:
            CALEGattConnectionStateChanged(isConnected, address);
            break;
        case ADAPTER_BOTH_CLIENT_SERVER:
            CALEGattConnectionStateChanged(isConnected, address);
            CALEGattServerConnectionStateChanged(isConnected, address);
            break;
        default:
            break;
    }
    ca_mutex_unlock(g_bleIsServerMutex);
#endif

    if(!isConnected)
    {
#ifndef SINGLE_THREAD
        if(g_bleClientSenderInfo)
        {
            CALERemoveReceiveQueueData(g_bleClientSenderInfo, address);
        }

        if(g_bleServerSenderInfo)
        {
            CALERemoveReceiveQueueData(g_bleServerSenderInfo, address);
        }

        // remove data of send queue.
        if (g_bleClientSendQueueHandle)
        {
            CALERemoveSendQueueData(g_bleClientSendQueueHandle,
                                    g_bleClientSendDataMutex,
                                    address);
        }

        if (g_bleServerSendQueueHandle)
        {
            CALERemoveSendQueueData(g_bleServerSendQueueHandle,
                                    g_bleServerSendDataMutex,
                                    address);
        }
#endif
    }

    CAEndpoint_t localEndpoint = { .adapter = CA_ADAPTER_GATT_BTLE };
    OICStrcpy(localEndpoint.addr, sizeof(localEndpoint.addr), address);

    ca_mutex_lock(g_bleNetworkCbMutex);
    if (g_connectionCallback)
    {
        g_connectionCallback(&localEndpoint, isConnected);
    }
    ca_mutex_unlock(g_bleNetworkCbMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CALEDeviceStateChangedCb(CAAdapterState_t adapter_state)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEDeviceStateChangedCb");

    if (CA_ADAPTER_ENABLED == adapter_state)
    {
        ca_mutex_lock(g_bleIsServerMutex);
        switch (g_adapterType)
        {
            case ADAPTER_SERVER:
                CALEAdapterGattServerStart();
                break;
            case ADAPTER_CLIENT:
                CALEAdapterGattClientStart();
                break;
            case ADAPTER_BOTH_CLIENT_SERVER:
                CALEAdapterGattServerStart();
                CALEAdapterGattClientStart();
                break;
            default:
                break;
        }
        ca_mutex_unlock(g_bleIsServerMutex);
    }
    else
    {
        ca_mutex_lock(g_bleIsServerMutex);
        switch (g_adapterType)
        {
            case ADAPTER_SERVER:
                CALEAdapterGattServerStop();
                break;
            case ADAPTER_CLIENT:
                CALEAdapterGattClientStop();
                break;
            case ADAPTER_BOTH_CLIENT_SERVER:
                CALEAdapterGattServerStop();
                CALEAdapterGattClientStop();
                break;
            default:
                break;
        }
        ca_mutex_unlock(g_bleIsServerMutex);
    }

    ca_mutex_lock(g_bleNetworkCbMutex);
    if (NULL != g_networkCallback)
    {
        g_networkCallback(CA_ADAPTER_GATT_BTLE, adapter_state);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "g_networkCallback is NULL");
    }
    ca_mutex_unlock(g_bleNetworkCbMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
}

static CAResult_t CALEAdapterClientSendData(const CAEndpoint_t *remoteEndpoint,
                                            const uint8_t *data,
                                            uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(data, TAG, "Param data is NULL");
#ifndef SINGLE_THREAD
    VERIFY_NON_NULL_RET(g_bleClientSendQueueHandle, TAG,
                        "g_bleClientSendQueueHandle is  NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(g_bleClientSendDataMutex, TAG,
                        "g_bleClientSendDataMutex is NULL",
                        CA_STATUS_FAILED);

    VERIFY_NON_NULL_RET(g_bleClientSendQueueHandle, TAG,
                        "g_bleClientSendQueueHandle",
                        CA_STATUS_FAILED);

    OIC_LOG_V(DEBUG, TAG, "Data Sending to LE layer [%u]", dataLen);

    CALEData_t *bleData = CACreateLEData(remoteEndpoint, data, dataLen, NULL);
    if (!bleData)
    {
        OIC_LOG(ERROR, TAG, "Failed to create bledata!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    // Add message to send queue
    ca_mutex_lock(g_bleClientSendDataMutex);
    CAQueueingThreadAddData(g_bleClientSendQueueHandle, bleData, sizeof(CALEData_t));
    ca_mutex_unlock(g_bleClientSendDataMutex);
#endif
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CALEAdapterServerSendData(const CAEndpoint_t *remoteEndpoint,
                                            const uint8_t *data,
                                            uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(data, TAG, "Param data is NULL");

#ifdef SINGLE_THREAD
    if (!CAIsLEConnected())
    {
        OIC_LOG(ERROR, TAG, "le not conn");
        return CA_STATUS_FAILED;
    }

    CAResult_t result = CA_STATUS_OK;
    const uint32_t dataLimit = dataLen / CA_SUPPORTED_BLE_MTU_SIZE;
    for (uint32_t iter = 0; iter < dataLimit; iter++)
    {
        result =
            CAUpdateCharacteristicsToAllGattClients(
                data + (iter * CA_SUPPORTED_BLE_MTU_SIZE),
                CA_SUPPORTED_BLE_MTU_SIZE);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "Update characteristics failed");
            return CA_STATUS_FAILED;
        }

        CALEDoEvents();
    }

    const uint32_t remainingLen = dataLen % CA_SUPPORTED_BLE_MTU_SIZE;
    if (remainingLen)
    {
        result =
            CAUpdateCharacteristicsToAllGattClients(
                data + (dataLimit * CA_SUPPORTED_BLE_MTU_SIZE),
                remainingLen);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "Update characteristics failed");
            return CA_STATUS_FAILED;
        }
        CALEDoEvents();
    }
#else
    VERIFY_NON_NULL_RET(g_bleServerSendQueueHandle, TAG,
                        "BleClientReceiverQueue is NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(g_bleServerSendDataMutex, TAG,
                        "BleClientSendDataMutex is NULL",
                        CA_STATUS_FAILED);

    VERIFY_NON_NULL_RET(g_bleServerSendQueueHandle, TAG, "sendQueueHandle",
                        CA_STATUS_FAILED);

    OIC_LOG_V(DEBUG, TAG, "Data Sending to LE layer [%d]", dataLen);

    CALEData_t * const bleData =
        CACreateLEData(remoteEndpoint, data, dataLen, NULL);

    if (!bleData)
    {
        OIC_LOG(ERROR, TAG, "Failed to create bledata!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Add message to send queue
    ca_mutex_lock(g_bleServerSendDataMutex);
    CAQueueingThreadAddData(g_bleServerSendQueueHandle,
                            bleData,
                            sizeof(CALEData_t));
    ca_mutex_unlock(g_bleServerSendDataMutex);
#endif
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CALEAdapterServerReceivedData(const char *remoteAddress,
                                                const uint8_t *data,
                                                uint32_t dataLength,
                                                uint32_t *sentLength)
{
    OIC_LOG(DEBUG, TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(data, TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, TAG, "Sent data length holder is null");

#ifdef SINGLE_THREAD
    if(g_networkPacketReceivedCallback)
    {
        // will be filled by upper layer
        const CASecureEndpoint_t endpoint =
            { .endpoint = { .adapter = CA_ADAPTER_GATT_BTLE } };


        g_networkPacketReceivedCallback(&endpoint, data, dataLength);
    }
#else
    VERIFY_NON_NULL_RET(g_bleReceiverQueue,
                        TAG,
                        "g_bleReceiverQueue",
                        CA_STATUS_FAILED);

    //Add message to data queue
    CAEndpoint_t * const remoteEndpoint =
        CACreateEndpointObject(CA_DEFAULT_FLAGS,
                               CA_ADAPTER_GATT_BTLE,
                               remoteAddress,
                               0);

    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    // Create bleData to add to queue
    OIC_LOG_V(DEBUG,
              TAG,
              "Data received from LE layer [%d]",
              dataLength);

    CALEData_t * const bleData =
        CACreateLEData(remoteEndpoint, data, dataLength, g_bleServerSenderInfo);

    if (!bleData)
    {
        OIC_LOG(ERROR, TAG, "Failed to create bledata!");
        CAFreeEndpoint(remoteEndpoint);
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAFreeEndpoint(remoteEndpoint);
    // Add message to receiver queue
    CAQueueingThreadAddData(g_bleReceiverQueue, bleData, sizeof(CALEData_t));

    *sentLength = dataLength;
#endif
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CALEAdapterClientReceivedData(const char *remoteAddress,
                                                const uint8_t *data,
                                                uint32_t dataLength,
                                                uint32_t *sentLength)
{
    OIC_LOG(DEBUG, TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(data, TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, TAG, "Sent data length holder is null");
#ifndef SINGLE_THREAD
    VERIFY_NON_NULL_RET(g_bleReceiverQueue, TAG,
                        "g_bleReceiverQueue",
                        CA_STATUS_FAILED);

    //Add message to data queue
    CAEndpoint_t *remoteEndpoint = CACreateEndpointObject(CA_DEFAULT_FLAGS,
                                                          CA_ADAPTER_GATT_BTLE,
                                                          remoteAddress, 0);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "Data received from LE layer [%u]", dataLength);

    // Create bleData to add to queue
    CALEData_t *bleData = CACreateLEData(remoteEndpoint, data,
                                         dataLength, g_bleClientSenderInfo);
    if (!bleData)
    {
        OIC_LOG(ERROR, TAG, "Failed to create bledata!");
        CAFreeEndpoint(remoteEndpoint);
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAFreeEndpoint(remoteEndpoint);
    // Add message to receiver queue
    CAQueueingThreadAddData(g_bleReceiverQueue, bleData, sizeof(CALEData_t));

    *sentLength = dataLength;
#endif
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static void CASetLEAdapterThreadPoolHandle(ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "IN");

    ca_mutex_lock(g_bleAdapterThreadPoolMutex);
    g_bleAdapterThreadPool = handle;
    ca_mutex_unlock(g_bleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CASetLEReqRespAdapterCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");

    ca_mutex_lock(g_bleAdapterReqRespCbMutex);

    g_networkPacketReceivedCallback = callback;

    ca_mutex_unlock(g_bleAdapterReqRespCbMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CALEErrorHandler(const char *remoteAddress,
                             const uint8_t *data,
                             uint32_t dataLen,
                             CAResult_t result)
{
    OIC_LOG(DEBUG, TAG, "CALEErrorHandler IN");

    VERIFY_NON_NULL_VOID(data, TAG, "Data is null");

    CAEndpoint_t *rep = CACreateEndpointObject(CA_DEFAULT_FLAGS,
                                               CA_ADAPTER_GATT_BTLE,
                                               remoteAddress,
                                               0);

    // if required, will be used to build remote endpoint
    g_errorHandler(rep, data, dataLen, result);

    CAFreeEndpoint(rep);

    OIC_LOG(DEBUG, TAG, "CALEErrorHandler OUT");
}

#ifndef SINGLE_THREAD
static void CALERemoveSendQueueData(CAQueueingThread_t *queueHandle, ca_mutex mutex,
                                    const char* address)
{
    OIC_LOG(DEBUG, TAG, "CALERemoveSendQueueData");

    VERIFY_NON_NULL_VOID(queueHandle, TAG, "queueHandle");
    VERIFY_NON_NULL_VOID(address, TAG, "address");

    ca_mutex_lock(mutex);
    while (u_queue_get_size(queueHandle->dataQueue) > 0)
    {
        OIC_LOG(DEBUG, TAG, "get data from queue");
        u_queue_message_t *message = u_queue_get_element(queueHandle->dataQueue);
        if (NULL != message)
        {
            CALEData_t *bleData = (CALEData_t *) message->msg;
            if (bleData && bleData->remoteEndpoint)
            {
                if (!strcmp(bleData->remoteEndpoint->addr, address))
                {
                    OIC_LOG(DEBUG, TAG, "found the message of disconnected device");
                    if (NULL != queueHandle->destroy)
                    {
                        queueHandle->destroy(message->msg, message->size);
                    }
                    else
                    {
                        OICFree(message->msg);
                    }

                    OICFree(message);
                }
            }
        }
    }
    ca_mutex_unlock(mutex);
}

static void CALERemoveReceiveQueueData(u_arraylist_t *dataInfoList, const char* address)
{
    OIC_LOG(DEBUG, TAG, "CALERemoveReceiveQueueData");

    VERIFY_NON_NULL_VOID(dataInfoList, TAG, "dataInfoList");
    VERIFY_NON_NULL_VOID(address, TAG, "address");

    CABLESenderInfo_t *senderInfo = NULL;
    uint32_t senderIndex = 0;

    if(CA_STATUS_OK == CALEGetSenderInfo(address, dataInfoList, &senderInfo,
                                         &senderIndex))
    {
        u_arraylist_remove(dataInfoList, senderIndex);
        OICFree(senderInfo->defragData);
        OICFree(senderInfo->remoteEndpoint);
        OICFree(senderInfo);

        OIC_LOG(DEBUG, TAG, "SenderInfo is removed for disconnection");
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "SenderInfo doesn't exist");
    }
}
#endif
