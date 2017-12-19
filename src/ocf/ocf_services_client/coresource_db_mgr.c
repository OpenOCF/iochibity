/* coresource_db_mgr.c */

#include "coresource_db_mgr.h"
#include <string.h>
#include <stdio.h>

int msg_count = 0;

char **msg_urls;

static u_linklist_t *g_responses = NULL;

static oc_mutex g_responses_mutex = NULL;

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

    g_responses = u_linklist_create();

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return OC_STACK_OK;
}

OCStackResult oocf_cosp_mgr_terminate()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    /* assert(g_CoServiceProviders == NULL); */

    if (g_responses_mutex != NULL) {
	oc_mutex_free(g_responses_mutex);
	g_responses_mutex = NULL;
    }

    u_linklist_free(&g_responses);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return OC_STACK_OK;
}

/**
 * save response. if new response is a dup (same device ID and URI)
 * then replace (and free) the previous one.
 */
void cosp_mgr_register_coresource(OCClientResponse *data)
EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY %p", __func__, data);
    oc_mutex_lock(g_responses_mutex);

    /* LIFO */
    u_linklist_add_head(g_responses, (void*) data);

    /* FIXME: should we remove duplicates? */
 exit:
    oc_mutex_unlock(g_responses_mutex);
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

void cosp_mgr_unregister_coresource(OCClientResponse *data)
EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    oc_mutex_lock(g_responses_mutex);

    u_linklist_iterator_t *iterTable = NULL;
    u_linklist_init_iterator(g_responses, &iterTable);

    /* FIXME: free msg, not just list node! */
    u_linklist_remove(g_responses, iterTable);

    // find response in list, then remove it
    /* struct OOCF_ResponseMsg *curr_p, *prev_p;
     * prev_p = NULL;
     * for (curr_p = g_responses;
     * 	 curr_p != NULL;
     * 	 prev_p = curr_p, curr_p = curr_p->next) {
     * 	if (curr_p->data == data) {
     * 	    OIC_LOG_V(DEBUG, TAG, "FOUND OOCF_ResponseMsg node; removing");
     * 	    if (prev_p == NULL) { /\* item 0 *\/
     * 		OIC_LOG_V(DEBUG, TAG, "removing root node");
     * 		g_responses = curr_p->next;
     * 	    } else {
     * 		OIC_LOG_V(DEBUG, TAG, "removing non-root node");
     * 		prev_p->next = curr_p->next;
     * 	    }
     * 	    OCPayloadDestroy(curr_p->data->payload);
     * 	    OICFree(curr_p->data->resourceUri);
     * 	    OICFree(curr_p->data);
     * 	    OICFree(curr_p);
     * 	    break;
     * 	} /\* else loop *\/
     * } */
    oc_mutex_unlock(g_responses_mutex);
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

OCClientResponse *oocf_coresource_db_mgr_get_message(int index)
EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY, index %d", __func__, index);
    oc_mutex_lock(g_responses_mutex);

    u_linklist_iterator_t *iterTable = NULL;
    u_linklist_init_iterator(g_responses, &iterTable);

    int i = 0;
    while (NULL != iterTable)
    {
        OCClientResponse *msg = (OCClientResponse*)u_linklist_get_data(iterTable);
	OIC_LOG_V(INFO, TAG, "MSG ptr: %p", msg);
	if (i == index) {
	    oc_mutex_unlock(g_responses_mutex);
	    return msg;
	}
        u_linklist_get_next(&iterTable);
	i++;
    }
    oc_mutex_unlock(g_responses_mutex);
    OIC_LOG_V(ERROR, TAG, "Msg %d not found", index);

    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
    return NULL;
}

int oocf_ocf_version(OCClientResponse *msg)
EXPORT
{
    uint8_t vOptionData[MAX_HEADER_OPTION_DATA_LENGTH];
    size_t vOptionDataSize = sizeof(vOptionData);
    uint16_t actualDataSize = 0;
    OCGetHeaderOption(msg->rcvdVendorSpecificHeaderOptions,
		      msg->numRcvdVendorSpecificHeaderOptions,
		      OCF_CONTENT_FORMAT_VERSION,
		      vOptionData,
		      vOptionDataSize,
		      &actualDataSize);
    if (actualDataSize) {
	/* content_format_version */
	return (vOptionData[0] * 0x0100) + vOptionData[1];
    } else {
	/* missing OCF_CONTENT_FORMAT_VERSION header means pre-ocf 1.0.0? */
	return -1;
    }
}

