config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "util",
    alwayslink = True,
    copts = ["-Isrc",
             "-Isrc/logger",
             "-Isrc/portability"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
            "//src/portability"],
    hdrs = ["base64.h",
            "byte_array.h",
            "utlist.h"],
    srcs = ["base64.c",
            "trace.c",
	    "trace.h",
            "uarraylist.c",
            "uarraylist.h"],
    visibility = ["//src:__subpackages__"]
    # "//src/comm:__pkg__",
#		  "//src]
)
