load("//config:variables.bzl", "DEFINES")

config_setting(
    name = "darwin",
    values = { "cpu": "darwin" })
config_setting(
    name = "darwin64",
    values = { "cpu": "darwin_x86_64" })
config_setting(
    name = "windows",
    values = { "cpu": "x64_windows" }
)

cc_library(
    name = "crypto",
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
    ] + select({"windows": [],
                "darwin": ["-std=c11", "-UDEBUG"],
                "darwin64": ["-std=c11", "UDEBUG"],
                "//conditions:default": ["-std=c11"]}),

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



