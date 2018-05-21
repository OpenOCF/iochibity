#include "openocf.h"

#define DEV_UUID {0x32,0x32,0x32,0x32, \
		   0x32,0x32,0x32,0x32,	\
		   0x32,0x32,0x32,0x32,	\
		   0x32,0x32,0x32,0x32}


/* typedef struct OicSecRsrc_t */
/* { */
/*     char *href; // 0:R:S:Y:String */
/*     char *rel; // 1:R:S:N:String */
/*     char** types; // 2:R:S:N:String Array */
/*     size_t typeLen; // the number of elts in types */
/*     char** interfaces; // 3:R:S:N:String Array */
/*     size_t interfaceLen; // the number of elts in interfaces */
/*     OicSecAceResourceWildcard_t wildcard; */
/*     OicSecRsrc_t *next; */
/* } OicSecRsrc_t; */

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
     .subjectuuid = {.id = DEV_UUID},
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
    .rownerID = {.id = DEV_UUID},
    .aces = vendor_aces
};

OicSecAcl_t *get_vendor_acl()
{
    return &vendor_acl;
}
