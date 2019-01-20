#include "oocf_coap_options.h"

#if INTERFACE
#include "coap_config.h"
#include "coap/pdu.h"           /* must come before bits.h */
#include "coap/bits.h"
#include "coap/option.h"
#endif


/* NB!!! "Instead of specifying the Option Number directly, the
   instances MUST appear in order of their Option Numbers..." RFC 7252
   section 3.1 */

/**
 * option types - the highest option number 63.
 */
#if EXPORT_INTERFACE
#define CA_OPTION_IF_MATCH 1          /**< match option */
#define CA_OPTION_ETAG 4              /**< ETAG option */
#define CA_OPTION_IF_NONE_MATCH 5     /**< non match option */
#define CA_OPTION_OBSERVE 6           /**< observe option */
#define CA_OPTION_LOCATION_PATH 8     /**< location path option */
#define CA_OPTION_URI_PATH 11         /**< URI path option */
#define COAP_OPTION_CONTENT_FORMAT 12   /**< content format option */
#define CA_OPTION_CONTENT_TYPE COAP_OPTION_CONTENT_FORMAT /**< content type option */
#define CA_OPTION_MAXAGE 14           /**< max age option */
#define CA_OPTION_URI_QUERY 15        /**< uri query option */
#define CA_OPTION_ACCEPT 17           /**< accept option */
#define CA_OPTION_LOCATION_QUERY 20   /**< location query option */

/**
 * Option ID of header option. The values match CoAP option types in pdu.h.
 */
typedef enum
{
    CA_HEADER_OPTION_ID_LOCATION_PATH = 8,
    CA_HEADER_OPTION_ID_LOCATION_QUERY = 20
} CAHeaderOptionId_t;
#endif

/* CoAP Option numbers and values */
/* https://www.iana.org/assignments/core-parameters/core-parameters.xhtml */
/* OCF spec 12.2.5 */
#if EXPORT_INTERFACE
 /* application/vnd.ocf+cbor */
#define COAP_MEDIATYPE_JSON                     50
#define COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR 10000
#define OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION       2049  /* CoAP option number */
#define OCF_OPTION_CONTENT_FORMAT_VERSION              2053  /* CoAP Option number */

/** Integer value of spec version (OCF v1.0 0 = b0000:1000:0000:0000 = 2048).*/
/* These are values for the OCF version CoAP Options above */
#define OCF_VERSION_1_0_0                2048
#define OCF_VERSION_1_1_0                2112
#define OC_SPEC_VERSION_VALUE            OCF_VERSION_1_0_0

// The Accept Version and Content-Format Version for OCF 1.0.0 (0b0000 1000 0000 0000).
#define DEFAULT_VERSION_VALUE            OCF_VERSION_1_0_0
#endif	/* EXPORT_INTERFACE */

/**
 * Option numbers for Signaling messages are specific to the message code.
 * They do not share the number space with CoAP options for request/response
 * messages or with Signaling messages using other codes.
 */
#if INTERFACE
#define CA_OPTION_SERVER_NAME_SETTING 1    /**< Capability and Settings messages, code=7.01 */
#define CA_OPTION_MAX_MESSAGE_SIZE 2       /**< Capability and Settings messages, code=7.01 */
#define CA_OPTION_BLOCK_WISE_TRANSFER 4    /**< Capability and Settings messages, code=7.01 */
#define CA_OPTION_CUSTODY 2                /**< Ping and Pong Messages, code=7.02 */
#define CA_OPTION_BAD_SERVER_NAME 2        /**< Release Messages, code=7.04 */
#define CA_OPTION_ALTERNATE_ADDRESS 4      /**< Abort Messages, code=7.05 */
#define CA_OPTION_HOLD_OFF 6               /**< Abort Messages, code=7.05 */
#endif

#if EXPORT_INTERFACE
#include <stdint.h>
struct oocf_coap_option
{
    //CATransportProtocolID_t protocolID;                     /**< Protocol ID:  CoAP or ?? */
    uint16_t optionID;                                      /**< CoAP option number */
    uint16_t optionLength;                                  /**< Option Length **/
    uint8_t /* char */ optionData[CA_MAX_HEADER_OPTION_DATA_LENGTH];      /**< Optional data values**/
};
typedef struct oocf_coap_option CAHeaderOption_t;
typedef struct oocf_coap_option OCHeaderOption;


