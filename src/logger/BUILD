config_setting(
    name = "ndk",
    values = { "crosstool_top": "//platforms/ndk:toolchain" }
)
config_setting(
    name = "windows",
# FIXME: make sure this is mingw64
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
    name = "logger",
    copts = ["-Isrc",
             "-Iexternal/openocf/src"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    # deps = ["//src:config"],
    hdrs = ["logger.h"],
    linkopts = select({":ndk": ["-llog"],
                       # FIXME: select android_cc
              "//conditions:default": []}),
    # alwayslink = True,
    linkstatic = 1,
    srcs = ["logger.c",
	    "logger.h",
            "trace.c",
            "trace.h"
    ] + select({":windows": ["logger_windows.c", "logger_windows.h"],
                ":msvc": ["logger_windows.c", "logger_windows.h"],
                ":msys": ["logger_windows.c", "logger_windows.h"],
                "//conditions:default": []}),
    visibility = ["//src:__subpackages__"]
)
