/* svrs_codec.c */

#include <errno.h>
#include "openocf.h"
#include "svrs_codec.h"

#include "cJSON.h"
#include "coap/pdu.h"

char *g_msg_type[12];

static cJSON *cred_to_json(OCSecurityPayload *payload)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    OIC_LOG_V(DEBUG, TAG, "3 payload type: %x", payload->base.type);

    OicSecCred_t *cred_payload = NULL;
    OicUuid_t     *rownerId = NULL;

    OCStackResult r = CBORPayloadToCred(payload->securityData,
					payload->payloadSize,
					&cred_payload,
					&rownerId);
    if ( r != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "CBORPayloadToCred failure: %d", r);
    }
    OIC_LOG_V(DEBUG, TAG, "4 payload type: %x", payload->base.type);

    /* FIXME: iterate over OicSecCred_t linked list */

    cJSON *root;
    root = cJSON_CreateObject();

/*     uint16_t            credId;         // 0:R:S:Y:UINT16 */
    cJSON_AddNumberToObject(root, "credid", cred_payload->credId);
/*     OicUuid_t           subject;        // 1:R:S:Y:oic.uuid */
    char *subject_uuid = NULL;
    ConvertUuidToStr(&cred_payload->subject, &subject_uuid);
    OIC_LOG_V(INFO, TAG, "subject uuid: %s", subject_uuid);
    cJSON_AddItemToObject(root,
			  "subjectuuid",
			  cJSON_CreateString(subject_uuid));

/*     // If roleId.id is all zeroes, this property is not set. */
/*     OicSecRole_t        roleId;         // 2:R:M:N:oic.sec.roletype */
/*     OicSecCredType_t    credType;       // 3:R:S:Y:oic.sec.credtype */
    cJSON_AddNumberToObject(root, "credtype", cred_payload->credType);
/* #if defined(__WITH_DTLS__) || defined(__WITH_TLS__) */
/*     OicSecKey_t         publicData;     // own cerificate chain */
/*     char            *credUsage;            // 4:R:S:N:String */
/*     OicSecOpt_t        optionalData;   // CA's cerificate chain */
/* #endif /\* __WITH_DTLS__  or __WITH_TLS__*\/ */
/*     OicSecKey_t         privateData;    // 6:R:S:N:oic.sec.key */
    OIC_LOG_V(INFO, TAG,
	      "privatedata.encoding: %d",
	      cred_payload->privateData.encoding);
    cJSON_AddNumberToObject(root,
			    "privatedata.encoding",
			    cred_payload->privateData.encoding);
/*     char                *period;        // 7:R:S:N:String */
/*     OicUuid_t            rownerID;      // 8:R:S:Y:oic.uuid */
    char *rowner = NULL;
    ConvertUuidToStr(&cred_payload->rownerID, &rowner);
    OIC_LOG_V(INFO, TAG, "rowneruuid: %s", rowner);
    cJSON_AddItemToObject(root, "rowneruuid", cJSON_CreateString(rowner));
/* #ifdef MULTIPLE_OWNER */
/*     OicUuid_t            *eownerID;     //9:R:S:N:oic.uuid */
/* #endif //MULTIPLE_OWNER */
/*     OicSecCred_t        *next; */
    return root;
}

static cJSON *doxm_to_json(OCSecurityPayload *payload)
{
    OicSecDoxm_t *doxm_payload = NULL;
    OCStackResult r = CBORPayloadToDoxm(payload->securityData, payload->payloadSize, &doxm_payload);
    if ( r != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "CBORPayloadToDoxm failure: %d", r);
    }

    cJSON *root;
    root = cJSON_CreateObject();

    /* enum OicSecOxm_t *oxm */
    cJSON *oxms;		/* Ownership Transfer Methods */
    oxms = cJSON_CreateArray();

    /* size_t oxmLen  // oxms count */
    /* OIC_LOG_V(INFO, TAG, "doxm oxms len: %d", doxm_payload->oxmLen); */
    cJSON *json_oxms[doxm_payload->oxmLen];

    for (int i=0; i < doxm_payload->oxmLen; i++) {
	/* OIC_LOG_V(INFO, TAG, "doxm oxm[%d]: 0x%X", i, doxm_payload->oxm[i]); */
	json_oxms[i] = cJSON_CreateNumber(doxm_payload->oxm[i]);
	cJSON_AddItemToArray(oxms, json_oxms[i]);
    }
    cJSON_AddItemToObject(root, "oxms", oxms);

    /* OixSecOxm_t *oxmSel */
    /* OIC_LOG_V(INFO, TAG, "doxm oxmsel: 0x%X", doxm_payload->oxmSel); */
    cJSON_AddNumberToObject(root, "oxmsel", doxm_payload->oxmSel);

    /* OicSecCredType_t sct  Supported Cred Types enum */
    cJSON_AddNumberToObject(root, "sct", doxm_payload->sct);

    /* bool owned */
    cJSON_AddItemToObject(root, "owned", cJSON_CreateBool(doxm_payload->owned));

    /* OicUuid_t deviceID */
    char *duuid = NULL;
    ConvertUuidToStr(&doxm_payload->deviceID, &duuid);
    OIC_LOG_V(INFO, TAG, "doxm uuid: %s", duuid);
    cJSON_AddItemToObject(root, "deviceuuid", cJSON_CreateString(duuid));

    /* bool dpc   Device Policy Controller? */
    cJSON_AddItemToObject(root, "dpc", cJSON_CreateBool(doxm_payload->dpc));

    /* OicUuid_t owner */
    char *deviceowner = NULL;
    ConvertUuidToStr(&doxm_payload->owner, &deviceowner);
    OIC_LOG_V(INFO, TAG, "doxm owner: %s", deviceowner);
    cJSON_AddItemToObject(root, "devowneruuid", cJSON_CreateString(deviceowner));

    /* if MULTIPLE_OWNER */
    /* OOicSecSubOwner_t *subOwners */
    /* OicSecMom_t *mom */
    /* endif */

    /* OicUuid_t rownerID */
    char *rowner = NULL;
    ConvertUuidToStr(&doxm_payload->rownerID, &rowner);
    OIC_LOG_V(INFO, TAG, "doxm rowner: %s", rowner);
    cJSON_AddItemToObject(root, "rowneruuid", cJSON_CreateString(rowner));
    return root;
}

