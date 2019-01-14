//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#include "openocf.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include <signal.h>
#include <stdbool.h>

/* #include <ncurses.h> */

/* #include "ocstack.h"
 * #include "logger.h" */

#define TAG ("ocserver")

#define CLOG_MAIN
#include "clog.h"
const int MY_LOGGER = 0; /* Unique identifier for logger */

int gQuitFlag = 0;
OCStackResult createLightResource();

typedef struct LIGHTRESOURCE{
    OCResourceHandle handle;
    bool power;
} LightResource;

static LightResource Light;

bool resource_satisfies_query(char * query)
{
    /* return true if the resource satisfies the query */
    return true;
}

OCEntityHandlerResult put_light_react (oocf_inbound_request *inbound_stimulus,
				       OCRepPayload **payload)
{
    return OC_EH_OK;
}

OCEntityHandlerResult get_light_react (oocf_inbound_request *inbound_stimulus,
				       OCRepPayload **payload)
{
    clog_info(CLOG(MY_LOGGER), "%s ENTRY", __func__);
    clog_info(CLOG(MY_LOGGER), "%s inbound_stimulus resource handle: %p", __func__, inbound_stimulus->resource);
    OCEntityHandlerResult result;

    //FIXME
    if (resource_satisfies_query(get_query_from_inbound_request(inbound_stimulus))) {
	/* OCRepPayload *getResp = constructResponse(inbound_stimulus); */
	/* if(!getResp) { */
	/*     clog_error(CLOG(MY_LOGGER), "constructResponse failed"); */
	/*     return OC_EH_ERROR; */
	/* } */
	if (get_payload_from_inbound_request(inbound_stimulus)
             &&
             ( get_payload_from_inbound_request(inbound_stimulus)->type != PAYLOAD_TYPE_REPRESENTATION)) {
            //if(inbound_stimulus->payload && inbound_stimulus->payload->type != PAYLOAD_TYPE_REPRESENTATION) {
	    clog_error(CLOG(MY_LOGGER), PCF("Incoming payload not a representation"));
	    return OC_EH_ERROR;
	}

	clog_info(CLOG(MY_LOGGER), "%s creating rep payload", __func__);
	/* OCRepPayload* _payload = OCRepPayloadCreate(); */
	/* if(!payload) { */
	/*     clog_error(CLOG(MY_LOGGER), PCF("Failed to allocate Payload")); */
	/*     return OC_EH_ERROR; */
	/* } */

	/* copy resource state to payload */
	/* Note the inbound request does not contain the target URL,
	   but it does carry the resource handle, which we can use to
	   get the resource uri from the db. */
	/* OCRepPayloadSetUri(_payload, OCGetResourceUri(inbound_stimulus->resource)); */

	/* we can also invent ad-hoc properties IF we used OCRepPayloadCreate */
	/* OCRepPayloadSetPropBool(_payload, "foo", true); */
	/* OCRepPayloadSetPropBool(_payload, "bar", false); */

	/* resource "properties" actually means "policies", e.g. discoverable, secure, etc. */
	/* OCResourceProperty props = OCGetResourceProperties(inbound_stimulus->resource); */

	OCRepPayload *_payload = NULL;
	OCStackResult rc = BuildResponseRepresentation(inbound_stimulus->resource,
						      &_payload, // OCRepPayload** payload,
                                                      get_origin_ep_from_inbound_request(inbound_stimulus));
        if (rc != OC_STACK_OK) {
            clog_error(CLOG(MY_LOGGER), "BuildResponseRepresentation failure, rc: %u", rc);
            return OC_EH_ERROR;
        }
        //&inbound_stimulus->devAddr);  // OCDevAddr *devAddr)
	OCRepPayloadSetPropBool(_payload, "foo", true);
	OCRepPayloadSetPropBool(_payload, "bar", false);

	*payload = _payload;
	result = OC_EH_OK;
    } else {
	/* return empty if query fails */
        result = OC_EH_OK;
    }

    clog_info(CLOG(MY_LOGGER), "%s EXIT", __func__);
    return result;
}

