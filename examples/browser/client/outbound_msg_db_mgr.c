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
#include "outbound_msg_db_mgr.h"

CDKSCROLL *outbound_msg_scroller;

bool outbound_msg_scroller_dirty;

/* CDKSCROLL *coresource_scroller; */
/* int coresource_current_item; */

static char **msgs = NULL;
static char **s = NULL;
static int msg_count = 0;

static char     *msg_details[1];

int outbound_current_item   = 1;
/* int coresource_current_item = 1; */

#define ENUMERATE false

/* CDK gui structures */
/* SCREEN     *screen; */

/* CDKSWINDOW *request_log  = 0;
 * const char *request_log_title    = "<C></5>Request Log"; */

/* CDKSWINDOW *resources_list  = 0;
 * const char *resources_list_title    = "<C></5>Resources"; */

/* FIXME: this routine is obsolete. It was intended so support
   realtime updating of the msg log. Since ncurses is not thread-safe
   that does not work so well. Now we just update the display when the
   user navigates to it. */
void* outbound_msg_ui_writer(void *arg)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self());
    int len = 0;
    char **msgs = NULL;
    char **s = NULL;
    int msg_count;
    static int rc;

    while(1) {
	OIC_LOG(DEBUG, TAG, "waiting on outbound_msg_semaphore");
	rc = sem_wait(outbound_msg_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(ERROR, TAG, "sem_wait(&outbound_msg_semaphore) rc: %s", strerror(errno));
	} else {
	    OIC_LOG_V(DEBUG, TAG, "acquired outbound_msg_semaphore (%d)", g_quit_flag);
	    if (g_quit_flag != 0) {
	    	/* OIC_LOG_V(ERROR, TAG, "%s: quiting", __func__); */
	    	break;
	    }
	    rc = pthread_mutex_lock(&msgs_mutex);
	    if (rc < 0) {
		OIC_LOG_V(ERROR, TAG, "mutex lock failed on msgs_mutex rc: %s", strerror(errno));
	    }
	    /* OIC_LOG_V(DEBUG, TAG, "CONSUMER acquired outbound_msg_semaphore"); */
	    len = u_linklist_length(outbound_msgs);
	    if (msgs) free(msgs);
	    msgs = calloc(len, sizeof(char*));
	    s = &(msgs[len-1]);
	    /* OIC_LOG_V(INFO, TAG, ">>>>>>>>>>>>>>>> GOT outbound msg; count: %d", len); */
	    u_linklist_iterator_t *iterTable = NULL;
	    u_linklist_init_iterator(outbound_msgs, &iterTable);
	    while (NULL != iterTable) {
		*s-- = (char*) u_linklist_get_data(iterTable);
		u_linklist_get_next(&iterTable);
	    }
	    /* if (outbound_msg_scroller) { */
	    /* 	reset_scroller(outbound_msg_scroller); */
	    /* reinitialize_outbound_msg_scroller(); */
	    /* flockfile(stdout); */
	    /* setCDKScrollItems (outbound_msg_scroller, (CDK_CSTRING2) msgs, len, ENUMERATE); */
	    /* setCDKScrollCurrentItem(outbound_msg_scroller, */
	    /* 			    outbound_msg_scroller->listSize - outbound_current_item); */

	    /* funlockfile(stdout); */
	    /* } */
	    pthread_mutex_unlock(&msgs_mutex);
	}
	rc = sem_post(outbound_msg_log_ready_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(DEBUG, TAG, "sem_post(&outbound_msg_log_ready_semaphore)");
	}
    }
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

static int outbound_msg_scroller_pre_process (EObjectType cdktype GCC_UNUSED,
			      void *object,
			      void *clientData,
			      chtype input GCC_UNUSED)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 1;			/* 0 = no further action */
}

static int outbound_msg_scroller_post_process (EObjectType cdktype GCC_UNUSED,
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
	outbound_current_item = scroller->listSize - getCDKScrollCurrentItem(scroller);
	OIC_LOG_V(DEBUG, TAG, "outbound preprocess, curritem: %d", outbound_current_item);

	the_item= chtype2Char (outbound_msg_scroller->item[getCDKScrollCurrentItem(scroller)]);
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

static int outbound_msg_scroller_quit (EObjectType cdktype GCC_UNUSED,
			 void *object,
			 void *clientData GCC_UNUSED,
			 chtype key GCC_UNUSED)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return TRUE;
}

