load("//config:variables.bzl", "DEFINES", "OS_COPTS")

cc_library(
    name = "certificates",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/logger",
             "-Isrc/comm/api",
             "-Isrc/ocf",
             "-Isrc/portability",
             "-Isrc/sec",
             "-Isrc/util",
             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist",
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
            "//third_party/utlist",
            "@libcoap//:libcoap",
            "@tinycbor//:tinycbor"],
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//visibility:public"],
)



