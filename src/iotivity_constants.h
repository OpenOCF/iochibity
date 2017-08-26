#ifndef IOTIVITY_CONSTANTS_
#define IOTIVITY_CONSTANTS_

/*GAR: migrated from octypes.h */

/** Version of IoTivity. */
#define IOTIVITY_VERSION                      "1.3.0"

/**
 * OIC Virtual resources supported by every OIC device.
 */
/**
 *  Default discovery mechanism using '/oic/res' is supported by all OIC devices
 *  That are Discoverable.
 */
#define OC_RSRVD_WELL_KNOWN_URI               "/oic/res"

/** Device URI.*/
#define OC_RSRVD_DEVICE_URI                   "/oic/d"

/** Platform URI.*/
#define OC_RSRVD_PLATFORM_URI                 "/oic/p"

/** Resource Type.*/
#define OC_RSRVD_RESOURCE_TYPES_URI           "/oic/res/types/d"

/** Gateway URI.*/
#define OC_RSRVD_GATEWAY_URI                  "/oic/gateway"

/** MQ Broker URI.*/
#define OC_RSRVD_WELL_KNOWN_MQ_URI            "/oic/ps"

/** KeepAlive URI.*/
#define OC_RSRVD_KEEPALIVE_URI                "/oic/ping"

/** Presence */

/** Presence URI through which the OIC devices advertise their presence.*/
#define OC_RSRVD_PRESENCE_URI                 "/oic/ad"

/** Presence URI through which the OIC devices advertise their device presence.*/
#define OC_RSRVD_DEVICE_PRESENCE_URI         "/oic/prs"

/** Sets the default time to live (TTL) for presence.*/
#define OC_DEFAULT_PRESENCE_TTL_SECONDS       (60)

/** For multicast Discovery mechanism.*/
#define OC_MULTICAST_DISCOVERY_URI            "/oic/res"

/** Separator for multiple query string.*/
#define OC_QUERY_SEPARATOR                    "&;"

/** Delimeter for keys and values in query string.*/
#define OC_KEY_VALUE_DELIMITER                "="

/**
 *  OC_DEFAULT_PRESENCE_TTL_SECONDS sets the default time to live (TTL) for presence.
 */
#define OC_DEFAULT_PRESENCE_TTL_SECONDS (60)

/**
 *  OC_MAX_PRESENCE_TTL_SECONDS sets the maximum time to live (TTL) for presence.
 *  NOTE: Changing the setting to a longer duration may lead to unsupported and untested
 *  operation.
 *  60 sec/min * 60 min/hr * 24 hr/day
 */
#define OC_MAX_PRESENCE_TTL_SECONDS     (60 * 60 * 24)


/**
 *  Presence "Announcement Triggers".
 */

/** To create.*/
#define OC_RSRVD_TRIGGER_CREATE         "create"

/** To change.*/
#define OC_RSRVD_TRIGGER_CHANGE         "change"

/** To delete.*/
#define OC_RSRVD_TRIGGER_DELETE         "delete"

/**
 *  Attributes used to form a proper OIC conforming JSON message.
 */

#define OC_RSRVD_OC                     "oic"


/** For payload. */

#define OC_RSRVD_PAYLOAD                "payload"

/** To represent href */
#define OC_RSRVD_HREF                   "href"

/** To represent property*/
#define OC_RSRVD_PROPERTY               "prop"

/** For representation.*/
#define OC_RSRVD_REPRESENTATION         "rep"

/** To represent content type.*/
#define OC_RSRVD_CONTENT_TYPE           "ct"

/** To represent resource type.*/
#define OC_RSRVD_RESOURCE_TYPE          "rt"

/** To represent resource type with presence.*/
#define OC_RSRVD_RESOURCE_TYPE_PRESENCE "oic.wk.ad"

/** To represent resource type with device.*/
#define OC_RSRVD_RESOURCE_TYPE_DEVICE   "oic.wk.d"

/** To represent resource type with platform.*/
#define OC_RSRVD_RESOURCE_TYPE_PLATFORM "oic.wk.p"

