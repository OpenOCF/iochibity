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
#include <errno.h>

/* #include <ncurses.h> */

/* #include "ocstack.h"
 * #include "logger.h" */

#define TAG ("ocserver")

FILE                   *logfd;

static int g_quit_flag = 0;
OCStackResult createLightResource();

typedef struct LIGHTRESOURCE{
    OCResourceHandle handle;
    bool power;
} LightResource;

static LightResource Light;

/* SIGINT handler: set g_quit_flag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        g_quit_flag = 1;
    }
}

FILE *logfd;

static char SVR_CONFIG_FILE[] = "tools/browser/server/server_config.cbor";

/* local fopen for svr database overrides default filename */
FILE* server_fopen(const char *path, const char *mode)
{
    OIC_LOG_V(INFO, TAG, "%s path %s", __func__, path);

    if (0 == strcmp(path, SVR_DB_DAT_FILE_NAME)) { /* "oic_svr_db.dat" */
	OIC_LOG_V(INFO, TAG, "%s opening %s", __func__, SVR_CONFIG_FILE);
	FILE *f = fopen(SVR_CONFIG_FILE, mode);
	if (f == NULL) {
	    OIC_LOG_V(ERROR, TAG, "PS file open failed %d %s", errno, strerror(errno));
	    printf("PS file open \"%s\" failed errno %d %s\n", SVR_CONFIG_FILE, errno, strerror(errno));
	    exit(EXIT_FAILURE);
	}
	return f;
    }
    else
    {
	return fopen(path, mode);
    }
}

/* Spec: pg 34: A query string shall contain a list of <name>=<value>
 * segments (aka “name-value pair”) each separated by a ‘&’
 * (ampersand). The query string will be mapped to the appropriate
 * syntax of the protocol used for messaging. (e.g., CoAP)
 *
 * E.g. RETRIEVE /a/room/1?if=oic.if.b&ins=11111
 *
 * But CoAP says the query _may_ have the form "key=value", so
 * "key<value" etc. are ok?

 * See also 7.10 Query Parameters. Mentions but does not define "query
 * filters".
 */
bool resource_satisfies_query(char * query)
{
    /* return true if the resource satisfies the query */
    return true;
}

OCEntityHandlerResult put_light_react (OCEntityHandlerRequest *inbound_stimulus,
				       OCRepPayload **payload)
{
    return OC_EH_OK;
}

OCEntityHandlerResult get_light_react (OCEntityHandlerRequest *inbound_stimulus,
				       OCRepPayload **payload)
{
    OIC_LOG_V (INFO, TAG, "%s ENTRY", __func__);
    OIC_LOG_V (INFO, TAG, "%s inbound_stimulus resource handle: %p", __func__, inbound_stimulus->resource);
    OCEntityHandlerResult result;

    if (resource_satisfies_query(inbound_stimulus->query)) {
	/* OCRepPayload *getResp = constructResponse(inbound_stimulus); */
	/* if(!getResp) { */
	/*     OIC_LOG(ERROR, TAG, "constructResponse failed"); */
	/*     return OC_EH_ERROR; */
	/* } */
	if(inbound_stimulus->payload && inbound_stimulus->payload->type != PAYLOAD_TYPE_REPRESENTATION) {
	    OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
	    return OC_EH_ERROR;
	}

	OIC_LOG_V (INFO, TAG, "%s creating rep payload", __func__);
	/* OCRepPayload* _payload = OCRepPayloadCreate(); */
	/* if(!payload) { */
	/*     OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload")); */
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
	OCStackResult r = BuildResponseRepresentation(inbound_stimulus->resource,
						      &_payload, // OCRepPayload** payload,
						      &inbound_stimulus->devAddr);  // OCDevAddr *devAddr)
	OCRepPayloadSetPropBool(_payload, "foo", true);
	OCRepPayloadSetPropBool(_payload, "bar", false);

	*payload = _payload;
	result = OC_EH_OK;
    } else {
	/* return empty if query fails */
        result = OC_EH_OK;
    }

    OIC_LOG_V (INFO, TAG, "%s EXIT", __func__);
    return result;
}

