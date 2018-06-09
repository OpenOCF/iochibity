# c dialect - https://gcc.gnu.org/onlinedocs/gcc/Standards.html#Standards

config_setting(name = "cstd_c11", define_values = { "cstd": "c11" },
    visibility = ["//visibility:public"])
config_setting(name = "cstd_iso9899_2011", define_values = { "cstd": "iso9899_2011" }, # = c11
    visibility = ["//visibility:public"])
config_setting(name = "cstd_c99", define_values = { "cstd": "c99" },
    visibility = ["//visibility:public"])

config_setting(name = "cstd_iso9899_1999", define_values = { "cstd": "iso9899_1999" }, # = c99
    visibility = ["//visibility:public"])
config_setting(name = "cstd_gnu99", define_values = { "cstd": "gnu99" },
    visibility = ["//visibility:public"])

config_setting(name = "cstd_c90", define_values = { "cstd": "c90" },
    visibility = ["//visibility:public"])
config_setting(name = "cstd_iso9899_1990", define_values = { "cstd": "iso9899_1990" }, # = c90
    visibility = ["//visibility:public"])
config_setting(name = "cstd_gnu90", define_values = { "cstd": "gnu90" },
    visibility = ["//visibility:public"])

config_setting(name = "cstd_ansi", define_values = { "cstd": "ansi" },
    visibility = ["//visibility:public"])

config_setting(
    name = "enable_logging",
    define_values = {
        "enable_logging": "true",
    },
    visibility = ["//visibility:public"]
)

# dtls is enabled by default and required for conformance
# this option allows developers to disable it for dev/testing
config_setting(name = "disable_dtls", visibility = ["//visibility:public"],
               define_values = {"disable_dtls": "true"})

# tcp is officially supported but disabled by default, so use enable_tcp
config_setting(name = "enable_tcp",
               visibility = ["//visibility:public"],
               define_values = {"enable_tcp": "true"})

# tls is on by default (if tcp is enabled)
config_setting(name = "disable_tls",
               visibility = ["//visibility:public"],
               define_values = {"disable_tls": "true"})

config_setting(
    name = "with_cloud",
    define_values = {
        #"with_tcp": "true",
        "with_cloud": "true",
    },
    visibility = ["//visibility:public"]
)

config_setting(name = "enable_dtls_tls",
               visibility = ["//visibility:public"],
               define_values = {"enable_dtls+tls": "true"})

config_setting(name = "with_ble",
               visibility = ["//visibility:public"],
               define_values = {"with_ble": "true"})

config_setting(
    name = "with_rd_client",
    define_values = {
        "with_rd": "client",
    },
    visibility = ["//visibility:public"]
)

config_setting(
    name = "with_rd_server",
    define_values = {
        "with_rd": "server",
    },
    visibility = ["//visibility:public"]
)

config_setting(
    name = "with_mq",
    define_values = {
        "with_mq": "true"
    },
    visibility = ["//visibility:public"]
)

config_setting(
    name = "with_mq_pub",
    define_values = {
        "mq": "pub",
    },
    visibility = ["//visibility:public"]
)

config_setting(
    name = "with_mq_sub",
    define_values = {
        "mq": "sub",
    },
    visibility = ["//visibility:public"]
)

config_setting(
    name = "with_mq_broker",
    define_values = {
        "mq": "broker",
    },
    visibility = ["//visibility:public"]
)

config_setting(
    name = "disable_security",
    define_values = {
        "disable_security": "true",
    },
    visibility = ["//visibility:public"]
)

################################################################
####  Linux

# local
config_setting(
    name = "linux",
    values = { "host_cpu": "k8",
               "cpu": "k8"},
    visibility = ["//visibility:public"]
)

# cross-compile
config_setting(
    name = "linux-x86_64",
    values = {
        "crosstool_top": "//tools/ctng:toolchain",
        "cpu": "x86_64"
    },
    visibility = ["//visibility:public"]
)

################################################################
####  Darwin

config_setting(
    name = "darwin",
    values = { "host_cpu": "darwin",
               "cpu": "darwin"
    },
    visibility = ["//visibility:public"]
)
config_setting(
    name = "darwin64",
    values = { "host_cpu": "darwin_x86_64",
               "cpu": "darwin_x86_64" },
    visibility = ["//visibility:public"]
)

################################################################
####  Windows

