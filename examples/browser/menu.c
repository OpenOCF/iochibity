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
#include "client_ui.h"

#ifdef HAVE_XCURSES
char *XCursesProgramName = "menu_ex";
#endif

#define NUMBER_ITEMS false

static pthread_t        incoming_msg_ui_thread;
static pthread_t        outgoing_msg_ui_thread;

/* CDK gui structures */
SCREEN     *screen;
/* CDKSWINDOW *request_log  = 0;
 * const char *request_log_title    = "<C></5>Request Log"; */

/* CDKSWINDOW *resources_list  = 0;
 * const char *resources_list_title    = "<C></5>Resources"; */

static CDKLABEL *msg_box;
static char *msg_box_msg[1];

static CDKMENU *menu        = 0;
static char *mesg[10];
static int menu_selection;
/* const char *mesg[5]; */
static static char temp[256];

static int displayCallback (EObjectType cdktype, void *object,
			    void *clientData,
			    chtype input);

static const char *menulist[MAX_MENU_ITEMS][MAX_SUB_ITEMS];
static const char *menuInfo[5][6] =
{
   {
      "",
      "Discover Platforms",
      "Discover Devices",
      "Discover Resources",
      "", ""
   },
   {
      "",
      "View Platforms",
      "View Devices",
      "View remote Resources",
      "View local CoResources",
      "View Transaction Log"
   },
   {
      "",
      "Unowned devices",
      "Owned devices",
      "Transfer",
      ""
   },
   {
      "",
      "Credentials",
      "Access Control Lists",
      "", ""
   },
   {
      "",
      "Help Discovery",
      "Help View",
      "Info about the program",
      "", "" }
};

void gui_discover_resources ()
{
    /* FIXME: put this on a worker thread! */
    discover_resources();

 /* FIXME: provide visual feedback */

    /* Draw the scrolling window. */
    /* drawCDKSwindow (request_log, ObjOf (request_log)->box); */

    /* Load up the scrolling window. */
    /* addCDKSwindow (request_log, "<C></11>Sent DiscoverResource request", TOP); */

    /* Activate the scrolling window. */
    /* activateCDKSwindow (request_log, 0); */
}

void gui_view_resources ()
{
    char **resource_uris = list_resource_uris();

    /* Draw the scrolling window. */
    /* drawCDKSwindow (request_log, ObjOf (request_log)->box); */

    /* Load up the scrolling window. */
    /* for each uri ...*/
    /* addCDKSwindow (request_log, "<C></11>Sent DiscoverResource request", TOP); */

    /* Activate the scrolling window. */
    /* activateCDKSwindow (request_log, 0); */
}


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
    int submenusize[5], menuloc[5];

    /* Set up the menu. */
    menulist[0][0] = "</B>Discover<!B>";
    menulist[0][1] = "</B>Platforms<!B>";
    menulist[0][2] = "</B>Devices<!B>";
    menulist[0][3] = "</B>Resources<!B>";
    submenusize[0] = 4;
    menuloc[0] = LEFT;

    menulist[1][0] = "</B>View<!B>";
    menulist[1][1] = "</B>Platforms<!B> ";
    menulist[1][2] = "</B>Devices<!B>";
    menulist[1][3] = "</B>Resources<!B>";
    menulist[1][4] = "</B>CoResources<!B>";
    menulist[1][5] = "</B>Transaction Log<!B>";
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

    menulist[4][0] = "</B>Help<!B>";
    menulist[4][1] = "</B>On Discovery <!B>";
    menulist[4][2] = "</B>On View <!B>";
    menulist[4][3] = "</B>About...<!B>";
    submenusize[4] = 4;
    menuloc[4] = RIGHT;

    /* Create the label window. */
    mesg[0] = "                                          ";
    mesg[1] = "                                          ";
    mesg[2] = "                                          ";
    mesg[3] = "                                          ";
    infoBox = newCDKLabel (cdkscreen, CENTER, CENTER,
			   (CDK_CSTRING2) mesg, 4,
			   TRUE, TRUE);

    /* Create the menu. */
    menu = newCDKMenu (cdkscreen,
		       menulist, 5, submenusize,
		       menuloc,
		       TOP, A_UNDERLINE, A_REVERSE);

   /* Create the processing functions. */
   setCDKMenuPreProcess (menu, menu_pre_process, infoBox);
   setCDKMenuPostProcess (menu, menu_post_process, infoBox);
}

