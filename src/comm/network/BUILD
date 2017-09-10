config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "ip",
    copts = ["-Isrc",
             "-Isrc/util",
    	     "-Isrc/comm",
    	     "-Isrc/comm/adapter_util",
    	     "-Isrc/comm/api",
    	     "-Isrc/comm/common",
    	     "-Isrc/comm/interface",
	     "-Isrc/comm/util",
             "-Isrc/logger",
	     "-Isrc/portability",
	     "-Ithird_party/coap/include"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
            "//src/util",
    	    "//src/comm/adapter_util",
    	    "//src/comm:hdrs",
    	    # "//src/comm:ifcontrol",
            # "//src/comm:util",
            "//src/portability",
	    "//third_party/coap"],
    # hdrs = ["caipadapter.h"],
    srcs = ["ip_adapter/caipadapter.c",
            "ip_adapter/caipserver.c"]
    + select({"windows": ["ip_adapter/windows/caipnwmonitor.c"],
              "//conditions:default": ["ip_adapter/linux/caipnwmonitor.c"]}),
    visibility = ["//src/comm:__pkg__"]
)
