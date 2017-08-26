cc_library(
    name = "api",
    copts = ["-std=c11",
             "-Isrc/comm/common"],
    deps  = ["//src:config",
             "//src/portability"],
    hdrs  = ["cacommon.h",
             "cainterface.h",
             "casecurityinterface.h"],
    srcs  = ["cacommon.h",
             "cainterface.h",
             "casecurityinterface.h"],
    visibility = ["//src/comm:__subpackages__",
                  "//src/ocf:__pkg__",
                  "//src/ocf/stack:__pkg__",
                  "//src/sec:__subpackages__",
                  "//src/provisioning:__pkg__"]
)

