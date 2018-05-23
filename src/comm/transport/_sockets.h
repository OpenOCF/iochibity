/*
 * Socket types and error definitions.
 */
/* #if EXPORT_INTERFACE */
#ifdef HAVE_WINSOCK2_H
#  define OPTVAL_T(t)    (const char*)(t)
#  define OC_CLOSE_SOCKET(s) closesocket(s)
#else
#  define OPTVAL_T(t)    (t)
#include <unistd.h>
#  define OC_CLOSE_SOCKET(s) close(s)
#endif
/* #endif */

#ifdef HAVE_WINSOCK2_H
#include <Winsock2.h>
#include <windows.h>
#define OC_SOCKET_ERROR      SOCKET_ERROR
#define OC_INVALID_SOCKET    INVALID_SOCKET
/* typedef SOCKET CASocketFd_t; */
#define CASocketFd_t SOCKET

#else // not HAVE_WINSOCK2_H
#define OC_SOCKET_ERROR      (-1)
#define OC_INVALID_SOCKET    (-1)
#define POSIX_SOCKET_ERROR      (-1)
#define POSIX_FCNTL_ERROR      (-1)
#define POSIX_SETSOCKOPT_ERROR      (-1)
#define POSIX_GETSOCKNAME_ERROR      (-1)
/* typedef int    CASocketFd_t; */
#define CASocketFd_t int
#endif

/**
 * Hold global variables for CA layer. (also used by RI layer)
 */
#include <inttypes.h>
typedef struct
{
    CASocketFd_t fd;    /**< socket fd */
    uint16_t port;      /**< socket port */
} CASocket_t;

/**
 * Hold interface index for keeping track of comings and goings.
 */
typedef struct
{
    int32_t ifIndex; /**< network interface index */
} CAIfItem_t;

/**
 * Hold the port number assigned from application.
 * It will be used when creating a socket.
 */
typedef struct
{
    struct udpports
    {
        uint16_t u6;    /**< unicast IPv6 socket port */
        uint16_t u6s;   /**< unicast IPv6 socket secure port */
        uint16_t u4;    /**< unicast IPv4 socket port */
        uint16_t u4s;   /**< unicast IPv4 socket secure port */
    } udp;
#ifdef TCP_ADAPTER
    struct tcpports
    {
        uint16_t u4;    /**< unicast IPv4 socket port */
        uint16_t u4s;   /**< unicast IPv6 socket secure port */
        uint16_t u6;    /**< unicast IPv6 socket port */
        uint16_t u6s;   /**< unicast IPv6 socket secure port */
    } tcp;
#endif
} CAPorts_t;
