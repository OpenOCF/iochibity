/* IP Interface stuff */

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
