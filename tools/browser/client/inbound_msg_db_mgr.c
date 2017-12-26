#include <cdk_test.h>
#include <pthread.h>

#include "openocf.h"
#include "inbound_msg_db_mgr.h"

#define ENUMERATE false

/* #define INBOUND 0
 * #define OUTBOUND 1
 * #define CORESOURCES 2 */

CDKSCROLL *inbound_msg_scroller;

bool inbound_msg_scroller_dirty;

int inbound_current_item   = 1;

static CDKDIALOG *inbound_msg_mgr_dlg  = 0;
static const char *mesg[3];

static char **inbound_msgs = NULL;
static char **s = NULL;
static int inbound_msg_count = 0;
static int dlg_msg_count = 0;

/* static char temp[100]; */
static int selection;

/* FIXME: this routine is obsolete. It was intended so support
   realtime updating of the msg log. Since ncurses is not thread-safe
   that does not work so well. Now we just update the display when the
   user navigates to it. */
void* inbound_msg_ui_writer(void *arg)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self());
    static int rc;

    while(1) {
	rc = sem_post(inbound_msg_log_ready_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(ERROR, TAG, "sem_post(inbound_msg_log_ready_semaphore) rc: %s", strerror(errno));
	}
	OIC_LOG(DEBUG, TAG, "waiting on inbound_msg_semaphore");
	rc = sem_wait(inbound_msg_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(ERROR, TAG, "sem_wait(inbound_msg_semaphore) rc: %s", strerror(errno));
	} else {
	    if (g_quit_flag) {
	    	/* OIC_LOG_V(ERROR, TAG, "%s: quiting", __func__); */
	    	break;
	    }
	    pthread_mutex_lock(&msgs_mutex);
	    OIC_LOG_V(DEBUG, TAG, "CONSUMER acquired inbound_msg_semaphore");
	    /* /\* len = u_linklist_length(inbound_msgs); *\/ */
	    /* /\* len = oocf_coresource_db_count(); *\/ */
	    /* OIC_LOG_V(DEBUG, TAG, "MSG COUNT: %d", msg_count); */
	    /* if (msg_count > 0) { */
	    /* 	/\* for(int i=0; i<msg_count; i++) { *\/ */
	    /* 	OIC_LOG_V(DEBUG, TAG, "FREEing inbound msg labels"); */
	    /* 	OICFree(msgs[0]); */
	    /* 	/\* } *\/ */
	    /* 	OICFree(msgs); */
	    /* } */

	    /* reinitialize_inbound_msg_scroller(); */
	    /* msg_count = oocf_coresource_db_msg_labels(&msgs); */
	    /* if (inbound_msg_scroller != NULL) { */
	    /* 	reset_scroller(inbound_msg_scroller); */
	    /* 	flockfile(stdout); */
	    /* 	setCDKScrollItems (inbound_msg_scroller, (CDK_CSTRING2) msgs, msg_count, ENUMERATE); */
	    /* 	setCDKScrollCurrentItem(inbound_msg_scroller, */
	    /* 				inbound_msg_scroller->listSize - inbound_current_item); */

	    /* 	/\* pthread_mutex_lock(&display_mutex); *\/ */
	    /* 	/\* FIXME *\/ */
	    /* 	/\* drawCDKScroll(inbound_msg_scroller,  /\\* boxed? *\\/ TRUE); *\/ */
	    /* 	/\* pthread_mutex_unlock(&display_mutex); *\/ */

	    /* 	funlockfile(stdout); */
	    /* } */
	    pthread_mutex_unlock(&msgs_mutex);
	    /* update_coresource_scroller(); */
	}
	if (rc < 0) {
	    OIC_LOG_V(ERROR, TAG, "sem_post(inbound_msg_log_ready_semaphore) rc: %s", strerror(errno));
	} else {
	    /* OIC_LOG_V(DEBUG, TAG, "CONSUMER sem_post(inbound_msg_log_ready_semaphore)"); */
	}
    }
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

static void purge_inbound_msg_db()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    pthread_mutex_lock(&dirty_mutex);
    oocf_coresource_mgr_reset();
    inbound_msg_scroller_dirty = true;
    coresource_scroller_dirty = true;
    pthread_mutex_unlock(&dirty_mutex);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

