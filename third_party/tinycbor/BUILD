package(default_visibility = ["//visibility:public"])

load("//tools/config:variables.bzl", "COPTS_ANDROID")

licenses(["notice"])

config_setting(
    name = "android",
    values = {# "define": "target=android",
        # "crosstool_top": "@android_ndk//:toolchain-libcpp",
        "cpu": "armeabi-v7a",
              # "android_cpu": "x86",
              # "android_crosstool_top": "//platforms/ndk:toolchain"
    }
)
config_setting(
    name = "darwin",
    values = { "cpu": "darwin" }
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
    name = "tinycbor",
    copts = select({"windows": [],
                    "msvc": [],
                    "msys": [],
                    "darwin": ["-std=c11", "-x c"],
                    # "android": COPTS_ANDROID,
                    "//conditions:default": ["-std=c11", "-x c"]}),
    # deps = ["//third_party/ndk"],
    hdrs = ["src/cbor.h"],
    srcs = [
        "src/cbor.h",
        "src/assert_p.h",
        "src/cborconstants_p.h",
        "src/cborencoder.c",
        #"src/cborencoder_close_container_checked.c",
        "src/cborerrorstrings.c",
        #"src/cborjson.h",
        "src/cborparser.c",
        "src/cborparser_dup_string.c",
        #"src/cborpretty.c",
        #"src/cbortojson.c",
        "src/compilersupport_p.h",
        "src/extract_number_p.h",
        #"src/math_support_p.h",
        #"src/open_memstream.c"
    ]
    # visibility = ["//src/ocf:__pkg__",
    #               "//src/ocf/stack:__pkg__",
    #               "//src/sec:__pkg__",
    #               "//src/sec/acl:__pkg__",
    #               "//src/provisioning:__pkg__"]
)
