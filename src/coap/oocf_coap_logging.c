#include "oocf_coap_logging.h"

#include "coap/pdu.h"

void log_header_options (OCClientResponse *clientResponse)
{
    /* COAP option numbers enumerated in libcoap pdu.h; except for the
       OCF custom ones: OCF-Content-Format-Version (2049) and
       OCF-Accept-Content-Format-Version (2053) */
    /* See RFC 7252, section 5.10 for option definitions */

    uint16_t content_format = 0;
    uint16_t content_format_version = 0;
    uint16_t option_id  = 0;
    uint16_t option_len = 0;
    for (int i=0; i < clientResponse->numRcvdVendorSpecificHeaderOptions; i++) {
	OIC_LOG_V(INFO, TAG, "\tOption %d:", i);

	/* With OCF 1.0, the version of the content (payload) format must also be negotiated.
	 * OCF 1.0 section 12.2.5 says messages must always indicate format version etc..
	 * COAP_OPTION_ACCEPT_VERSION 2049 = OCF-Accept-Content-Format-Version (set by client requesting payload)
	 * COAP_OPTION_CONTENT_VERSION 2053 = OCF-Content-Format-Version (set if msg has payload)
	 * Values:  2048 = 0x0800 = OCF version 1.0; 2112 = 0x0840 = OIC version 1.1 */

	/* So we expect two headers, one to indicate the payload format, and
	   another to indicate format version */

	/* OIC_LOG_V(INFO, TAG, "\t\t protocol id FIXME: %d", */
	/* 	  clientResponse->rcvdVendorSpecificHeaderOptions[i].protocolID); */

	option_id = clientResponse->rcvdVendorSpecificHeaderOptions[i].optionID;
	option_len = clientResponse->rcvdVendorSpecificHeaderOptions[i].optionLength;
	switch (option_id) {
	case  COAP_OPTION_ACCEPT: /* uint */
	    /* Client and server negotiate content (payload) format using COAP header options
	     * COAP_OPTION_ACCEPT 12 and COAP_OPTION_CONTENT_VERSION; options are:
	     * application/cbor 60 = 0x3C
	     * application/vnd.ocf+cbor 10000 = 0x2710
	     * application/json 50 = 0x32
	     * see: https://www.iana.org/assignments/core-parameters/core-parameters.xhtml */
	    OIC_LOG_V(INFO, TAG, "\t\t Accept (code %d)",
		      option_id);
	    /* uint */
	    break;
	case OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION: /* 2 byte uint */
	    OIC_LOG_V(INFO, TAG, "\t\t OCF-Accept-Content-Version-Format (code %d), len %d",
		      option_id, option_len);
	    break;

	case  COAP_OPTION_CONTENT_FORMAT: /* uint */
	    content_format =
		(clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[0] * 0x0100
		 + clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[1]);
	    OIC_LOG_V(INFO, TAG, "\t\t Content-Format (%d) = %s (%d)",
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

	case OCF_OPTION_CONTENT_FORMAT_VERSION:
	    /* 2 byte uint */
	    content_format_version =
		(clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[0] * 0x0100
		 + clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[1]);
	    OIC_LOG_V(INFO, TAG, "\t\t OCF-Content-Version-Format (%d) = %s (%d)",
		      option_id,
		      /* option_len, */
		      (OCF_VERSION_1_0_0 == content_format_version)? "OCF 1.0.0"
		      : (OCF_VERSION_1_1_0 == content_format_version)? "OCF 1.1.0"
		      : "(UNKNOWN)",
		       content_format_version);
	    break;
	case  COAP_OPTION_IF_MATCH:
	    /* opaque */
	    OIC_LOG_V(INFO, TAG, "\t\t If-Match (code %d)", option_id);
	    break;
	case  COAP_OPTION_URI_HOST:
	    /* string */
	    OIC_LOG_V(INFO, TAG, "\t\t Uri-Host (code %d)", option_id);
	    break;
	case  COAP_OPTION_ETAG:
	    OIC_LOG_V(INFO, TAG, "\t\t ETag (code %d)",
		      option_id);
	    /* empty */
	    break;
	case  COAP_OPTION_IF_NONE_MATCH:
	    OIC_LOG_V(INFO, TAG, "\t\t If-None-Match (code %d)",
		      option_id);
	    /* empty */
	    break;
	case  COAP_OPTION_URI_PORT:
	    OIC_LOG_V(INFO, TAG, "\t\t Uri-Port (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_LOCATION_PATH:
	    OIC_LOG_V(INFO, TAG, "\t\t Location-Path (code %d)",
		      option_id);
	    /* string */
	    break;
	case  COAP_OPTION_URI_PATH:
	    OIC_LOG_V(INFO, TAG, "\t\t Uri-Path (code %d)",
		      option_id);
	    /* string */
	    break;

	case  COAP_OPTION_MAXAGE:
	    OIC_LOG_V(INFO, TAG, "\t\t Max-Age (code %d)",
		      option_id);
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
	    OIC_LOG_V(INFO, TAG, "\t\t Size2 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_OBSERVE:
	    OIC_LOG_V(INFO, TAG, "\t\t Observe (code %d)",
		      option_id);
	    /* empty/uint */
	    break;
	    /* duplicate of COAP_OPTON_OBSERVE */
	    /* case  COAP_OPTION_SUBSCRIPTION:
	     *     OIC_LOG_V(INFO, TAG, "\t\t Observe (code %d)",
	     * 	  option_id);
	     *     /\* empty/uint *\/
	     *     break; */
	case  COAP_OPTION_BLOCK2:
	    OIC_LOG_V(INFO, TAG, "\t\t Block2 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_OPTION_BLOCK1:
	    OIC_LOG_V(INFO, TAG, "\t\t Block1 (code %d)",
		      option_id);
	    /* uint */
	    break;
	case  COAP_MAX_OPT:
	    OIC_LOG_V(INFO, TAG, "\t\t Max-Opt (code %d)",
		      option_id);
	    break;
	default:
	    OIC_LOG_V(INFO, TAG, "\t\t UNKOWN (code %d)",
		      option_id);
	}
	/* for (int k = 0; k < clientResponse->rcvdVendorSpecificHeaderOptions[i].optionLength; k++) {
	 *     OIC_LOG_V(INFO, TAG, "\t\t datum[%d]: 0x%X", k,
	 * 	      clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData[k]); */
    }
}


void log_coap_msg_code(const coap_pdu_t *pdu, CATransportAdapter_t transport)
// const CASecureEndpoint_t *origin_sep)
{
    if (transport & CA_ADAPTER_IP) {
        /* NOTE: libcoap incorrectly uses RESPONSE class, should be MSG CLASS (covers both requests and responses */
        uint8_t code = pdu->transport_hdr->udp.code & 0x2F;
        if ((0 == COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code))
                   &&
            (0 == (pdu->transport_hdr->udp.code & 0x2F))) {
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE CLASS = %d %s",
                      COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code), "EMPTY MSG");
        }
        if ((0 == COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code))
            &&
            (0 < (pdu->transport_hdr->udp.code & 0x2F))) {
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE CLASS = 0 %s", "REQUEST");
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE DETAIL = %d %s",
                      code,
                      (code == 1)? "GET"
                      :(code == 2)? "POST"
                      :(code == 3)? "PUT"
                      :(code == 2)? "DELETE"
                      :"UNKNOWN");
        }
        if (2 == COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code)) {
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE CLASS = 2 %s", "SUCCESS");
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE DETAIL = %d %s",
                      code,
                      (code == 1)? "CREATED"
                      :(code == 2)? "DELETED"
                      :(code == 3)? "VALID"
                      :(code == 4)? "CHANGED"
                      :(code == 5)? "CONTENT"
                      :"UNKNOWN");
        }
        if (4 == COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code)) {
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE CLASS = 4 %s", "CLIENT ERROR");
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE DETAIL = %d %s",
                      code,
                      (code == 0)? "BAD REQUEST"
                      :(code == 1)? "AUTHORIZED"
                      :(code == 2)? "BAD OPTIONS"
                      :(code == 3)? "FORBIDDEN"
                      :(code == 4)? "NOT FOUND"
                      :(code == 5)? "METHOD NOT ALLOWED"
                      :(code == 6)? "NOT ACCEPTABLE"
                      :(code == 12)? "PRECONDITION FAILED"
                      :(code == 13)? "REQUEST ENTITY TOO LARGE"
                      :(code == 16)? "UNSUPPORTED CONTENT_FORMAT"
                      :"UNKNOWN");
        }
        if (5 == COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code)) {
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE CLASS = 5 %s", "SERVER ERROR");
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE DETAIL = %d %s",
                      code,
                      (code == 0)? "INTERNAL SERVER ERROR"
                      :(code == 1)? "NOT IMPLEMENTED"
                      :(code == 2)? "BAD GATEWAY"
                      :(code == 3)? "SERVICE UNAVAILABLE"
                      :(code == 4)? "GATEWAY TIMEOUT"
                      :(code == 5)? "PROXYING NOT SUPPORTED"
                      :"UNKNOWN");
        }
    }
}

