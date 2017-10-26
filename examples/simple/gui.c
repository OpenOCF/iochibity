#include "openocf.h"

#include <cdk/cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "menu_ex";
#endif

/* CDK gui structures */
SCREEN     *screen;
CDKSCREEN  *cdkscreen = 0;
CDKSWINDOW *request_log  = 0;
const char *request_log_title    = "<C></5>Request Log";

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

/*
 * This program demonstratres the Cdk menu widget.
 */
int run_gui (void)
{
   /* *INDENT-EQLS* */
   CDKLABEL *infoBox    = 0;
   CDKMENU *menu        = 0;
   int submenusize[5], menuloc[5];
   const char *mesg[5];
   char temp[256];
   int selection;

   /* FILE *fd = fopen("/dev/tty", "r+"); */
   /* SCREEN *scr = newterm(NULL, stderr, stdin);
    * cdkscreen = initCDKScreen (scr); */
    cdkscreen = initCDKScreen (NULL);

   /* Start CDK color. */
   initCDKColor ();

   /* initialize windows */
   CDK_PARAMS params;
   request_log = newCDKSwindow (cdkscreen,
				CDKparamValue (&params, 'X', CENTER),
				CDKparamValue (&params, 'Y', CENTER),
				CDKparamValue (&params, 'H', 10),
				CDKparamValue (&params, 'W', 65),
				request_log_title,
				100, /* saveLines */
				CDKparamValue (&params, 'N', TRUE),
				CDKparamValue (&params, 'S', TRUE));
   /* Is the window null. */
   if (request_log == 0)
       {
	   /* Exit CDK. */
	   destroyCDKScreen (cdkscreen);
	   endCDK ();

	   printf ("Cannot create request scrolling window. ");
	   printf ("Is the window too small?\n");
	   ExitProgram (EXIT_FAILURE);
       }

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

   /* Create the post process function. */
   setCDKMenuPostProcess (menu, displayCallback, infoBox);

   /* Do this until they hit q or escape. */
   for (;;)
       {
	   /* Draw the CDK screen. */
	   refreshCDKScreen (cdkscreen);

	   /* Activate the menu. */
	   selection = activateCDKMenu (menu, 0);

	   /* Determine how the user exited from the widget. */
	   switch (menu->exitType) {
	   case vESCAPE_HIT:
	       {
		   goto exit;
		   break;
	       }
	   case vNORMAL:
	       {
		   if (selection/100 == 0 && selection % 100 == 2) {
		       mesg[0] = "Discovering resources ...";
		       gui_discover_resources();
		       /* fflush(stdout);
		        * refreshCDKScreen (cdkscreen); */
		   } else if (selection/100 == 1 && selection % 100 == 3) {
		       mesg[0] = "Viewing resources ...";
		       gui_view_resources();
		       /* fflush(stdout);
			* refreshCDKScreen (cdkscreen); */
		   } else {
		       sprintf (temp, "</31>You selected menu #%d, submenu #%d",
				selection / 100,
				selection % 100);
		       mesg[0] = temp;
		       mesg[1] = "";
		       mesg[2] = "<C>Press any key to continue.";
		       popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
		   }
		   break;
	       }
	   default:
	       {
		   mesg[0] = "<C>Huh?. No menu item was selected.";
		   /* mesg[1] = "", */
		   printf("exitType: %d\n", menu->exitType);
		       mesg[2] = "<C>Press any key to continue.";
		   popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
		   break;
	       }
	       break;
	   }
       }
 exit:
   /* Clean up. */
   destroyCDKMenu (menu);
   destroyCDKLabel (infoBox);
   /* destroyCDKSwindow (request_log); */
   destroyCDKScreen (cdkscreen);
   endCDK ();
   /* delscreen(scr); */
   ExitProgram (EXIT_SUCCESS);
}

/*
 * This gets called after every movement.
 */
static int displayCallback (EObjectType cdktype GCC_UNUSED,
			    void *object,
			    void *clientData,
			    chtype input GCC_UNUSED)
{
   /* *INDENT-EQLS* */
   CDKMENU *menu        = (CDKMENU *)object;
   CDKLABEL *infoBox    = (CDKLABEL *)clientData;
   char *mesg[10];
   char temp[256];

   /* Recreate the label message. */
   /* sprintf (temp, "Title: %.*s",
    * 	    (int)(sizeof (temp) - 20),
    * 	    menulist[menu->currentTitle][0]);
    * mesg[0] = strdup (temp); */
   /* sprintf (temp, "Sub-Title: %.*s",
    * 	    (int)(sizeof (temp) - 20),
    * 	    menulist[menu->currentTitle][menu->currentSubtitle + 1]);
    * mesg[1] = strdup (temp);
    * mesg[2] = strdup (""); */
   sprintf (temp, "<C>%.*s",
	    (int)(sizeof (temp) - 20),
	    menuInfo[menu->currentTitle][menu->currentSubtitle + 1]);
   mesg[0] = strdup (temp);

   /* Set the message of the label. */
   setCDKLabel (infoBox, (CDK_CSTRING2) mesg, 1, TRUE);
   drawCDKLabel (infoBox, TRUE);

   freeCharList (mesg, 1);
   return 0;
}
