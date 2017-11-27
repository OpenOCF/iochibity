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

# cc_library(
#     name = "hdrs",
#     hdrs = ["base64.h",
#             # "byte_array.h",
#             "configuration.h"],
#     visibility = ["//visibility:public"],
# )

cc_library(
    name = "util",
    # alwayslink = True,
    copts = [#"-Isrc",
    #          "-Iexternal/openocf/src",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/portability",
    #          "-Iexternal/openocf/src/portability",
    #          "-Isrc/util",
    #          "-Iexternal/openocf/src/util"]
        "-Ithird_party/tinycbor/src",
        "-Iexternal/openocf/third_party/tinycbor/src"]
    + select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11",
                                       "-UDEBUG"]}),
    deps = ["//third_party/tinycbor",
            "//src/logger",
            "//src/portability"
    ],
    # hdrs = ["base64.h",
    #         # "byte_array.h",
    #         "configuration.h",
    #         # select ifaddrs.h only for platforms that do not have getifaddrs (e.g. Android)
    #         # "ifaddrs.h",
    #         # "tree.h",
    # ],
    srcs = ["base64.c",
            "base64.h",
            # "byte_array.c",
            # "errcheck.c",
            "uarraylist.c",
            "uarraylist.h",
            "ulinklist.c",
            "ulinklist.h",
            "uqueue.c",
            "uqueue.h",
            "configuration.c",
            "configuration.h"]
    + select({#":ndk": ["ifaddrs.c",
              #         "ifaddrs.h"],
              ":android": ["ifaddrs.c",
                           "ifaddrs.h"
              ],
              "//conditions:default": []}),
    visibility = ["//src:__subpackages__"]
    # "//src/comm:__pkg__",
#		  "//src]
)
