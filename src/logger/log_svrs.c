
#include "log_svrs.h"

#include "utlist.h"

#include <mbedtls/x509_crt.h>


/* source: acl_logging.h */
void printACE(LogLevel level, const OicSecAce_t *ace)
{
    OIC_LOG(level, ACL_TAG, "=================================================");
    OIC_LOG_V(level, ACL_TAG, "ACE @ %p", ace);

    if (NULL == ace)
    {
        return;
    }

    OIC_LOG_V(level, ACL_TAG, "    aceid = %d", ace->aceid);

    OIC_LOG_V(level, ACL_TAG, "    permission = %#x", (uint32_t)ace->permission);

    // Log the subject
    if (ace->subjectType == OicSecAceUuidSubject)
    {
        char uuidString[UUID_STRING_SIZE] = { 0 };
        bool convertedUUID = OCConvertUuidToString(ace->subjectuuid.id, uuidString);
        OIC_LOG_V(level, ACL_TAG, "    subject UUID = %s", convertedUUID ? uuidString : "incorrect format");
    }
    else if (ace->subjectType == OicSecAceRoleSubject)
    {
        OIC_LOG_V(level, ACL_TAG, "    role id = %s", ace->subjectRole.id);
        OIC_LOG_V(level, ACL_TAG, "    authority = %s", ace->subjectRole.authority);
    }
    else if (ace->subjectType == OicSecAceConntypeSubject)
    {
        const char *conntype;
        if (ANON_CLEAR == ace->subjectConn)
        {
            conntype = "ANON_CLEAR";
        }
        else if (AUTH_CRYPT == ace->subjectConn)
        {
            conntype = "AUTH_CRYPT";
        }
        else
        {
            conntype = "Unknown conntype in subjectConn";
        }
        OIC_LOG_V(level, ACL_TAG, "    conntype = %s", conntype);
    }
    else
    {
        OIC_LOG(level, ACL_TAG, "    subject = (subject of unknown type)");
    }

    // Log all resources this ACE applies to.
    OicSecRsrc_t *resource = NULL;
    size_t resourceCount = 0;
    LL_FOREACH(ace->resources, resource)
    {
        OIC_LOG_V(level, ACL_TAG, "    resources[%" PRIuPTR "]:", resourceCount);
        OIC_LOG_V(level, ACL_TAG, "        href = %s", resource->href ? resource->href : "null");

        for (size_t i = 0; i < resource->typeLen; i++)
        {
            OIC_LOG_V(level, ACL_TAG, "        types[%" PRIuPTR "] = %s", i,
                resource->types[i] ? resource->types[i] : "null");
        }

        for (size_t i = 0; i < resource->interfaceLen; i++)
        {
            OIC_LOG_V(level, ACL_TAG, "        interfaces[%" PRIuPTR "] = %s", i,
                resource->interfaces[i] ? resource->interfaces[i] : "null");
        }

        resourceCount++;
    }

    // Log the validities.
    OicSecValidity_t *validity = NULL;
    size_t validityCount = 0;
    LL_FOREACH(ace->validities, validity)
    {
        OIC_LOG_V(level, ACL_TAG, "    validities[%" PRIuPTR "]:", validityCount);
        OIC_LOG_V(level, ACL_TAG, "        period = %s", validity->period);
        for (size_t i = 0; i < validity->recurrenceLen; i++)
        {
            OIC_LOG_V(level, ACL_TAG, "    recurrences[%" PRIuPTR "] = %s", i,
                validity->recurrences[i] ? validity->recurrences[i] : "null");
        }
        validityCount++;
    }

    OIC_LOG(level, ACL_TAG, "=================================================");
}

void printACL(LogLevel level, const OicSecAcl_t* acl)
{
    OIC_LOG_V(level, ACL_TAG, "Print ACL @ %p:", acl);

    if (NULL == acl)
    {
        return;
    }

    char rowner[UUID_STRING_SIZE] = { 0 };
    if (OCConvertUuidToString(acl->rownerID.id, rowner))
    {
        OIC_LOG_V(level, ACL_TAG, "rowner id = %s", rowner);
    }
    else
    {
        OIC_LOG(ERROR, ACL_TAG, "Can't convert rowner uuid to string");
    }

    const OicSecAce_t *ace = acl->aces;
    size_t ace_count = 0;
    while (ace)
    {
        OIC_LOG_V(level, ACL_TAG, "Print ace[%" PRIuPTR "]:", ace_count);
        printACE(level, ace);
        ace = ace->next;
        ace_count++;
    }
}
#if INTERFACE
#define OIC_LOG_ACL(level, acl) printACL((level),(acl))
#define OIC_LOG_ACE(level, ace) printACE((level),(ace))
#endif

