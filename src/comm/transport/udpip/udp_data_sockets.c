/** @file udp_data_sockets.c
 *
 */

#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542 // for PKTINFO
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for in6_pktinfo
#endif

#include "udp_data_sockets.h"

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif

#if INTERFACE
#include <sys/socket.h>
#include <netinet/in.h>
#include <inttypes.h>
#endif

#include <errno.h>

#define USE_IP_MREQN

/* udp globals */
// from CAGlobals_t in _globals.h
#ifndef PORT_U6
#define PORT_U6 0
#endif
CASocket_t udp_u6  = { .fd = OC_INVALID_SOCKET, .port = PORT_U6 };   /**< unicast   IPv6 */

#ifndef PORT_U6S
#define PORT_U6S 0
#endif
CASocket_t udp_u6s = { .fd = OC_INVALID_SOCKET, .port = PORT_U6S }; /**< unicast   IPv6 secure */

#ifndef PORT_U4
#define PORT_U4 0
#endif
CASocket_t udp_u4  = { .fd = OC_INVALID_SOCKET, .port = PORT_U4 };   /**< unicast   IPv4 */

#ifndef PORT_U4S
#define PORT_U4S 0
#endif
CASocket_t udp_u4s = { .fd = OC_INVALID_SOCKET, .port = PORT_U4S };  /**< unicast   IPv4 secure */

CASocket_t udp_m6  = { .fd = OC_INVALID_SOCKET, .port = CA_COAP };        /**< multicast IPv6 */
CASocket_t udp_m6s = { .fd = OC_INVALID_SOCKET, .port = CA_SECURE_COAP }; /**< multicast IPv6 secure */
CASocket_t udp_m4  = { .fd = OC_INVALID_SOCKET, .port = CA_COAP };        /**< multicast IPv4 */
CASocket_t udp_m4s = { .fd = OC_INVALID_SOCKET, .port = CA_SECURE_COAP }; /**< multicast IPv4 secure */

//FIXME: status and shutdown monitoring are platform-specific

#ifndef __APPLE__
int udp_netlinkFd;              /**< netlink */
#endif

int udp_shutdownFds[2]; // = { 80, 81 }; /**< pipe used to signal threads to stop */

CASocketFd_t udp_maxfd;         /**< highest fd (for select) */


void *udp_threadpool;           /**< threadpool between Initialize and Start */
int   udp_selectTimeout;          /**< in seconds */

bool  udp_is_started;               /**< the IP adapter has started */
bool  udp_is_terminating;             /**< the IP adapter needs to stop */
bool  udp_ipv6_is_enabled = true;    /**< IPv6 enabled by OCInit flags */
bool  udp_ipv4_is_enabled = true;    /**< IPv4 enabled by OCInit flags */
bool  udp_is_dualstack   = true;    /**< IPv6 and IPv4 enabled */

/* DELEGATED: #define UDP_CHECKFD(FD) */

bool PORTABLE_check_setsockopt_err() EXPORT
{
    return EADDRINUSE != errno;
}

bool PORTABLE_check_setsockopt_m4s_err(struct ip_mreqn *mreq, int ret) EXPORT
{
    /* args not used in posix, used in windows */
    (void)mreq;
    (void)ret;
    return EADDRINUSE != errno;
}

bool PORTABLE_check_setsockopt_m6_err(CASocketFd_t fd, struct ipv6_mreq *mreq,  int ret) EXPORT
{
    /* args not used in posix, used in windows */
    (void)fd;
    (void)mreq;
    (void)ret;
    return EADDRINUSE != errno;
}

CASocketFd_t udp_create_socket(int family, uint16_t *port, bool isMulticast)
{
    int socktype = SOCK_DGRAM;
#ifdef SOCK_CLOEXEC
    socktype |= SOCK_CLOEXEC;
#endif
    /* NB: CASocketFd_t is for (windows) portability */
    CASocketFd_t fd = socket(family, socktype, IPPROTO_UDP);
    /* if (POSIX_SOCKET_ERROR == fd) */
    if (OC_INVALID_SOCKET == fd)
    {
        OIC_LOG_V(ERROR, TAG, "create socket failed: %s", CAIPS_GET_ERROR);
        return OC_INVALID_SOCKET;
	/* FIXME: retry here rather than in calling routine? */
    }

#if !defined(SOCK_CLOEXEC) && defined(FD_CLOEXEC)
    int fl = fcntl(fd, F_GETFD);
    if (-1 == fl || -1 == fcntl(fd, F_SETFD, fl|FD_CLOEXEC))
    {
        OIC_LOG_V(ERROR, TAG, "set FD_CLOEXEC failed: %s", strerror(errno));
        close(fd);
        return OC_INVALID_SOCKET;
    }
#endif
    struct sockaddr_storage sa = { .ss_family = (short)family };
    socklen_t socklen = 0;

    if (family == AF_INET6)
    {
        int on = 1;

        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, OPTVAL_T(&on), sizeof (on)))
        {
            OIC_LOG_V(ERROR, TAG, "IPV6_V6ONLY failed: %s", CAIPS_GET_ERROR);
        }

