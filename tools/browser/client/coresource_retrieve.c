#include <cdk_test.h>
#include <coap/pdu.h>
#include "openocf.h"
#include "coresource_retrieve.h"

#define SUBMIT 0
#define CANCEL 1
#define HELP   2

#define DEFAULT_CONTEXT_VALUE 0x99

static CDKDIALOG *dlg_retrieve  = 0;

static char *urlmsg[MSG_MAX];
static char urlbuffer[8192];
static int urlmsg_count = 0;

static char *epstrings[MSG_MAX];
static char epbuffer[8192];
static int ep_count = 0;
static int selected_ep = 0;

/* usually a resource will have 1-4 eps; we alloc more than we need */
static OCEndpointPayload *eps[8];

/*
 * tps (Transport Protocol Suite) map
 * map tps string from OCEndpointPayload to OCTransportAddapter value
 * needed for OCCDevAddr in send msgs
 */
struct tps_map {
    char *tps_string;
    int adapter;
};
static struct tps_map tps_maps[] = {
    {"coap", OC_ADAPTER_IP},	/* UDP/IP */
    {"coaps", OC_ADAPTER_IP},	/* UDP/IP */
    {"coap+tcp", OC_ADAPTER_TCP},
    {"coaps+tcp", OC_ADAPTER_TCP},
    /* {"http", OC_ADAPTER_TCP}, ??? */
    /* {"https", OC_ADAPTER_TCP} ??? */
};
static const int tps_maps_count = 4;

static char text[MSG_MAX][80];		/* since message is const we need a work buffer */

/* static char *header[MSG_MAX];
 * static int    header_count; */

static char temp[100];

static OCResourcePayload *resource;
static int ocf_version;

static int radioCB (EObjectType cdktype GCC_UNUSED,
		    void *object GCC_UNUSED,
		    void *clientData, chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   (void)injectCDKButtonbox (buttonbox, key);
   return (TRUE);
}

static int show_retrieve_dlg()
{
    CDKLABEL *urlbox;
    /* const char *mesg[10]; */

    CDKBUTTONBOX *buttonWidget   = 0;
    const char *buttons[3] = {
	"<C>SUBMIT",
	"<C>Cancel",
	"<C>Help",
    };
    CDKRADIO     *radio;

    /* the url label box. */
    urlbox = newCDKLabel (cdkscreen, CENTER, CENTER,
			(CDK_CSTRING2) urlmsg, urlmsg_count,
			TRUE, FALSE);

    /* Is the label null? */
    if (urlbox == 0)
	{
	    /* Clean up the memory. */
	    destroyCDKScreen (cdkscreen);

	    /* End curses... */
	    endCDK ();

	    printf ("Cannot create the label. Is the window too small?\n");
	    ExitProgram (EXIT_FAILURE);
	}
    setCDKLabelLLChar (urlbox, ACS_LTEE);
    setCDKLabelLRChar (urlbox, ACS_RTEE);

    refreshCDKScreen (cdkscreen);

    /* Create the endpoint radio list. */
    for (int i=0; i<ep_count; i++) {
	OIC_LOG_V(INFO, TAG, "ep %d: %s", i, epstrings[i]);
    }

    radio = newCDKRadio (cdkscreen,
			 CENTER, /* x pos */
			 getbegy(urlbox->win) + urlbox->boxHeight - 1, /* y pos */
			 NONE,	 /* scroller pos */
			 ep_count + 5,	 /* height */
			 urlbox->boxWidth - 5, /* width */
			 "</U>Select an Endpoint:<!U>\n", /* title */
			 (CDK_CSTRING2)epstrings, ep_count,
			 '#' | A_REVERSE, /* choice char */
			 0,		 /* default item? */
			 A_REVERSE, /* choice highlight? */
			 TRUE,	   /* box */
			 FALSE);   /* shadow */

    /* Check if the radio list is NULL. */
    if (radio == (CDKRADIO *)NULL)
	{
	    /* Exit CDK. */
	    destroyCDKScreen (cdkscreen);
	    endCDK ();

	    printf ("Cannot create the radio widget. ");
	    printf ("Is the window too small?\n");
	    ExitProgram (EXIT_FAILURE);
	}

    /* Create the button box widget. */
    buttonWidget = newCDKButtonbox (cdkscreen,
				    getbegx (radio->win),
				    getbegy (radio->win) + radio->boxHeight - 1,
				    1, radio->boxWidth - 1,
				    0, /* title */
				    1, /* rows */
				    3, /* cols */
				    (CDK_CSTRING2) buttons, 3,
				    A_REVERSE,
				    TRUE, FALSE);
    if (buttonWidget == 0)
	{
	    destroyCDKScreen (cdkscreen);
	    endCDK ();

	    fprintf (stderr, "Cannot create buttonbox-widget\n");
	    ExitProgram (EXIT_FAILURE);
	}

    /* Set the lower left and right characters of the box. */
    setCDKRadioULChar (radio, ACS_LTEE);
    setCDKRadioURChar (radio, ACS_RTEE);
    setCDKRadioLLChar (radio, ACS_LTEE);
    setCDKRadioLRChar (radio, ACS_RTEE);
    setCDKButtonboxULChar (buttonWidget, ACS_LTEE);
    setCDKButtonboxURChar (buttonWidget, ACS_RTEE);

    /*
     * Bind the Tab key in the radio field to send a
     * Tab key to the button box widget.
     */
    bindCDKObject (vRADIO, radio, KEY_TAB, radioCB, buttonWidget);

    drawCDKButtonbox (buttonWidget, TRUE);
    /* Activate the radio box. */
    selected_ep = activateCDKRadio (radio, 0);
    OIC_LOG_V(INFO, TAG, "selected ep: %d", selected_ep);
    int action = buttonWidget->currentButton;
    destroyCDKLabel(urlbox);
    destroyCDKRadio(radio);
    destroyCDKButtonbox(buttonWidget);
    return action;
}

