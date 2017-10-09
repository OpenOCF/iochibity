config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "common",
    alwayslink = True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
             "-Isrc/comm/api",
             "-Iexternal/openocf/src/comm/api",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
             "-Isrc/util",
             "-Iexternal/openocf/src/util"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps  = ["//src:config",
             "//src/logger",
             "//src/portability",
    	     "//src/comm/api",
             "//src/util"
    	     #"//src/comm/adapters"
    ],
    hdrs = ["cacommonutil.h"],
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
    visibility = ["//src/comm:__subpackages__",
                  "//src/ocf:__pkg__",
                  "//src/sec:__pkg__",
                  "//src/sec:__subpackages__"]
)

