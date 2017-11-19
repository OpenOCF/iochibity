/* co_service_provider_mgr.c */

#include "co_service_provider_mgr.h"
#include "oic_malloc.h"
#include "logger.h"

static struct OOCF_ResponseItem *g_responses;

OCStackResult oocf_cosp_mgr_init()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    if (NULL == g_responses_mutex)
    {
        g_responses_mutex = oc_mutex_new();
        if (NULL == g_responses_mutex)
        {
            OIC_LOG(ERROR, TAG, "oc_mutex_new has failed for g_responses_mutex");
            return OC_STACK_ERROR;
        }
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

OCStackResult oocf_cosp_mgr_terminate()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    /* assert(g_CoServiceProviders == NULL); */

    if (g_responses_mutex != NULL)
	{
	    oc_mutex_free(g_responses_mutex);
	    g_responses_mutex = NULL;
	}
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

/**
 * save response. if new response is a dup (same device ID and URI)
 * then replace (and free) the previous one.
 */
void oocf_cosp_mgr_save_response(OCClientResponse *response)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    oc_mutex_lock(g_responses_mutex);
    struct OOCF_ResponseItem *new_r = OICMalloc(sizeof(struct OOCF_ResponseItem));
    if (NULL == new_r) OIC_LOG_V(FATAL, TAG, "Malloc fail for struct OOCF_ResponseItem");
    new_r->response = response;
    // find last link in list and add new_r to it
    struct OOCF_ResponseItem *last_r = g_responses;
    int i = 0;
    if (last_r) {
	while (last_r->next) {
	    /* check for dup: matching URI and device ID */
	    if (0 == strncmp(last_r->response->resourceUri,
			     response->resourceUri,
			     strlen(response->resourceUri))) {
		if (0 == strncmp(last_r->response->devAddr.remoteId,
				 response->devAddr.remoteId,
				 MAX_IDENTITY_SIZE)) {
		    OIC_LOG_V(DEBUG, TAG, "Found duplicate ResponseItem at index %d", i);
		    OICFree(new_r);
		    OCPayloadDestroy(last_r->response->payload);
		    OICFree(last_r->response);
		    last_r->response = response;
		    goto exit;
		}
	    }
	    last_r = last_r->next;
	    i++;
	}
	/* last node: check for dup */
	if (0 == strncmp(last_r->response->resourceUri,
			 response->resourceUri,
			 strlen(response->resourceUri))) {
	    if (0 == strncmp(last_r->response->devAddr.remoteId,
			     response->devAddr.remoteId,
			     MAX_IDENTITY_SIZE)) {
		OIC_LOG_V(DEBUG, TAG, "Found duplicate ResponseItem at index %d; replacing", i);
		OICFree(new_r);
		OCPayloadDestroy(last_r->response->payload);
		OICFree(last_r->response);
		last_r->response = response;
		goto exit;
	    }
	}
	OIC_LOG_V(DEBUG, TAG, "Adding new ResponseItem at index %d; replacing", i);
	OIC_LOG_V(DEBUG, TAG, "URI: %s, device ID: %s",
		  response->resourceUri, response->devAddr.remoteId);
	last_r->next = new_r;
	i++;
    } else {
	/* this is the first item */
	OIC_LOG_V(DEBUG, TAG, "Adding new ResponseItem at root node");
	OIC_LOG_V(DEBUG, TAG, "URI: %s, device ID: %s",
		  response->resourceUri, response->devAddr.remoteId);
	g_responses = new_r;
    }
 exit:
    oc_mutex_unlock(g_responses_mutex);
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

void oocf_cosp_mgr_free_response(OCClientResponse *response)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    oc_mutex_lock(g_responses_mutex);
    // find response in list, then remove it
    struct OOCF_ResponseItem *curr_p, *prev_p;
    prev_p = NULL;
    for (curr_p = g_responses;
	 curr_p != NULL;
	 prev_p = curr_p, curr_p = curr_p->next) {
	if (curr_p->response == response) {
	    OIC_LOG_V(DEBUG, TAG, "FOUND OOCF_ResponseItem node; removing");
	    if (prev_p == NULL) { /* item 0 */
		OIC_LOG_V(DEBUG, TAG, "removing root node");
		g_responses = curr_p->next;
	    } else {
		OIC_LOG_V(DEBUG, TAG, "removing non-root node");
		prev_p->next = curr_p->next;
	    }
	    OICFree(curr_p);
	} /* else loop */
    }
    oc_mutex_unlock(g_responses_mutex);
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

char **oocf_cosp_list_resource_uris()
{
    struct OOCF_ResponseItem *item = g_responses;
    OCClientResponse *msg;
    OCResourcePayload* link;
    int i = 0;
    /* get uri count */
    while (item) {
	msg = item->response;
	/* then iterate over the links */
	link = ( (OCDiscoveryPayload*)msg->payload)->resources;
	while (link) { i++; link=link->next; }
	item = item->next;
	/* i++; /\* one for /oic/res *\/ */
    }
    i++; /* for null */

    CoServiceProvider *cosps = OICCalloc(1, i * sizeof(CoServiceProvider*));
    char **uris = OICCalloc(1, i * sizeof(char*));


    item = g_responses;
    i = 0;
    while (item) {
	msg = item->response;
	/* first do the (virtual) /oic/res resource */

	/* then iterate over the links */
	link = ( (OCDiscoveryPayload*)msg->payload)->resources;
	while (link) {
	    // malloc a string, then strcat
	    int sz = strlen(link->anchor) + strlen(link->uri) + 10;
	    uris[i] = OICMalloc(sz);
	    // FIXME check malloc results
	    snprintf(uris[i], sz, "%s%s", link->anchor, link->uri);
	    link = link->next;
	    i++;
	}
	item = item->next;
	i++;
    }
    char **xuris = uris;
    /* OIC_LOG_V(DEBUG, TAG, "xuris: %p", xuris); */

    return uris;
}

