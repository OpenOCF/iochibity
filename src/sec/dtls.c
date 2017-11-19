#include "dtls.h"

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
#if INTERFACE
/**
 * @enum CADtlsPskCredType_t
 * Type of PSK credential required during DTLS handshake
 * It does not make much sense in bringing in all definitions from dtls.h into here.
 * Therefore, redefining them here.
 */
typedef enum
{
    CA_DTLS_PSK_HINT,
    CA_DTLS_PSK_IDENTITY,
    CA_DTLS_PSK_KEY
} CADtlsPskCredType_t;

/**
 * Binary structure containing PKIX related info
 * own certificate chain, public key, CA's and CRL's
 * The data member of each ByteArray_t must be allocated with OICMalloc or OICCalloc.
 * The SSL adapter takes ownership of this memory and will free it internally after use.
 * Callers should not reference this memory after it has been provided to the SSL adapter via the
 * callback.
 */
typedef struct
{
    ByteArray_t crt;    /**< own certificate chain as a null-terminated PEM string of certificates */
    ByteArray_t key;    /**< own private key as binary-encoded DER */
    ByteArray_t ca;     /**< trusted CAs as a null-terminated PEM string of certificates */
    ByteArray_t crl;    /**< trusted CRLs as binary-encoded DER */
} PkiInfo_t;

/**
 * This internal callback is used by CA layer to
 * retrieve PSK credentials from SRM.
 *
 * @param[in]  type type of PSK data required by CA layer during DTLS handshake set.
 * @param[in]  desc    Additional request information.
 * @param[in]  desc_len The actual length of desc.
 * @param[out] result  Must be filled with the requested information.
 * @param[in]  result_length  Maximum size of @p result.
 *
 * @return The number of bytes written to @p result or a value
 *         less than zero on error.
 */
typedef int (*CAgetPskCredentialsHandler)(CADtlsPskCredType_t type,
              const uint8_t *desc, size_t desc_len,
              uint8_t *result, size_t result_length);

/**
 * @brief   Callback function type for getting PKIX info
 *
 * @param   inf[out]   PKIX related info
 *
 * @return  NONE
 */
typedef void (*CAgetPkixInfoHandler)(PkiInfo_t * inf);

/**
 * This internal callback is used by CA layer to
 * retrieve all credential types from SRM
 *
 * @param[out]  list of enabled credential types for CA handshake.
 * @param[in]   device uuid.
 *
 */
#endif	/* INTERFACE */

#if INTERFACE
typedef void (*CAgetCredentialTypesHandler)(bool * list, const char* deviceId);
#endif	/* INTERFACE */

