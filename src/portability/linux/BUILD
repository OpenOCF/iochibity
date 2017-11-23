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
    name = "linux",
    copts = ["-I.",
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
	     "-Isrc/comm/transport/udpip",
             "-Iexternal/openocf/src/comm/transport/udpip",
    	     "-Isrc/comm/security",
             "-Iexternal/openocf/src/comm/security",
	     "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/util",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
	     "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
	     "-Ithird_party/coap/include",
	     "-Iexternal/openocf/third_party/coap/include",
             "-Iexternal/local_jdk/include",
             "-Iexternal/local_jdk/include/darwin"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            # "//src/logger",
            "//src/util",
    	    # "//src/comm/api",
    	    # "//src/comm/common",
    	    # "//src/comm:hdrs",
    	    # "//src/comm/interface",
    	    # "//src/comm/transport/udpip:hdrs",
    	    # "//src/comm/transport/util",
            # "//src/comm/security",
            # "//src/portability",
	    # "//third_party/coap"],
            ],
    srcs = [#"monitor_impl.c",
	    "caipnwmonitor_linux.c",
	    "caipnwmonitor_linux.h",
            "caipserver_linux.c",
            "caipserver_linux.h"
    ],
    visibility = ["//visibility:public"]
)
