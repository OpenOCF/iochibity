cc_library(
    name = "common",
    copts = ["-std=c11",
             "-Isrc",
             "-Isrc/comm",
             "-Isrc/comm/api",
             "-Isrc/portability",
             "-Isrc/util"],
    deps  = ["//src:config",
             "//src/portability",
    	     "//src/comm/api",
    	     #"//src/comm/adapters"
    ],
    srcs  = [#"uarraylist.c",
             #"uarraylist.h",
             "ulinklist.c",
             "ulinklist.h",
             "uqueue.c",
             "uqueue.h",
             "caremotehandler.c",
             "caremotehandler.h",
             "cathreadpool_pthreads.c",
             "cathreadpool.h"], # target = posix or windows
    visibility = ["//src/comm:__subpackages__"]
)

