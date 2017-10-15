config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_binary(
    name = "client",
    linkstatic = 1,
    copts = ["-Isrc",
             "-Isrc/comm",
             "-Isrc/logger",
             "-Isrc/ocf"],
    # 	     "-Isrc/util"],
    deps = ["//src:config",
            "//src/ocf"],
    srcs = ["occlient.c"]
)

cc_binary(
    name = "server",
    # linkstatic = 1,
    copts = ["-Isrc",
             "-Isrc/comm",
             "-Isrc/logger",
             "-Isrc/ocf"],
    # 	     "-Isrc/util"],
    deps = ["//src:config",
            "//src/ocf"],
    linkopts = ["-lncurses"],
    srcs = ["ocserver.c"]
)

