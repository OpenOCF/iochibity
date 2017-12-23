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

#if INTERFACE
#define MSG_MAX 40
#endif

#define INBOUND 0
#define OUTBOUND 1
#define CORESOURCES 2
int next_scroller = OUTBOUND;

static pthread_t        inbound_msg_ui_thread;
static pthread_t        outbound_msg_ui_thread;

/* CDK gui structures */
SCREEN     *screen;
/* CDKSWINDOW *request_log  = 0;
 * const char *request_log_title    = "<C></5>Request Log"; */

/* CDKSWINDOW *resources_list  = 0;
 * const char *resources_list_title    = "<C></5>Resources"; */

CDKLABEL *msg_box;
char *msg_box_msg[1];

/* static CDKMENU *menu        = 0; */
static char *mesg[10];
/* static int menu_selection; */
/* const char *mesg[5]; */
static char temp[256];

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
    sem_post(outbound_msg_semaphore);
    if (rc < 0) {
    	OIC_LOG_V(ERROR, TAG, "Q sem_post(outbound_msg_semaphore) rc: %s", strerror(errno));
    }
    OIC_LOG(DEBUG, TAG, "QQQQQQQQQQQQQQQQ; waiting on ui threads");
    pthread_join(inbound_msg_ui_thread, NULL);
    pthread_join(outbound_msg_ui_thread, NULL);
    /* pthread_mutex_lock(&msgs_mutex); */
    OIC_LOG(DEBUG, TAG, "QQQQQQQQQQQQQQQQ; cleanup");
    destroyCDKScroll(inbound_msg_scroller);
    destroyCDKScroll(outbound_msg_scroller);
    destroyCDKScroll(coresource_scroller);
    destroyCDKLabel(msg_box);
    destroyCDKScreen (cdkscreen);
    endCDK ();
    pthread_mutex_unlock(&msgs_mutex);
    ExitProgram (EXIT_SUCCESS);
}

int main_ui (void)
{
    int msg_selection;

    /* Start curses. */
    /* (void) initCDKScreen (NULL); */
    curs_set (0);
    cbreak();			/* ncurses: disable line buffering */

    /* Create a basic window. */
    /* subWindow = newwin (LINES - 5, COLS - 10, 2, 5); */

    /* Start Cdk. */
    cdkscreen = initCDKScreen (NULL);
    keypad(cdkscreen, TRUE);

    /* Start CDK color. */
    initCDKColor ();

    /* box (subWindow, ACS_VLINE, ACS_HLINE);
     * wrefresh (subWindow); */

    refreshCDKScreen (cdkscreen);

    /* initialize_menu(); */

    /* init_msg_log_scrollers();
     * erase_scrollers(); */

    /* draw_scrollers(); */

    /* msg_box_msg[0] = "                                                                                ";
     * msg_box = newCDKLabel (cdkscreen, CENTER, BOTTOM,
     * 			   (CDK_CSTRING2) msg_box_msg,
     * 			   1,
     * 			   TRUE, FALSE); */

    /* drawCDKLabel(msg_box, TRUE); */

    OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self());
    int err = pthread_create(&(inbound_msg_ui_thread), NULL, &inbound_msg_ui_writer, NULL);
    if (err != 0) {
	OIC_LOG_V(DEBUG, TAG, "Can't create inbound_msg_ui_thread :[%s]", strerror(err));
    }

    err = pthread_create(&(outbound_msg_ui_thread), NULL, &outbound_msg_ui_writer, NULL);
    if (err != 0) {
	OIC_LOG_V(DEBUG, TAG, "Can't create outbound_msg_ui_thread :[%s]", strerror(err));
    }

    /* this is vi stuff? */
    /* *INDENT-EQLS* */

    /* while(1) {
     * 	if (g_quit_flag) break;
     * 	run_menu();
     * } */

    run_menu();

    exit:
	OIC_LOG(DEBUG, TAG, "QQQQQQQQQQQQQQQQ; quiting");
	exit_ui();
	OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

