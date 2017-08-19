cc_library(
    name = "util",
    copts = ["-Isrc",
             "-Isrc/portability"],
    deps = ["//src:config",
            "//src/portability"],
    hdrs = ["logger.h"],
    srcs = ["logger.c",
    	    "logger.h",
	    "logger_types.h",
	    "trace.c",
	    "trace.h",
            "uarraylist.c",
            "uarraylist.h"],
    visibility = ["//src:__subpackages__"]
    # "//src/comm:__pkg__",
#		  "//src]
)
