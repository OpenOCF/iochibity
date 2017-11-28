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

cc_library(
    name = "provisioning",
    copts = ["-Isrc",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/tinycbor/src",
             "-Iexternal/openocf/third_party/tinycbor/src",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap",
             "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/utlist",
             "-Ithird_party/sqlite3"
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    linkstatic = 1,
    deps = ["//src:config",
            "//src/comm",
            # "//src/comm/api",
            # "//src/comm/interface",
            "//src/logger",
            # "//src/ocf:interface",
            #  "//src/ocf",
            "//src/portability",
            "//src/sec:strings",
            # "//src/sec:interface",
            "//src/sec/svrs",
            "//src/util",
            "//include",
            "//third_party/coap",
            "//third_party/mbedtls",
            "//third_party/tinycbor",
	    "//third_party/sqlite3",
	    "//third_party/utlist"
    ],
    # + select({"windows": ["//third_party/sqlite3"],
    #           "//conditions:default": []}),
    # hdrs = ["include/pmtypes.h",
    #         "include/pmutility.h"],
    srcs = ["credentialgenerator.c",
            "credentialgenerator.h",
            "ocprovisioningmanager.c",
            "ocprovisioningmanager.h",
            "otmcontextlist.c",
            "otmcontextlist.h",
            "ownershiptransfermanager.c",
            "ownershiptransfermanager.h",
            "oxmjustworks.c",
            "oxmjustworks.h",

            "oxmmanufacturercert.c",
            "oxmmanufacturercert.h",

            "oxmrandompin.c",
            "oxmrandompin.h",
            "pmutility.c",
            "pmutility.h",
            # "pmutilityinternal.h",
            "provisioningdatabasemanager.c",
            "provisioningdatabasemanager.h",
            "secureresourceprovider.c",
            "secureresourceprovider.h",

            # if MULTIPLE_OWNER
            #"multipleownershiptransfermanager.c",
            #"multipleownershiptransfermanager.h",
            #"oxmpreconfpin.c"
            #"oxmpreconfpin.h"

            # if WITH_TCP
            # "cloud/utils.c",
            # "cloud/crl.c",
            # "cloud/aclgroup.c",
            # "cloud/aclinvite.c",
            # "cloud/aclid.c",
            # "cloud/csr.c"
    ],
    visibility = ["//src/ocf:__pkg__",
                  "//src/sec/acl:__pkg__",
                  "//src/sec:__pkg__"]
    )