#if defined(IPV6_RECVPKTINFO)
        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof (on)))
#else
        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IPV6, IPV6_PKTINFO, OPTVAL_T(&on), sizeof (on)))
#endif
        {
            OIC_LOG_V(ERROR, TAG, "IPV6_RECVPKTINFO failed: %s",CAIPS_GET_ERROR);
        }

        ((struct sockaddr_in6 *)&sa)->sin6_port = htons(*port);
        socklen = sizeof (struct sockaddr_in6);
    }
    else
    {
        int on = 1;
        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IP, IP_PKTINFO, OPTVAL_T(&on), sizeof (on)))
        {
            OIC_LOG_V(ERROR, TAG, "IP_PKTINFO failed: %s", CAIPS_GET_ERROR);
        }

        ((struct sockaddr_in *)&sa)->sin_port = htons(*port);
        socklen = sizeof (struct sockaddr_in);
    }

    if (isMulticast && *port) // use the given port
    {
        int on = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, OPTVAL_T(&on), sizeof (on)))
        {
            OIC_LOG_V(ERROR, TAG, "SO_REUSEADDR failed: %s", CAIPS_GET_ERROR);
            OC_CLOSE_SOCKET(fd);
            return OC_INVALID_SOCKET;
        }
    }

    if (OC_SOCKET_ERROR == bind(fd, (struct sockaddr *)&sa, socklen))
    {
	OIC_LOG_V(ERROR, TAG, "bind socket failed, port %d: %s", *port, CAIPS_GET_ERROR);
        OC_CLOSE_SOCKET(fd);
        return OC_INVALID_SOCKET;
    }

    if (!*port) // return the assigned port
    {
        if (OC_SOCKET_ERROR == getsockname(fd, (struct sockaddr *)&sa, &socklen))
        {
            OIC_LOG_V(ERROR, TAG, "getsockname failed: %s", CAIPS_GET_ERROR);
            OC_CLOSE_SOCKET(fd);
            return OC_INVALID_SOCKET;
        }
        *port = ntohs(family == AF_INET6 ?
                      ((struct sockaddr_in6 *)&sa)->sin6_port :
                      ((struct sockaddr_in *)&sa)->sin_port);
    }

    return fd;
}

// @rewrite: use <transport>_* instead of caglobals
#if INTERFACE
#define TCP_CHECKFD(FD) \
do \
{ \
 if (FD > tcp_maxfd) \
    { \
        tcp_maxfd = FD; \
    } \
} while (0)
#endif

/* #define CA_FD_SET(TYPE, FDS) \ */
/*     if (caglobals.tcp.TYPE.fd != OC_INVALID_SOCKET) \ */
/*     { \ */
/*         FD_SET(caglobals.tcp.TYPE.fd, FDS); \ */
/*     } */

/* FIXME: eliminate this heinous macro! */
#define NEWSOCKET(FAMILY, NAME, ROUTING) \
do \
{ \
    NAME.fd = OC_INVALID_SOCKET; \
    NAME.fd = udp_create_socket(FAMILY, &NAME.port, ROUTING); \
    if (NAME.fd == OC_INVALID_SOCKET)	   \
    {   \
        NAME.port = 0; \
        NAME.fd = udp_create_socket(FAMILY, &NAME.port, ROUTING);	\
    }   \
    UDP_CHECKFD(NAME.fd); \
 } while(0);

#if INTERFACE
/* LINUX: */
#include <sys/ioctl.h>
#include <net/if.h>
#define IFF_UP_RUNNING_FLAGS  (IFF_UP|IFF_RUNNING)

#endif	/* INTERFACE */