/** To represent resource type with maintenance.*/
#define OC_RSRVD_RESOURCE_TYPE_MAINTENANCE "oic.wk.mnt"

/** To represent resource type with collection.*/
#define OC_RSRVD_RESOURCE_TYPE_COLLECTION "oic.wk.col"

/** To represent resource type with RES.*/
#define OC_RSRVD_RESOURCE_TYPE_RES      "oic.wk.res"

/** To represent content type with MQ Broker.*/
#define OC_RSRVD_RESOURCE_TYPE_MQ_BROKER "oic.wk.ps"

/** To represent content type with MQ Topic.*/
#define OC_RSRVD_RESOURCE_TYPE_MQ_TOPIC  "oic.wk.ps.topic"

/** To represent resource type with introspection.*/
#define OC_RSRVD_RESOURCE_TYPE_INTROSPECTION "oic.wk.introspection"

/** To represent resource type with introspection payload.*/
#define OC_RSRVD_RESOURCE_TYPE_INTROSPECTION_PAYLOAD "oic.wk.introspection.payload"

/** To represent interface.*/
#define OC_RSRVD_INTERFACE              "if"

/** To indicate how long RD should publish this item.*/
#define OC_RSRVD_DEVICE_TTL             "lt"

/** To represent time to live.*/
#define OC_RSRVD_TTL                    "ttl"

/** To represent non*/
#define OC_RSRVD_NONCE                  "non"

/** To represent trigger type.*/
#define OC_RSRVD_TRIGGER                "trg"

/** To represent links.*/
#define OC_RSRVD_LINKS                  "links"

/** To represent default interface.*/
#define OC_RSRVD_INTERFACE_DEFAULT      "oic.if.baseline"

/** To represent read-only interface.*/
#define OC_RSRVD_INTERFACE_READ         "oic.if.r"

/** To represent read-write interface.*/
#define OC_RSRVD_INTERFACE_READ_WRITE   "oic.if.rw"

/** To represent ll interface.*/
#define OC_RSRVD_INTERFACE_LL           "oic.if.ll"

/** To represent batch interface.*/
#define OC_RSRVD_INTERFACE_BATCH        "oic.if.b"

/** To represent actuator interface.*/
#define OC_RSRVD_INTERFACE_ACTUATOR     "oic.if.a"

/** To represent sensor interface.*/
#define OC_RSRVD_INTERFACE_SENSOR       "oic.if.s"

/** To represent interface group.*/
#define OC_RSRVD_INTERFACE_GROUP        "oic.mi.grp"

/** To represent FW version.*/
#define OC_RSRVD_FW_VERSION             "mnfv"

/** To represent host name.*/
#define OC_RSRVD_HOST_NAME              "hn"

/** To represent policy.*/
#define OC_RSRVD_POLICY                 "p"

/** To represent bitmap.*/
#define OC_RSRVD_BITMAP                 "bm"

/** For security.*/
#define OC_RSRVD_SECURE                 "sec"

/** Security Database persistent store name(json). */
#define OC_SECURITY_DB_FILE_NAME        "oic_svr_db.json"

/** Security Database persistent store name(dat).*/
#define OC_SECURITY_DB_DAT_FILE_NAME    "oic_svr_db.dat"

/** Port. */
#define OC_RSRVD_HOSTING_PORT           "port"

/** TCP Port. */
#define OC_RSRVD_TCP_PORT               "x.org.iotivity.tcp"

/** TLS Port. */
#define OC_RSRVD_TLS_PORT               "x.org.iotivity.tls"

/** For Server instance ID.*/
#define OC_RSRVD_SERVER_INSTANCE_ID     "sid"

/** To represent endpoints.*/
#define OC_RSRVD_ENDPOINTS              "eps"

/** To represent endpoint.*/
#define OC_RSRVD_ENDPOINT               "ep"

/** To represent priority.*/
#define OC_RSRVD_PRIORITY               "pri"

/** For resource instance ID.*/
#define OC_RSRVD_INSTANCE_ID            "id"

