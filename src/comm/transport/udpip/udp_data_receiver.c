/** @file udp_data_receiver.c
 *
 */

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
void udp_handle_inbound_data()
{
    /* OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); */
    fd_set readFds;
    struct timeval timeout;

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

    int ret = select(udp_maxfd + 1, &readFds, NULL, NULL, tv);

    if (udp_terminate)
    {
        OIC_LOG_V(DEBUG, TAG, "Packet receiver Stop request received.");
        return;
    }

    if (0 == ret)
    {
        return;
    }
    else if (0 < ret)
    {
        // udp_process_ready_sockets(&readFds, ret);
#define UDPSET(SOCK) ( SOCK.fd != OC_INVALID_SOCKET && FD_ISSET(SOCK.fd, &readFds))
	//while (!udp_terminate)
	    {
		// ISSET(udp_u6,  readFds, CA_IPV6)
		if ( UDPSET(udp_u6) ) {
		    (void)CAReceiveMessage(udp_u6.fd, CA_IPV6);
		    FD_CLR(udp_u6.fd, &readFds);
		}
		if (udp_terminate) return;
		// ISSET(udp_u6s, readFds, CA_IPV6 | CA_SECURE)
		if ( UDPSET(udp_u6s) ) {
		    (void)CAReceiveMessage(udp_u6s.fd, CA_IPV6 | CA_SECURE);
		    FD_CLR(udp_u6s.fd, &readFds);
		}
		if (udp_terminate) return;
		/* else ISSET(udp_u4,  readFds, CA_IPV4) */
		if ( UDPSET(udp_u4) ) {
		    (void)CAReceiveMessage(udp_u4.fd, CA_IPV4);
		    FD_CLR(udp_u4.fd, &readFds);
		}
		if (udp_terminate) return;
		/* else ISSET(udp_u4s, readFds, CA_IPV4 | CA_SECURE) */
		if ( UDPSET(udp_u4s) ) {
		    (void)CAReceiveMessage(udp_u4s.fd, CA_IPV4 | CA_SECURE);
		    FD_CLR(udp_u4s.fd, &readFds);
		}
		if (udp_terminate) return;
		/* else ISSET(udp_m6,  readFds, CA_MULTICAST | CA_IPV6) */
		if ( UDPSET(udp_m6) ) {
		    (void)CAReceiveMessage(udp_m6.fd, CA_MULTICAST | CA_IPV6);
		    FD_CLR(udp_m6.fd, &readFds);
		}
		if (udp_terminate) return;
		/* else ISSET(udp_m6s, readFds, CA_MULTICAST | CA_IPV6 | CA_SECURE) */
		if ( UDPSET(udp_m6s) ) {
		    (void)CAReceiveMessage(udp_m6s.fd, CA_MULTICAST | CA_IPV6 | CA_SECURE);
		    FD_CLR(udp_m6s.fd, &readFds);
		}
		if (udp_terminate) return;
		/* else ISSET(udp_m4,  readFds, CA_MULTICAST | CA_IPV4) */
		if ( UDPSET(udp_m4) ) {
		    (void)CAReceiveMessage(udp_m4.fd, CA_MULTICAST | CA_IPV4);
		    FD_CLR(udp_m4.fd, &readFds);
		}
		if (udp_terminate) return;
		/* else ISSET(udp_m4s, readFds, CA_MULTICAST | CA_IPV4 | CA_SECURE) */
		if ( UDPSET(udp_m4s) ) {
		    (void)CAReceiveMessage(udp_m4s.fd, CA_MULTICAST | CA_IPV4 | CA_SECURE);
		    FD_CLR(udp_m4s.fd, &readFds);
		}

		// FIXME: we don't even need to read the socket, it's enough for select to wake up
		if (FD_ISSET(caglobals.ip.shutdownFds[0], &readFds)) {
		    char buf[10] = {0};
		    ssize_t len = read(udp_shutdownFds[0], buf, sizeof (buf));
		    if (len >= 0) 
			return;
		    }
		}
	    }
    }
    else // if (0 > ret)
    {
        OIC_LOG_V(FATAL, TAG, "select error %s", CAIPS_GET_ERROR);
     }
}

// runs on a dedicated thread, put there by CAIPStartServer
// @rewrite: udp_data_receiver_runloop @was CAReceiveHandler
void udp_data_receiver_runloop(void *data)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    (void)data;

    // we're on a thread, this is our run loop to read the data sockets
    // @rewrite while (!udp_terminate)
    while (!udp_terminate)
    {
        // @rewrite udp_monitor_data_sockets @was CAFindReadyMessage();
	udp_handle_inbound_data();
    }
    // @rewrite udp_shutdown() @was CACloseFDs();
    udp_cleanup();
    //oc_mutex_lock(udp_data_receiver_runloop_mutex);
    oc_cond_signal(udp_data_receiver_runloop_cond);
    //oc_mutex_unlock(udp_data_receiver_runloop_mutex);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}