#if INTERFACE
#include <coap/pdu.h>
#endif
void log_coap_pdu_hdr(const coap_pdu_t *pdu, CATransportAdapter_t transport)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);

    //coap_transport_t transport = COAP_UDP;
#ifdef WITH_TCP
    if (CAIsSupportedCoAPOverTCP(transport))
    {
        transport = coap_get_tcp_header_type_from_initbyte(((unsigned char *)pdu->transport_hdr)[0] >> 4);
    }
#endif

    OIC_LOG_V(DEBUG, TAG, "transport: %s",
              (transport == CA_ADAPTER_IP)? "UDP"
              :(transport == CA_ADAPTER_IP)? "TCP"
              :"OTHER");
    if (transport == CA_ADAPTER_IP) { // COAP_UDP) {
        OIC_LOG_V(DEBUG, TAG, "coap msg type: %u %s",
                  pdu->transport_hdr->udp.type,
                  (pdu->transport_hdr->udp.type == COAP_MESSAGE_CON)? "CONfirmable"
                  :(pdu->transport_hdr->udp.type == COAP_MESSAGE_NON)? "NON-confirmable"
                  :(pdu->transport_hdr->udp.type == COAP_MESSAGE_ACK)? "ACK"
                  :(pdu->transport_hdr->udp.type == COAP_MESSAGE_RST)? "RESET"
                  :"UNKNOWN");

        OIC_LOG_V(DEBUG, TAG, "coap msg id: %u", pdu->transport_hdr->udp.id);
        OIC_LOG_V(DEBUG, TAG, "coap msg token:");
        OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *)pdu->transport_hdr->udp.token,
                       pdu->transport_hdr->udp.token_length);
    }

    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