/**
 *  Platform.
 */

/** Platform ID. */
#define OC_RSRVD_PLATFORM_ID            "pi"

/** Platform MFG NAME. */
#define OC_RSRVD_MFG_NAME               "mnmn"

/** Platform URL. */
#define OC_RSRVD_MFG_URL                "mnml"

/** Model Number.*/
#define OC_RSRVD_MODEL_NUM              "mnmo"

/** Platform MFG Date.*/
#define OC_RSRVD_MFG_DATE               "mndt"

/** Platform versio.n */
#define OC_RSRVD_PLATFORM_VERSION       "mnpv"

/** Platform Operating system version. */
#define OC_RSRVD_OS_VERSION             "mnos"

/** Platform Hardware version. */
#define OC_RSRVD_HARDWARE_VERSION       "mnhw"

/**Platform Firmware version. */
#define OC_RSRVD_FIRMWARE_VERSION       "mnfv"

/** Support URL for the platform. */
#define OC_RSRVD_SUPPORT_URL            "mnsl"

/** System time for the platform. */
#define OC_RSRVD_SYSTEM_TIME            "st"

/** VID for the platform. */
#define OC_RSRVD_VID                    "vid"

/**
 *  Device.
 */

/** Device ID.*/
#define OC_RSRVD_DEVICE_ID               "di"

/** Device Name.*/
#define OC_RSRVD_DEVICE_NAME             "n"

/** Device specification version.*/
#define OC_RSRVD_SPEC_VERSION            "icv"

/** Device data model.*/
#define OC_RSRVD_DATA_MODEL_VERSION      "dmv"

/** Device description, localized */
#define OC_RSRVD_DEVICE_DESCRIPTION      "ld"

/** Device software version */
#define OC_RSRVD_SOFTWARE_VERSION        "sv"

/** Device manufacturer name, localized */
#define OC_RSRVD_DEVICE_MFG_NAME         "dmn"

/** Device model number */
#define OC_RSRVD_DEVICE_MODEL_NUM        "dmno"

/** Protocol-Independent ID.*/
#define OC_RSRVD_PROTOCOL_INDEPENDENT_ID "piid"

/** Device specification version.*/
#define OC_SPEC_VERSION                  "ocf.1.1.0"

/** Integer value of spec version (OCF1.0 0b0000:1000:0000:0000).*/
#define OC_SPEC_VERSION_VALUE            2048

/** Device Data Model version.*/
#define OC_DATA_MODEL_VERSION            "ocf.res.1.1.0,ocf.sh.1.1.0"

/**
 *  Introspection.
 */
/** Name property name(n).*/
#define OC_RSRVD_INTROSPECTION_NAME     "n"

/** Value of name.*/
#define OC_RSRVD_INTROSPECTION_NAME_VALUE "Introspection"

/** Url info Object.*/
#define OC_RSRVD_INTROSPECTION_URL_INFO "urlInfo"

/** Url property.*/
#define OC_RSRVD_INTROSPECTION_URL      "url"

/** Protocol property.*/
#define OC_RSRVD_INTROSPECTION_PROTOCOL    "protocol"

/** Content-Type property.*/
#define OC_RSRVD_INTROSPECTION_CONTENT_TYPE "content-type"

/** Content-Type property value.*/
#define OC_RSRVD_INTROSPECTION_CONTENT_TYPE_VALUE "application/cbor"

/** Version property.*/
#define OC_RSRVD_INTROSPECTION_VERSION    "version"

/** Version property value.*/
#define OC_RSRVD_INTROSPECTION_VERSION_VALUE 1

/** Introspection persistent store name.*/
#define OC_INTROSPECTION_FILE_NAME      "introspection.dat"

/**
 *  These provide backward compatibility - their use is deprecated.
 */
#ifndef GOING_AWAY

/** Multicast Prefix.*/
#define OC_MULTICAST_PREFIX             "224.0.1.187:5683"

/** Multicast IP address.*/
#define OC_MULTICAST_IP                 "224.0.1.187"

