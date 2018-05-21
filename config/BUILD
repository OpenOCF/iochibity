config_setting(
    name = "linux",
    values = { "host_cpu": "k8",
               "cpu": "k8"},
    visibility = ["//visibility:public"]
)

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

# fat_apk_cpu values: armeabi, armeabi-v7a, arm64-v8a, mips, mips64, x86, x86_64

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


config_setting(
    name = "android",
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
    name = "android_arm64",
    values = {
        # android binaries:
        #"cpu": "arm64-v8a",
        "android_cpu": "arm64-v8a",

        # //:mkhdrs
        # "fat_apk_cpu": "arm64-v8a",
        # "android_cpu": "arm64-v8a",
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

