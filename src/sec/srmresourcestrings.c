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

#include "srmresourcestrings.h"

#include <stdlib.h>

#if EXPORT_INTERFACE
extern const char * SVR_DB_FILE_NAME;
extern const char * SVR_DB_DAT_FILE_NAME;
#endif
const char * SVR_DB_FILE_NAME = OC_SECURITY_DB_FILE_NAME;
const char * SVR_DB_DAT_FILE_NAME = OC_SECURITY_DB_DAT_FILE_NAME;

//AMACL
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_AMACL;
extern const char * OIC_RSRC_AMACL_URI;
extern const char * OIC_JSON_AMACL_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_AMACL = "oic.r.amacl";
const char * OIC_RSRC_AMACL_URI =  "/oic/sec/amacl";
const char * OIC_JSON_AMACL_NAME = "amacl";

//ACL
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_ACL;
extern const char * OIC_RSRC_ACL_URI;
extern const char * OIC_JSON_ACL_NAME;
extern const char * OIC_JSON_ACLIST_NAME;
extern const char * OIC_JSON_ACLIST2_NAME;
extern const char * OIC_JSON_ACES_NAME;
extern const char * OIC_JSON_ACEID_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_ACL = "oic.r.acl";
const char * OIC_RSRC_ACL_URI =  "/oic/sec/acl";
const char * OIC_JSON_ACL_NAME = "acl";
const char * OIC_JSON_ACLIST_NAME = "aclist";
const char * OIC_JSON_ACLIST2_NAME = "aclist2";
const char * OIC_JSON_ACES_NAME = "aces";
const char * OIC_JSON_ACEID_NAME = "aceid";

#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_ACL2;
extern const char * OIC_RSRC_TYPE_SEC_ACE2;
extern const char * OIC_RSRC_ACL2_URI;
extern const char * OIC_JSON_ACL2_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_ACL2 = "oic.r.acl2";
const char * OIC_RSRC_TYPE_SEC_ACE2 = "oic.r.ace2";
const char * OIC_RSRC_ACL2_URI = "/oic/sec/acl2";
const char * OIC_JSON_ACL2_NAME = "acl2";

//Pstat
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_PSTAT;
extern const char * OIC_RSRC_PSTAT_URI;
extern const char * OIC_JSON_PSTAT_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_PSTAT = "oic.r.pstat";
const char * OIC_RSRC_PSTAT_URI =  "/oic/sec/pstat";
const char * OIC_JSON_PSTAT_NAME = "pstat";

//sp
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_SP;
extern const char * OIC_RSRC_SP_URI;
extern const char * OIC_JSON_SP_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_SP = "oic.r.sp";
const char * OIC_RSRC_SP_URI =  "/oic/sec/sp";
const char * OIC_JSON_SP_NAME = "sp";

//doxm
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_DOXM;
extern const char * OIC_RSRC_DOXM_URI;
extern const char * OIC_JSON_DOXM_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_DOXM = "oic.r.doxm";
const char * OIC_RSRC_DOXM_URI =  "/oic/sec/doxm";
const char * OIC_JSON_DOXM_NAME = "doxm";

//cred
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_CRED;
extern const char * OIC_RSRC_CRED_URI;
extern const char * OIC_JSON_CRED_NAME;
extern const char * OIC_JSON_CREDS_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_CRED = "oic.r.cred";
const char * OIC_RSRC_CRED_URI =  "/oic/sec/cred";
const char * OIC_JSON_CRED_NAME = "cred";
const char * OIC_JSON_CREDS_NAME = "creds";

//CSR
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_CSR;
extern const char * OIC_RSRC_CSR_URI;
extern const char * OIC_JSON_CSR_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_CSR = "oic.r.csr";
const char * OIC_RSRC_CSR_URI = "/oic/sec/csr";
const char * OIC_JSON_CSR_NAME = "csr";

//roles
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_ROLES;
extern const char * OIC_RSRC_ROLES_URI;
extern const char * OIC_JSON_ROLES_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_ROLES = "oic.r.roles";
const char * OIC_RSRC_ROLES_URI = "/oic/sec/roles";
const char * OIC_JSON_ROLES_NAME = "roles";

