/** @file tcp_status_manager.c
 *
 */


#include "tcp_status_manager.h"

#include "utlist.h"

#define TAG "TCPSTATUSMGR"

//static u_arraylist_t *g_netInterfaceList = NULL;

static CAResult_t CATCPInitializeNetworkMonitorList(void)
{
    if (!g_networkMonitorContextMutex)
    {
        g_networkMonitorContextMutex = oc_mutex_new();
        if (!g_networkMonitorContextMutex)
        {
            OIC_LOG(ERROR, TAG, "oc_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (!g_netInterfaceList)
    {
        g_netInterfaceList = u_arraylist_create();
        if (!g_netInterfaceList)
        {
            OIC_LOG(ERROR, TAG, "u_arraylist_create has failed");
            CAIPDestroyNetworkInterfaceList();
            return CA_STATUS_FAILED;
        }
    }
    return CA_STATUS_OK;
}

// @rewrite CAIPDestroyNetworkAddressList @was CAIPDestroyNetworkMonitorList
void CAIPDestroyNetworkAddressList()
{
    if (g_netInterfaceList)
    {
        u_arraylist_destroy(g_netInterfaceList);
        g_netInterfaceList = NULL;
    }

    if (g_networkMonitorContextMutex)
    {
        oc_mutex_free(g_networkMonitorContextMutex);
        g_networkMonitorContextMutex = NULL;
    }
}

CAResult_t CATCPSetNetworkMonitorCallback(void (*callback)(CATransportAdapter_t adapter,
							   CANetworkStatus_t status),
					  //CAIPAdapterStateChangeCallback callback,
					  CATransportAdapter_t adapter)
{
    if (!callback)
    {
        OIC_LOG(ERROR, TAG, "callback is null");
        return CA_STATUS_INVALID_PARAM;
    }

    CAIPCBData_t *cbitem = NULL;
    LL_FOREACH(g_adapterCallbackList, cbitem)
    {
        if (cbitem && adapter == cbitem->adapter && callback == cbitem->ip_status_change_event_handler)
        {
            OIC_LOG(DEBUG, TAG, "this callback is already added");
            return CA_STATUS_OK;
        }
    }

    cbitem = (CAIPCBData_t *)OICCalloc(1, sizeof(*cbitem));
    if (!cbitem)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed");
        return CA_STATUS_FAILED;
    }

    cbitem->adapter = adapter;
    cbitem->ip_status_change_event_handler = callback;
    LL_APPEND(g_adapterCallbackList, cbitem);

    return CA_STATUS_OK;
}

CAResult_t CATCPStartNetworkMonitor(void (*callback)(CATransportAdapter_t adapter,
						     CANetworkStatus_t status),
				    CATransportAdapter_t adapter)
{
    CAResult_t res = CATCPInitializeNetworkMonitorList();
    if (CA_STATUS_OK == res)
    {
	// insert into g_adapterCallbackList
        return CATCPSetNetworkMonitorCallback(callback, adapter);
    }
    return res;
}

void CATCPConnectionHandler(const CAEndpoint_t *endpoint, bool isConnected, bool isClient)
{
    // Pass the changed connection status to RI Layer for keepalive.
    if (tcp_connKeepAliveCallback)
    {
        tcp_connKeepAliveCallback(endpoint, isConnected, isClient);
    }

    // Pass the changed connection status to CAUtil.
    if (tcp_connectionChangeCallback)
    {
        tcp_connectionChangeCallback(endpoint, isConnected);
    }
}

/**
 * Pass the changed network status through the stored callback.
 */
// GAR: since IF changes are low in the stack they are passed up to both transports
// NOTE: this is called recursively, from CAIPGetInterfaceInformation
// @rewrite: tcp_if_change_handler @was CAIPPassNetworkChangesToAdapter
void tcp_if_change_handler(CANetworkStatus_t status)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, status: %d", __func__, status);

    tcp_status_change_handler(CA_ADAPTER_TCP, status);
    // log state of TCP services, not nw interface
    CALogAdapterStateInfo(CA_ADAPTER_TCP, status);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

// GAR this is passed to CAIPStartNetworkMonitor, in CAStartTCP
// signature: CAIPAdapterStateChangeCallback
// see also CAIPAdapterHandler (= udp_status_change_handler)
void tcp_status_change_handler(CATransportAdapter_t adapter, // @was CATCPAdapterHandler
			       CANetworkStatus_t status)
{
    if (tcp_networkChangeCallback)
    {
        tcp_networkChangeCallback(adapter, status);
    }

    if (CA_INTERFACE_DOWN == status)
    {
        OIC_LOG(DEBUG, TAG, "Network status is down, close all session");
        CATCPStopServer();
    }
    else if (CA_INTERFACE_UP == status)
    {
        OIC_LOG(DEBUG, TAG, "Network status is up, create new socket for listening");

        CAResult_t ret = CA_STATUS_FAILED;
#ifndef SINGLE_THREAD
        ret = CATCPStartServer((const ca_thread_pool_t)caglobals.tcp.threadpool);
#else
        ret = CATCPStartServer();
#endif
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(DEBUG, TAG, "CATCPStartServer failed[%d]", ret);
        }
    }
}
