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

// @rewrite udp_handle_inboud_data  @was CAFindReadyMessage()
void udp_handle_inbound_data()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
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
    /* if (udp_shutdownFds[0] != -1) */
    /* { */
    /*     FD_SET(udp_shutdownFds[0], &readFds); */
    /* } */
    /* if (udp_netlinkFd != OC_INVALID_SOCKET) */
    /* { */
    /*     FD_SET(udp_netlinkFd, &readFds); */
    /* } */

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
        udp_process_ready_sockets(&readFds, ret);
    }
    else // if (0 > ret)
    {
        OIC_LOG_V(FATAL, TAG, "select error %s", CAIPS_GET_ERROR);
        return;
    }
}

/* process FDs that are ready for reading */
// @rewrite udp_process_ready_sockets @was CASelectReturned
LOCAL void udp_process_ready_sockets(fd_set *readFds, int ret)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    (void)ret;			/* ret = fd count */
    CASocketFd_t fd = OC_INVALID_SOCKET;
    CATransportFlags_t flags = CA_DEFAULT_FLAGS;

    // why are we looping until termination? because we break out of
    // the loop as soon as no more FDs are ready
    while (!udp_terminate)
    {
        ISSET(udp_u6,  readFds, CA_IPV6)
        else ISSET(udp_u6s, readFds, CA_IPV6 | CA_SECURE)
        else ISSET(udp_u4,  readFds, CA_IPV4)
        else ISSET(udp_u4s, readFds, CA_IPV4 | CA_SECURE)
        else ISSET(udp_m6,  readFds, CA_MULTICAST | CA_IPV6)
        else ISSET(udp_m6s, readFds, CA_MULTICAST | CA_IPV6 | CA_SECURE)
        else ISSET(udp_m4,  readFds, CA_MULTICAST | CA_IPV4)
        else ISSET(udp_m4s, readFds, CA_MULTICAST | CA_IPV4 | CA_SECURE)
/*         else if ((udp_netlinkFd != OC_INVALID_SOCKET) && FD_ISSET(udp_netlinkFd, readFds)) */
/*         { */
/* #ifdef NETWORK_INTERFACE_CHANGED_LOGGING */
/*             OIC_LOG_V(DEBUG, TAG, "Rtnetlink event detected"); */
/* #endif */
/*             u_arraylist_t *iflist = CAFindInterfaceChange(); */
/*             if (iflist) */
/*             { */
/*                 size_t listLength = u_arraylist_length(iflist); */
/*                 for (size_t i = 0; i < listLength; i++) */
/*                 { */
/*                     CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i); */
/*                     if (ifitem) */
/*                     { */
/*                         CAProcessNewInterface(ifitem); */
/*                     } */
/*                 } */
/*                 u_arraylist_destroy(iflist); */
/*             } */
/*             break; */
/*         } */
/*         else if (FD_ISSET(udp_shutdownFds[0], readFds)) */
/*         { */
/*             char buf[10] = {0}; */
/*             ssize_t len = read(udp_shutdownFds[0], buf, sizeof (buf)); */
/*             if (-1 == len) */
/*             { */
/*                 continue; */
/*             } */
/*             break; */
/*         } */
        else
        {
            break;
        }
	/* at this point, any fd ready for reading, and flags, have been set by the ISSET macros above */
	// @rewrite: call it udp_handle_inbound_msg or similar
        (void)CAReceiveMessage(fd, flags); /* platform-specific */
        FD_CLR(fd, readFds);
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
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}


