licenses(["unencumbered"])

config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "sqlite3",
    copts = select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    hdrs = ["sqlite3.h"],
    srcs = ["sqlite3.c"],
    visibility = ["//src/provisioning:__pkg__"]
)
