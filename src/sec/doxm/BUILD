config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "doxm",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             # "-Isrc/comm",
             # "-Isrc/comm/api",
             # "-Isrc/comm/common",
             # "-Isrc/comm/include",
             # "-Isrc/logger",
             "-Isrc/ocf",
             # "-Isrc/ocf/internal",
             # "-Isrc/portability",
             # #"-Isrc/provisioning",
             # "-Isrc/sec/security",
             # "-Isrc/util",
             # "-Ithird_party/cjson",
             # "-Ithird_party/coap/include",
             # "-Ithird_party/mbedtls/include",
             # "-Ithird_party/tinycbor/src"
    ] + select({"windows": [],
                "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            # "//src/comm",
            # "//src/comm/api",
            # "//src/logger",
            # "//src/ocf:interface",
            # "//src/portability",
            #"//src/sec/acl",
            # "//src/util",
            # "//third_party/tinycbor",
            # "//third_party/cjson",
            # "//third_party/coap",
            # "//third_party/mbedtls",
            # "//third_party/mbedtls:mbedtls-crypto",
            # "//third_party/mbedtls:mbedtls-x509"
    ],
    srcs = ["doxmresource.c",
            "doxmresource.h",
            # "security/doxmresource.h",
            # "resourcemanager.h",
            # "pstatresource.h",
            # "deviceonboardingstate.h",
            # #"aclresource.h",
            # "amaclresource.h",
            # "pconfresource.h",
            # "psinterface.h",
            # "srmresourcestrings.h",
            # "credresource.h",
            # "rolesresource.h",
            # "srmutility.h",
            # "pinoxmcommon.h",
            # "oxmverifycommon.h",
            # "pkix_interface.h"
    ],
    visibility = ["//src/provisioning:__pkg__",
                  "//src/sec:__pkg__",
                  "//src/ocf:__pkg__"]
)



