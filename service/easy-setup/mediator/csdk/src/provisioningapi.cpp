//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <sstream>

#include "provisioningapi.h"
#include "oic_string.h"

#define ES_PROV_ADAP_TAG "ES_PROVISIONING_ADAPTER"

//Use ipv4addr for both InitDiscovery and InitDeviceDiscovery
char ipv4addr[IPV4_ADDR_SIZE] =
{ 0 };

#ifdef REMOTE_ARDUINO_ENROLEE
//Arduino Enrollee needs mediator application provide IP and port55555 which is specific
// to Arduino WiFi enrollee
static const char * UNICAST_PROVISIONING_QUERY = "coap://%s:%d/oic/res?rt=oic.r.prov";
#else
static const char * UNICAST_PROVISIONING_QUERY = "/oic/res?rt=oic.r.prov";
#endif

volatile static OCProvisioningStatusCB cbData = NULL;

OCStackResult InitProvProcess()
{

    OCStackResult result = OC_STACK_ERROR;

    if (InitProvisioningHandler() == OC_STACK_OK)
    {
        result = OC_STACK_OK;
        OIC_LOG(DEBUG, ES_PROV_ADAP_TAG, "InitProvisioningHandler returned Success");
    }
    else
    {
        result = OC_STACK_ERROR;
        OIC_LOG_V(ERROR, ES_PROV_ADAP_TAG, "InitProvisioningHandler returned error = %d",
                result);
    }

    return result;
}

OCStackResult ResetProvProcess()
{
    return TerminateProvisioningHandler();
}

OCStackResult RegisterCallback(OCProvisioningStatusCB provisioningStatusCallback)
{
    OCStackResult result = OC_STACK_OK;

    if (provisioningStatusCallback != NULL)
    {
        cbData = provisioningStatusCallback;
    }
    else
    {
        result = OC_STACK_ERROR;
        OIC_LOG(ERROR, ES_PROV_ADAP_TAG, "provisioningStatusCallback is NULL");
    }

    return result;
}

void UnRegisterCallback()
{
    if (cbData)
    {
        cbData = NULL;
    }
}

OCStackResult StartProvisioning(const ProvConfig *provConfig, WiFiOnboadingConnection *onboardConn)
{

    char findQuery[MAX_QUERY_LENGTH] =
    { 0 };

    if (provConfig == NULL || onboardConn == NULL)
    {
        return OC_STACK_ERROR;
    }

#ifdef REMOTE_ARDUINO_ENROLEE
    //Arduino Enrollee needs mediator application provide IP and port55555 which is specific
    // to Arduino WiFi enrollee
    snprintf(findQuery, sizeof(findQuery) - 1, UNICAST_PROVISIONING_QUERY,
            onboardConn->ipAddress, IP_PORT);
#else
    OICStrcpy(findQuery, sizeof(findQuery) - 1, UNICAST_PROVISIONING_QUERY);
#endif

    return StartProvisioningProcess(provConfig, onboardConn, cbData, findQuery);
}

OCStackResult StopProvisioning(OCConnectivityType /*connectivityType*/)
{
    OCStackResult result = OC_STACK_OK;

    StopProvisioningProcess();

    return result;
}