OCEntityHandlerResult light_react (OCEntityHandlerFlag flag, /* OC_REQUEST_FLAG, OC_OBSERVE_FLAG */
				   oocf_inbound_request *inbound_stimulus,
				   void* param)
{
    clog_info(CLOG(MY_LOGGER), "%s ENTRY - flags: 0x%x", __func__, flag);
    // Validate pointer
    if (!inbound_stimulus)
    {
        clog_error(CLOG(MY_LOGGER), "Invalid inbound oocf_inbound_request pointer");
        return OC_EH_ERROR;
    }

    if (get_payload_from_inbound_request(inbound_stimulus))
	clog_debug(CLOG(MY_LOGGER), "payload type: %x", get_payload_from_inbound_request(inbound_stimulus)->type);
    else
	clog_debug(CLOG(MY_LOGGER), "no payload");

    //OCEntityHandlerResponse
    struct oocf_outbound_response outbound_response = { 0, 0, OC_EH_ERROR, 0, 0, { },{ 0 }, false };

    // Initialize certain outbound_response fields
    outbound_response.numSendVendorSpecificHeaderOptions = 0;
    //outbound_response.coap_option_count = 0;
    memset(outbound_response.sendVendorSpecificHeaderOptions,
            0, sizeof outbound_response.sendVendorSpecificHeaderOptions);
    memset(outbound_response.resourceUri, 0, sizeof outbound_response.resourceUri);
    OCRepPayload* payload = NULL;

    OCEntityHandlerResult response_code = OC_EH_OK;

    if (flag & OC_REQUEST_FLAG)
    {
        clog_info(CLOG(MY_LOGGER), "Flag includes OC_REQUEST_FLAG");

	switch(((struct CARequestInfo*)inbound_stimulus->requestHandle)->method) {
	case CA_GET:
            clog_info(CLOG(MY_LOGGER), "Received OC_REST_GET from client");
            response_code = get_light_react(inbound_stimulus, &payload);
	    break;
	case CA_DISCOVER:
            clog_info(CLOG(MY_LOGGER), "Received OC_REST_DISCOVER from client");
            //response_code = get_light_react(inbound_stimulus, &payload);
	    break;
	default:
            clog_info(CLOG(MY_LOGGER), "Received unsupported method %d from client",
                      ((struct CARequestInfo*)inbound_stimulus->requestHandle)->method);
            response_code = OC_EH_ERROR;
	}

        /* else if (OC_REST_PUT == inbound_stimulus->method) */
        /* { */
        /*     clog_info(CLOG(MY_LOGGER), "Received OC_REST_PUT from client"); */
        /*     response_code = ProcessPutRequest (inbound_stimulus, &payload); */
        /* } */
        /* else if (OC_REST_POST == inbound_stimulus->method) */
        /* { */
        /*     clog_info(CLOG(MY_LOGGER), "Received OC_REST_POST from client"); */
        /*     response_code = ProcessPostRequest (inbound_stimulus, &outbound_response, &payload); */
        /* } */
        /* else if (OC_REST_DELETE == inbound_stimulus->method) */
        /* { */
        /*     clog_info(CLOG(MY_LOGGER), "Received OC_REST_DELETE from client"); */
        /*     response_code = ProcessDeleteRequest (inbound_stimulus); */
        /* } */
        /* else */
        /* { */
        /*     clog_info(CLOG(MY_LOGGER), "Received unsupported method %d from client", */
        /*               inbound_stimulus->method); */
        /*     response_code = OC_EH_ERROR; */
        /* } */
        // If the response_code isn't an error or forbidden, send outbound_response
        if (!((response_code == OC_EH_ERROR) || (response_code == OC_EH_FORBIDDEN)))
        {
            // Format the outbound_response.  Note this requires some info about the request
            outbound_response.requestHandle = inbound_stimulus->requestHandle;
            outbound_response.ehResult = response_code;
            outbound_response.payload  = (OCPayload*)(payload);
            // Indicate that outbound_response is NOT in a persistent buffer
            outbound_response.persistentBufferFlag = 0;

            // Handle vendor specific options
            if(inbound_stimulus->requestHandle->info.options
               //rcvdVendorSpecificHeaderOptions
               &&
               inbound_stimulus->requestHandle->info.numOptions)
                // numRcvdVendorSpecificHeaderOptions)
            {
                clog_info(CLOG(MY_LOGGER), "Received vendor specific options");
                uint8_t i = 0;
                struct oocf_coap_options *rcvdOptions = inbound_stimulus->requestHandle->info.options;
                // inbound_stimulus->rcvdVendorSpecificHeaderOptions;
                for( i = 0;
                     /* i < inbound_stimulus->numRcvdVendorSpecificHeaderOptions; */
                     i < inbound_stimulus->requestHandle->info.numOptions;
                     i++)
                {
                    /* if(((struct oocf_coap_options /\* OCHeaderOption *\/)rcvdOptions[i]).protocolID == OC_COAP_ID) */
                    /* { */
                        clog_info(CLOG(MY_LOGGER),
                                  "Received option with option ID %u", rcvdOptions[i].optionID);

                        /* OIC_LOG_BUFFER(INFO, TAG, rcvdOptions[i].optionData, */
                        /*                MAX_HEADER_OPTION_DATA_LENGTH); */
                    /* } */
                }
                // Check on Accept Version option.
                uint8_t vOptionData[MAX_HEADER_OPTION_DATA_LENGTH];
                size_t vOptionDataSize = sizeof(vOptionData);
                uint16_t actualDataSize = 0;
                OCGetHeaderOption(inbound_stimulus->requestHandle->info.options, // rcvdVendorSpecificHeaderOptions,
				  inbound_stimulus->requestHandle->info.numOptions, //numRcvdVendorSpecificHeaderOptions,
				  OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION, // @was COAP_OPTION_ACCEPT_VERSION,
				  vOptionData, vOptionDataSize, &actualDataSize);
                if (actualDataSize)
                {
                    clog_info(CLOG(MY_LOGGER), "accept version option exists");
                    OIC_LOG_BUFFER(INFO, TAG, vOptionData, MAX_HEADER_OPTION_DATA_LENGTH);
                    uint16_t acceptVersion = vOptionData[0]*256 + vOptionData[1];
                    if (OC_SPEC_VERSION_VALUE == acceptVersion)
                    {
                        clog_info(CLOG(MY_LOGGER), "accept version equals to default OC_SPEC_VERSION_VALUE.");
                    }
                }

                actualDataSize = 0;
                OCHeaderOption* sendOptions = outbound_response.sendVendorSpecificHeaderOptions;
                size_t numOptions = outbound_response.numSendVendorSpecificHeaderOptions;
                // Check if the option header has already existed before adding it in.
                uint8_t optionData[MAX_HEADER_OPTION_DATA_LENGTH];
                size_t optionDataSize = sizeof(optionData);

                actualDataSize = 0;
                OCGetHeaderOption(outbound_response.sendVendorSpecificHeaderOptions,
                                  outbound_response.numSendVendorSpecificHeaderOptions,
                                  2248,
                                  optionData,
                                  optionDataSize,
                                  &actualDataSize);
                if (actualDataSize == 0)
                {
                    uint8_t option2[] = {21,22,23,24,25,26,27,28,29,30};
                    uint16_t optionID2 = 2248;
                    size_t optionDataSize2 = sizeof(option2);
                    OCSetHeaderOption(sendOptions,
                                      &numOptions,
                                      optionID2,
                                      option2,
                                      optionDataSize2);
                }

                actualDataSize = 0;
                OCGetHeaderOption(outbound_response.sendVendorSpecificHeaderOptions,
                                  outbound_response.numSendVendorSpecificHeaderOptions,
                                  2600,
                                  optionData,
                                  optionDataSize,
                                  &actualDataSize);
                if (actualDataSize == 0)
                {
                    uint8_t option3[] = {31,32,33,34,35,36,37,38,39,40};
                    uint16_t optionID3 = 2600;
                    size_t optionDataSize3 = sizeof(option3);
                    OCSetHeaderOption(sendOptions,
                                      &numOptions,
                                      optionID3,
                                      option3,
                                      optionDataSize3);
                }
            }

            // Send the outbound_response
            if (OCDoResponse(&outbound_response) != OC_STACK_OK)
            {
                clog_error(CLOG(MY_LOGGER), "Error sending outbound_response");
                response_code = OC_EH_ERROR;
            }
        }
    }
    if (flag & OC_OBSERVE_FLAG)
    {
        OIC_LOG(INFO, TAG, "Flag includes OC_OBSERVE_FLAG");

        /* if (OC_OBSERVE_REGISTER == inbound_stimulus->obsInfo.action) */
        /* { */
        /*     clog_info(CLOG(MY_LOGGER), "Received OC_OBSERVE_REGISTER from client"); */
        /*     ProcessObserveRegister (inbound_stimulus); */
        /* } */
        /* else if (OC_OBSERVE_DEREGISTER == inbound_stimulus->obsInfo.action) */
        /* { */
        /*     clog_info(CLOG(MY_LOGGER), "Received OC_OBSERVE_DEREGISTER from client"); */
        /*     ProcessObserveDeregister (inbound_stimulus); */
        /* } */
    }

    OCPayloadDestroy(outbound_response.payload);
    return response_code;
}

