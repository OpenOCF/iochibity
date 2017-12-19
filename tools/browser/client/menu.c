#include <cdk_test.h>
#include <ncurses.h>

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "openocf.h"
#include "menu.h"

#define NUMBER_ITEMS false

static pthread_t        inbound_msg_ui_thread;
static pthread_t        outgoing_msg_ui_thread;

/* CDK gui structures */
/* SCREEN     *screen; */
/* CDKSWINDOW *request_log  = 0;
 * const char *request_log_title    = "<C></5>Request Log"; */

CDKLABEL *infoBox    = 0;

/* CDKSWINDOW *resources_list  = 0;
 * const char *resources_list_title    = "<C></5>Resources"; */

/* static CDKLABEL *msg_box;
 * static char *msg_box_msg[1]; */

static CDKMENU *menu        = 0;
static char *mesg[10];
static int menu_selection;
/* const char *mesg[5]; */
static static char temp[256];


static const char *menulist[MAX_MENU_ITEMS][MAX_SUB_ITEMS];
/* static const char *menuInfo[6][6] =
 * {
 *    {
 *       "",
 *       "Discover Platforms",
 *       "Discover Devices",
 *       "Discover Resources",
 *       "", ""
 *    },
 *    {
 *       "",
 *       "View Platforms",
 *       "View Devices",
 *       "View remote Resources",
 *       "View local CoResources",
 *       "View Transaction Log"
 *    },
 *    {
 *       "",
 *       "Unowned devices",
 *       "Owned devices",
 *       "Transfer",
 *       ""
 *    },
 *    {
 *       "",
 *       "Credentials",
 *       "Access Control Lists",
 *       "", ""
 *    },
 *    {
 *       "",
 *       "Help Discovery",
 *       "Help View",
 *       "Info about the program",
 *       "", ""
 *    },
 *    {
 *       "",
 *       "",
 *       "", "" }
 * }; */

/* while pre- and post-processing is running, other threads have a
   chance to update the screen? */
/* called for every keystroke before it's passed to CDK */
static int menu_pre_process (EObjectType cdktype GCC_UNUSED,
			      void *object,
			      void *clientData,
			      chtype input GCC_UNUSED)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 1;
}

/*
 * This gets called for every keystroke, after CDK processing.
 */
static int menu_post_process (EObjectType cdktype GCC_UNUSED,
			      void *object,
			      void *clientData,
			      chtype input GCC_UNUSED)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

   OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
   return 0;
}

void erase_menu()
{
    eraseCDKMenu(menu);
}

void initialize_menu()
{
    int submenusize[6], menuloc[6];

    /* Set up the menu. */
    menulist[0][0] = "</B>Discover<!B>";
    menulist[0][1] = "</B>Platforms<!B>";
    menulist[0][2] = "</B>Devices<!B>";
    menulist[0][3] = "</B>Resources<!B>";
    submenusize[0] = 4;
    menuloc[0] = LEFT;

    menulist[1][0] = "</B>View<!B>";
    menulist[1][1] = "</B>Message Logs<!B>";
    menulist[1][2] = "</B>CoResources<!B>";
    menulist[1][3] = "</B>Platforms<!B> ";
    menulist[1][4] = "</B>Devices<!B>";
    menulist[1][5] = "</B>Resources<!B>";
    submenusize[1] = 6;
    menuloc[1] = LEFT;

    menulist[2][0] = "</B>Ownership<!B>";
    menulist[2][1] = "</B>Discover unowned devices<!B> ";
    menulist[2][2] = "</B>Discover owned devices<!B>";
    menulist[2][3] = "</B>Transfer ownership<!B>";
    submenusize[2] = 4;
    menuloc[2] = LEFT;

    menulist[3][0] = "</B>Provisioning<!B>";
    menulist[3][1] = "</B>Credentials<!B> ";
    menulist[3][2] = "</B>ACLs<!B>";
    submenusize[3] = 3;
    menuloc[3] = LEFT;

    menulist[4][0] = "</B>Exit<!B>";
    menulist[4][1] = "</B>Exit<!B>";
    submenusize[4] = 2;
    menuloc[4] = RIGHT;

    menulist[5][0] = "</B>Help<!B>";
    menulist[5][1] = "</B>On Discovery <!B>";
    menulist[5][2] = "</B>On View <!B>";
    menulist[5][3] = "</B>About...<!B>";
    submenusize[5] = 4;
    menuloc[5] = RIGHT;

    /* Create the label window. */
    /* mesg[0] = "                                          ";
     * mesg[1] = "                                          ";
     * mesg[2] = "                                          ";
     * mesg[3] = "                                          ";
     * infoBox = newCDKLabel (cdkscreen, CENTER, CENTER,
     * 			   (CDK_CSTRING2) mesg, 4,
     * 			   TRUE, TRUE); */

    /* Create the menu. */
    menu = newCDKMenu (cdkscreen,
		       menulist, 6, submenusize,
		       menuloc,
		       TOP, A_UNDERLINE, A_REVERSE);

   /* Create the processing functions. */
   /* setCDKMenuPreProcess (menu, menu_pre_process, infoBox);
    * setCDKMenuPostProcess (menu, menu_post_process, infoBox); */
}

void dismiss_menu(void)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    eraseCDKMenu(menu);
    /* destroyCDKMenu(menu); */
    /* refreshCDKScreen (cdkscreen); */
}

void run_menu()
{
    /* pthread_mutex_lock(&msgs_mutex); */

    initialize_menu();

    /* Do this until they hit q or escape. */
    for (;;)
	{
	    /* Draw the CDK screen. */
	    /* refreshCDKScreen (cdkscreen); */

	    /* Activate the menu. */
	    OIC_LOG_V(INFO, TAG, "activating menu");
	    menu_selection = activateCDKMenu (menu, 0);

	    /* Determine how the user exited from the widget. */
	    switch (menu->exitType) {
	    case vESCAPE_HIT:
		OIC_LOG_V(INFO, TAG, "%s ESC", __func__);
		goto exit;
		break;
	    case vNORMAL:
		switch(menu_selection/100) {
		case 0:
		    if (menu_selection % 100 == 2) {
			OIC_LOG_V(INFO, TAG, "discovering resources");
			discover_resources();
		    }
		    break;
		case 1:
		    switch (menu_selection % 100) {
		    case 0:
			dismiss_menu();
			next_scroller = INBOUND;
			run_msg_logs();
			/* erase_msg_scrollers(); */
			break;
		    case 3:
			OIC_LOG_V(INFO, TAG, "viewing resources");
			gui_view_resources();
			goto exit;
			break;
		    }
		    break;

		case 5:
		    switch(menu_selection % 100) {
		    case 0:
			goto exit;
		    }
		default:
		    sprintf (temp, "</31>You selected menu #%d, submenu #%d",
			     menu_selection / 100,
			     menu_selection % 100);
		    mesg[0] = temp;
		    mesg[1] = "";
		    mesg[2] = "<C>Press any key to continue.";
		    popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
		    /* goto exit; */
		}
		break;
	    default:
		mesg[0] = "<C>Huh?. No menu item was selected.";
		/* mesg[1] = "", */
		OIC_LOG_V(INFO, TAG, "%s DEFAULT: %d", __func__, menu->exitType);
		/* printf("exitType: %d\n", menu->exitType); */
		mesg[2] = "<C>Press any key to continue.";
		popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
		break;
	    }
	    OIC_LOG_V(INFO, TAG, "%s LOOP", __func__);
	}
 exit:
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
    /* pthread_mutex_unlock(&msgs_mutex); */

    /* refreshCDKScreen (cdkscreen); */
    return;
}
