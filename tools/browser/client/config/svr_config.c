/* DO NOT EDIT - generated from svr_config.edn */
#include "openocf.h"

#include <stdio.h>

uint8_t psk1[] = {0xAA,0xAA,0xAA,0xAA,
		  0xAA,0xAA,0xAA,0xAA};

#define SERVER1_UUID {0x31,0x31,0x31,0x31, \
		   0x31,0x31,0x31,0x31,	\
		   0x31,0x31,0x31,0x31,	\
		   0x31,0x31,0x31,0x31}

#define OWNER1_UUID {0x32,0x32,0x32,0x32, \
		   0x32,0x32,0x32,0x32, \
		   0x32,0x32,0x32,0x32, \
		   0x32,0x32,0x32,0x32}

uint8_t owner1_psk[] = {0xAA,0xAA,0xAA,0xAA,
		      0xAA,0xAA,0xAA,0xAA};

#define DEV3_UUID {0x31,0x39,0x31,0x39, \
		   0x31,0x39,0x31,0x39, \
		   0x31,0x39,0x31,0x39, \
		   0x31,0x39,0x31,0x39}

uint8_t dev3_psk[] = {0xBB,0xBB,0xBB,0xBB,
		      0xBB,0xBB,0xBB,0xBB};

OicSecOxm_t gDoxmDefaultOxm[] = {OIC_JUST_WORKS, OIC_RANDOM_DEVICE_PIN};

        /* :oxms [0], */
        /* :oxmsel 0, */
        /* :sct 1, */
        /* :owned true, */
        /* :deviceuuid "32323232-3232-3232-3232-323232323232", */
        /* :devowneruuid "32323232-3232-3232-3232-323232323232", */
        /* :rowneruuid "32323232-3232-3232-3232-323232323232" */
OicSecDoxm_t vendor_doxm =
{
    // gDoxmDefaultOxm,       /* uint16_t *oxm */
    .oxm = OIC_JUST_WORKS, /* OIC_RANDOM_DEVICE_PIN}, */
    .oxmLen = 1,
    .oxmSel = OIC_JUST_WORKS,
    .sct = SYMMETRIC_PAIR_WISE_KEY | SIGNED_ASYMMETRIC_KEY,
    .deviceID = {.id = OWNER1_UUID},
    .owned = true,
    .owner = {.id = OWNER1_UUID},
    .rownerID = {.id = OWNER1_UUID},
#ifdef MULTIPLE_OWNER
    .subOwners = NULL,
    .mom = NULL,
#endif //MULTIPLE_OWNER
    .dpc = false /* DEPRECATED dpc (direct pairing capability) */
};

OicSecDoxm_t* get_vendor_doxm()
{
    /* FIXME: on error set errno */
    return &vendor_doxm;
}

OicSecCred_t vendor_cred[1] = {
    {/* cred for server device (owned by this device) */
	.credId              = 1,
	.subject             = {.id = SERVER1_UUID},
	.credType            = SYMMETRIC_PAIR_WISE_KEY,
	.privateData         = {.data = psk1, /* AAAAAAAAAAAAAAAA */
				.len = 8, .encoding = OIC_ENCODING_RAW},
	.period              =  "20150630T060000/20990920T220000",
	.rownerID            = {.id = OWNER1_UUID},
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
	.publicData          = {.data = NULL, .len = 0, .encoding = 0},
	.credUsage           = NULL,
	.optionalData        = {.data = NULL, .len = 0,
				.encoding = 0, .revstat = false},
#endif
	.roleId              = {.id = {0}, .authority = {0}},
#ifdef MULTIPLE_OWNER
	.eownerID            = {.id = {0}},
#endif
	.next                = NULL
    }
};

OicSecCred_t *get_vendor_cred()
{
    return (OicSecCred_t*)vendor_cred;
}

OicSecDpom_t SUPPORTED_MODES[] = {SINGLE_SERVICE_CLIENT_DRIVEN};

OicSecPstat vendor_pstat =
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //OicSecDostype_t     dos;            // -:RW:S:Y:oic.sec.dostype
    .dos = {.state=DOS_RFNOP,	/* device onboarding state */
	    .pending=false},
    //bool                isOp;           // 0:R:S:Y:Boolean
    .isOp = true,
    //OicSecDpm_t         cm;             // 1:R:S:Y:oic.sec.dpmtype
    .cm = NORMAL,		/* current mode */
    //OicSecDpm_t         tm;             // 2:RW:S:Y:oic.sec.dpmtype
    .tm = NORMAL,		/* target mode */
    //OicSecDpom_t        om;             // 4:RW:M:Y:oic.sec.dpom
    .om = SINGLE_SERVICE_CLIENT_DRIVEN, /* Operational Mode */
    //size_t              smLen;          // the number of elts in Sm
    .smLen = 1,
    //OicSecDpom_t        *sm;            // 5:R:M:Y:oic.sec.dpom
    .sm = SUPPORTED_MODES, /* Supported Modes */
    //uint16_t            commitHash;     // 6:R:S:Y:oic.sec.sha256
    .commitHash = 0,
    //OicUuid_t           rownerID;       // 7:R:S:Y:oic.uuid
    .rownerID = {.id = OWNER1_UUID}
};

OicSecPstat_t *get_vendor_pstat()
{
    return &vendor_pstat;
}
