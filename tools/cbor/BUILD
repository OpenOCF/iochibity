# FIXME: remove deps on third_party, should depend only on openocf

config_setting(
    name = "windows",
    values = { "cpu": "x64_windows" }
)
config_setting(
    name = "android",
    values = {"define": "target=android",
              "android_cpu": "x86",
              "android_crosstool_top": "//platforms/ndk:toolchain" }
)
config_setting(
    name = "linux",
    values = { "cpu": "k8" }
)
config_setting(
    name = "wrlinux",
    values = { "crosstool_top": "//platforms/wrlinux:toolchain" }
)
config_setting(
    name = "rpi3b",
     values = { "crosstool_top": "//platforms/rpi3b:toolchain" }
)

cc_binary(
    name = "json2cbor",
    # linkstatic = 1,
    copts = [# "-Iinclude",
             "-Isrc/portability",
             "-Isrc/sec/svrs",
             "-Isrc/util",
             "-Ithird_party/utlist",
             "-Ithird_party/cjson",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             "-Ithird_party/tinycbor/src"],
    # + select({":rpi3b": ["-Iexternal/cosysroot_rpi3b/usr/include",
    #                      "-Iexternal/cosysroot_rpi3b/usr/include/cdk",
    #                      "-std=c11"],
    #           ":android": ["-Iexternal/cosysroot_ndk/usr/include",
    #                      "-Iexternal/cosysroot_ndk/usr/include/cdk",
    #                      "-std=c11"],
    #           ":windows": ["-Iexternal/usr_local/include",
    #                        "-Iexternal/usr_local/include/cdk"],
    #           "//conditions:default": ["-Iexternal/usr_local/include",
    #                                    "-Iexternal/usr_local/in[<clude/cdk",
    #                                    "-std=c11",
    #                                    "-U DEBUG"]}), # OS X
    deps = [# "//include",
            "//src/comm",
            "//src/ocf",
            "//src/portability",
            "//src/provisioning",
            "//src/sec",
            "//src/sec/crypto",
            "//src/sec/svrs",
            "//src/util",
            "//third_party/utlist",
            "//third_party/cjson",
            "//third_party/coap",
            "//third_party/tinycbor"],
    # + select({":rpi3b": ["@cosysroot_rpi3b//:cdk"],
    #           ":android": ["@cosysroot_ndk//:cdk"],
    #           ":wrlinux": ["@cosysroot_wrlinux//:cdk"],
    #           "//conditions:default": ["@usr_local//:cdk"]}),
    # linkopts = ["-Wl,-dead_strip"],
    srcs = ["json2cbor.c",
            "json2cbor.h"]
)
# json2cbor cborencoder.c cborerrorstrings.o cborencoder_close_container_checked.o cJSON.o

# cbordump: cbordump.o cborparser.o cborerrorstrings.o cborpretty.o cbortojson.o open_memstream.o
