#include <cdk_test.h>
#include "openocf.h"
#include "coresource_mgr.h"

#define ENUMERATE false
#define INBOUND 0
#define OUTGOING 1
#define CORESOURCES 2

bool coresource_scroller_dirty;

static const char *msg[3];
static char temp[100];
static int selection;

static bool initialized = false;

static const char *message[MSG_MAX];
static char **msgs = NULL;
static char **s = NULL;
static int msg_count = 0;

CDKSCROLL *coresource_scroller;
int coresource_current_item;

/* static char text[MSG_MAX][80];		/\* since message is const we need a work buffer *\/ */

void reinitialize_coresource_scroller()
{
    int ct = coresource_scroller->listSize;
    for (int i=0; i< ct; i++) {
	deleteCDKScrollItem(coresource_scroller, i);
    }

    char **msgs;
    int msg_count = oocf_cosp_mgr_list_coresource_uris(&msgs);

    /* for (int i=0; i < len; i++) {
     * 	sprintf(text[i], "%s%s", msg->devAddr.addr, msg->devAddr.port, msg->resourceUri);
     * } */

    setCDKScrollItems (coresource_scroller, (CDK_CSTRING2) msgs, msg_count, ENUMERATE);
    setCDKScrollCurrentItem(coresource_scroller,
			    coresource_scroller->listSize - inbound_current_item);

    /* pthread_mutex_lock(&display_mutex); */
    drawCDKScroll(coresource_scroller,  /* boxed? */ TRUE);
    coresource_scroller_dirty = false;
}

void update_coresource_scroller(void)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self());
    int len = 0;
    char **coresource_uris = NULL;
    char **s = NULL;
    int coresource_count;
    static int rc;

    if (coresource_scroller_dirty) {
	if (coresource_uris) {
	    for (int i=0; i<coresource_count; i++) {
		OIC_LOG_V(DEBUG, TAG, "freeing coresource uri: %s", coresource_uris[i]);
		OICFree(coresource_uris[i]);
	    }
	    OICFree(coresource_uris);
	}
	/* pthread_mutex_lock(&dirty_mutex); */
	coresource_count = oocf_cosp_mgr_list_coresource_uris(&coresource_uris);
	/* pthread_mutex_unlock(&dirty_mutex); */
	OIC_LOG_V(DEBUG, TAG, "Coresource count: %d", coresource_count);
	for (int i=0; i<coresource_count; i++) {
	    OIC_LOG_V(DEBUG, TAG, "Coresource uri: %s", coresource_uris[i]);
	}
    }

    /* pthread_mutex_lock(&msgs_mutex); */
    if (coresource_scroller) {

	setCDKScrollItems (coresource_scroller,
			   (CDK_CSTRING2) coresource_uris,
			   coresource_count,
			   ENUMERATE);

	flockfile(stdout);
	setCDKScrollCurrentItem(coresource_scroller,
				coresource_scroller->listSize - coresource_current_item);

	/* pthread_mutex_lock(&display_mutex); */
/* FIXME */
	drawCDKScroll(coresource_scroller,  /* boxed? */ TRUE);
	/* pthread_mutex_unlock(&display_mutex); */

	funlockfile(stdout);
    /* } else {
     * 	OIC_LOG_V(ERROR, TAG, "coresource_scroller not initialized");
     * 	exit(EXIT_FAILURE); */
    }
    /* pthread_mutex_unlock(&msgs_mutex); */
    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

static int coresource_scroller_pre_process (EObjectType cdktype GCC_UNUSED,
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
    case KEY_ENTER:
    	OIC_LOG_V(DEBUG, TAG, "Keystroke: ENTER; running coresource_inspector");
	the_item = chtype2Char (coresource_scroller->item[getCDKScrollCurrentItem(scroller)]);
	OIC_LOG_V(DEBUG, TAG, "the_item: %s, %d", the_item, getCDKScrollCurrentItem(scroller));
	run_coresource_inspector(getCDKScrollCurrentItem(scroller));
	return 0;		/* no further action */
    	break;
    case '<':
	OIC_LOG_V(DEBUG, TAG, "Keystroke: <");
	break;
    default:
	i =  input & A_CHARTEXT;
	OIC_LOG_V(DEBUG, TAG, "chtype: 0x%X", input);
	OIC_LOG_V(DEBUG, TAG, "char: %c", (char)i);
    }


    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 1;			/* 0 = no further action */
}

static int coresource_scroller_post_process (EObjectType cdktype GCC_UNUSED,
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
	OIC_LOG_V(DEBUG, TAG, "coresource post-process, curritem: %d", coresource_current_item);
	the_item = chtype2Char (coresource_scroller->item[getCDKScrollCurrentItem(scroller)]);
	setCDKLabelMessage(msg_box, &the_item, 1);
	freeChar(the_item);

	coresource_current_item = scroller->listSize - getCDKScrollCurrentItem(scroller);

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

static int coresource_scroller_quit (EObjectType cdktype GCC_UNUSED,
			 void *object,
			 void *clientData GCC_UNUSED,
			 chtype key GCC_UNUSED)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return TRUE;
}

