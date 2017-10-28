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
    name = "udpip",
    copts = ["-Isrc",
             "-Iexternal/openocf/src/",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
    	     "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
    	     "-Isrc/comm/transport/util",
             "-Iexternal/openocf/src/comm/transport/util",
    	     "-Isrc/comm/api",
             "-Iexternal/openocf/src/comm/api",
    	     "-Isrc/comm/common",
             "-Iexternal/openocf/src/comm/common",
    	     "-Isrc/comm/interface",
             "-Iexternal/openocf/src/comm/interface",
    	     "-Isrc/comm/security",
             "-Iexternal/openocf/src/comm/security",
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
    	    "//src/comm/transport/util",
    	    "//src/comm/api",
    	    "//src/comm/common",
    	    "//src/comm:hdrs",
    	    "//src/comm/interface",
            "//src/comm/security",
            "//src/portability",
	    "//third_party/coap"],
    hdrs = ["caipadapter.h",
            "caipinterface.h",
            "caipnwmonitor.h"],
    srcs = ["caipadapter.c",
            "caipserver.c"]
    + select({":windows": ["windows/caipnwmonitor.c"],
              ":msvc": ["windows/caipnwmonitor.c"],
              ":msys": ["linux/caipnwmonitor.c"],
              "//conditions:default": ["linux/caipnwmonitor.c"]}),
    visibility = ["//src/comm:__subpackages__"]
)
