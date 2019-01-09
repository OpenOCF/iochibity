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
    data = [":config_client"],
    deps = ["//:openocf"],
    srcs = ["client.c", "clog.h"]
)

genrule(
    name = "json_client",
    srcs = ["client_config.edn"],
    outs = ["client_config.json"],
    cmd = "/usr/local/bin/edn-to-json $(location client_config.edn)  > \"$@\"",
)

genrule(
    name = "config_client",
    srcs = ["client_config.edn"],
    outs = ["client_config.cbor"],
    #output_to_bindir = 1,
    cmd = "\n".join([
        "/usr/local/bin/edn-to-json $(location client_config.edn) > client_config.json;",
        "/home/gar/bin/json2cbor client_config.json > \"$@\""
    ])
)

cc_binary(
    name = "server",
    copts = ["-Iinclude",
             "-Iexternal/libcoap/include",
             "-Iexternal/tinycbor/src",
             "-DTB_LOG"
    ]+ CSTD + OS_COPTS,
    data = [":config_server"],
    deps = ["//:openocf"],
    srcs = ["server.c", "clog.h"]
)

genrule(
    name = "json_server",
    srcs = ["server_config.edn"],
    outs = ["server_config.json"],
    cmd = "/usr/local/bin/edn-to-json $(location server_config.edn)  > \"$@\"",
)

genrule(
    name = "config_server",
    srcs = ["server_config.edn"],
    outs = ["server_config.cbor"],
    #output_to_bindir = 1,
    cmd = "\n".join([
        "/usr/local/bin/edn-to-json $(location server_config.edn) > server_config.json;",
        "/home/gar/bin/json2cbor server_config.json > \"$@\""
        # "cd examples/discovery;",
        # "ls -l;",
        # "cp server_config.cbor \"$@\";"
    ])
                     #"/home/gar/bin/json2cbor server_config2.json > server_config.cbor",
                     # "chmod ug+rw server_config.cbor",
                     # "ls -l",
                     # "cp -v server_config.cbor \"$@\"",
                     # "chmod -v ug+rw \"$@\""
                     # "echo FOO",


                     #"chmod ug+rw \"$@\""])
                     #"json2cbor $(location server_config.json) > \"$@\""]),
    # tools = ["edn-to-json"],
)
