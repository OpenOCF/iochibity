#include "openocf.h"

#include <cdk_test.h>
#include <ncurses.h>

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "client.h"

#ifdef HAVE_XCURSES
char *XCursesProgramName = "menu_ex";
#endif

#define NUMBER_ITEMS false

pthread_mutex_t display_mutex;

static pthread_t        incoming_msg_ui_thread;
static pthread_t        outgoing_msg_ui_thread;

/* CDK gui structures */
SCREEN     *screen;
CDKSCREEN  *cdkscreen = 0;
CDKSWINDOW *request_log  = 0;
const char *request_log_title    = "<C></5>Request Log";

CDKSCROLL *incoming_msg_scroller = NULL;
int incoming_current_item = 0;

CDKSCROLL *outgoing_msg_scroller = NULL;
int outgoing_current_item = 0;

CDKLABEL *msg_box;
char *msg_details[1];

CDKSWINDOW *resources_list  = 0;
const char *resources_list_title    = "<C></5>Resources";

CDKSWINDOW *details_win  = 0;

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

void reset_scroller(CDKSCROLL *scroller)
{
    int ct = getCDKScrollCurrentTop(scroller);
    for (int i=0; i< ct; i++) {
	deleteCDKScrollItem(scroller, i);
    }
}

void* incoming_msg_ui_writer(void *arg)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self());
    int len = 0;
    char **msgs = NULL;
    char **s = NULL;
    int msg_count;
    static int rc;

    sem_post(incoming_msg_log_ready_semaphore);
    if (rc < 0) {
	OIC_LOG_V(ERROR, TAG, "sem_post(incoming_msg_log_ready_semaphore) rc: %s", strerror(errno));
    } else {
	OIC_LOG_V(DEBUG, TAG, "CONSUMER sem_post(incoming_msg_log_ready_semaphore)");
    }

    while(1) {
	OIC_LOG(DEBUG, TAG, "waiting on incoming_msg_semaphore");
	rc = sem_wait(incoming_msg_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(ERROR, TAG, "sem_wait(incoming_msg_semaphore) rc: %s", strerror(errno));
	} else {
	    if (incoming_msg_scroller) {
		OIC_LOG_V(DEBUG, TAG, "CONSUMER acquired incoming_msg_semaphore");
		pthread_mutex_lock(&msgs_mutex);
		len = u_linklist_length(incoming_msgs);
		reset_scroller(incoming_msg_scroller);
		if (msgs) free(msgs);
		msgs = calloc(len, sizeof(char*));
		s = &(msgs[len-1]);
		/* s = msgs; */
		OIC_LOG_V(INFO, TAG, "<<<<<<<<<<<<<<<< Got incoming msg; count: %d", len);
		u_linklist_iterator_t *iterTable = NULL;
		u_linklist_init_iterator(incoming_msgs, &iterTable);
		while (NULL != iterTable) {
		    *s-- = (char*) u_linklist_get_data(iterTable);
		    u_linklist_get_next(&iterTable);
		}
		flockfile(stdout);
		setCDKScrollItems (incoming_msg_scroller, (CDK_CSTRING2) msgs, len, NUMBER_ITEMS);
		setCDKScrollCurrentItem(incoming_msg_scroller,
					incoming_msg_scroller->listSize - incoming_current_item);
		drawCDKScroll(incoming_msg_scroller,  /* boxed? */ TRUE);
		funlockfile(stdout);
		pthread_mutex_unlock(&msgs_mutex);
	    } else {
		OIC_LOG_V(ERROR, TAG, "incoming_msg_scroller not initialized");
		exit(EXIT_FAILURE);
	    }
	}
	rc = sem_post(incoming_msg_log_ready_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(ERROR, TAG, "sem_post(incoming_msg_log_ready_semaphore) rc: %s", strerror(errno));
	} else {
	    OIC_LOG_V(DEBUG, TAG, "CONSUMER sem_post(incoming_msg_log_ready_semaphore)");
	}
    }
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

 void* outgoing_msg_ui_writer(void *arg)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self());
    int len = 0;
    char **msgs = NULL;
    char **s = NULL;
    int msg_count;
    static int rc;

    sem_post(outgoing_msg_log_ready_semaphore);
    if (rc < 0) {
	OIC_LOG_V(ERROR, TAG, "sem_post(outgoing_msg_log_ready_semaphore) rc: %s", strerror(errno));
    } else {
	OIC_LOG_V(DEBUG, TAG, "CONSUMER sem_post(outgoing_msg_log_ready_semaphore)");
    }

    while(1) {
	OIC_LOG(DEBUG, TAG, "waiting on outgoing_msg_semaphore");
	rc = sem_wait(outgoing_msg_semaphore);
	if (rc < 0) {
	    /* OIC_LOG_V(ERROR, TAG, "sem_wait(&outgoing_msg_semaphore) rc: %s", strerror(errno)); */
	} else {
	    if (outgoing_msg_scroller) {
		OIC_LOG_V(DEBUG, TAG, "CONSUMER acquired outgoing_msg_semaphore");
		pthread_mutex_lock(&msgs_mutex);
		len = u_linklist_length(outgoing_msgs);
		reset_scroller(outgoing_msg_scroller);
		if (msgs) free(msgs);
		msgs = calloc(len, sizeof(char*));
		s = &(msgs[len-1]);
		OIC_LOG_V(INFO, TAG, ">>>>>>>>>>>>>>>> GOT outgoing msg; count: %d", len);
		u_linklist_iterator_t *iterTable = NULL;
		u_linklist_init_iterator(outgoing_msgs, &iterTable);
		while (NULL != iterTable) {
		    *s-- = (char*) u_linklist_get_data(iterTable);
		    u_linklist_get_next(&iterTable);
		}
		flockfile(stdout);
		setCDKScrollItems (outgoing_msg_scroller, (CDK_CSTRING2) msgs, len, NUMBER_ITEMS);
		setCDKScrollCurrentItem(outgoing_msg_scroller,
					outgoing_msg_scroller->listSize - outgoing_current_item);
		drawCDKScroll(outgoing_msg_scroller,  /* boxed? */ TRUE);
		funlockfile(stdout);
		pthread_mutex_unlock(&msgs_mutex);
	    } else {
		OIC_LOG_V(ERROR, TAG, "outgoing_msg_scroller not initialized");
		exit(EXIT_FAILURE);
	    }
	}
	rc =sem_post(outgoing_msg_log_ready_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(DEBUG, TAG, "sem_post(&outgoing_msg_log_ready_semaphore)");
	}
    }
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

