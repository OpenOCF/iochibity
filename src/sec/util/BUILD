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
    name = "util",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Iconfig/darwin",
             "-Iconfig/darwin/coap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = ["//src:config",
            "//src/logger",
            "//src/portability",
            "@libcoap//:libcoap"],
    srcs = glob(["srmutility.c",
                 "srmutility.h"]),
    visibility = ["//src/sec/svrs:__pkg__",
                  "//src/sec/dox:__pkg__",
                  "//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__"]
)
