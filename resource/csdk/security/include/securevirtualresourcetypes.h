//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 * Data type definitions for all oic.sec.* types defined in the
 * OIC Security Specification.
 *
 * Note that throughout, ptrs are used rather than arrays.  There
 * are two primary reasons for this:
 * 1) The Spec defines many structures with optional fields, so pre-
 *    allocating these would be wasteful.
 * 2) There are in many cases arrays of Strings or arrays of Structs,
 *    which could not be defined as variable length arrays (e.g. array[])
 *    without breaking from the structure order and definition in the Spec.
 *
 * The primary drawback to this decision is that marshalling functions
 * will have to be written by hand to marshal these structures (e.g. to/from
 * Persistent Storage, or across memory boundaries).
 *
 * TODO reconcile against latest OIC Security Spec to ensure all fields correct.
 * (Last checked against v0.95)
 */

#ifndef OC_SECURITY_RESOURCE_TYPES_H
#define OC_SECURITY_RESOURCE_TYPES_H

#include <stdint.h> // for uint8_t typedef
#include <stdbool.h>
#ifdef __WITH_X509__
#include "byte_array.h"
#endif /* __WITH_X509__ */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Values used to create bit-maskable enums for single-value response with
 * embedded code.
 */
#define ACCESS_GRANTED_DEF            (1 << 0)
#define ACCESS_DENIED_DEF             (1 << 1)
#define INSUFFICIENT_PERMISSION_DEF   (1 << 2)
#define SUBJECT_NOT_FOUND_DEF         (1 << 3)
#define RESOURCE_NOT_FOUND_DEF        (1 << 4)
#define POLICY_ENGINE_ERROR_DEF       (1 << 5)
#define INVALID_PERIOD_DEF            (1 << 6)
#define ACCESS_WAITING_DEF            (1 << 7)
#define AMS_SERVICE_DEF               (1 << 8)
#define REASON_MASK_DEF               (INSUFFICIENT_PERMISSION_DEF | \
                                       INVALID_PERIOD_DEF | \
                                       SUBJECT_NOT_FOUND_DEF | \
                                       RESOURCE_NOT_FOUND_DEF | \
                                       POLICY_ENGINE_ERROR_DEF)


/**
 * Access policy in least significant bits (from Spec):
 * 1st lsb:  C (Create)
 * 2nd lsb:  R (Read, Observe, Discover)
 * 3rd lsb:  U (Write, Update)
 * 4th lsb:  D (Delete)
 * 5th lsb:  N (Notify)
 */
#define PERMISSION_CREATE       (1 << 0)
#define PERMISSION_READ         (1 << 1)
#define PERMISSION_WRITE        (1 << 2)
#define PERMISSION_DELETE       (1 << 3)
#define PERMISSION_NOTIFY       (1 << 4)
#define PERMISSION_FULL_CONTROL (PERMISSION_CREATE | \
                                 PERMISSION_READ | \
                                 PERMISSION_WRITE | \
                                 PERMISSION_DELETE | \
                                 PERMISSION_NOTIFY)

/**
 * @brief   Response type for all Action requests from CA layer;
 *          may include a reason code.
 *
 * To extract codes use GetReasonCode function on SRMAccessResponse:
 *
 * SRMAccessResponse_t response = SRMRequestHandler(obj, info);
 * if(SRM_TRUE == IsAccessGranted(response)) {
 *     SRMAccessResponseReasonCode_t reason = GetReasonCode(response);
 *     switch(reason) {
 *         case INSUFFICIENT_PERMISSION:
 *         ...etc.
 *     }
 * }
 */
