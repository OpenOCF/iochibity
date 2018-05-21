/** @file udp_status_monitor.c
 *
 */

#include "udp_status_monitor.h"

/**
 * Pass the changed network status through the stored callback.
 */
// GAR: since IF changes are low in the stack they are passed up to both transports
// NOTE: this is called recursively, from CAIPGetInterfaceInformation
// @rewrite: CAIPPassNetworkChangesToTransports @was CAIPPassNetworkChangesToAdapter
void CAIPPassNetworkChangesToTransports(CANetworkStatus_t status)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s: status = %d", __func__, status);

    // @rewrite: we only ever have two handlers at most, one for udp and one for tcp.
    // what if we have multiple physical interfaces per transport?
#ifdef ENABLE_IP
    udp_status_change_handler(CA_ADAPTER_IP, status);
    // log state of TRANSPORT adapter, not nw interface
    CALogAdapterStateInfo(CA_ADAPTER_IP, status);
#endif
#ifdef ENABLE_TCP
    tcp_status_change_handler(CA_ADAPTER_TCP, status);
    CALogAdapterStateInfo(CA_ADAPTER_TCP, status);
#endif

    /* CAIPCBData_t *cbitem = NULL; */
    /* LL_FOREACH(g_adapterCallbackList, cbitem) */
    /* { */
    /*     if (cbitem && cbitem->adapter) */
    /*     { */
    /*         cbitem->ip_status_change_event_handler(cbitem->adapter, status); */
    /*         CALogAdapterStateInfo(cbitem->adapter, status); */
    /*     } */
    /* } */
    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);
}

// GAR this is passed to CAIPStartNetworkMonitor, in CAStartUDP
// signature: CAIPAdapterStateChangeCallback
// @rewrite: udp_status_change_handler @was CAIPAdapterHandler
// @rewrite: will be called directly instead of passed to setup
void udp_status_change_handler(CATransportAdapter_t adapter, CANetworkStatus_t status)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    // GAR update g_ownIpEndpointList
    CAUpdateStoredIPAddressInfo(status);

    // @rewrite: g_networkChangeCallback is always set to CAAdapterChangedCallback
    // so just call that
    /* if (g_networkChangeCallback) */
    /* { */
    /*     g_networkChangeCallback(adapter, status); */
    /* } */
    /* else */
    /* { */
    /*     OIC_LOG(ERROR, TAG, "g_networkChangeCallback is NULL"); */
    /* } */
    // GAR: propagate change to generic layer (e.g. connectivitymanager)
    CAAdapterChangedCallback(adapter, status);


    if (CA_INTERFACE_DOWN == status)
    {
        OIC_LOG(DEBUG, TAG, "Network status for IP is down");
#ifdef __WITH_DTLS__
        OIC_LOG(DEBUG, TAG, "close all ssl session");
        CAcloseSslConnectionAll(CA_ADAPTER_IP);
#endif
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

