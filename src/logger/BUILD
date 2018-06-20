load("//config:variables.bzl", "CSTD", "DEFINES", "DBG_COPTS", "OS_COPTS")

cc_library(
    name = "logger",
    copts = ["-Isrc",
             "-Iexternal/openocf/src"]
    + select({"//config:windows": [],
              "//config:msvc": [],
              "//config:msys": [],
              "//config:darwin": ["-U DEBUG", "-std=c11"],
              "//conditions:default": ["-std=c11"]})
    + select({"//config:debug_threads": ["-DDEBUG_THREADS"],
              "//conditions:default": []})
    + CSTD + DBG_COPTS,

    defines = DEFINES,

    hdrs = ["logger.h"],
    linkopts = select({"//config:android": ["-llog"],
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
              "//config:darwin": ["logger_posix.c", "logger_posix.h"],
              "//config:linux": ["logger_posix.c", "logger_posix.h"],
              "//config:linux-x86_64": ["logger_posix.c", "logger_posix.h"],
              "//config:rpi_arm8": ["logger_posix.c", "logger_posix.h"],
              "//conditions:default": ["BROKEN"]}),
              # "//conditions:default": ["logger_posix.c", "logger_posix.h"]}),
    visibility = ["//src:__subpackages__"]
)
