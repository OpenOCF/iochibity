/* $Id: dialog_ex.c,v 1.14 2016/12/04 15:22:16 tom Exp $ */

#include <cdk_test.h>
#include "openocf.h"
#include "action_dialog.h"

#ifdef HAVE_XCURSES
char *XCursesProgramName = "dialog_ex";
#endif

static CDKDIALOG *dlg_action  = 0;
static const char *message[10];
static const char *mesg[3];
static char temp[100];
static int selection;

static bool initialized = false;


static void initialize()
{
    const char *buttons[] = {"</B16>Browse",
			     "</B/24>CRUDN",
			     "</B16>Ownership",
			     "</B>Provisioning",
			     "<C>Help",
			     "<C>Quit"};

    /* Create the message within the dialog box. */
    message[0] = "<C></U>Dialog Widget Demo";
    message[1] = "";
    message[2] = "<C>The dialog widget allows the programmer to create";
    message[3] = "<C>a popup dialog box with buttons. The dialog box";
    message[4] = "<C>can contain </B/32>colours<!B!32>, </R>character attributes<!R>";
    message[5] = "<R>and even be right justified.";
    message[6] = "<L>and left.";

    /* Create the dialog box. */
    dlg_action = newCDKDialog (cdkscreen,
			     CENTER,
			     CENTER,
			     (CDK_CSTRING2) message, 7,
			     (CDK_CSTRING2) buttons, 6,
			     COLOR_PAIR (2) | A_REVERSE,
			     TRUE,
			     TRUE,
			     FALSE);

    /* Check if we got a null value back. */
    if (dlg_action == 0)
	{
	    /* Shut down Cdk. */
	    destroyCDKScreen (cdkscreen);
	    endCDK ();

	    printf ("Cannot create the dialog box. Is the window too small?\n");
	    ExitProgram (EXIT_FAILURE);
	}
}

int run_action_dialog (CDKSCREEN *cdkscreen, char* item)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY; msg: %s", __func__, item);
    if (!initialized) initialize();

    /* pthread_mutex_lock(&display_mutex); */
    flockfile(stdout);
    selection = activateCDKDialog (dlg_action, 0);
    /* pthread_mutex_unlock(&display_mutex); */

    /* Tell them what was selected. */
    if (dlg_action->exitType == vESCAPE_HIT)
	{
	    mesg[0] = "hello";
	    mesg[1] = "";
	    mesg[2] = "<C>Press any key to continue.";
	    popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
	}
    else if (dlg_action->exitType == vNORMAL)
	{
	    sprintf (temp, "<C>%s", "hi there");
	    mesg[0] = temp;
	    mesg[1] = "";
	    mesg[2] = "<C>Press any key to continue.";
	    popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
	}
    funlockfile(stdout);

    eraseCDKDialog(dlg_action);
    draw_msg_scrollers();
    /* Clean up. */
    /* destroyCDKDialog (dlg_action);
     * destroyCDKScreen (cdkscreen); */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}
