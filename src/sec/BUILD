config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "pbkdf2",
    srcs = ["pbkdf2.c",
            "pbkdf2.h"]
)

cc_library(
    name = "srm",
    copts = ["-Isrc/ocf"],
    deps = ["//src/ocf"],
    srcs = ["secureresourcemanager.c",
            "secureresourcemanager.h"]
)

cc_library(
    name = "interface",
    copts = ["-Isrc/comm/api"],
    #          "-Isrc/comm/common",
    #          "-Isrc/comm/include",
    #          "-Isrc/comm/util",
    #          "-Isrc/ocf",
    #          "-Isrc/ocf/internal",
    #          "-Isrc/util",
    #          #"-Isrc/sec/ocsecurity",
    #          "-Ithird_party/coap",
    #          "-Ithird_party/cbor"],
    deps = ["//src/comm/api"],
    #         "//src/util",       # c_common
    #         "//third_party/cbor",
    #         "//third_party/cjson",
    #         "//src/routing",
    #         "//src/comm"],
    #         #"//src/sec",
    #         #"//src/provisioning"],
    hdrs = ["securevirtualresourcetypes.h"],
    srcs = ["securevirtualresourcetypes.h"],
    visibility = ["//src/sec:__pkg__",
                  "//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__"]
)

cc_library(
    name = "aclx",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/comm",
             "-Isrc/comm/adapter_util",
             "-Isrc/comm/api",
             "-Isrc/comm/common",
             "-Isrc/comm/include",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Isrc/ocf/internal",
             "-Isrc/portability",
             "-Isrc/sec",
             "-Isrc/sec/security",
             "-Isrc/util",
             "-Ithird_party/cjson",
             "-Ithird_party/coap/include",
             "-Ithird_party/mbedtls/include",
             "-Ithird_party/tinycbor/src"]
                 + select({"windows": [],
                           "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/comm",
            "//src/comm/adapter_util:utils",
            "//src/comm/api",
            "//src/logger",
            "//src/ocf:interface",
            "//src/portability",
            #"//src/provisioning",
            "//src/util",
            "//third_party/tinycbor",
            "//third_party/cjson",
            "//third_party/coap",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509"],
    srcs = [#"aclresource.c",
            # "aclresource.h",
            # "acl_logging.h",
            "security/doxmresource.h",

            "rolesresource.c",
            "rolesresource.h",
            "certhelpers.h",

            "secureresourcemanager.h",
            "deviceonboardingstate.h",
            "security_internals.h",

	    "amaclresource.c",
	    "amaclresource.h",

    	    "psinterface.h",
	    "resourcemanager.c",
	    "resourcemanager.h",
            "pstatresource.h",
            "credresource.h",
	    "crlresource.h",    # SECURED=1
            "csrresource.h",

            "securevirtualresourcetypes.h",
            "srmresourcestrings.h",
            "srmutility.h"
    ],    
    visibility = ["//src/sec:__subpackages__"]

)

cc_library(
    name = "helpers",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/comm/api",
             "-Isrc/comm/common",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Isrc/portability",
             "-Isrc/provisioning/include",
             "-Isrc/sec/security",
             "-Isrc/util",
             "-Ithird_party/mbedtls/include",
             "-Ithird_party/coap/include",
             "-Ithird_party/tinycbor/src"],
    deps = ["//src:config",
            "//src/comm/api",
            "//src/comm/common",
            "//src/logger",
            #"//src/ocf",
            "//src/portability",
            #"//src/sec/doxm",
            "//src/provisioning:utility",
            "//src/util",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509",
            "//third_party/coap",
            "//third_party/tinycbor"],
    # hdrs = ["securevirtualresourcetypes.h"],
    srcs = ["certhelpers.c",
            "certhelpers.h"],
    visibility = ["//src/sec:__pkg__",
                  "//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__"]
)

