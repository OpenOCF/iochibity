# FIXME: ifaddrs to portability, do not process with makeheaders if platform has getifaddrs

load("//tools/config:variables.bzl", "COPTS_ANDROID")

config_setting(
    name = "android",
    values = {# "define": "target=android",
        # "crosstool_top": "@android_ndk//:toolchain-libcpp",
	#"target_system_name": "arm-linux-androideabi",
	# "crosstool_top": "@androidndk//:toolchain-libcpp",
        "cpu": "armeabi-v7a",
              # "android_cpu": "x86",
              # "android_crosstool_top": "//platforms/ndk:toolchain"
    }
)
config_setting(
    name = "ndk",
    values = { "crosstool_top": "//platforms/ndk:toolchain" }
)
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
    # alwayslink = True,
    copts = [#"-Isrc",
        # "-Iexternal/openocf/src",
        "-Isrc/logger",
        "-Iexternal/openocf/src/logger",
        "-Isrc/portability",
        "-Ithird_party/tinycbor/src",
        "-Iexternal/openocf/third_party/tinycbor/src"]
    + select({"windows": [],
              "msvc": [],
              "msys": [],
              "android": COPTS_ANDROID,
              "//conditions:default": ["-std=c11",
                                       "-UDEBUG"]}),
    deps = ["//src/logger",
            "//third_party/tinycbor",
            # "//third_party/ndk",
            # "//src/portability"
    ],
    linkopts = select({"android": ["-llog"],
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
            "ulinklist.c",
            "ulinklist.h",
            "uqueue.c",
            "uqueue.h"],
    # FIXME: move to portability:
    # + select({":android": ["ifaddrs_android.c",
    #                        "ifaddrs_android.h"],
    #           "//conditions:default": []}),
    visibility = ["//src:__subpackages__",
                  "//tools/makeheaders:__pkg__",
                  "//tools/cbor:__subpackages__"]
)
