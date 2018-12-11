/** @file udp_data_receiver.c
 *
 */

#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542 // for PKTINFO
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for in6_pktinfo
#endif

#ifdef _WIN32
#undef ERROR
#endif
#include "udp_data_receiver.h"

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif

#if INTERFACE
#include <inttypes.h>
#endif

#include <errno.h>

// runs on a dedicated thread, put there by CAIPStartServer
// @rewrite: udp_data_receiver_runloop @was CAReceiveHandler
void udp_data_receiver_runloop(void *data) // @was CAReceiveHandler
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    (void)data;

    // we're on a thread, this is our run loop to read the data sockets
    // @rewrite while (!udp_is_terminating)
    while (!udp_is_terminating)
    {
	udp_handle_inbound_data(); // @was CAFindReadyMessage();
    }
    udp_cleanup(); // @was CACloseFDs();
    //oc_mutex_lock(udp_data_receiver_runloop_mutex);
    oc_cond_signal(udp_data_receiver_runloop_cond);
    //oc_mutex_unlock(udp_data_receiver_runloop_mutex);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

/* static void CAUDPPacketReceivedCB(const CASecureEndpoint_t *sep, */
/* 				  const void *data, */
/* 				  size_t dataLength) */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); */
/*     VERIFY_NON_NULL_VOID(sep, TAG, "sep is NULL"); */
/*     VERIFY_NON_NULL_VOID(data, TAG, "data is NULL"); */

/*     OIC_LOG_V(DEBUG, TAG, "Address: %s, port:%d", sep->endpoint.addr, sep->endpoint.port); */

/*     // @rewrite udp_networkPacketCallback holds cainterfacecontroller::CAReceivedPacketCallback */
/*     // we can just call that directly */

/*     if (udp_networkPacketCallback) */
/*     { */
/* 	OIC_LOG_V(DEBUG, TAG, "CALLING udp_networkPacketCallback!!!"); */
/*         udp_networkPacketCallback(sep, data, dataLength); */
/*     } else { */
/* 	OIC_LOG_V(DEBUG, TAG, "NO udp_networkPacketCallback!!!"); */
/*     } */

/* } */

