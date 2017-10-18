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
    name = "api",
    copts = ["-Isrc/comm/common",
             "-Iexternal/openocf/src/comm/common"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11"]}),
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

