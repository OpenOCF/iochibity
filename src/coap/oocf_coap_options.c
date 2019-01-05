
#include "oocf_coap_options.h"

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
#define COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR 10000
#define OCF_ACCEPT_CONTENT_FORMAT_VERSION       2049  /* CoAP option number */
#define OCF_CONTENT_FORMAT_VERSION              2053  /* CoAP Option number */

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
struct oocf_coap_options
{
    CATransportProtocolID_t protocolID;                     /**< Protocol ID of the Option */
    uint16_t optionID;                                      /**< CoAP option number */
    uint16_t optionLength;                                  /**< Option Length **/
    char optionData[CA_MAX_HEADER_OPTION_DATA_LENGTH];      /**< Optional data values**/
};
typedef oocf_coap_options CAHeaderOption_t;
/**
 * This structure will be used to define the vendor specific header options to be included
 * in communication packets.
 */
typedef struct OCHeaderOption
{
    OCTransportProtocolID protocolID;
    uint16_t optionID;
    uint16_t optionLength;
    uint8_t optionData[MAX_HEADER_OPTION_DATA_LENGTH];
} OCHeaderOption;
#endif

/* OCStackResult OC_CALL OCGetHeaderOption(OCHeaderOption* ocHdrOpt, size_t numOptio
ns, */
/*                                         uint16_t optionID, void* optionData, size_t optionDataLength, */
/*                                         uint16_t* receivedDataLength) */
OCStackResult OC_CALL OCGetHeaderOption(struct oocf_coap_options /* CAHeaderOption_t */ *options_array, /**< array */
                                        size_t numOptions,
                                        uint16_t optionID,
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

