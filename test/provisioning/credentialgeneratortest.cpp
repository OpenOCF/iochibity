/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include "gtest/gtest.h"
#include "credentialgenerator.h"
#include "oic_malloc.h"


TEST(PMGeneratePairWiseCredentialsTest, InvalidProvisioningtoolDevID)
{
    OicUuid_t *firstDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(firstDevID)
    {
        firstDevID->id[0] = 1;
    }
    OicUuid_t *SecondDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(SecondDevID)
    {
        SecondDevID->id[0] = 2;
    }
    OicSecCred_t *cred1 = NULL;
    OicSecCred_t *cred2 = NULL;
    size_t keySize = OWNER_PSK_LENGTH_128;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PMGeneratePairWiseCredentials(NO_SECURITY_MODE,
             keySize, NULL, firstDevID, SecondDevID, &cred1, &cred2));
    OICFree(firstDevID);
    OICFree(SecondDevID);
}

TEST(PMGeneratePairWiseCredentialsTest, InvalidFirstDevID)
{
    OicUuid_t *SecondDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(SecondDevID)
    {
        SecondDevID->id[0] = 2;
    }
    OicUuid_t *provisioningDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(provisioningDevID)
    {
        provisioningDevID->id[0] = 1;
    }
    OicSecCred_t *cred1 = NULL;
    OicSecCred_t *cred2 = NULL;
    size_t keySize = OWNER_PSK_LENGTH_128;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PMGeneratePairWiseCredentials(NO_SECURITY_MODE,
              keySize, provisioningDevID, NULL, SecondDevID, &cred1, &cred2));
    OICFree(SecondDevID);
    OICFree(provisioningDevID);
}

TEST(PMGeneratePairWiseCredentialsTest, InvalidSecondDevID)
{
    OicUuid_t *firstDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(firstDevID)
    {
        firstDevID->id[0] = 1;
    }
    OicUuid_t *provisioningDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(provisioningDevID)
    {
        provisioningDevID->id[0] = 2;
    }
    OicSecCred_t *cred1 = NULL;
    OicSecCred_t *cred2 = NULL;
    size_t keySize = OWNER_PSK_LENGTH_128;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PMGeneratePairWiseCredentials(NO_SECURITY_MODE, keySize,
              provisioningDevID, firstDevID, NULL, &cred1, &cred2));
    OICFree(firstDevID);
    OICFree(provisioningDevID);
}

TEST(PMGeneratePairWiseCredentialsTest, InvalidCred)
{
    OicUuid_t *firstDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(firstDevID)
    {
        firstDevID->id[0] = 1;
    }
    OicUuid_t *SecondDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(SecondDevID)
    {
        SecondDevID->id[0] = 2;
    }
    OicUuid_t *provisioningDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(provisioningDevID)
    {
        provisioningDevID->id[0] = 3;
    }
    size_t keySize = OWNER_PSK_LENGTH_128;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PMGeneratePairWiseCredentials(NO_SECURITY_MODE, keySize,
              provisioningDevID, firstDevID, SecondDevID, NULL, NULL));
    OICFree(firstDevID);
    OICFree(SecondDevID);
    OICFree(provisioningDevID);
}

#ifdef __WITH_X509__

TEST(PMGenerateCertificateCredentialsTest, InvalidProvisioningtoolDevID)
{
    OicUuid_t *DevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(DevID)
    {
        DevID->id[0] = 1;
    }
    OicSecCred_t *cred = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PMGenerateCertificateCredentials(NULL, DevID, &cred));
    OICFree(DevID);
}

TEST(PMGenerateCertificateCredentialsTest, InvalidDevID)
{
    OicUuid_t *provisioningDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(provisioningDevID)
    {
        provisioningDevID->id[0] = 1;
    }
    OicSecCred_t *cred = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PMGenerateCertificateCredentials(provisioningDevID,
              NULL, &cred));
    OICFree(provisioningDevID);
}

TEST(PMGenerateCertificateCredentialsTest, InvalidCred)
{
    OicUuid_t *provisioningDevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(provisioningDevID)
    {
        provisioningDevID->id[0] = 1;
    }
    OicUuid_t *DevID = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    if(DevID)
    {
        DevID->id[0] = 1;
    }
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PMGenerateCertificateCredentials(provisioningDevID, DevID,
    NULL));
    OICFree(provisioningDevID);
    OICFree(DevID);
}

#endif // __WITH_X509__

