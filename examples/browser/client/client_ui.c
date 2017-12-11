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

static pthread_t        inbound_msg_ui_thread;
static pthread_t        outgoing_msg_ui_thread;

/* CDK gui structures */
SCREEN     *screen;
/* CDKSWINDOW *request_log  = 0;
 * const char *request_log_title    = "<C></5>Request Log"; */

/* CDKSWINDOW *resources_list  = 0;
 * const char *resources_list_title    = "<C></5>Resources"; */

CDKLABEL *msg_box;
static char *msg_box_msg[1];

/* static CDKMENU *menu        = 0; */
static char *mesg[10];
/* static int menu_selection; */
/* const char *mesg[5]; */
static static char temp[256];

static int displayCallback (EObjectType cdktype, void *object,
			    void *clientData,
			    chtype input);

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
    /* char **resource_uris = list_resource_uris(); */

    /* Draw the scrolling window. */
    /* drawCDKSwindow (request_log, ObjOf (request_log)->box); */

    /* Load up the scrolling window. */
    /* for each uri ...*/
    /* addCDKSwindow (request_log, "<C></11>Sent DiscoverResource request", TOP); */

    /* Activate the scrolling window. */
    /* activateCDKSwindow (request_log, 0); */
}

WINDOW *subWindow;

void exit_ui(void)
{
    /* Clean up. */
    int rc;

    rc = sem_post(quit_semaphore);
    if (rc < 0) {
    	OIC_LOG_V(ERROR, TAG, "Q sem_post(quit_semaphore) rc: %s", strerror(errno));
    }
    sem_post(inbound_msg_semaphore);
    if (rc < 0) {
    	OIC_LOG_V(ERROR, TAG, "Q sem_post(inbound_msg_semaphore) rc: %s", strerror(errno));
    }
    sem_post(outgoing_msg_semaphore);
    if (rc < 0) {
    	OIC_LOG_V(ERROR, TAG, "Q sem_post(outgoing_msg_semaphore) rc: %s", strerror(errno));
    }
    OIC_LOG(DEBUG, TAG, "QQQQQQQQQQQQQQQQ; waiting on ui threads");
    pthread_join(inbound_msg_ui_thread, NULL);
    pthread_join(outgoing_msg_ui_thread, NULL);
    /* pthread_mutex_lock(&msgs_mutex); */
    OIC_LOG(DEBUG, TAG, "QQQQQQQQQQQQQQQQ; cleanup");
    destroyCDKScroll(inbound_msg_scroller);
    destroyCDKScroll(outgoing_msg_scroller);
    destroyCDKScroll(coresource_scroller);
    destroyCDKLabel(msg_box);
    destroyCDKScreen (cdkscreen);
    endCDK ();
    pthread_mutex_unlock(&msgs_mutex);
    ExitProgram (EXIT_SUCCESS);
}

#define INBOUND 0
#define OUTGOING 1
#define CORESOURCES 2
int next_scroller = OUTGOING;

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
    int err = pthread_create(&(inbound_msg_ui_thread), NULL, &inbound_msg_ui_writer, NULL);
    if (err != 0) {
	OIC_LOG_V(DEBUG, TAG, "Can't create inbound_msg_ui_thread :[%s]", strerror(err));
    }
    OIC_LOG_V(INFO, TAG, "%s FOO", __func__);
    err = pthread_create(&(outgoing_msg_ui_thread), NULL, &outgoing_msg_ui_writer, NULL);
    if (err != 0) {
	OIC_LOG_V(DEBUG, TAG, "Can't create outgoing_msg_ui_thread :[%s]", strerror(err));
    }

    /* this is vi stuff? */
    /* *INDENT-EQLS* */

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
	    switch(next_scroller) {
	    case INBOUND:
		run_inbound_msg_db_mgr();
		OIC_LOG_V(DEBUG, TAG, "returned from run_inbound_msg_db_mgr; dirty? %d",
			inbound_msg_scroller_dirty);
		if (inbound_msg_scroller_dirty) {
		    reinitialize_inbound_msg_scroller();
		}
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
		drawCDKScroll(inbound_msg_scroller,  /* boxed? */ TRUE);
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
		    next_scroller = INBOUND;
		}
		draw_scrollers();
		drawCDKScroll(inbound_msg_scroller,  /* boxed? */ TRUE);
		drawCDKScroll(outgoing_msg_scroller,  /* boxed? */ TRUE);
		break;
	    case CORESOURCES:
		run_coresource_mgr();

	    break;
	default:
	    OIC_LOG_V(DEBUG, TAG, "GETCH: %c", c);
	}
    }

 exit:
    OIC_LOG(DEBUG, TAG, "QQQQQQQQQQQQQQQQ; quiting");
    exit_ui();
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

