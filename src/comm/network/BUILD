cc_library(
    name = "ip",
    copts = ["-std=c11",
             "-Isrc",
             "-Isrc/util",
    	     "-Isrc/comm",
    	     "-Isrc/comm/adapter_util",
    	     "-Isrc/comm/api",
    	     "-Isrc/comm/common",
    	     "-Isrc/comm/interface",
	     "-Isrc/comm/util",
             "-Isrc/logger",
	     "-Isrc/portability",
	     "-Ithird_party/coap/include"],
    deps = ["//src:config",
            "//src/logger",
            "//src/util",
    	    "//src/comm/adapter_util",
    	    "//src/comm:ifcontrol",
            # "//src/comm:util",
            "//src/portability",
	    "//third_party/coap"],
    # hdrs = ["caipadapter.h"],
    srcs = ["ip_adapter/caipadapter.c",
            "ip_adapter/caipserver.c",
            #GAR FIXME: select by platform
            "ip_adapter/linux/caipnwmonitor.c"
    ],
    visibility = ["//src/comm:__pkg__"]
)