/* CoAP Options */
char *get_coap_option_key_string(int i) EXPORT
//uint16_t id) EXPORT
{
    /* https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#option-numbers */
    switch (i) {
    case COAP_OPTION_IF_MATCH: return "If-Match"; /* 1 */
        break;
    case COAP_OPTION_URI_HOST: return "Uri-Host"; /* 3 */
        break;
    case COAP_OPTION_ETAG: return "ETag";        /* 4 */
        break;
    case COAP_OPTION_IF_NONE_MATCH: return "If-None-Match"; /* 5 */
        break;
    case COAP_OPTION_OBSERVE: return "Observe";  /* 6 */
        break;
    case COAP_OPTION_URI_PORT: return "Uri-Port"; /* 7 */
        break;
    case COAP_OPTION_LOCATION_PATH: return "Location-Path"; /* 8 */
        break;
    case COAP_OPTION_URI_PATH: return "Uri-Path"; /* 11 */
        break;
    case COAP_OPTION_CONTENT_FORMAT: return "Content-Format"; /* 12 */
        break;
    case COAP_OPTION_MAXAGE: return "Max-Age";   /* 14 */
        break;
    case COAP_OPTION_URI_QUERY: return "Uri-Query"; /* 15 */
        break;
    case COAP_OPTION_ACCEPT: return "Accept";    /* 17 */
        break;
    case COAP_OPTION_LOCATION_QUERY: return "Location-Query"; /* 20 */
        break;
    case COAP_OPTION_BLOCK2: return "Block2";    /* 23, RFC 7959, 8323 */
        break;
    case COAP_OPTION_BLOCK1: return "Block1";    /* 27, RFC 7959, 8323 */
        break;
    case COAP_OPTION_SIZE2: return "Size2";      /* 28, RFC 7959 */
        break;
    case COAP_OPTION_PROXY_URI: return "Proxy-Uri"; /* 35 */
        break;
    case COAP_OPTION_PROXY_SCHEME: return "Proxy-Scheme"; /* 39 */
        break;
    case COAP_OPTION_SIZE1: return "Size1";      /* 60 */
        break;
    case COAP_OPTION_NORESPONSE: return "No-Response"; /* 258, RFC 7967 */
        break;
    case OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION: return "OCF-Accept-Content-Format-Version"; /* 2049 */
        break;
    case OCF_OPTION_CONTENT_FORMAT_VERSION: return "OCF-Content-Format-Version"; /* 2053 */
        break;
    default: return "DEFAULT";
        break;
    }
}

