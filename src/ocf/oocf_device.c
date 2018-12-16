#include "oocf_device.h"

void CleanUpDeviceProperties(OCDeviceProperties **deviceProperties)
{
    if (!deviceProperties || !(*deviceProperties))
    {
        return;
    }

    OICFreeAndSetToNull((void**)(deviceProperties));
}

OCStackResult CreateDeviceProperties(const char *piid, OCDeviceProperties **deviceProperties)
{
    OIC_LOG(DEBUG, TAG, "CreateDeviceProperties IN");

    OCStackResult result = OC_STACK_OK;

    if (!piid || !deviceProperties)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *deviceProperties = (OCDeviceProperties*)OICCalloc(1, sizeof(OCDeviceProperties));
    if (*deviceProperties)
    {
        OICStrcpy((*deviceProperties)->protocolIndependentId, UUID_STRING_SIZE, piid);
    }
    else
    {
        result = OC_STACK_NO_MEMORY;
    }

    OIC_LOG(DEBUG, TAG, "CreateDeviceProperties OUT");

    return result;
}

OCStackResult GenerateDeviceProperties(OCDeviceProperties **deviceProperties)
{
    OCStackResult result = OC_STACK_OK;
    OicUuid_t generatedProtocolIndependentId = {.id = {0}};
    char* protocolIndependentId = NULL;

    if (!deviceProperties)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *deviceProperties = NULL;

    // Generate a UUID for the Protocol Independent ID
    if (OCGenerateUuid(generatedProtocolIndependentId.id))
    {
        protocolIndependentId = (char*)OICCalloc(UUID_STRING_SIZE, sizeof(char));
        if (protocolIndependentId)
        {
            if (!OCConvertUuidToString(generatedProtocolIndependentId.id, protocolIndependentId))
            {
                OIC_LOG(ERROR, TAG, "ConvertUuidToStr failed");
                result = OC_STACK_ERROR;
            }
        }
        else
        {
            result = OC_STACK_NO_MEMORY;
        }
    }
    else
    {
        OIC_LOG(FATAL, TAG, "Generate UUID for Device Properties Protocol Independent ID failed!");
        result = OC_STACK_ERROR;
    }

    if (OC_STACK_OK == result)
    {
        result = CreateDeviceProperties(protocolIndependentId, deviceProperties);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CreateDeviceProperties failed");
        }
    }

    // Clean Up
    OICFreeAndSetToNull((void**)&protocolIndependentId);

    return result;
}

OCStackResult OC_CALL OCGetDeviceId(OCUUIdentity *deviceId)
{
    OicUuid_t oicUuid;
    OCStackResult ret = OC_STACK_ERROR;

    ret = GetDoxmDeviceID(&oicUuid);
    if (OC_STACK_OK == ret)
    {
        memcpy(deviceId, &oicUuid, UUID_IDENTITY_SIZE);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Device ID Get error");
    }
    return ret;
}

OCStackResult OC_CALL OCSetDeviceId(const OCUUIdentity *deviceId)
{
    OicUuid_t oicUuid;
    OCStackResult ret = OC_STACK_ERROR;

    memcpy(&oicUuid, deviceId, UUID_LENGTH);
    for (int i = 0; i < UUID_LENGTH; i++)
    {
        OIC_LOG_V(INFO, TAG, "Set Device Id %x", oicUuid.id[i]);
    }
    ret = SetDoxmDeviceID(&oicUuid);
    return ret;
}

OCStackResult OC_CALL OCGetDeviceOwnedState(bool *isOwned)
{
    bool isDeviceOwned = true;
    OCStackResult ret = OC_STACK_ERROR;

    ret = GetDoxmIsOwned(&isDeviceOwned);
    if (OC_STACK_OK == ret)
    {
        *isOwned = isDeviceOwned;
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Device Owned State Get error");
    }
    return ret;
}




