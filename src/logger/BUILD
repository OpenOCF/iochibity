cc_library(
    name = "logger",
    copts = ["-Isrc",
             "-Iexternal/openocf/src"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config"],
    hdrs = ["logger.h"],
    srcs = ["logger.c",
	    "logger_types.h",
            "trace.c",
            "trace.h"],
    visibility = ["//src:__subpackages__"]
)

config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)
