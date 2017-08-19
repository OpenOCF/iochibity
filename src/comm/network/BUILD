cc_library(
    name = "ip",
    copts = ["-std=c11",
             "-Isrc",
             "-Isrc/util",
    	     "-Isrc/comm/api",
    	     "-Isrc/comm/common",
	     "-Isrc/comm/util",
	     "-Isrc/portability",
	     "-Ithird_party/coap"],
    deps = ["//src:config",
            "//src/util",
    	    "//src/comm:shared",
            "//src/comm:util",
            "//src/portability",
	    "//third_party/coap"],
    # hdrs = ["caipadapter.h"],
    srcs = ["ip_adapter/caipadapter.c",
            "ip_adapter/caipserver.c",
	    #"caipadapter.h",
	    #"caipinterface.h"
    ],
    visibility = ["//src/comm:__pkg__"]
)

