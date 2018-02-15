config_setting(
    name = "android",
    values = {# "define": "target=android",
        # "crosstool_top": "@android_ndk//:toolchain-libcpp",
        "cpu": "armeabi-v7a",
              # "android_cpu": "x86",
              # "android_crosstool_top": "//platforms/ndk:toolchain"
    }
)

config_setting(
    name = "ndk",
    values = { "crosstool_top": "//platforms/ndk:toolchain" }
)
config_setting(
    name = "linux",
    values = { "cpu": "k8" }
)
config_setting(
    name = "darwin",
    values = { "cpu": "darwin" }
)
config_setting(
    name = "darwin64",
    values = { "cpu": "darwin_x86_64" }
)
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
    name = "logger",
    copts = ["-Isrc",
             "-Iexternal/openocf/src"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    # deps = ["//src:config"],
    hdrs = ["logger.h"],
    linkopts = select({"android": ["-llog"],
                       "//conditions:default": []}),
    # alwayslink = True,
    linkstatic = 1,
    srcs = ["logger.c",
            "logger.h",
            "trace.c",
            "trace.h"]
    + select({":windows":  glob(["logger_windows.*"]),
              ":msvc":   glob(["logger_windows.*"]),
              ":msys":   glob(["logger_windows.*"]),
              ":android": ["logger_android.c", "logger_android.h"],
              # ":darwin": ["logger_posix.c", "logger_posix.h"],
              # ":linux": ["logger_posix.c", "logger_posix.h"],
              "//conditions:default": ["logger_posix.c", "logger_posix.h"]}),
    visibility = ["//src:__subpackages__"]
)