typedef enum
{
    ACCESS_GRANTED = ACCESS_GRANTED_DEF,
    ACCESS_DENIED = ACCESS_DENIED_DEF,
    ACCESS_DENIED_INVALID_PERIOD = ACCESS_DENIED_DEF
        | INVALID_PERIOD_DEF,
    ACCESS_DENIED_INSUFFICIENT_PERMISSION = ACCESS_DENIED_DEF
        | INSUFFICIENT_PERMISSION_DEF,
    ACCESS_DENIED_SUBJECT_NOT_FOUND = ACCESS_DENIED_DEF
        | SUBJECT_NOT_FOUND_DEF,
    ACCESS_DENIED_RESOURCE_NOT_FOUND = ACCESS_DENIED_DEF
        | RESOURCE_NOT_FOUND_DEF,
    ACCESS_DENIED_POLICY_ENGINE_ERROR = ACCESS_DENIED_DEF
        | POLICY_ENGINE_ERROR_DEF,
    ACCESS_WAITING_FOR_AMS = ACCESS_WAITING_DEF
        | AMS_SERVICE_DEF,
    ACCESS_DENIED_AMS_SERVICE_ERROR = ACCESS_DENIED
        | AMS_SERVICE_DEF
} SRMAccessResponse_t;

/**
 * Reason code for SRMAccessResponse.
 */
typedef enum
{
    NO_REASON_GIVEN = 0,
    INSUFFICIENT_PERMISSION = INSUFFICIENT_PERMISSION_DEF,
    SUBJECT_NOT_FOUND = SUBJECT_NOT_FOUND_DEF,
    RESOURCE_NOT_FOUND = RESOURCE_NOT_FOUND_DEF,
} SRMAccessResponseReasonCode_t;

/**
 * Extract Reason Code from Access Response.
 */
static inline SRMAccessResponseReasonCode_t GetReasonCode(
    SRMAccessResponse_t response)
{
    SRMAccessResponseReasonCode_t reason =
        (SRMAccessResponseReasonCode_t)(response & REASON_MASK_DEF);
    return reason;
}

/**
 * Returns 'true' iff request should be passed on to RI layer.
 */
static inline bool IsAccessGranted(SRMAccessResponse_t response)
{
    if(ACCESS_GRANTED == (response & ACCESS_GRANTED))
    {
        return true;
    }
    else
    {
        return false;
    }
}

typedef struct OicSecAcl OicSecAcl_t;

typedef struct OicSecAmacl OicSecAmacl_t;

typedef struct OicSecCred OicSecCred_t;

/**
 * Aid for assigning/testing vals with OicSecCredType_t.
 * Example:
 *  OicSecCredType_t ct = PIN_PASSWORD | ASYMMETRIC_KEY;
 *  if((ct & PIN_PASSWORD) == PIN_PASSWORD)
 *  {
 *      // ct contains PIN_PASSWORD flag.
 *  }
 */
typedef enum OSCTBitmask
{
    NO_SECURITY_MODE                = 0x0,
    SYMMETRIC_PAIR_WISE_KEY         = (0x1 << 0),
    SYMMETRIC_GROUP_KEY             = (0x1 << 1),
    ASYMMETRIC_KEY                  = (0x1 << 2),
    SIGNED_ASYMMETRIC_KEY           = (0x1 << 3),
    PIN_PASSWORD                    = (0x1 << 4),
    ASYMMETRIC_ENCRYPTION_KEY       = (0x1 << 5),
} OSCTBitmask_t;

/**
 * /oic/sec/credtype (Credential Type) data type.
 * Derived from OIC Security Spec /oic/sec/cred; see Spec for details.
 *              0:  no security mode
 *              1:  symmetric pair-wise key
 *              2:  symmetric group key
 *              4:  asymmetric key
 *              8:  signed asymmetric key (aka certificate)
 *              16: PIN /password
 */
typedef OSCTBitmask_t OicSecCredType_t;

typedef struct OicSecDoxm OicSecDoxm_t;

typedef enum OicSecDpm
{
    NORMAL                          = 0x0,
    RESET                           = (0x1 << 0),
    TAKE_OWNER                      = (0x1 << 1),
    BOOTSTRAP_SERVICE               = (0x1 << 2),
    SECURITY_MANAGEMENT_SERVICES    = (0x1 << 3),
    PROVISION_CREDENTIALS           = (0x1 << 4),
    PROVISION_ACLS                  = (0x1 << 5),
    // << 6 THROUGH 15 RESERVED
} OicSecDpm_t;