void run_menu()
{
    pthread_mutex_lock(&msgs_mutex);

    /* Do this until they hit q or escape. */
    for (;;)
	{
	    /* Draw the CDK screen. */
	    refreshCDKScreen (cdkscreen);

	    /* Activate the menu. */
	    menu_selection = activateCDKMenu (menu, 0);

	    /* Determine how the user exited from the widget. */
	    switch (menu->exitType) {
	    case vESCAPE_HIT:
		{
		    OIC_LOG_V(INFO, TAG, "%s ESC", __func__);
		    goto exit;
		    break;
		}
	    case vNORMAL:
		{
		    if (menu_selection/100 == 0 && menu_selection % 100 == 2) {
			mesg[0] = "Discovering resources ...";
			// gui_discover_resources();
			goto exit;
			/* fflush(stdout);
			 * refreshCDKScreen (cdkscreen); */
		    } else if (menu_selection/100 == 1 && menu_selection % 100 == 3) {
			mesg[0] = "Viewing resources ...";
			gui_view_resources();
			goto exit;
			/* fflush(stdout);
			 * refreshCDKScreen (cdkscreen); */
		    } else {
			sprintf (temp, "</31>You selected menu #%d, submenu #%d",
				 menu_selection / 100,
				 menu_selection % 100);
			mesg[0] = temp;
			mesg[1] = "";
			mesg[2] = "<C>Press any key to continue.";
			popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
			goto exit;
		    }
		    break;
		}
	    default:
		{
		    mesg[0] = "<C>Huh?. No menu item was selected.";
		    /* mesg[1] = "", */
		    OIC_LOG_V(INFO, TAG, "%s DEFAULT: %d", __func__, menu->exitType);
		    /* printf("exitType: %d\n", menu->exitType); */
		    mesg[2] = "<C>Press any key to continue.";
		    popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
		    break;
		}
		break;
	    }
	}
 exit:
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
    eraseCDKMenu(menu);
    /* destroyCDKMenu(menu); */
    pthread_mutex_unlock(&msgs_mutex);

    /* refreshCDKScreen (cdkscreen); */
    return;
}

WINDOW *subWindow;

int run_gui (void)
{
    int msg_selection;

    /* Start curses. */
    (void) initCDKScreen (NULL);
    curs_set (0);
    cbreak();			/* ncurses: disable line buffering */

    /* Create a basic window. */
    /* subWindow = newwin (LINES - 5, COLS - 10, 2, 5); */

    /* Start Cdk. */
    cdkscreen = initCDKScreen (NULL);
    keypad(cdkscreen, TRUE);

    /* Start CDK color. */
    initCDKColor ();

    box (subWindow, ACS_VLINE, ACS_HLINE);
    wrefresh (subWindow);

    refreshCDKScreen (cdkscreen);

    /* initialize_menu(); */

    init_msg_log_scrollers();

    draw_scrollers();

    msg_box_msg[0] = "                                                                                ";
    msg_box = newCDKLabel (cdkscreen, CENTER, BOTTOM,
			   (CDK_CSTRING2) msg_box_msg,
			   1,
			   TRUE, FALSE);

    drawCDKLabel(msg_box, TRUE);

    OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self());
    int err = pthread_create(&(incoming_msg_ui_thread), NULL, &incoming_msg_ui_writer, NULL);
    if (err != 0) {
	OIC_LOG_V(DEBUG, TAG, "Can't create incoming_msg_ui_thread :[%s]", strerror(err));
    }
    OIC_LOG_V(INFO, TAG, "%s FOO", __func__);
    err = pthread_create(&(outgoing_msg_ui_thread), NULL, &outgoing_msg_ui_writer, NULL);
    if (err != 0) {
	OIC_LOG_V(DEBUG, TAG, "Can't create outgoing_msg_ui_thread :[%s]", strerror(err));
    }

    /* this is vi stuff? */
    /* *INDENT-EQLS* */

/* #define TAB 9
 * #define ESC 27 */