/* messages for cdk must be passed as an array of char ptrs. so we
   construct one buffer holding all the strings in sequence (instead
   of allocating each string separately), and to go with it an array
   of pointers to those strings. the latter will be passed to the cdk
   widget*/

/* index: index into the coresource db maintained by the engine */
/* ptrs: [i/o] array of char* passed in; we fill it in */
/* buffer: [i/o] char buffer passed in, we insert the strings */
/* count: number of msg strings (ptrs) */
int get_url(char *ptrs[], char *buffer, int *count)
{
    int i = 0;

    ptrs[i++] = buffer;
    sprintf(buffer, "<C></R>Retrieve Coresource<!R>");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    sprintf(buffer, "");
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    sprintf(buffer, " GET %s%s ", resource->anchor, resource->uri);
    buffer += strlen(buffer) + 1;
    ptrs[i++] = buffer;

    /* Common Properties for all resources: rt, if, n, id */

    /* OIC_LOG_V(INFO, TAG, "instance: %d", i);
     * sprintf(buffer, "ins: %d", i + 1);
     * ptrs[i++] = buffer;
     * buffer += strlen(buffer) + 1; */

    *count = i - 1;
    return 0;
}

int get_eps()
{
    char *buffer = epbuffer;
    memset(buffer, '\0', 8192);
    int i = 0;
    ep_count = 0;

    if ( ocf_version == OCF_VERSION_1_0_0 || ocf_version == OCF_VERSION_1_1_0) {
    	OCEndpointPayload *endpoint = resource->eps;
    	while(endpoint) {
	    eps[i] = endpoint;
    	    int eplen = strlen(endpoint->tps)
    		+ 3		/* :// */
    		+ strlen(endpoint->addr)
    		+ 10				/* "pri: %d" */
    		+ 1; 		/* : */
    	    /* char *epstring = malloc(eplen + 6); /\* largest val for port is 5 chars (uint16) *\/ */
	    if (endpoint->family == OC_IP_USE_V6) { /* (1 << 5) */
		snprintf(buffer, eplen + 6, " %s://[%s]:%d ",
			 endpoint->tps, endpoint->addr, endpoint->port);
		/* snprintf(buffer, eplen + 6, "\t%s://[%s]:%d pri: %d", */
		/* 	 endpoint->tps, endpoint->addr, endpoint->port, endpoint->pri); */
	    } else {
		snprintf(buffer, eplen + 6, " %s://%s:%d ",
			 endpoint->tps, endpoint->addr, endpoint->port);
	    }
	    OIC_LOG_V(INFO, TAG, "ep %d: %s", i, buffer);
    	    endpoint = endpoint->next;
    	    epstrings[i++] = buffer;
    	    buffer += strlen(buffer) + 1;
    	}
    } else {
	OIC_LOG_V(ERROR, TAG, "Unsupported OCF version %d", ocf_version);
	/* exit(EXIT_FAILURE); */
    }
    ep_count = i;
    return 0;
}

