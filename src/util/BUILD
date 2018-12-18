# FIXME: ifaddrs to portability, do not process with makeheaders if platform has getifaddrs

load("//config:variables.bzl", "COPTS_ANDROID")

cc_library(
    name = "util",
    alwayslink = True,
    copts = ["-Isrc/portability",
             "-Iexternal/tinycbor/src"]
    + select({"//config:android_arm7": COPTS_ANDROID,
              "//config:android_arm8": COPTS_ANDROID,
              "//config:android_x86": COPTS_ANDROID,
              "//config:darwin": ["-U DEBUG"],
              "//config:ios": ["-U DEBUG"],
              "//config:mingw": [],
              "//config:msvc": [],
              "//config:msys": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src/logger",
            "@uthash//:uthash",
            "@tinycbor//:tinycbor"],
    linkopts = select({"//config:android_arm7": ["-llog"],
                       "//config:android_arm8": ["-llog"],
                       "//config:android_x86": ["-llog"],
                       "//conditions:default": []}),
    srcs = ["_byte_array.h",
            "_errcheck.h",
            "_strutils.h",
            "_tree.h",
            "_uuid.h",
            "base64.c",
            "base64.h",
            "configuration.c",
            "configuration.h",
            "uarraylist.c",
            "uarraylist.h",
            "ulinklist.c",  # FIXME: used only by routingmanager.c, obsolete
            "ulinklist.h",
            "uqueue.c",
            "uqueue.h"],
    visibility = ["//visibility:public"]
    # visibility = ["//src:__subpackages__",
    #               "//tools/makeheaders:__pkg__",
    #               "//tools/cbor:__subpackages__"]
)

