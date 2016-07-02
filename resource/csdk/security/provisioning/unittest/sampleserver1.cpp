/******************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/
///////////////////////////////////////////////////////////////////////
//NOTE :  This sample server is generated based on ocserverbasicops.cpp
///////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "ocstack.h"
#include "logger.h"
#include "ocpayload.h"
#include "oic_string.h"

#define TAG "UNITTEST_SERVER_1"

int gQuitFlag = 0;

/* Structure to represent a LED resource */
typedef struct LEDRESOURCE{
    OCResourceHandle handle;
    bool state;
    int power;
} LEDResource;

static LEDResource LED;
// This variable determines instance number of the LED resource.
// Used by POST method to create a new instance of LED resource.
static int gCurrLedInstance = 0;
#define SAMPLE_MAX_NUM_POST_INSTANCE  2
static LEDResource gLedInstance[SAMPLE_MAX_NUM_POST_INSTANCE];

char *gResourceUri= (char *)"/a/led";

//Secure Virtual Resource database for Iotivity Server
//It contains Server's Identity and the PSK credentials
//of other devices which the server trusts
static char CRED_FILE[] = "oic_svr_db_server1.dat";

static uint8_t DEFAULT_SVR_DB[] = {
    0xBF, 0x63, 0x61, 0x63, 0x6C, 0x59, 0x02, 0x76, 0xA2, 0x66, 0x61, 0x63, 0x6C, 0x69, 0x73, 0x74,
    0xA1, 0x64, 0x61, 0x63, 0x65, 0x73, 0x84, 0xA3, 0x6B, 0x73, 0x75, 0x62, 0x6A, 0x65, 0x63, 0x74,
    0x75, 0x75, 0x69, 0x64, 0x61, 0x2A, 0x69, 0x72, 0x65, 0x73, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x73,
    0x86, 0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x68, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x72, 0x65, 0x73,
    0x63, 0x72, 0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60, 0x62, 0x69, 0x66, 0x60, 0xA4, 0x64, 0x68,
    0x72, 0x65, 0x66, 0x66, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x64, 0x63, 0x72, 0x65, 0x6C, 0x60, 0x62,
    0x72, 0x74, 0x60, 0x62, 0x69, 0x66, 0x60, 0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x66, 0x2F, 0x6F,
    0x69, 0x63, 0x2F, 0x70, 0x63, 0x72, 0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60, 0x62, 0x69, 0x66,
    0x60, 0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x6A, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x72, 0x65, 0x73,
    0x2F, 0x64, 0x63, 0x72, 0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60, 0x62, 0x69, 0x66, 0x60, 0xA4,
    0x64, 0x68, 0x72, 0x65, 0x66, 0x70, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x72, 0x65, 0x73, 0x2F, 0x74,
    0x79, 0x70, 0x65, 0x73, 0x2F, 0x64, 0x63, 0x72, 0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60, 0x62,
    0x69, 0x66, 0x60, 0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x6D, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x70,
    0x72, 0x65, 0x73, 0x65, 0x6E, 0x63, 0x65, 0x63, 0x72, 0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60,
    0x62, 0x69, 0x66, 0x60, 0x6A, 0x70, 0x65, 0x72, 0x6D, 0x69, 0x73, 0x73, 0x69, 0x6F, 0x6E, 0x02,
    0xA3, 0x6B, 0x73, 0x75, 0x62, 0x6A, 0x65, 0x63, 0x74, 0x75, 0x75, 0x69,  0x64, 0x61, 0x2A, 0x69,
    0x72, 0x65, 0x73, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x73, 0x84, 0xA4, 0x64, 0x68, 0x72, 0x65, 0x66,
    0x6D, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x73, 0x65, 0x63, 0x2F, 0x64, 0x6F, 0x78, 0x6D, 0x63, 0x72,
    0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60, 0x62, 0x69, 0x66, 0x60, 0xA4, 0x64, 0x68, 0x72, 0x65,
    0x66, 0x6E, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x73, 0x65, 0x63, 0x2F, 0x70, 0x73, 0x74, 0x61, 0x74,
    0x63, 0x72, 0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60, 0x62, 0x69, 0x66, 0x60, 0xA4, 0x64, 0x68,
    0x72, 0x65, 0x66, 0x6C, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x73, 0x65, 0x63, 0x2F, 0x61, 0x63, 0x6C,
    0x63, 0x72, 0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60, 0x62, 0x69, 0x66, 0x60, 0xA4, 0x64, 0x68,
    0x72, 0x65, 0x66, 0x6D, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x73, 0x65, 0x63, 0x2F, 0x63, 0x72, 0x65,
    0x64, 0x63, 0x72, 0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60, 0x62, 0x69, 0x66, 0x60, 0x6A, 0x70,
    0x65, 0x72, 0x6D, 0x69, 0x73, 0x73, 0x69, 0x6F, 0x6E, 0x06, 0xA3, 0x6B, 0x73, 0x75, 0x62, 0x6A,
    0x65, 0x63, 0x74, 0x75, 0x75, 0x69, 0x64, 0x61, 0x2A, 0x69, 0x72, 0x65, 0x73, 0x6F, 0x75, 0x72,
    0x63, 0x65, 0x73, 0x82, 0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x6E, 0x2F, 0x6F, 0x69, 0x63, 0x2F,
    0x73, 0x65, 0x63, 0x2F, 0x70, 0x63, 0x6F, 0x6E, 0x66, 0x63, 0x72, 0x65, 0x6C, 0x60, 0x62, 0x72,
    0x74, 0x60, 0x62, 0x69, 0x66, 0x60, 0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x71, 0x2F, 0x6F, 0x69,
    0x63, 0x2F, 0x73, 0x65, 0x63, 0x2F, 0x64, 0x70, 0x61, 0x69, 0x72, 0x69, 0x6E, 0x67, 0x63, 0x72,
    0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60, 0x62, 0x69, 0x66, 0x60, 0x6A, 0x70, 0x65, 0x72, 0x6D,
    0x69, 0x73, 0x73, 0x69, 0x6F, 0x6E, 0x06, 0xA3, 0x6B, 0x73, 0x75, 0x62, 0x6A, 0x65, 0x63, 0x74,
    0x75, 0x75, 0x69, 0x64, 0x61, 0x2A, 0x69, 0x72, 0x65, 0x73, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x73,
    0x81, 0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x6C, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x73, 0x65, 0x63,
    0x2F, 0x76, 0x65, 0x72, 0x63, 0x72, 0x65, 0x6C, 0x60, 0x62, 0x72, 0x74, 0x60, 0x62, 0x69, 0x66,
    0x60, 0x6A, 0x70, 0x65, 0x72, 0x6D, 0x69, 0x73, 0x73, 0x69, 0x6F, 0x6E, 0x02, 0x6A, 0x72, 0x6F,
    0x77, 0x6E, 0x65, 0x72, 0x75, 0x75, 0x69, 0x64, 0x78, 0x24, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31,
    0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x65, 0x70,
    0x73, 0x74, 0x61, 0x74, 0x58, 0x79, 0xA7, 0x64, 0x69, 0x73, 0x6F, 0x70, 0xF4, 0x6A, 0x64, 0x65,
    0x76, 0x69, 0x63, 0x65, 0x75, 0x75, 0x69, 0x64, 0x78, 0x24, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31,
    0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x62, 0x63,
    0x6D, 0x02, 0x62, 0x74, 0x6D, 0x00, 0x62, 0x6F, 0x6D, 0x03, 0x62, 0x73, 0x6D, 0x03, 0x6A, 0x72,
    0x6F, 0x77, 0x6E, 0x65, 0x72, 0x75, 0x75, 0x69, 0x64, 0x78, 0x24, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31,
    0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x64,
    0x64, 0x6F, 0x78, 0x6D, 0x58, 0xB6, 0xA8, 0x64, 0x6F, 0x78, 0x6D, 0x73, 0x81, 0x00, 0x66, 0x6F,
    0x78, 0x6D, 0x73, 0x65, 0x6C, 0x00, 0x63, 0x73, 0x63, 0x74, 0x01, 0x65, 0x6F, 0x77, 0x6E, 0x65,
    0x64, 0xF4, 0x6A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x75, 0x75, 0x69, 0x64, 0x78, 0x24, 0x31,
    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31,
    0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x31, 0x31, 0x31, 0x6C, 0x64, 0x65, 0x76, 0x6F, 0x77, 0x6E, 0x65, 0x72, 0x75, 0x75, 0x69, 0x64,
    0x78, 0x24, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2D, 0x30, 0x30, 0x30, 0x30, 0x2D,
    0x30, 0x30, 0x30, 0x30, 0x2D, 0x30, 0x30, 0x30, 0x30, 0x2D, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x6A, 0x72, 0x6F, 0x77, 0x6E, 0x65, 0x72, 0x75, 0x75, 0x69,
    0x64, 0x78, 0x24, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31,
    0x2D, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x2D, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x63, 0x64, 0x70, 0x63, 0xF5, 0xFF
};

