cc_library(
    name = "ip",
    copts = ["-std=c11",
             "-Isrc",
             "-Isrc/util",
    	     "-Isrc/comm/api",
    	     "-Isrc/comm/common",
	     "-Isrc/comm/util",
	     "-Isrc/portability",
	     "-Isrc/protocol/coap"],
    deps = ["//src:config",
            "//src/util",
    	    "//src/comm:shared",
            "//src/comm:util",
            "//src/portability",
	    "//src/protocol/coap"],
    hdrs = ["caipadapter.h"],
    srcs = ["caipserver.c",
	    "caipadapter.c",
	    "caipadapter.h",
	    "caipinterface.h"],
    visibility = ["//src/comm:__pkg__"]
)

