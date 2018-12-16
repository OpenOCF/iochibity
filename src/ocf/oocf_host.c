#include "oocf_host.h"

/**
 * Host Mode of Operation.
 */
#if INTERFACE
typedef enum
{
    OC_CLIENT = 0,
    OC_SERVER,
    OC_CLIENT_SERVER,
    OC_GATEWAY          /**< Client server mode along with routing capabilities.*/
} OCMode;
#endif

OCMode myStackMode = 0;
