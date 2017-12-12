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

#ifdef HAVE_XCURSES
char *XCursesProgramName = "menu_ex";
#endif

CDKSCROLL *outgoing_msg_scroller;
/* int outgoing_current_item; */

CDKSCROLL *coresource_scroller;
/* int coresource_current_item; */

static char     *msg_details[1];

int outgoing_current_item   = 1;
int coresource_current_item = 1;

#define ENUMERATE false

/* CDK gui structures */
SCREEN     *screen;

/* CDKSWINDOW *request_log  = 0;
 * const char *request_log_title    = "<C></5>Request Log"; */

/* CDKSWINDOW *resources_list  = 0;
 * const char *resources_list_title    = "<C></5>Resources"; */

static int displayCallback (EObjectType cdktype, void *object,
			    void *clientData,
			    chtype input);


void reset_scroller(CDKSCROLL *scroller)
{
    int ct = scroller->listSize;
    for (int i=0; i< ct; i++) {
	deleteCDKScrollItem(scroller, i);
    }
}

 void* outgoing_msg_ui_writer(void *arg)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self());
    int len = 0;
    char **msgs = NULL;
    char **s = NULL;
    int msg_count;
    static int rc;

    while(1) {
	/* OIC_LOG(DEBUG, TAG, "waiting on outgoing_msg_semaphore"); */
	rc = sem_wait(outgoing_msg_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(ERROR, TAG, "sem_wait(&outgoing_msg_semaphore) rc: %s", strerror(errno));
	} else {
	    /* OIC_LOG_V(DEBUG, TAG, "acquired outgoing_msg_semaphore (%d)", g_quit_flag); */
	    if (g_quit_flag != 0) {
	    	/* OIC_LOG_V(ERROR, TAG, "%s: quiting", __func__); */
	    	break;
	    }
	    rc = pthread_mutex_lock(&msgs_mutex);
	    if (rc < 0) {
		OIC_LOG_V(ERROR, TAG, "mutex lock failed on msgs_mutex rc: %s", strerror(errno));
	    }
	    if (outgoing_msg_scroller) {
		/* OIC_LOG_V(DEBUG, TAG, "CONSUMER acquired outgoing_msg_semaphore"); */
		len = u_linklist_length(outgoing_msgs);
		reset_scroller(outgoing_msg_scroller);
		if (msgs) free(msgs);
		msgs = calloc(len, sizeof(char*));
		s = &(msgs[len-1]);
		/* OIC_LOG_V(INFO, TAG, ">>>>>>>>>>>>>>>> GOT outgoing msg; count: %d", len); */
		u_linklist_iterator_t *iterTable = NULL;
		u_linklist_init_iterator(outgoing_msgs, &iterTable);
		while (NULL != iterTable) {
		    *s-- = (char*) u_linklist_get_data(iterTable);
		    u_linklist_get_next(&iterTable);
		}
		flockfile(stdout);
		setCDKScrollItems (outgoing_msg_scroller, (CDK_CSTRING2) msgs, len, ENUMERATE);
		setCDKScrollCurrentItem(outgoing_msg_scroller,
					outgoing_msg_scroller->listSize - outgoing_current_item);

		/* pthread_mutex_lock(&display_mutex); */
		/* FIXME: only update if on top in z order */
		drawCDKScroll(outgoing_msg_scroller,  /* boxed? */ TRUE);
		/* pthread_mutex_unlock(&display_mutex); */

		funlockfile(stdout);
	    } else {
		OIC_LOG_V(ERROR, TAG, "outgoing_msg_scroller not initialized");
		exit(EXIT_FAILURE);
	    }
	    pthread_mutex_unlock(&msgs_mutex);
	}
	rc = sem_post(outgoing_msg_log_ready_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(DEBUG, TAG, "sem_post(&outgoing_msg_log_ready_semaphore)");
	}
    }
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
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
    /* case KEY_TAB:
     * 	OIC_LOG_V(DEBUG, TAG, "Keystroke: TAB");
     * 	break; */
    /* case 'q':
     * 	(void)injectCDKScroll(scroller, KEY_TAB);
     * 	break; */
    case KEY_UP:
    case KEY_DOWN:
	outgoing_current_item = scroller->listSize - getCDKScrollCurrentItem(scroller);
	OIC_LOG_V(DEBUG, TAG, "outgoing preprocess, curritem: %d", outgoing_current_item);

	the_item= chtype2Char (outgoing_msg_scroller->item[getCDKScrollCurrentItem(scroller)]);
	setCDKLabelMessage(msg_box, &the_item, 1);
	freeChar(the_item);

	break;
    default:
	i =  input & A_CHARTEXT;
	OIC_LOG_V(DEBUG, TAG, "chtype: 0x%X", input);
	OIC_LOG_V(DEBUG, TAG, "char: %c", (char)i);
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 0;
}

static int outgoing_msg_scroller_quit (EObjectType cdktype GCC_UNUSED,
			 void *object,
			 void *clientData GCC_UNUSED,
			 chtype key GCC_UNUSED)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return TRUE;
}

void draw_scrollers()
{
    drawCDKScroll(inbound_msg_scroller,  /* boxed? */ TRUE);
    drawCDKScroll(outgoing_msg_scroller,  /* boxed? */ TRUE);
    drawCDKScroll(coresource_scroller,    /* boxed? */ TRUE);
}

void init_msg_log_scrollers()
{
    initialize_inbound_msg_scroller();

    outgoing_msg_scroller = newCDKScroll (cdkscreen,
					  LEFT, /* x */
					  CENTER, /* y */
					  RIGHT,  /* scrollbar */
					  -5,     /* H, 0 = max  */
					  36,     /* W */
					  "<C></U/05>Outbound Request Messages", /* title */
					  0, /* item list - list of strings or 0 */
					  0,     /* item count */
					  ENUMERATE,  /* numbered */
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
    setCDKScrollPreProcess (outgoing_msg_scroller, outgoing_msg_scroller_pre_process, NULL);
    setCDKScrollPostProcess (outgoing_msg_scroller, outgoing_msg_scroller_post_process, NULL);
    bindCDKObject(vSCROLL, outgoing_msg_scroller, 'q', outgoing_msg_scroller_quit, 0);

    initialize_coresource_scroller();
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
