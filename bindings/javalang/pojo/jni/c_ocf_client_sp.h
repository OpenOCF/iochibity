#ifndef _c_co_service_manager_h
#define _c_co_service_manager_h

#include "openocf.h"

OCClientResponse* g_Platform;
OCClientResponse* g_Device;
OCClientResponse* g_Resources;

/* ServiceProviderHashMap registeredSPs; */

void register_platform(OCClientResponse* c_OCClientResponse);

void register_device(OCClientResponse* c_OCClientResponse);

void register_resources(OCClientResponse* c_OCClientResponse);

#endif
