licenses(["notice"])

load("//tools/config:variables.bzl", "COPTS_ANDROID")

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

config_setting(
    name = "android",
    values = {# "define": "target=android",
        # "crosstool_top": "@android_ndk//:toolchain-libcpp",
        "cpu": "armeabi-v7a",
              # "android_cpu": "x86",
              # "android_crosstool_top": "//platforms/ndk:toolchain"
    }
)

cc_library(
    name = "cjson",
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
             # if windows:
             "-DCJSON_HIDE_SYMBOLS"]
    + select({"windows": ["-DCJSON_HIDE_SYMBOLS"],
              "msvc": [],
              "msys": [],
              "android": COPTS_ANDROID,
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/portability"],
    hdrs = ["cJSON.h"],
    # -lm needed for android; linux? darwin? but not msvc
    linkopts = select({"android": ["-lm"],
              "//conditions:default": []}),
    srcs = ["cJSON.c"],
    visibility = ["//visibility:public"]
)
