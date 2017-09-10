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
    name = "strings",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/sec",
             "-Isrc/util"],
    deps = ["//src:config",
            "//src/util"],
    srcs = [# "srmresourcestrings.c",
            "srmresourcestrings.h",
            "securevirtualresourcetypes.h"],
    visibility = ["//src/provisioning:__pkg__",
    "//src/sec:__pkg__"]
)

# cc_library(
#     name = "srm",
#     copts = ["-Isrc",
#              "-Isrc/logger",
#              "-Isrc/ocf",
#              "-Isrc/sec/acl",
#              "-Isrc/util"],
#     deps = ["//src:config",
#             "//src/logger",
#             "//src/ocf:hdrs",
#             "//src/sec/acl",
#             "//src/util"],
#     srcs = ["resourcemanager.c",
#             "resourcemanager.h",
#             "secureresourcemanager.c",
#             "secureresourcemanager.h",
#             "securevirtualresourcetypes.h"]
#     # visibility = ["//src/sec/doxm:__pkg__"]
# )

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

# cc_library(
#     name = "aclx",
#     alwayslink = 1,
#     linkstatic = True,
#     copts = ["-Isrc",
#              "-Isrc/comm",
#              "-Isrc/comm/adapter_util",
#              "-Isrc/comm/api",
#              "-Isrc/comm/common",
#              "-Isrc/comm/include",
#              "-Isrc/logger",
#              "-Isrc/ocf",
#              "-Isrc/ocf/internal",
#              "-Isrc/portability",
#              "-Isrc/sec",
#              "-Isrc/sec/security",
#              "-Isrc/util",
#              "-Ithird_party/cjson",
#              "-Ithird_party/coap/include",
#              "-Ithird_party/mbedtls/include",
#              "-Ithird_party/tinycbor/src"]
#                  + select({"windows": [],
#                            "//conditions:default": ["-std=c11"]}),
#     deps = ["//src:config",
#             "//src/comm",
#             "//src/comm/adapter_util",
#             "//src/comm/api",
#             "//src/logger",
#             "//src/ocf:interface",
#             "//src/portability",
#             #"//src/provisioning",
#             "//src/util",
#             "//third_party/tinycbor",
#             "//third_party/cjson",
#             "//third_party/coap",
#             "//third_party/mbedtls",
#             "//third_party/mbedtls:mbedtls-crypto",
#             "//third_party/mbedtls:mbedtls-x509"],
#     srcs = [#"aclresource.c",
#             # "aclresource.h",
#             # "acl_logging.h",
#             "doxmresource.h",

#             "rolesresource.c",
#             "rolesresource.h",
#             "certhelpers.h",

#             "secureresourcemanager.h",
#             "deviceonboardingstate.h",
#             "security_internals.h",

# 	    "amaclresource.c",
# 	    "amaclresource.h",

#     	    "psinterface.h",
# 	    "resourcemanager.c",
# 	    "resourcemanager.h",
#             "pstatresource.h",
#             "credresource.h",
# 	    "crlresource.h",    # SECURED=1
#             "csrresource.h",

#             "securevirtualresourcetypes.h",
#             "srmresourcestrings.h",
#             "srmutility.h"
#     ],
#     visibility = ["//src/sec:__subpackages__"]
# )

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
            "//src/ocf:hdrs",
            "//src/portability",
            "//src/sec:strings",
            "//src/util",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509",
            "//third_party/coap",
            "//third_party/tinycbor"],
    # hdrs = ["securevirtualresourcetypes.h"],
    srcs = ["certhelpers.c",
            "certhelpers.h",
            "doxmresource.h",
            "srmutility.c",
            "srmutility.h"],
    visibility = ["//src/sec:__pkg__",
                  "//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__"]
)

cc_library(
    name = "resources",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/comm/adapter_util",
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
            "//src/comm/adapter_util",
            "//src/comm/api",
            "//src/comm/common",
            "//src/logger",
            "//src/ocf:hdrs",
            "//src/portability",
            "//src/sec:helpers",
            "//src/sec:strings",
            "//src/util",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509",
            "//third_party/coap",
            "//third_party/tinycbor"],
    hdrs = ["securevirtualresourcetypes.h"],
    srcs = ["aclresource.c",
            "aclresource.h",
            "acl_logging.h",

	    "amaclresource.c",
	    "amaclresource.h",

            "credresource.c",
            "credresource.h",

            "doxmresource.c",
            "doxmresource.h",
            "pinoxmcommon.h",    # included by doxmresource.c
            "oxmverifycommon.h", # included by doxmresource.c

            # if env.get('DTLS_WITH_X509') == '1':  # and env.get('SECURED'):
	    "crlresource.c",    # SECURED=1
	    "crlresource.h",    # SECURED=1
            "crl_logging.h",

            "csrresource.c",
            "csrresource.h",

	    "pstatresource.c",
	    "pstatresource.h",

            "deviceonboardingstate.c",
            "deviceonboardingstate.h",

            "rolesresource.c",
            "rolesresource.h",

            "resourcemanager.c",
            "resourcemanager.h",

            "secureresourcemanager.c",
            "secureresourcemanager.h",
            "policyengine.h",   # included by secureresourcemanager.c
            "policyengine.c",
            "srmresourcestrings.c",
            "srmresourcestrings.h",

            # "iotvticalendar.c",
            "iotvticalendar.h",

            "psinterface.h",
            "srmutility.h",
            "pbkdf2.h",
            "pkix_interface.h",
            "security_internals.h"
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
            #"//src/ocf",
            "//src/portability",
            "//src/provisioning",
            #"//src/sec/acl",
            #"//src/sec/doxm",
            "//src/sec:interface",
            "//src/sec:resources",
            # "//src/sec:strings",
            "//src/util",
            "//third_party/tinycbor",
            "//third_party/cjson",
            "//third_party/coap",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509"],
    hdrs = ["securevirtualresourcetypes.h"],
    srcs = ["iotvticalendar.c",
	    "iotvticalendar.h",

            "occertutility.c",  # SECURED=1
            "occertutility.h",  # SECURED=1

            "ocsecurity.c",
            "ocsecurity.h",

            "oxmpincommon.c",   # SECURED=1

            "oxmverifycommon.c", # SECURED=1
            "oxmverifycommon.h", # SECURED=1

            "pbkdf2.c",         # SECURED=1
            "pbkdf2.h",        # SECURED=1

	    #DEPRECATED "pconfresource.c",  # pairing config
	    #DEPRECATED "pconfresource.h",

            "pinoxmcommon.h",

            "pkix_interface.c", # SECURED=1
            "pkix_interface.h", # SECURED=1

	    # "policyengine.c",
	    # "policyengine.h",

	    "psinterface.c",
	    "psinterface.h",

	    # "pstatresource.c",
	    # "pstatresource.h",

	    # "resourcemanager.c",
	    # "resourcemanager.h",

            # "rolesresource.c",   # SECURED=1
            # "rolesresource.h",   # SECURED=1

            # "secureresourcemanager.c",
            # "secureresourcemanager.h",

            "security_internals.h"],

	    # "srmresourcestrings.c",
	    # "srmresourcestrings.h"]
    # + select({"windows": ["strptime.c"],
    #           "//conditions:default": []}),
    visibility = ["//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__"]
    )
