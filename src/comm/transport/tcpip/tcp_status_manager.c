/** @file tcp_status_manager.c
 *
 */


#include "tcp_status_manager.h"

#include "utlist.h"

#define TAG "TCPSTATUSMGR"

//static u_arraylist_t *g_netInterfaceList = NULL;

/* struct CANetworkCallback_t */
/* { */

/*     /\** Linked list; for multiple callback list.*\/ */
/*     struct CANetworkCallback *next; */

/*     /\** Adapter state changed event callback. *\/ */
/*     CAAdapterStateChangedCB adapter; */

/*     /\** Connection state changed event callback. *\/ */
/*     CAConnectionStateChangedCB conn; */

/* }; */

// FIXME: the "network monitor list" is transport-independent - the
// exact same code is in the UDP package. move it to pkg IP
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

    /* if (!g_netInterfaceList) */
    /* { */
    /*     g_netInterfaceList = u_arraylist_create(); */
    /*     if (!g_netInterfaceList) */
    /*     { */
    /*         OIC_LOG(ERROR, TAG, "u_arraylist_create has failed"); */
    /*         CAIPDestroyNetworkInterfaceList(); */
    /*         return CA_STATUS_FAILED; */
    /*     } */
    /* } */
    return CA_STATUS_OK;
}

// @rewrite CAIPDestroyNetworkAddressList @was CAIPDestroyNetworkMonitorList
void CAIPDestroyNetworkAddressList()
{
    /* if (g_netInterfaceList) */
    /* { */
    /*     u_arraylist_destroy(g_netInterfaceList); */
    /*     g_netInterfaceList = NULL; */
    /* } */

    if (g_networkMonitorContextMutex)
    {
        oc_mutex_free(g_networkMonitorContextMutex);
        g_networkMonitorContextMutex = NULL;
    }
}

// called by CAStartTCP
/* CAResult_t CATCPStartNetworkMonitor(void (*callback)(CATransportAdapter_t adapter, */
/* 						     CANetworkStatus_t status), */
/* 				    CATransportAdapter_t adapter) */
/* { */
/*     OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__); */
/*     CAResult_t res = CATCPInitializeNetworkMonitorList(); */
/*     if (CA_STATUS_OK == res) */
/*     { */
/* 	// insert into g_adapterCallbackList */
/*         /\* return CATCPSetNetworkMonitorCallback(callback, adapter); *\/ */
/*     } */
/*     return res; */
/* } */

void CATCPConnectionHandler(const CAEndpoint_t *endpoint, bool isConnected, bool isClient)
{
    // Pass the changed connection status to RI Layer for keepalive.
    if (tcp_connKeepAliveCallback)
    {
        tcp_connKeepAliveCallback(endpoint, isConnected, isClient);
    }

    // Pass the changed connection status to CAUtil.
    /* if (tcp_connectionChangeCallback) */
    /*     @was: g_connectionChangeCallback = CAConnectionChangedCallback */
    /* { */
    /*     tcp_connectionChangeCallback(endpoint, isConnected); */
    /* } */
    oocf_enqueue_tcp_conn_ch_work_pkg(endpoint, isConnected); // @was CAConnectionChangedCallback
}

