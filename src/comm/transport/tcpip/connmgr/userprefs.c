#include "userprefs.h"

#if EXPORT_INTERFACE
#if defined(TCP_ADAPTER) && defined(WITH_CLOUD)
/**
 * User Preference of connectivity channel
 */
typedef enum
{
    OC_USER_PREF_CLOUD     = 0,     /* TCP */
    OC_USER_PREF_LOCAL_UDP = 1,
    OC_USER_PREF_LOCAL_TCP = 2
} OCConnectUserPref_t;
#endif
#endif	/* INTERFACE */