CDKLABEL *infoBox    = 0;

static int incoming_msg_scroller_pre_process (EObjectType cdktype GCC_UNUSED,
			      void *object,
			      void *clientData,
			      chtype input)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    CDKSCROLL *scroller        = (CDKSCROLL *)object;
    CDKLABEL *infoBox    = (CDKLABEL *)clientData;
    const char *mesg[5];
    char temp[256];
    char *the_item;
    int i;

    /* c = getchCDKObject(ObjOf (scroller), &functionKey); */
    /* switch(input) {
     * case KEY_TAB:
     * 	OIC_LOG_V(DEBUG, TAG, "Keystroke: TAB");
     * 	break;
     * case 'q':
     * 	(void)injectCDKScroll(scroller, KEY_TAB);
     * 	break;
     * case KEY_UP:
     * case KEY_DOWN:
     * 	i = getCDKScrollCurrentItem(scroller);
     * 	the_item = chtype2Char (scroller->item[i]);
     * 
     * 	OIC_LOG_V(DEBUG, TAG, "SELECTION: index %d, %s", i, the_item);
     * 	sprintf (temp, "<C>%.*s", (int)(sizeof (temp) - 20), the_item);
     * 	mesg[1] = temp;
     * 	mesg[2] = "<C>Press any key to continue.";
     * 	popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
     * 	freeChar (the_item);
     * 	setCDKScrollCurrentItem(scroller, i);
     * 	return 0;
     * 	break;
     * default:
     * 	OIC_LOG_V(DEBUG, TAG, "Keystroke: %c", input);
     * } */


    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 1;			/* 0 = no further action */
}

static int outgoing_msg_scroller_pre_process (EObjectType cdktype GCC_UNUSED,
			      void *object,
			      void *clientData,
			      chtype input GCC_UNUSED)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 1;			/* 0 = no further action */
}

