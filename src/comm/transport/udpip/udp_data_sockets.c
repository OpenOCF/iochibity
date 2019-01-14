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
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <sys/types.h>
/* #ifdef HAVE_IFADDRS_H */
#include <ifaddrs.h>
/* #endif */



/* #ifdef HAVE_NETINET_IP_H */
/* #include <netinet/ip.h> */
/* #endif */

#include <inttypes.h>
#endif

#include <errno.h>

/* list of nifs with IFF_RUNNING, IFF_UP, IFF_MULTICAST */
uint8_t  _oocf_nif_count;
uint32_t _oocf_nifs[32] = { 0 };
uint32_t _oocf_nif_flags[32] = { 0 };


// from CAGlobals_t in _globals.h
/* unicast sockets use system-assigned ports */
CASocket_t udp_u6  = { .fd = OC_INVALID_SOCKET, .port = 0 };   /**< unicast   IPv6 */
CASocket_t udp_u6s = { .fd = OC_INVALID_SOCKET, .port = 0 }; /**< unicast   IPv6 secure */
CASocket_t udp_u4  = { .fd = OC_INVALID_SOCKET, .port = 0 };   /**< unicast   IPv4 */
CASocket_t udp_u4s = { .fd = OC_INVALID_SOCKET, .port = 0 };  /**< unicast   IPv4 secure */
/* multicast listen sockets use OCF-defined ports */
CASocket_t udp_m6  = { .fd = OC_INVALID_SOCKET, .port = CA_COAP };        /**< multicast IPv6 */
CASocket_t udp_m6s = { .fd = OC_INVALID_SOCKET, .port = CA_SECURE_COAP }; /**< multicast IPv6 secure */
CASocket_t udp_m4  = { .fd = OC_INVALID_SOCKET, .port = CA_COAP };        /**< multicast IPv4 */
CASocket_t udp_m4s = { .fd = OC_INVALID_SOCKET, .port = CA_SECURE_COAP }; /**< multicast IPv4 secure */

//FIXME: status and shutdown monitoring are platform-specific

void *udp_threadpool;           /**< threadpool between Initialize and Start */
int   udp_selectTimeout = 1;          /**< in seconds */

bool  udp_is_started;               /**< the IP adapter has started */
bool  udp_is_terminating;             /**< the IP adapter needs to stop */
bool  udp_ipv6_is_enabled = true;    /**< IPv6 enabled by OCInit flags */
bool  udp_ipv4_is_enabled = true;    /**< IPv4 enabled by OCInit flags */
bool  udp_is_dualstack   = true;    /**< IPv6 and IPv4 enabled */

int nif_count = 0;
unsigned int nifs[8];

/* DELEGATED: #define UDP_CHECKFD(FD) */

/* bool PORTABLE_check_setsockopt_err(void) EXPORT */
/* { */
/*     return EADDRINUSE != errno; */
/* } */

/* bool PORTABLE_check_setsockopt_m4s_err(struct ip_mreqn *mreq, int ret) EXPORT */
/* { */
/*     /\* args not used in posix, used in windows *\/ */
/*     (void)mreq; */
/*     (void)ret; */
/*     return EADDRINUSE != errno; */
/* } */

/* bool PORTABLE_check_setsockopt_m6_err(CASocketFd_t fd, struct ipv6_mreq *mreq,  int ret) EXPORT */
/* { */
/*     /\* args not used in posix, used in windows *\/ */
/*     (void)fd; */
/*     (void)mreq; */
/*     (void)ret; */
/*     return EADDRINUSE != errno; */
/* } */

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

        /* enable ipv6 ancillary (control) info */
#if defined(IPV6_RECVPKTINFO)
        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof (on)))
#else
        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IPV6, IPV6_PKTINFO, OPTVAL_T(&on), sizeof (on)))
