load("//config:variables.bzl", "CSTD", "DEFINES", "OS_COPTS")

cc_library(
    name = "provisioning",
    copts = ["-Isrc",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Iexternal/tinycbor/src",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
             "-Iexternal/sqlite3"
    ] + CSTD + OS_COPTS
    + select({"//config:multi_own": ["-DMULTIPLE_OWNER"],
              "//conditions:default": []}),

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
	    "@uthash//:uthash"
    ]+ select({"//config:multi_own": ["//src/provisioning/multiown"],
              "//conditions:default": []}),
    #         # if WITH_TCP
    #         # "//src/provitioning/cloud"
    # + select({"windows": ["//third_party/sqlite3"],
    #           "//conditions:default": []}),
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//visibility:public"]
    # visibility = ["//src/ocf:__pkg__",
    #               "//src/sec/acl:__pkg__",
    #               "//src/sec:__pkg__",
    #               "//tools/cbor:__subpackages__"]
    )