/* Function that creates a new LED resource by calling the
 * OCCreateResource() method.
 */
int createLEDResource (char *uri, LEDResource *ledResource, bool resourceState, int resourcePower);

/* This method converts the payload to JSON format */
OCRepPayload* constructResponse (OCEntityHandlerRequest *ehRequest);

/* Following methods process the PUT, GET, POST
 * requests
 */
OCEntityHandlerResult ProcessGetRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);
OCEntityHandlerResult ProcessPutRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);
OCEntityHandlerResult ProcessPostRequest (OCEntityHandlerRequest *ehRequest,
                                        OCEntityHandlerResponse *response,
                                        OCRepPayload **payload);

/* Entity Handler callback functions */
OCEntityHandlerResult
OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        void* callbackParam);

const char *getResult(OCStackResult result) {
    switch (result) {
    case OC_STACK_OK:
        return "OC_STACK_OK";
    case OC_STACK_RESOURCE_CREATED:
        return "OC_STACK_RESOURCE_CREATED";
    case OC_STACK_RESOURCE_DELETED:
        return "OC_STACK_RESOURCE_DELETED";
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
    #ifdef WITH_PRESENCE
    case OC_STACK_PRESENCE_STOPPED:
        return "OC_STACK_PRESENCE_STOPPED";
    #endif
    case OC_STACK_ERROR:
        return "OC_STACK_ERROR";
    default:
        return "UNKNOWN";
    }
}

