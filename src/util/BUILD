# FIXME: ifaddrs to portability, do not process with makeheaders if platform has getifaddrs


config_setting(
    name = "android",
    values = {"define": "target=android",
              "android_cpu": "x86",
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
    #          "-Iexternal/openocf/src",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/portability",
        "-Ithird_party/tinycbor/src",
        "-Iexternal/openocf/third_party/tinycbor/src"]
    + select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11",
                                       "-UDEBUG"]}),
    deps = ["//src/logger",
            "//third_party/tinycbor",
            # "//src/portability"
    ],
    srcs = glob(["*.c"], exclude=["ifaddrs.*"]) + glob(["*.h"])
    + select({#":ndk": ["ifaddrs.c",
              #         "ifaddrs.h"],
              ":android": ["ifaddrs.c",
                           "ifaddrs.h"
              ],
              "//conditions:default": []}),
    visibility = ["//src:__subpackages__",
                  "//tools/makeheaders:__pkg__",
                  "//tools/cbor:__subpackages__"]
)
