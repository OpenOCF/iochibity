#include "oocf_device.h"

#include "cbor.h"

/**
 * Default cbor payload size. This value is increased in case of CborErrorOutOfMemory.
 * The value of payload size is increased up to CBOR_MAX_SIZE.
 */
static const uint16_t CBOR_SIZE = 512;

/**
 * Max cbor size payload.
 */
static const uint16_t CBOR_MAX_SIZE = 4400;

OCStackResult InitializeDeviceProperties(void)
{
    OIC_LOG(DEBUG, TAG, "InitializeDeviceProperties IN");

    OCStackResult result = OC_STACK_OK;
    OCDeviceProperties *deviceProperties = NULL;
    bool updateDatabase = false;

    // Populate OCDeviceProperties from persistent storage
    result = ReadDevicePropertiesFromDatabase(&deviceProperties);
    if (OC_STACK_OK != result)
    {
        OIC_LOG(ERROR, TAG, "ReadDevicePropertiesFromDatabase failed");
    }

    // If the device properties in persistent storage are corrupted or
    // not available for some reason, a default OCDeviceProperties is created.
    if ((OC_STACK_OK != result) || !deviceProperties)
    {
        result = GenerateDeviceProperties(&deviceProperties);
        if (OC_STACK_OK == result)
        {
            updateDatabase = true;
        }
        else
        {
            OIC_LOG(ERROR, TAG, "GenerateDeviceProperties failed");
        }
    }

    // Set the device properties information on the device info resource. This will
    // also persist OCDeviceProperties so they can be used in the future if they are
    // not already in the database.
    if (OC_STACK_OK == result)
    {
        result = UpdateDeviceInfoResourceWithDeviceProperties(deviceProperties, updateDatabase);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(ERROR, TAG, "UpdateDeviceInfoResourceWithDeviceProperties failed");
        }
    }

    // Clean Up
    CleanUpDeviceProperties(&deviceProperties);

    OIC_LOG(DEBUG, TAG, "InitializeDeviceProperties OUT");

    return result;
}

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

LOCAL OCStackResult UpdateDeviceInfoResourceWithDeviceProperties(const OCDeviceProperties *deviceProperties, bool updateDatabase)
{
    OCStackResult result = OC_STACK_OK;
    OCResource *resource = NULL;

    if (!deviceProperties)
    {
        return OC_STACK_INVALID_PARAM;
    }

    resource = FindResourceByUri(OC_RSRVD_DEVICE_URI);
    if (resource)
    {
        result = SetAttributeInternal(resource, OC_RSRVD_PROTOCOL_INDEPENDENT_ID, deviceProperties->protocolIndependentId, updateDatabase);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(ERROR, TAG, "OCSetPropertyValue failed to set Protocol Independent ID");
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Resource does not exist.");
        result = OC_STACK_NO_RESOURCE;
    }

    return result;
}

LOCAL OCStackResult ReadDevicePropertiesFromDatabase(OCDeviceProperties **deviceProperties)
{
    OIC_LOG_V(INFO, TAG, "%s: ENTRY", __func__);
    uint8_t *data = NULL;
    size_t size = 0;

    /* read device_properties.dat unless overridden by user */
    /* key:  DeviceProperties */
    OCStackResult result = ReadDatabaseFromPS(OC_DEVICE_PROPS_FILE_NAME, OC_JSON_DEVICE_PROPS_NAME, &data, &size);
    if (OC_STACK_OK == result)
    {
        // Read device properties from PS
        result = CBORPayloadToDeviceProperties(data, size, deviceProperties);
        if (OC_STACK_OK != result)
        {
            OIC_LOG(WARNING, TAG, "CBORPayloadToDeviceProperties failed");
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "ReadDatabaseFromPS failed");
    }

    // Clean Up
    OICFreeAndSetToNull((void**)&data);

    return result;
}

LOCAL OCStackResult UpdateDevicePropertiesDatabase(const OCDeviceProperties *deviceProperties)
{
    OCStackResult result = OC_STACK_OK;
    uint8_t *payload = NULL;
    size_t size = 0;

    if (!deviceProperties)
    {
        return OC_STACK_INVALID_PARAM;
    }

    // Check to see if persistent storage exists. If it doesn't then
    // we just allow the device properties to exist in memory and
    // it is the application's job to manage them.
    if (!OCGetPersistentStorageHandler())
    {
        OIC_LOG(DEBUG, TAG, "Persistent Storage handler is NULL.");
        return OC_STACK_OK;
    }

    // Convert OCDeviceProperties into CBOR to use for updating Persistent Storage
    result = DevicePropertiesToCBORPayload(deviceProperties, &payload, &size);
    if ((OC_STACK_OK == result) && payload)
    {
        result = UpdateResourceInPS(OC_DEVICE_PROPS_FILE_NAME, OC_JSON_DEVICE_PROPS_NAME, payload, size);
        if (OC_STACK_OK != result)
        {
            OIC_LOG_V(ERROR, TAG, "UpdateResourceInPS failed with %d!", result);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "DevicePropertiesToCBORPayload failed with %d!", result);
    }

    // Clean Up
    OICFreeAndSetToNull((void**)&payload);

    return result;
}