static int inbound_msg_scroller_pre_process (EObjectType cdktype GCC_UNUSED,
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
    /* case KEY_ESC:
     * 	OIC_LOG_V(DEBUG, TAG, "Keystroke: ESC; running inbound_msg_dbm");
     * 	flockfile(stdout);
     * 	int rc = run_inbound_msg_db_mgr_dlg();
     * 	OIC_LOG_V(DEBUG, TAG, "inbound_msg_db_mgr_dlg return: %d", rc);
     * 	funlockfile(stdout);
     * 	draw_msg_scrollers();
     * 	drawCDKScroll(inbound_msg_scroller,  /\* boxed? *\/ TRUE);
     * 	switch(rc) {
     * 	case -1:			/\* ESC *\/
     * 	    OIC_LOG_V(DEBUG, TAG, "inbound_msg_db_mgr_dlg return: ESC");
     * 	    return 0;		/\* no further action - same as cancel *\/
     * 	case 0:				/\* purge *\/
     * 	    OIC_LOG_V(DEBUG, TAG, "inbound_msg_db_mgr_dlg return: purge");
     * 	    return 1;		/\* 0 = no further action *\/
     * 	    break;
     * 	case 1:			/\* cancel *\/
     * 	    OIC_LOG_V(DEBUG, TAG, "inbound_msg_db_mgr_dlg return: cancel");
     * 	    return 0;		/\* no further action *\/
     * 	    break;
     * 	case 2:			/\* quit *\/
     * 	    OIC_LOG_V(DEBUG, TAG, "inbound_msg_db_mgr_dlg return: quit");
     * 	    /\* return 0; *\/
     * 	    break;
     * 	/\* default:
     * 	 *     return 0;		/\\* remain in scroller activation *\\/ *\/
     * 	}
     * 	return 1;
     * 	break; */
    case KEY_ENTER:
    	OIC_LOG_V(DEBUG, TAG, "Keystroke: ENTER; running msg inspector");
	/* the_item = chtype2Char (inbound_msg_scroller->item[getCDKScrollCurrentItem(scroller)]);
	 * OIC_LOG_V(DEBUG, TAG, "the_item: %s", the_item); */
	run_inbound_msg_inspector(cdkscreen, getCDKScrollCurrentItem(scroller));
	return 0;
	break;
    /* case '<':
     * 	OIC_LOG_V(DEBUG, TAG, "Keystroke: <");
     * 	break; */
    /* default: */
	/* i =  input & A_CHARTEXT;
	 * OIC_LOG_V(DEBUG, TAG, "chtype: 0x%X", input);
	 * OIC_LOG_V(DEBUG, TAG, "char: %c", (char)i); */
    }


    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 1;			/* 0 = no further action */
}

static int inbound_msg_scroller_post_process (EObjectType cdktype GCC_UNUSED,
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
    /* case KEY_TAB:
     * 	OIC_LOG_V(DEBUG, TAG, "Keystroke: TAB");
     * 	break;
     * case KEY_ENTER:
     * 	OIC_LOG_V(DEBUG, TAG, "Keystroke: ENTER");
     * 	break; */
    /* case 'q':
     * 	(void)injectCDKScroll(scroller, KEY_TAB);
     * 	break; */
    case KEY_UP:
    case KEY_DOWN:
	OIC_LOG_V(DEBUG, TAG, "ARROW key, curritem: %d", inbound_current_item);
	the_item = chtype2Char(inbound_msg_scroller->item[getCDKScrollCurrentItem(scroller)]);
	sprintf(temp, "<C>%s", the_item);
	setCDKLabelMessage(msg_box, &the_item, 1);
	freeChar(the_item);

	inbound_current_item = scroller->listSize - getCDKScrollCurrentItem(scroller);

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
	i =  input & A_CHARTEXT;
	OIC_LOG_V(DEBUG, TAG, "chtype: 0x%X", input);
	OIC_LOG_V(DEBUG, TAG, "char: %c", (char)i);
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 0;
}

static int inbound_msg_scroller_quit (EObjectType cdktype GCC_UNUSED,
			 void *object,
			 void *clientData GCC_UNUSED,
			 chtype key GCC_UNUSED)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return TRUE;
}

void initialize_inbound_msg_scroller(void)
{
  inbound_msg_scroller = newCDKScroll (cdkscreen,
				       RIGHT, /* x */
				       CENTER, /* y */
				       RIGHT,  /* scrollbar */
				       -5,     /* H, 0 = max  */
				       56,     /* W */
				       "<C></U/05>Inbound Response Messages", /* title */
				       0, /* item list - list of strings or 0 */
				       0,     /* item count */
				       ENUMERATE,  /* numbered */
				       A_REVERSE, /* highlighting */
				       TRUE,	 /* boxed */
				       FALSE);	 /* shadowed */

  /* Is the scrolling list null? */
  if (inbound_msg_scroller == 0)
    {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK ();

      printf ("Cannot make inbound_msg_scroller. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
    }
  setCDKScrollPreProcess (inbound_msg_scroller, inbound_msg_scroller_pre_process, NULL);
  setCDKScrollPostProcess (inbound_msg_scroller, inbound_msg_scroller_post_process, NULL);
  /* bindCDKObject(vSCROLL, inbound_msg_scroller, 'q', inbound_msg_scroller_quit, 0); */
}

void reinitialize_inbound_msg_scroller()
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    static int rc;

    flockfile(stdout);

    destroyCDKScroll(inbound_msg_scroller);
    initialize_inbound_msg_scroller();

    inbound_msg_count = oocf_coresource_db_msg_labels(&inbound_msgs);

    for (int i=0; i<inbound_msg_count; i++) {
	OIC_LOG_V(DEBUG, TAG, "Inbound msg str: %s", inbound_msgs[i]);
    }

    setCDKScrollItems (inbound_msg_scroller, (CDK_CSTRING2) inbound_msgs, inbound_msg_count, ENUMERATE);
    setCDKScrollCurrentItem(inbound_msg_scroller,
			    inbound_msg_scroller->listSize - inbound_current_item);

    inbound_msg_scroller_dirty = false;
    funlockfile(stdout);
}