void initialize_outbound_msg_scroller(void)
{
    outbound_msg_scroller = newCDKScroll (cdkscreen,
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
    if (outbound_msg_scroller == 0)
	{
	    /* Exit CDK. */
	    destroyCDKScreen (cdkscreen);
	    endCDK ();

	    printf ("Cannot make scrolling list. Is the window too small?\n");
	    ExitProgram (EXIT_FAILURE);
	}
    setCDKScrollPreProcess (outbound_msg_scroller, outbound_msg_scroller_pre_process, NULL);
    setCDKScrollPostProcess (outbound_msg_scroller, outbound_msg_scroller_post_process, NULL);
    /* bindCDKObject(vSCROLL, outbound_msg_scroller, 'q', outbound_msg_scroller_quit, 0); */
}

void reinitialize_outbound_msg_scroller()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    static int rc;

    flockfile(stdout);
    destroyCDKScroll(outbound_msg_scroller);
    initialize_outbound_msg_scroller();
    /* erase_msg_scrollers(); */

    msg_count = u_linklist_length(outbound_msgs);
    if (msgs) free(msgs);
    msgs = calloc(msg_count, sizeof(char*));
    s = &(msgs[msg_count-1]);
    /* OIC_LOG_V(INFO, TAG, ">>>>>>>>>>>>>>>> GOT outbound msg; count: %d", msg_count); */
    u_linklist_iterator_t *iterTable = NULL;
    u_linklist_init_iterator(outbound_msgs, &iterTable);
    while (NULL != iterTable) {
	*s-- = (char*) u_linklist_get_data(iterTable);
	u_linklist_get_next(&iterTable);
    }
    /* if (outbound_msg_scroller) { */
	/* reset_scroller(outbound_msg_scroller); */
	/* reinitialize_outbound_msg_scroller(); */
	/* flockfile(stdout); */
	setCDKScrollItems (outbound_msg_scroller, (CDK_CSTRING2) msgs, msg_count, ENUMERATE);
	setCDKScrollCurrentItem(outbound_msg_scroller,
				outbound_msg_scroller->listSize - outbound_current_item);
    /* } */
    /* char **msgs; */
    /* int msg_count = oocf_coresource_db_msg_labels(&msgs); */

    /* setCDKScrollItems (outbound_msg_scroller, (CDK_CSTRING2) msgs, msg_count, ENUMERATE);
     * setCDKScrollCurrentItem(outbound_msg_scroller,
     * 			    outbound_msg_scroller->listSize - outbound_current_item); */

    /* pthread_mutex_lock(&display_mutex); */
    /* drawCDKScroll(outbound_msg_scroller,  /\* boxed? *\/ TRUE); */
    outbound_msg_scroller_dirty = false;
    funlockfile(stdout);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

int run_outbound_msg_db_mgr(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    static int msg_selection;
    static int sz;

    reinitialize_outbound_msg_scroller();

    setCDKScrollBackgroundColor(outbound_msg_scroller, "</31>");

    sz = outbound_msg_scroller->listSize;
    if (sz > 0) {
	int sel = sz - outbound_current_item;
	char *tmp = chtype2Char (outbound_msg_scroller->item[sel]);
	OIC_LOG_V(DEBUG, TAG, "MSG details: %s", tmp);
	setCDKLabelMessage(msg_box, &tmp, 1);
	freeChar(tmp);
    }
    setCDKScrollCurrentItem(outbound_msg_scroller, sz - outbound_current_item);
    OIC_LOG_V(DEBUG, TAG, "set current item to: %d", outbound_current_item);
    /* fflush(stdout); */
    /* pthread_mutex_lock(&display_mutex); */
    /* drawCDKScroll(inbound_msg_scroller,  /\* boxed? *\/ TRUE); */

    msg_selection = activateCDKScroll(outbound_msg_scroller, 0);
    /* pthread_mutex_unlock(&display_mutex); */

    outbound_current_item
	= outbound_msg_scroller->listSize - getCDKScrollCurrentItem(outbound_msg_scroller);
    OIC_LOG_V(DEBUG, TAG, "outbound_msg_scroller item: %d", outbound_current_item);
    setCDKScrollBackgroundColor(outbound_msg_scroller, "<!31>");
    if (msg_selection < 0) {
	OIC_LOG(DEBUG, TAG, "User struck ESC");
	destroy_msg_scrollers();
	refreshCDKScreen(cdkscreen);
    } else {
	OIC_LOG(DEBUG, TAG, "User struck TAB");
	/* ungetch(KEY_TAB); // , stdin); */
	next_scroller = INBOUND;
	draw_msg_scrollers();
    }
    /* if (outbound_msg_scroller_dirty) { */
    /* 	reinitialize_outbound_msg_scroller(); */
    /* } */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return msg_selection;
}