config_setting(
    name = "windows",
    values = { "cpu": "x64_windows" },
    visibility = ["//visibility:public"]
)
config_setting(
    name = "msvc",
    values = { "cpu": "x64_windows_msvc" },
    visibility = ["//visibility:public"]
)
config_setting(
    name = "msys",
    values = { "cpu": "x64_windows_msys" },
    visibility = ["//visibility:public"]
)

################################################################
####  ARM

config_setting(
    name = "arm8",
     # values = { "crosstool_top": "//platforms/rpi3b:toolchain",
     #            "host_cpu": "darwin"},
     #            #"cpu": "armv8a"},
    define_values = {
        "arm8": "true"
    },
    visibility = ["//visibility:public"]
)

################################################################
####  Raspberry Pi 3B

# build:rpi-arm8 --crosstool_top=//tools/ctng:toolchain
# build:rpi-arm8 --define arm=true
# build:rpi-arm8 --cpu=arm64-v8a

config_setting(
    name = "rpi_arm8",
    values = {
        "crosstool_top": "//tools/ctng:toolchain",
        "cpu": "arm64-v8a"
    },
    define_values = {
        "arm": "true"
    },
    visibility = ["//visibility:public"]
)

################################################################
####  Java

config_setting(
    name = "darwin_with_jni",
    values = {"host_cpu": "darwin",
#              "cpu": "armeabi-v7a",
#              "fat_apk_cpu": "x86_64"
#              "java_deps": "true"
              "host_java_toolchain": "@bazel_tools//tools/jdk:toolchain"
    },
    # define_values = {
    #     "with_jni": "true"
    # },
    visibility = ["//visibility:public"]
)

config_setting(
    name = "linux_with_jni",
    values = {"host_cpu": "k8",
              #"cpu": "armeabi-v7a",
              "host_java_toolchain": "@bazel_tools//tools/jdk:toolchain"
    },
    visibility = ["//visibility:public"]
)

################################################################
#### ANDROID
# see https://developer.android.com/ndk/guides/abis
# fat_apk_cpu values: armeabi, armeabi-v7a, arm64-v8a, mips, mips64, x86, x86_64

config_setting(name = "android", visibility = ["//visibility:public"],
               define_values = {"with_android": "true" })

config_setting(
    name = "android_arm7",
    values = {
        "cpu": "armeabi-v7a",
        #"fat_apk_cpu": "armeabi-v7a",
        "android_cpu": "armeabi-v7a",
        #"android_crosstool_top": "//external:android/crosstool",
        #"android_sdk": "@bazel_tools//tools/android:sdk"
    },
    # define_values = {
    #     "with_android": "true"
    # },
    visibility = ["//visibility:public"]
)

# it appears that config settings vary by build target.
# if the target is android_binary one thing happens, if it is cc_* something else happens,

# for android_binary:
config_setting(
    name = "android_arm8",
    values = {
        # android binaries:
        #"cpu": "arm64-v8a",
        #"android_cpu": "arm64-v8a",
        #"crosstool_top": "@androidndk//:default_crosstool",

        # //:mkhdrs
        #"fat_apk_cpu": "arm64-v8a",
        #"android_cpu": "arm64-v8a",
    },
    define_values = {
        "android_arm64": "true"
    },
    visibility = ["//visibility:public"]
)

config_setting(
    name = "android_x86",
    values = {
        #"fat_apk_cpu": "x86",
        # passing --fat_apk_cpu=x86 sets android_cpu?
        "cpu": "x86",
        "android_cpu": "x86",
        #"android_crosstool_top": "//external:android/crosstool",
        #"android_sdk": "@bazel_tools//tools/android:sdk"
    },
    # define_values = {
    #     "with_android": "true"
    # },
    visibility = ["//visibility:public"]
)

config_setting(
    name = "android_x86_64",
    values = {
        #"fat_apk_cpu": "x86",
        # passing --fat_apk_cpu=x86 sets android_cpu?
        "cpu": "x86_64",
        "android_cpu": "x86_64",
        #"android_crosstool_top": "//external:android/crosstool",
        #"android_sdk": "@bazel_tools//tools/android:sdk"
    },
    # define_values = {
    #     "with_android": "true"
    # },
    visibility = ["//visibility:public"]
)

################################################################
####  Debugging

config_setting(
    name = "debug_threads",
    visibility = ["//visibility:public"],
    define_values = { "debug_threads": "true"}
)

