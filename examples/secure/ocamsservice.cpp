
#include "iotivity_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <signal.h>
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#include "ocstack.h"
#include "experimental/logger.h"
#include "common.h"

#define TAG  PCF("SRM-AMSS")

int gQuitFlag = 0;

//AMS service database, hold AMS service Identity and
//the PSK credentials of trusted devices
static char AMSS_DB_FILE[] = "oic_amss_db.dat";

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum)
{
    if (signum == SIGINT)
    {
        gQuitFlag = 1;
    }
}

FILE* service_fopen(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(AMSS_DB_FILE, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}

int main(int /*argc*/, char* /*argv*/[])
{
    struct timespec timeout;

    OIC_LOG(DEBUG, TAG, "OCAMS service is starting...");

    // Initialize Persistent Storage for SVR database
    OCPersistentStorage ps = { service_fopen, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000000L;

    // Break from loop with Ctrl-C
    OIC_LOG(INFO, TAG, "Entering ocamsservice main loop...");
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

    OIC_LOG(INFO, TAG, "Exiting ocamsservice main loop...");

    if (OCStop() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack process error");
    }

    return 0;
}
