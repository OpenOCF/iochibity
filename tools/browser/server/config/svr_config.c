/* DO NOT EDIT - generated from svr_config.edn */
#include "openocf.h"

#include <stdio.h>

/* this device: */
#define SERVER1_UUID {0x31,0x31,0x31,0x31, \
		   0x31,0x31,0x31,0x31,	\
		   0x31,0x31,0x31,0x31,	\
		   0x31,0x31,0x31,0x31}

#define OWNER2_UUID {0x32,0x32,0x32,0x32, \
		   0x32,0x32,0x32,0x32, \
		   0x32,0x32,0x32,0x32, \
		   0x32,0x32,0x32,0x32}

uint8_t owner2_psk[] = {0xAA,0xAA,0xAA,0xAA,
		      0xAA,0xAA,0xAA,0xAA};

#define DEV3_UUID {0x31,0x39,0x31,0x39, \
		   0x31,0x39,0x31,0x39, \
		   0x31,0x39,0x31,0x39, \
		   0x31,0x39,0x31,0x39}

uint8_t dev3_psk[] = {0xBB,0xBB,0xBB,0xBB,
		      0xBB,0xBB,0xBB,0xBB};

OicSecOxm_t oxms[] = {OIC_JUST_WORKS, OIC_RANDOM_DEVICE_PIN};

OicSecDoxm_t vendor_doxm = {
    .oxm = oxms,
    .oxmLen = 2,
    .oxmSel = OIC_JUST_WORKS,
    .sct = SYMMETRIC_PAIR_WISE_KEY | SIGNED_ASYMMETRIC_KEY,
    .owned = true,
    .deviceID = {.id = SERVER1_UUID},
    .dpc = false, /* DEPRECATED dpc (direct pairing capability) */
    .owner = {.id = OWNER2_UUID},
#ifdef MULTIPLE_OWNER
    .subOwners = NULL,
    .mom = NULL,
#endif
    .rownerID = {.id = SERVER1_UUID}
};

OicSecDoxm_t* get_vendor_doxm()
{
    return &vendor_doxm;
}

/* NB: /oic/cred resource is owned by the device owning this device: */
OicSecCred_t vendor_cred[2] = {
    {/* cred for device that owns this device */
	.credId              = 1,
	.subject             = {.id = OWNER2_UUID},
	.credType            = SYMMETRIC_PAIR_WISE_KEY,
	.privateData         = {.data = owner2_psk, /* AAAAAAAAAAAAAAAA */
				.len = 8, .encoding = OIC_ENCODING_RAW},
	.period              =  "20150630T060000/20990920T220000",
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
	.publicData          = {.data = NULL, .len = 0,
				.encoding = OIC_ENCODING_UNKNOWN},
	.credUsage           = NULL,
	.optionalData        = {.data = NULL, .len = 0,
				.encoding = OIC_ENCODING_RAW,
				.revstat = false},
#endif
	.roleId              = {.id = {0}, .authority = {0}},
	.rownerID            = {.id = OWNER2_UUID},
#ifdef MULTIPLE_OWNER
	.eownerID            = {.id = {0}},
#endif
	.next                = &vendor_cred[1]
    },

    {/* some other device, that does not own this device: */
	.credId              = 2,
	.subject              = {.id = DEV3_UUID},
	.credType = SYMMETRIC_PAIR_WISE_KEY,
	.privateData         = {.data = dev3_psk, /* BBBBBBBBBBBBBBBB */
				.len = 16, .encoding = OIC_ENCODING_RAW},
	.period =   "20150630T060000/20990920T220000", /* char* */
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
	.publicData        = {.data = NULL, .len = 0,
			      .encoding = OIC_ENCODING_UNKNOWN},
	.credUsage           = NULL,
	.optionalData        = {.data = NULL, .len = 0,
				.encoding = OIC_ENCODING_UNKNOWN,
				.revstat = false},
#endif
	.roleId              = {.id = {0}, .authority = {0}},
	.rownerID            = {.id = OWNER2_UUID},
#ifdef MULTIPLE_OWNER
	.eownerID            = {.id = {0}},	 /* OicUuid_t */
#endif
	.next                = NULL		/* OicSecCred_t* */
    }
};

OicSecCred_t *get_vendor_cred()
{
    return (OicSecCred_t*)vendor_cred;
}

OicSecRsrc_t ocf_core_resources[4] = {
    {.href = "/oic/res",
     .next = &ocf_core_resources[1]},
    {.href = "/oic/d",
     .next = &ocf_core_resources[2]},
    {.href = "/oic/p",
     .next = &ocf_core_resources[3]},
    {.href = "/oic/sec/doxm",
     .next = NULL}
};

OicSecRsrc_t owner_resources = {
    .wildcard = ALL_RESOURCES	/* json '*' */
};

OicSecAce_t vendor_aces[] = {
    {.aceid = 1,
     /* OicSecAceUuidSubject, */
     /* OicSecAceRoleSubject, */
     .subjectType = OicSecAceConntypeSubject,
     .subjectConn = ANON_CLEAR,
     //OicSecRsrc_t *resources;	  /* struct */
     .resources = ocf_core_resources,
     //uint16_t permission;                // 2:R:S:Y:UINT16
     .permission = PERMISSION_READ | PERMISSION_WRITE,
     //OicSecValidity_t *validities,       /* struct */
     // .validities = 
#ifdef MULTIPLE_OWNER
     //OicUuid_t* eownerID;                //4:R:S:N:oic.uuid
     //.eownerID =
#endif
     //OicSecAce_t *next;
     .next = &vendor_aces[1]
    },
    {.aceid = 2,
     /* OicSecAceUuidSubject, */
     /* OicSecAceRoleSubject, */
     .subjectType = OicSecAceConntypeSubject,
     .subjectConn = AUTH_CRYPT,
     .resources = ocf_core_resources,
     .permission = PERMISSION_READ | PERMISSION_WRITE,
     //OicSecValidity_t *validities,       /* struct */
     // .validities = 
#ifdef MULTIPLE_OWNER
     //OicUuid_t* eownerID;                //4:R:S:N:oic.uuid
     //.eownerID =
#endif
     //OicSecAce_t *next;
     .next = &vendor_aces[2]
    },
    {.aceid = 3,
     .subjectType = OicSecAceUuidSubject,
     .subjectuuid = {.id = OWNER2_UUID},
     .resources = &owner_resources,
     //uint16_t permission;                // 2:R:S:Y:UINT16
     .permission = PERMISSION_READ | PERMISSION_WRITE,
     //OicSecValidity_t *validities,       /* struct */
     // .validities = 
#ifdef MULTIPLE_OWNER
     //OicUuid_t* eownerID;                //4:R:S:N:oic.uuid
     //.eownerID =
#endif
     //OicSecAce_t *next;
     //     .next = &vendor_aces[3]
    }
};

OicSecAcl_t vendor_acl = {
    .rownerID = {.id = OWNER2_UUID},
    .aces = vendor_aces
};

OicSecAcl_t *get_vendor_acl()
{
    return &vendor_acl;
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
    .rownerID = {.id = SERVER1_UUID} /* self-owned */
};

OicSecPstat_t *get_vendor_pstat()
{
    return &vendor_pstat;
}
