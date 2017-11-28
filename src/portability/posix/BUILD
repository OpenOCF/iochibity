config_setting(
    name = "androidx86",
    values = {"fat_apk_cpu": "x86",
              # "android_crosstool_top": "//platforms/ndk:toolchain"
    }
)
config_setting(
    name = "wrlinux",
    values = { "crosstool_top": "//platforms/wrlinux:toolchain" }
)
config_setting(
    name = "linux",
    values = { "cpu": "k8" }
)
config_setting(
    name = "rpi3b",
     values = { "crosstool_top": "//platforms/rpi3b:toolchain" }
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

# if platform != windows and defined(HAVE_LIBPTHREAD)
# NB: for now we ignore platforms lacking pthreads
cc_library(
    name = "posix",
    copts = ["-Ithird_party/utlist",
             "-DWITH_POSIX",
    #          "-Iexternal/openocf/src",
    #          "-Isrc/logger",
    #          "-Iexternal/openocf/src/logger",
    #          "-Isrc/portability"]
             "-Ithird_party/mbedtls/include/mbedtls"
    ] + select({"windows": [],
                     "msvc": [],
                     "msys": [],
                     "//conditions:default": ["-std=c11",
                                              "-U DEBUG"]}), # OS X
    deps = [#"//src:config",
            "//src/logger",
            # "//src/portability:hdrs",
            "//third_party/mbedtls",
            "//third_party/utlist"],
    linkopts = select({"windows": [],
                       "msvc": [],
                       "msys": ["-lpthread"],
                       "linux": ["-lpthread"],
                       "rpi3b": ["-lpthread"],
                       "wrlinux": ["-lpthread"],
                       # "android": [],
                       "androidx86": [],
                       #"ndk": [],
                       "//conditions:default": []}),
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//visibility:public"]
)
