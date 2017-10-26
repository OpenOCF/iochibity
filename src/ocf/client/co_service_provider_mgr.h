#ifndef CO_SERVICE_PROVIDER_MGR_H_
#define CO_SERVICE_PROVIDER_MGR_H_

#include "iotivity_config.h"

#include <string.h>

#include "trace.h"
#include "oic_malloc.h"
#include "ocstack.h"
#include "ocresource.h"
#include "ocpresence.h"
#include "octhread.h"
#include "cacommon.h"
#include "co_service_provider.h"

/**
 * Mutex to synchronize the access to g_responses variable.
 */
static oc_mutex g_responses_mutex = NULL;

/* FIXME: typedef this? */
struct OOCF_ResponseItem {
    OCClientResponse * response;
    struct OOCF_ResponseItem *next;
};

struct OOCF_ResponseItem *g_responses;

OCStackResult oocf_cosp_mgr_init();
OCStackResult oocf_cosp_mgr_terminate();
void oocf_cosp_mgr_save_response(OCClientResponse *response);
void oocf_cosp_mgr_free_response(OCClientResponse *response);

char **oocf_cosp_list_resource_uris();

#endif /* CO_SERVICE_PROVIDER_MGR_H_ */
