 config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_binary(
    name = "client",
    copts = ["-Isrc",
             "-Isrc/comm/api",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Isrc/ocf/client",
             "-Isrc/portability",
             "-I/usr/local/include",
             "-Ithird_party/cjson",
             "-Ithird_party/coap/include"],
    deps = ["//src:config",
            # "//src/comm/api",
            "//src/ocf",
            "//src/ocf:client",
            "//third_party/cjson",
            "//third_party/coap",
            "@sys_cdk//:cdk-pkg"
    ],
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
             "-Ithird_party/coap/include"],
    deps = ["//src:config",
            "//src/ocf"],
    linkopts = ["-lncurses"],
    srcs = ["ocserver.c"]
)

