/* #if EXPORT_INTERFACE */
#ifdef HAVE_WINSOCK2_H
#  define OPTVAL_T(t)    (const char*)(t)
#  define OC_CLOSE_SOCKET(s) closesocket(s)
#else
#  define OPTVAL_T(t)    (t)
#  define OC_CLOSE_SOCKET(s) close(s)
#endif
/* #endif */

/* FIXME: split up this global monstrosity: ip-stuff, udp-stuff, tcp-stuff, and other stuff */

/* #if EXPORT_INTERFACE */
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#endif
typedef struct
{
    CATransportFlags_t clientFlags; /**< flag for client */
    CATransportFlags_t serverFlags; /**< flag for server */
    bool client; /**< client mode */
    bool server; /**< server mode */

    CAPorts_t ports;

  /* FIXME: split into platform-independent msg sockets,
     platformm-dependent monitoring data, platform-independent
     status indicators, */
    struct sockets
    {
        void *threadpool;           /**< threadpool between Initialize and Start */
        CASocket_t u6;              /**< unicast   IPv6 */
        CASocket_t u6s;             /**< unicast   IPv6 secure */
        CASocket_t u4;              /**< unicast   IPv4 */
        CASocket_t u4s;             /**< unicast   IPv4 secure */
        CASocket_t m6;              /**< multicast IPv6 */
        CASocket_t m6s;             /**< multicast IPv6 secure */
        CASocket_t m4;              /**< multicast IPv4 */
        CASocket_t m4s;             /**< multicast IPv4 secure */
#if defined(_WIN32)
        WSAEVENT addressChangeEvent;/**< Event used to signal address changes */
        WSAEVENT shutdownEvent;     /**< Event used to signal threads to stop */
#elif defined(__APPLE__) && defined(__MACH__)
	/* FIXME: use SystemConfiguration Framework? netlinkFd does not break but does not work */
        int netlinkFd;              /**< netlink */
        int shutdownFds[2];         /**< pipe used to signal threads to stop */
        CASocketFd_t maxfd;         /**< highest fd (for select) */
#else
	/* GAR: netlink_socket, not int? */
        int netlinkFd;              /**< netlink */
        int shutdownFds[2];         /**< pipe used to signal threads to stop */
        CASocketFd_t maxfd;         /**< highest fd (for select) */
#endif
        int selectTimeout;          /**< in seconds */
        bool started;               /**< the IP adapter has started */
        bool terminate;             /**< the IP adapter needs to stop */
        bool ipv6enabled;           /**< IPv6 enabled by OCInit flags */
        bool ipv4enabled;           /**< IPv4 enabled by OCInit flags */
        bool dualstack;             /**< IPv6 and IPv4 enabled */
#if defined (_WIN32)
        LPFN_WSARECVMSG wsaRecvMsg; /**< Win32 function pointer to WSARecvMsg() */
#endif

        struct networkmonitors
        {
            CAIfItem_t *ifItems; /**< current network interface index list */
            size_t sizeIfItems;  /**< size of network interface index array */
            size_t numIfItems;   /**< number of valid network interfaces */
        } nm;
    } ip;

    struct calayer
    {
        CAHistory_t requestHistory;  /**< filter IP family in requests */
    } ca;

#ifdef TCP_ADAPTER
    /**
     * Hold global variables for TCP Adapter.
     */
    struct tcpsockets
    {
        void *threadpool;       /**< threadpool between Initialize and Start */
        CASocket_t ipv4;        /**< IPv4 accept socket */
        CASocket_t ipv4s;       /**< IPv4 accept socket secure */
        CASocket_t ipv6;        /**< IPv6 accept socket */
        CASocket_t ipv6s;       /**< IPv6 accept socket secure */
        int selectTimeout;      /**< in seconds */
        int listenBacklog;      /**< backlog counts*/
#if defined(_WIN32)
        WSAEVENT updateEvent;   /**< Event used to signal thread to stop or update the FD list */
#else
        int shutdownFds[2];     /**< shutdown pipe */
        int connectionFds[2];   /**< connection pipe */
        CASocketFd_t maxfd;     /**< highest fd (for select) */
#endif
        bool started;           /**< the TCP adapter has started */
        volatile bool terminate;/**< the TCP adapter needs to stop */
        bool ipv4tcpenabled;    /**< IPv4 TCP enabled by OCInit flags */
        bool ipv6tcpenabled;    /**< IPv6 TCP enabled by OCInit flags */
    } tcp;
#endif
/* FIXME: move to comm/transport/ble */
//  CATransportBTFlags_t bleFlags;   /**< flags related BLE transport */
} CAGlobals_t;
/* NB: used for one global var in caconnectivitymanager.c */

/* #endif	/\* INTERFACE *\/ */

/* extern CAGlobals_t caglobals; */
