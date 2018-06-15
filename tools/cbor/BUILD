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
             "-Iexternal/libcoap",
             "-Iexternal/libcoap/include",
        "-Iexternal/tinycbor/src"],
    deps = [# "//include",
            "//:openocf",
            # "//src/ocf",
            # "//src/portability",
            # "//src/provisioning",
            # "//src/sec",
            # "//src/sec/crypto",
            # "//src/sec/svrs",
            # "//src/util",
            "//third_party/utlist",
            "@cjson//:cjson",
            "@libcoap//:libcoap",
            "@tinycbor//:tinycbor"
    ],
    # + select({":rpi3b": ["@cosysroot_rpi3b//:cdk"],
    #           ":android": ["@cosysroot_ndk//:cdk"],
    #           ":wrlinux": ["@cosysroot_wrlinux//:cdk"],
    #           "//conditions:default": ["@usr_local//:cdk"]}),
    # linkopts = ["-Wl,-dead_strip"],
    srcs = ["json2cbor.c",
            "json2cbor.h"]
)

cc_binary(
    name = "cbordump",
    copts = ["-Iexternal/tinycbor/src"],
    srcs = ["cbordump.c", "cbor2json.c"],
    deps = ["@tinycbor//:tinycbor"]
)

# cbordump: cbordump.o cborparser.o cborerrorstrings.o cborpretty.o cbortojson.o open_memstream.o