#endif
        {
            OIC_LOG_V(ERROR, TAG, "IPV6_RECVPKTINFO failed: %s",CAIPS_GET_ERROR);
        }

        ((struct sockaddr_in6 *)&sa)->sin6_addr = in6addr_any; /* let's be explicit */
        ((struct sockaddr_in6 *)&sa)->sin6_port = htons(*port);
        socklen = sizeof (struct sockaddr_in6);
    }
    else                        /* family == AF_INET */
    {
        int on = 1;
        /* enable ipv6 ancillary (control) info */
        if (OC_SOCKET_ERROR == setsockopt(fd, IPPROTO_IP, IP_PKTINFO, OPTVAL_T(&on), sizeof (on)))
        {
            OIC_LOG_V(ERROR, TAG, "IP_PKTINFO failed: %s", CAIPS_GET_ERROR);
        }

        ((struct sockaddr_in *)&sa)->sin_addr.s_addr = INADDR_ANY; /* let's be explicit */
        ((struct sockaddr_in *)&sa)->sin_port = htons(*port);
        socklen = sizeof (struct sockaddr_in);
    }

    if (isMulticast && *port)   /* port required for multicast */
    {
        int on = 1;
        /* enable udp port sharing across processes  */
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
    // FIXME: also save addr in the CASocket_t structtat

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
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#endif	/* INTERFACE */

#if EXPORT_INTERFACE
#define IPv4_MULTICAST     "224.0.1.187"
#define IPv6_DOMAINS       16
#endif

struct in_addr IPv4MulticastAddress = { 0 };

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
        int res;
        res = inet_pton(AF_INET, IPv4_MULTICAST, &IPv4MulticastAddress);
        if (res != 1) {
            OIC_LOG_V(DEBUG, TAG, "inet_pton failed for IPv4MulticastAddress", __func__);
        }
        res = inet_pton(AF_INET6, IPv6_MULTICAST_INT, &IPv6MulticastAddressInt);
        res = inet_pton(AF_INET6, IPv6_MULTICAST_LNK, &IPv6MulticastAddressLnk);
        if (res != 1) {
            OIC_LOG_V(DEBUG, TAG, "inet_pton failed for IPv6MulticastAddressLnk", __func__);
        }
        char addr_str[256] = {0}; // debugging
        inet_ntop(AF_INET6,
                  &IPv6MulticastAddressLnk.s6_addr,
                  addr_str, sizeof(addr_str));
        OIC_LOG_V(DEBUG, TAG, "IPv6MulticastAddressLnk: %s", addr_str);

        res = inet_pton(AF_INET6, IPv6_MULTICAST_RLM, &IPv6MulticastAddressRlm);
        res = inet_pton(AF_INET6, IPv6_MULTICAST_ADM, &IPv6MulticastAddressAdm);
        res = inet_pton(AF_INET6, IPv6_MULTICAST_SIT, &IPv6MulticastAddressSit);
        res = inet_pton(AF_INET6, IPv6_MULTICAST_ORG, &IPv6MulticastAddressOrg);
        res = inet_pton(AF_INET6, IPv6_MULTICAST_GLB, &IPv6MulticastAddressGlb);
    }

    //    if (udp_ipv6_is_enabled) {
#ifdef ENABLE_IPV6
    OIC_LOG_V(DEBUG, TAG, "Enabling IPv6 UDP data sockets");
    NEWSOCKET(AF_INET6, udp_u6, false);
    NEWSOCKET(AF_INET6, udp_u6s, false);
    NEWSOCKET(AF_INET6, udp_m6, true);
    NEWSOCKET(AF_INET6, udp_m6s, true);
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
#else
#pragma message ("WARNING: IPv4 SUPPORT IS DISABLED")
#endif
	//    }

    OIC_LOG_V(DEBUG, TAG,
              "socket fd summary: u6=%d, u6s=%d, u4=%d, u4s=%d, m6=%d, m6s=%d, m4=%d, m4s=%d",
              udp_u6.fd, udp_u6s.fd, udp_u4.fd, udp_u4s.fd,
              udp_m6.fd, udp_m6s.fd, udp_m4.fd, udp_m4s.fd);

    OIC_LOG_V(DEBUG, TAG,
              "socket port summary: u6 port=%d, u6s port=%d, u4 port=%d, u4s port=%d, m6 port=%d,"
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
    CAUnregisterForAddressChanges();
}