OCResourcePayload *oocf_coresource_db_mgr_get_coresource(int index, /* [out] */ int *ocf_version)
EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY, index %d", __func__, index);
    oc_mutex_lock(g_responses_mutex);

    OCClientResponse *msg = NULL;
    OCResourcePayload*  coresource_pl;
    OCDiscoveryPayload* discovery_pl;

    u_linklist_iterator_t *iter = NULL;
    u_linklist_init_iterator(g_responses, &iter);

    int i = 0;
    while (NULL != iter)
    {
        msg = (OCClientResponse*)u_linklist_get_data(iter);
	if (msg->payload) {
	    discovery_pl = (OCDiscoveryPayload*) msg->payload;
	    coresource_pl = (OCResourcePayload*) discovery_pl->resources;
	    while(coresource_pl) {
		OIC_LOG_V(INFO, TAG, "testing coresource %d", i);
		if (i == index) {
		    *ocf_version = oocf_ocf_version(msg);
		    oc_mutex_unlock(g_responses_mutex);
		    OIC_LOG_V(INFO, TAG, "matched coresource %d, %p", i, coresource_pl);
		    return coresource_pl;
		}
		coresource_pl = coresource_pl->next;
		i++;
	    }
	    /* u_linklist_get_next(&iter); */
	} else {
	    OIC_LOG_V(DEBUG, TAG, "NO PAYLOAD");
	}
        u_linklist_get_next(&iter);
    }
    OIC_LOG_V(ERROR, TAG, "Resource index %d not found", index);
    oc_mutex_unlock(g_responses_mutex);

    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
    return NULL;
}

void oocf_coresource_mgr_reset(void)
EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);

    if (!g_responses) {
	OIC_LOG_V(INFO, TAG, "g_resources is already NULL", __func__);
	goto exit;
    }

    oc_mutex_lock(g_responses_mutex);

    u_linklist_iterator_t *iterTable = NULL;
    u_linklist_init_iterator(g_responses, &iterTable);

    while (NULL != iterTable)
    {
        OCClientResponse *msg = (OCClientResponse*)u_linklist_get_data(iterTable);
	/* OIC_LOG_V(INFO, TAG, "Purging: %p", msg); */
      	OCPayloadDestroy(msg->payload);
      	OICFree(msg->resourceUri);
	/* invalidate any pointers to the msg: */
	memset(msg, '\0', sizeof(OCClientResponse));
      	OICFree(msg);
        u_linklist_get_next(&iterTable);
    }
    u_linklist_free(&g_responses);

    /* // find response in list, then remove it
     * struct OOCF_ResponseMsg *curr_p, *next_p;
     * next_p = NULL;
     * /\* for (curr_p = g_responses;
     *  * 	 curr_p != NULL;
     *  * 	 prev_p = curr_p, curr_p = curr_p->next) { *\/
     * next_p = curr_p = g_responses;
     * while(next_p) {
     * 	OIC_LOG_V(INFO, TAG, "Freeing msg %p", curr_p->data);
     * 	curr_p = next_p;
     * 	next_p = curr_p->next;
     * 	OCPayloadDestroy(curr_p->data->payload);
     * 	OICFree(curr_p->data->resourceUri);
     * 	OICFree(curr_p->data);
     * 	OICFree(curr_p);
     * } */
    oc_mutex_unlock(g_responses_mutex);
 exit:
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

OCClientResponse *oocf_coresource_db_msgs(void)
EXPORT
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    return g_responses_mutex;
}