OCRepPayload* getPayload(const char* uri, int64_t power, bool state)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate Payload");
        return NULL;
    }

    OCRepPayloadSetUri(payload, uri);
    OCRepPayloadSetPropBool(payload, "state", state);
    OCRepPayloadSetPropInt(payload, "power", power);

    return payload;
}

//This function takes the request as an input and returns the response
OCRepPayload* constructResponse (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, "Incoming payload not a representation");
        return NULL;
    }

    OCRepPayload* input = (OCRepPayload*)(ehRequest->payload);

    LEDResource *currLEDResource = &LED;

    if (ehRequest->resource == gLedInstance[0].handle)
    {
        currLEDResource = &gLedInstance[0];
        gResourceUri = (char *) "/a/led/0";
    }
    else if (ehRequest->resource == gLedInstance[1].handle)
    {
        currLEDResource = &gLedInstance[1];
        gResourceUri = (char *) "/a/led/1";
    }

    if(OC_REST_PUT == ehRequest->method)
    {
        // Get pointer to query
        int64_t pow;
        if(OCRepPayloadGetPropInt(input, "power", &pow))
        {
            currLEDResource->power =pow;
        }

        bool state;
        if(OCRepPayloadGetPropBool(input, "state", &state))
        {
            currLEDResource->state = state;
        }
    }

    return getPayload(gResourceUri, currLEDResource->power, currLEDResource->state);
}

OCEntityHandlerResult ProcessGetRequest (OCEntityHandlerRequest *ehRequest,
        OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructResponse(ehRequest);

    if(getResp)
    {
        *payload = getResp;
        ehResult = OC_EH_OK;
    }
    else
    {
        ehResult = OC_EH_ERROR;
    }

    return ehResult;
}

OCEntityHandlerResult ProcessPutRequest (OCEntityHandlerRequest *ehRequest,
        OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *putResp = constructResponse(ehRequest);

    if(putResp)
    {
        *payload = putResp;
        ehResult = OC_EH_OK;
    }
    else
    {
        ehResult = OC_EH_ERROR;
    }

    return ehResult;
}

