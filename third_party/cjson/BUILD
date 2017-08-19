licenses(["notice"])

cc_library(
    name = "cjson",
    copts = ["-Isrc/portability"],
    deps = ["//src/portability"],
    hdrs = ["cJSON.h"],
    srcs = ["cJSON.c"],
    visibility = ["//src/ocf:__pkg__",
                  "//src/sec:__pkg__"]
)