int oocf_coresource_db_msg_labels(/* out */ char ***label_list)
EXPORT
{
    OCClientResponse *msg = NULL;

    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    int count = u_linklist_length(g_responses);

    OIC_LOG_V(INFO, TAG, "Msg count: %d", count);

    char **label_ptrs = (char**) calloc(count, sizeof(char*));
    if (!label_ptrs) {
	OIC_LOG(ERROR, TAG, "OICCalloc failure: label_ptrs");
	return 0;
    }
    char **lbl_ptrs = label_ptrs;

    char *labels = (char *) calloc(count, 80);
    if (!labels) {
	OIC_LOG(ERROR, TAG, "OICCalloc failure: labels");
	return 0;
    }
    char *labelp = labels;

    u_linklist_iterator_t *iter = NULL;
    u_linklist_init_iterator(g_responses, &iter);

    int i = 0;
    while (NULL != iter) {
	msg = (OCClientResponse*)u_linklist_get_data(iter);
	if (msg != NULL) {
	    sprintf(labelp, "%s:%d%s", msg->devAddr.addr, msg->devAddr.port, msg->resourceUri);
	    /* OIC_LOG_V(DEBUG, TAG, "LABEL: %s (%p)", labelp, labelp); */
	    *label_ptrs = labelp;
	    /* OIC_LOG_V(DEBUG, TAG, "LABEL PTR: %p, %p", label_ptrs, *label_ptrs); */
	    label_ptrs++;
	    labelp += 81; // strlen(labelp) + 1;
	    /* coresource_pl = coresource_pl->next; */
	}
	u_linklist_get_next(&iter);
    }
    *label_list = lbl_ptrs;
    return count;
}

int oocf_coresource_db_count(void)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    oc_mutex_lock(g_responses_mutex);
    int count = u_linklist_length(g_responses_mutex);
    oc_mutex_unlock(g_responses_mutex);
    return count;
}

/* caller is reponsible for freeing the result */
int oocf_cosp_mgr_list_coresource_uris(/* out */ const char ***uri_list)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OCClientResponse *msg = NULL;
    OCResourcePayload*  coresource_pl;
    OCDiscoveryPayload* discovery_pl;

    OIC_LOG_V(DEBUG, TAG, "MSG count: %d", u_linklist_length(g_responses));

    oc_mutex_lock(g_responses_mutex);

    int cosp_count = 0;

    u_linklist_iterator_t *iter = NULL;
    u_linklist_init_iterator(g_responses, &iter);

    while (NULL != iter) {
	msg = (OCClientResponse*)u_linklist_get_data(iter);
	if (msg != NULL) {
	    if (msg->payload) {
		discovery_pl = (OCDiscoveryPayload*)msg->payload;
		coresource_pl = discovery_pl->resources;
		/* cosp_count++; /\* 1 for anchor *\/ */
		while(coresource_pl) {
		    cosp_count++;
		    coresource_pl = coresource_pl->next;
		}
		u_linklist_get_next(&iter);
	    } else {
		OIC_LOG_V(DEBUG, TAG, "NO PAYLOAD");
	    }
	} else {
	    OIC_LOG_V(DEBUG, TAG, "NO MSG");
	}
    }

    OIC_LOG_V(DEBUG, TAG, "Resource count: %d", cosp_count);

    char **uri_ptrs = (char**) calloc(cosp_count, sizeof(char*));
    if (!uri_ptrs) {
	OIC_LOG(ERROR, TAG, "OICCalloc failure: uri_ptrs");
	return 0;
    }
    char **uptrs = uri_ptrs;

    char *uris = (char *) calloc(cosp_count, 80);
    if (!uris) {
	OIC_LOG(ERROR, TAG, "OICCalloc failure: uris");
	return 0;
    }

    char *urip = uris;

    u_linklist_init_iterator(g_responses, &iter);

    int i = 0;
    while (NULL != iter) {
	msg = (OCClientResponse*)u_linklist_get_data(iter);
	if (msg != NULL) {
	    if (msg->payload) {
		discovery_pl = (OCDiscoveryPayload*)msg->payload;
		coresource_pl = discovery_pl->resources;
		/* urip = coresource_pl->anchor;
		 * urip += strlen(coresource_pl->anchor); */
		while(coresource_pl) {
		    sprintf(urip, "%s%s", coresource_pl->anchor, coresource_pl->uri);
		    /* OIC_LOG_V(DEBUG, TAG, "URI: %s (%d)", urip, strlen(urip)); */
		    *uri_ptrs++ = urip;
		    urip += strlen(urip) + 1;
		    coresource_pl = coresource_pl->next;
		}
		u_linklist_get_next(&iter);
	    } else {
		OIC_LOG_V(DEBUG, TAG, "NO PAYLOAD");
	    }
	} else {
	    OIC_LOG_V(DEBUG, TAG, "NO MSG");
	}
    }
    *uri_list = uptrs;
    oc_mutex_unlock(g_responses_mutex);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return cosp_count;
}
