/* DO NOT EDIT - generated from svr_config.edn */
#include "openocf.h"

#include <stdio.h>

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
    .owned = true,
    .deviceID = {.id = {0x32,0x32,0x32,0x32, /* 16-byte uuid */
			0x32,0x32,0x32,0x32,
			0x32,0x32,0x32,0x32,
			0x32,0x32,0x32,0x32}},
    /* DEPRECATED dpc (direct pairing capability) */
    .dpc = false,
    /* FIXME: UUID: uint8_t[] */
    /* do we need to use memcpy? */
    .owner = {.id = {0x32,0x32,0x32,0x32,
		     0x32,0x32,0x32,0x32,
		     0x32,0x32,0x32,0x32,
		     0x32,0x32,0x32,0x32}},
#ifdef MULTIPLE_OWNER
    .subOwners = NULL,
    .mom = NULL,
#endif //MULTIPLE_OWNER
    .rownerID = {.id = {0x32,0x32,0x32,0x32, /* 16-byte uuid */
			0x32,0x32,0x32,0x32,
			0x32,0x32,0x32,0x32,
			0x32,0x32,0x32,0x32}}
};

OicSecDoxm_t* get_vendor_doxm()
{
    printf(">>>>>>>>>>>>>>>>QQQQQQQQQQQQQQQQTTTTTTTTTTTTTTTT\n");
    /* FIXME: on error set errno */
    return &vendor_doxm;
}