static int incoming_msg_scroller_post_process (EObjectType cdktype GCC_UNUSED,
						void *object,
						void *clientData,
						chtype input)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    CDKSCROLL *scroller        = (CDKSCROLL *)object;
    CDKLABEL *infoBox    = (CDKLABEL *)clientData;
    const char *mesg[5];
    char temp[256];
    char *the_item;
    int i;

    switch(input) {
    case KEY_TAB:
	OIC_LOG_V(DEBUG, TAG, "Keystroke: TAB");
	break;
    case 'q':
	(void)injectCDKScroll(scroller, KEY_TAB);
	break;
    case KEY_UP:
    case KEY_DOWN:
	incoming_current_item = scroller->listSize - getCDKScrollCurrentItem(scroller);
	OIC_LOG_V(DEBUG, TAG, "incoming preprocess, curritem: %d", incoming_current_item);

	the_item= chtype2Char (incoming_msg_scroller->item[getCDKScrollCurrentItem(scroller)]);
	/* sprintf (temp, "<C>%s", msg_details); */
	setCDKLabelMessage(msg_box, &the_item, 1);
	freeChar(the_item);

	/* i = getCDKScrollCurrentItem(scroller);
	 * the_item = chtype2Char (scroller->item[i]);
	 * 
	 * OIC_LOG_V(DEBUG, TAG, "SELECTION: index %d, %s", i, the_item);
	 * sprintf (temp, "<C>%.*s", (int)(sizeof (temp) - 20), the_item);
	 * mesg[1] = temp;
	 * mesg[2] = "<C>Press any key to continue.";
	 * popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 2);
	 * freeChar (the_item); */
	/* setCDKScrollCurrentItem(scroller, i);
	 * drawCDKScroll(scroller, TRUE); */
	/* return 0; */
	break;
    default:
	OIC_LOG_V(DEBUG, TAG, "Keystroke: %c", input);
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 0;
}

static int outgoing_msg_scroller_post_process (EObjectType cdktype GCC_UNUSED,
						void *object,
						void *clientData,
						chtype input GCC_UNUSED)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    CDKSCROLL *scroller        = (CDKSCROLL *)object;
    CDKLABEL *infoBox    = (CDKLABEL *)clientData;
    const char *mesg[5];
    char temp[256];
    char *the_item;
    int i;

    switch(input) {
    case KEY_TAB:
	/* OIC_LOG_V(DEBUG, TAG, "Keystroke: TAB"); */
	break;
    case 'q':
	(void)injectCDKScroll(scroller, KEY_TAB);
	break;
    case KEY_UP:
    case KEY_DOWN:
	outgoing_current_item = scroller->listSize - getCDKScrollCurrentItem(scroller);
	OIC_LOG_V(DEBUG, TAG, "outgoing preprocess, curritem: %d", outgoing_current_item);

	the_item= chtype2Char (outgoing_msg_scroller->item[getCDKScrollCurrentItem(scroller)]);
	setCDKLabelMessage(msg_box, &the_item, 1);
	freeChar(the_item);

	break;
    default:
	OIC_LOG_V(DEBUG, TAG, "Keystroke: %c", input);
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 0;
}

static int incoming_msg_scroller_quit (EObjectType cdktype GCC_UNUSED,
			 void *object,
			 void *clientData GCC_UNUSED,
			 chtype key GCC_UNUSED)
{
    /* OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
     *
     * OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__); */
    return TRUE;
}