//CRL
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_CRL;
extern const char * OIC_RSRC_CRL_URI;
extern const char * OIC_JSON_CRL_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_CRL = "oic.r.crl";
const char * OIC_RSRC_CRL_URI =  "/oic/sec/crl";
const char * OIC_JSON_CRL_NAME = "crl";

//SACL - not implemented yet
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_SACL;
extern const char * OIC_RSRC_SACL_URI;
extern const char * OIC_JSON_SACL_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_SACL = "oic.r.sacl";
const char * OIC_RSRC_SACL_URI =  "/oic/sec/sacl";
const char * OIC_JSON_SACL_NAME = "sacl";

/*
//svc - removed from the OCF 1.0 Security spec and from IoTivity
const char * OIC_RSRC_TYPE_SEC_SVC = "oic.r.svc";
const char * OIC_RSRC_SVC_URI =  "/oic/sec/svc";
const char * OIC_JSON_SVC_NAME = "svc";
*/

//version
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_SEC_VER;
extern const char * OIC_RSRC_VER_URI;
extern const char * OIC_JSON_VER_NAME;
#endif
const char * OIC_RSRC_TYPE_SEC_VER = "oic.r.ver";
const char * OIC_RSRC_VER_URI =  "/oic/sec/ver";
const char * OIC_JSON_VER_NAME = "ver";

//cloud conf
#if EXPORT_INTERFACE
extern const char * OIC_RSRC_TYPE_CLOUD_CONF;
extern const char * OIC_RSRC_CLOUD_CONF_URI;
extern const char * OIC_JSON_CLOUD_CONF_NAME;
#endif
const char * OIC_RSRC_TYPE_CLOUD_CONF = "oic.r.coapcloudconf";
const char * OIC_RSRC_CLOUD_CONF_URI = "/oic/r/coapcloudconf";
const char * OIC_JSON_CLOUD_CONF_NAME = "coapcloudconf";


