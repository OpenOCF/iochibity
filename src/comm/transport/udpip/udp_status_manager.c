/** @file udp_status_manager.c
 *
 */

#include "udp_status_manager.h"

#include "utlist.h"

#include <fcntl.h>
#include <errno.h>

#define TAG "UDPSTATUSMGR"

/*
 * Structure for IP address information, to be used to construct a CAEndpoint_t.  The
 * structure name is a misnomer, as there is one entry per address not one per interface.
 * An interface with 4 addresses should result in 4 instances of CAInterface_t.
 */
#if EXPORT_INTERFACE
#define INTERFACE_NAME_MAX 16
#include <stddef.h>

typedef struct
{
    char name[INTERFACE_NAME_MAX];
    uint32_t index;
    uint32_t flags;
    uint16_t family;
    char addr[MAX_ADDR_STR_SIZE_CA];
} CAInterface_t;		// @rewrite to CAIfAddress_t */

typedef struct CAIPCBData_t
{
    struct CAIPCBData_t *next;
    CATransportAdapter_t adapter; /* will always be either CA_ADAPTER_IP (UDP) or CA_ADAPTER_TCP */
    void (*ip_status_change_event_handler)(CATransportAdapter_t transport_type,
					   CANetworkStatus_t status);
    // @rewrite CAIPAdapterStateChangeCallback callback;
} CAIPCBData_t;
#endif

/**
 * Mutex for synchronizing access to cached interface and IP address information.
 */
/* #if EXPORT_INTERFACE */
oc_mutex g_networkMonitorContextMutex = NULL;
/* #endif */

/**
 * Used to storing network interface.
 * list of CAInterface_t* (i.e., IF address structs)
 */
// g_netInterfaceList is a list of nw INTERFACES! one entry per interface,
// regardless of address and address family. InterfaceListContains compares only the IF index.
// @rewrite g_netInterfaceList @was g_netInterfaceList
u_arraylist_t *g_netInterfaceList = NULL;

// struct CAIPCBData_t *g_adapterCallbackList = NULL;

/**
 * Let the network monitor update the polling interval.
 * @param[in] interval Current polling interval, in seconds
 *
 * @return  desired polling interval
 */
/* int CAGetPollingInterval(int interval) */
/* { */
/*     return interval; */
/* } */

// FIXME: the "network monitor list" is transport-independent - the
// exact same code is in the TCP package. move it to pkg IP
CAResult_t ip_create_network_interface_list() // @was CAIPInitializeNetworkMonitorList
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

/**
 * Pass the changed network status through the stored callback.
 */
// GAR: since IF changes are low in the stack they are passed up to both transports
// NOTE: this is called recursively, from udp_get_ifs_for_rtm_newaddr
// callers:
// udp_status_manager_posix::udp_get_ifs_for_rtm_newaddr
// udp_status_manager_linux::CAFindInterfaceChange, called by CASelectReturned when netlinkFd ready
//    RTM_DELADDR => CAIPPassNetworkChangesToTransports
//    RTM_NEWADDR => posix:udp_get_ifs_for_rtm_newaddr => getifaddrs then CAIPPassNetworkChangesToTransports
// udp_status_manager_darwin::CAFindInterfaceChange, called on nw interface change
// caipnwmonitor_windows::CAFindInterfaceChange
/* void udp_if_change_handler(CANetworkStatus_t status)  // @was CAIPPassNetworkChangesToAdapter */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "%s ENTRY, status: %d", __func__, status); */

/* #ifdef IP_ADAPTER */
/*     udp_status_change_handler(CA_ADAPTER_IP, status); // @was CAIPAdapterHandler */
/* #endif */
/* #ifdef TCP_ADAPTER */
/*     tcp_status_change_handler(CA_ADAPTER_IP, status); // @was CATCPAdapterHandler */
/* #endif */

/*     // etc. for other transports */

/*     /\* CAIPCBData_t *cbitem = NULL; *\/ */
/*     /\* LL_FOREACH(g_adapterCallbackList, cbitem) *\/ */
/*     /\* { *\/ */
/*     /\*     if (cbitem && cbitem->adapter) *\/ */
/*     /\*     { *\/ */
/*     /\*         cbitem->callback(cbitem->adapter, status); *\/ */
/*     /\*         CALogAdapterStateInfo(cbitem->adapter, status); *\/ */
/*     /\*     } *\/ */
/*     /\* } *\/ */

/*     // log state of UDP services, not nw interface */
/*     /\* CALogAdapterStateInfo(CA_ADAPTER_IP, status); *\/ */

/*     OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__); */
/* } */