static const char *oc_result_to_str(OCStackResult result)
{
    switch (result)
    {
        case OC_STACK_OK:
            return "OC_STACK_OK";
        case OC_STACK_RESOURCE_CREATED:
            return "OC_STACK_RESOURCE_CREATED";
        case OC_STACK_RESOURCE_DELETED:
            return "OC_STACK_RESOURCE_DELETED";
        case OC_STACK_RESOURCE_CHANGED:
            return "OC_STACK_RESOURCE_CHANGED";
        case OC_STACK_INVALID_URI:
            return "OC_STACK_INVALID_URI";
        case OC_STACK_INVALID_QUERY:
            return "OC_STACK_INVALID_QUERY";
        case OC_STACK_INVALID_IP:
            return "OC_STACK_INVALID_IP";
        case OC_STACK_INVALID_PORT:
            return "OC_STACK_INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
            return "OC_STACK_INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
            return "OC_STACK_INVALID_METHOD";
        case OC_STACK_NO_MEMORY:
            return "OC_STACK_NO_MEMORY";
        case OC_STACK_COMM_ERROR:
            return "OC_STACK_COMM_ERROR";
        case OC_STACK_INVALID_PARAM:
            return "OC_STACK_INVALID_PARAM";
        case OC_STACK_NOTIMPL:
            return "OC_STACK_NOTIMPL";
        case OC_STACK_NO_RESOURCE:
            return "OC_STACK_NO_RESOURCE";
        case OC_STACK_RESOURCE_ERROR:
            return "OC_STACK_RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
            return "OC_STACK_SLOW_RESOURCE";
        case OC_STACK_NO_OBSERVERS:
            return "OC_STACK_NO_OBSERVERS";
        case OC_STACK_UNAUTHORIZED_REQ:
            return "OC_STACK_UNAUTHORIZED_REQ";
        case OC_STACK_NOT_ACCEPTABLE:
            return "OC_STACK_NOT_ACCEPTABLE";
#ifdef WITH_PRESENCE
        case OC_STACK_PRESENCE_STOPPED:
            return "OC_STACK_PRESENCE_STOPPED";
        case OC_STACK_PRESENCE_TIMEOUT:
            return "OC_STACK_PRESENCE_TIMEOUT";
#endif
        case OC_STACK_ERROR:
            return "OC_STACK_ERROR";
        default:
            return "UNKNOWN";
    }
}

OCStackApplicationResult get_cb(void* ctx, OCDoHandle handle,
				OCClientResponse * clientResponse)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    if (clientResponse == NULL)
    {
        OIC_LOG(INFO, TAG, "getReqCB received NULL clientResponse");
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (clientResponse->result != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "StackResult: %s",  oc_result_to_str(clientResponse->result));
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OIC_LOG(INFO, TAG, "Callback Context for GET query recvd successfully");
    }

    OIC_LOG_V(INFO, TAG, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
    OIC_LOG_PAYLOAD(INFO, clientResponse->payload);

    if (clientResponse->numRcvdVendorSpecificHeaderOptions > 0)
    {
        OIC_LOG (INFO, TAG, "Received vendor specific options");
        uint8_t i = 0;
        OCHeaderOption * rcvdOptions = clientResponse->rcvdVendorSpecificHeaderOptions;
        for( i = 0; i < clientResponse->numRcvdVendorSpecificHeaderOptions; i++)
        {
            if (((OCHeaderOption)rcvdOptions[i]).protocolID == OC_COAP_ID)
            {
		if (OCF_CONTENT_FORMAT_VERSION == ((OCHeaderOption)rcvdOptions[i]).optionID) { /* 2053 */
		    uint16_t versionValue = rcvdOptions[i].optionData[0] * 256 + rcvdOptions[i].optionData[1];
		    OIC_LOG_V(INFO, TAG, "CoAP Option %u (OCF_CONTENT_FORMAT_VERSION), value: %s (%u)",
			      ((OCHeaderOption)rcvdOptions[i]).optionID,
			      (versionValue == 2048)? "1.0.0"
			      : (versionValue == 2112)? "1.1.0"
			      : "UNKNOWN",
			      versionValue);

		} else {
		    if (COAP_OPTION_CONTENT_FORMAT == ((OCHeaderOption)rcvdOptions[i]).optionID) { /* 12 */
			uint16_t formatValue = rcvdOptions[i].optionData[0] * 256 + rcvdOptions[i].optionData[1];
			OIC_LOG_V(INFO, TAG, "CoAP Option %u (CONTENT_FORMAT), value: %u",
				  ((OCHeaderOption)rcvdOptions[i]).optionID, formatValue);
		    }
		}
		OIC_LOG_V(INFO, TAG, "Option buffer:");
                OIC_LOG_BUFFER(INFO, TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
			       MAX_HEADER_OPTION_DATA_LENGTH);
            }
        }
    }

    /* switch (TestCase) */
    /* { */
    /* case TEST_INTROSPECTION: */
    /*     InitIntrospectionPayload(clientResponse); */
    /*     break; */
    /* default: */
    /*     break; */
    /* } */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return OC_STACK_DELETE_TRANSACTION;
}

