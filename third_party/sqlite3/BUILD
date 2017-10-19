licenses(["unencumbered"])

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
    name = "sqlite3",
    copts = select({"windows": [],
                    "msvc": [],
                    "msys": [],
                    "//conditions:default": ["-std=c11"]}),
    hdrs = ["sqlite3.h"],
    srcs = ["sqlite3.c"],
    visibility = ["//src/provisioning:__pkg__"]
)
