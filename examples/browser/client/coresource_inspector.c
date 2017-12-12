#include <cdk_test.h>
#include "openocf.h"
#include "coresource_inspector.h"

static CDKDIALOG *dlg_coresource  = 0;

static const char *message[MSG_MAX];

static char **msgs = NULL;
static char **s = NULL;
static int msg_count = 0;

static char text[MSG_MAX][80];		/* since message is const we need a work buffer */

static char temp[100];
static int selection;

static construct_coresource_hdr(OCResourcePayload *coresource)
{
    static int i = 0;
    for (i=0; i<MSG_MAX; i++) {
	memset(text[i], '\0', 80);
    }
    i = 0;
    sprintf(text[i++], "%s", "<C></R>Coresource<!R>");
    sprintf(text[i++], "uri:     %s", coresource->uri);
    sprintf(text[i++], "rel:     %s", coresource->rel);
    sprintf(text[i++], "anchor:     %s", coresource->anchor);
    sprintf(text[i++], "port:     %d", coresource->port);
    sprintf(text[i++], "secure:   %d", coresource->secure);

    sprintf(text[i++], "types:"); //     %s", coresource->anchor);
    sprintf(text[i++], "interfaces:"); //     %s", coresource->anchor);
    sprintf(text[i++], "policy mask:");
    sprintf(text[i++], "endpoints::");
    msg_count = i;

    for (i=0; i < MSG_MAX; i++) { message[i] = text[i]; }

}

static void initialize_coresource_dlg()
{
    const char *buttons[] = {"</B/16>Cancel",
			     "<C>Exit",
			     "<C>Help",
    };

    dlg_coresource = newCDKDialog (cdkscreen,
				   CENTER,
				   CENTER,
				   (CDK_CSTRING2) message, msg_count,
				   (CDK_CSTRING2) buttons, 3,
				   COLOR_PAIR (2) | A_REVERSE,
				   TRUE,
				   TRUE,
				   FALSE);

    /* Check if we got a null value back. */
    if (dlg_coresource == 0)
	{
	    /* Shut down Cdk. */
	    destroyCDKScreen (cdkscreen);
	    endCDK ();

	    printf ("Cannot create the dialog box. Is the window too small?\n");
	    ExitProgram (EXIT_FAILURE);
	}
}

int run_coresource_inspector(int index)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY; index: %d", __func__, index);

    /* char *ptrstr = strrchr(item, ' ') + 1;
     * char *endptr;
     * unsigned long val = strtoul(ptrstr, &endptr, 16);
     * OIC_LOG_V(INFO, TAG, "MSG ID: %p", (OCClientResponse*)val);
     * OCClientResponse *msg = (OCClientResponse*) val;
     * OIC_LOG_V(INFO, TAG, "MSG ptr: %p", msg);
     * OIC_LOG_V(INFO, TAG, "resourceUri: %s", msg->resourceUri); */

    OCResourcePayload *resource = NULL;

    resource = oocf_coresource_db_mgr_get_coresource(index);
    if (!resource) {
	OIC_LOG_V(ERROR, TAG, "resource %d not found", index);
    } else {
	OIC_LOG_V(ERROR, TAG, "found resource %d: %p", index, resource);
    }

    construct_coresource_hdr(resource);
    initialize_coresource_dlg();

    /* pthread_mutex_lock(&display_mutex); */
    flockfile(stdout);
    selection = activateCDKDialog (dlg_coresource, 0);
    /* pthread_mutex_unlock(&display_mutex); */

    /* Tell them what was selected. */
    if (dlg_coresource->exitType == vESCAPE_HIT)
	{
	    OIC_LOG_V(DEBUG, TAG, "%s ESC", __func__);
	    /* msg[0] = msg->resourceUri;
	     * msg[1] = "";
	     * msg[2] = "<C>Press any key to continue.";
	     * popupLabel (cdkscreen, (CDK_CSTRING2) msg, 3); */
	}
    else if (dlg_coresource->exitType == vNORMAL)
	{
	    switch (selection) {
	    case 0:
		sprintf (temp, "<C>You selected Browse");
		/* browse_coresource_json(msg); */
		break;
	    case 1:
		sprintf (temp, "<C>You selected Exit");
		break;
	    case 2:
		sprintf (temp, "<C>You selected Help");
		break;
	    }
	    /* OIC_LOG_V(DEBUG, TAG, "%s NORMAL", __func__);
	     * sprintf (temp, "<C>You selected button #%d", selection); */
	    /* msg[0] = temp;
	     * msg[1] = "";
	     * msg[2] = "<C>Press any key to continue.";
	     * popupLabel (cdkscreen, (CDK_CSTRING2) msg, 3); */
	}
    funlockfile(stdout);

    /* eraseCDKDialog(dlg_coresource); */
    destroyCDKDialog(dlg_coresource);
    draw_scrollers();
    /* Clean up. */
    /* destroyCDKDialog (dlg_coresource);
     * destroyCDKScreen (cdkscreen); */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

