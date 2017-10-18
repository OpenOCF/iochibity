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
    name = "ip",
    copts = ["-Isrc",
             "-Iexternal/openocf/src/",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
    	     "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
    	     "-Isrc/comm/adapter_util",
             "-Iexternal/openocf/src/comm/adapter_util",
    	     "-Isrc/comm/api",
             "-Iexternal/openocf/src/comm/api",
    	     "-Isrc/comm/common",
             "-Iexternal/openocf/src/comm/common",
    	     "-Isrc/comm/interface",
             "-Iexternal/openocf/src/comm/interface",
	     "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/api",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
	     "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
	     "-Ithird_party/coap/include",
	     "-Iexternal/openocf/third_party/coap/include"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
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
    + select({":windows": ["ip_adapter/windows/caipnwmonitor.c"],
              ":msvc": ["ip_adapter/windows/caipnwmonitor.c"],
              ":msys": ["ip_adapter/linux/caipnwmonitor.c"],
              "//conditions:default": ["ip_adapter/linux/caipnwmonitor.c"]}),
    visibility = ["//src/comm:__pkg__"]
)