void applyMulticastToInterface4(uint32_t ifindex) /* add_nif4_to_mcast_group */
{
    OIC_LOG_V (INFO, TAG, "%s ENTRY; nif index: %d", __func__, ifindex);
    if (!udp_ipv4_is_enabled)
    {
        return;
    }

    char addr_str[INET_ADDRSTRLEN + 1] = {0}; // debugging

#if defined(HAVE_MREQN)
    struct ip_mreqn mreq = {0};
    memcpy(&mreq.imr_multiaddr.s_addr, (void*)&IPv4MulticastAddress, sizeof(struct in_addr));
    mreq.imr_address.s_addr = htonl(INADDR_ANY);
    mreq.imr_ifindex = 0;  // htonl(ifindex);
#else
    struct ip_mreq mreq  = { .imr_multiaddr.s_addr = IPv4MulticastAddress.s_addr,
                             .imr_interface.s_addr = htonl(ifindex) };
#endif
    inet_ntop(AF_INET, &mreq.imr_multiaddr.s_addr, addr_str, sizeof(addr_str));

    int ret;
    ret = setsockopt(udp_m4.fd, IPPROTO_IP,
                     IP_ADD_MEMBERSHIP,
                     OPTVAL_T(&mreq), sizeof (mreq));
    if (OC_SOCKET_ERROR == ret)
    {
	if (PORTABLE_check_setsockopt_err())
        {
            OIC_LOG_V(ERROR, TAG, "       IPv4 IP_ADD_MEMBERSHIP failed: %s", strerror(errno));
        }
    } else {
        OIC_LOG_V(DEBUG, TAG, "nif %u added to ipv4 mcast grp %s:%d", ifindex, addr_str, udp_m4.port);

    }
    ret = setsockopt(udp_m4s.fd, IPPROTO_IP,
                     IP_ADD_MEMBERSHIP,
                     OPTVAL_T(&mreq), sizeof (mreq));
    if (OC_SOCKET_ERROR == ret)
    {
 	if ( PORTABLE_check_setsockopt_m4s_err(&mreq, ret) )
        {
            OIC_LOG_V(ERROR, TAG, "SECURE IPv4 IP_ADD_MEMBERSHIP failed: %s", CAIPS_GET_ERROR);
        }
    } else {
        OIC_LOG_V(DEBUG, TAG, "nif %u added to ipv4 secure mcast grp %s:%d", ifindex, addr_str, udp_m4s.port);
    }
}

LOCAL void applyMulticast6(struct in6_addr *addr, uint32_t ifindex)
{
    struct ipv6_mreq mreq = { .ipv6mr_interface = ifindex,
                              .ipv6mr_multiaddr = { .s6_addr = {0} }};

    // VS2013 has problems with struct copies inside struct initializers, so copy separately.
    mreq.ipv6mr_multiaddr = *addr;

    memcpy(&mreq.ipv6mr_multiaddr, addr, sizeof(struct in6_addr));

    /* int res = inet_pton(AF_INET6, IPv6_MULTICAST_LNK, &mreq.ipv6mr_multiaddr); */

    char addr_str[INET6_ADDRSTRLEN + 1] = {0}; // debugging
    inet_ntop(AF_INET6, addr->s6_addr, addr_str, sizeof(addr_str));

    int ret;
    ret = setsockopt(udp_m6.fd, IPPROTO_IPV6,
                     // IPV6_ADD_MEMBERSHIP, /* linux */
                     IPV6_JOIN_GROUP, /* BSD */
                     OPTVAL_T(&mreq), sizeof (mreq));
    if (OC_SOCKET_ERROR == ret)
    {
	if (PORTABLE_check_setsockopt_m6_err(udp_m6.fd, &mreq, ret))
        {
            OIC_LOG_V(ERROR, TAG, "IPV6_JOIN_GROUP failed for [%s]:%d: %s", addr_str, udp_m6.port, CAIPS_GET_ERROR);
        }
    } else {
        OIC_LOG_V(ERROR, TAG, "nif %u joined ipv6 mcast grp [%s]:%u", ifindex, addr_str, udp_m6.port);
    }
    ret = setsockopt(udp_m6s.fd, IPPROTO_IPV6,
                     // IPV6_ADD_MEMBERSHIP,
                     IPV6_JOIN_GROUP,
                     OPTVAL_T(&mreq), sizeof (mreq));
    if (OC_SOCKET_ERROR == ret)
    {
	if (PORTABLE_check_setsockopt_m6_err(udp_m6s.fd, &mreq, ret))
        {
            OIC_LOG_V(ERROR, TAG, "IPV6_JOIN_GROUP failed for [%s]:%d: %s", addr_str, udp_m6s.port, CAIPS_GET_ERROR);
        }
    } else {
        OIC_LOG_V(ERROR, TAG, "nif %u joined secure ipv6 mcast grp [%s]:%u", ifindex, addr_str, udp_m6s.port);
    }
}