//reset profile
#if EXPORT_INTERFACE
extern const char * OIC_JSON_RESET_PF_NAME;
extern const char * OIC_JSON_RESOURCES_NAME;
extern const char * OIC_JSON_AMSS_NAME;
extern const char * OIC_JSON_AMS_NAME;
extern const char * OIC_JSON_PERMISSION_NAME;
extern const char * OIC_JSON_OWNERS_NAME;
extern const char * OIC_JSON_OWNER_NAME;
extern const char * OIC_JSON_DEVOWNERID_NAME;
#ifdef MULTIPLE_OWNER
extern const char * OIC_JSON_SUBOWNERID_NAME;
#endif //MULTIPLE_OWNER
extern const char * OIC_JSON_OWNED_NAME;
extern const char * OIC_JSON_OXMS_NAME;
#ifdef MULTIPLE_OWNER
extern const char * OIC_JSON_MOM_NAME;
#endif //MULTIPLE_OWNER
extern const char * OIC_JSON_OXM_SEL_NAME;
extern const char * OIC_JSON_DEVICE_ID_FORMAT_NAME;
extern const char * OIC_JSON_ISOP_NAME;
extern const char * OIC_JSON_COMMIT_HASH_NAME;
extern const char * OIC_JSON_DEVICE_ID_NAME;
extern const char * OIC_JSON_CM_NAME;
extern const char * OIC_JSON_TM_NAME;
extern const char * OIC_JSON_OM_NAME;
extern const char * OIC_JSON_SM_NAME;
extern const char * OIC_JSON_SUPPORTED_SP_NAME;
extern const char * OIC_JSON_CURRENT_SP_NAME;
extern const char * OIC_JSON_CREDID_NAME;
extern const char * OIC_JSON_SUBJECT_NAME;
extern const char * OIC_JSON_SUBJECTID_NAME;
extern const char * OIC_JSON_ROLEID_NAME;
extern const char * OIC_JSON_ROLE_NAME;
extern const char * OIC_JSON_AUTHORITY_NAME;
extern const char * OIC_JSON_CREDTYPE_NAME;
extern const char * OIC_JSON_PUBLICDATA_NAME;
extern const char * OIC_JSON_PRIVATEDATA_NAME;
extern const char * OIC_JSON_PUBDATA_NAME;
extern const char * OIC_JSON_PRIVDATA_NAME;
extern const char * OIC_JSON_OPTDATA_NAME;
extern const char * OIC_JSON_CREDUSAGE_NAME;
extern const char * OIC_JSON_REVOCATION_STATUS_NAME;
extern const char * OIC_JSON_SERVICE_DEVICE_ID;
extern const char * OIC_JSON_SERVICE_TYPE;
extern const char * OIC_JSON_VALIDITY_NAME;
extern const char * OIC_JSON_PERIOD_NAME;
extern const char * OIC_JSON_PERIODS_NAME;
extern const char * OIC_JSON_CRMS_NAME;
extern const char * OIC_JSON_RECURRENCES_NAME;
extern const char * OIC_JSON_SUPPORTED_CRED_TYPE_NAME;
extern const char * OIC_JSON_DPC_NAME;
extern const char * OIC_JSON_EDP_NAME;
extern const char * OIC_JSON_PIN_NAME;
extern const char * OIC_JSON_PDACL_NAME;
extern const char * OIC_JSON_PDDEV_LIST_NAME;
extern const char * OIC_JSON_PRM_NAME;
extern const char * OIC_JSON_SPM_NAME;
extern const char * OIC_JSON_PDEVICE_ID_NAME;
extern const char * OIC_JSON_RLIST_NAME;
extern const char * OIC_JSON_HREF_NAME;
extern const char * OIC_JSON_REL_NAME;
extern const char * OIC_JSON_RT_NAME;
extern const char * OIC_JSON_IF_NAME;
extern const char * OIC_JSON_ROWNERID_NAME;
#ifdef MULTIPLE_OWNER
extern const char * OIC_JSON_EOWNERID_NAME;
#endif //MULTIPLE_OWNER
extern const char * OIC_JSON_ENCODING_NAME;
extern const char * OIC_JSON_DATA_NAME;
extern const char * OIC_JSON_SEC_V_NAME;
extern const char * OIC_JSON_DOS_NAME;
extern const char * OIC_JSON_S_NAME;
extern const char * OIC_JSON_P_NAME;
extern const char * OIC_JSON_UUID_NAME;
extern const char * OIC_JSON_CONNTYPE_NAME;
extern const char * OIC_JSON_AUTHCRYPT_NAME;
extern const char * OIC_JSON_ANONCLEAR_NAME;
extern const char * OIC_JSON_WC_NAME;
extern const char * OIC_JSON_WC_PLUS_NAME;
extern const char * OIC_JSON_WC_MINUS_NAME;
extern const char * OIC_JSON_WC_ASTERISK_NAME;
extern const char * OIC_JSON_EMPTY_STRING;
extern const char * OIC_JSON_ROWNERID_NAME;

#endif

