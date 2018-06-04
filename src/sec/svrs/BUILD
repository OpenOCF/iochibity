# svrs = Security Virtual Resources
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
config_setting(
    name = "msvc",
    values = { "cpu": "x64_windows_msvc" }
)
config_setting(
    name = "msys",
    values = { "cpu": "x64_windows_msys" }
)

cc_library(
    name = "pbkdf2",
    srcs = glob(["pbkdf2.*"])
)

cc_library(
    name = "svrs",
    # alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Iconfig/darwin",
             "-Iconfig/darwin",
             "-Iconfig/darwin/coap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
             "-Iconfig",
             "-Iconfig/mbedtls",
             "-Iexternal/mbedtls/patched/include",
             "-Iexternal/tinycbor/src",
             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist",
    ] + select({":windows": [],
                ":msvc": [],
                ":msys": [],
                "darwin": ["-std=c11", "-UDEBUG"],
                "darwin64": ["-std=c11", "UDEBUG"],
                "//conditions:default": ["-std=c11"]}),

    defines = DEFINES,

    deps = ["//src:config",
            "//src/comm/transport/util",
            "//src/logger",
            "//src/portability",
            "//src/sec/authorization",
            "//src/util",
            "@libcoap//:libcoap",
            "@mbedtls//:mbedtls",
            "@tinycbor//:tinycbor",
            "//third_party/utlist",
    ],
    linkopts = select({":msys": ["-L:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lcrypt32"],
                       ":msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 crypt32.lib"],
                       ":windows": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 crypt32.lib"],
              "//conditions:default": []}),
    srcs = glob(["*.c"], exclude = ["_events.c"])
    + glob(["*.h"]),
    visibility = ["//visibility:public"]
)

