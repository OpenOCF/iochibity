load("//config:variables.bzl", "CSTD", "OS_COPTS")

# to turn on logging for entire stack add to //user.bazelrc:
# build --define enable_logging=true

cc_binary(
    name = "client",
    copts = ["-Iinclude",
             "-Iexternal/libcoap/include",
             "-Iexternal/tinycbor/src",
             "-DTB_LOG"
    ]+ CSTD + OS_COPTS,
    deps = ["//:openocf"],
    srcs = ["client.c", "clog.h"]
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