OCEntityHandlerResult ProcessPostRequest (OCEntityHandlerRequest *ehRequest,
        OCEntityHandlerResponse *response, OCRepPayload **payload)
{
    OCRepPayload *respPLPost_led = NULL;
    OCEntityHandlerResult ehResult = OC_EH_OK;

    /*
     * The entity handler determines how to process a POST request.
     * Per the REST paradigm, POST can also be used to update representation of existing
     * resource or create a new resource.
     * In the sample below, if the POST is for /a/led then a new instance of the LED
     * resource is created with default representation (if representation is included in
     * POST payload it can be used as initial values) as long as the instance is
     * lesser than max new instance count. Once max instance count is reached, POST on
     * /a/led updated the representation of /a/led (just like PUT)
     */

    if (ehRequest->resource == LED.handle)
    {
        if (gCurrLedInstance < SAMPLE_MAX_NUM_POST_INSTANCE)
        {
            // Create new LED instance
            char newLedUri[15] = "/a/led/";
            int newLedUriLength = strlen(newLedUri);
            snprintf (newLedUri + newLedUriLength, sizeof(newLedUri)-newLedUriLength, "%d", gCurrLedInstance);

            respPLPost_led = OCRepPayloadCreate();
            OCRepPayloadSetUri(respPLPost_led, gResourceUri);
            OCRepPayloadSetPropString(respPLPost_led, "createduri", newLedUri);

            if (0 == createLEDResource (newLedUri, &gLedInstance[gCurrLedInstance], false, 0))
            {
                OIC_LOG (INFO, TAG, "Created new LED instance");
                gLedInstance[gCurrLedInstance].state = 0;
                gLedInstance[gCurrLedInstance].power = 0;
                gCurrLedInstance++;
                strncpy ((char *)response->resourceUri, newLedUri, MAX_URI_LENGTH);
                ehResult = OC_EH_RESOURCE_CREATED;
            }
        }
        else
        {
            respPLPost_led = constructResponse(ehRequest);
        }
    }
    else
    {
        for (int i = 0; i < SAMPLE_MAX_NUM_POST_INSTANCE; i++)
        {
            if (ehRequest->resource == gLedInstance[i].handle)
            {
                if (i == 0)
                {
                    respPLPost_led = constructResponse(ehRequest);
                    break;
                }
                else if (i == 1)
                {
                    respPLPost_led = constructResponse(ehRequest);
                }
            }
        }
    }

    if (respPLPost_led != NULL)
    {
        *payload = respPLPost_led;
        ehResult = OC_EH_OK;
    }
    else
    {
        OIC_LOG_V (INFO, TAG, "Payload was NULL");
        ehResult = OC_EH_ERROR;
    }

    return ehResult;
}

OCEntityHandlerResult
OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        void* callbackParam)
{
    OIC_LOG_V (INFO, TAG, "Inside entity handler - flags: 0x%x", flag);
    (void)callbackParam;
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    OCEntityHandlerResponse response;
    memset(&response, 0, sizeof(response));

    // Validate pointer
    if (!entityHandlerRequest)
    {
        OIC_LOG (ERROR, TAG, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    OCRepPayload* payload = NULL;

    if (flag & OC_REQUEST_FLAG)
    {
        OIC_LOG (INFO, TAG, "Flag includes OC_REQUEST_FLAG");
        if (entityHandlerRequest)
        {
            if (OC_REST_GET == entityHandlerRequest->method)
            {
                OIC_LOG (INFO, TAG, "Received OC_REST_GET from client");
                ehResult = ProcessGetRequest (entityHandlerRequest, &payload);
            }
            else if (OC_REST_PUT == entityHandlerRequest->method)
            {
                OIC_LOG (INFO, TAG, "Received OC_REST_PUT from client");
                ehResult = ProcessPutRequest (entityHandlerRequest, &payload);
            }
            else if (OC_REST_POST == entityHandlerRequest->method)
            {
                OIC_LOG (INFO, TAG, "Received OC_REST_POST from client");
                ehResult = ProcessPostRequest (entityHandlerRequest, &response, &payload);
            }
            else
            {
                OIC_LOG_V (INFO, TAG, "Received unsupported method %d from client",
                        entityHandlerRequest->method);
                ehResult = OC_EH_ERROR;
            }

            if (ehResult == OC_EH_OK && ehResult != OC_EH_FORBIDDEN)
            {
                // Format the response.  Note this requires some info about the request
                response.requestHandle = entityHandlerRequest->requestHandle;
                response.resourceHandle = entityHandlerRequest->resource;
                response.ehResult = ehResult;
                response.payload = (OCPayload*)(payload);
                response.numSendVendorSpecificHeaderOptions = 0;
                memset(response.sendVendorSpecificHeaderOptions, 0,
                       sizeof(response.sendVendorSpecificHeaderOptions));
                memset(response.resourceUri, 0, sizeof(response.resourceUri));
                // Indicate that response is NOT in a persistent buffer
                response.persistentBufferFlag = 0;

                // Send the response
                if (OCDoResponse(&response) != OC_STACK_OK)
                {
                    OIC_LOG(ERROR, TAG, "Error sending response");
                    ehResult = OC_EH_ERROR;
                }
            }
        }
    }

    OCPayloadDestroy(response.payload);
    return ehResult;
}

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum)
{
    if (signum == SIGINT)
    {
        gQuitFlag = 1;
    }
}

static void GetCurrentWorkingDirectory(char* buf, size_t bufsize)
{
    char cwd[1024] = {0};
    const char* unittest_path = "resource/csdk/security/provisioning/unittest";
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        if(strstr(cwd, unittest_path) == NULL)
        {
#if defined __linux__
#if __x86_64__
        snprintf(buf, bufsize, "%s/out/linux/x86_64/release/%s/", cwd, unittest_path);
        snprintf(buf, bufsize, "%s/out/linux/x86_64/release/%s/", cwd, unittest_path);
#else
        snprintf(buf, bufsize, "%s/out/linux/x86/release/%s/", cwd, unittest_path);
        snprintf(buf, bufsize, "%s/out/linux/x86/release/%s/", cwd, unittest_path);
#endif //__x86_64__
#endif //defined __linux__
        }
        else
        {
            snprintf(buf, bufsize, "%s/", cwd);
        }
    }
}