#define INCOMING 0
#define OUTGOING 1
#define CORESOURCES 2

    /* need this code to distinguish between ESC and arrow keys (e.g. esc[A, esc[B, etc.) */
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    fd_set savefds = readfds;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int rc;
    int c, c2;
    int next = OUTGOING;

    char *msg_details;

    int sz;

    while(1) {
	if (g_quit_flag) break;
	c = getch();
	switch(c) {
	case 'q':
	    OIC_LOG(DEBUG, TAG, "Received quit key; quiting");
	    goto exit;
	    break;
	case KEY_ENTER:
	    OIC_LOG(DEBUG, TAG, "EEEEEEEEEEEEEEEE KEY_ENTER");
	    break;
	case KEY_DOWN:
	    OIC_LOG(DEBUG, TAG, "DDDDDDDDDDDDDDDD KEY_DOWN");
	    break;
	case KEY_ESC:
	    rc = select(1, &readfds, NULL, NULL, &timeout);
	    readfds = savefds;
	    if (rc == 0) {
		OIC_LOG(DEBUG, TAG, "ESC: running action dialog");
		run_action_dialog(cdkscreen, "");
		/* run_menu(); */
		OIC_LOG(DEBUG, TAG, "action dialog finished");
		/* goto exit; */
	    } else {
		/* must be an arrow key, e.g. esc[A; ignore */
		OIC_LOG(DEBUG, TAG, "AAAAAAAAAAAAAAAAAAAAAAAA ARROW KEY");
		getch(); 	/* eat '[' */
		getch();	/* A, B, C, or D */
	    }
	    break;
	case KEY_TAB:
	    switch(next) {
	    case INCOMING:
		/* if (incoming_msg_scroller) */
		/* setCDKScrollCurrentItem(incoming_msg_scroller, 1); */
		/* setCDKScrollHighlight(outgoing_msg_scroller, A_REVERSE); */
		setCDKScrollBackgroundColor(incoming_msg_scroller, "</31>");
		sz = incoming_msg_scroller->listSize;
		if (sz > 0) {
		    int sel = sz - incoming_current_item;
		    char *tmp = chtype2Char (incoming_msg_scroller->item[sel]);
		    OIC_LOG_V(DEBUG, TAG, "MSG details: %s", tmp);
		    setCDKLabelMessage(msg_box, &tmp, 1);
		    freeChar(tmp);
		}
		setCDKScrollCurrentItem(incoming_msg_scroller, sz - incoming_current_item);
		OIC_LOG_V(DEBUG, TAG, "set current item to: %d", incoming_current_item);

		/* pthread_mutex_lock(&display_mutex); */
		msg_selection = activateCDKScroll(incoming_msg_scroller, 0);
		/* pthread_mutex_unlock(&display_mutex); */

		incoming_current_item
		    = incoming_msg_scroller->listSize - getCDKScrollCurrentItem(incoming_msg_scroller);
		OIC_LOG(DEBUG, TAG, "incoming_msg_scroller EXIT");
		/* Determine how the widget was exited. */
		if (msg_selection < 0) {
		    OIC_LOG(DEBUG, TAG, "User struck ESC");
		    setCDKScrollBackgroundColor(incoming_msg_scroller, "<!31>");
		} else {
		    /* user struck RETURN or TAB */
		    setCDKScrollBackgroundColor(incoming_msg_scroller, "<!31>");
		    next = CORESOURCES;
		    ungetch(KEY_TAB); /* NB: this is from ncurses */
		}
		/* refreshCDKScreen (cdkscreen); */
		break;
	    case OUTGOING:
		/* setCDKScrollHighlight(outgoing_msg_scroller, A_REVERSE); */
		setCDKScrollBackgroundColor(outgoing_msg_scroller, "</31>");
		sz = outgoing_msg_scroller->listSize;
		if (sz > 0) {
		    int sel = sz - outgoing_current_item;
		    char *tmp = chtype2Char (outgoing_msg_scroller->item[sel]);
		    OIC_LOG_V(DEBUG, TAG, "MSG details: %s", tmp);
		    setCDKLabelMessage(msg_box, &tmp, 1);
		    freeChar(tmp);
		}
		setCDKScrollCurrentItem(outgoing_msg_scroller, sz - outgoing_current_item);

		/* pthread_mutex_lock(&display_mutex); */
		msg_selection = activateCDKScroll(outgoing_msg_scroller, 0);
		/* pthread_mutex_unlock(&display_mutex); */

		outgoing_current_item
		    = outgoing_msg_scroller->listSize - getCDKScrollCurrentItem(outgoing_msg_scroller);
		OIC_LOG(DEBUG, TAG, "outgoing_msg_scroller EXIT");
		if (msg_selection < 0) {
		    OIC_LOG(DEBUG, TAG, "User struck ESC");
		    setCDKScrollBackgroundColor(outgoing_msg_scroller, "<!31>");
		} else {
		    OIC_LOG(DEBUG, TAG, "User struck TAB");
		    ungetch(KEY_TAB); // , stdin);
		    setCDKScrollBackgroundColor(outgoing_msg_scroller, "<!31>");
		    next = INCOMING;
		}
		/* refreshCDKScreen (cdkscreen); */
		break;
	    case CORESOURCES:
		setCDKScrollBackgroundColor(coresource_scroller, "</31>");
		sz = coresource_scroller->listSize;
		if (sz > 0) {
		    int sel = sz - coresource_current_item;
		    char *tmp = chtype2Char (coresource_scroller->item[sel]);
		    OIC_LOG_V(DEBUG, TAG, "MSG details: %s", tmp);
		    setCDKLabelMessage(msg_box, &tmp, 1);
		    freeChar(tmp);
		}
		setCDKScrollCurrentItem(coresource_scroller, sz - coresource_current_item);

		/* pthread_mutex_lock(&display_mutex); */
		msg_selection = activateCDKScroll(coresource_scroller, 0);
		/* pthread_mutex_unlock(&display_mutex); */

		coresource_current_item
		    = coresource_scroller->listSize - getCDKScrollCurrentItem(coresource_scroller);
		OIC_LOG(DEBUG, TAG, "coresource_scroller EXIT");
		if (msg_selection < 0) {
		    OIC_LOG(DEBUG, TAG, "User struck ESC");
		    setCDKScrollBackgroundColor(coresource_scroller, "<!31>");
		} else {
		    OIC_LOG(DEBUG, TAG, "User struck TAB");
		    ungetch(KEY_TAB); // , stdin);
		    setCDKScrollBackgroundColor(coresource_scroller, "<!31>");
		    next = OUTGOING;
		}
		/* refreshCDKScreen (cdkscreen); */
		break;
	    }
	    drawCDKLabel(msg_box, TRUE);
	    draw_scrollers();
	    break;
	default:
	    OIC_LOG_V(DEBUG, TAG, "GETCH: %c", c);
	}
    }

 exit:
    OIC_LOG(DEBUG, TAG, "QQQQQQQQQQQQQQQQ; quiting");
    /* Clean up. */
    /* g_quit_flag = 1; */
    rc = sem_post(quit_semaphore);
    /* if (rc < 0) {
     * 	OIC_LOG_V(ERROR, TAG, "Q sem_post(quit_semaphore) rc: %s", strerror(errno));
     * }
     * sem_post(incoming_msg_semaphore);
     * if (rc < 0) {
     * 	OIC_LOG_V(ERROR, TAG, "Q sem_post(incoming_msg_semaphore) rc: %s", strerror(errno));
     * }
     * sem_post(outgoing_msg_semaphore);
     * if (rc < 0) {
     * 	OIC_LOG_V(ERROR, TAG, "Q sem_post(outgoing_msg_semaphore) rc: %s", strerror(errno));
     * } */
    OIC_LOG(DEBUG, TAG, "QQQQQQQQQQQQQQQQ; waiting on ui threads");
    pthread_join(incoming_msg_ui_thread, NULL);
    pthread_join(outgoing_msg_ui_thread, NULL);
    /* pthread_mutex_lock(&msgs_mutex); */
    OIC_LOG(DEBUG, TAG, "QQQQQQQQQQQQQQQQ; cleanup");
    destroyCDKMenu (menu);
    destroyCDKLabel (infoBox);
    /* FIXME: scrollers, mutexes, etc. */
    destroyCDKScroll(incoming_msg_scroller);
    destroyCDKScroll(outgoing_msg_scroller);
    destroyCDKLabel(msg_box);
    destroyCDKScreen (cdkscreen);
    endCDK ();
    pthread_mutex_unlock(&msgs_mutex);
    /* delscreen(scr); */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}