void applyMulticastToInterface6(uint32_t ifindex)
{
    OIC_LOG_V (INFO, TAG, "%s ENTRY; nif index %u", __func__, ifindex);
    if (!udp_ipv6_is_enabled)
    {
        return;
    }
    applyMulticast6(&IPv6MulticastAddressLnk, ifindex);
    applyMulticast6(&IPv6MulticastAddressRlm, ifindex);
    applyMulticast6(&IPv6MulticastAddressSit, ifindex);
}

// cache unique nifs, join mcast groups, create local ep list
CAResult_t udp_configure_eps() /* @was CAIPStartListenServer */
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    if (udp_is_started)
    {
        OIC_LOG(DEBUG, TAG, "Adapter is started already, exiting");
        return CA_STATUS_OK;
    }
    struct ifaddrs *ifp = NULL;
    errno = 0;
    int r = 0;

    r = getifaddrs(&ifp);
    if (-1 == r)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to getifaddrs: %s", strerror(errno));
        return CA_STATUS_FAILED;
    }
    struct ifaddrs *ifa = NULL;
    unsigned int ifindex = 0;
    int i = 1;  // debugging
    char addr_str[256] = {0}; // debugging
    in_port_t port;
    uint32_t flowinfo;
    uint32_t scope;

    for (ifa = ifp; ifa; ifa = ifa->ifa_next) {

	if (ifa->ifa_name)
            ifindex = if_nametoindex(ifa->ifa_name);
        else
            ifindex = 0;

#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
        if (ifa->ifa_addr->sa_family == AF_INET) {
	    inet_ntop(ifa->ifa_addr->sa_family,
		      &(((struct sockaddr_in*)ifa->ifa_addr)->sin_addr),
		      addr_str, sizeof(addr_str));
            port = ((struct sockaddr_in*)ifa->ifa_addr)->sin_port;
        }
        if (ifa->ifa_addr->sa_family == AF_INET6) {
	    inet_ntop(ifa->ifa_addr->sa_family,
		      &(((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr),
		      addr_str, sizeof(addr_str));
            port = ((struct sockaddr_in6*)ifa->ifa_addr)->sin6_port;
            flowinfo = ((struct sockaddr_in6*)ifa->ifa_addr)->sin6_flowinfo;
            scope = ((struct sockaddr_in6*)ifa->ifa_addr)->sin6_scope_id;
        }
        OIC_LOG_V(DEBUG, TAG, "Item %d: %s (%d): [%s]:%"PRIu32, i++,
                  ifa->ifa_name, ifindex, addr_str, port);
#endif
        int family = ifa->ifa_addr->sa_family;

        if (ifa->ifa_flags & IFF_DEBUG) {
            //#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "\tSkipping debug NIF %d, family %d", ifindex, family);
            //#endif
	    continue;
	}
        if (ifa->ifa_flags & IFF_LOOPBACK) {
            //#ifdef NETWORK_INTERFACE_CHANGED_LOGGING
	    OIC_LOG_V(DEBUG, TAG, "\tSkipping loopback NIF %d, family %d", ifindex, family);
            //#endif
	    continue;
	}
        if (ifa->ifa_addr->sa_family == AF_INET6) {
            if ( 0 == strlen((char*)((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr.s6_addr) ) {
                OIC_LOG_V(DEBUG, TAG, "\tSkipping NIF %d - no ipv6 address", ifindex);
                continue;
            }
        } else if (ifa->ifa_addr->sa_family == AF_INET) {
            if ( 0 == (((struct sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr) ) {
                OIC_LOG_V(DEBUG, TAG, "\tSkipping NIF %d - no ipv4 address", ifindex);
                continue;
            }
        } else {
	    OIC_LOG_V(DEBUG, TAG, "\tSkipping NIF %d, (not ipv4/ipv6)", ifindex);
            OIC_LOG_V(DEBUG, TAG, "\tFamily: %d", family);
            continue;
        }
        if ((ifa->ifa_flags & IFF_UP_RUNNING_FLAGS) != IFF_UP_RUNNING_FLAGS) {
	    OIC_LOG_V (INFO, TAG, "\tSkipping NIF %d (not up and running)", ifindex);
            continue;
        }

	if (ifa->ifa_name) {
            /* 1. save to nif index array */
            bool dup = false;
            for (int i=0; i<nif_count; i++) {
                if (nifs[i] == ifindex) {
                    dup = true;
                    break;
                }
            }
            if (!dup) nifs[nif_count++] = ifindex;
            /* 2. create endpoint */
            //g_local_endpoint_cache
            CAEndpoint_t *ep  = udp_create_endpoint(ifa, ifindex, false);
            OIC_LOG_V(DEBUG, TAG, "Created ep: %s port %d", ep->addr, ep->port);
            u_arraylist_add(g_local_endpoint_cache, (void *)ep);
            CAEndpoint_t *sep = udp_create_endpoint(ifa, ifindex, true);
            OIC_LOG_V(DEBUG, TAG, "Created secure ep: %s port %d", sep->addr, sep->port);
            u_arraylist_add(g_local_endpoint_cache, (void *)sep);
        } else {
            OIC_LOG_V(DEBUG, TAG, "Skipping, no nif name");
        }
        /* SIOCGIFFLAGS (manpage NETDEVICE(7)) */
        OIC_LOG_V(DEBUG, TAG, "\tflowinfo: %"PRIu32 ", scope id: %"PRIu32, flowinfo, scope);
        OIC_LOG_V(DEBUG, TAG, "\tIFF_UP? %s", ((ifa->ifa_flags & IFF_UP)? "Y":"N"));
        OIC_LOG_V(DEBUG, TAG, "\tIFF_RUNNING? %s", ((ifa->ifa_flags & IFF_RUNNING)? "Y":"N"));
        OIC_LOG_V(DEBUG, TAG, "\tIFF_ALLMULTI? %s", ((ifa->ifa_flags & IFF_ALLMULTI)? "Y":"N"));
        OIC_LOG_V(DEBUG, TAG, "\tIFF_MULTICAST? %s", ((ifa->ifa_flags & IFF_MULTICAST)? "Y":"N"));
        OIC_LOG_V(DEBUG, TAG, "\tIFF_DYNAMIC? %s", ((ifa->ifa_flags & IFF_DYNAMIC)? "Y":"N"));
        OIC_LOG_V(DEBUG, TAG, "\tIFF_NOARP? %s", ((ifa->ifa_flags & IFF_NOARP)? "Y":"N"));
        OIC_LOG_V(DEBUG, TAG, "\tIFF_POINTOPOINT? %s", ((ifa->ifa_flags & IFF_POINTOPOINT)? "Y":"N"));
    }

    OIC_LOG_V(DEBUG, TAG, "NIF count: %d", nif_count);
    OIC_LOG_V(DEBUG, TAG, "g_local_endpoint_cache count: %d", u_arraylist_length(g_local_endpoint_cache));

    for (int i=0; i < nif_count; i++) {
        applyMulticastToInterface4(nifs[i]);
        applyMulticastToInterface6(nifs[i]);
    }

    return CA_STATUS_OK;
}

/* CAResult_t Xudp_add_nifs_to_multicast_groups() */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__); */
/*     if (udp_is_started) */
/*     { */
/*         OIC_LOG(DEBUG, TAG, "Adapter is started already, exiting"); */
/*         return CA_STATUS_OK; */
/*     } */

/*     // GAR: get all if/addresses; list is on heap, it is NOT g_network_interfaces */
/*     errno = 0; */
/*     u_arraylist_t *iflist = udp_get_all_nifs(); */
/*     if (!iflist) */
/*     { */
/*         OIC_LOG_V(ERROR, TAG, "udp_get_all_nifs failed: %s", strerror(errno)); */
/*         return CA_STATUS_FAILED; */
/*     } */

/*     size_t len = u_arraylist_length(iflist); */
/*     OIC_LOG_V(DEBUG, TAG, "IP network interfaces found: %" PRIuPTR, len); */

/*     for (size_t i = 0; i < len; i++) */
/*     { */
/*         CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i); */

/*         if (!ifitem) */
/*         { */
/*             continue; */
/*         } */
/*         if ((ifitem->flags & IFF_UP_RUNNING_FLAGS) != IFF_UP_RUNNING_FLAGS) */
/*         { */
/* 	    OIC_LOG_V (INFO, TAG, "%s IF %d: %s not up and running ", */
/* 		   __func__, ifitem->index, ifitem->name); */
/*             continue; */
/*         } */
/* 	OIC_LOG_V (INFO, TAG, "%s adding IF %d: %s to multicast groups ", */
/* 		   __func__, ifitem->index, ifitem->name); */

/*         if (ifitem->family == AF_INET) */
/*         { */
/*             applyMulticastToInterface4(ifitem->index); */
/*         } */
/*         if (ifitem->family == AF_INET6) */
/*         { */
/*             applyMulticastToInterface6(ifitem->index); */
/*         } */
/*     } */

/*     u_arraylist_destroy(iflist); */

/*     OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__); */
/*     return CA_STATUS_OK; */
/* } */

/* void udp_add_nif_to_multicast_groups(CAInterface_t *ifitem) // @was CAProcessNewInterface */
/* { */
/*     if (!ifitem) */
/*     { */
/*         OIC_LOG(DEBUG, TAG, "ifitem is null"); */
/*         return; */
/*     } */

/*     if (ifitem->family == AF_INET6) */
/*     { */
/*         applyMulticastToInterface6(ifitem->index); */
/*     } */
/*     if (ifitem->family == AF_INET) */
/*     { */
/*         applyMulticastToInterface4(ifitem->index); */
/*     } */
/* } */

// @rewrite @was CAIPStopListenServer
CAResult_t udp_close_sockets()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    /* u_arraylist_t *iflist = udp_get_nifs_for_rtm_newaddr(0); */
    /* if (!iflist) */
    /* { */
    /*     OIC_LOG_V(ERROR, TAG, "Get interface info failed: %s", strerror(errno)); */
    /*     return CA_STATUS_FAILED; */
    /* } */

    /* size_t len = u_arraylist_length(iflist); */
    /* OIC_LOG_V(DEBUG, TAG, "IP network interfaces found: %" PRIuPTR, len); */

    /* for (size_t i = 0; i < len; i++) */
    /* { */
    /*     CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i); */

    /*     if (!ifitem) */
    /*     { */
    /*         continue; */
    /*     } */
    /*     if ((ifitem->flags & IFF_UP_RUNNING_FLAGS) != IFF_UP_RUNNING_FLAGS) */
    /*     { */
    /*         continue; */
    /*     } */
    /*     if (ifitem->family == AF_INET) */
    /*     { */
    //#ifdef ENABLE_IPV4
            CLOSE_SOCKET(udp_m4);
            CLOSE_SOCKET(udp_m4s);
            OIC_LOG_V(DEBUG, TAG, "udp_m4, udp_m4s sockets closed"); // , ifitem->name);
            //#endif
        /* } */
        /* if (ifitem->family == AF_INET6) */
        /* { */
#ifndef DISABLE_IPV6
            CLOSE_SOCKET(udp_m6);
            CLOSE_SOCKET(udp_m6s);
            OIC_LOG_V(DEBUG, TAG, "udp_m6, udp_m6s sockets closed");
#endif
    /*     } */
    /* } */
    /* u_arraylist_destroy(iflist); */
    return CA_STATUS_OK;
}