const char * OIC_JSON_RESET_PF_NAME = "resetpf";
const char * OIC_JSON_RESOURCES_NAME = "resources";
const char * OIC_JSON_AMSS_NAME = "amss";
const char * OIC_JSON_AMS_NAME = "ams";
const char * OIC_JSON_PERMISSION_NAME = "permission";
const char * OIC_JSON_OWNERS_NAME = "ownrs";
const char * OIC_JSON_OWNER_NAME = "ownr";
const char * OIC_JSON_DEVOWNERID_NAME = "devowneruuid";
#ifdef MULTIPLE_OWNER
const char * OIC_JSON_SUBOWNERID_NAME = "x.org.iotivity.subowneruuid";
#endif //MULTIPLE_OWNER
const char * OIC_JSON_OWNED_NAME = "owned";
const char * OIC_JSON_OXMS_NAME = "oxms";
#ifdef MULTIPLE_OWNER
const char * OIC_JSON_MOM_NAME = "x.org.iotivity.mom";
#endif //MULTIPLE_OWNER
const char * OIC_JSON_OXM_SEL_NAME = "oxmsel";
const char * OIC_JSON_DEVICE_ID_FORMAT_NAME = "didformat";
const char * OIC_JSON_ISOP_NAME = "isop";
const char * OIC_JSON_COMMIT_HASH_NAME = "ch";
const char * OIC_JSON_DEVICE_ID_NAME = "deviceuuid";
const char * OIC_JSON_CM_NAME = "cm";
const char * OIC_JSON_TM_NAME = "tm";
const char * OIC_JSON_OM_NAME = "om";
const char * OIC_JSON_SM_NAME = "sm";
const char * OIC_JSON_SUPPORTED_SP_NAME = "supportedprofiles";
const char * OIC_JSON_CURRENT_SP_NAME = "currentprofile";
const char * OIC_JSON_CREDID_NAME = "credid";
const char * OIC_JSON_SUBJECT_NAME = "subject";
const char * OIC_JSON_SUBJECTID_NAME = "subjectuuid";
const char * OIC_JSON_ROLEID_NAME = "roleid";
const char * OIC_JSON_ROLE_NAME = "role";
const char * OIC_JSON_AUTHORITY_NAME = "authority";
const char * OIC_JSON_CREDTYPE_NAME = "credtype";
const char * OIC_JSON_PUBLICDATA_NAME = "publicdata";
const char * OIC_JSON_PRIVATEDATA_NAME = "privatedata";
const char * OIC_JSON_PUBDATA_NAME = "pubdata";
const char * OIC_JSON_PRIVDATA_NAME = "privdata";
const char * OIC_JSON_OPTDATA_NAME = "optionaldata";
const char * OIC_JSON_CREDUSAGE_NAME = "credusage";
const char * OIC_JSON_REVOCATION_STATUS_NAME = "revstat";
const char * OIC_JSON_SERVICE_DEVICE_ID = "svcdid";
const char * OIC_JSON_SERVICE_TYPE = "svct";
const char * OIC_JSON_VALIDITY_NAME = "validity";
const char * OIC_JSON_PERIOD_NAME = "period";
const char * OIC_JSON_PERIODS_NAME = "prds";
const char * OIC_JSON_CRMS_NAME = "crms";
const char * OIC_JSON_RECURRENCES_NAME = "recurrence";
const char * OIC_JSON_SUPPORTED_CRED_TYPE_NAME = "sct";
const char * OIC_JSON_DPC_NAME = "dpc";
const char * OIC_JSON_EDP_NAME = "edp";
const char * OIC_JSON_PIN_NAME = "pin";
const char * OIC_JSON_PDACL_NAME = "pdacl";
const char * OIC_JSON_PDDEV_LIST_NAME = "pddev";
const char * OIC_JSON_PRM_NAME = "prm";
const char * OIC_JSON_SPM_NAME = "spm";
const char * OIC_JSON_PDEVICE_ID_NAME = "pdeviceuuid";
const char * OIC_JSON_RLIST_NAME = "rlist";
const char * OIC_JSON_HREF_NAME = OC_RSRVD_HREF;
const char * OIC_JSON_REL_NAME = OC_RSRVD_REL;
const char * OIC_JSON_RT_NAME = OC_RSRVD_RESOURCE_TYPE;
const char * OIC_JSON_IF_NAME = OC_RSRVD_INTERFACE;
const char * OIC_JSON_ROWNERID_NAME = "rowneruuid";
#ifdef MULTIPLE_OWNER
const char * OIC_JSON_EOWNERID_NAME = "x.org.iotivity.eowneruuid";
#endif //MULTIPLE_OWNER
const char * OIC_JSON_ENCODING_NAME = "encoding";
const char * OIC_JSON_DATA_NAME = "data";
const char * OIC_JSON_SEC_V_NAME = "secv";
const char * OIC_JSON_DOS_NAME = "dos";
const char * OIC_JSON_S_NAME = "s";
const char * OIC_JSON_P_NAME = "p";
const char * OIC_JSON_UUID_NAME = "uuid";
const char * OIC_JSON_CONNTYPE_NAME = "conntype";
const char * OIC_JSON_AUTHCRYPT_NAME = "auth-crypt";
const char * OIC_JSON_ANONCLEAR_NAME = "anon-clear";
const char * OIC_JSON_WC_NAME = "wc";
const char * OIC_JSON_WC_PLUS_NAME = "+";
const char * OIC_JSON_WC_MINUS_NAME = "-";
const char * OIC_JSON_WC_ASTERISK_NAME = "*";
const char * OIC_JSON_EMPTY_STRING = "";