/* Produce debugging output for all credentials, output metadata. */
void logCredMetadata(void)
{
    OicSecCred_t * temp = NULL;
    size_t count = 0;
    char uuidString[UUID_STRING_SIZE];
    char rownerUuidString[UUID_STRING_SIZE];
    OicUuid_t ownUuid;

    OIC_LOG_V(DEBUG, TAG, "IN %s:", __func__);

    if (GetDoxmDeviceID(&ownUuid) == OC_STACK_OK && OCConvertUuidToString(ownUuid.id, uuidString))
    {
        OIC_LOG_V(DEBUG, TAG, "Own UUID: %s", uuidString);
    }

    LL_FOREACH(gCred, temp)
    {
        count++;
        OIC_LOG(DEBUG, TAG, " ");
        OIC_LOG_V(DEBUG, TAG, "Cred ID: %d", temp->credId);
        if (OCConvertUuidToString(temp->subject.id, uuidString))
        {
            OIC_LOG_V(DEBUG, TAG, "Subject UUID: %s", uuidString);
        }
        if (OCConvertUuidToString(temp->rownerID.id, rownerUuidString))
        {
            OIC_LOG_V(DEBUG, TAG, "Rowner UUID: %s", rownerUuidString);
        }
        if (IsNonEmptyRole(&temp->roleId))
        {
            OIC_LOG_V(DEBUG, TAG, "Role ID: %s", temp->roleId.id);
            OIC_LOG_V(DEBUG, TAG, "Role authority: %s", temp->roleId.authority);
        }
        OIC_LOG_V(DEBUG, TAG, "Cred Type: %d", temp->credType);
        OIC_LOG_V(DEBUG, TAG, "privateData length: %" PRIuPTR ", encoding: %d", temp->privateData.len, temp->privateData.encoding);

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
        OIC_LOG_V(DEBUG, TAG, "publicData length: %" PRIuPTR ", encoding: %d", temp->publicData.len, temp->publicData.encoding);
        if (temp->credUsage)
        {
            OIC_LOG_V(DEBUG, TAG, "credUsage: %s", temp->credUsage);
        }

        OIC_LOG_V(DEBUG, TAG, "optionalData length: %" PRIuPTR", encoding: %d", temp->optionalData.len, temp->optionalData.encoding);
#endif

    }

    OIC_LOG_V(DEBUG, TAG, "Found %" PRIuPTR " credentials.", count);

    OIC_LOG_V(DEBUG, TAG, "OUT %s:", __func__);
}

void LogCert(uint8_t *data, size_t len, OicEncodingType_t encoding, const char* tag)
{
#if defined (__WITH_TLS__) || defined(__WITH_DTLS__)

#define CERT_INFO_BUF_LEN 4000

    char infoBuf[CERT_INFO_BUF_LEN];
    int mbedRet = 0;
    OCStackResult ret = OC_STACK_OK;
    size_t pemLen = 0;
    uint8_t *pem = NULL;
    mbedtls_x509_crt mbedCert;
    bool needTofreePem = false;

    if ((0 < len) && (NULL != data))
    {
        // extract PEM data
        if (OIC_ENCODING_PEM == encoding)
        {
            pem = data;
            pemLen = len;
        }
        else if (OIC_ENCODING_DER == encoding)
        {
            ret = ConvertDerCertToPem(data, len, &pem);
            if (OC_STACK_OK == ret )
            {
                pemLen = strlen((char*)pem) + 1;
                needTofreePem = true;
            }
            else
            {
                pemLen = 0;
            }
        }

        if ((NULL != pem) && (0 < pemLen))
        {
            // cert dump
            mbedtls_x509_crt_init(&mbedCert);
            mbedRet = mbedtls_x509_crt_parse(&mbedCert, pem, pemLen);
            if ( 0 <= mbedRet )
            {
                mbedRet = mbedtls_x509_crt_info(infoBuf, CERT_INFO_BUF_LEN, tag, &mbedCert);
                if (0 < mbedRet)
                {
                    size_t pos = strlen(infoBuf)-1;
                    if (infoBuf[pos] == '\n')
                    {
                        infoBuf[pos] = '\0';
                    }
                    OIC_LOG_V(DEBUG, tag, "%s", infoBuf);
                }
            }
            mbedtls_x509_crt_free(&mbedCert);

            // raw pem dump
            size_t pos = strlen((char *)pem)-1;
            if (pem[pos] == '\n')
            {
                pem[pos] = '\0';
            }
            snprintf(infoBuf, CERT_INFO_BUF_LEN, "\n%s", pem);
            OIC_LOG_V(DEBUG, tag, "%s", infoBuf);
        }
        if ( true == needTofreePem )
        {
            OICFree(pem);
            needTofreePem = false;
        }
    }

#else
    OC_UNUSED(data);
    OC_UNUSED(len);
    OC_UNUSED(encoding);
    OC_UNUSED(tag);
#endif // defined(__WITH_TLS__) || defined(__WITH_DTLS__)
}