/** Multicast Port.*/
#define OC_MULTICAST_PORT               (5683)
#endif // GOING_AWAY


/** Length of MAC address */
#define MAC_ADDR_STR_SIZE (17)

/** Blocks of MAC address */
#define MAC_ADDR_BLOCKS (6)

/** Max identity size. */
#define MAX_IDENTITY_SIZE (37)

/** Universal unique identity size. */
#define UUID_IDENTITY_SIZE (128/8)

/** Resource Directory */

/** Resource Directory URI used to Discover RD and Publish resources.*/
#define OC_RSRVD_RD_URI                  "/oic/rd"

/** To represent resource type with rd.*/
#define OC_RSRVD_RESOURCE_TYPE_RD        "oic.wk.rd"

/** RD Discovery bias factor type. */
#define OC_RSRVD_RD_DISCOVERY_SEL        "sel"

/** Resource URI used to discover Proxy */
#define OC_RSRVD_PROXY_URI "/oic/chp"

/** Resource URI used to discover Proxy */
#define OC_RSRVD_PROXY_OPTION_ID 35

/** Unique value per collection/link. */
#define OC_RSRVD_INS                     "ins"

/** Allowable resource types in the links. */
#define OC_RSRVD_RTS                     "rts"

/** Default relationship. */
#define OC_RSRVD_DREL                    "drel"

/** Defines relationship between links. */
#define OC_RSRVD_REL                     "rel"

/** Defines title. */
#define OC_RSRVD_TITLE                   "title"

/** Defines URI. */
#define OC_RSRVD_URI                     "anchor"

/** Defines media type. */
#define OC_RSRVD_MEDIA_TYPE              "type"

/** To represent resource type with Publish RD.*/
#define OC_RSRVD_RESOURCE_TYPE_RDPUBLISH "oic.wk.rdpub"

/** Cloud Account */

/** Account URI.*/
#define OC_RSRVD_ACCOUNT_URI               "/oic/account"

/** Account user URI.*/
#define OC_RSRVD_ACCOUNT_SEARCH_URI        "/oic/account/search"

/** Account session URI.*/
#define OC_RSRVD_ACCOUNT_SESSION_URI       "/oic/account/session"

/** Account token refresh URI.*/
#define OC_RSRVD_ACCOUNT_TOKEN_REFRESH_URI "/oic/account/tokenrefresh"

/** ACL group URI.*/
#define OC_RSRVD_ACL_GROUP_URI             "/oic/acl/group"

/** ACL invite URI.*/
#define OC_RSRVD_ACL_INVITE_URI            "/oic/acl/invite"

/** Defines auth provider. */
#define OC_RSRVD_AUTHPROVIDER              "authprovider"

/** Defines auth code. */
#define OC_RSRVD_AUTHCODE                  "authcode"

/** Defines access token. */
#define OC_RSRVD_ACCESS_TOKEN              "accesstoken"

/** Defines login. */
#define OC_RSRVD_LOGIN                     "login"

/** Defines search. */
#define OC_RSRVD_SEARCH                    "search"

/** Defines grant type. */
#define OC_RSRVD_GRANT_TYPE                "granttype"

/** Defines refresh token. */
#define OC_RSRVD_REFRESH_TOKEN             "refreshtoken"

/** Defines user UUID. */
#define OC_RSRVD_USER_UUID                 "uid"

/** Defines group ID. */
#define OC_RSRVD_GROUP_ID                  "gid"

/** Defines member of group ID. */
#define OC_RSRVD_MEMBER_ID                 "mid"

/** Defines invite. */
#define OC_RSRVD_INVITE                    "invite"

/** Defines accept. */
#define OC_RSRVD_ACCEPT                    "accept"

/** Defines operation. */
#define OC_RSRVD_OPERATION                 "op"

/** Defines add. */
#define OC_RSRVD_ADD                       "add"

/** Defines delete. */
#define OC_RSRVD_DELETE                    "delete"

/** Defines owner. */
#define OC_RSRVD_OWNER                     "owner"

/** Defines members. */
#define OC_RSRVD_MEMBERS                   "members"

