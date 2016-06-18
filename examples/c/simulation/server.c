//******************************************************************
//
// Copyright 2016 NORC at the University of Chicago
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "server.h"

typedef OCEntityHandlerResult (* ServiceRoutine) (OCEntityHandlerRequest *ehRequest,
						  OCRepPayload **payload);

typedef struct observers
{
    OCObservationId observationId;
    bool valid;
    OCResourceHandle resourceHandle;
} observers_t;


/*
 * Temperature Instrument Manager
 */
/* code to manipulate the hw */

/*
 *  Light Servlet
 */
typedef struct properties_light
{
  OCResourceHandle handle;
  bool state;
  int power;
  OCEntityHandler	svc_temp_servlet;
  ServiceRoutine	svc_temp_create;
  ServiceRoutine	svc_temp_retrieve;
  ServiceRoutine	svc_temp_update;
  ServiceRoutine	svc_temp_delete;
  ServiceRoutine	svc_temp_notify_register;
  ServiceRoutine	svc_temp_notify_deregister;
} servlet_temp_t;

/* CREATE service routine */
OCEntityHandlerResult
svc_temp_create (OCEntityHandlerRequest *oic_request,
		 OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;
}

/* RETRIEVE service routine */
OCEntityHandlerResult
svc_temp_retrieve (OCEntityHandlerRequest *oic_request,
		   OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;
}

/* UPDATE service routine */
OCEntityHandlerResult
svc_temp_update (OCEntityHandlerRequest *oic_request,
		 OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;
}

/* DELETE service routine */
OCEntityHandlerResult
svc_temp_delete (OCEntityHandlerRequest *oic_request,
		 OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;
}

OCEntityHandlerResult
svc_temp_servlet (OCEntityHandlerFlag flag,
		  OCEntityHandlerRequest *oic_request, /* just like HttpRequest */
		  char* uri,
		  void* cb /*callbackParam*/)
{
    OIC_LOG_V (INFO, TAG, "Inside temperature servlet main service routine - flags: 0x%x, uri: %s", flag, uri);

    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response;
}

int register_servlet (char *uri,
		      properties *state,
		      ServiceRoutine svc_service)
{
}

int register_servlet_2 (char *uri,
			properties *state,
			ServiceRoutine svc_service,
			bool param,
			int  other_param)
{
}

/* **************************************************************** */
static void PrintUsage()
{
    OIC_LOG(INFO, TAG, "Usage : ocserver -o <0|1>");
    OIC_LOG(INFO, TAG, "-o 0 : Notify all observers");
    OIC_LOG(INFO, TAG, "-o 1 : Notify list of observers");
}

int main(int argc, char* argv[])
{
    int opt = 0;
    while ((opt = getopt(argc, argv, "o:s:p:d:u:w:r:j:")) != -1)
    {
        switch(opt)
        {
            case 'o':
                g_observe_notify_type = atoi(optarg);
                break;
            default:
                PrintUsage();
                return -1;
        }
    }

    if ((g_observe_notify_type != 0) && (g_observe_notify_type != 1))
    {
        PrintUsage();
        return -1;
    }

    OIC_LOG(DEBUG, TAG, "OCServer is starting...");

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    OCSetDefaultDeviceEntityHandler(svc_temp_servlet, NULL);

    OCStackResult registration_result = OCSetPlatformInfo(platform_info);

    if (registration_result != OC_STACK_OK)
    {
        OIC_LOG(INFO, TAG, "Platform Registration failed!");
        exit (EXIT_FAILURE);
    }

    OCResourcePayloadAddStringLL(&device_info.types, "oic.d.tv");

    registration_result = OCSetDeviceInfo(device_info);

    if (registration_result != OC_STACK_OK)
    {
        OIC_LOG(INFO, TAG, "Device Registration failed!");
        exit (EXIT_FAILURE);
    }

    /*
     * Declare and create the example resource: Light
     */
    //createLightResource(gResourceUri, &Light);
    register_servlet();

    // Initialize observations data structure for the resource
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        interestedObservers[i].valid = false;
    }


    /*
     * Create a thread for generating changes that cause presence notifications
     * to be sent to clients
     */

    #ifdef WITH_PRESENCE
    pthread_create(&threadId_presence, NULL, presenceNotificationGenerator, (void *)NULL);
    #endif

    // Break from loop with Ctrl-C
    OIC_LOG(INFO, TAG, "Entering ocserver main loop...");

    DeletePlatformInfo();
    DeleteDeviceInfo();

    signal(SIGINT, handleSigInt);

    while (!gQuitFlag)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }
    }

    if (observeThreadStarted)
    {
        pthread_cancel(threadId_observe);
        pthread_join(threadId_observe, NULL);
    }

    pthread_cancel(threadId_presence);
    pthread_join(threadId_presence, NULL);

    OIC_LOG(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack process error");
    }

    return 0;
}
