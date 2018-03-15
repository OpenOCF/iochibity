config_setting(
    name = "android",
    # values = { "crosstool_top": "//external:android/crosstool."} # "@android_ndk//:toolchain-libcpp"
    values = {"cpu": "armeabi-v7a"}
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
    name = "util",
    alwayslink = True,
    copts = ["-D_GNU_SOURCE",
     "-DWITH_POSIX"
    ] + select({":windows": [],
                ":msvc": [],
                ":msys": [],
		# FIXME: don't we want jni.h, jni_md.h from the target instead of the build host???
                ":android": ["-std=c11",
                             "-Iexternal/local_jdk/include",
                             "-Iexternal/local_jdk/include/linux"],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps  = ["//src:config",
    	     # "//src/comm",
             "//src/logger",
             "//src/portability",
             "//src/util",
             "//third_party/coap",
             "//third_party/mbedtls",
             "//third_party/mbedtls:mbedtls-crypto",
             "//third_party/mbedtls:mbedtls-x509"]
    + select({":android": [":jnihdrs"],
              "//conditions:default": []}),
    hdrs = ["caadapterutils.h",
            #"ca_adapter_net_ssl.h"
    ],
    srcs  = ["caadapterutils.c"]
    + select({":android": glob(["*_android.c"]) + glob(["*_android.h"]),
              "//conditions:default": []}),
    visibility = ["//visibility:public"]
# "//src/comm:__subpackages__",
#                   "//src/sec/svrs:__pkg__"]
)

cc_library(
    name = "jnihdrs",
    copts = ["-std=c11"],
    hdrs = ["@local_jdk//:jni_header",
            "@local_jdk//:jni_md_header-linux"],
)