void LogCred(OicSecCred_t *cred, const char* tag)
{
    OCStackResult ret = OC_STACK_OK;
    char uuidString[UUID_STRING_SIZE];
    char* uuid = NULL;
    OicUuid_t ownUuid;

    OIC_LOG_V(DEBUG, tag, "credId: %hu", cred->credId);
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
    OIC_LOG_V(DEBUG, tag, "credusage: %s", cred->credUsage);
#endif
    OIC_LOG_V(DEBUG, tag, "credtype: %u", cred->credType);


    uuid = NULL;
    ret = GetDoxmDeviceID(&ownUuid);
    if ( OC_STACK_OK == ret )
    {
        if (OCConvertUuidToString(ownUuid.id, uuidString))
        {
            uuid = uuidString;
        }
    }
    OIC_LOG_V(DEBUG, tag, "own uuid:  %s", (NULL != uuid) ? uuid : "None or Error");

    uuid = NULL;
    if (OCConvertUuidToString(cred->subject.id, uuidString))
    {
        uuid = uuidString;
    }
    OIC_LOG_V(DEBUG, tag, "subj uuid: %s", (NULL != uuid) ? uuid : "None or Error");

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)

    const char * encodingType[] = {
        "Unknown",
        "Raw",
        "Base 64",
        "PEM",
        "DER"
    };
    // encodingType is only used in logging. With some build options it is unused.
    OC_UNUSED(encodingType);

    OIC_LOG(DEBUG, tag, "...............................................");
    if ( (SIGNED_ASYMMETRIC_KEY == cred->credType) && (0 < cred->publicData.len) && (NULL != cred->publicData.data) )
    {
        OIC_LOG_V(DEBUG, tag, "publicData (encoding = %s)", encodingType[cred->publicData.encoding]);
        LogCert (cred->publicData.data, cred->publicData.len, cred->publicData.encoding, tag );
    }
    else
    {
        OIC_LOG(DEBUG, tag, "publicData: none");
    }

    OIC_LOG(DEBUG, tag, "...............................................");
    if ( (0 < cred->optionalData.len) && (NULL != cred->optionalData.data) )
    {
        OIC_LOG_V(DEBUG, tag, "optionalData (encoding = %s)", encodingType[cred->optionalData.encoding]);
        OIC_LOG_BUFFER(DEBUG, tag,  (const unsigned char*)&(cred->optionalData.data), cred->optionalData.len);
    }
    else
    {
        OIC_LOG(DEBUG, tag, "optionalData: none");
    }

#endif // defined(__WITH_DTLS__) || defined(__WITH_TLS__)

}

void LogCredResource(OicSecCred_t *cred, const char* tag, const char* label)
{
    // label is only used in logging. With some build options it is unused.
    OC_UNUSED(label);
    OicSecCred_t *curCred = NULL;
    int curCredIdx = 0;
    char uuidString[UUID_STRING_SIZE];
    char* uuid = NULL;

    OIC_LOG_V(DEBUG, tag, "=== %s ========================", (NULL != label) ? label : "cred" );
    VERIFY_NOT_NULL(tag, cred, ERROR);

    uuid = NULL;
    if (OCConvertUuidToString(gRownerId.id, uuidString))
    {
        uuid = uuidString;
    }
    OIC_LOG_V(DEBUG, tag, "rowner uuid:  %s", (NULL != uuid) ? uuid : "None or Error");

    LL_FOREACH(cred, curCred)
    {
        OIC_LOG_V(DEBUG, tag, "#### CRED ENTRY %d:", curCredIdx);
        LogCred(curCred, tag);
        curCredIdx++;
    }

    exit:
        OIC_LOG(DEBUG, tag, "============================================================");

    return;
}

void LogCurrrentCredResource(void)
{
    LogCredResource(gCred, "CRED", "Server cred Resource");
}

void printCRL(LogLevel level, const OicSecCrl_t *crl)
{
    OIC_LOG_V(level, CRL_TAG, "Print CRL @ %p:", crl);

    if (NULL == crl)
    {
        return;
    }

    OIC_LOG(level, CRL_TAG, "CRL object contains:");
    OIC_LOG_V(level, CRL_TAG, "id = %d", crl->CrlId);
    OIC_LOG_BUFFER(level, CRL_TAG, crl->ThisUpdate.data, crl->ThisUpdate.len);

    OIC_LOG(level, CRL_TAG, "crl:");
    OIC_LOG_V(level, CRL_TAG, "encoding = %d", crl->CrlData.encoding);
    OIC_LOG_V(level, CRL_TAG, "data (length = %" PRIuPTR "):", crl->CrlData.len);
    OIC_LOG_BUFFER(level, CRL_TAG, crl->CrlData.data, crl->CrlData.len);
}

#if INTERFACE
#define OIC_LOG_CRL(level, crl) printCRL((level),(crl))
#endif