FILE* server_fopen(const char *path, const char *mode)
{
    (void)path;
    char cwd[1024] = {0};
    char cred_path[1024] = {0};
    GetCurrentWorkingDirectory(cwd, sizeof(cwd));
    snprintf(cred_path, sizeof(cred_path), "%s%s", cwd, CRED_FILE);
    return fopen(cred_path, mode);
}

int main()
{
    struct timespec timeout;

    //Delete previous SVR DB, if exist.
    char cwd[1024] = {0};
    char cred_path[1024] = {0};
    char del_cmd[1024] = {0};
    FILE* fp = NULL;
    GetCurrentWorkingDirectory(cwd, sizeof(cwd));
    snprintf(del_cmd, sizeof(del_cmd), "rm -rf %s%s", cwd, CRED_FILE);
    system(del_cmd);

    //Generate default SVR DB.
    snprintf(cred_path, sizeof(cred_path), "%s%s", cwd, CRED_FILE);
    fp = fopen(cred_path, "w");
    if(NULL != fp)
    {
        size_t numberItems = fwrite(DEFAULT_SVR_DB, 1, sizeof(DEFAULT_SVR_DB), fp);
        if(sizeof(DEFAULT_SVR_DB) != numberItems)
        {
            OIC_LOG_V(ERROR, TAG, "Failed to initialize SVR DB (%s)", cred_path);
            fclose(fp);
            return -1;
        }
        fclose(fp);
    }

    OIC_LOG(DEBUG, TAG, "OCServer is starting...");

    // Initialize Persistent Storage for SVR database
    OCPersistentStorage ps = {server_fopen, fread, fwrite, fclose, unlink};

    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    /*
     * Declare and create the example resource: LED
     */
    createLEDResource(gResourceUri, &LED, false, 0);

    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000000L;

    // Break from loop with Ctrl-C
    OIC_LOG(INFO, TAG, "Entering ocserver main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }
        nanosleep(&timeout, NULL);
    }

    OIC_LOG(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack process error");
    }

    return 0;
}

int createLEDResource (char *uri, LEDResource *ledResource, bool resourceState, int resourcePower)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    ledResource->state = resourceState;
    ledResource->power= resourcePower;
    OCStackResult res = OCCreateResource(&(ledResource->handle),
            "core.led",
            OC_RSRVD_INTERFACE_DEFAULT,
            uri,
            OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE|OC_OBSERVABLE | OC_SECURE);
    OIC_LOG_V(INFO, TAG, "Created LED resource with result: %s", getResult(res));

    return 0;
}
