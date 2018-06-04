# config_setting(
#     name = "android",
#     values = {# "define": "target=android",
#         # "crosstool_top": "@android_ndk//:toolchain-libcpp",
#         "cpu": "armeabi-v7a",
#               # "android_cpu": "x86",
#               # "android_crosstool_top": "//platforms/ndk:toolchain"
#     }
# )

cc_library(
    name = "logger",
    copts = ["-Isrc",
             "-Iexternal/openocf/src"]
    + select({"//config:windows": [],
              "//config:msvc": [],
              "//config:msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    # deps = ["//src:config"],
    hdrs = ["logger.h"],
    linkopts = select({"//config:android": ["-llog"],
                       "//config:android_arm64": ["-llog"],
                       "//config:android_x86": ["-llog"],
                       "//conditions:default": []}),
    # alwayslink = True,
    linkstatic = 1,
    srcs = ["logger.c",
            "logger.h",
            "trace.c",
            "trace.h"
    ]
    + select({"//config:windows":  glob(["logger_windows.*"]),
              "//config:msvc":   glob(["logger_windows.*"]),
              "//config:msys":   glob(["logger_windows.*"]),
              "//config:android": ["logger_android.c", "logger_android.h"],
              "//config:android_x86": ["logger_android.c", "logger_android.h"],
              "//config:android_arm64": ["logger_android.c", "logger_android.h"],
              "//config:darwin": ["logger_posix.c", "logger_posix.h"],
              "//config:linux": ["logger_posix.c", "logger_posix.h"],
              "//config:linux-x86_64": ["logger_posix.c", "logger_posix.h"],
              "//config:rpi": ["logger_posix.c", "logger_posix.h"],
              "//conditions:default": ["BROKEN"]}),
              # "//conditions:default": ["logger_posix.c", "logger_posix.h"]}),
    visibility = ["//src:__subpackages__"]
)