OCStackResult OC_CALL OCSetDeviceInfo(OCDeviceInfo info)
EXPORT
{
    OCResource *resource = FindResourceByUri(OC_RSRVD_DEVICE_URI);
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Device Resource does not exist.");
        goto exit;
    }
    if (!info.deviceName || info.deviceName[0] == '\0')
    {
        OIC_LOG(ERROR, TAG, "Null or empty device name.");
       return OC_STACK_INVALID_PARAM;
    }

    if (OCGetServerInstanceIDString() == NULL)
    {
        OIC_LOG(INFO, TAG, "Device ID generation failed");
        goto exit;
    }

    VERIFY_SUCCESS_1(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME, info.deviceName));
    for (OCStringLL *temp = info.types; temp; temp = temp->next)
    {
        if (temp->value)
        {
            VERIFY_SUCCESS_1(OCBindResourceTypeToResource(resource, temp->value));
        }
    }
    VERIFY_SUCCESS_1(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_SPEC_VERSION, info.specVersion ?
        info.specVersion: OC_SPEC_VERSION));

    if (info.dataModelVersions)
    {
        char *dmv = OCCreateString(info.dataModelVersions);
        VERIFY_PARAM_NON_NULL(TAG, dmv, "Failed allocating dataModelVersions");
        OCStackResult r = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DATA_MODEL_VERSION, dmv);
        OICFree(dmv);
        VERIFY_SUCCESS_1(r);
    }
    else
    {
        VERIFY_SUCCESS_1(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DATA_MODEL_VERSION,
            OC_DATA_MODEL_VERSION));
    }
    OIC_LOG(INFO, TAG, "Device parameter initialized successfully.");
    return OC_STACK_OK;

exit:
    return OC_STACK_ERROR;
}

LOCAL OCStackResult DevicePropertiesToCBORPayload(const OCDeviceProperties *deviceProperties, uint8_t **payload, size_t *size)
{
    OCStackResult result = OC_STACK_OK;
    CborError cborResult = CborNoError;
    uint8_t *cborPayload = NULL;
    size_t cborLen = CBOR_SIZE;
    CborEncoder encoder;
    CborEncoder dpMap;

    if (!deviceProperties || !payload || !size || (*size > CBOR_MAX_SIZE))
    {
        return OC_STACK_INVALID_PARAM;
    }

    // Reset the CBOR length if we need to
    if (*size > 0)
    {
        cborLen = *size;
    }

    *payload = NULL;
    *size = 0;

    cborPayload = (uint8_t*)OICCalloc(1, cborLen);
    if (NULL != cborPayload)
    {
        cbor_encoder_init(&encoder, cborPayload, cborLen, 0);

        // Create the Device Properties encoder map
        cborResult = cbor_encoder_create_map(&encoder, &dpMap, CborIndefiniteLength);
        if (CborNoError != cborResult)
        {
            OIC_LOG(ERROR, TAG, "Failed to create encoder map!");
            result = OC_STACK_ERROR;
        }
    }
    else
    {
        return OC_STACK_NO_MEMORY;
    }

    // Protocol Independent ID - Mandatory
    if (OC_STACK_OK == result)
    {
        cborResult = cbor_encode_text_string(&dpMap, OC_RSRVD_PROTOCOL_INDEPENDENT_ID, strlen(OC_RSRVD_PROTOCOL_INDEPENDENT_ID));
        if (CborNoError == cborResult)
        {
            cborResult = cbor_encode_text_string(&dpMap,
                                                 deviceProperties->protocolIndependentId,
                                                 strlen(deviceProperties->protocolIndependentId));
            if (CborNoError != cborResult)
            {
                OIC_LOG(ERROR, TAG, "Failed to encode protocolIndependentId!");
                result = OC_STACK_ERROR;
            }
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Failed to encode OC_RSRVD_PROTOCOL_INDEPENDENT_ID!");
            result = OC_STACK_ERROR;
        }
    }

    // Encoding is finished
    if (OC_STACK_OK == result)
    {
        cborResult = cbor_encoder_close_container(&encoder, &dpMap);
        if (CborNoError != cborResult)
        {
            OIC_LOG(ERROR, TAG, "Failed to close dpMap container!");
            result = OC_STACK_ERROR;
        }
    }

    if (OC_STACK_OK == result)
    {
        *size = cbor_encoder_get_buffer_size(&encoder, cborPayload);
        *payload = cborPayload;
        cborPayload = NULL;
    }
    else if ((CborErrorOutOfMemory == cborResult) && (cborLen < CBOR_MAX_SIZE))
    {
        OICFreeAndSetToNull((void**)&cborPayload);

        // Realloc and try again
        cborLen += cbor_encoder_get_buffer_size(&encoder, encoder.end);
        result = DevicePropertiesToCBORPayload(deviceProperties, payload, &cborLen);
        if (OC_STACK_OK == result)
        {
            *size = cborLen;
        }
    }
    else
    {
        OICFreeAndSetToNull((void**)&cborPayload);
    }

    return result;
}

LOCAL OCStackResult CBORPayloadToDeviceProperties(const uint8_t *payload, size_t size, OCDeviceProperties **deviceProperties)
{
    OCStackResult result = OC_STACK_OK;
    CborError cborResult = CborNoError;
    char* protocolIndependentId = NULL;
    CborParser parser;
    CborValue dpCbor;
    CborValue dpMap;

    if (!payload || (size <= 0) || !deviceProperties)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *deviceProperties = NULL;

    cbor_parser_init(payload, size, 0, &parser, &dpCbor);

    // piid: Protocol Independent ID - Mandatory
    cborResult = cbor_value_map_find_value(&dpCbor, OC_RSRVD_PROTOCOL_INDEPENDENT_ID, &dpMap);
    if ((CborNoError == cborResult) && cbor_value_is_text_string(&dpMap))
    {
        size_t len = 0;

        cborResult = cbor_value_dup_text_string(&dpMap, &protocolIndependentId, &len, NULL);
        if (CborNoError != cborResult)
        {
            OIC_LOG(ERROR, TAG, "Failed to get Protocol Independent Id!");
            result = OC_STACK_ERROR;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Protocol Independent Id is not present or invalid!");
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

