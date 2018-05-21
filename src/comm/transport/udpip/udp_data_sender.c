/** @file udp_data_sender.c
 *
 */

#include "udp_data_sender.h"

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

// @rewrite
void CAIPSendData(CAEndpoint_t *endpoint, const void *data, size_t datalen,
                  bool isMulticast)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    bool isSecure = (endpoint->flags & CA_SECURE) != 0;

    if (isMulticast)
    {
        endpoint->port = isSecure ? CA_SECURE_COAP : CA_COAP;

        u_arraylist_t *iflist = CAIPGetInterfaceInformation(0);
        if (!iflist)
        {
            OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
            return;
        }

        if ((endpoint->flags & CA_IPV6) && udp_ipv6enabled)
        {
            sendMulticastData6(iflist, endpoint, data, datalen);
        }
        if ((endpoint->flags & CA_IPV4) && udp_ipv4enabled)
        {
            sendMulticastData4(iflist, endpoint, data, datalen);
        }

        u_arraylist_destroy(iflist);
    }
    else
    {
        if (!endpoint->port)    // unicast discovery
        {
            endpoint->port = isSecure ? CA_SECURE_COAP : CA_COAP;
        }

        CASocketFd_t fd;
        if (udp_ipv6enabled && (endpoint->flags & CA_IPV6))
        {
            fd = isSecure ? udp_u6s.fd : udp_u6.fd;
#ifndef __WITH_DTLS__
            fd = udp_u6.fd;
#endif
            sendData(fd, endpoint, data, datalen, "unicast", "ipv6");
        }
        if (udp_ipv4enabled && (endpoint->flags & CA_IPV4))
        {
            fd = isSecure ? udp_u4s.fd : udp_u4.fd;
#ifndef __WITH_DTLS__
            fd = udp_u4.fd;
#endif
            sendData(fd, endpoint, data, datalen, "unicast", "ipv4");
        }
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

// @rewrite udp_send_data @was sendData
void sendData(CASocketFd_t fd, const CAEndpoint_t *endpoint,
	      const void *data, size_t dlen,
	      const char *cast, const char *fam)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);

    if (!endpoint)
    {
        OIC_LOG(DEBUG, TAG, "endpoint is null");
	// @rewrite: no need to use g_ipErrorHandler, we just call CAIPErrorHandler directly
        /* if (g_ipErrorHandler) */
        /* { */
        /*     g_ipErrorHandler(endpoint, data, dlen, CA_STATUS_INVALID_PARAM); */
        /* } */
	CAIPErrorHandler(endpoint, data, dlen, CA_STATUS_INVALID_PARAM);
        return;
    }

    (void)cast;  // eliminates release warning
    (void)fam;

    struct sockaddr_storage sock = { .ss_family = 0 };
    CAConvertNameToAddr(endpoint->addr, endpoint->port, &sock);

    socklen_t socklen = 0;
    if (sock.ss_family == AF_INET6)
    {
        socklen = sizeof(struct sockaddr_in6);
    }
    else
    {
        socklen = sizeof(struct sockaddr_in);
    }
    PORTABLE_sendto(fd, data, dlen, 0, &sock, socklen, endpoint, cast, fam);
}

void sendMulticastData6(const u_arraylist_t *iflist,
			CAEndpoint_t *endpoint,
			const void *data, size_t datalen)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    if (!endpoint)
	{
	    OIC_LOG(DEBUG, TAG, "endpoint is null");
	    return;
	}

    int scope = endpoint->flags & CA_SCOPE_MASK;
    char *ipv6mcname = ipv6mcnames[scope];
    if (!ipv6mcname)
	{
	    OIC_LOG_V(INFO, TAG, "IPv6 multicast scope invalid: %d", scope);
	    return;
	}
    OICStrcpy(endpoint->addr, sizeof(endpoint->addr), ipv6mcname);
    CASocketFd_t fd = udp_u6.fd;

    size_t len = u_arraylist_length(iflist);
    for (size_t i = 0; i < len; i++)
	{
	    CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
	    if (!ifitem)
		{
		    continue;
		}
	    if ((ifitem->flags & IFF_UP_RUNNING_FLAGS) != IFF_UP_RUNNING_FLAGS)
		{
		    continue;
		}
	    if (ifitem->family != AF_INET6)
		{
		    continue;
		}

	    int index = ifitem->index;
	    if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_IF, OPTVAL_T(&index), sizeof (index)))
		{
		    OIC_LOG_V(ERROR, TAG, "setsockopt6 failed: %s", CAIPS_GET_ERROR);
		    return;
		}
	    sendData(fd, endpoint, data, datalen, "multicast", "ipv6");
	}
}

void sendMulticastData4(const u_arraylist_t *iflist,
			CAEndpoint_t *endpoint,
			const void *data, size_t datalen)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");

#if defined(USE_IP_MREQN)
    struct ip_mreqn mreq = { .imr_multiaddr = IPv4MulticastAddress,
                             .imr_address.s_addr = htonl(INADDR_ANY),
                             .imr_ifindex = 0};
#else
    struct ip_mreq mreq  = { .imr_multiaddr.s_addr = IPv4MulticastAddress.s_addr,
                             .imr_interface = {0}};
#endif

    OICStrcpy(endpoint->addr, sizeof(endpoint->addr), IPv4_MULTICAST);
    CASocketFd_t fd = udp_u4.fd;

    size_t len = u_arraylist_length(iflist);
    for (size_t i = 0; i < len; i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
        if (!ifitem)
        {
            continue;
        }
        if ((ifitem->flags & IFF_UP_RUNNING_FLAGS) != IFF_UP_RUNNING_FLAGS)
        {
            continue;
        }
        if (ifitem->family != AF_INET)
        {
            continue;
        }
#if defined(USE_IP_MREQN)
        mreq.imr_ifindex = ifitem->index;
#else
        mreq.imr_interface.s_addr = htonl(ifitem->index);
#endif
        if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, OPTVAL_T(&mreq), sizeof (mreq)))
        {
            OIC_LOG_V(ERROR, TAG, "send IP_MULTICAST_IF failed: %s (using defualt)",
                    CAIPS_GET_ERROR);
        }
        sendData(fd, endpoint, data, datalen, "multicast", "ipv4");
    }
}
