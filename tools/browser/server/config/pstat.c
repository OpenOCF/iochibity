#include "openocf.h"

#define DEV1_UUID {0x31,0x31,0x31,0x31, \
		   0x31,0x31,0x31,0x31,	\
		   0x31,0x31,0x31,0x31,	\
		   0x31,0x31,0x31,0x31}

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
    .rownerID = {.id = DEV1_UUID}
};

OicSecPstat_t *get_vendor_pstat()
{
    return &vendor_pstat;
}
