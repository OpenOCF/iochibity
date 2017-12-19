#include <cdk_test.h>
#include "openocf.h"
#include "inbound_msg_inspector.h"

static CDKDIALOG *inbound_msg_inspector  = 0;

static bool initialized = false;

static const char *msg_ptr[MSG_MAX];
static char msg_str[MSG_MAX][80];		/* since msg_ptr is const we need a work buffer */
static int msg_count;

static char temp[100];
static int selection;

void browse_payload_json(OCClientResponse *msg)
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
    *    mesg[0] = "<C>Escape hit. No Button selected.";
    *    mesg[1] = "";
    *    mesg[2] = "<C>Press any key to continue.";
    *    popupLabel (cdkscreen, (CDK_CSTRING2)mesg, 3);
    * }
    * else if (json_viewer->exitType == vNORMAL)
    * {
    *    sprintf (temp, "<C>You selected button %d", selected);
    *    mesg[0] = temp;
    *    mesg[1] = "";
    *    mesg[2] = "<C>Press any key to continue.";
    *    popupLabel (cdkscreen, (CDK_CSTRING2)mesg, 3);
    * } */

   /* Clean up. */
   destroyCDKViewer (json_viewer);
}

static construct_msg(OCClientResponse *msg)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY; msg: %p", __func__, msg);

    static int i = 0;
    for (i=0; i<MSG_MAX; i++) {
	memset(msg_str[i], '\0', 80);
    }
    i = 0;
    sprintf(msg_str[i++], "%s", "<C></R>Inbound Response Message<!R>");
    OIC_LOG_V(DEBUG, TAG, "Address: %s", msg->devAddr.addr);

    sprintf(msg_str[i++], "Sequence nbr:     %d", msg->sequenceNumber);
    sprintf(msg_str[i++], "Remote device ID: %s", msg->devAddr.remoteId);
    sprintf(msg_str[i++], "Identity:         %s", msg->identity.id);
    sprintf(msg_str[i++], "Address:        %s:%d", msg->devAddr.addr, msg->devAddr.port);
    sprintf(msg_str[i++], "Resource URI:   %s", msg->resourceUri);
    sprintf(msg_str[i++], "Transport:      %s",
	    msg->devAddr.adapter == OC_ADAPTER_IP ? "UDP/IP"
	    : msg->devAddr.adapter == OC_ADAPTER_GATT_BTLE ? "GATT"
	    : msg->devAddr.adapter == OC_ADAPTER_RFCOMM_BTEDR ? "BREDR"
	    : msg->devAddr.adapter == OC_ADAPTER_TCP ? "TCP"
	    : msg->devAddr.adapter == OC_ADAPTER_NFC ? "NFC"
	    : "UNKNOWN");
    sprintf(msg_str[i++], "Network:        %s",
	    ( (OC_IP_USE_V4 & msg->devAddr.flags) > 0)  ? "IPv4"  /* (1 << 6) */
	    : ( (OC_IP_USE_V6 & msg->devAddr.flags) > 0)? "IPv6" /* (1 << 5) */
	    : "");

    if ((OC_IP_USE_V6 & msg->devAddr.flags) > 0) {
	sprintf(msg_str[i++], "IPv6 Scopes:    %s",
		(OC_SCOPE_LINK & msg->devAddr.flags) ? /* 0x2 */
		"Link-Local" : "FIXME"); /* FIXME */
    /* if (OC_SCOPE_INTERFACE & clientResponse->devAddr.flags) /\* 0x1 *\/
     * 	OIC_LOG_V(INFO, TAG, "\tInterface-Local"); */
    }

    sprintf(msg_str[i++], "OC Security:    %d", (msg->devAddr.flags & OC_FLAG_SECURE));
    sprintf(msg_str[i++], "CT Security:    %d", (msg->connType & CT_FLAG_SECURE));

    sprintf(msg_str[i++], "Multicast?:     %s",
	    (OC_MULTICAST & msg->devAddr.flags)? /* (1 << 7) */
	    "TRUE" : "FALSE");

    sprintf(msg_str[i++], "Header Options: %d", msg->numRcvdVendorSpecificHeaderOptions);

    uint16_t content_format = 0;
    uint16_t content_format_version = 0;
    uint16_t option_id  = 0;
    uint16_t option_len = 0;
    for (int j=0; j < msg->numRcvdVendorSpecificHeaderOptions; j++) {
	/* sprintf(msg_str[i++], "Option %d", j); */
	/* OIC_LOG_V(INFO, TAG, "    Option %d:", i); */

	/* With OCF 1.0, the version of the content (payload) format must also be negotiated.
	 * OCF 1.0 section 12.2.5 says messages must always indicate format version etc..
	 * COAP_OPTION_ACCEPT_VERSION 2049 = OCF-Accept-Content-Format-Version (set by client requesting payload)
	 * COAP_OPTION_CONTENT_VERSION 2053 = OCF-Content-Format-Version (set if msg has payload)
	 * Values:  2048 = 0x0800 = OCF version 1.0; 2112 = 0x0840 = OIC version 1.1 */

	/* So we expect two headers, one to indicate the payload format, and
	   another to indicate format version */

	/* OIC_LOG_V(INFO, TAG, "    protocol id FIXME: %d",
	 * 	  clientResponse->rcvdVendorSpecificHeaderOptions[j].protocolID); */

	option_id = msg->rcvdVendorSpecificHeaderOptions[j].optionID;
	option_len = msg->rcvdVendorSpecificHeaderOptions[j].optionLength;
	switch (option_id) {
	case  COAP_OPTION_ACCEPT:
	    /* Client and server negotiate content (payload) format using COAP header options
	     * COAP_OPTION_ACCEPT 12 and COAP_OPTION_CONTENT_VERSION; options are:
	     * application/cbor 60 = 0x3C
	     * application/vnd.ocf+cbor 10000 = 0x2710
	     * application/json 50 = 0x32
	     * see: https://www.iana.org/assignments/core-parameters/core-parameters.xhtml */
	    /* OIC_LOG_V(INFO, TAG, "    Accept (code %d)",
	     * 	      option_id); */
	    sprintf(msg_str[i++], "    Accept (code %d)", option_id);
	    /* uint */
	    break;
	case OCF_ACCEPT_CONTENT_FORMAT_VERSION:
	    sprintf(msg_str[i++], "    OCF-Accept-Content-Version-Format (code %d), len %d",
		    option_id, option_len);
	    /* OIC_LOG_V(INFO, TAG, "    OCF-Accept-Content-Version-Format (code %d), len %d",
	     * 	      option_id, option_len); */
	    /* 2 byte uint */
	    break;

	case  COAP_OPTION_CONTENT_FORMAT: /* uint */
	    content_format =
		(msg->rcvdVendorSpecificHeaderOptions[j].optionData[0] * 0x0100
		 + msg->rcvdVendorSpecificHeaderOptions[j].optionData[1]);
	    sprintf(msg_str[i++], "    Content-Format (%d) = %s (%d)",
	    /* OIC_LOG_V(INFO, TAG, "    Content-Format (%d) = %s (%d)", */
		      option_id,
		      /* option_len, */
		      (COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR
		      == content_format)? "application/vnd.ocf+cbor"
		      : (COAP_MEDIATYPE_APPLICATION_CBOR
			 == content_format_version)? "application/cbor"
		      : (COAP_MEDIATYPE_APPLICATION_JSON
			 == content_format_version)? "application/json"
		      : "(UNKNOWN)",
		       content_format);
	    break;
	    /* duplicate of COAP_OPTION_CONTENT_FORMAT: COAP_OPTION_CONTENT_TYPE */

	case OCF_CONTENT_FORMAT_VERSION:
	    /* 2 byte uint */
	    content_format_version =
		(msg->rcvdVendorSpecificHeaderOptions[j].optionData[0] * 0x0100
		 + msg->rcvdVendorSpecificHeaderOptions[j].optionData[1]);
	    sprintf(msg_str[i++], "    OCF-Content_Version-Format (%d) = %s (%d)",
	    /* OIC_LOG_V(INFO, TAG, "\t\t OCF-Content-Version-Format (%d) = %s (%d)", */
	    	      option_id,
	    	      /* option_len, */
	    	      (OCF_VERSION_1_0_0 == content_format_version)? "OCF 1.0.0"
	    	      : (OCF_VERSION_1_1_0 == content_format_version)? "OCF 1.1.0"
	    	      : "(UNKNOWN)",
	    	       content_format_version);
	    break;
	case  COAP_OPTION_IF_MATCH:
	    /* opaque */
	    /* OIC_LOG_V(INFO, TAG, "\t\t If-Match (code %d)", option_id); */
	    break;
	case  COAP_OPTION_URI_HOST:
	    /* string */
	    /* OIC_LOG_V(INFO, TAG, "\t\t Uri-Host (code %d)", option_id); */
	    break;
	case  COAP_OPTION_ETAG:
	    /* OIC_LOG_V(INFO, TAG, "\t\t ETag (code %d)",
	     * 	      option_id); */
	    /* empty */
	    break;
	case  COAP_OPTION_IF_NONE_MATCH:
	    /* OIC_LOG_V(INFO, TAG, "\t\t If-None-Match (code %d)",
	     * 	      option_id); */
	    /* empty */
	    break;
	case  COAP_OPTION_URI_PORT:
	    /* OIC_LOG_V(INFO, TAG, "\t\t Uri-Port (code %d)",
	     * 	      option_id); */
	    /* uint */
	    break;
	case  COAP_OPTION_LOCATION_PATH:
	    /* OIC_LOG_V(INFO, TAG, "\t\t Location-Path (code %d)",
	     * 	      option_id); */
	    /* string */
	    break;
	case  COAP_OPTION_URI_PATH:
	    /* OIC_LOG_V(INFO, TAG, "\t\t Uri-Path (code %d)",
	     * 	      option_id); */
	    /* string */
	    break;

	case  COAP_OPTION_MAXAGE:
	    /* OIC_LOG_V(INFO, TAG, "\t\t Max-Age (code %d)",
	     * 	      option_id); */
	    /* uint */
	    break;
	case  COAP_OPTION_URI_QUERY:
	    OIC_LOG_V(INFO, TAG, "\t\t Uri-Query (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_LOCATION_QUERY:
	    OIC_LOG_V(INFO, TAG, "\t\t Location-Query (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_PROXY_URI:
	    OIC_LOG_V(INFO, TAG, "\t\t Proxy-Uri (code %d)",
		      option_id);
	    /* string */
	case  COAP_OPTION_PROXY_SCHEME:
	    OIC_LOG_V(INFO, TAG, "\t\t Proxy-Scheme (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_SIZE1:
	    OIC_LOG_V(INFO, TAG, "\t\t Size1 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_SIZE2:
	    /* OIC_LOG_V(INFO, TAG, "\t\t Size2 (code %d)",
	     * 	      option_id); */
	    /* uint */
	    break;
	case  COAP_OPTION_OBSERVE:
	    /* OIC_LOG_V(INFO, TAG, "\t\t Observe (code %d)",
	     * 	      option_id); */
	    /* empty/uint */
	    break;
	    /* duplicate of COAP_OPTON_OBSERVE */
	    /* case  COAP_OPTION_SUBSCRIPTION:
	     *     OIC_LOG_V(INFO, TAG, "\t\t Observe (code %d)",
	     * 	  option_id);
	     *     /\* empty/uint *\/
	     *     break; */
	case  COAP_OPTION_BLOCK2:
	    /* OIC_LOG_V(INFO, TAG, "\t\t Block2 (code %d)",
	     * 	      option_id); */
	    /* uint */
	    break;
	case  COAP_OPTION_BLOCK1:
	    /* OIC_LOG_V(INFO, TAG, "\t\t Block1 (code %d)",
	     * 	      option_id); */
	    /* uint */
	    break;
	case  COAP_MAX_OPT:
	    /* OIC_LOG_V(INFO, TAG, "\t\t Max-Opt (code %d)",
	     * 	      option_id); */
	    break;
	/* default: */
	    /* OIC_LOG_V(INFO, TAG, "\t\t UNKOWN (code %d)",
	     * 	      option_id); */
	}
	/* for (int k = 0; k < msg->rcvdVendorSpecificHeaderOptions[j].optionLength; k++) {
	 *     OIC_LOG_V(INFO, TAG, "\t\t datum[%d]: 0x%X", k,
	 * 	      msg->rcvdVendorSpecificHeaderOptions[j].optionData[k]); */
    }

    sprintf(msg_str[i++], "Payload type: %s",
	    (msg->payload->type ==  PAYLOAD_TYPE_DISCOVERY) ? "DISCOVERY"
	    : (msg->payload->type == PAYLOAD_TYPE_DEVICE) ? "DEVICE"
	    : (msg->payload->type == PAYLOAD_TYPE_PLATFORM) ? "PLATFORM"
	    : (msg->payload->type == PAYLOAD_TYPE_REPRESENTATION) ? "REPRESENTATION"
	    : (msg->payload->type == PAYLOAD_TYPE_SECURITY) ? "SECURITY"
	    : (msg->payload->type == PAYLOAD_TYPE_PRESENCE) ? "PRESENCE"
	    : (msg->payload->type == PAYLOAD_TYPE_DIAGNOSTIC) ? "DIAGNOSTIC"
	    : (msg->payload->type == PAYLOAD_TYPE_INTROSPECTION) ? "INTROSPECTION"
	    : (msg->payload->type == PAYLOAD_TYPE_INVALID) ? "INVALID"
	    : "UNKNOWN");

    msg_count = i;

    for (i=0; i < MSG_MAX; i++) { msg_ptr[i] = msg_str[i]; }

}

static void initialize()
{
    const char *buttons[] = {"</B/16>Browse Payload",
			     "<C>Cancel",
			     "<C>Help",
    };

    inbound_msg_inspector = newCDKDialog (cdkscreen,
			     CENTER,
			     CENTER,
			     (CDK_CSTRING2) msg_ptr, msg_count,
			     (CDK_CSTRING2) buttons, 3,
			     COLOR_PAIR (2) | A_REVERSE,
			     TRUE,
			     TRUE,
			     FALSE);

    /* Check if we got a null value back. */
    if (inbound_msg_inspector == 0)
	{
	    /* Shut down Cdk. */
	    destroyCDKScreen (cdkscreen);
	    endCDK ();

	    printf ("Cannot create the dialog box. Is the window too small?\n");
	    ExitProgram (EXIT_FAILURE);
	}
}

/* bindCDKObject (vDIALOG, inbound_msg_inspector, '?', dialogHelpCB, 0); */

int run_inbound_msg_inspector(CDKSCREEN *cdkscreen, int index)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY; index: %d", __func__, index);

    /* char *ptrstr = strrchr(item, ' ') + 1;
     * char *endptr;
     * unsigned long val = strtoul(ptrstr, &endptr, 16);
     * OIC_LOG_V(INFO, TAG, "MSG ID: %p", (OCClientResponse*)val);
     * OCClientResponse *msg = (OCClientResponse*) val;
     * OIC_LOG_V(INFO, TAG, "MSG ptr: %p", msg);
     * OIC_LOG_V(INFO, TAG, "resourceUri: %s", msg->resourceUri); */

    OCClientResponse *msg = oocf_coresource_db_mgr_get_message(index);
    OIC_LOG_V(DEBUG, TAG, "MSG retrieved: %p", msg);

    construct_msg(msg);
    initialize();

    /* pthread_mutex_lock(&display_mutex); */
    flockfile(stdout);
    selection = activateCDKDialog (inbound_msg_inspector, 0);
    /* pthread_mutex_unlock(&display_mutex); */

    /* Tell them what was selected. */
    if (inbound_msg_inspector->exitType == vESCAPE_HIT)
	{
	    OIC_LOG_V(DEBUG, TAG, "%s ESC", __func__);
	    /* mesg[0] = msg->resourceUri;
	     * mesg[1] = "";
	     * mesg[2] = "<C>Press any key to continue.";
	     * popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3); */
	}
    else if (inbound_msg_inspector->exitType == vNORMAL)
	{
	    switch (selection) {
	    case 0:
		sprintf (temp, "<C>You selected Browse");
		browse_payload_json(msg);
		break;
	    /* case 1:
	     * 	sprintf (temp, "<C>You selected Cancel");
	     * 	break; */
	    case 2:
		sprintf (temp, "<C>You selected Help");
		break;
	    }
	    /* OIC_LOG_V(DEBUG, TAG, "%s NORMAL", __func__);
	     * sprintf (temp, "<C>You selected button #%d", selection); */
	    /* mesg[0] = temp;
	     * mesg[1] = "";
	     * mesg[2] = "<C>Press any key to continue.";
	     * popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3); */
	}
    funlockfile(stdout);

    /* eraseCDKDialog(inbound_msg_inspector); */
    destroyCDKDialog(inbound_msg_inspector);
    draw_msg_scrollers();
    /* Clean up. */
    /* destroyCDKDialog (inbound_msg_inspector);
     * destroyCDKScreen (cdkscreen); */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}