/** To represent grant type with refresh token. */
#define OC_RSRVD_GRANT_TYPE_REFRESH_TOKEN  "refresh_token"

/** Cloud CRL */
#define OC_RSRVD_PROV_CRL_URL              "/oic/credprov/crl"

#define OC_RSRVD_LAST_UPDATE              "lu"

#define OC_RSRVD_THIS_UPDATE              "thisupdate"

#define OC_RSRVD_NEXT_UPDATE              "nu"

#define OC_RSRVD_SERIAL_NUMBERS           "rcsn"

#define OC_RSRVD_CRL                      "crldata"

#define OC_RSRVD_CRL_ID                   "crlid"

/** Cloud ACL */
#define OC_RSRVD_GROUP_URL                "/oic/group"

#define OC_RSRVD_ACL_GROUP_URL            "/oic/acl/group"

#define OC_RSRVD_ACL_INVITE_URL           "/oic/acl/invite"

#define OC_RSRVD_ACL_VERIFY_URL           "/oic/acl/verify"

#define OC_RSRVD_ACL_ID_URL               "/oic/acl/id"

#define OC_RSRVD_MEMBER_ID                "mid"

#define OC_RSRVD_GROUP_ID                 "gid"

#define OC_RSRVD_OWNER_ID                 "oid"

#define OC_RSRVD_ACL_ID                   "aclid"

#define OC_RSRVD_ACE_ID                   "aceid"

#define OC_RSRVD_DEVICE_ID                "di"

#define OC_RSRVD_SUBJECT_ID               "sid"

#define OC_RSRVD_REQUEST_METHOD           "rm"

#define OC_RSRVD_REQUEST_URI              "uri"

#define OC_RSRVD_GROUP_MASTER_ID          "gmid"

#define OC_RSRVD_GROUP_TYPE               "gtype"

#define OC_RSRVD_SUBJECT_TYPE             "stype"

#define OC_RSRVD_GROUP_ID_LIST            "gidlist"

#define OC_RSRVD_MEMBER_ID_LIST           "midlist"

#define OC_RSRVD_DEVICE_ID_LIST           "dilist"

#define OC_RSRVD_ACCESS_CONTROL_LIST      "aclist"

#define OC_RSRVD_RESOURCES                "resources"

#define OC_RSRVD_VALIDITY                 "validity"

#define OC_RSRVD_PERIOD                   "period"

#define OC_RSRVD_RECURRENCE               "recurrence"

#define OC_RSRVD_INVITE                   "invite"

#define OC_RSRVD_INVITED                  "invited"

#define OC_RSRVD_ENCODING                 "encoding"

#define OC_OIC_SEC                        "oic.sec"

#define OC_RSRVD_BASE64                   "base64"

#define OC_RSRVD_DER                      "der"

#define OC_RSRVD_PEM                      "pem"

#define OC_RSRVD_RAW                      "raw"

#define OC_RSRVD_UNKNOWN                  "unknown"

#define OC_RSRVD_DATA                     "data"

#define OC_RSRVD_RESOURCE_OWNER_UUID      "rowneruuid"

#define OC_RSRVD_SUBJECT_UUID             "subjectuuid"

#define OC_RSRVD_PERMISSION_MASK          "permission"

#define OC_RSRVD_GROUP_PERMISSION         "gp"

#define OC_RSRVD_GROUP_ACL                "gacl"

/** Certificete Sign Request */
#define OC_RSRVD_PROV_CERT_URI            "/oic/credprov/cert"

#define OC_RSRVD_CSR                      "csr"

#define OC_RSRVD_CERT                     "cert"

#define OC_RSRVD_CACERT                   "certchain"

#define OC_RSRVD_TOKEN_TYPE               "tokentype"

#define OC_RSRVD_EXPIRES_IN               "expiresin"

#define OC_RSRVD_REDIRECT_URI             "redirecturi"

#define OC_RSRVD_CERTIFICATE              "certificate"

#endif /* IOTIVITY_CONSTANTS_ */
