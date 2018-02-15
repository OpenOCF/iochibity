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

static char SVR_CONFIG_FILE[] = "tmp/server_config.cbor";

/* local fopen for svr database overrides default filename */
FILE* server_fopen(const char *path, const char *mode)
{
    if (0 == strcmp(path, SVR_DB_DAT_FILE_NAME)) /* "oic_svr_db.dat" */
    {
        return fopen(SVR_CONFIG_FILE, mode);
    }
    else
    {
        return fopen(path, mode);
    }
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

OCStackResult createLightResource() {
    Light.power = false;
    OCStackResult res = OCCreateResource(&Light.handle,
					 "core.light",
					 "core.rw",
					 "/a/led",
					 0,
					 NULL,
					 OC_SECURE | /* Transport-level security */
					 OC_NONSECURE |
					 OC_DISCOVERABLE |
					 OC_OBSERVABLE);
    return res;
}
