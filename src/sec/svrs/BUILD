# svrs = Security Virtual Resources
load("//config:variables.bzl", "DEFINES")

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
    ] + select({"//config:msvc": [],
                "//config:darwin": ["-std=c11", "-UDEBUG", "-DWITH_POSIX"],
                "//config:darwin64": ["-std=c11", "UDEBUG"],
                "//conditions:default": ["-std=c11", "-DWITH_POSIX"]}),

    defines = DEFINES,

    deps = ["//src:config",
            "//src/comm/transport",
            "//src/comm/transport/util",
            "//src/logger",
            "//src/portability",
            "//src/sec/authorization",
            "//src/util",
            "@libcoap//:libcoap",
            "@mbedtls//:mbedtls",
            "@tinycbor//:tinycbor",
            "@uthash//:uthash"],
    linkopts = select({"//config:msys": ["-L:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lcrypt32"],
                       "//config:msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 crypt32.lib"],
                       "//config:mingw": ["-L:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lcrypt32"],
              "//conditions:default": []}),
    srcs = glob(["*.c"], exclude = ["_events.c"])
    + glob(["*.h"]),
    visibility = ["//visibility:public"]
)

