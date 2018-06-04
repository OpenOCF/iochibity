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

#define TAG "UDPSEND"

static int32_t CAQueueIPData(bool isMulticast, const CAEndpoint_t *endpoint,
                             const void *data, uint32_t dataLength)
{
    VERIFY_NON_NULL_RET(endpoint, TAG, "remoteEndpoint", -1);
    VERIFY_NON_NULL_RET(data, TAG, "data", -1);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, TAG, "Invalid Data Length");
        return -1;
    }

#ifdef SINGLE_THREAD

    CAIPSendData(endpoint, data, dataLength, isMulticast);
    return dataLength;

#else

    // VERIFY_NON_NULL_RET(udp_sendQueueHandle, TAG, "sendQueueHandle", -1);
    // Create IPData to add to queue
    CAIPData_t *ipData = CACreateIPData(endpoint, data, dataLength, isMulticast);
    if (!ipData)
    {
        OIC_LOG(ERROR, TAG, "Failed to create ipData!");
        return -1;
    }
    // Add message to send queue
    CAQueueingThreadAddData(&udp_sendQueueHandle, ipData, sizeof(CAIPData_t));

#endif // SINGLE_THREAD

    return dataLength;
}

#ifdef __WITH_DTLS__
ssize_t CAIPPacketSendCB(CAEndpoint_t *endpoint, const void *data, size_t dataLength)
{
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint is NULL", -1);
    VERIFY_NON_NULL_RET(data, TAG, "data is NULL", -1);

    CAIPSendData(endpoint, data, dataLength, false);
    return dataLength;
}
#endif

int32_t CASendIPUnicastData(const CAEndpoint_t *endpoint,
                            const void *data, uint32_t dataLength,
                            CADataType_t dataType)
{
    (void)dataType;
    return CAQueueIPData(false, endpoint, data, dataLength);
}

int32_t CASendIPMulticastData(const CAEndpoint_t *endpoint, const void *data, uint32_t dataLength,
                              CADataType_t dataType)
{
    (void)dataType;
    return CAQueueIPData(true, endpoint, data, dataLength);
}

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
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
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
	CAIPErrorHandler(endpoint, data, dlen, CA_STATUS_INVALID_PARAM);


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

LOCAL void udp_send_data(CASocketFd_t fd, /*  @was sendData */
			 const CAEndpoint_t *endpoint,
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

        u_arraylist_t *iflist = udp_get_ifs_for_rtm_newaddr(0);
        if (!iflist)
        {
            OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
            return;
        }

        if ((endpoint->flags & CA_IPV6) && udp_ipv6_is_enabled)
        {
            sendMulticastData6(iflist, endpoint, data, datalen);
        }
        if ((endpoint->flags & CA_IPV4) && udp_ipv4_is_enabled)
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
        if (udp_ipv6_is_enabled && (endpoint->flags & CA_IPV6))
        {
            fd = isSecure ? udp_u6s.fd : udp_u6.fd;
#ifndef __WITH_DTLS__
            fd = udp_u6.fd;
#endif
            udp_send_data(fd, endpoint, data, datalen, "unicast", "ipv6");
        }
        if (udp_ipv4_is_enabled && (endpoint->flags & CA_IPV4))
        {
            fd = isSecure ? udp_u4s.fd : udp_u4.fd;
#ifndef __WITH_DTLS__
            fd = udp_u4.fd;
#endif
            udp_send_data(fd, endpoint, data, datalen, "unicast", "ipv4");
        }
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
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
	    udp_send_data(fd, endpoint, data, datalen, "multicast", "ipv6");
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
        udp_send_data(fd, endpoint, data, datalen, "multicast", "ipv4");
    }
}

#ifndef SINGLE_THREAD

void CAIPSendDataThread(void *threadData)
{
    CAIPData_t *ipData = (CAIPData_t *) threadData;
    if (!ipData)
    {
        OIC_LOG(DEBUG, TAG, "Invalid ip data!");
        return;
    }

    if (ipData->isMulticast)
    {
        //Processing for sending multicast
        OIC_LOG(DEBUG, TAG, "Sending Multicast");
        CAIPSendData(ipData->remoteEndpoint, ipData->data, ipData->dataLen, true);
    }
    else
    {
        //Processing for sending unicast
        OIC_LOG(DEBUG, TAG, "Sending Unicast");
#ifdef __WITH_DTLS__
        if (ipData->remoteEndpoint && ipData->remoteEndpoint->flags & CA_SECURE)
        {
            OIC_LOG(DEBUG, TAG, "Sending encrypted");
            CAResult_t result = CAencryptSsl(ipData->remoteEndpoint, ipData->data, ipData->dataLen);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, TAG, "CAencryptSsl failed: %d", result);
            }
            OIC_LOG_V(DEBUG, TAG, "CAencryptSsl returned with result[%d]", result);
        }
        else
        {
            OIC_LOG(DEBUG, TAG, "Sending unencrypted");
            CAIPSendData(ipData->remoteEndpoint, ipData->data, ipData->dataLen, false);
        }
#else
        CAIPSendData(ipData->remoteEndpoint, ipData->data, ipData->dataLen, false);
#endif
    }
}

#endif

#ifndef SINGLE_THREAD
// create IP packet for sending
CAIPData_t *CACreateIPData(const CAEndpoint_t *remoteEndpoint, const void *data,
                           uint32_t dataLength, bool isMulticast)
{
    VERIFY_NON_NULL_RET(remoteEndpoint, TAG, "remoteEndpoint is NULL", NULL);
    VERIFY_NON_NULL_RET(data, TAG, "IPData is NULL", NULL);

    CAIPData_t *ipData = (CAIPData_t *) OICMalloc(sizeof(*ipData));
    if (!ipData)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return NULL;
    }

    ipData->remoteEndpoint = CACloneEndpoint(remoteEndpoint);
    ipData->data = (void *) OICMalloc(dataLength);
    if (!ipData->data)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        CAFreeIPData(ipData);
        return NULL;
    }

    memcpy(ipData->data, data, dataLength);
    ipData->dataLen = dataLength;

    ipData->isMulticast = isMulticast;

    return ipData;
}

void CAFreeIPData(CAIPData_t *ipData)
{
    VERIFY_NON_NULL_VOID(ipData, TAG, "ipData is NULL");

    CAFreeEndpoint(ipData->remoteEndpoint);
    OICFree(ipData->data);
    OICFree(ipData);
}

void CADataDestroyer(void *data, uint32_t size)
{
    if (size < sizeof(CAIPData_t))
    {
        OIC_LOG_V(ERROR, TAG, "Destroy data too small %p %d", data, size);
    }
    CAIPData_t *etdata = (CAIPData_t *) data;

    CAFreeIPData(etdata);
}

#endif // SINGLE_THREAD