static cJSON *pstat_to_json(OCSecurityPayload *payload)
{
    OicSecPstat_t *pstat_payload = NULL;
    OCStackResult r = CBORPayloadToPstat(payload->securityData, payload->payloadSize, &pstat_payload);
    if ( r != OC_STACK_OK) {
	OIC_LOG_V(ERROR, TAG, "CBORPayloadToPstat failure: %d", r);
    }

    cJSON *root;
    root = cJSON_CreateObject();

    /* OicSecDostype_t     dos;            // -:RW:S:Y:oic.sec.dostype */

    /* bool                isOp;           // 0:R:S:Y:Boolean */
    /* bool dpc   Device Policy Controller? */
    cJSON_AddItemToObject(root, "isop", cJSON_CreateBool(pstat_payload->isOp));

    /* OicSecDpm_t         cm;             // 1:R:S:Y:oic.sec.dpmtype */
    /* OicSecDpm_t         tm;             // 2:RW:S:Y:oic.sec.dpmtype */
    /* OicSecDpom_t        om;             // 4:RW:M:Y:oic.sec.dpom */
    /* size_t              smLen;          // the number of elts in Sm */
    /* OicSecDpom_t        *sm;            // 5:R:M:Y:oic.sec.dpom */
    /* uint16_t            commitHash;     // 6:R:S:Y:oic.sec.sha256 */

    /* OicUuid_t           rownerID;       // 7:R:S:Y:oic.uuid */
    char *rowner = NULL;
    ConvertUuidToStr(&pstat_payload->rownerID, &rowner);
    OIC_LOG_V(INFO, TAG, "pstat rowner: %s", rowner);
    cJSON_AddItemToObject(root, "rowneruuid", cJSON_CreateString(rowner));
    return root;
}

/* FIXME: this should be in the engine, accessible via OCFClientSP */
/* caller responsible for freeing result */
static cJSON* security_to_json(OCClientResponse *msg)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY; resource URI: %s", __func__, msg->resourceUri);
    cJSON *root;
    OCPayload *payload = (OCPayload*)msg->payload;

    /* TODO: abstract; same code appears in inbound_msg_inspector.c */
    if (starts_with("/oic/sec/doxm", msg->resourceUri)) {
	OIC_LOG_V(INFO, TAG, "decoding device ownership transfer management svr");
	sprintf(g_msg_type, "%s", "doxm");
	root = doxm_to_json( (OCSecurityPayload*) payload);
    } else if (starts_with("/oic/sec/cred", msg->resourceUri)) {
	OIC_LOG_V(INFO, TAG, "decoding credentials svr");
	sprintf(g_msg_type, "%s", "cred");
	root = cred_to_json( (OCSecurityPayload*) payload);
    } else if (starts_with("/oic/sec/pstat", msg->resourceUri)) {
	OIC_LOG_V(INFO, TAG, "decoding provisioning status svr");
	sprintf(g_msg_type, "%s", "pstat");
	root = pstat_to_json( (OCSecurityPayload*) payload);
    } else if (starts_with("/oic/sec/acl2", msg->resourceUri)) {
	sprintf(g_msg_type, "%s", "acl2");
	OIC_LOG_V(INFO, TAG, "decoding access control list 2 svr");
    } else if (starts_with("/oic/sec/crl", msg->resourceUri)) {
	sprintf(g_msg_type, "%s", "crl");
	OIC_LOG_V(INFO, TAG, "decoding certificate revocation list svr");
    } else if (starts_with("/oic/sec/csr", msg->resourceUri)) {
	sprintf(g_msg_type, "%s", "csr");
	OIC_LOG_V(INFO, TAG, "decoding certificate signing request svr");
    } else if (starts_with("/oic/sec/roles", msg->resourceUri)) {
	sprintf(g_msg_type, "%s", "roles");
	OIC_LOG_V(INFO, TAG, "decoding roles svr");
    } else {
	/* unknown svr */
	sprintf(g_msg_type, "%s", "unknown");
	OIC_LOG_V(ERROR, TAG, "UKNOWN SVR: %s", msg->resourceUri);
    }

    return root;
}

void log_security_msg(OCClientResponse *clientResponse)
{
    cJSON *security_json = security_to_json(clientResponse);
    char* rendered = cJSON_Print(security_json);

    char fname[256];
    sprintf(fname, "./logs/client/%s_%p.txt", g_msg_type, clientResponse);
    OIC_LOG_V(INFO, TAG, "security json filename: %s", fname);
    FILE *fd = fopen(fname, "w");
    if (fd == NULL) {
        OIC_LOG_V(INFO, TAG, "fopen %s err: %s", fname, strerror(errno));
	exit(EXIT_FAILURE);
    }
    fprintf(fd, "%s", rendered);
    fclose(fd);
    cJSON_Delete(security_json);
}
