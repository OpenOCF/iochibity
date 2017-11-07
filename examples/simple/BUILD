#  config_setting(
#     name = "windows",
#     values = { "define": "platform=windows" }
# )

config_setting(
    name = "rpi3b",
     values = { "crosstool_top": "//platforms/rpi3b:toolchain" }
)

cc_binary(
    name = "client",
    copts = ["-Isrc",
             "-Isrc/comm/api",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Isrc/ocf/client",
             "-Isrc/portability",
             "-Ithird_party/cjson",
             "-Ithird_party/coap/include"]
    + select({":rpi3b": ["-Iexternal/sysroot_rpi3b/usr/include",
                         "-Iexternal/sysroot_rpi3b/usr/include/cdk",
                         "-std=c11"],
              "//conditions:default": ["-Iexternal/usr_local/include",
                                       "-Iexternal/usr_local/include/cdk"]}),
    deps = ["//src:config",
            "//src/ocf",
            "//src/ocf:client",
            "//third_party/cjson",
            "//third_party/coap"]
    + select({":rpi3b": ["@sysroot_rpi3b//:cdk"],
              "//conditions:default": ["@usr_local//:cdk"]}),
    linkstatic = 1,
    srcs = ["occlient.c",
            "gui.h",
            "gui.c",]
)

cc_binary(
    name = "server",
    # linkstatic = 1,
    copts = ["-Isrc",
             "-Isrc/comm",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Ithird_party/coap/include"]
    + select({":rpi3b": ["-Iexternal/sysroot_rpi3b/usr/include",
                         "-Iexternal/sysroot_rpi3b/usr/include/cdk",
                         "-std=c11"],
              "//conditions:default": ["-Iexternal/usr_local/include",
                                       "-Iexternal/usr_local/include/cdk",
                                       "-std=c11"]}),
    deps = ["//src:config",
            "//src/ocf"]
    + select({":rpi3b": ["@sysroot_rpi3b//:rpi3b"],
              "//conditions:default": ["@usr_local//:cdk"]}),
    # linkopts = ["-Wl,-dead_strip"],
    srcs = ["ocserver.c"]
)
