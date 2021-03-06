/******************************************************************
 *
 * Copyright 2015 Intel Corporation All Rights Reserved.
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

#include "recv.h"

#include "caremotehandler.h"
#include "oic_malloc.h"
#include "oic_string.h"

#include <string.h>
#include <assert.h>


// Logging tag.
#define TAG "BLE_RECV"

/**
 * Information used to keep track of received data fragments.
 */
typedef struct _CAGattRecvInfo
{

    /**
     * @name Peer Connection-specific Fields
     *
     * These fields are valid only as long as the peer is connected.
     */
    //@{
    /// Peer address.
    char * peer;

    /// Callback invoked upon receiving all data from GATT peer.
    CABLEDataReceivedCallback on_packet_received;

    /**
     * Context object containing lock used for synchronized access to
     * the @c on_packet_received callback since that callback is actually
     * owned by it.
     */
    CALEContext * context;
    //@}

} CAGattRecvInfo;

static CAGattRecvInfo const g_null_info =
    {
        .peer = NULL
    };

void CAGattRecvInfoInitialize(CAGattRecvInfo * info)
{
    *info = g_null_info;
}

void CAGattRecvInfoDestroy(CAGattRecvInfo * info)
{
    OICFree(info->peer);
    *info = g_null_info;
}

bool CAGattRecv(CAGattRecvInfo * info,
                uint8_t const * data,
                uint32_t length)
{
    uint32_t sent_length = 0;

    oc_mutex_lock(info->context->lock);

    bool const success =
        info->on_packet_received(info->peer,
                                 data,
                                 length,
                                 &sent_length) == CA_STATUS_OK;

    oc_mutex_unlock(info->context->lock);

    return success && length == sent_length;
}
