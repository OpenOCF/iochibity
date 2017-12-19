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
#include "msg_scrollers.h"

#if INTERFACE
#define INBOUND 0
#define OUTBOUND 1
#define CORESOURCES 2
#endif

/* CDKSCROLL *outbound_msg_scroller; */
/* int outbound_current_item; */

static char     *msg_details[1];

/* int outbound_current_item   = 1;
 * int coresource_current_item = 1; */

#define ENUMERATE false

/* CDK gui structures */
/* SCREEN     *screen; */

/* CDKSWINDOW *request_log  = 0;
 * const char *request_log_title    = "<C></5>Request Log"; */

/* CDKSWINDOW *resources_list  = 0;
 * const char *resources_list_title    = "<C></5>Resources"; */

/* CDKLABEL *msg_box;
 * static char *msg_box_msg[1]; */

void reset_scroller(CDKSCROLL *scroller)
{
    int ct = scroller->listSize;
    OIC_LOG_V(DEBUG, TAG, "%s scroller sz: %d", __func__, ct);
    if (ct > 0) {
	for (int i=0; i< ct; i++) {
	    deleteCDKScrollItem(scroller, i);
	}
    }
}

void draw_msg_scrollers()
{
    /* flockfile(stdout); */
    if (outbound_msg_scroller)
	drawCDKScroll(outbound_msg_scroller,  /* boxed? */ TRUE);
    if (inbound_msg_scroller)
	drawCDKScroll(inbound_msg_scroller,  /* boxed? */ TRUE);
    /* drawCDKScroll(coresource_scroller,    /\* boxed? *\/ TRUE); */
    /* funlockfile(stdout); */
}

void destroy_msg_scrollers()
{
    /* erase does not seem to work */
    destroyCDKScroll(inbound_msg_scroller);
    destroyCDKScroll(outbound_msg_scroller);
    destroyCDKLabel(msg_box);
    /* eraseCDKScroll(coresource_scroller); */
}

/* void init_msg_log_scrollers() */
/* { */
/*     initialize_inbound_msg_scroller(); */
/*     initialize_outbound_msg_scroller(); */
/*     initialize_coresource_scroller(); */

/*     msg_details[0] = "                                                                               "; */
/*     msg_box = newCDKLabel (cdkscreen, CENTER, BOTTOM, */
/* 			   (CDK_CSTRING2) msg_details, 1, */
/* 			   FALSE, FALSE); */
/*     /\* Is the label null? *\/ */
/*     if (msg_box == 0) */
/* 	{ */
/* 	    /\* Clean up the memory. *\/ */
/* 	    destroyCDKScreen (cdkscreen); */

/* 	    /\* End curses... *\/ */
/* 	    endCDK (); */

/* 	    printf ("Cannot create the msg_box label. Is the window too small?\n"); */
/* 	    ExitProgram (EXIT_FAILURE); */
/* 	} */

/* } */

void run_msg_logs (void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    int msg_selection;

    /* initialize_inbound_msg_scroller(); */
    reinitialize_inbound_msg_scroller();
    /* initialize_outbound_msg_scroller(); */
    reinitialize_outbound_msg_scroller();
    /* draw_msg_scrollers(); */
    /* drawCDKScroll(inbound_msg_scroller,  /\* boxed? *\/ TRUE);
     * drawCDKScroll(outbound_msg_scroller,  /\* boxed? *\/ TRUE); */

    msg_box_msg[0] = "                                                                                ";
    msg_box = newCDKLabel (cdkscreen, CENTER, BOTTOM,
			   (CDK_CSTRING2) msg_box_msg,
			   1,
			   TRUE, FALSE);

    drawCDKLabel(msg_box, TRUE);

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
	switch(next_scroller) {
	case INBOUND:
	    rc = run_inbound_msg_db_mgr();
	    if (rc < 0) {
		/* ESC */
		OIC_LOG_V(DEBUG, TAG, "msg_selection: %d (ESC)", rc);
		goto exit;
	    } else {
		/* TAB or ENTER */
		OIC_LOG_V(DEBUG, TAG, "msg_selection: %d", rc);
		next_scroller = OUTBOUND;
	    }
	    if (inbound_msg_scroller_dirty) {
		reinitialize_inbound_msg_scroller();
	    }
	    break;
	case OUTBOUND:
	    rc = run_outbound_msg_db_mgr();
	    if (rc < 0) {
		/* ESC */
		OIC_LOG_V(DEBUG, TAG, "msg_selection: %d (ESC)", rc);
		goto exit;
	    } else {
		/* TAB or ENTER */
		OIC_LOG_V(DEBUG, TAG, "msg_selection: %d", rc);
		next_scroller = INBOUND;
	    }
	    if (outbound_msg_scroller_dirty) {
		reinitialize_outbound_msg_scroller();
	    }
	    break;
	/* case 99: */
	/* case CORESOURCES:
	 *     run_coresource_mgr();
	 *     break; */
	/* default:
	 *     OIC_LOG_V(DEBUG, TAG, "GETCH: %c", c); */
	}
    }
 exit:
    destroy_msg_scrollers();
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return;
}

