#include <cdk_test.h>
#include "openocf.h"
#include "coresource_retrieve.h"

static CDKDIALOG *dlg_retrieve  = 0;

static char *msg[MSG_MAX];
static char buffer[8192];

/* static char **msgs = NULL;
 * static char **s = NULL; */
static int msg_count = 0;

static char text[MSG_MAX][80];		/* since message is const we need a work buffer */

/* static char *header[MSG_MAX];
 * static int    header_count; */

static char temp[100];

static void initialize_retrieve_dlg()
{
    const char *buttons[] = {"</B/16>OK",
			     "<C>Cancel",
			     "<C>Help",
    };

    dlg_retrieve = newCDKDialog (cdkscreen,
				   CENTER,
				   CENTER,
				   (CDK_CSTRING2) msg, msg_count,
				   (CDK_CSTRING2) buttons, 3,
				   COLOR_PAIR (2) | A_REVERSE,
				   TRUE,
				   TRUE,
				   FALSE);

    /* Check if we got a null value back. */
    if (dlg_retrieve == 0)
	{
	    /* Shut down Cdk. */
	    destroyCDKScreen (cdkscreen);
	    endCDK ();

	    printf ("Cannot create the dialog box. Is the window too small?\n");
	    ExitProgram (EXIT_FAILURE);
	}
}

int get_retrieve_msg(int index, char *ptrs[], char *buffer, int *count)
{
    int ocf_version;
    OCResourcePayload *resource = oocf_coresource_db_mgr_get_coresource(index, &ocf_version);
    OIC_LOG_V(INFO, TAG, "ocf version: %d", ocf_version);

    int i = 0;

    ptrs[i++] = buffer;
    sprintf(buffer, "<C></R>Retrieve Coresource<!R>");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;


    sprintf(buffer, " GET %s%s", resource->anchor, resource->uri);
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    /* Common Properties for all resources: rt, if, n, id */

    /* OIC_LOG_V(INFO, TAG, "instance: %d", i);
     * sprintf(buffer, "ins: %d", i + 1);
     * ptrs[i++] = buffer;
     * buffer += strlen(buffer) + 1; */

    /* Mandatory props for discoverable resource (resource): href, rt, if */
     /* legacy: "sec" and "port" not used for OCF 1.0, which uses eps instead */
    if ( ocf_version == OCF_VERSION_1_0_0 || ocf_version == OCF_VERSION_1_1_0) {
	sprintf(buffer, " eps:");
	buffer += strlen(buffer);
	/* ptrs[i++] = buffer; */

	OCEndpointPayload *endpoint = resource->eps;
	while(endpoint) {
	    /* cJSON *ep = cJSON_CreateObject(); */
	    /* char port[INT_MAX + 1];
	     * snprintf(port, INT_MAX, "%d", endpoint->port); */
	    int eplen = strlen(endpoint->tps)
		+ 3		/* :// */
		+ strlen(endpoint->addr)
		+ 10				/* "pri: %d" */
		+ 1; 		/* : */
	    char *epstring = malloc(eplen + 6); /* largest val for port is 5 chars (uint16) */
	    snprintf(buffer, eplen + 6, "\t%s://%s:%d pri: %d",
		     endpoint->tps, endpoint->addr, endpoint->port, endpoint->pri);
	    /* cJSON_AddItemToObject(ep, "ep", cJSON_CreateString(epstring));
	     * free(epstring);
	     * /\* cJSON_AddItemToObject(ep, "tps", cJSON_CreateString(endpoint->tps));
	     *  * cJSON_AddItemToObject(ep, "addr", cJSON_CreateString(endpoint->addr));
	     *  * cJSON_AddNumberToObject(ep, "port", endpoint->port); *\/
	     * cJSON_AddNumberToObject(ep, "pri", endpoint->pri);
	     * cJSON_AddItemToArray(eps, ep); */
	    endpoint = endpoint->next;
	    buffer += strlen(buffer) + 1;
	    ptrs[i++] = buffer;
	}

    } else {
        /* cJSON_AddItemToObject(policy, "sec", cJSON_CreateBool(resource->secure)); /\* security *\/
         * cJSON_AddNumberToObject(policy, "port", resource->port); /\* secure port *\/ */
    }



    *count = i - 1;
    return 0;
}

int retrieve_coresource(int index)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY; index: %d", __func__, index);

    memset(buffer, '\0', 8192);
    int rc = get_retrieve_msg(index, msg, buffer, &msg_count);
    initialize_retrieve_dlg();

    /* pthread_mutex_lock(&display_mutex); */
    flockfile(stdout);
    int selection = activateCDKDialog (dlg_retrieve, 0);
    /* pthread_mutex_unlock(&display_mutex); */

    /* Tell them what was selected. */
    if (dlg_retrieve->exitType == vESCAPE_HIT)
	{
	    OIC_LOG_V(DEBUG, TAG, "%s ESC", __func__);
	    /* msg[0] = msg->resourceUri;
	     * msg[1] = "";
	     * msg[2] = "<C>Press any key to continue.";
	     * popupLabel (cdkscreen, (CDK_CSTRING2) msg, 3); */
	}
    else if (dlg_retrieve->exitType == vNORMAL) {
	OIC_LOG_V(DEBUG, TAG, "exitType vNORMAL");
	switch (selection) {
	case 0:		/* RETRIEVE */
	    OIC_LOG_V(DEBUG, TAG, "case 0: OK");
	    /* steps: pick an ep, construct query string, send request */
	    break;
	/* case 1:		/\* UPDATE *\/ */
	/*     OIC_LOG_V(DEBUG, TAG, "case 1: UPDATE"); */
	/*     break; */
	/* case 2:		/\* DELETE *\/ */
	/*     OIC_LOG_V(DEBUG, TAG, "case 2: DELETE"); */
	/*     break; */
	case 1:		/* CANCEL */
	    OIC_LOG_V(DEBUG, TAG, "case 1: CANCEL");
	    break;
	case 2:		/* HELP */
	    OIC_LOG_V(DEBUG, TAG, "case 2: HELP");
	    break;
	}
	/* OIC_LOG_V(DEBUG, TAG, "%s NORMAL", __func__); */
    }
    funlockfile(stdout);

    /* eraseCDKDialog(dlg_retrieve); */
    destroyCDKDialog(dlg_retrieve);
    /* draw_msg_scrollers(); */
    /* Clean up. */
    /* destroyCDKDialog (dlg_retrieve);
     * destroyCDKScreen (cdkscreen); */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}