typedef enum OicSecDpom
{
    MULTIPLE_SERVICE_SERVER_DRIVEN  = 0x0,
    SINGLE_SERVICE_SERVER_DRIVEN    = 0x1,
    MULTIPLE_SERVICE_CLIENT_DRIVEN  = 0x2,
    SINGLE_SERVICE_CLIENT_DRIVEN    = 0x3,
} OicSecDpom_t;

typedef enum OicSecSvcType
{
    SERVICE_UNKNOWN                 = 0x0,
    ACCESS_MGMT_SERVICE             = 0x1,  //urn:oic.sec.ams
} OicSecSvcType_t;


//TODO: Need more clarification on deviceIDFormat field type.
#if 0
typedef enum
{
    URN = 0x0
}OicSecDvcIdFrmt_t;
#endif

typedef enum
{
    OIC_R_ACL_TYPE = 0,
    OIC_R_AMACL_TYPE,
    OIC_R_CRED_TYPE,
    OIC_R_CRL_TYPE,
    OIC_R_DOXM_TYPE,
    OIC_R_DPAIRING_TYPE,
    OIC_R_PCONF_TYPE,
    OIC_R_PSTAT_TYPE,
    OIC_R_SACL_TYPE,
    OIC_R_SVC_TYPE,
    OIC_SEC_SVR_TYPE_COUNT, //define the value to number of SVR
    NOT_A_SVR_RESOURCE = 99
}OicSecSvrType_t;

typedef enum
{
    OIC_JUST_WORKS                          = 0x0,
    OIC_RANDOM_DEVICE_PIN                   = 0x1,
    OIC_MANUFACTURER_CERTIFICATE           = 0x2,
    OIC_OXM_COUNT
}OicSecOxm_t;

typedef enum
{
    OIC_ENCODING_UNKNOW = 0,
    OIC_ENCODING_RAW = 1,
    OIC_ENCODING_BASE64 = 2
}OicEncodingType_t;

typedef struct OicSecKey OicSecKey_t;

typedef struct OicSecPstat OicSecPstat_t;

typedef struct OicSecRole OicSecRole_t;

typedef struct OicSecSacl OicSecSacl_t;

typedef struct OicSecSvc OicSecSvc_t;

typedef char *OicUrn_t; //TODO is URN type defined elsewhere?

typedef struct OicUuid OicUuid_t; //TODO is UUID type defined elsewhere?


#ifdef __WITH_X509__
typedef struct OicSecCrl OicSecCrl_t;
typedef ByteArray OicSecCert_t;
#else
typedef void OicSecCert_t;
#endif /* __WITH_X509__ */

/**
 * /oic/uuid (Universal Unique Identifier) data type.
 */
#define UUID_LENGTH 128/8 // 128-bit GUID length
//TODO: Confirm the length and type of ROLEID.
#define ROLEID_LENGTH 128/8 // 128-bit ROLEID length
#define OWNER_PSK_LENGTH_128 128/8 //byte size of 128-bit key size
#define OWNER_PSK_LENGTH_256 256/8 //byte size of 256-bit key size

struct OicUuid
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in unless this is defined elsewhere?
    uint8_t             id[UUID_LENGTH];
};

/**
 * /oic/sec/jwk (JSON Web Key) data type.
 * See JSON Web Key (JWK)  draft-ietf-jose-json-web-key-41
 */
#define JWK_LENGTH 256/8 // 256 bit key length
struct OicSecKey
{
    uint8_t                *data;
    size_t                  len;

    // TODO: This field added as workaround. Will be replaced soon.
    OicEncodingType_t encoding;

};

/**
 * /oic/sec/acl (Access Control List) data type.
 * Derived from OIC Security Spec; see Spec for details.
 */
struct OicSecAcl
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    OicUuid_t           subject;        // 0:R:S:Y:uuid TODO: this deviates
                                        // from spec and needs to be updated
                                        // in spec (where it's a String).
    size_t              resourcesLen;   // the number of elts in Resources
    char                **resources;    // 1:R:M:Y:String
    uint16_t            permission;     // 2:R:S:Y:UINT16
    size_t              prdRecrLen;     // the number of elts in Periods
    char                **periods;       // 3:R:M*:N:String (<--M*; see Spec)
    char                **recurrences;   // 5:R:M:N:String
    OicUuid_t           rownerID;        // 8:R:S:Y:oic.uuid
    OicSecAcl_t         *next;
};

