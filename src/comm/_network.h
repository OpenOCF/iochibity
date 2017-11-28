/**
 * Callback function type for network status changes delivery from CA common logic.
 * @param[out]   info       Endpoint object from which the network status is changed.
 *                          It contains endpoint address based on the connectivity type.
 * @param[out]   status     Current network status info.
 */
typedef void (*CANetworkMonitorCallback)(const CAEndpoint_t *info, CANetworkStatus_t status);