void send_get_msg ()
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    OCDoHandle handle;
    OCCallbackData cbData;
    cbData.cb = get_cb;
    cbData.context = NULL;
    cbData.cd = NULL;

    /* create query from OCResourcePayload */
    char szQueryUri[MAX_QUERY_LENGTH] = { 0 };
    strcpy(szQueryUri, resource->uri);
    OIC_LOG_V(DEBUG, TAG, "query: %s", szQueryUri);

    /* construct destination from eps[selected_ep] */
    int i = 0;
    for (i; i<tps_maps_count; i++) {
	if (strcmp(tps_maps[i].tps_string, eps[selected_ep]->tps) == 0)
	    break;
    }
    OIC_LOG_V(DEBUG, TAG, "adapter: %d (%s)", tps_maps[i].adapter, tps_maps[i].tps_string);

    /* FIXME: deal with priority field! */
    OCDevAddr server_ep;
    memset(&server_ep, 0, sizeof(server_ep));
    server_ep.adapter = tps_maps[i].adapter;
    /* server_ep.flags = OCTransportFlags enum; it multiplexes flags:
       secure, IP version, IPv6 scope, multicast */
    /* copy directly from ep struct (NB: ep.family is misnamed) */
    server_ep.flags   = eps[selected_ep]->family;
    OIC_LOG_V(DEBUG, TAG, "secured? %d", server_ep.flags & OC_FLAG_SECURE);
    /* server_ep.flags = server_ep.flags | OC_FLAG_SECURE; */
    /* OIC_LOG_V(DEBUG, TAG, "secured? %d", server_ep.flags & OC_FLAG_SECURE); */

    server_ep.port    = eps[selected_ep]->port;
    OIC_LOG_V(DEBUG, TAG, "port: %d", server_ep.port);
    memcpy(server_ep.addr, eps[selected_ep]->addr, sizeof(server_ep.addr));
    OIC_LOG_V(DEBUG, TAG, "address: %s", server_ep.addr);

    /* FIXME: support coap header options */

    OIC_LOG_V(INFO, TAG, "Sending get request >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    OCStackResult res;
    /* OCDoResource is deprecated, use OCDoRequest */
    res = OCDoRequest(&handle,	      /* OCDoHandle */
		      OC_REST_GET,      /* method */
		      szQueryUri,       /* request uri */
		      &server_ep,	      /* destination */
		      NULL,	      /* payload - caller must free */
		      /* FIXME: select CONNTYPE from resource */
		      CT_ADAPTER_IP,     /* connectivity type */
		      /* CT_DEFAULT,	      /\* connectivity type *\/ */
		      OC_LOW_QOS,	      /* qos */
		      &cbData,	      /* callback */
		      NULL,	      /* header options */
		      0		      /* nbr hdr options */
		      );
    if (res != OC_STACK_OK) {
	OIC_LOG(ERROR, TAG, "OCStack resource error");
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

int retrieve_coresource(int index)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY; index: %d", __func__, index);

    resource = oocf_coresource_db_mgr_get_coresource(index, &ocf_version);
    OIC_LOG_V(INFO, TAG, "ocf version: %d", ocf_version);

    memset(urlbuffer, '\0', 8192);
    int rc = get_url(urlmsg, urlbuffer, &urlmsg_count);
    if (rc) {
	OIC_LOG_V(ERROR, TAG, "get_url rc: %d", rc);
    }

    rc = get_eps();
    if (rc) {
	OIC_LOG_V(ERROR, TAG, "get_eps rc: %d", rc);
    }

    /* flockfile(stdout); */
    int action = show_retrieve_dlg();
    switch (action) {
    case SUBMIT:
	OIC_LOG_V(DEBUG, TAG, "%s SUBMIT ep %d", __func__, selected_ep);
	send_get_msg();;
	break;
    case CANCEL:
	OIC_LOG_V(DEBUG, TAG, "%s CANCEL", __func__);
	break;
    case HELP:
	OIC_LOG_V(DEBUG, TAG, "%s HELP", __func__);
	break;
    }
    /* funlockfile(stdout); */

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}