/**
 * Transport Protocol IDs.
 */
/* typedef enum */
/* { */
/*     /\** For invalid ID.*\/ */
/*     OC_INVALID_ID   = (1 << 0), */

/*     /\* For coap ID.*\/ */
/*     OC_COAP_ID      = (1 << 1) */
/* } OCTransportProtocolID; */

/**
 * This structure will be used to define the vendor specific header options to be included
 * in communication packets.
 */
/* typedef struct OCHeaderOption */
/* { */
/*     // protocolID is evidently speculative; CoAP options do not */
/*     // include a protocol id.  Maybe this is in case some protocol */
/*     // other than CoAP should be added later? */
/*     /\* OCTransportProtocolID protocolID; *\/ */
/*     uint16_t optionID; */
/*     uint16_t optionLength; */
/*     uint8_t optionData[MAX_HEADER_OPTION_DATA_LENGTH]; */
/* } OCHeaderOption; */
#endif

/* OCStackResult OC_CALL OCGetHeaderOption(OCHeaderOption* ocHdrOpt, size_t numOptio
ns, */
/*                                         uint16_t optionID, void* optionData, size_t optionDataLength, */
/*                                         uint16_t* receivedDataLength) */
OCStackResult OC_CALL OCGetHeaderOption(const struct oocf_coap_option /* CAHeaderOption_t */ *options_array,
                                        const size_t numOptions,
                                        const uint16_t optionID,
                                        void *optionData, /**< [out]  */
                                        size_t optionDataLength,
                                        uint16_t* receivedDataLength /**< [out]   */
                                        ) EXPORT
{
    if (!options_array || !numOptions)
    {
        OIC_LOG (INFO, TAG, "No options present");
        return OC_STACK_OK;
    }

    if (!optionData)
    {
        OIC_LOG (INFO, TAG, "optionData are NULL");
        return OC_STACK_INVALID_PARAM;
    }

    if (!receivedDataLength)
    {
        OIC_LOG (INFO, TAG, "receivedDataLength is NULL");
        return OC_STACK_INVALID_PARAM;
    }

    for (size_t i = 0; i < numOptions; i++)
    {
        if (options_array[i].optionID == optionID)
        {
            if (optionDataLength >= options_array->optionLength)
            {
                memcpy(optionData, options_array[i].optionData, options_array[i].optionLength);
                *receivedDataLength = options_array[i].optionLength;
                return OC_STACK_OK;
            }
            else
            {
                OIC_LOG (ERROR, TAG, "optionDataLength is less than the length of received data");
                return OC_STACK_ERROR;
            }
        }
    }
    return OC_STACK_OK;
}

#if INTERFACE
#include "coap/coap_list.h"
#endif
CAResult_t CAAddOptionsToPDU(coap_pdu_t *pdu, coap_list_t **options)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    // after adding the block option to option list, add option list to pdu.
    if (*options)
    {
        for (coap_list_t *opt = *options; opt; opt = opt->next)
        {
            // OIC_LOG_V(DEBUG, TAG, "pdu length: %d", pdu->length);
            /* OIC_LOG_V(DEBUG, TAG, "adding option %d %s, len %d, val: %s", */
            /*           COAP_OPTION_KEY(*(coap_option *) opt->data), */
            /*           get_coap_option_key_string(COAP_OPTION_KEY(*(coap_option *) opt->data)), */
            /*           COAP_OPTION_LENGTH(*(coap_option *) opt->data), */
            /*           COAP_OPTION_DATA(*(coap_option *) opt->data)); */

            log_coap_option((coap_option*)opt->data);

            size_t ret = coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option *) opt->data),
                                         COAP_OPTION_LENGTH(*(coap_option *) opt->data),
                                         COAP_OPTION_DATA(*(coap_option *) opt->data));
            if (!ret)
            {
                return CA_STATUS_FAILED;
            }
        }
    }

    OIC_LOG_V(DEBUG, TAG, "[%d] pdu length after option", pdu->length);

    return CA_STATUS_OK;
}

