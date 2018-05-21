/* DO NOT EDIT - generated from svr_config.edn */
#include "openocf.h"

#include <stdio.h>

#include "svr_config.h"

OicSecOxm_t oxms[] = {OIC_JUST_WORKS, OIC_RANDOM_DEVICE_PIN};

OicSecDoxm_t vendor_doxm = {
    .oxm = oxms,
    .oxmLen = 2,
    .oxmSel = OIC_JUST_WORKS,
    .sct = SYMMETRIC_PAIR_WISE_KEY | SIGNED_ASYMMETRIC_KEY,
    .owned = true,
    .deviceID = {.id = DEV1_UUID},
    .dpc = false, /* DEPRECATED dpc (direct pairing capability) */
    .owner = {.id = DEV2_UUID},
#ifdef MULTIPLE_OWNER
    .subOwners = NULL,
    .mom = NULL,
#endif
    .rownerID = {.id = DEV1_UUID}
};

OicSecDoxm_t* get_vendor_doxm()
{
    printf(">>>>>>>>>>>>>>>>QQQQQQQQQQQQQQQQTTTTTTTTTTTTTTTT\n");
    /* FIXME: on error set errno */
    return &vendor_doxm;
}
