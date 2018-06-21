/** @file udp_data_receiver.c
 *
 */

#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542 // for PKTINFO
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for in6_pktinfo
#endif

#include "udp_data_receiver_linux.h"

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

#define SET(TYPE, FDS) \
    if (TYPE.fd != OC_INVALID_SOCKET) \
    { \
        FD_SET(TYPE.fd, FDS); \
    }

#define ISSET(TYPE, FDS, FLAGS) \
    if (TYPE.fd != OC_INVALID_SOCKET && FD_ISSET(TYPE.fd, FDS)) \
    { \
        fd = TYPE.fd; \
        flags = FLAGS; \
    }

// FIXME: if there is no inbound data, this will cause hang upon termination?
// @rewrite udp_handle_inboud_data  @was CAFindReadyMessage()
/* called by udp_data_receiver_runloop */
void udp_handle_inbound_data() // @was CAFindReadyMessage
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    fd_set readFds;
    struct timeval timeout;

    static int ready_count;
    ready_count = 0;
    OIC_LOG_V(DEBUG, TAG, "ready count: %d", ready_count);

    timeout.tv_sec = udp_selectTimeout;
    timeout.tv_usec = 0;
    struct timeval *tv = udp_selectTimeout == -1 ? NULL : &timeout;

    FD_ZERO(&readFds);
    SET(udp_u6,  &readFds)
	SET(udp_u6s, &readFds)
	SET(udp_u4,  &readFds)
	SET(udp_u4s, &readFds)
	SET(udp_m6,  &readFds)
	SET(udp_m6s, &readFds)
	SET(udp_m4,  &readFds)
	SET(udp_m4s, &readFds)

	// FIXME: don't do shutdown and status sockets here!
    if (udp_shutdownFds[0] != -1)
    {
        FD_SET(udp_shutdownFds[0], &readFds);
    }

    if (udp_netlinkFd != OC_INVALID_SOCKET)
    {
        FD_SET(udp_netlinkFd, &readFds);
    }

    ready_count = select(udp_maxfd + 1, &readFds, NULL, NULL, tv);
    OIC_LOG_V(DEBUG, TAG, "SELECT ready_count: %d", ready_count);

    if (udp_is_terminating)
    {
        OIC_LOG_V(DEBUG, TAG, "Packet receiver Stop request received.");
        return;
    }

    if (0 == ready_count)
    {
        return;
    }
    else if (0 < ready_count) {
	// udp_process_ready_sockets(&readFds, ready_count);
#define UDPSET(SOCK) ( SOCK.fd != OC_INVALID_SOCKET && FD_ISSET(SOCK.fd, &readFds))

	//while (!udp_is_terminating)

	// ISSET(udp_u6,  readFds, CA_IPV6)
	if ( UDPSET(udp_u6) ) {
	    OIC_LOG(DEBUG, TAG, "udp_u6 socket ready");
	    (void)CAReceiveMessage(udp_u6.fd, CA_IPV6);
	    FD_CLR(udp_u6.fd, &readFds);
	    ready_count--;
	}
	if (ready_count < 1) return;
	if (udp_is_terminating) return;

	// ISSET(udp_u6s, readFds, CA_IPV6 | CA_SECURE)
	if ( UDPSET(udp_u6s) ) {
	    OIC_LOG(DEBUG, TAG, "udp_u6s socket ready");
	    (void)CAReceiveMessage(udp_u6s.fd, CA_IPV6 | CA_SECURE);
	    FD_CLR(udp_u6s.fd, &readFds);
	    ready_count--;
	}
	if (ready_count < 1) return;
	if (udp_is_terminating) return;

	/* else ISSET(udp_u4,  readFds, CA_IPV4) */
	if ( UDPSET(udp_u4) ) {
	    OIC_LOG(DEBUG, TAG, "udp_u4 socket ready");
	    (void)CAReceiveMessage(udp_u4.fd, CA_IPV4);
	    FD_CLR(udp_u4.fd, &readFds);
	    ready_count--;
	}
	if (ready_count < 1) return;
	if (udp_is_terminating) return;

	/* else ISSET(udp_u4s, readFds, CA_IPV4 | CA_SECURE) */
	if ( UDPSET(udp_u4s) ) {
	    OIC_LOG(DEBUG, TAG, "udp_u4s socket ready");
	    (void)CAReceiveMessage(udp_u4s.fd, CA_IPV4 | CA_SECURE);
	    FD_CLR(udp_u4s.fd, &readFds);
	    ready_count--;
	}
	if (ready_count < 1) return;
       	if (udp_is_terminating) return;

	/* else ISSET(udp_m6,  readFds, CA_MULTICAST | CA_IPV6) */
	if ( UDPSET(udp_m6) ) {
	    OIC_LOG(DEBUG, TAG, "udp_m6 socket ready");
	    (void)CAReceiveMessage(udp_m6.fd, CA_MULTICAST | CA_IPV6);
	    FD_CLR(udp_m6.fd, &readFds);
	    ready_count--;
	}
	if (ready_count < 1) return;
	if (udp_is_terminating) return;

	/* else ISSET(udp_m6s, readFds, CA_MULTICAST | CA_IPV6 | CA_SECURE) */
	if ( UDPSET(udp_m6s) ) {
	    OIC_LOG(DEBUG, TAG, "udp_m6s socket ready");
	    (void)CAReceiveMessage(udp_m6s.fd, CA_MULTICAST | CA_IPV6 | CA_SECURE);
	    FD_CLR(udp_m6s.fd, &readFds);
	    ready_count--;
	}
	if (ready_count < 1) return;
	if (udp_is_terminating) return;

	/* else ISSET(udp_m4,  readFds, CA_MULTICAST | CA_IPV4) */
	if ( UDPSET(udp_m4) ) {
	    OIC_LOG(DEBUG, TAG, "udp_m4 socket ready");
	    (void)CAReceiveMessage(udp_m4.fd, CA_MULTICAST | CA_IPV4);
	    FD_CLR(udp_m4.fd, &readFds);
	    ready_count--;
	}
	if (ready_count < 1) return;
	if (udp_is_terminating) return;

	/* else ISSET(udp_m4s, readFds, CA_MULTICAST | CA_IPV4 | CA_SECURE) */
	if ( UDPSET(udp_m4s) ) {
	    OIC_LOG(DEBUG, TAG, "udp_m4s socket ready");
	    (void)CAReceiveMessage(udp_m4s.fd, CA_MULTICAST | CA_IPV4 | CA_SECURE);
	    FD_CLR(udp_m4s.fd, &readFds);
	    ready_count--;
	}
	if (ready_count < 1) return;
	if (udp_is_terminating) return;

	// FIXME: put status monitoring on separate thread?
        if ((udp_netlinkFd != OC_INVALID_SOCKET) && FD_ISSET(udp_netlinkFd, &readFds))
	{
	    OIC_LOG(DEBUG, TAG, "udp_netlinkFd socket ready");
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
            OIC_LOG_V(DEBUG, TAG, "UDP Netlink event detected");
#endif
	    // get list of RTM_NEWADDR interfaces (not addresses)
            u_arraylist_t *iflist = udp_if_change_handler_linux(); // @was CAFindInterfaceChange();
            if (iflist)
            {
                size_t listLength = u_arraylist_length(iflist);
                for (size_t i = 0; i < listLength; i++)
                {
                    CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
                    if (ifitem)
                    {
			udp_add_if_to_multicast_groups(ifitem); // @was CAProcessNewInterface(ifitem);
                    }
                }
                u_arraylist_destroy(iflist);
            }
	    return;
        }

	// FIXME: we don't even need to read the shutdown socket, it's enough for select to wake up?
	if (FD_ISSET(udp_shutdownFds[0], &readFds)) {
	    OIC_LOG(DEBUG, TAG, "udp_shutdownFds[0] socket ready");
	    char buf[10] = {0};
	    ssize_t len = read(udp_shutdownFds[0], buf, sizeof (buf));
	    /* if (len >= 0) */
	    /* 	return; */
	}
    }
    else // if (0 > ready_count)
    {
        OIC_LOG_V(FATAL, TAG, "select error %s", CAIPS_GET_ERROR);
     }
}