// @rewrite: udp_status_change_handler @was CAIPAdapterHandler
void udp_status_change_handler(CATransportAdapter_t adapter,  //@was CAIPAdapterHandler
			       CANetworkStatus_t status)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    udp_update_local_endpoint_cache(status); // @was CAUpdateStoredIPAddressInfo (g_ownIpEndpointList)

    // original code called g_networkChangeCallback, which is ptr to CAAdapterChangedCallback
    // we do not need to go through g_networkChangeCallbackList, we can just call directly
    oocf_enqueue_nw_chg_work_pkg(CA_ADAPTER_IP, status); // @was CAAdapterChangedCallback
    // the handler is OCDefaultAdapterStateChangedHandler(CA_ADAPTER_IP, status);

    // CAAdapterChangedCallback in turn iterates over
    // g_networkChangeCallbackList, putting a task on the msg queue
    // for each.

    // code from CAAdapterChangedCallback:
    /* CANetworkCallbackThreadInfo_t *info */
    /* 	= (CANetworkCallbackThreadInfo_t *) OICCalloc(1, sizeof(CANetworkCallbackThreadInfo_t)); */
    /* if (!info) */
    /* 	{ */
    /* 	    OIC_LOG(ERROR, TAG, "OICCalloc to info failed!"); */
    /* 	    return; */
    /* 	} */

    /* // the CB is OCDefaultAdapterStateChangedHandler */
    /* info->adapterCB = OCDefaultAdapterStateChangedHandler; // @was callback->adapter; */
    /* info->adapter = CA_ADAPTER_IP;  // @was adapter; */
    /* info->isInterfaceUp = (CA_INTERFACE_UP == status); */

    /* CAQueueingThreadAddData(&g_networkChangeCallbackThread, info, */
    /* 			    sizeof(CANetworkCallbackThreadInfo_t)); */

    if (CA_INTERFACE_DOWN == status)
    {
        OIC_LOG(DEBUG, TAG, "Network status for IP is down");
#ifdef __WITH_DTLS__
        OIC_LOG(DEBUG, TAG, "close all ssl session");
        CAcloseSslConnectionAll(CA_ADAPTER_IP);
#endif
    }
    CALogAdapterStateInfo(CA_ADAPTER_IP, status);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

/**
 * Set callback for receiving local IP/TCP adapter connection status.
 *
 * @param[in]  callback     Callback to be notified when IP/TCP adapter connection state changes.
 * @param[in]  adapter      Transport adapter.
 * @return ::CA_STATUS_OK or an appropriate error code.
 */
// GAR: called by both CAStartUDP and CAStartTCP via CAIPStartNetworkMonitor, to install their
// status change event handlers
// @rewrite we can eliminate this by just calling the handlers
// @rewrite directly in CAIPPassNetworkChangesToAdapter
/* CAResult_t REMOVED_CAIPSetNetworkMonitorCallback(void */
/* 					 (*ip_status_change_handler)(CATransportAdapter_t adapter, */
/* 									    CANetworkStatus_t status), */
/* 					 // CAIPAdapterStateChangeCallback callback, */
/*                                          CATransportAdapter_t adapter) */
/* { */
/*     if (!ip_status_change_handler) */
/*     { */
/*         OIC_LOG(ERROR, TAG, "ip_status_change_handler is null"); */
/*         return CA_STATUS_INVALID_PARAM; */
/*     } */

/*     CAIPCBData_t *cbitem = NULL; */
/*     LL_FOREACH(g_adapterCallbackList, cbitem) */
/*     { */
/*         if (cbitem */
/* 	    && (cbitem->adapter == adapter) */
/* 	    && (cbitem->ip_status_change_event_handler == ip_status_change_handler)) */
/*         { */
/*             OIC_LOG(DEBUG, TAG, "this ip_status_change_handler is already added"); */
/*             return CA_STATUS_OK; */
/*         } */
/*     } */

/*     cbitem = (CAIPCBData_t *)OICCalloc(1, sizeof(*cbitem)); */
/*     if (!cbitem) */
/*     { */
/*         OIC_LOG(ERROR, TAG, "Malloc failed"); */
/*         return CA_STATUS_FAILED; */
/*     } */

/*     cbitem->adapter = adapter; */
/*     cbitem->ip_status_change_event_handler = ip_status_change_handler; */
/*     LL_APPEND(g_adapterCallbackList, cbitem); */

/*     return CA_STATUS_OK; */
/* } */

/**
 * Unset callback for receiving local IP/TCP adapter connection status.
 *
 * @param[in]  adapter      Transport adapter.
 * @return CA_STATUS_OK.
 */
