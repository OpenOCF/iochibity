cc_library(
    name = "pbkdf2",
    srcs = ["pbkdf2.c",
            "pbkdf2.h"]
)

cc_library(
    name = "acl",
    copts = ["-Isrc",
             "-Isrc/comm",
             "-Isrc/comm/common",
             "-Isrc/comm/include",
             "-Isrc/ocf",
             "-Isrc/ocf/internal",
             "-Isrc/portability",
             "-Isrc/util",
             "-Isrc/sec/ocsecurity",
             "-Ithird_party/cbor",
             "-Ithird_party/cjson",
             "-Ithird_party/coap",
             "-Ithird_party/tinydtls"],
    deps = ["//src:config",
            "//src/comm",
            "//src/ocf:interface",
            "//src/portability",
            #"//src/provisioning",
            "//src/util",
            "//third_party/cbor",
            "//third_party/cjson",
            "//third_party/coap",
            "//third_party/tinydtls"],
    srcs = ["aclresource.c",
            "aclresource.h",
            "acl_logging.h",
            "doxmresource.h",
            "rolesresource.h",
            "secureresourcemanager.h",
            "deviceonboardingstate.h",
            "security_internals.h",

	    "amaclresource.c",
	    "amaclresource.h",

    	    "psinterface.h",
	    "resourcemanager.h",
            "experimental/securevirtualresourcetypes.h",
            "srmresourcestrings.h",
            "srmutility.h"
]
)

cc_library(
    name = "sec",          # libocsrm
    copts = ["-Isrc",
             "-Isrc/comm",
             "-Isrc/comm/common",
             "-Isrc/comm/include",
             "-Isrc/ocf",
             "-Isrc/ocf/internal",
             "-Isrc/portability",
             "-Isrc/util",
             "-Isrc/sec/ocsecurity",
             "-Ithird_party/cbor",
             "-Ithird_party/cjson",
             "-Ithird_party/coap",
             "-Ithird_party/tinydtls",
             "-Iexternal/mbedtls/mbedtls-2.4.2/include"],
    deps = ["//src:config",
            "//src/comm",
            "//src/ocf:interface",
            "//src/portability",
            #"//src/provisioning",
            "//src/util",
            "//third_party/cbor",
            "//third_party/cjson",
            "//third_party/coap",
            "//third_party/tinydtls",
            "@mbedtls//:mbedtls-lib"],

#  	'#resource/csdk/stack',
#  	'#resource/csdk/logger',
#  	'#resource/csdk/stack/internal',
# # 	'#resource/oc_logger/include',
#  	'#resource/csdk/connectivity/lib/libcoap-4.1.1',
# # 	'#resource/csdk/connectivity/external/inc',
#  	'#resource/csdk/connectivity/common',
# # 	'#resource/csdk/connectivity/inc',
#  	'#resource/csdk/connectivity/api',
# # 	'#resource/csdk/security',
#  	'#resource/csdk/security/ocsecurity',
#  	'#resource/csdk/provisioning'

    srcs = ["aclresource.c",
            "aclresource.h",

	    "amaclresource.c",
	    "amaclresource.h",
	    # "amsmgr.c",  # removed in 1.3
	    # "amsmgr.h",

	    "base64.c",
	    "base64.h",

            "certhelpers.c",    # SECURED=1
            "certhelpers.h",    # SECURED=1

            "csrresource.c",
            "csrresource.h",

	    "credresource.c",
	    "credresource.h",

            # if env.get('DTLS_WITH_X509') == '1':  # and env.get('SECURED'):
	    "crlresource.c",    # SECURED=1
	    "crlresource.h",    # SECURED=1
            "crl_logging.h",

            "deviceonboardingstate.c",
            "deviceonboardingstate.h",

	    # DEPRECATED "directpairing.c",
	    # DEPRECATED "directpairing.h",

	    "doxmresource.c",
	    "doxmresource.h",

	    # DEPRECATED "dpairingresource.c",
	    # DEPRECATED "dpairingresource.h",

            "experimental/byte_array.h",
            "experimental/securevirtualresourcetypes.h",

	    "iotvticalendar.c",
	    "iotvticalendar.h",

            "occertutility.c",  # SECURED=1
            "occertutility.h",  # SECURED=1

            "ocsecurity.c",     # SECURED=1
            "ocsecurity.h",

            # "securevirtualresourcetypes.h",

            "oxmpincommon.c",   # SECURED=1

            "oxmverifycommon.c", # SECURED=1
            "oxmverifycommon.h", # SECURED=1

            "pbkdf2.c",         # SECURED=1
            "pbkdf2.h",        # SECURED=1

	    "pconfresource.c",
	    "pconfresource.h",

            "pinoxmcommon.h",

            "pkix_interface.c", # SECURED=1
            "pkix_interface.h", # SECURED=1

	    "policyengine.c",
	    "policyengine.h",

	    "psinterface.c",
	    "psinterface.h",

	    "pstatresource.c",
	    "pstatresource.h",

	    "resourcemanager.c",
	    "resourcemanager.h",

            "rolesresource.c",   # SECURED=1
            "rolesresource.h",   # SECURED=1

            "secureresourcemanager.c",
            "secureresourcemanager.h",

            "security_internals.h",

	    "srmresourcestrings.c",
	    "srmresourcestrings.h",

	    "srmutility.c",
	    "srmutility.h"],

	    # "svcresource.c",
	    # "svcresource.h"],

    visibility = ["//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__"]
    )