#if EXPORT_INTERFACE
#define IPv4_MULTICAST     "224.0.1.187"
#define IPv6_DOMAINS       16
#endif

struct in_addr IPv4MulticastAddress = { 0 };

// Can we statically initialize these addresses?
#define IPv6_MULTICAST_INT "ff01::158"
static struct in6_addr IPv6MulticastAddressInt;
#define IPv6_MULTICAST_LNK "ff02::158"
static struct in6_addr IPv6MulticastAddressLnk;
#define IPv6_MULTICAST_RLM "ff03::158"
static struct in6_addr IPv6MulticastAddressRlm;
#define IPv6_MULTICAST_ADM "ff04::158"
static struct in6_addr IPv6MulticastAddressAdm;
#define IPv6_MULTICAST_SIT "ff05::158"
static struct in6_addr IPv6MulticastAddressSit;
#define IPv6_MULTICAST_ORG "ff08::158"
static struct in6_addr IPv6MulticastAddressOrg;
#define IPv6_MULTICAST_GLB "ff0e::158"
static struct in6_addr IPv6MulticastAddressGlb;

char *ipv6mcnames[IPv6_DOMAINS] = {
    NULL,
    IPv6_MULTICAST_INT,
    IPv6_MULTICAST_LNK,
    IPv6_MULTICAST_RLM,
    IPv6_MULTICAST_ADM,
    IPv6_MULTICAST_SIT,
    NULL,
    NULL,
    IPv6_MULTICAST_ORG,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    IPv6_MULTICAST_GLB,
    NULL
};

CAResult_t udp_config_data_sockets()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    CAResult_t res = CA_STATUS_OK;
    if (!IPv4MulticastAddress.s_addr)
    {
        (void)inet_pton(AF_INET, IPv4_MULTICAST, &IPv4MulticastAddress);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_INT, &IPv6MulticastAddressInt);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_LNK, &IPv6MulticastAddressLnk);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_RLM, &IPv6MulticastAddressRlm);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_ADM, &IPv6MulticastAddressAdm);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_SIT, &IPv6MulticastAddressSit);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_ORG, &IPv6MulticastAddressOrg);
        (void)inet_pton(AF_INET6, IPv6_MULTICAST_GLB, &IPv6MulticastAddressGlb);
    }

    //    if (udp_ipv6_is_enabled) {
#ifdef ENABLE_IPV6
    OIC_LOG_V(DEBUG, TAG, "Enabling IPv6 UDP data sockets");
    NEWSOCKET(AF_INET6, udp_u6, false);
    NEWSOCKET(AF_INET6, udp_u6s, false);
    NEWSOCKET(AF_INET6, udp_m6, true);
    NEWSOCKET(AF_INET6, udp_m6s, true);
    OIC_LOG_V(INFO, TAG, "IPv6 unicast port: %u", udp_u6.port);
#else
#pragma message ("WARNING: IPv6 SUPPORT IS DISABLED")
#endif
	//    }

	//    if (udp_ipv4_is_enabled) {
#ifdef ENABLE_IPV4
    OIC_LOG_V(DEBUG, TAG, "Enabling IPv4 UDP data sockets");
    udp_ipv4_is_enabled = true;  // only needed to run CA tests
    NEWSOCKET(AF_INET, udp_u4, false);
    NEWSOCKET(AF_INET, udp_u4s, false);
    NEWSOCKET(AF_INET, udp_m4, true);
    NEWSOCKET(AF_INET, udp_m4s, true);
    OIC_LOG_V(INFO, TAG, "IPv4 unicast port: %u", udp_u4.port);
#else
#pragma message ("WARNING: IPv4 SUPPORT IS DISABLED")
#endif
	//    }

    OIC_LOG_V(DEBUG, TAG,
              "socket summary: u6=%d, u6s=%d, u4=%d, u4s=%d, m6=%d, m6s=%d, m4=%d, m4s=%d",
              udp_u6.fd, udp_u6s.fd, udp_u4.fd, udp_u4s.fd,
              udp_m6.fd, udp_m6s.fd, udp_m4.fd, udp_m4s.fd);

    OIC_LOG_V(DEBUG, TAG,
              "port summary: u6 port=%d, u6s port=%d, u4 port=%d, u4s port=%d, m6 port=%d,"
              "m6s port=%d, m4 port=%d, m4s port=%d",
              udp_u6.port, udp_u6s.port, udp_u4.port,
              udp_u4s.port, udp_m6.port, udp_m6s.port,
              udp_m4.port, udp_m4s.port);

 /* FIXME: windows: */
