package openocf.r;

import openocf.exceptions.OCFNotImplementedException;

import java.util.LinkedList;
import java.util.UUID;

// FIXME: put these in a constants classes, e.g.

// constants.OCF_URI - e.g. OCF_URI.PLATFORM = "oic/p"
// constants.Identifiers - e.g. "oic.wk.p"
// constants.ResourceType - e.g. RT.PLATFORM = "oic.wk.p"
// constants.Interface -    e.g. IF.BASELINE = "oic.if.baseline"
// constants.ResourceNames - e.g. ResourceName.ROWNER = "rowner"
// etc.

public class Resource
{
    // Policy constants (OCResourceProperty enum)
    // constants.ResourcePolicy
    // public static final int NOMETHOD      = 0;
    // public static final int NONE          = 0;
    // public static final int DISCOVERABLE  = (1 << 0);
    // public static final int WATCHABLE     = (1 << 1);
    // public static final int ACTIVE        = (1 << 2);
    // public static final int SLOW          = (1 << 3);
    // public static final int SECURE        = (1 << 4);
    // public static final int EXPLICIT_DISCOVERABLE   = (1 << 5);

    // standard resource URIs, properties, types, interfaces
    public static final String URI_PLATFORM    = "oic/p";
    public static final String RT_PLATFORM     = "oic.wk.p";
    public static final String N_PLATFORM_ID   = "pi";
    public static final String N_MFG_NAME      = "mnmn";
    public static final String N_MFG_DTLS_LINK = "mnml";
    public static final String N_MFG_MDL_NBR   = "mnmo";
    public static final String N_MFG_DATE      = "mndt";
    public static final String N_PLATFORM_VERSION = "mnpv";
    public static final String N_OS_VERSION       = "mnos";
    public static final String N_HW_VERSION       = "mnhw"; // hardware version
    public static final String N_FW_VERSION       = "mnfw"; // firmware version
    public static final String N_SUPPORT_LINK     = "mnsl"; // a URI
    public static final String N_SYSTEM_TIME      = "st";   // date and time concatted
    public static final String N_VENDOR_ID        = "vid";  // string

    public static final String URI_DEVICE         = "oic/d";
    public static final String RT_DEVICE          = "oic.wk.d";
    public static final String N_NAME             = "n";
    public static final String N_SPEC_VERSION     = "icv"; // core.1.1.0 (core.<major>.<minor>.<subversion>)
    public static final String N_DEVICE_ID        = "di";
    public static final String N_DATA_MDL_VERSION = "dmv";

    public static final String URI_RESOURCES  = "oic/res";
    public static final String RT_RESOURCES   = "oic.wk.res";
    public static final String N_LINKS        = "links";
    public static final String N_MSG_PROTOCOL = "mpro";

    public static final String URI_CONFIG    = "oic/con";
    public static final String RT_CONFIG     = "oic.wk.con";

    public static final String URI_MAINTENANCE = "oic/mnt";
    public static final String RT_MAINTENANCE  = "oic.wk.mnt";


    public static final String URI_ADVERT    = "oic/ad";
    public static final String URI_RD        = "oic/rd";

    public static final String T_AD          = "oic.wk.ad";

    public static final String IF_BASELINE   = "oic.if.baseline";
    public static final String IF_READ       = "oic.if.r";
    public static final String IF_LINKLIST   = "oic.if.ll";
    public static final String IF_BATCH      = "oic.if.b";
    public static final String IF_GROUP      = "oic.if.grp";

    // security resources
    public static final String URI_DOXM        = "oic/sec/doxm"; // dev ownership xfer method
    public static final String T_DOXM_RESOURCE = "oic.r.doxm";
    public static final String T_DOXM_SEC      = "oic.sec.doxmtype";
    public static final String T_OXM_JUST_WORKS     = "oic.sec.doxm.jw";
    public static final String T_OXM_RANDOM_PIN     = "oic.sec.doxm.rdp";
    public static final String T_OXM_MFG_CERT       = "oic.sec.doxm.mfgcert";

    public static final String URI_ACL       = "oic/sec/acl";
    public static final String T_ACL         = "oic.r.acl";
    public static final String URI_AMACL     = "oic/sec/amacl"; // access manager
    public static final String T_AMACL       = "oic.r.amacl";
    public static final String URI_SACL      = "oic/sec/sacl"; // signed acl
    public static final String T_SACL        = "oic.r.sacl";

    public static final String URI_PROVISIONING_STATUS = "oic/sec/pstat";
    public static final String T_PROVISIONING_STATUS   = "oic.r.pstat";

    public static final String URI_CREDENTIALS = "oic/sec/cred";
    public static final String T_CREDENTIALS   = "oic.r.cred";

    public static final String URI_CERT_REVOCATION_LIST = "oic/sec/crl";
    public static final String N_CRL_ID                 = "crlid";
    public static final String N_CRL_THISUPDATE         = "thisupdate";
    public static final String N_CRL_DATA               = "crldata";

    public static final String URI_SEC_SERVICES   = "oic/sec/svc";
    public static final String T_SEC_SERVICES     = "oic.r.svc";
    public static final String T_SVC_HOSTS                = "hosts";
    public static final String N_SVC_ID                   = "svcid";
    public static final String N_SVC_SVC_TS               = "svcs";
    public static final String N_SVC_CRED_TS              = "sct";
    public static final String N_SVC_SERVER_CRED_ID       = "scid";
    public static final String N_SVC_CLIENT_CRED_ID       = "ccid";
    public static final String N_SVC_CRED_REFRESH_METHODS = "crms";

    public static final String T_SEC_SVC_TYPE     = "oic.sec.svctype";
    public static final String N_SVC_LIST         = "sl";
    public static final String N_SVC_ROWNER       = "rowner";
}
