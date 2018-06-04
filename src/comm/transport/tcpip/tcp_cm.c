#include "tcp_cm.h"

/* from cautilinterface.c */
/* #if defined(TCP_ADAPTER) && defined(WITH_CLOUD) */
/* #include "caconnectionmanager.h" */
/* #endif */

/* from cautilinterface.c */
#if defined(TCP_ADAPTER) && defined(WITH_CLOUD)
CAResult_t CAUtilCMInitailize()
{
    return CACMInitialize();
}

CAResult_t CAUtilCMTerminate()
{
    return CACMTerminate();
}

CAResult_t CAUtilCMUpdateRemoteDeviceInfo(const CAEndpoint_t *endpoint, bool isCloud)
{
    return CACMUpdateRemoteDeviceInfo(endpoint, isCloud);
}

CAResult_t CAUtilCMResetRemoteDeviceInfo()
{
    return CACMResetRemoteDeviceInfo();
}

CAResult_t CAUtilCMSetConnectionUserConfig(CAConnectUserPref_t connPrefer)
{
    return CACMSetConnUserConfig(connPrefer);
}

CAResult_t CAUtilCMGetConnectionUserConfig(CAConnectUserPref_t *connPrefer)
{
    return CACMGetConnUserConfig(connPrefer);
}
#endif

