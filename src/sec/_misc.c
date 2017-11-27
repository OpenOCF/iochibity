
#if EXPORT_INTERFACE
struct OicSecOpt
{
    uint8_t                *data;
    size_t                  len;

    OicEncodingType_t encoding;
    bool                revstat;
};

typedef struct OicSecOpt OicSecOpt_t;
#endif	/* INTERFACE */

typedef struct OicSecSvc OicSecSvc_t;

typedef char *OicUrn_t; //TODO is URN type defined elsewhere?

#if EXPORT_INTERFACE
#include <stdint.h>
typedef struct OicUuid OicUuid_t; //TODO is UUID type defined elsewhere?

/**
 * /oic/uuid (Universal Unique Identifier) data type.
 */
/* #define UUID_LENGTH 128/8 // 128-bit GUID length - ca_adapter_net_ssl.c */
//TODO: Confirm the length and type of ROLEID.
#define ROLEID_LENGTH 64 // 64-byte authority max length
#define ROLEAUTHORITY_LENGTH 64 // 64-byte authority max length
#define OWNER_PSK_LENGTH_128 128/8 //byte size of 128-bit key size
#define OWNER_PSK_LENGTH_256 256/8 //byte size of 256-bit key size

struct OicUuid
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in unless this is defined elsewhere?
    uint8_t             id[UUID_LENGTH];
};
#endif	/* INTERFACE */