// Certificates provided by Cloud
#if EXPORT_INTERFACE
extern const char * TRUST_CA;
extern const char * PRIMARY_CERT;
extern const char * ROLE_CERT;
#endif
const char * TRUST_CA = "oic.sec.cred.trustca";
const char * PRIMARY_CERT = "oic.sec.cred.cert"; /* identity certificate, EKU OID 1.3.6.1.4.1.44924.1.6 */
const char * ROLE_CERT = "oic.sec.cred.rolecert";

// Certificates provided by manufacturer
const char * MF_TRUST_CA = "oic.sec.cred.mfgtrustca";
const char * MF_PRIMARY_CERT = "oic.sec.cred.mfgcert";

OicUuid_t WILDCARD_SUBJECT_ID = {"*"};
OicUuid_t WILDCARD_SUBJECT_B64_ID = { .id = {'2', '2', '2', '2', '2', '2', '2', '2',
                                             '2', '2', '2', '2', '2', '2', '2', '2' }};
#if EXPORT_INTERFACE
#include <stdlib.h>
#endif
size_t WILDCARD_SUBJECT_ID_LEN = 1;

#if EXPORT_INTERFACE
extern const char * WILDCARD_RESOURCE_URI;
#endif
const char * WILDCARD_RESOURCE_URI = "*";

//Ownership Transfer Methods
const char * OXM_JUST_WORKS = "oic.sec.doxm.jw";
const char * OXM_RANDOM_DEVICE_PIN = "oic.sec.doxm.rdp";
const char * OXM_MANUFACTURER_CERTIFICATE = "oic.sec.doxm.mfgcert";
#ifdef MULTIPLE_OWNER
const char * OXM_PRECONF_PIN = "x.org.iotivity.sec.doxm.pcp";
#endif //MULTIPLE_OWNER
const char * OXM_MV_JUST_WORKS = "x.org.iotivity.sec.doxm.mvjw";
const char * OXM_CON_MFG_CERT = "x.org.iotivity.conmfgcert";

//Mutual Verified Just-Works Message Prefix
const char * MUTUAL_VERIF_NUM = "mutualVerifNum";

//Credential data encoding methods
#if EXPORT_INTERFACE
extern const char * OIC_SEC_ENCODING_BASE64;
extern const char * OIC_SEC_ENCODING_RAW;
extern const char * OIC_SEC_ENCODING_PEM;
extern const char * OIC_SEC_ENCODING_DER;
#endif
const char * OIC_SEC_ENCODING_BASE64 = "oic.sec.encoding.base64";
const char * OIC_SEC_ENCODING_RAW = "oic.sec.encoding.raw";
const char * OIC_SEC_ENCODING_PEM = "oic.sec.encoding.pem";
const char * OIC_SEC_ENCODING_DER = "oic.sec.encoding.der";

const char * OIC_SEC_TRUE = "true";
const char * OIC_SEC_FALSE = "false";

const char * OIC_SEC_REST_QUERY_SEPARATOR = ";";
char OIC_SEC_REST_QUERY_DELIMETER = '=';

//Spec Version
const char * DEFAULT_SPEC_VERSION = "core.0.0.0";

//Preconfigured Pin credential identifier
#ifdef MULTIPLE_OWNER
const char * PRECONFIG_PIN_CRED = "x.org.iotivity.sec.cred.pcp";
#endif //MULTIPLE_OWNER
