#include <cdk_test.h>
#include "openocf.h"
#include "coresource_inspector.h"

static CDKDIALOG *dlg_coresource  = 0;

static char *msg[MSG_MAX];
static char buffer[8192];

/* static char **msgs = NULL;
 * static char **s = NULL; */
static int msg_count = 0;

static char text[MSG_MAX][80];		/* since message is const we need a work buffer */

/* static char *header[MSG_MAX];
 * static int    header_count; */

static char temp[100];
static int selection;

static construct_coresource_hdr(OCResourcePayload *coresource)
{
    static int i = 0;
    for (i=0; i<MSG_MAX; i++) {
	memset(text[i], '\0', 80);
    }
    i = 0;
    sprintf(text[i++], "%s", "<C></R>Coresource Inspector<!R>");
    sprintf(text[i++], "uri:     %s", coresource->uri);
    sprintf(text[i++], "rel:     %s", coresource->rel);
    sprintf(text[i++], "anchor:  %s", coresource->anchor);
    sprintf(text[i++], "port:    %d", coresource->port);
    sprintf(text[i++], "secure:  %d", coresource->secure);

    sprintf(text[i++], "types:"); //     %s", coresource->anchor);
    sprintf(text[i++], "interfaces:"); //     %s", coresource->anchor);
    sprintf(text[i++], "policy mask:");
    sprintf(text[i++], "endpoints::");
    msg_count = i;

    for (i=0; i < MSG_MAX; i++) { msg[i] = text[i]; }

}

static void initialize_coresource_dlg()
{
    const char *buttons[] = {"</B/16>Retrieve",
			     "</B/16>Update",
			     "</B/16>Delete",
			     "<C>Dismiss",
			     "<C>Help",
    };

    dlg_coresource = newCDKDialog (cdkscreen,
				   CENTER,
				   CENTER,
				   (CDK_CSTRING2) msg, msg_count,
				   (CDK_CSTRING2) buttons, 5,
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

    memset(buffer, '\0', 8192);
    int rc = decode_resource(index, msg, buffer, &msg_count);
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
    else if (dlg_coresource->exitType == vNORMAL) {
	OIC_LOG_V(DEBUG, TAG, "exitType vNORMAL");
	switch (selection) {
	case 0:		/* RETRIEVE */
	    OIC_LOG_V(DEBUG, TAG, "case 0: RETRIEVE");
	    /* steps: pick an ep, construct query string, send request */
	    eraseCDKDialog(dlg_coresource);
	    retrieve_coresource(index);
	    break;
	case 1:		/* UPDATE */
	    OIC_LOG_V(DEBUG, TAG, "case 1: UPDATE");
	    break;
	case 2:		/* DELETE */
	    OIC_LOG_V(DEBUG, TAG, "case 2: DELETE");
	    break;
	case 3:		/* DISMISS */
	    OIC_LOG_V(DEBUG, TAG, "case 3: DISMISS");
	    break;
	case 4:		/* HELP */
	    OIC_LOG_V(DEBUG, TAG, "case 4: HELP");
	    break;
	}
	/* OIC_LOG_V(DEBUG, TAG, "%s NORMAL", __func__); */
    }
    funlockfile(stdout);

    /* eraseCDKDialog(dlg_coresource); */
    destroyCDKDialog(dlg_coresource);
    /* draw_msg_scrollers(); */
    /* Clean up. */
    /* destroyCDKDialog (dlg_coresource);
     * destroyCDKScreen (cdkscreen); */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}
