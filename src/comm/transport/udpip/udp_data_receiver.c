/** @file udp_data_receiver.c
 *
 */

#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542 // for PKTINFO
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for in6_pktinfo
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

CAResult_t CAReceiveMessage(CASocketFd_t fd, CATransportFlags_t flags)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    char recvBuffer[RECV_MSG_BUF_LEN] = {0};
    int level = 0;
    int type = 0;
    int namelen = 0;
    struct sockaddr_storage srcAddr = { .ss_family = 0 };
    unsigned char *pktinfo = NULL;
    size_t len = 0;
    struct cmsghdr *cmp = NULL;
    struct iovec iov = { .iov_base = recvBuffer, .iov_len = sizeof (recvBuffer) };
    union control
    {
        struct cmsghdr cmsg;
        unsigned char data[CMSG_SPACE(sizeof (struct in6_pktinfo))];
    } cmsg;

    if (flags & CA_IPV6)
    {
        namelen = sizeof (struct sockaddr_in6);
        level = IPPROTO_IPV6;
        type = IPV6_PKTINFO;
        len = sizeof (struct in6_pktinfo);
    }
    else
    {
        namelen = sizeof (struct sockaddr_in);
        level = IPPROTO_IP;
        type = IP_PKTINFO;
        len = sizeof (struct in6_pktinfo);
    }

    struct msghdr msg = { .msg_name = &srcAddr,
                          .msg_namelen = namelen,
                          .msg_iov = &iov,
                          .msg_iovlen = 1,
                          .msg_control = &cmsg,
                          .msg_controllen = CMSG_SPACE(len) };

    ssize_t recvLen = recvmsg(fd, &msg, flags);
    if (OC_SOCKET_ERROR == recvLen)
    {
        OIC_LOG_V(ERROR, TAG, "Recvfrom failed %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

    for (cmp = CMSG_FIRSTHDR(&msg); cmp != NULL; cmp = CMSG_NXTHDR(&msg, cmp))
    {
        if (cmp->cmsg_level == level && cmp->cmsg_type == type)
        {
            pktinfo = CMSG_DATA(cmp);
        }
    }
    if (!pktinfo)
    {
        OIC_LOG(ERROR, TAG, "pktinfo is null");
        return CA_STATUS_FAILED;
    }

    CASecureEndpoint_t sep = {.endpoint = {.adapter = CA_ADAPTER_IP, .flags = flags}};

    if (flags & CA_IPV6)
    {
        sep.endpoint.ifindex = ((struct in6_pktinfo *)pktinfo)->ipi6_ifindex;

        if (flags & CA_MULTICAST)
        {
            struct in6_addr *addr = &(((struct in6_pktinfo *)pktinfo)->ipi6_addr);
            unsigned char topbits = ((unsigned char *)addr)[0];
            if (topbits != 0xff)
            {
                sep.endpoint.flags &= ~CA_MULTICAST;
            }
        }
    }
    else
    {
        sep.endpoint.ifindex = ((struct in_pktinfo *)pktinfo)->ipi_ifindex;

        if (flags & CA_MULTICAST)
        {
            struct in_addr *addr = &((struct in_pktinfo *)pktinfo)->ipi_addr;
            uint32_t host = ntohl(addr->s_addr);
            unsigned char topbits = ((unsigned char *)&host)[3];
            if (topbits < 224 || topbits > 239)
            {
                sep.endpoint.flags &= ~CA_MULTICAST;
            }
        }
    }

    CAConvertAddrToName(&srcAddr, namelen, sep.endpoint.addr, &sep.endpoint.port);

    if (flags & CA_SECURE)
    {
#ifdef __WITH_DTLS__
#ifdef TB_LOG
        int decryptResult =
#endif
        CAdecryptSsl(&sep, (uint8_t *)recvBuffer, recvLen);
        OIC_LOG_V(DEBUG, TAG, "CAdecryptSsl returns [%d]", decryptResult);
#else
        OIC_LOG(ERROR, TAG, "Encrypted message but no DTLS");
#endif // __WITH_DTLS__
    }
    else
    {
        /* if (g_packetReceivedCallback) */
        /* { */
        /*     g_packetReceivedCallback(&sep, recvBuffer, recvLen); */
        /* } */
	mh_CAReceivedPacketCallback(&sep, recvBuffer, recvLen);
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

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

