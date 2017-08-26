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
#include <string.h>
#include "credentialgenerator.h"
#include "base64.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocpayload.h"
#include "payload_logging.h"
#include "credresource.h"
#include "ocrandom.h"
#include "srmutility.h"
#include "stdbool.h"

#define TAG "OIC_SRPAPI_CG"

OCStackResult PMGeneratePairWiseCredentials(OicSecCredType_t type, size_t keySize,
        const OicUuid_t *ptDeviceId, const OicUuid_t *firstDeviceId,
        const OicUuid_t *secondDeviceId, 
        const OicSecRole_t *firstRole, const OicSecRole_t *secondRole,
        OicSecCred_t **firstCred, OicSecCred_t **secondCred)
{
    if (NULL == ptDeviceId || NULL == firstDeviceId || NULL == firstCred || NULL != *firstCred || \
        NULL == secondDeviceId || NULL == secondCred || NULL != *secondCred)
    {
        OIC_LOG(INFO, TAG, "Invalid params");
        return OC_STACK_INVALID_PARAM;
    }
    if (!(keySize == OWNER_PSK_LENGTH_128 || keySize == OWNER_PSK_LENGTH_256))
    {
        OIC_LOG(INFO, TAG, "Invalid key size");
        return OC_STACK_INVALID_PARAM;
    }
    OCStackResult res = OC_STACK_ERROR;
    OicSecCred_t *tempFirstCred = NULL;
    OicSecCred_t *tempSecondCred = NULL;

    size_t privDataKeySize = keySize;

    uint8_t *privData = (uint8_t *)OICCalloc(privDataKeySize, sizeof(uint8_t));
    VERIFY_NOT_NULL(TAG, privData, ERROR);

    OicSecKey_t privKey;
    memset(&privKey, 0, sizeof(privKey));
    privKey.data = privData;
    privKey.len = keySize;
    privKey.encoding = OIC_ENCODING_RAW;

    if (!OCGetRandomBytes(privData, privDataKeySize))
    {
        OIC_LOG(ERROR, TAG, "Failed to generate private key");
        res = OC_STACK_ERROR;
        goto exit;
    }

    // TODO: currently owner array is 1. only provisioning tool's id.
    tempFirstCred =  GenerateCredential(secondDeviceId, type, NULL, &privKey, ptDeviceId, NULL);
    VERIFY_NOT_NULL(TAG, tempFirstCred, ERROR);

    // TODO: currently owner array is 1. only provisioning tool's id.
    tempSecondCred =  GenerateCredential(firstDeviceId, type, NULL, &privKey, ptDeviceId, NULL);
    VERIFY_NOT_NULL(TAG, tempSecondCred, ERROR);

    // firstRole and secondRole are the roles granted to the client when authenticating with this credential;
    // therefore, the role to be granted has to be stored on the corresponding server. This is why secondRole
    // is assigned to tempFirstCred and vice versa.
    if (NULL != secondRole)
    {
        tempFirstCred->roleId = *secondRole;
    }
    if (NULL != firstRole)
    {
        tempSecondCred->roleId = *firstRole;
    }

    *firstCred = tempFirstCred;
    *secondCred = tempSecondCred;
    res = OC_STACK_OK;

exit:
    OICClearMemory(privData, privDataKeySize);
    OICFree(privData);

    if (res != OC_STACK_OK)
    {
        OICFree(tempFirstCred);
        OICFree(tempSecondCred);
        *firstCred = NULL;
        *secondCred = NULL;
    }

    return res;
}
