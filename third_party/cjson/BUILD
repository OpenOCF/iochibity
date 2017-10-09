licenses(["notice"])

config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "cjson",
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/portability"],
    hdrs = ["cJSON.h"],
    srcs = ["cJSON.c"],
    visibility = ["//src/ocf:__pkg__",
                  "//src/sec:__pkg__"]
)
