# FIXME: remove deps on third_party, should depend only on openocf

cc_binary(
    name = "server",
    # linkstatic = 1,
    copts = ["-Isrc",
             "-Iinclude",
             "-Isrc/comm",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Iconfig/darwin",
             "-Iconfig/darwin/coap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
             "-Iexternal/tinycbor/src",
    ]
    + select({"//config:rpi_arm8": ["-Iexternal/cosysroot_rpi3b/usr/include",
                                    "-Iexternal/cosysroot_rpi3b/usr/include/cdk",
                                    "-std=c11"],
              "//config:android_arm8": ["-Iexternal/cosysroot_ndk/usr/include",
                                        "-Iexternal/cosysroot_ndk/usr/include/cdk",
                                        "-std=c11"],
              "//config:windows": ["-Iexternal/usr_local/include",
                                   "-Iexternal/usr_local/include/cdk"],
              "//conditions:default": ["-Iexternal/usr_local/include",
                                       "-Iexternal/usr_local/in[<clude/cdk",
                                       "-std=c11",
                                       "-U DEBUG"]}), # OS X
    data = [":config"], # ["server_config.cbor"],
    deps = ["//:openocf",
            ## FIXME: remove all deps except //src/ocf
            "//src:config",
            "@libcoap//:libcoap",
            "@tinycbor//:tinycbor"],
    # + select({":rpi3b": ["@cosysroot_rpi3b//:cdk"],
    #           ":android": ["@cosysroot_ndk//:cdk"],
    #           ":wrlinux": ["@cosysroot_wrlinux//:cdk"],
    #           "//conditions:default": ["@usr_local//:cdk"]}),
    linkopts = select({"//config:windows": [],
                       # "msvc": [],
                       # "msys": ["-lpthread"],
                       "//config:linux": ["-lpthread"],
                       "//config:rpi_arm8": ["-lpthread"],
                       #"wrlinux": ["-lpthread"],
                       # "android": [],
                       # "androidx86": [],
                       #"ndk": [],
                       "//conditions:default": []}),
    srcs = ["server.c",
    ]
    # experimental static init:
    #"config/svr_config.c"
    # "config/acl.c",
    # "config/pstat.c",
    # "config/doxm.c",
    # "config/cred.c"]
)

# genrule(
#     name = "svrs",
#     srcs = ["server_config.edn"],
#     outs = ["config/server_config.cbor"],
#     # output_to_bindir = 1,
#     cmd = "\n".join(["edn-to-json $(location server_config.edn)  > server_config.json",
#                      "json2cbor server_config.json > /tmp/server_config.cbor",
#                      "chmod ug+rw /tmp/server_config.cbor",
#                      "ls -l /tmp",
#                      "cp -p /tmp/server_config.cbor \"$@\"",])
# )

# https://www.npmjs.com/package/edn-to-json
genrule(
    name = "json",
    srcs = ["server_config.edn"],
    outs = ["server_config.json"],
    cmd = "edn-to-json $(location server_config.edn)  > \"$@\"",
)

genrule(
    name = "config",
    srcs = ["server_config.edn"],
    outs = ["server_config.cbor"],
    #output_to_bindir = 1,
    cmd = "\n".join(["edn-to-json $(location server_config.edn)  > server_config2.json",
                     "echo FOO",
                     "pwd",
                     "ls -l",
                     "json2cbor server_config2.json > server_config.cbor",
                     "chmod ug+rw server_config.cbor",
                     "ls -l",
                     "cp -v server_config.cbor \"$@\"",
                     "chmod -v ug+rw \"$@\""])
                     #"chmod ug+rw \"$@\""])
                     #"json2cbor $(location server_config.json) > \"$@\""]),
    # tools = ["edn-to-json"],
)