/**
 * /oic/sec/amacl (Access Manager Service Accesss Control List) data type.
 * Derived from OIC Security Spec; see Spec for details.
 */
struct OicSecAmacl
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    size_t              resourcesLen;   // the number of elts in Resources
    char                **resources;    // 0:R:M:Y:String
    size_t              amssLen;        // the number of elts in Amss
    OicUuid_t           *amss;          // 1:R:M:Y:acl
    OicUuid_t           rownerID;        // 2:R:S:Y:oic.uuid
    OicSecAmacl_t         *next;
};

/**
 * /oic/sec/cred (Credential) data type.
 * Derived from OIC Security Spec; see Spec for details.
 */
struct OicSecCred
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    uint16_t            credId;         // 0:R:S:Y:UINT16
    OicUuid_t           subject;        // 1:R:S:Y:oic.uuid
    //Note: Need further clarification on roleID data type
    //NOTE: Need further clarification on roleId datatype.
    //size_t              roleIdsLen;     // the number of elts in RoleIds
    //OicSecRole_t        *roleIds;       // 2:R:M:N:oic.sec.role
    OicSecCredType_t    credType;       // 3:R:S:Y:oic.sec.credtype
#ifdef __WITH_X509__
    OicSecCert_t        publicData;     // chain of certificates
#endif /* __WITH_X509__ */
    OicSecKey_t         privateData;    // 6:R:S:N:oic.sec.key
    char                *period;        // 7:R:S:N:String
    OicUuid_t           rownerID;        // 8:R:S:Y:oic.uuid
    OicSecCred_t        *next;
};

/**
 * /oic/sec/doxm (Device Owner Transfer Methods) data type
 * Derived from OIC Security Spec; see Spec for details.
 */
struct OicSecDoxm
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    OicUrn_t            *oxmType;       // 0:R:M:N:URN
    size_t              oxmTypeLen;     // the number of elts in OxmType
    OicSecOxm_t         *oxm;           // 1:R:M:N:UINT16
    size_t              oxmLen;         // the number of elts in Oxm
    OicSecOxm_t         oxmSel;         // 2:R/W:S:Y:UINT16
    OicSecCredType_t    sct;            // 3:R:S:Y:oic.sec.credtype
    bool                owned;          // 4:R:S:Y:Boolean
    //TODO: Need more clarification on deviceIDFormat field type.
    //OicSecDvcIdFrmt_t   deviceIDFormat; // 5:R:S:Y:UINT8
    OicUuid_t           deviceID;       // 6:R:S:Y:oic.uuid
    bool                dpc;            // 7:R:S:Y:Boolean
    OicUuid_t           owner;          // 8:R:S:Y:oic.uuid
    OicUuid_t           rownerID;       // 9:R:S:Y:oic.uuid
};

/**
 * /oic/sec/pstat (Provisioning Status) data type.
 * NOTE: this struct is ahead of Spec v0.95 in definition to include Sm.
 * TODO: change comment when reconciled to Spec v0.96.
 */
struct OicSecPstat
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    bool                isOp;           // 0:R:S:Y:Boolean
    OicSecDpm_t         cm;             // 1:R:S:Y:oic.sec.dpm
    OicSecDpm_t         tm;             // 2:RW:S:Y:oic.sec.dpm
    OicUuid_t           deviceID;       // 3:R:S:Y:oic.uuid
    OicSecDpom_t        om;             // 4:RW:M:Y:oic.sec.dpom
    size_t              smLen;          // the number of elts in Sm
    OicSecDpom_t        *sm;            // 5:R:M:Y:oic.sec.dpom
    uint16_t            commitHash;     // 6:R:S:Y:oic.sec.sha256
    OicUuid_t           rownerID;       // 7:R:S:Y:oic.uuid
};

/**
 * /oic/sec/role (Role) data type.
 * Derived from OIC Security Spec; see Spec for details.
 */
struct OicSecRole
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in with Role definition
    uint8_t             id[ROLEID_LENGTH];
};

