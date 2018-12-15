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

void PORTABLE_sendto(CASocketFd_t fd,
                     const void *data,
		     size_t dlen,
		     int flags,
		     struct sockaddr_storage * sockaddrptr,
		     socklen_t socklen,
		     const CAEndpoint_t *endpoint,
		     const char *cast, const char *fam)
EXPORT
{
    (void)flags;
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OIC_LOG_V(DEBUG, TAG, "%s dest socket: %d", __func__, fd);
#ifdef TB_LOG
    const char *secure = (endpoint->flags & CA_SECURE) ? "secure " : "insecure ";
#endif
    ssize_t len = sendto(fd, data, dlen, 0, (struct sockaddr *)sockaddrptr, socklen);
    if (OC_SOCKET_ERROR == len)
    {
	// @rewrite: g_ipErrorHandler removed, call CAIPErrorHandler directly
        /* if (g_ipErrorHandler) */
        /* { */
        /*     g_ipErrorHandler(endpoint, data, dlen, CA_SEND_FAILED); */
        /* } */
	// FIXME: short-circuit to CAAdapterErrorHandleCallback
	//CAIPErrorHandler(endpoint, data, dlen, CA_STATUS_INVALID_PARAM);
	CAErrorHandler(endpoint, data, dlen, CA_STATUS_INVALID_PARAM);

        OIC_LOG_V(ERROR, TAG, "%s%s %s sendTo failed: %s", secure, cast, fam, strerror(errno));
        CALogSendStateInfo(endpoint->adapter, endpoint->addr, endpoint->port,
                           len, false, strerror(errno));
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "%s%s %s sendTo is successful: %zd bytes", secure, cast, fam, len);
        CALogSendStateInfo(endpoint->adapter, endpoint->addr, endpoint->port,
                           len, true, NULL);
    }
}
