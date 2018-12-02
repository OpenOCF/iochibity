load("//config:variables.bzl", "CSTD", "DEFINES", "OS_COPTS")

cc_library(
    name = "multiown",
    copts = ["-Isrc",
             "-Iexternal/tinycbor/src",
             "-Iexternal/libcoap/include",
             "-Iexternal/utlist/src",
    ] + CSTD + OS_COPTS,

    # defines = DEFINES,

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
	    "@utlist//:utlist"
    ],
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//visibility:public"]
    # visibility = ["//src/ocf:__pkg__",
    #               "//src/sec/acl:__pkg__",
    #               "//src/sec:__pkg__",
    #               "//tools/cbor:__subpackages__"]
    )