OCEntityHandlerResult light_react (OCEntityHandlerFlag flag, /* OC_REQUEST_FLAG, OC_OBSERVE_FLAG */
				   OCEntityHandlerRequest *inbound_stimulus,
				   void* param)
{
    OIC_LOG_V (INFO, TAG, "%s ENTRY - flags: 0x%x", __func__, flag);
    if (inbound_stimulus->payload)
	OIC_LOG_V(DEBUG, TAG, "payload type: %x", inbound_stimulus->payload->type);
    else
	OIC_LOG_V(DEBUG, TAG, "no payload");

    OCEntityHandlerResult result = OC_EH_OK;
    OCEntityHandlerResponse outbound_response = { 0, 0, OC_EH_ERROR, 0, 0, { },{ 0 }, false };

    // Validate pointer
    if (!inbound_stimulus)
    {
        OIC_LOG (ERROR, TAG, "Invalid inbound OCEntityHandlerRequest pointer");
        return OC_EH_ERROR;
    }

    // Initialize certain outbound_response fields
    outbound_response.numSendVendorSpecificHeaderOptions = 0;
    memset(outbound_response.sendVendorSpecificHeaderOptions,
            0, sizeof outbound_response.sendVendorSpecificHeaderOptions);
    memset(outbound_response.resourceUri, 0, sizeof outbound_response.resourceUri);
    OCRepPayload* payload = NULL;

    if (flag & OC_REQUEST_FLAG)
    {
        OIC_LOG (INFO, TAG, "Flag includes OC_REQUEST_FLAG");

	switch(inbound_stimulus->method) {
	case OC_REST_GET:
            OIC_LOG (INFO, TAG, "Received OC_REST_GET from client");
            result = get_light_react(inbound_stimulus, &payload);
	    break;
	case OC_REST_DISCOVER:
            OIC_LOG (INFO, TAG, "Received OC_REST_DISCOVER from client");
            //result = get_light_react(inbound_stimulus, &payload);
	    break;
	default:
            OIC_LOG_V (INFO, TAG, "Received unsupported method %d from client",
		       inbound_stimulus->method);
            result = OC_EH_ERROR;
	}

        /* else if (OC_REST_PUT == inbound_stimulus->method) */
        /* { */
        /*     OIC_LOG (INFO, TAG, "Received OC_REST_PUT from client"); */
        /*     result = ProcessPutRequest (inbound_stimulus, &payload); */
        /* } */
        /* else if (OC_REST_POST == inbound_stimulus->method) */
        /* { */
        /*     OIC_LOG (INFO, TAG, "Received OC_REST_POST from client"); */
        /*     result = ProcessPostRequest (inbound_stimulus, &outbound_response, &payload); */
        /* } */
        /* else if (OC_REST_DELETE == inbound_stimulus->method) */
        /* { */
        /*     OIC_LOG (INFO, TAG, "Received OC_REST_DELETE from client"); */
        /*     result = ProcessDeleteRequest (inbound_stimulus); */
        /* } */
        /* else */
        /* { */
        /*     OIC_LOG_V (INFO, TAG, "Received unsupported method %d from client", */
        /*               inbound_stimulus->method); */
        /*     result = OC_EH_ERROR; */
        /* } */
        // If the result isn't an error or forbidden, send outbound_response
        if (!((result == OC_EH_ERROR) || (result == OC_EH_FORBIDDEN)))
        {
            // Format the outbound_response.  Note this requires some info about the request
            outbound_response.requestHandle = inbound_stimulus->requestHandle;
            outbound_response.ehResult = result;
            outbound_response.payload  = (OCPayload*)(payload);
            // Indicate that outbound_response is NOT in a persistent buffer
            outbound_response.persistentBufferFlag = 0;

            // Handle vendor specific options
            if(inbound_stimulus->rcvdVendorSpecificHeaderOptions &&
                    inbound_stimulus->numRcvdVendorSpecificHeaderOptions)
            {
                OIC_LOG (INFO, TAG, "Received vendor specific options");
                uint8_t i = 0;
                OCHeaderOption * rcvdOptions =
                        inbound_stimulus->rcvdVendorSpecificHeaderOptions;
                for( i = 0; i < inbound_stimulus->numRcvdVendorSpecificHeaderOptions; i++)
                {
                    if(((OCHeaderOption)rcvdOptions[i]).protocolID == OC_COAP_ID)
                    {
                        OIC_LOG_V(INFO, TAG, "Received option with OC_COAP_ID and ID %u with",
                                ((OCHeaderOption)rcvdOptions[i]).optionID );

                        OIC_LOG_BUFFER(INFO, TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
                            MAX_HEADER_OPTION_DATA_LENGTH);
                    }
                }
                // Check on Accept Version option.
                uint8_t vOptionData[MAX_HEADER_OPTION_DATA_LENGTH];
                size_t vOptionDataSize = sizeof(vOptionData);
                uint16_t actualDataSize = 0;
                OCGetHeaderOption(inbound_stimulus->rcvdVendorSpecificHeaderOptions,
				  inbound_stimulus->numRcvdVendorSpecificHeaderOptions,
				  OCF_ACCEPT_CONTENT_FORMAT_VERSION, // @was COAP_OPTION_ACCEPT_VERSION,
				  vOptionData, vOptionDataSize, &actualDataSize);
                if (actualDataSize)
                {
                    OIC_LOG_V(INFO, TAG, "accept version option exists");
                    OIC_LOG_BUFFER(INFO, TAG, vOptionData, MAX_HEADER_OPTION_DATA_LENGTH);
                    uint16_t acceptVersion = vOptionData[0]*256 + vOptionData[1];
                    if (OC_SPEC_VERSION_VALUE == acceptVersion)
                    {
                        OIC_LOG_V(INFO, TAG, "accept version equals to default OC_SPEC_VERSION_VALUE.");
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
                OIC_LOG(ERROR, TAG, "Error sending outbound_response");
                result = OC_EH_ERROR;
            }
        }
    }
    if (flag & OC_OBSERVE_FLAG)
    {
        OIC_LOG(INFO, TAG, "Flag includes OC_OBSERVE_FLAG");

        /* if (OC_OBSERVE_REGISTER == inbound_stimulus->obsInfo.action) */
        /* { */
        /*     OIC_LOG (INFO, TAG, "Received OC_OBSERVE_REGISTER from client"); */
        /*     ProcessObserveRegister (inbound_stimulus); */
        /* } */
        /* else if (OC_OBSERVE_DEREGISTER == inbound_stimulus->obsInfo.action) */
        /* { */
        /*     OIC_LOG (INFO, TAG, "Received OC_OBSERVE_DEREGISTER from client"); */
        /*     ProcessObserveDeregister (inbound_stimulus); */
        /* } */
    }

    OCPayloadDestroy(outbound_response.payload);
    return result;
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

    OIC_LOG_V (INFO, TAG, "%s EXIT", __func__);
    return res;
}

int main() {
    /* logfd = fopen("./logs/server.log", "w"); */
    /* if (logfd) */
    	OCLogInit(NULL);	/* log to stdout */
    /* else { */
    /* 	printf("fopen failed on ./logs/server.log\n"); */
    /* 	exit(EXIT_FAILURE); */
    /* } */

    // Step one: initialize Persistent Storage for SVR database
    OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);

    OIC_LOG_V(INFO, TAG, "Starting ocserver");
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }
    OIC_LOG(INFO, TAG, "OCStack initialized");

    /*
     * Declare and create the example resource: Light
     */
    if(createLightResource() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack cannot create resource...");
    }

    // Break from loop with Ctrl-C
    OIC_LOG(INFO, TAG, "Entering ocserver main loop...");
    signal(SIGINT, handleSigInt);
    while (!g_quit_flag) {

        if (OCProcess() != OC_STACK_OK) {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }

        sleep(1);
    }

    OIC_LOG(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack process error");
    }
    /* fclose(logfd); */
    return 0;
}
