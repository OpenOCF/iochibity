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
    name = "logger",
    copts = ["-Isrc",
             "-Iexternal/openocf/src"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config"],
    hdrs = ["logger.h"],
    srcs = ["logger.c",
	    "logger_types.h",
            "trace.c",
            "trace.h"],
    visibility = ["//src:__subpackages__"]
)
