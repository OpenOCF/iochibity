/** @file tcp_status_manager.c
 *
 */


#include "udp_status_manager.h"

#include "utlist.h"

#define TAG "TCPSTATUSMGR"


/**
 * Pass the changed network status through the stored callback.
 */
// GAR: since IF changes are low in the stack they are passed up to both transports
// NOTE: this is called recursively, from CAIPGetInterfaceInformation
// @rewrite: CAIPPassNetworkChangesToTransports @was CAIPPassNetworkChangesToAdapter
void CAIPPassNetworkChangesToTransports(CANetworkStatus_t status)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, status: %d", __func__, status);

    tcp_status_change_handler(CA_ADAPTER_TCP, status);
    // log state of TCP services, not nw interface
    CALogAdapterStateInfo(CA_ADAPTER_TCP, status);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

