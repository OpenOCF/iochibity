load("//config:variables.bzl", "DEFINES", "OS_COPTS")

cc_library(
    name = "authorization",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/comm/api",
             "-Isrc/ocf",
             "-Isrc/portability",
             "-Isrc/sec",
             "-Isrc/util",
             "-Iconfig/darwin",
             "-Iconfig/darwin/coap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
             "-Iexternal/tinycbor/src",
    ] + OS_COPTS,

    defines = DEFINES,

    deps = ["//src:config",
            "//src/logger",
            "//src/portability",
            "//src/sec/calendar",
            "//src/util",
            "@uthash//:uthash",
            "@libcoap//:libcoap",
            "@tinycbor//:tinycbor"],
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//visibility:public"],
)



