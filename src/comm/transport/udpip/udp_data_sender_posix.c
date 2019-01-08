/** @file udp_data_sender.c
 *
 */

#include "udp_data_sender_posix.h"

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
#include <string.h>
#include <stdio.h>

#define TAG "UDPSEND"

// FIXME: @rename. vm_sendto? (virtual method) or _oocf_vm_sendto, _oocf_platform_sendto
void PORTABLE_sendto(CASocketFd_t fd,
                     const void *data,
		     size_t dlen,
		     int flags,
		     struct sockaddr_storage * dest_sockaddr,
		     socklen_t socklen,
		     const CAEndpoint_t *endpoint)
		     /* const char *routing, const char *fam) */
EXPORT
{
    (void)flags;
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_LOG_V(DEBUG, TAG, "%s dest socket: %d", __func__, fd);
    LogEndpoint(endpoint);
    OIC_LOG_V(DEBUG, TAG,
              "sending %s %s %s",
              (endpoint->flags & CA_IPV4) ? "IPV4" : (endpoint->flags & CA_IPV6)? "IPV6" : "ERR",
              (endpoint->flags & CA_SECURE) ? "secure " : "insecure ",
              (endpoint->flags & CA_MULTICAST) ? "multicast" : "unicast");

    char dest_addr[INET6_ADDRSTRLEN + 1] = {0};
    if (dest_sockaddr->ss_family == AF_INET6) {
        inet_ntop(AF_INET6,
                  &(((struct sockaddr_in6*)dest_sockaddr)->sin6_addr),
                  dest_addr, sizeof(dest_addr));
        OIC_LOG_V(DEBUG, TAG, "\tdest addr: [%s]:%u", dest_addr,
                  ntohs(((struct sockaddr_in6*)dest_sockaddr)->sin6_port));
    }
    if (dest_sockaddr->ss_family == AF_INET) {
        inet_ntop(AF_INET,
                  &(((struct sockaddr_in*)dest_sockaddr)->sin_addr),
                  dest_addr, sizeof(dest_addr));
        OIC_LOG_V(DEBUG, TAG, "\tdest addr: [%s]:%u", dest_addr,
                  &(((struct sockaddr_in*)dest_sockaddr)->sin_port));
    }

    ssize_t len = sendto(fd, data, dlen, 0, (struct sockaddr *)dest_sockaddr, socklen);

    if (OC_SOCKET_ERROR == len)
    {
	// @rewrite: g_ipErrorHandler removed, call CAIPErrorHandler directly
        /* if (g_ipErrorHandler) */
        /* { */
        /*     g_ipErrorHandler(endpoint, data, dlen, CA_SEND_FAILED); */
        /* } */
	// FIXME: short-circuit to CAAdapterErrorHandleCallback
	//CAIPErrorHandler(endpoint, data, dlen, CA_STATUS_INVALID_PARAM);
        OIC_LOG_V(ERROR, TAG, "sendto failed: %s", strerror(errno));

	CAErrorHandler(endpoint, data, dlen, CA_STATUS_INVALID_PARAM);
        CALogSendStateInfo(endpoint->adapter, endpoint->addr, endpoint->port,
                           len, false, strerror(errno));
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "sendto successful: %zd bytes", len);
        CALogSendStateInfo(endpoint->adapter, endpoint->addr, endpoint->port,
                           len, true, NULL);
    }
}