int run_inbound_msg_db_mgr(void) /* called by client_ui::run_gui */
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    static int msg_selection;
    static int sz;

    /* reinitialize_outbound_msg_scroller(); /\* FIXME: why is this needed? *\/ */
    reinitialize_inbound_msg_scroller();

    setCDKScrollBackgroundColor(inbound_msg_scroller, "</31>");
    sz = inbound_msg_scroller->listSize;
    if (sz > 0) {
	int sel = sz - inbound_current_item;
	OIC_LOG_V(DEBUG, TAG, "sz: %d, inbound current item: %d, sel: %d", sz, inbound_current_item, sel);
	char *tmp = chtype2Char (inbound_msg_scroller->item[sel]);
	OIC_LOG_V(DEBUG, TAG, "MSG details: %s", tmp);
	setCDKLabelMessage(msg_box, &tmp, 1);
	freeChar(tmp);
    }
    setCDKScrollCurrentItem(inbound_msg_scroller, sz - inbound_current_item);
    OIC_LOG_V(DEBUG, TAG, "set current item to: %d", inbound_current_item);

    draw_msg_scrollers();

    msg_selection = activateCDKScroll(inbound_msg_scroller, 0);

    inbound_current_item
	= inbound_msg_scroller->listSize - getCDKScrollCurrentItem(inbound_msg_scroller);
    /* OIC_LOG_V(DEBUG, TAG, "msg_selection: %d", msg_selection); */
    setCDKScrollBackgroundColor(inbound_msg_scroller, "<!31>");
    if (msg_selection < 0) {
	OIC_LOG_V(DEBUG, TAG, "msg_selection: %d (ESC)", msg_selection);
	/* if (g_quit_flag) */
	/*     setCDKScrollBackgroundColor(inbound_msg_scroller, "<!31>"); */
	destroy_msg_scrollers();
	refreshCDKScreen(cdkscreen);
    } else {
	OIC_LOG_V(DEBUG, TAG, "msg_selection: %d", msg_selection);
	next_scroller = OUTBOUND; // CORESOURCES;
	/* ungetch(KEY_TAB); /\* NB: this is from ncurses *\/ */
	draw_msg_scrollers();
    }
    /* drawCDKScroll(inbound_msg_scroller,  /\* boxed? *\/ TRUE); */
    /* drawCDKScroll(outbound_msg_scroller,  /\* boxed? *\/ TRUE); */
    /* drawCDKLabel(msg_box, TRUE); */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return msg_selection;
}

static void initialize_dlg(void)
{
    const char *buttons[] = {"</B/16>Purge",
			     "<C>Cancel",
			     "<C>Quit",
    };

    inbound_msg_mgr_dlg = newCDKDialog (cdkscreen,
			     CENTER,
			     CENTER,
			     (CDK_CSTRING2) mesg, dlg_msg_count,
			     (CDK_CSTRING2) buttons, 3,
			     COLOR_PAIR (2) | A_REVERSE,
			     TRUE,
			     TRUE,
			     FALSE);

    /* Check if we got a null value back. */
    if (inbound_msg_mgr_dlg == 0)
	{
	    /* Shut down Cdk. */
	    destroyCDKScreen (cdkscreen);
	    endCDK ();

	    printf ("Cannot create the dialog box. Is the window too small?\n");
	    ExitProgram (EXIT_FAILURE);
	}
}

int run_inbound_msg_db_mgr_dlg(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    mesg[0] = "<C>Inbound Message DB Manager";
    mesg[1] = "<Purge> to delete local msg database.";
    mesg[2] = "<Exit> to quit application.";
    dlg_msg_count = 3;

    initialize_dlg();

    /* flockfile(stdout); */
    selection = activateCDKDialog (inbound_msg_mgr_dlg, 0);
    /* pthread_mutex_unlock(&display_mutex); */

    /* Tell them what was selected. */
    switch(inbound_msg_mgr_dlg->exitType) {
    case vESCAPE_HIT:
	OIC_LOG_V(DEBUG, TAG, "msg db mgr exit: vESCAPE_HIT (%d)", selection);
	break;
    case vNORMAL:		/* RETURN or ENTER */
	OIC_LOG_V(DEBUG, TAG, "msg db mgr exit: vNORMAL (%d)", selection);
	switch (selection) {
	case 0:			/* purge */
	    purge_inbound_msg_db();
	    break;
	/* case 1:
	 *     sprintf (temp, "<C>You selected Cancel");
	 *     break; */
	case 2:
	    /* sprintf (temp, "<C>You selected Quit"); */
	    g_quit_flag = 1;
	    break;
	}
    }
    funlockfile(stdout);

    destroyCDKDialog(inbound_msg_mgr_dlg);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return selection;
}