#if defined (SIO_GET_EXTENSION_FUNCTION_POINTER)
    udp_wsaRecvMsg = NULL;
    GUID GuidWSARecvMsg = WSAID_WSARECVMSG;
    DWORD copied = 0;
    int err = WSAIoctl(udp_u4.fd, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidWSARecvMsg, sizeof(GuidWSARecvMsg), &(udp_wsaRecvMsg), sizeof(udp_wsaRecvMsg), &copied, 0, 0);
    if (0 != err)
    {
        OIC_LOG_V(ERROR, TAG, "WSAIoctl failed %i", WSAGetLastError());
        return CA_STATUS_FAILED;
    }
#endif

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return res;
}

// @rewrite udp_cleanup() @was CACloseFDs();
void udp_cleanup()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

/* #if !defined(WSA_WAIT_EVENT_0)
 *     if (udp_shutdownFds[0] != -1)
 *     {
 *         close(udp_shutdownFds[0]);
 *         udp_shutdownFds[0] = -1;
 *     }
 * #endif */
    // @rewrite CADeInitializeIPGlobals();
    udp_close_data_sockets();
    CADeInitializeMonitorGlobals();
}

#if INTERFACE
#define CLOSE_SOCKET(TYPE) \
    if (TYPE.fd != OC_INVALID_SOCKET) \
    { \
        OC_CLOSE_SOCKET(TYPE.fd); \
        TYPE.fd = OC_INVALID_SOCKET; \
    }
#endif

// LOCAL void CADeInitializeIPGlobals()
LOCAL void udp_close_data_sockets()
{
    CLOSE_SOCKET(udp_u6);
    CLOSE_SOCKET(udp_u6s);
    CLOSE_SOCKET(udp_u4);
    CLOSE_SOCKET(udp_u4s);
    CLOSE_SOCKET(udp_m6);
    CLOSE_SOCKET(udp_m6s);
    CLOSE_SOCKET(udp_m4);
    CLOSE_SOCKET(udp_m4s);
    /* CAUnregisterForAddressChanges(); */
}

void applyMulticastToInterface4(uint32_t ifindex)
{
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG_V(DEBUG, TAG, "Adding IF %d to IPv4 multicast group", ifindex);
#endif
    if (!udp_ipv4_is_enabled)
    {
        return;
    }

#if defined(USE_IP_MREQN)
    struct ip_mreqn mreq = { .imr_multiaddr = IPv4MulticastAddress,
                             .imr_address.s_addr = htonl(INADDR_ANY),
                             .imr_ifindex = ifindex };
#else
    struct ip_mreq mreq  = { .imr_multiaddr.s_addr = IPv4MulticastAddress.s_addr,
                             .imr_interface.s_addr = htonl(ifindex) };
#endif

    int ret;
    ret = setsockopt(udp_m4.fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, OPTVAL_T(&mreq), sizeof (mreq));
    if (OC_SOCKET_ERROR == ret)
    {
	if (PORTABLE_check_setsockopt_err())
        {
            OIC_LOG_V(ERROR, TAG, "       IPv4 IP_ADD_MEMBERSHIP failed: %s", CAIPS_GET_ERROR);
        }
    }
    ret = setsockopt(udp_m4s.fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, OPTVAL_T(&mreq), sizeof (mreq));
    if (OC_SOCKET_ERROR == ret)
    {
 	if ( PORTABLE_check_setsockopt_m4s_err(&mreq, ret) )
        {
            OIC_LOG_V(ERROR, TAG, "SECURE IPv4 IP_ADD_MEMBERSHIP failed: %s", CAIPS_GET_ERROR);
        }
    }
}

LOCAL void applyMulticast6(CASocketFd_t fd, struct in6_addr *addr, uint32_t ifindex)
{
    struct ipv6_mreq mreq = { .ipv6mr_interface = ifindex };

    // VS2013 has problems with struct copies inside struct initializers, so copy separately.
    mreq.ipv6mr_multiaddr = *addr;

    int ret = setsockopt(fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, OPTVAL_T(&mreq), sizeof (mreq));
    if (OC_SOCKET_ERROR == ret)
    {
	if (PORTABLE_check_setsockopt_m6_err(fd, &mreq, ret))
        {
            OIC_LOG_V(ERROR, TAG, "IPv6 IPV6_JOIN_GROUP failed: %s", CAIPS_GET_ERROR);
        }
    }
}