cc_library(
    name = "resources",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/comm/api",
             "-Isrc/comm/common",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Isrc/portability",
             "-Isrc/sec/security",
             "-Isrc/util",
             "-Ithird_party/mbedtls/include",
             "-Ithird_party/coap/include",
             "-Ithird_party/tinycbor/src"],
    deps = ["//src:config",
            "//src/comm/api",
            "//src/comm/common",
            "//src/logger",
            #"//src/ocf",
            "//src/portability",
            "//src/sec/doxm",
            "//src/sec:helpers",
            "//src/util",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509",
            "//third_party/coap",
            "//third_party/tinycbor"],
    hdrs = ["securevirtualresourcetypes.h"],
    srcs = [#"doxmresource.c",

            "credresource.c",
            "credresource.h",
            #"secureresourcemanager.c",
            "secureresourcemanager.h",

            # if env.get('DTLS_WITH_X509') == '1':  # and env.get('SECURED'):
	    "crlresource.c",    # SECURED=1
	    "crlresource.h",    # SECURED=1
            "crl_logging.h",

            "csrresource.c",
            "csrresource.h",

            # "rolesresource.c",
            # "rolesresource.h",

            # "iotvticalendar.c",
            "iotvticalendar.h",

            "psinterface.h",
            "resourcemanager.h",
            "srmutility.h",
            "pbkdf2.h",
            "pkix_interface.h",
            "srmresourcestrings.h"
    ],
    visibility = ["//src/sec:__pkg__",
                  "//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__"]
)


cc_library(
    name = "sec",
    alwayslink = True,
    copts = ["-Isrc",
             "-Isrc/comm",
             "-Isrc/comm/api",
             "-Isrc/comm/common",
             "-Isrc/comm/interface",
             "-Isrc/comm/include",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Isrc/ocf/internal",
             "-Isrc/portability",
             "-Isrc/provisioning/include",
             "-Isrc/util",
             "-Isrc/sec",
             "-Isrc/sec/security",
             "-Ithird_party/cjson",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             "-Ithird_party/mbedtls/include",
             "-Ithird_party/tinycbor/src"],
    deps = ["//src:config",
            "//src/comm",
            "//src/comm/api",
            "//src/comm/interface",
            "//src/logger",
            "//src/ocf:interface",
            "//src/ocf",
            "//src/portability",
            "//src/provisioning",
            "//src/sec/acl",
            "//src/sec/doxm",
            "//src/sec:interface",
            #"//src/sec:resources",
            "//src/util",
            "//third_party/tinycbor",
            "//third_party/cjson",
            "//third_party/coap",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509"],
    hdrs = ["securevirtualresourcetypes.h"],
    srcs = [#"aclresource.c",
            #"aclresource.h",

	    #"amaclresource.c",
	    #"amaclresource.h",
	    # "amsmgr.c",  # removed in 1.3
	    # "amsmgr.h",

	    # "base64.c", # moved to src/util
	    # "base64.h",

            # "certhelpers.c",    # SECURED=1
            # "certhelpers.h",    # SECURED=1

	    # "credresource.c",
	    # "credresource.h",

            "deviceonboardingstate.c",
            "deviceonboardingstate.h",

	    # DEPRECATED "directpairing.c",
	    # DEPRECATED "directpairing.h",

	    # "doxmresource.c",
	    # "doxmresource.h",

	    # DEPRECATED "dpairingresource.c",
	    # DEPRECATED "dpairingresource.h",

	    "iotvticalendar.c",
	    "iotvticalendar.h",

            "occertutility.c",  # SECURED=1
            "occertutility.h",  # SECURED=1

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

	    # "resourcemanager.c",
	    # "resourcemanager.h",

            # "rolesresource.c",   # SECURED=1
            # "rolesresource.h",   # SECURED=1

            "secureresourcemanager.c",
            "secureresourcemanager.h",

            "security_internals.h",

	    "srmresourcestrings.c",
	    "srmresourcestrings.h",

	    "srmutility.c",
	    "srmutility.h"]
    + select({"windows": ["strptime.c"]}),

    visibility = ["//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__"]
    )
