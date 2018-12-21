load("//config:variables.bzl", "CSTD", "DEFINES", "OS_COPTS")

# select({"//config:enable_logging": ["logger.c",
#                                                "payload_logging.c",
#                                                "payload_logging.h",
#                                                "trace.c",
#                                                "trace.h"]

cc_library(
    name = "logger",
    copts =  select({"//config:msvc": [],
                     "//config:mingw": ["-DNOGDI", "-DNOSERVICE"], # avoid warnings about redefined macros
                     "//config:darwin": ["-U DEBUG", "-std=c11"],
                     "//conditions:default": ["-std=c11"]})
    + select({"//config:debug_threads": ["-DDEBUG_THREADS"],
              "//conditions:default": []})
    + CSTD,

    defines = DEFINES,

    linkopts = select({"//config:android": ["-llog"],
                       "//conditions:default": []}),
    alwayslink = True,
    linkstatic = 1,
    deps = select({"//config:enable_logging": [":logger_impl"],
                   "//conditions:default": []}),
    srcs = select({"//config:enable_logging": [],
                   "//conditions:default": ["nologger.c", "nologger.h"]}),
    visibility = ["//visibility:public"]
)

cc_library(
    name = "logger_impl",
    copts = ["-Iexternal/uthash/src",
             "-Iexternal/tinycbor/src",
             "-Iexternal/mbedtls"
    ]
    + select({"//config:msvc": [],
                     "//config:darwin": ["-U DEBUG", "-std=c11"],
                     "//conditions:default": ["-std=c11"]})
    + select({"//config:debug_threads": ["-DDEBUG_THREADS"],
              "//conditions:default": []})
    + CSTD,

    defines = DEFINES,

    linkopts = select({"//config:android": ["-llog"],
                       "//conditions:default": []}),
    alwayslink = True,
    linkstatic = 1,
    deps = ["@mbedtls//:mbedtls",
            "@tinycbor//:tinycbor",
            "@uthash//:uthash"],
    hdrs = ["logger.h"],
    srcs = ["logger.c",
            "log_svrs.c",
            "log_svrs.h",
            "payload_logging.c",
            "payload_logging.h",
            "trace.c",
            "trace.h"]
    # FIXME: trace.c for android, tizen
    + select({"//config:android": ["logger_android.c", "logger_android.h"],
              "//config:darwin": ["logger_posix.c", "logger_posix.h"],
              "//config:ios_arm7": ["logger_posix.c", "logger_posix.h"],
              "//config:ios_arm64": ["logger_posix.c", "logger_posix.h"],
              "//config:ios_x86": ["logger_posix.c", "logger_posix.h"],
              "//config:linux": ["logger_posix.c", "logger_posix.h"],
              "//config:linux64": ["logger_posix.c", "logger_posix.h"],
              "//config:mingw":  glob(["logger_windows.*"]),
              "//config:msvc":   glob(["logger_windows.*"]),
              "//config:msys":   glob(["logger_windows.*"]),
              "//config:rpi_arm8": ["logger_posix.c", "logger_posix.h"],
              #"//conditions:default": ["BROKEN"]}),
              "//conditions:default": []}),

     visibility = ["//visibility:public"]
)
