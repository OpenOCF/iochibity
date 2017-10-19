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
    name = "hdrs",
    hdrs = ["ocatomic.h",
            "ocevent.h",
            "ocrandom.h",
            "octhread.h",
            "oic_malloc.h",
            "oic_platform.h",
            "oic_string.h",
            "oic_time.h",
            "iotivity_commontypes.h"]
)

# if platform != windows and defined(HAVE_LIBPTHREAD)
# NB: for now we ignore platforms lacking pthreads
cc_library(
    name = "posix",
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/portability"]
             # "-Ithird_party/mbedtls/include"]
    + select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
            "//src/portability:hdrs",
            "//third_party/mbedtls"],
    linkopts = select({"windows": [],
                       "msvc": [],
                       "msys": ["-lpthread"], 
                       "//conditions:default": ["-lpthread"]}),
    srcs = ["oic_platform.c",
            "posix/ocatomic.c",
            "posix/ocevent.c",
            "posix/octhread.c"]
)

cc_library(
    name = "win",
    # alwayslink = True,
    # linkstatic = 1,
    copts = ["-Isrc",
             "-Isrc/logger",
             "-Isrc/portability",
             "-Ithird_party/mbedtls/include"]
    + select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
            "//src/portability:hdrs",
            "//third_party/mbedtls"],
    # linkopts = ["-W,crypt32.lib"],
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
            "windows/strptime.c",
            "windows/vs12_snprintf.h",
            "windows/win_sleep.c",
            "windows/win_sleep.h",

            "ocrandom.h",

            "oic_platform.c",
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
    + select({":windows": ["//src/portability:win"],
              ":msvc": ["//src/portability:win"],
              ":msys": ["//src/portability:posix"],
              "//conditions:default": ["//src/portability:posix"]}),
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
	     "-Isrc/portability/windows",
    	     "-Isrc/util",
             "-Iexternal/openocf/src/util"]
    + select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11"]}),
    hdrs = glob(["*.h"]),
    srcs = ["iotivity_commontypes.h",
    	    "oic_malloc.c",
	    # "ocatomic.h",
	    # "ocevent.h",
            "ocrandom.c",
            # "ocrandom.h",
            # "ocrandom_seed.c",
	    "octimer.c",
	    # "octimer.h",
	    # "octhread.h",
            "oic_string.c",
            # "oic_string.h",
            "oic_time.c",
            # "oic_time.h",
            # "oic_platform.h"
    ],
    visibility = ["//src:__subpackages__",
                  "//third_party:__subpackages__"]
 )
