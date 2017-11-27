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
    name = "win",
    # alwayslink = True,
    # linkstatic = 1,
    # copts = ["-Isrc",
    #          "-Isrc/logger",
    #          "-Isrc/portability",
    #          "-Ithird_party/mbedtls/include"]
    copts = select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = [#"//src:config",
            "//src/logger",
            # "//src/portability:hdrs",
            "//third_party/mbedtls"],
    # linkopts = ["-W,crypt32.lib"],
    srcs = ["ocatomic_windows.c",
            "ocatomic_windows.h",
            "ocevent_windows.c",
            "ocevent_windows.h",
            "octhread_windows.c",
            "octhread_windows.h",
            "getopt.c",
            "getopt.h",
            "memmem.c",
            "memmem.h",
            "snprintf.c",
            "snprintf.h",
            "strptime.c",
            "strptime.h",
            # "vs12_snprintf.h",
            "win_sleep.c",
            "win_sleep.h",

            # "ocrandom.h",

            # "oic_platform.c",
            # "oic_platform.h",
            # FIXME: convert to c11
            # "oic_winplatform.cpp",

            # "oic_string.h"
    ] + select({"msvc": ["pthread_create.c",
                         "pthread_create.h"],
                "windows": ["pthread_create.c",
                            "pthread_create.h"],
                "msys": [],
              "//conditions:default": []}),
    visibility = ["//src/portability:__pkg__"]
)
