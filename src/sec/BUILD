config_setting(
    name = "windows",
    values = { "cpu": "x64_windows" }
)
config_setting(
    name = "msvc",
    values = { "cpu": "x64_windows_msvc" }
)
config_setting(
    name = "msys",
    values = { "cpu": "x64_windows_msys" }
)

# cc_library(
#     name = "pbkdf2",
#     srcs = ["pbkdf2.c",
#             "pbkdf2.h"]
# )

cc_library(
    name = "strings",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/sec",
             "-Isrc/util"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = ["//src:config",
            "//src/util"],
    srcs = ["srmresourcestrings.c",
            "srmresourcestrings.h",
            # "securevirtualresourcetypes.h"
    ],
    visibility = ["//src/provisioning:__pkg__",
    "//src/sec/resources:__pkg__"]
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

# cc_library(
#     name = "interface",
#     # copts = ["-Isrc/comm/api"]
#     + select({":windows": [],
#               ":msvc": [],
#               ":msys": [],
#               "//conditions:default": ["-std=c11"]}),
#     # deps = ["//src/comm/api"],
#     #         "//src/util",       # c_common
#     #         "//third_party/cbor",
#     #         "//third_party/cjson",
#     #         "//src/routing",
#     #         "//src/comm"],
#     #         #"//src/sec",
#     #         #"//src/provisioning"],
#     hdrs = ["securevirtualresourcetypes.h"],
#     srcs = ["securevirtualresourcetypes.h"],
#     visibility = ["//src/sec:__pkg__",
#                   "//src/ocf:__pkg__",
#                   "//src/provisioning:__pkg__"]
# )

cc_library(
    name = "helpers",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Ithird_party/mbedtls/include",
             "-Iexternal/openocf/third_party/mbedtls/include",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/tinycbor/src",
             "-Iexternal/openocf/third_party/tinycbor/src"
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = ["//src:config",
            # "//src/comm/api",
            # "//src/comm/common",
            "//src/logger",
            # "//src/ocf:hdrs",
            "//src/portability",
            "//src/sec/certificates",
            "//src/sec:strings",
            "//src/util",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509",
            "//third_party/coap",
            "//third_party/tinycbor"],
    # hdrs = ["securevirtualresourcetypes.h"],
    srcs = [#"certhelpers.c",
            #"certhelpers.h",
            # "doxmresource.c",
            # "doxmresource.h",
            "srmutility.c",
            "srmutility.h"],
    visibility = ["//src/sec/resources:__pkg__",
                  "//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__"]
)

# cc_library(
#     name = "resources",
#     alwayslink = 1,
#     linkstatic = True,
#     copts = ["-Isrc",
#     #          "-Iexternal/openocf/src",
#     #          "-Isrc/comm/transport/util",
#     #          "-Iexternal/openocf/src/comm/transport/util",
#     #          "-Isrc/comm/api",
#     #          "-Iexternal/openocf/src/comm/api",
#     #          "-Isrc/comm/common",
#     #          "-Iexternal/openocf/src/comm/common",
#     #          "-Isrc/comm/interface",
#     #          "-Iexternal/openocf/src/comm/interface",
#     #          "-Isrc/comm/security",
#     #          "-Iexternal/openocf/src/comm/security",
#     #          "-Isrc/logger",
#     #          "-Iexternal/openocf/src/logger",
#     #          "-Isrc/ocf",
#     #          "-Iexternal/openocf/src/ocf",
#     #          "-Isrc/portability",
#     #          "-Iexternal/openocf/src/portability",
#     #          "-Isrc/sec/security",
#     #          "-Iexternal/openocf/src/sec/security",
#              "-Isrc/util",
#              "-Iexternal/openocf/src/util",
#              "-Ithird_party/mbedtls/include",
#              "-Iexternal/openocf/third_party/mbedtls/include",
#              # "-Ithird_party/coap",
#              # "-Iexternal/openocf/third_party/coap",
#              "-Ithird_party/coap/include",
#              # "-Iexternal/openocf/third_party/coap/include",
#              "-Ithird_party/tinycbor/src",
#              "-Iexternal/openocf/third_party/tinycbor/src",
#              "-Ithird_party/utlist",
#     ] + select({":windows": [],
#               ":msvc": [],
#               ":msys": [],
#               "//conditions:default": ["-std=c11",
#                                        "-U DEBUG"]}), # OS X
#     deps = ["//src:config",
#             "//src/comm/transport/util",
#             # "//src/comm/api",
#             "//src/comm/common",
#             # "//src/comm/interface",
#             "//src/comm/security",
#             "//src/logger",
#             # "//src/ocf:hdrs",
#             "//src/portability",
#             "//src/sec:helpers",
#             "//src/sec:strings",
#             "//src/util",
#             "//third_party/mbedtls",
#             "//third_party/mbedtls:mbedtls-crypto",
#             "//third_party/mbedtls:mbedtls-x509",
#             "//third_party/tinycbor",
#             "//third_party/utlist",
#     ],
#     # hdrs = ["securevirtualresourcetypes.h"],
#     srcs = ["aclresource.c",
#             "aclresource.h",
#             # "acl_logging.h",

# 	    "amaclresource.c",
# 	    "amaclresource.h",

#             "credresource.c",
#             "credresource.h",

#             "doxmresource.c",
#             "doxmresource.h",

#             # "pinoxmcommon.h",    # included by doxmresource.c
#             # "oxmverifycommon.h", # included by doxmresource.c

#             # if env.get('DTLS_WITH_X509') == '1':  # and env.get('SECURED'):
# 	    "crlresource.c",    # SECURED=1
# 	    "crlresource.h",    # SECURED=1
#             # "crl_logging.h",

#             "csrresource.c",
#             "csrresource.h",

# 	    "pstatresource.c",
# 	    "pstatresource.h",

#             "deviceonboardingstate.c",
#             "deviceonboardingstate.h",

#             "rolesresource.c",
#             "rolesresource.h",

#             "resourcemanager.c",
#             "resourcemanager.h",

#             "secureresourcemanager.c",
#             "secureresourcemanager.h",
#             "policyengine.h",   # included by secureresourcemanager.c
#             "policyengine.c",
#             # "srmresourcestrings.c",
#             # "srmresourcestrings.h",

#             # "iotvticalendar.c",
#             "iotvticalendar.h",

#             "psinterface.h",
#             "srmutility.h",
#             "pbkdf2.h",
#             "pkix_interface.h",
#             # "security_internals.h"
#     ],
#     visibility = ["//src/sec:__pkg__",
#                   "//src/ocf:__pkg__",
#                   "//src/provisioning:__pkg__"]
# )


cc_library(
    name = "sec",
    # alwayslink = True,
    linkstatic = True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
             "-Isrc/comm/include",
             "-Iexternal/openocf/src/comm/include",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/ocf",
             "-Iexternal/openocf/src/ocf",
             "-Isrc/ocf/internal",
             "-Iexternal/openocf/src/ocf/internal",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
             "-Isrc/provisioning/include",
             "-Iexternal/openocf/src/provisioning/include",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Isrc/sec",
             "-Iexternal/openocf/src/sec",
             "-Ithird_party/cjson",
             "-Iexternal/openocf/third_party/cjson",
             "-Ithird_party/coap",
             "-Iexternal/openocf/third_party/coap",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/mbedtls/include",
             "-Iexternal/openocf/third_party/mbedtls/include",
             "-Ithird_party/tinycbor/src",
             "-Iexternal/openocf/third_party/tinycbor/src",
             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist"
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = ["//src:config",
            "//src/comm",
            # "//src/comm/api",
            # "//src/comm/interface",
            "//src/logger",
            # "//src/ocf:interface",
            #"//src/ocf",
            "//src/portability",
            "//src/provisioning",
            "//src/sec/certificates",
            "//src/sec/crypto",
            "//src/sec/dox",
            # "//src/sec:interface",
            "//src/sec/resources",
            # "//src/sec:strings",
            "//src/util",
            "//third_party/utlist",
            "//third_party/tinycbor",
            "//third_party/cjson",
            "//third_party/coap",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509"],
    # hdrs = ["securevirtualresourcetypes.h"],
    srcs = [#"occertutility.c",  # SECURED=1
            #"occertutility.h",  # SECURED=1

            "ocsecurity.c",
            "ocsecurity.h",

            # "pbkdf2.c",         # SECURED=1
            # "pbkdf2.h",        # SECURED=1

	    #DEPRECATED "pconfresource.c",  # pairing config
	    #DEPRECATED "pconfresource.h",

            # "pinoxmcommon.h",

            # "pkix_interface.c", # SECURED=1
            # "pkix_interface.h", # SECURED=1

	    # "policyengine.c",
	    # "policyengine.h",

	    "psinterface.c",
	    "psinterface.h",

            # "security_internals.h"
    ],

	    # "srmresourcestrings.c",
	    # "srmresourcestrings.h"]
    # + select({"windows": ["strptime.c"],
    #           "//conditions:default": []}),
    visibility = ["//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__"]
    )