void initialize_coresource_scroller(void)
{
    coresource_scroller = newCDKScroll (cdkscreen,
					CENTER,                /* x */
					CENTER,                /* y */
					RIGHT,                 /* scrollbar */
					-5,                    /* H, 0 = max  */
					64,                    /* W */
					"<C></U/05>Coresource Links",  /* title */
					0,                     /* item list - list of strings or 0 */
					0,                     /* item count */
					ENUMERATE,          /* numbered */
					A_REVERSE,             /* highlighting */
					TRUE,                  /* boxed */
					FALSE);                /* shadowed */

    /* Is the scrolling list null? */
    if (coresource_scroller == 0)
	{
	    /* Exit CDK. */
	    destroyCDKScreen (cdkscreen);
	    endCDK ();

	    printf ("Cannot make coresource_scroller. Is the window too small?\n");
	    ExitProgram (EXIT_FAILURE);
	}
    setCDKScrollPreProcess (coresource_scroller, coresource_scroller_pre_process, NULL);
    setCDKScrollPostProcess (coresource_scroller, coresource_scroller_post_process, NULL);
    bindCDKObject(vSCROLL, coresource_scroller, 'q', coresource_scroller_quit, 0);
    eraseCDKScroll(coresource_scroller);
}

void browse_coresource_json(OCClientResponse *msg)
{
   CDKVIEWER *json_viewer   = 0;
   const char *button[5];

   int selected, lines;
   char **info          = 0;
   char vTitle[256];
   int interp_it;		/* interpret embedded markup */
   int link_it;			/* load file via embedded link */

    char filename[256];
    sprintf(filename, "logs/client/msg_%p.txt", msg);
    OIC_LOG_V(INFO, TAG, "BROWSING %s", filename);

   /* Create the viewer buttons. */
   button[0] = "</5><OK><!5>";
   button[1] = "</5><Cancel><!5>";

   /* Create the file viewer to view the file selected. */
   json_viewer = newCDKViewer (cdkscreen,
			   CENTER,
			   CENTER,
			   00,
			   -2,
			   (CDK_CSTRING2)button, 2, A_REVERSE,
			   TRUE,
			   FALSE);

   /* Could we create the viewer widget? */
   if (json_viewer == 0)
   {
      /* Exit CDK. */
      /* destroyCDKFselect (fSelect);
       * destroyCDKScreen (cdkscreen); */
      /* endCDK (); */

       OIC_LOG_V(FATAL, TAG, "Cannot create viewer. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

   /* if (link_it)
    * {
    *    info = (char **)calloc (2, sizeof (char *));
    *    info[0] = (char *)malloc (5 + strlen (filename));
    *    sprintf (info[0], "<F=%s>", filename);
    *    lines = -1;
    *    interp_it = TRUE;
    * }
    * else
    * { */
      setCDKViewer (json_viewer, "reading...", 0, 0, A_REVERSE, TRUE, TRUE, TRUE);
      /* Open the file and read the contents. */
      lines = CDKreadFile (filename, &info);
      if (lines == -1)
      {
	 endCDK ();
	 printf ("Could not open \"%s\"\n", filename);
	 ExitProgram (EXIT_FAILURE);
      }
   /* } */

   /* Set up the viewer title, and the contents to the widget. */
   sprintf (vTitle, "<C></B/21>Filename:<!21></22>%20s<!22!B>", filename);
   setCDKViewer (json_viewer, vTitle,
		 (CDK_CSTRING2)info, lines,
		 A_REVERSE, interp_it, TRUE, TRUE);

   CDKfreeStrings (info);

   /* Activate the viewer widget. */
   selected = activateCDKViewer (json_viewer, 0);

   /* /\* Check how the person exited from the widget. *\/
    * if (json_viewer->exitType == vESCAPE_HIT)
    * {
    *    msg[0] = "<C>Escape hit. No Button selected.";
    *    msg[1] = "";
    *    msg[2] = "<C>Press any key to continue.";
    *    popupLabel (cdkscreen, (CDK_CSTRING2)msg, 3);
    * }
    * else if (json_viewer->exitType == vNORMAL)
    * {
    *    sprintf (temp, "<C>You selected button %d", selected);
    *    msg[0] = temp;
    *    msg[1] = "";
    *    msg[2] = "<C>Press any key to continue.";
    *    popupLabel (cdkscreen, (CDK_CSTRING2)msg, 3);
    * } */

   /* Clean up. */
   destroyCDKViewer (json_viewer);
}

void run_coresource_mgr(void)
{
    static int msg_selection;
    static int sz;

    if (coresource_scroller_dirty) {
	reinitialize_coresource_scroller();
    }

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
/* FIXME */
	/* drawCDKScroll(coresource_scroller,  /\* boxed? *\/ TRUE); */
    } else {
	OIC_LOG(DEBUG, TAG, "User struck TAB");
	ungetch(KEY_TAB); // , stdin);
	setCDKScrollBackgroundColor(coresource_scroller, "<!31>");
	next_scroller = OUTGOING;
    }
/* FIXME */
    /* drawCDKScroll(coresource_scroller,  /\* boxed? *\/ TRUE); */
    drawCDKLabel(msg_box, TRUE);
    /* draw_scrollers(); */
}
