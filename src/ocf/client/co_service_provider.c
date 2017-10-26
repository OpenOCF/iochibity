/* co_service_provider.c */

#include "co_service_provider.h"

struct CoServiceProvider_s {
    OCClientResponse* discovery_response;
    OCDiscoveryPayload* device;	/* this struct is a linked list, so we need a ptr to it */
    OCResourcePayload* co_service_provider;
};

