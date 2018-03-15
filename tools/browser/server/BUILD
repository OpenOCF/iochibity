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
    name = "server",
    # linkstatic = 1,
    copts = ["-Isrc",
             "-Iinclude",
             "-Isrc/comm",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             "-Ithird_party/tinycbor/src"]
    + select({":rpi3b": ["-Iexternal/cosysroot_rpi3b/usr/include",
                         "-Iexternal/cosysroot_rpi3b/usr/include/cdk",
                         "-std=c11"],
              ":android": ["-Iexternal/cosysroot_ndk/usr/include",
                         "-Iexternal/cosysroot_ndk/usr/include/cdk",
                         "-std=c11"],
              ":windows": ["-Iexternal/usr_local/include",
                           "-Iexternal/usr_local/include/cdk"],
              "//conditions:default": ["-Iexternal/usr_local/include",
                                       "-Iexternal/usr_local/in[<clude/cdk",
                                       "-std=c11",
                                       "-U DEBUG"]}), # OS X
    data = [":config"],
    deps = ["//src/ocf",
            ## FIXME: remove all deps except //src/ocf
            "//src:config",
            "//third_party/coap",
            "//third_party/tinycbor"],
    # + select({":rpi3b": ["@cosysroot_rpi3b//:cdk"],
    #           ":android": ["@cosysroot_ndk//:cdk"],
    #           ":wrlinux": ["@cosysroot_wrlinux//:cdk"],
    #           "//conditions:default": ["@usr_local//:cdk"]}),
    # linkopts = ["-Wl,-dead_strip"],
    srcs = ["server.c"]
)

genrule(
    name = "json",
    srcs = ["server_config.edn"],
    outs = ["server_config.json"],
    cmd = "edn-to-json $(location server_config.edn)  > \"$@\"",
)

genrule(
    name = "config",
    srcs = [":json"],
    outs = ["server_config.cbor"],
    cmd = "json2cbor $(location :json) > \"$@\"",
    # tools = ["edn-to-json"],
)