void init_msg_log_scrollers()
{
   incoming_msg_scroller = newCDKScroll (cdkscreen,
			      RIGHT, /* x */
			      CENTER, /* y */
			      RIGHT,  /* scrollbar */
			      -4,     /* H, 0 = max  */
			      36,     /* W */
			      "</U/05>Incoming Responses", /* title */
			      0, /* item list - list of strings or 0 */
			      0,     /* item count */
			      NUMBER_ITEMS,  /* numbered */
			      A_REVERSE, /* highlighting */
			      TRUE,	 /* boxed */
			      FALSE);	 /* shadowed */

   /* Is the scrolling list null? */
   if (incoming_msg_scroller == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK ();

      printf ("Cannot make scrolling list. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }
   setCDKScrollPreProcess (incoming_msg_scroller, incoming_msg_scroller_pre_process, infoBox);
   setCDKScrollPostProcess (incoming_msg_scroller, incoming_msg_scroller_post_process, infoBox);
   bindCDKObject(vSCROLL, incoming_msg_scroller, 'q', incoming_msg_scroller_quit, 0);

   outgoing_msg_scroller = newCDKScroll (cdkscreen,
			      LEFT, /* x */
			      CENTER, /* y */
			      RIGHT,  /* scrollbar */
			      -4,     /* H, 0 = max  */
			      36,     /* W */
			      "</U/05>Outgoing Requests", /* title */
			      0, /* item list - list of strings or 0 */
			      0,     /* item count */
			      NUMBER_ITEMS,  /* numbered */
			      A_REVERSE, /* highlighting */
			      TRUE,	 /* boxed */
			      FALSE);	 /* shadowed */

   /* Is the scrolling list null? */
   if (outgoing_msg_scroller == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK ();

      printf ("Cannot make scrolling list. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }
   setCDKScrollPreProcess (outgoing_msg_scroller, outgoing_msg_scroller_pre_process, infoBox);
   setCDKScrollPostProcess (outgoing_msg_scroller, outgoing_msg_scroller_post_process, infoBox);

   msg_details[0] = "                                                                               ";
   msg_box = newCDKLabel (cdkscreen, CENTER, BOTTOM,
			   (CDK_CSTRING2) msg_details, 1,
			   FALSE, FALSE);
   /* Is the label null? */
   if (msg_box == 0)
   {
      /* Clean up the memory. */
      destroyCDKScreen (cdkscreen);

      /* End curses... */
      endCDK ();

      printf ("Cannot create the msg_box label. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

}

CDKMENU *menu        = 0;
char *mesg[10];
int menu_selection;
/* const char *mesg[5]; */
char temp[256];
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
    /* doesn't work: */
    /* moveCDKScroll(incoming_msg_scroller, 0, 10, /\* relative *\/ TRUE, /\* refresh *\/ FALSE);
     * moveCDKScroll(outgoing_msg_scroller, 0, 10, /\* relative *\/ TRUE, /\* refresh *\/ FALSE); */

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

    /* cbreak();			/\* ncurses: disable line buffering *\/
     * keypad(cdkscreen, TRUE); */

   /* Start curses. */
   (void) initCDKScreen (NULL);
   curs_set (0);

   /* Create a basic window. */
   subWindow = newwin (LINES - 5, COLS - 10, 2, 5);

   /* Start Cdk. */
   cdkscreen = initCDKScreen (subWindow);

    /* Start CDK color. */
    initCDKColor ();

   box (subWindow, ACS_VLINE, ACS_HLINE);
   wrefresh (subWindow);

    refreshCDKScreen (cdkscreen);

    initialize_menu();

    init_msg_log_scrollers();

    drawCDKScroll(incoming_msg_scroller, TRUE);
    drawCDKScroll(outgoing_msg_scroller, TRUE);
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

    while(1) {
	c = getchar();
	switch(c) {
	case 'q':
	    goto exit;
	    break;
	case KEY_ESC:
	    rc = select(1, &readfds, NULL, NULL, &timeout);
	    if (rc == 0) {
		OIC_LOG(DEBUG, TAG, "ESC: running action dialog");
		run_action_dialog(cdkscreen);
		/* run_menu(); */
		OIC_LOG(DEBUG, TAG, "action dialog finished");
		break;
		/* goto exit; */
	    }
	    break;
	case KEY_TAB:
	    OIC_LOG_V(DEBUG, TAG, "TAB; next: %d", next);
	    if (next == INCOMING) {
		/* if (incoming_msg_scroller) */
		/* setCDKScrollCurrentItem(incoming_msg_scroller, 1); */
		/* setCDKScrollHighlight(outgoing_msg_scroller, A_REVERSE); */
		setCDKScrollBackgroundColor(incoming_msg_scroller, "</31>");
		int sz = incoming_msg_scroller->listSize;
		OIC_LOG_V(DEBUG, TAG, "SCROLLER currentitem: %d, current top: %d",
			  getCDKScrollCurrentItem(incoming_msg_scroller),
			  getCDKScrollCurrentTop(incoming_msg_scroller));
		/* if (getCDKScrollCurrentTop(incoming_msg_scroller) != 0) { */
		int sel = sz - incoming_current_item;
		/* getCDKScrollCurrentItem(incoming_msg_scroller); */
		    char *tmp = chtype2Char (incoming_msg_scroller->item[sel]);
		    /* char *tmp = chtype2Char (incoming_msg_scroller->item[sel]); */
		    /* sprintf (msg, "<C>%s", msg_details[0]); */
		    OIC_LOG_V(DEBUG, TAG, "MSG details: %s", tmp);
		    setCDKLabelMessage(msg_box, &tmp, 1);
		    freeChar(tmp);
		/* } */
		setCDKScrollCurrentItem(incoming_msg_scroller, sz - incoming_current_item);
		OIC_LOG_V(DEBUG, TAG, "set current item to: %d", incoming_current_item);
		msg_selection = activateCDKScroll(incoming_msg_scroller, 0);
		incoming_current_item
		    = incoming_msg_scroller->listSize - getCDKScrollCurrentItem(incoming_msg_scroller);
		OIC_LOG(DEBUG, TAG, "incoming_msg_scroller EXIT");
		/* Determine how the widget was exited. */
		if (msg_selection < 0) {
		    OIC_LOG(DEBUG, TAG, "User struck ESC");
		    /* user struck ESC */
		    /* eraseCDKScroll(incoming_msg_scroller);
		     * eraseCDKScroll(outgoing_msg_scroller); */
		    setCDKScrollBackgroundColor(incoming_msg_scroller, "<!31>");
		    /* OIC_LOG_V(DEBUG, TAG, "object: %p", incoming_msg_scroller->win); */
		    /* refreshCDKScreen (cdkscreen); */
		    /* drawCDKScroll(outgoing_msg_scroller,  /\* boxed? *\/ TRUE); */
		    /* run_menu(); */
		    run_action_dialog(cdkscreen);
		    /* setCDKScrollCurrentItem(incoming_msg_scroller, 0); */
		    /* setCDKScrollBackgroundColor(incoming_msg_scroller, "</31>"); */
		    /* ungetc(ESC, stdin); */
		} else {
		    /* user struck RETURN or TAB */
		    setCDKScrollBackgroundColor(incoming_msg_scroller, "<!31>");
		    /* setCDKScrollCurrentItem(incoming_msg_scroller, 1);
		     * setCDKScrollHighlight(incoming_msg_scroller, A_REVERSE); */
		    next = OUTGOING;
		    ungetc(KEY_TAB, stdin);
		}
		/* refreshCDKScreen (cdkscreen); */
	    } else {
		/* setCDKScrollHighlight(outgoing_msg_scroller, A_REVERSE); */
		setCDKScrollBackgroundColor(outgoing_msg_scroller, "</31>");
		int sz = outgoing_msg_scroller->listSize;
		/* if (getCDKScrollCurrentTop(outgoing_msg_scroller) != 0) { */
		int sel = sz - outgoing_current_item;
		/* getCDKScrollCurrentItem(outgoing_msg_scroller); */
		    char *tmp = chtype2Char (outgoing_msg_scroller->item[sel]);
		    /* char *tmp = chtype2Char (outgoing_msg_scroller->item[sel]); */
		    /* sprintf (msg, "<C>%s", msg_details[0]); */
		    OIC_LOG_V(DEBUG, TAG, "MSG details: %s", tmp);
		    setCDKLabelMessage(msg_box, &tmp, 1);
		    freeChar(tmp);
		/* } */
		setCDKScrollCurrentItem(outgoing_msg_scroller, sz - outgoing_current_item);
		msg_selection = activateCDKScroll(outgoing_msg_scroller, 0);
		outgoing_current_item
		    = outgoing_msg_scroller->listSize - getCDKScrollCurrentItem(outgoing_msg_scroller);
		OIC_LOG(DEBUG, TAG, "outgoing_msg_scroller EXIT");
		if (msg_selection < 0) {
		    OIC_LOG(DEBUG, TAG, "User struck ESC");
		    /* eraseCDKScroll(outgoing_msg_scroller);
		     * eraseCDKScroll(outgoing_msg_scroller); */
		    /* refreshCDKScreen (cdkscreen); */
		    setCDKScrollBackgroundColor(outgoing_msg_scroller, "<!31>");
		    /* run_menu(); */
		    run_action_dialog(cdkscreen);
		    /* setCDKScrollBackgroundColor(outgoing_msg_scroller, "</31>"); */
		    /* setCDKScrollCurrentItem(outgoing_msg_scroller, 1); */
		    /* setCDKScrollHighlight(outgoing_msg_scroller, A_REVERSE); */
		} else {
		    ungetc(KEY_TAB, stdin);
		    setCDKScrollBackgroundColor(outgoing_msg_scroller, "<!31>");
		    /* setCDKScrollCurrentItem(outgoing_msg_scroller, 0); */
		    /* setCDKScrollHighlight(outgoing_msg_scroller, A_REVERSE); */
		    next = INCOMING;
		}
		/* refreshCDKScreen (cdkscreen); */
	    }
	    drawCDKScroll(incoming_msg_scroller,  /* boxed? */ TRUE);
	    drawCDKScroll(outgoing_msg_scroller,  /* boxed? */ TRUE);
	    break;
	default:
	    OIC_LOG_V(DEBUG, TAG, "GETCHAR: %c", c);
	}
    }

 exit:
    /* Clean up. */
    destroyCDKMenu (menu);
    destroyCDKLabel (infoBox);
    /* FIXME: scrollers, mutexes, etc. */
    destroyCDKScroll(incoming_msg_scroller);
    destroyCDKScroll(outgoing_msg_scroller);
    destroyCDKScroll(msg_box);
    destroyCDKScreen (cdkscreen);
    endCDK ();
    /* delscreen(scr); */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    ExitProgram (EXIT_SUCCESS);
}
