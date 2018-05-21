/* DO NOT EDIT - generated from svr_config.edn */
#include "openocf.h"

#include <stdio.h>

#include "svr_config.h"

uint8_t dev2_psk[] = DEV2_PSK;

uint8_t dev3_psk[] = DEV3_PSK;

OicSecCred_t vendor_cred[2] = {
    {/* cred for device that owns this device */
	.credId              = 1,
	.subject             = {.id = DEV2_UUID},
	.credType            = SYMMETRIC_PAIR_WISE_KEY,
	.privateData         = {.data = dev2_psk, /* AAAAAAAAAAAAAAAA */
				.len = 8, .encoding = OIC_ENCODING_RAW},
	.period              =  "20150630T060000/20990920T220000",
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
	.publicData          = {.data = NULL, .len = 0, .encoding = 0},
	.credUsage           = NULL,
	.optionalData        = {.data = NULL, .len = 0,
				.encoding = 0, .revstat = false},
#endif
	.roleId              = {.id = {0}, .authority = {0}},
	.rownerID            = {.id = {0}},
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
	.publicData        = {.data = NULL, .len = 0, .encoding = 0},
	.credUsage           = NULL,
	.optionalData        = {.data = NULL, .len = 0,
				.encoding = 0, .revstat = false},
#endif
	.roleId              = {.id = {0}, .authority = {0}},
	.rownerID            = {.id = {0}},	 /* OicUuid_t */
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
