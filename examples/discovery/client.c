#include "openocf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#define TAG ("client")

int gQuitFlag = 0;

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}

// This is a function called back when a device is discovered
OCStackApplicationResult resource_discovery_cb(OCClientResponse * clientResponse)
{
    (void)clientResponse;
    OIC_LOG(INFO, TAG, "Entering applicationDiscoverCB (Application Layer CB)");
    OIC_LOG_V(INFO, TAG, "Device =============> Discovered %s @ %s:%d",
                                    clientResponse->resourceUri,
                                    clientResponse->devAddr.addr,
                                    clientResponse->devAddr.port);
    //return OC_STACK_DELETE_TRANSACTION;
    return OC_STACK_KEEP_TRANSACTION;
}

int main()
{
    OIC_LOG_V(INFO, TAG, "Starting occlient");

    /* Initialize OCStack*/
    if (OCInit(NULL, 0, OC_CLIENT) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    /* Start a discovery query*/
    OCCallbackData cbData;
    cbData.cb = resource_discovery_cb;
    cbData.context = NULL;
    cbData.cd = NULL;
    char szQueryUri[MAX_QUERY_LENGTH] = { 0 };
    strcpy(szQueryUri, OC_MULTICAST_DISCOVERY_URI);
    OIC_LOG_V(INFO, TAG, "Starting Discovery >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    if (OCDoResource(NULL,
		     OC_REST_DISCOVER,
		     szQueryUri,
		     NULL, NULL,
		     CT_DEFAULT,
		     OC_LOW_QOS,
		     &cbData, NULL, 0) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    // Break from loop with Ctrl+C
    OIC_LOG(INFO, TAG, "Entering occlient main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag) {

        if (OCProcess() != OC_STACK_OK) {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }

        sleep(1);
    }

    OIC_LOG(INFO, TAG, "Exiting occlient main loop...");

    if (OCStop() != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
    }

    return 0;
}