OCStackResult createLightResource() {
    Light.power = false;
    OCStackResult res = OCCreateResource(&Light.handle,
					 "core.light",
					 "core.rw",
					 "/a/led",
					 light_react, /* handler */
					 NULL,
					 OC_SECURE | /* Transport-level security */
					 OC_NONSECURE |
					 OC_DISCOVERABLE |
					 OC_OBSERVABLE);

    clog_info(CLOG(MY_LOGGER), "%s EXIT", __func__);
    return res;
}

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}


static char SVR_CONFIG_FILE[] = "test/crudn/server_config.cbor";
//static char SVR_CONFIG_FILE[] = "server_config.cbor";

/* local fopen for svr database overrides default filename */
FILE* server_fopen(const char *path, const char *mode)
{
    clog_info(CLOG(MY_LOGGER), "%s path %s", __func__, path);

    if (0 == strcmp(path, SVR_DB_DAT_FILE_NAME)) { /* "oic_svr_db.dat" */
        clog_info(CLOG(MY_LOGGER), "%s opening %s", __func__, SVR_CONFIG_FILE);
	FILE *f = fopen(SVR_CONFIG_FILE, mode);
	if (f == NULL) {
            clog_info(CLOG(MY_LOGGER), "PS file open failed %d %s", errno, strerror(errno));
	    exit(EXIT_FAILURE);
	}
	return f;
    }
    else
    {
	return fopen(path, mode);
    }
}