char *log_coap_option(coap_option *option) EXPORT
//uint16_t id) EXPORT
{
    /* https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#option-numbers */
    switch (COAP_OPTION_KEY(*option)) {
    case COAP_OPTION_IF_MATCH:  /* 1 */
        OIC_LOG_V(DEBUG, "OPT", "option 1 If-Match, len %d: 0x%02x",
                  COAP_OPTION_LENGTH(*option), *COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_URI_HOST: /* 3 */
        OIC_LOG_V(DEBUG, "OPT", "option 3 Uri-Host, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_ETAG: /* 4 */
        OIC_LOG_V(DEBUG, "OPT", "option 4 ETag, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_IF_NONE_MATCH: /* 5 */
        OIC_LOG_V(DEBUG, "OPT", "option 5 If-None-Match, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_OBSERVE:   /* 6, uint */
        OIC_LOG_V(DEBUG, "OPT", "option 6 Observe, len %d: 0x%02x",
                  COAP_OPTION_LENGTH(*option), *COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_URI_PORT:  /* 7 */
        OIC_LOG_V(DEBUG, "OPT", "option 7 Uri-Port, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_LOCATION_PATH:  /* 8 */
        OIC_LOG_V(DEBUG, "OPT", "option 8 Location-Path, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_URI_PATH: /* 11 */
        OIC_LOG_V(DEBUG, "OPT", "option 11 Uri-Path, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_CONTENT_FORMAT: /* 12 */
        OIC_LOG_V(DEBUG, "OPT", "option 12 Content-Format, len %d: %u",
                  COAP_OPTION_LENGTH(*option),
                  ntohs( *((uint16_t*)COAP_OPTION_DATA(*option)))
                  );
        break;
    case COAP_OPTION_MAXAGE:   /* 14 */
        OIC_LOG_V(DEBUG, "OPT", "option 14 Max-Age, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_URI_QUERY: /* 15 */
        OIC_LOG_V(DEBUG, "OPT", "option 15 Uri-Query, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_ACCEPT:    /* 17 */
        OIC_LOG_V(DEBUG, "OPT", "option 17 Accept, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_LOCATION_QUERY: return "Location-Query"; /* 20 */
        break;
    case COAP_OPTION_BLOCK2:     /* 23, RFC 7959, 8323 */
        OIC_LOG_V(DEBUG, "OPT", "option 23 Block2, len %d: NUM %u, M: %1x, SZX %d",
                  COAP_OPTION_LENGTH(*option),
                  (*((uint8_t*)COAP_OPTION_DATA(*option)) >> 4),
                  (*((uint8_t*)COAP_OPTION_DATA(*option)) >> 3),
                  (0x7 & *((uint8_t*)COAP_OPTION_DATA(*option))));
        break;
    case COAP_OPTION_BLOCK1:    /* 27, RFC 7959, 8323 */
        OIC_LOG_V(DEBUG, "OPT", "option 27 Block1, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_SIZE2: /* 28, RFC 7959 */
        OIC_LOG_V(DEBUG, "OPT", "option 28 Size2, len %d: %u",
                  COAP_OPTION_LENGTH(*option),
                  ntohs( *((uint16_t*)COAP_OPTION_DATA(*option))));
        break;
    case COAP_OPTION_PROXY_URI:  /* 35 */
        OIC_LOG_V(DEBUG, "OPT", "option 35 Proxy-Uri, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_PROXY_SCHEME:  /* 39 */
        OIC_LOG_V(DEBUG, "OPT", "option 39 Proxy-Scheme, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_SIZE1: return "Size1";      /* 60 */
        OIC_LOG_V(DEBUG, "OPT", "option 60 Size1, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_NORESPONSE: /* 258, RFC 7967 */
        OIC_LOG_V(DEBUG, "OPT", "option 258 No-Response, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION: /* 2049 */
        OIC_LOG_V(DEBUG, "OPT", "option 2049 OCF-Accept-Content-Format-Version, len %d: %u",
                  COAP_OPTION_LENGTH(*option),
                  ntohs( *((uint16_t*)COAP_OPTION_DATA(*option))));
        break;
    case OCF_OPTION_CONTENT_FORMAT_VERSION: /* 2053 */
        OIC_LOG_V(DEBUG, "OPT", "option 2053 OCF-Accept-Content-Format-Version, len %d: %u",
                  COAP_OPTION_LENGTH(*option),
                  ntohs( *((uint16_t*)COAP_OPTION_DATA(*option))));
        break;
    default:
        OIC_LOG_V(DEBUG, "OPT", "default");
        break;
    }
}

char *log_coap_option_string(struct oocf_coap_options options[], int i) EXPORT
//uint16_t id) EXPORT
{
    /* https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#option-numbers */
    switch (options[i].optionID) {
    case COAP_OPTION_IF_MATCH: return "If-Match"; /* 1 */
        break;
    case COAP_OPTION_URI_HOST: return "Uri-Host"; /* 3 */
        break;
    case COAP_OPTION_ETAG: return "ETag";        /* 4 */
        break;
    case COAP_OPTION_IF_NONE_MATCH: return "If-None-Match"; /* 5 */
        break;
    case COAP_OPTION_OBSERVE: return "Observe";  /* 6 */
        break;
    case COAP_OPTION_URI_PORT: return "Uri-Port"; /* 7 */
        break;
    case COAP_OPTION_LOCATION_PATH: return "Location-Path"; /* 8 */
        break;
    case COAP_OPTION_URI_PATH: return "Uri-Path"; /* 11 */
        break;
    case COAP_OPTION_CONTENT_FORMAT: return "Content-Format"; /* 12 */
        break;
    case COAP_OPTION_MAXAGE: return "Max-Age";   /* 14 */
        break;
    case COAP_OPTION_URI_QUERY: return "Uri-Query"; /* 15 */
        break;
    case COAP_OPTION_ACCEPT: return "Accept";    /* 17 */
        break;
    case COAP_OPTION_LOCATION_QUERY: return "Location-Query"; /* 20 */
        break;
    case COAP_OPTION_BLOCK2: return "Block2";    /* 23, RFC 7959, 8323 */
        break;
    case COAP_OPTION_BLOCK1: return "Block1";    /* 27, RFC 7959, 8323 */
        break;
    case COAP_OPTION_SIZE2: return "Size2";      /* 28, RFC 7959 */
        break;
    case COAP_OPTION_PROXY_URI: return "Proxy-Uri"; /* 35 */
        break;
    case COAP_OPTION_PROXY_SCHEME: return "Proxy-Scheme"; /* 39 */
        break;
    case COAP_OPTION_SIZE1: return "Size1";      /* 60 */
        break;
    case COAP_OPTION_NORESPONSE: return "No-Response"; /* 258, RFC 7967 */
        break;
    case OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION: return "OCF-Accept-Content-Format-Version"; /* 2049 */
        break;
    case OCF_OPTION_CONTENT_FORMAT_VERSION: return "OCF-Content-Format-Version"; /* 2053 */
        break;
    default: return "DEFAULT";
        break;
    }
}

char *log_coap_option_value_string(struct oocf_coap_options options[], int i, uint16_t val) EXPORT
{
    /* https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#option-numbers */
    switch (options[i].optionID) {
    case COAP_OPTION_IF_MATCH: return "If-Match"; /* 1 */
        break;
    case COAP_OPTION_URI_HOST: return "Uri-Host"; /* 3 */
        break;
    case COAP_OPTION_ETAG: return "ETag";        /* 4 */
        break;
    case COAP_OPTION_IF_NONE_MATCH: return "If-None-Match"; /* 5 */
        break;
    case COAP_OPTION_OBSERVE: return "Observe";  /* 6 */
        break;
    case COAP_OPTION_URI_PORT: return "Uri-Port"; /* 7 */
        break;
    case COAP_OPTION_LOCATION_PATH: return "Location-Path"; /* 8 */
        break;
    case COAP_OPTION_URI_PATH: return "Uri-Path"; /* 11 */
        break;
    case COAP_OPTION_CONTENT_FORMAT: /* 12 */
        if (val == COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR)
            return "application/vnd.ocf+cbor";
        else if (val == COAP_MEDIATYPE_JSON)
            return "application/json";
        else
            return "UKNOWN";
        break;
    case COAP_OPTION_MAXAGE: return "Max-Age";   /* 14 */
        break;
    case COAP_OPTION_URI_QUERY: return "Uri-Query"; /* 15 */
        break;
    case COAP_OPTION_ACCEPT: return "Accept";    /* 17 */
        break;
    case COAP_OPTION_LOCATION_QUERY: return "Location-Query"; /* 20 */
        break;
    case COAP_OPTION_BLOCK2: return "Block2";    /* 23, RFC 7959, 8323 */
        break;
    case COAP_OPTION_BLOCK1: return "Block1";    /* 27, RFC 7959, 8323 */
        break;
    case COAP_OPTION_SIZE2: return "Size2";      /* 28, RFC 7959 */
        break;
    case COAP_OPTION_PROXY_URI: return "Proxy-Uri"; /* 35 */
        break;
    case COAP_OPTION_PROXY_SCHEME: return "Proxy-Scheme"; /* 39 */
        break;
    case COAP_OPTION_SIZE1: return "Size1";      /* 60 */
        break;
    case COAP_OPTION_NORESPONSE: return "No-Response"; /* 258, RFC 7967 */
        break;
    case OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION: /* 2049 */
        /* OCF-Accept-Content-Format-Version */
        if (val == OCF_VERSION_1_0_0)
            return "1.0.0";
        else if (val == OCF_VERSION_1_0_0)
            return "1.1.0";
        else
            return "UNKNOWN";
        break;
    case OCF_OPTION_CONTENT_FORMAT_VERSION: /* 2053 */
        /* OCF-Content-Format-Version */
        if (val == OCF_VERSION_1_0_0)
            return "1.0.0";
        else if (val == OCF_VERSION_1_0_0)
            return "1.1.0";
        else
            return "UNKNOWN";
        break;
    default: return "DEFAULT";
        break;
    }
}