/**
 * /oic/sec/sacl (Signed Access Control List) data type.
 * Derived from OIC Security Spec; see Spec for details.
 */
struct OicSecSacl
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in from OIC Security Spec
};

/**
 * /oic/sec/svc (Service requiring a secure connection) data type.
 * Derived from OIC Security Spec; see Spec for details.
 */
struct OicSecSvc
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    OicUuid_t               svcdid;                 //0:R:S:Y:oic.uuid
    OicSecSvcType_t         svct;                   //1:R:M:Y:OIC Service Type
    size_t                  ownersLen;              //2:the number of elts in Owners
    OicUuid_t               *owners;                //3:R:M:Y:oic.uuid
    OicSecSvc_t             *next;
};

#ifdef __WITH_X509__
struct OicSecCrl
{
    uint16_t CrlId;
    ByteArray ThisUpdate;
    ByteArray CrlData;
};
#endif /* __WITH_X509__ */

/**
 * @brief   direct pairing data type
 */
typedef struct OicPin OicDpPin_t;

typedef struct OicSecPdAcl OicSecPdAcl_t;

typedef struct OicSecPconf OicSecPconf_t;

typedef struct OicSecDpairing OicSecDpairing_t;

#define DP_PIN_LENGTH 8 // temporary length

/**
 * @brief   /oic/sec/prmtype (Pairing Method Type) data type.
 *              0:  not allowed
 *              1:  pre-configured pin
 *              2:  random pin
 */
typedef enum PRMBitmask
{
    PRM_NOT_ALLOWED             = 0x0,
    PRM_PRE_CONFIGURED        = (0x1 << 0),
    PRM_RANDOM_PIN               = (0x1 << 1),
} PRMBitmask_t;

typedef PRMBitmask_t OicSecPrm_t;


struct OicPin
{
    uint8_t             val[DP_PIN_LENGTH];
};

/**
 * @brief   oic.sec.dpacltype (Device Pairing Access Control List) data type.
 */
struct OicSecPdAcl
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    char                  **resources;        // 0:R:M:Y:String
    size_t                resourcesLen;      // the number of elts in Resources
    uint16_t             permission;        // 1:R:S:Y:UINT16
    char                  **periods;            // 2:R:M*:N:String (<--M*; see Spec)
    char                  **recurrences;    // 3:R:M:N:String
    size_t                prdRecrLen;         // the number of elts in Periods/Recurrences
    OicSecPdAcl_t    *next;
};

/**
 * @brief   /oic/sec/pconf (Pairing Configuration) data type
 */
struct OicSecPconf
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    bool                  edp;                // 0:W:S:M:Boolean
    OicSecPrm_t      *prm;              // 1:R:M:N:UINT16
    size_t                prmLen;          // the number of elts in Prm
    OicDpPin_t          pin;               // 2:R:S:Y:String
    OicSecPdAcl_t    *pdacls;         // 3:R:M:Y:oic.sec.pdacltype
    OicUuid_t           *pddevs;        // 4:R:M:Y:oic.uuid
    size_t                 pddevLen;     // the number of elts in pddev
    OicUuid_t           deviceID;       // 5:R:S:Y:oic.uuid
    OicUuid_t           rownerID;          // 6:R:S:Y:oic.uuid
};

/**
 * @brief   /oic/sec/dpairing (Device Pairing) data type
 */
struct OicSecDpairing
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    OicSecPrm_t      spm;               // 0:R/W:S:Y:UINT16
    OicUuid_t           pdeviceID;     // 1:R:S:Y:oic.uuid
    OicUuid_t           rownerID;          // 2:R:S:Y:oic.uuid
};

#define MAX_VERSION_LEN 16 // Security Version length. i.e., 00.00.000 + reserved space

/**
 * @brief   security version data type
 */
typedef struct OicSecVer OicSecVer_t;

/**
 * @brief   /oic/sec/ver (Security Version) data type
 */
struct OicSecVer
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    char              secv[MAX_VERSION_LEN];          // 0:R:S:Y:String
    OicUuid_t       deviceID;     // 1:R:S:Y:oic.uuid
};

#ifdef __cplusplus
}
#endif

#endif //OC_SECURITY_RESOURCE_TYPES_H