int main() {
    // OCLogInit(NULL);    /* log to stdout */
    /* logfd = fopen("./logs/server.log", "w"); */
    /* OCLogHookFd(logfd); */
    /* Initialize the logger */
    int r;
    r = clog_init_path(MY_LOGGER, "server.log");
    if (r != 0) {
        fprintf(stderr, "clog initialization failed.\n");
        return 1;
    }

    /* Set minimum log level to info (default: debug) */
    clog_set_level(MY_LOGGER, CLOG_INFO);

    clog_info(CLOG(MY_LOGGER), "HELLO, %s!", "world");

    OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };

    // if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK) {
    if (oocf_init(OC_SERVER, &ps) != OC_STACK_OK) {
        clog_info(CLOG(MY_LOGGER), "OCStack init error");
        clog_free(MY_LOGGER);
        return 0;
    }
    fprintf(stdout, "OpenOCF logfile: %s\n", oocf_get_logfile_name());
    fflush(stdout);
    clog_info(CLOG(MY_LOGGER), "logfile: %s", oocf_get_logfile_name());

    OIC_LOG(INFO, TAG, "OCStack initialized");

    /*
     * Declare and create the example resource: Light
     */
    if(createLightResource() != OC_STACK_OK)
    {
        clog_error(CLOG(MY_LOGGER), "OCStack cannot create resource...");
    }

    // Break from loop with Ctrl-C
    OIC_LOG(INFO, TAG, "Entering ocserver main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag) {

        if (OCProcess() != OC_STACK_OK) {
            clog_error(CLOG(MY_LOGGER), "OCStack process error");
            clog_free(MY_LOGGER);
            return 0;
        }

        sleep(1);
    }

    OIC_LOG(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK) {
        clog_error(CLOG(MY_LOGGER), "OCStack process error");
    }

    clog_free(MY_LOGGER);
    return 0;
}
