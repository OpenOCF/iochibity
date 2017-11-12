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
    name = "hdrs",
    hdrs = ["base64.h",
            "byte_array.h",
            "configuration.h",
            "utlist.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "util",
    # alwayslink = True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
             "-Isrc/util",
             "-Iexternal/openocf/src/util"]
    + select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
            "//src/portability"],
    hdrs = ["base64.h",
            "byte_array.h",
            "configuration.h",
            "ifaddrs.h",
            "utlist.h"],
    srcs = ["base64.c",
            # "trace.c",
	    # "trace.h",
            "uarraylist.c",
            "uarraylist.h",
            "configuration.c"]
    + select({#":ndk": ["ifaddrs.c",
              #         "ifaddrs.h"],
              ":android": ["ifaddrs.c",
                           # "ifaddrs.h"
              ],
              "//conditions:default": []}),
    visibility = ["//src:__subpackages__"]
    # "//src/comm:__pkg__",
#		  "//src]
)