/**
 * Set Header Option.
 * @param caHdrOpt            Pointer to existing options
 * @param numOptions          Number of existing options.
 * @param optionID            COAP option ID.
 * @param optionData          Option data value.
 * @param optionDataLength    Size of Option data value.

 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult SetHeaderOption(CAHeaderOption_t *caHdrOpt, size_t numOptions,
                              uint16_t optionID, void* optionData, size_t optionDataLength)
{
    if (!caHdrOpt)
    {
        return OC_STACK_INVALID_PARAM;
    }

    if (!optionData)
    {
        OIC_LOG (INFO, TAG, "optionData are NULL");
        return OC_STACK_INVALID_PARAM;
    }

    //caHdrOpt[numOptions].protocolID = CA_COAP_ID;
    caHdrOpt[numOptions].optionID = optionID;
    caHdrOpt[numOptions].optionLength =
            (optionDataLength < MAX_HEADER_OPTION_DATA_LENGTH) ?
                    (uint16_t) optionDataLength : MAX_HEADER_OPTION_DATA_LENGTH;
    memcpy(caHdrOpt[numOptions].optionData, (const void*) optionData,
            caHdrOpt[numOptions].optionLength);

    return OC_STACK_OK;
}

OCStackResult OC_CALL OCSetHeaderOption(OCHeaderOption* ocHdrOpt, size_t* numOptions, uint16_t optionID,
                                        void* optionData, size_t optionDataLength) EXPORT
{
    if (!ocHdrOpt)
    {
        OIC_LOG (INFO, TAG, "Header options are NULL");
        return OC_STACK_INVALID_PARAM;
    }

    if (!optionData)
    {
        OIC_LOG (INFO, TAG, "optionData are NULL");
        return OC_STACK_INVALID_PARAM;
    }

    if (!numOptions)
    {
        OIC_LOG (INFO, TAG, "numOptions is NULL");
        return OC_STACK_INVALID_PARAM;
    }

    if (*numOptions >= MAX_HEADER_OPTIONS)
    {
        OIC_LOG (INFO, TAG, "Exceeding MAX_HEADER_OPTIONS");
        return OC_STACK_NO_MEMORY;
    }

    ocHdrOpt += *numOptions;
    // ocHdrOpt->protocolID = OC_COAP_ID;
    ocHdrOpt->optionID = optionID;
    ocHdrOpt->optionLength =
            (optionDataLength < MAX_HEADER_OPTION_DATA_LENGTH) ?
                    (uint16_t)optionDataLength : MAX_HEADER_OPTION_DATA_LENGTH;
    memcpy(ocHdrOpt->optionData, (const void*) optionData, ocHdrOpt->optionLength);
    *numOptions += 1;

    return OC_STACK_OK;
}

bool checkProxyUri(OCHeaderOption *options, uint8_t numOptions)
{
    if (!options || 0 == numOptions)
    {
        OIC_LOG (INFO, TAG, "No options present");
        return false;
    }

    for (uint8_t i = 0; i < numOptions; i++)
    {
        if (options[i].optionID == OC_RSRVD_PROXY_OPTION_ID)
            /* && */
            /* options[i].protocolID == OC_COAP_ID) */
        {
            OIC_LOG(DEBUG, TAG, "Proxy URI is present");
            return true;
        }
    }
    return false;
}

CAResult_t CAGetOptionCount(coap_opt_iterator_t opt_iter, uint8_t *optionCount)
{
    CAResult_t result = CA_STATUS_OK;
    coap_opt_t *option = NULL;
    *optionCount = 0;

    while ((option = coap_option_next(&opt_iter)))
    {
        if (COAP_OPTION_URI_PATH != opt_iter.type && COAP_OPTION_URI_QUERY != opt_iter.type
            && COAP_OPTION_BLOCK1 != opt_iter.type && COAP_OPTION_BLOCK2 != opt_iter.type
            && COAP_OPTION_SIZE1 != opt_iter.type && COAP_OPTION_SIZE2 != opt_iter.type
            && COAP_OPTION_URI_HOST != opt_iter.type && COAP_OPTION_URI_PORT != opt_iter.type
            && COAP_OPTION_ETAG != opt_iter.type && COAP_OPTION_MAXAGE != opt_iter.type
            && COAP_OPTION_PROXY_SCHEME != opt_iter.type)
        {
            if (*optionCount < UINT8_MAX)
            {
                (*optionCount)++;
            }
            else
            {
                // Overflow. Return an error to the caller.
                assert(false);
                OIC_LOG_V(ERROR, TAG, "Overflow detected in %s", __func__);
                *optionCount = 0;
                result = CA_STATUS_FAILED;
                break;
            }
        }
    }

    return result;
}
