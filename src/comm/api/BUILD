cc_library(
    name = "api",
    copts = ["-std=c11"],
    deps  = ["//src:config",
             "//src/portability"
    	     #"//src/comm:util",
    	     #"//src/comm:cacommon"
    ],
    hdrs  = ["cacommon.h",
             "cainterface.h",
             "casecurityinterface.h",
             "cautilinterface.h"],
    visibility = ["//src/comm:__subpackages__"]
)