// @rewrite CAIPUnSetNetworkMonitorCallback is defunct
/* CAResult_t CAIPUnSetNetworkMonitorCallback(CATransportAdapter_t adapter) */
/* { */
/*     CAIPCBData_t *cbitem = NULL; */
/*     CAIPCBData_t *tmpCbitem = NULL; */
/*     LL_FOREACH_SAFE(g_adapterCallbackList, cbitem, tmpCbitem) */
/*     { */
/*         if (cbitem && adapter == cbitem->adapter) */
/*         { */
/*             OIC_LOG(DEBUG, TAG, "remove specific ip_status_change_event_handler"); */
/*             LL_DELETE(g_adapterCallbackList, cbitem); */
/*             OICFree(cbitem); */
/*             return CA_STATUS_OK; */
/*         } */
/*     } */
/*     return CA_STATUS_OK; */
/* } */

void CAInitializeFastShutdownMechanism(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    udp_selectTimeout = -1; // don't poll for shutdown
    int ret = -1;
#if defined(WSA_WAIT_EVENT_0)
    udp_shutdownEvent = WSACreateEvent();
    if (WSA_INVALID_EVENT != udp_shutdownEvent)
    {
        ret = 0;
    }
#elif defined(HAVE_PIPE2)
    ret = pipe2(udp_shutdownFds, O_CLOEXEC);
    UDP_CHECKFD(udp_shutdownFds[0]);
    UDP_CHECKFD(udp_shutdownFds[1]);
#else
    // ret = pipe(udp_shutdownFds);
    if (pipe(udp_shutdownFds) == -1) {
	OIC_LOG_V(ERROR, TAG, "pipe(udp_shutdownFds) fail: %s", CAIPS_GET_ERROR);
	goto errexit2;
    }

    OIC_LOG_V(DEBUG, TAG, "%s udp_shutdownFds[0] fd = %d", __func__, udp_shutdownFds[0]);
    OIC_LOG_V(DEBUG, TAG, "%s udp_shutdownFds[1] fd = %d", __func__, udp_shutdownFds[1]);

    int flags;

    flags = fcntl(udp_shutdownFds[0], F_GETFL);
    if (flags == -1) {
        OIC_LOG_V(ERROR, TAG, "udp_shutdownFds[0] F_GETFL: %s", CAIPS_GET_ERROR);
	goto errexit;
    }
    flags |= O_NONBLOCK;                /* Make read end nonblocking */
    if (fcntl(udp_shutdownFds[0], F_SETFL, flags) == -1) {
        OIC_LOG_V(ERROR, TAG, "udp_shutdownFds[0] F_SETFL: %s", CAIPS_GET_ERROR);
	goto errexit;
    }

    flags = fcntl(udp_shutdownFds[1], F_GETFL);
    if (flags == -1) {
        OIC_LOG_V(ERROR, TAG, "udp_shutdownFds[1] F_GETFL: %s", CAIPS_GET_ERROR);
	goto errexit;
    }
     flags |= O_NONBLOCK;                /* Make read end nonblocking */
     if (fcntl(udp_shutdownFds[1], F_SETFL, flags) == -1) {
        OIC_LOG_V(ERROR, TAG, "udp_shutdownFds[1] F_SETFL: %s", CAIPS_GET_ERROR);
	goto errexit;
     }

    /* if (-1 != ret) */
    /* { */
    /*     ret = fcntl(udp_shutdownFds[0], F_GETFD); */
    /*     if (-1 != ret) */
    /*     { */
    /*         ret = fcntl(udp_shutdownFds[0], F_SETFD, ret|FD_CLOEXEC); */
    /*     } */
    /*     if (-1 != ret) */
    /*     { */
    /*         ret = fcntl(udp_shutdownFds[1], F_GETFD); */
    /*     } */
    /*     if (-1 != ret) */
    /*     { */
    /*         ret = fcntl(udp_shutdownFds[1], F_SETFD, ret|FD_CLOEXEC); */
    /*     } */
    /*     if (-1 == ret) */
    /*     { */
    /*         close(udp_shutdownFds[1]); */
    /*         close(udp_shutdownFds[0]); */
    /*         udp_shutdownFds[0] = -1; */
    /*         udp_shutdownFds[1] = -1; */
    /*     } */
    /* } */

    UDP_CHECKFD(udp_shutdownFds[0]);
    UDP_CHECKFD(udp_shutdownFds[1]);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return;
#endif

 errexit:
    close(udp_shutdownFds[1]);
    close(udp_shutdownFds[0]);
 errexit2:
    udp_shutdownFds[0] = -1;
    udp_shutdownFds[1] = -1;
    /* if (-1 == ret) */
    /* { */
        /* OIC_LOG_V(ERROR, TAG, "fast shutdown mechanism init failed: %s", CAIPS_GET_ERROR); */
    udp_selectTimeout = SELECT_TIMEOUT; //poll needed for shutdown
    /* } */
    OIC_LOG_V(DEBUG, TAG, "%s ERROR EXIT", __func__);
}