void applyMulticastToInterface6(uint32_t ifindex)
{
#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
    OIC_LOG_V(DEBUG, TAG, "Adding IF %d to IPv6 multicast group", ifindex);
#endif
    if (!udp_ipv6_is_enabled)
    {
        return;
    }
    //applyMulticast6(udp_m6.fd, &IPv6MulticastAddressInt, ifindex);
    applyMulticast6(udp_m6.fd, &IPv6MulticastAddressLnk, ifindex);
    applyMulticast6(udp_m6.fd, &IPv6MulticastAddressRlm, ifindex);
    //applyMulticast6(udp_m6.fd, &IPv6MulticastAddressAdm, ifindex);
    applyMulticast6(udp_m6.fd, &IPv6MulticastAddressSit, ifindex);
    //applyMulticast6(udp_m6.fd, &IPv6MulticastAddressOrg, ifindex);
    //applyMulticast6(udp_m6.fd, &IPv6MulticastAddressGlb, ifindex);

    //applyMulticast6(udp_m6s.fd, &IPv6MulticastAddressInt, ifindex);
    applyMulticast6(udp_m6s.fd, &IPv6MulticastAddressLnk, ifindex);
    applyMulticast6(udp_m6s.fd, &IPv6MulticastAddressRlm, ifindex);
    //applyMulticast6(udp_m6s.fd, &IPv6MulticastAddressAdm, ifindex);
    applyMulticast6(udp_m6s.fd, &IPv6MulticastAddressSit, ifindex);
    //applyMulticast6(udp_m6s.fd, &IPv6MulticastAddressOrg, ifindex);
    //applyMulticast6(udp_m6s.fd, &IPv6MulticastAddressGlb, ifindex);
}

// @rewrite udp_add_ifs_to_multicast_groups @was CAIPStartListenServer
CAResult_t udp_add_ifs_to_multicast_groups()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    if (udp_is_started)
    {
        OIC_LOG(DEBUG, TAG, "Adapter is started already, exiting");
        return CA_STATUS_OK;
    }

    // GAR: get all if/addresses; list is on heap, it is NOT g_network_interfaces
    errno = 0;
    u_arraylist_t *iflist = udp_get_ifs_for_rtm_newaddr(0);
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "udp_get_ifs_for_rtm_newaddr() failed: %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

    size_t len = u_arraylist_length(iflist);
    OIC_LOG_V(DEBUG, TAG, "IP network interfaces found: %" PRIuPTR, len);

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
        if (ifitem->family == AF_INET)
        {
            applyMulticastToInterface4(ifitem->index);
        }
        if (ifitem->family == AF_INET6)
        {
            applyMulticastToInterface6(ifitem->index);
        }
    }

    u_arraylist_destroy(iflist);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return CA_STATUS_OK;
}

void udp_add_if_to_multicast_groups(CAInterface_t *ifitem) // @was CAProcessNewInterface
{
    if (!ifitem)
    {
        OIC_LOG(DEBUG, TAG, "ifitem is null");
        return;
    }

    if (ifitem->family == AF_INET6)
    {
        applyMulticastToInterface6(ifitem->index);
    }
    if (ifitem->family == AF_INET)
    {
        applyMulticastToInterface4(ifitem->index);
    }
}

// @rewrite @was CAIPStopListenServer
CAResult_t udp_close_sockets()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    u_arraylist_t *iflist = udp_get_ifs_for_rtm_newaddr(0);
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "Get interface info failed: %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

    size_t len = u_arraylist_length(iflist);
    OIC_LOG_V(DEBUG, TAG, "IP network interfaces found: %" PRIuPTR, len);

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
        if (ifitem->family == AF_INET)
        {
            CLOSE_SOCKET(udp_m4);
            CLOSE_SOCKET(udp_m4s);
            OIC_LOG_V(DEBUG, TAG, "IPv4 network interface: %s cloed", ifitem->name);
        }
        if (ifitem->family == AF_INET6)
        {
            CLOSE_SOCKET(udp_m6);
            CLOSE_SOCKET(udp_m6s);
            OIC_LOG_V(DEBUG, TAG, "IPv6 network interface: %s", ifitem->name);
        }
    }
    u_arraylist_destroy(iflist);
    return CA_STATUS_OK;
}

