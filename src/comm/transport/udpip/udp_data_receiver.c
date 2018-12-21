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

#ifdef ENABLE_GCD
/* experimental Grand Central Dispatch */
#include <dispatch/dispatch.h>

LOCAL dispatch_source_t udp4recv_source;

void gcd_handler_block(void *arg)
{
    long socket4FDBytesAvailable = dispatch_source_get_data(udp4recv_source);
    /* LogVerbose(@"socket4FDBytesAvailable: %lu", socket4FDBytesAvailable); */

    if (socket4FDBytesAvailable > 0) {
        // read
    } else {
    }
}

void gcd_runner(void)
{
    int socket4FD;
    dispatch_queue_t socketQueue;
    udp4recv_source = dispatch_source_create(DISPATCH_SOURCE_TYPE_READ, socket4FD, 0, socketQueue);
    /* NSAssert(sq != dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), */
    /*          @"The given socketQueue parameter must not be a concurrent queue."); */
    /* NSAssert(sq != dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), */
    /*          @"The given socketQueue parameter must not be a concurrent queue."); */
    /* NSAssert(sq != dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), */
    /*          @"The given socketQueue parameter must not be a concurrent queue."); */

    dispatch_source_set_event_handler_f(udp4recv_source, gcd_handler_block);
}
#endif

// runs on a dedicated thread, put there by CAIPStartServer
// @rewrite: udp_data_receiver_runloop @was CAReceiveHandler
void udp_data_receiver_runloop(void *ctx) // @was CAReceiveHandler
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    (void)ctx;

    // we're on a thread, this is our run loop to read the data sockets
    // @rewrite while (!udp_is_terminating)
    while (!udp_is_terminating)
    {
	udp_handle_inbound_data(); // @was CAFindReadyMessage();
    }
    OIC_LOG_V(DEBUG, TAG, "%s terminating", __func__);
    udp_cleanup(); // @was CACloseFDs();

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

