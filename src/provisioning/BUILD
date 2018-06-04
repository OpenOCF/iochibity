load("//config:variables.bzl", "CSTD", "DEFINES", "OS_COPTS")

cc_library(
    name = "provisioning",
    copts = ["-Isrc",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Iexternal/tinycbor/src",

             "-Iconfig/darwin",
             "-Iconfig/darwin/coap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",

             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist",
             "-Iexternal/sqlite3"
    ] + CSTD + OS_COPTS,

    defines = DEFINES,

    linkstatic = 1,
    deps = ["//src:config",
            "//src/comm",
            "//src/logger",
            "//src/portability",
            "//src/sec/svrs",
            "//src/util",
            "//include",
            "@libcoap//:libcoap",
            "@mbedtls//:mbedtls",
            "@tinycbor//:tinycbor",
	    "@sqlite3//:sqlite3",
	    "//third_party/utlist"
    ],
    #         # if MULTIPLE_OWNER
    #         #"//src/provisioning/multiowner"
    #         # if WITH_TCP
    #         # "//src/provitioning/cloud"
    # + select({"windows": ["//third_party/sqlite3"],
    #           "//conditions:default": []}),
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//src/ocf:__pkg__",
                  "//src/sec/acl:__pkg__",
                  "//src/sec:__pkg__",
                  "//tools/cbor:__subpackages__"]
    )
