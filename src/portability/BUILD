config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "hdrs",
    # deps = ["//src:config",
    #         "//src/logger"],
    # copts = ["-Isrc",
    #          "-Isrc/logger",
    #          "-Isrc/portability",
    #          "-Isrc/portability/windows",
    # 	     "-Isrc/util"],
    hdrs = ["ocatomic.h",
            "ocevent.h",
            "octhread.h",
            "oic_malloc.h",
            "iotivity_commontypes.h"]
)

cc_library(
    name = "posix",
    deps = ["//src:config",
            "//src/logger",
            "//src/portability:hdrs"],
    copts = ["-Isrc",
             "-Isrc/logger",
             "-Isrc/portability"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    srcs = ["octhread.h",
            "posix/octhread.c"]
)

cc_library(
    name = "win",
    alwayslink = True,
    linkstatic = 1,
    copts = ["-Isrc",
             "-Isrc/logger",
             "-Isrc/portability",
             "-Ithird_party/mbedtls/include"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
            "//src/portability:hdrs",
            "//third_party/mbedtls"],
    srcs = ["windows/ocatomic.c",
            "windows/ocevent.c",
            "windows/octhread.c",
            "windows/pthread_create.c",
            "windows/pthread_create.h",
            "windows/getopt.c",
            "windows/getopt.h",
            "windows/memmem.c",
            "windows/memmem.h",
            "windows/snprintf.c",
            "windows/vs12_snprintf.h",
            "windows/win_sleep.c",
            "windows/win_sleep.h",

            "ocrandom.h",

            "oic_platform.c",
            "oic_platform.h",
            # FIXME: convert to c11
            "windows/oic_winplatform.cpp",

            "oic_string.h"],
)


cc_library(
    name = "portability",
    alwayslink = True,
#    linkopts = ["/WHOLEARCHIVE:libvcruntime.lib"],
    linkstatic = 1,
    deps = ["//src:config",
            "//src/logger"]
    + select({":windows": ["//src/portability:win",],
              # ":gcc": ["//src/portability:gcc"],
              # ":posix": ["//src/portability:posix"],
    #             "//conditions:default": ["gcc/ocatomic.c",
    # 					 "gcc/ocevent.c",
    # 					 "posix/octhread.c"]}),
              "//conditions:default": []}),
    copts = ["-Isrc",
             "-Isrc/logger",
             "-Isrc/portability",
	     "-Isrc/portability/windows",
    	     "-Isrc/util"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    srcs = ["iotivity_commontypes.h",
    	    "oic_malloc.c",
            "oic_malloc.h",
	    "ocatomic.h",
	    "ocevent.h",
            "ocrandom.c",
            "ocrandom.h",
            # "ocrandom_seed.c",
	    "octimer.c",
	    "octimer.h",
	    "octhread.h",
            "oic_string.c",
            "oic_string.h",
            "oic_time.c",
            "oic_time.h",
            "oic_platform.h"],
    visibility = ["//src:__subpackages__",
                  "//third_party:__subpackages__"]
 )

