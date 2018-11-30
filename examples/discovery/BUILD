load("//config:variables.bzl", "CSTD", "OS_COPTS")

cc_binary(
    name = "client",
    copts = ["-Iinclude",
             "-Iexternal/libcoap/include",
             "-Iexternal/tinycbor/src",
             "-DTB_LOG"
    ]+ CSTD + OS_COPTS,
    deps = ["//:openocf"],
    srcs = ["client.c"]
)

cc_binary(
    name = "server",
    copts = ["-Iinclude",
             "-Iexternal/libcoap/include",
             "-Iexternal/tinycbor/src",
             "-DTB_LOG"
    ]+ CSTD + OS_COPTS,
    deps = ["//:openocf"],
    srcs = ["server.c"]
)