/* src: cainterfacecontroller.c */
#ifdef STATEFUL_PROTOCOL_SUPPORTED
void oocf_enqueue_tcp_conn_ch_work_pkg /* @was: CAConnectionChangedCallback ifctrlr */
(const CAEndpoint_t *endpoint, bool isConnected)
{
    OIC_LOG_V(DEBUG, TAG, "[%s] connection state is changed to [%d]", endpoint->addr, isConnected);

    // Call the callback.
    struct CANetworkCallback *callback = NULL;

    // udp
    CANetworkCallbackThreadInfo_t *info = (CANetworkCallbackThreadInfo_t *)
        OICCalloc(1, sizeof(CANetworkCallbackThreadInfo_t));
    if (!info)
        {
            OIC_LOG(ERROR, TAG, "OICCalloc to info failed!");
            return;
        }

    CAEndpoint_t *cloneEp = CACloneEndpoint(endpoint);
    if (!cloneEp)
        {
            OIC_LOG(ERROR, TAG, "CACloneEndpoint failed!");
            OICFree(info);
            return;
        }

    info->connectionCB = callback->conn;
    info->endpoint = cloneEp;
    info->isConnected = isConnected;

    CAQueueingThreadAddData(&g_networkChangeCallbackThread, info,
                            sizeof(CANetworkCallbackThreadInfo_t));

#ifdef ENABLE_TCP
    CANetworkCallbackThreadInfo_t *tcp_info = (CANetworkCallbackThreadInfo_t *)
        OICCalloc(1, sizeof(CANetworkCallbackThreadInfo_t));
    if (!tcp_info)
        {
            OIC_LOG(ERROR, TAG, "OICCalloc to tcp_info failed!");
            return;
        }

    CAEndpoint_t *tcp_cloneEp = CACloneEndpoint(endpoint);
    if (!tcp_cloneEp)
        {
            OIC_LOG(ERROR, TAG, "CACloneEndpoint failed!");
            OICFree(tcp_info);
            return;
        }

    tcp_info->connectionCB = callback->conn;
    tcp_info->endpoint = tcp_cloneEp;
    tcp_info->isConnected = isConnected;

    CAQueueingThreadAddData(&g_networkChangeCallbackThread, tcp_info,
                            sizeof(CANetworkCallbackThreadInfo_t));

#endif
}
#endif //STATEFUL_PROTOCOL_SUPPORTED

/**
 * Pass the changed network status through the stored callback.
 */
// GAR: since IF changes are low in the stack they are passed up to both transports
// NOTE: this is called recursively, from CAIPGetInterfaceInformation
// @rewrite: tcp_if_change_handler @was CAIPPassNetworkChangesToAdapter
/* void tcp_if_change_handler(CANetworkStatus_t status) */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "%s ENTRY, status: %d", __func__, status); */

/*     tcp_status_change_handler(CA_ADAPTER_TCP, status); */
/*     // log state of TCP services, not nw interface */
/*     CALogAdapterStateInfo(CA_ADAPTER_TCP, status); */

/*     OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__); */
/* } */

// GAR this is passed to CAIPStartNetworkMonitor, in CAStartTCP
// signature: CAIPAdapterStateChangeCallback
// see also CAIPAdapterHandler (= udp_status_change_handler)
// FIXME: naming. this is about interface status - tcp_if_status_chg_handler?
void tcp_interface_change_handler(CATransportAdapter_t adapter, // @was CATCPAdapterHandler
			       CANetworkStatus_t status)
{
    /* g_networkChangeCallback == CAAdapterChangedCallback, iterates over g_networkChangeCallbackList */
    oocf_enqueue_interface_chg_work_pkg(adapter, status); // @was CAAdapterChangedCallback

    if (CA_INTERFACE_DOWN == status)
    {
        OIC_LOG(DEBUG, TAG, "Network status is down, close all session");
        CATCPStopServer();
    }
    else if (CA_INTERFACE_UP == status)
    {
        OIC_LOG(DEBUG, TAG, "TCP Network status is up, create new socket for listening");

        CAResult_t ret = CA_STATUS_FAILED;

        ret = CATCPStartServer((const ca_thread_pool_t)tcp_threadpool);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(DEBUG, TAG, "CATCPStartServer failed[%d]", ret);
        }
    }
}

// FIXME: rename tcp_state_chg_handler?
/* src: occonnectionmanager.c */
void OCAdapterStateChangedHandler(CATransportAdapter_t adapter, bool enabled)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OC_UNUSED(adapter);
    // check user configuration
    CAConnectUserPref_t connPrefer = CA_USER_PREF_CLOUD;
    CAResult_t ret = CAUtilCMGetConnectionUserConfig(&connPrefer);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "CAUtilCMGetConnectionUserConfig failed with error %u", ret);
    }

    if (CA_USER_PREF_CLOUD != connPrefer)
    {
        //set connection callback
        if (true == enabled)
        {
            OIC_LOG(DEBUG, TAG, "CM ConnectionStatusChangedHandler ENABLED");
        }
        else
        {
            OIC_LOG(DEBUG, TAG, "CM ConnectionStatusChangedHandler DISABLED");
        }
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

