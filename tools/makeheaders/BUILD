config_setting(
    name = "android",
    values = {"define": "target=android",
              "android_cpu": "x86",
              # "android_crosstool_top": "//platforms/ndk:toolchain"
    }
)
config_setting(
    name = "ndk",
    values = { "crosstool_top": "//platforms/ndk:toolchain" })
config_setting(
    name = "windows",
    values = { "cpu": "x64_windows" })
config_setting(
    name = "msvc",
    values = { "cpu": "x64_windows_msvc" })
config_setting(
    name = "msys",
    values = { "cpu": "x64_windows_msys" })

cc_binary(
    name = "cnoh",
    copts = select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11",
                                       "-x c"]}),
    srcs = ["cnoh.c"],
)

cc_binary(
    name = "makeheaders",
    copts = select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11",
                                       "-x c"]}),
    srcs = ["makeheaders.c"],
    visibility = ["//visibility:public"]
)

# genrule(
#     name = "headers",
#     srcs = [],
#     outs = ["src/util/base64.h"],
#     cmd = "./$(location makeheaders) //src/util/base64.c",
#     tools = [":makeheaders"],
# )

# load(":makeheaders.bzl", "make_headers")

# make_headers(
#     name = "headers",
#     deps = [":makeheaders",
#             "//src/sec"],
# )

# make_headers(
#     name = "headers",
#     deps = [":makeheaders",
#             "//src/comm"],
# )

