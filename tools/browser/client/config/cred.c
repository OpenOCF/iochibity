/* DO NOT EDIT - generated from svr_config.edn */
#include "openocf.h"

uint8_t psk1[] = {0xAA,0xAA,0xAA,0xAA,
		  0xAA,0xAA,0xAA,0xAA};

#define dev1_uuid {0x31,0x31,0x31,0x31, \
	    0x31,0x31,0x31,0x31,	\
 	    0x31,0x31,0x31,0x31,	\
	    0x31,0x31,0x31,0x31}

OicSecCred_t vendor_cred[1] = {
    {/* cred for server device (owned by this device) */
	.credId              = 1, 	 /* uint16_t */
	.subject             = {.id = dev1_uuid},
	.credType            = SYMMETRIC_PAIR_WISE_KEY,
	.privateData         = {.data = psk1, /* AAAAAAAAAAAAAAAA */
				.len = 8, .encoding = OIC_ENCODING_RAW},
	.period              =  "20150630T060000/20990920T220000",
	.rownerID            = {.id = {0}},	 /* OicUuid_t */
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
	.publicData          = {.data = NULL, .len = 0, .encoding = 0},
	.credUsage           = NULL,	 /* char* */
	.optionalData        = {.data = NULL, .len = 0,
				.encoding = 0, .revstat = false},
#endif /* __WITH_DTLS__  or __WITH_TLS__*/
	.roleId              = {.id = {0}, .authority = {0}},
#ifdef MULTIPLE_OWNER
	.eownerID            = {.id = {0}},	 /* OicUuid_t */
#endif //MULTIPLE_OWNER
	.next                = NULL /* OicSecCred_t* */
    }
};

OicSecCred_t *get_vendor_cred()
{
    return (OicSecCred_t*)vendor_cred;
}