int32_t GetDtlsPskCredentials(CADtlsPskCredType_t type,
              const uint8_t *desc, size_t desc_len,
              uint8_t *result, size_t result_length)
{
    int32_t ret = -1;

    if (NULL == result)
    {
        return ret;
    }

    switch (type)
    {
        case CA_DTLS_PSK_HINT:
        case CA_DTLS_PSK_IDENTITY:
            {
                OicUuid_t deviceID = {.id={0}};
                // Retrieve Device ID from doxm resource
                if ( OC_STACK_OK != GetDoxmDeviceID(&deviceID) )
                {
                    OIC_LOG (ERROR, TAG, "Unable to retrieve doxm Device ID");
                    return ret;
                }

                if (result_length < sizeof(deviceID.id))
                {
                    OIC_LOG (ERROR, TAG, "Wrong value for result_length");
                    return ret;
                }
                memcpy(result, deviceID.id, sizeof(deviceID.id));
                return (sizeof(deviceID.id));
            }
            break;

        case CA_DTLS_PSK_KEY:
            {
                OicSecCred_t *cred = NULL;
                LL_FOREACH(gCred, cred)
                {
                    if (cred->credType != SYMMETRIC_PAIR_WISE_KEY)
                    {
                        continue;
                    }

                    if ((desc_len == sizeof(cred->subject.id)) &&
                        (memcmp(desc, cred->subject.id, sizeof(cred->subject.id)) == 0))
                    {
                        /*
                         * If the credentials are valid for limited time,
                         * check their expiry.
                         */
                        if (cred->period)
                        {
                            if(IOTVTICAL_VALID_ACCESS != IsRequestWithinValidTime(cred->period, NULL))
                            {
                                OIC_LOG (INFO, TAG, "Credentials are expired.");
                                return ret;
                            }
                        }

                        // Copy PSK.
                        // TODO: Added as workaround. Will be replaced soon.
                        if(OIC_ENCODING_RAW == cred->privateData.encoding)
                        {
                            if (ValueWithinBounds(cred->privateData.len, INT32_MAX))
                            {
                                size_t len = cred->privateData.len;
                                if (result_length < len)
                                {
                                    OIC_LOG (ERROR, TAG, "Wrong value for result_length");
                                    return ret;
                                }
                                memcpy(result, cred->privateData.data, len);
                                ret = (int32_t)len;
                            }
                        }
                        else if(OIC_ENCODING_BASE64 == cred->privateData.encoding)
                        {
                            size_t outBufSize = B64DECODE_OUT_SAFESIZE((cred->privateData.len + 1));
                            uint8_t* outKey = OICCalloc(1, outBufSize);
                            size_t outKeySize;
                            if(NULL == outKey)
                            {
                                OIC_LOG (ERROR, TAG, "Failed to allocate memory.");
                                return ret;
                            }

                            if(B64_OK == b64Decode((char*)cred->privateData.data, cred->privateData.len, outKey, outBufSize, &outKeySize))
                            {
                                if (ValueWithinBounds(outKeySize, INT32_MAX))
                                {
                                    if (result_length < outKeySize)
                                    {
                                        OIC_LOG (ERROR, TAG, "Wrong value for result_length");
                                        return ret;
                                    }
                                    memcpy(result, outKey, outKeySize);
                                    ret = (int32_t)outKeySize;
                                }
                            }
                            else
                            {
                                OIC_LOG (ERROR, TAG, "Failed base64 decoding.");
                            }

                            OICFree(outKey);
                        }

                        if (OC_STACK_OK != RegisterSymmetricCredentialRole(cred))
                        {
                            OIC_LOG(WARNING, TAG, "Couldn't RegisterRoleForSubject");
                        }

                        return ret;
                    }
                }
                OIC_LOG(DEBUG, TAG, "Can not find subject matched credential.");

#ifdef MULTIPLE_OWNER
                const OicSecDoxm_t* doxm = GetDoxmResourceData();
                if(doxm && doxm->mom && OIC_MULTIPLE_OWNER_DISABLE != doxm->mom->mode)
                {
                    // in case of multiple owner transfer authentication
                    if(OIC_PRECONFIG_PIN == doxm->oxmSel)
                    {
                        OicSecCred_t* wildCardCred = GetCredResourceData(&WILDCARD_SUBJECT_ID);
                        if(wildCardCred)
                        {
                            OIC_LOG(DEBUG, TAG, "Detected wildcard credential.");
                            if(PIN_PASSWORD == wildCardCred->credType)
                            {
                                //Read PIN/PW
                                char* pinBuffer = NULL;
                                size_t pinLength = 0;
                                if(OIC_ENCODING_RAW == wildCardCred->privateData.encoding)
                                {
                                    pinBuffer = OICCalloc(1, wildCardCred->privateData.len + 1);
                                    if(NULL == pinBuffer)
                                    {
                                        OIC_LOG (ERROR, TAG, "Failed to allocate memory.");
                                        return ret;
                                    }
                                    pinLength = wildCardCred->privateData.len;
                                    memcpy(pinBuffer, wildCardCred->privateData.data, pinLength);
                                }
                                else if(OIC_ENCODING_BASE64 == wildCardCred->privateData.encoding)
                                {
                                    size_t pinBufSize = B64DECODE_OUT_SAFESIZE((wildCardCred->privateData.len + 1));
                                    pinBuffer = OICCalloc(1, pinBufSize);
                                    if(NULL == pinBuffer)
                                    {
                                        OIC_LOG (ERROR, TAG, "Failed to allocate memory.");
                                        return ret;
                                    }

                                    if(B64_OK != b64Decode((char*)wildCardCred->privateData.data, wildCardCred->privateData.len, (uint8_t*)pinBuffer, pinBufSize, &pinLength))
                                    {
                                        OIC_LOG (ERROR, TAG, "Failed to base64 decoding.");
                                        return ret;
                                    }
                                }
                                else
                                {
                                    OIC_LOG(ERROR, TAG, "Unknown encoding type of PIN/PW credential.");
                                    return ret;
                                }

                                //Set the PIN/PW to derive PSK
                                if (OC_STACK_OK != SetPreconfigPin(pinBuffer, pinLength))
                                {
                                    OICFree(pinBuffer);
                                    OIC_LOG(ERROR, TAG, "Failed to load PIN data.");
                                    return ret;
                                }
                                OICFree(pinBuffer);

                                OicUuid_t myUuid;
                                if(OC_STACK_OK != GetDoxmDeviceID(&myUuid))
                                {
                                    OIC_LOG(ERROR, TAG, "Failed to read device ID");
                                    return ret;
                                }
                                SetUuidForPinBasedOxm(&myUuid);

                                //Calculate PSK using PIN/PW
                                if(0 == DerivePSKUsingPIN((uint8_t*)result))
                                {
                                    ret = OWNER_PSK_LENGTH_128;
                                }
                                else
                                {
                                    OIC_LOG_V(ERROR, TAG, "Failed to derive crypto key from PIN");
                                }

                                if(CA_STATUS_OK != CAregisterSslHandshakeCallback(MultipleOwnerDTLSHandshakeCB))
                                {
                                    OIC_LOG(WARNING, TAG, "Error while bind the DTLS Handshake Callback.");
                                }
                            }
                        }
                    }
                    else if(OIC_RANDOM_DEVICE_PIN == doxm->oxmSel)
                    {
                        if(0 == DerivePSKUsingPIN((uint8_t*)result))
                        {
                            ret = OWNER_PSK_LENGTH_128;
                        }
                        else
                        {
                            OIC_LOG_V(ERROR, TAG, "Failed to derive crypto key from PIN : result");
                            ret = -1;
                        }
                    }
                }
#endif //MULTIPLE_OWNER
            }
            break;
    }

    return ret;
}

#endif /* __WITH_DTLS__ */
