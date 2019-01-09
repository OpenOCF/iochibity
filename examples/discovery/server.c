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

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}


//static char SVR_CONFIG_FILE[] = "tools/browser/server/server_config.cbor";
static char SVR_CONFIG_FILE[] = "config/server_config.cbor";

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
    r = clog_init_path(MY_LOGGER, "logs/server.log");
    if (r != 0) {
        fprintf(stderr, "clog initialization failed.\n");
        return 1;
    }

    /* Set minimum log level to info (default: debug) */
    clog_set_level(MY_LOGGER, CLOG_INFO);

    clog_info(CLOG(MY_LOGGER), "HELLO, %s!", "world");

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK) {
        clog_info(CLOG(MY_LOGGER), "OCStack init error");
        clog_free(MY_LOGGER);
        return 0;
    }
    fprintf(stdout, "OpenOCF logfile: %s\n", oocf_get_logfile_name());
    fflush(stdout);
    clog_info(CLOG(MY_LOGGER), "logfile: %s", oocf_get_logfile_name());

    OIC_LOG(INFO, TAG, "OCStack initialized");

    OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);

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
    while (!gQuitFlag) {

        if (OCProcess() != OC_STACK_OK) {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            clog_free(MY_LOGGER);
            return 0;
        }

        sleep(1);
    }

    OIC_LOG(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack process error");
    }

    clog_free(MY_LOGGER);
    return 0;
}

OCStackResult createLightResource() {
    Light.power = false;
    OCStackResult res = OCCreateResource(&Light.handle,
					 "core.light",
					 "core.rw",
					 "/a/light",
					 0,
					 NULL,
					 OC_SECURE|OC_DISCOVERABLE|OC_OBSERVABLE);
    return res;
}
