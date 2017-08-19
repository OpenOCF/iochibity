/* *****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/
#include "cloudCommon.h"
#include "logger.h"
#include "cloudResource.h"

#define TAG "cloudServer"

extern LEDResource LED;
extern char *gResourceUri;

int main(int argc, char *argv[])
{
    if (!parseCommandLineArguments(argc, argv))
    {
        return -1;
    }

    if (OC_STACK_OK != initPersistentStorage())
    {
        return -2;
    }

    if (OC_STACK_OK != initProcess(OC_CLIENT_SERVER))
    {
        OIC_LOG(ERROR, TAG, "initProcess error, exit\n");
        return -3;
    }

    LED.state = false;
    LED.power = 0;
    /*
     * Declare and create the example resource: LED
     */
    if (OC_STACK_OK != createLEDResource(gResourceUri, &LED))
    {
        return -4;
    }

    OCMode mode = OC_SERVER;
    if (OC_STACK_OK != startRequestsThread(&mode))
    {
        return -5;
    }

    startProcess();

    freeThreadResources();

    return 0;
}
