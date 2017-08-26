licenses(["notice", "reciprocal"])

config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "coap",
    alwayslink = True,
    copts = ["-Isrc",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             "-Ithird_party/coap/include/coap"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    defines = ["WITH_POSIX"],
    deps = ["//src:config"],
    hdrs = ["include/coap/coap.h",
            "include/coap/config.h",
            "include/coap/uri.h",
            "include/coap/pdu.h"
    ],
    srcs = ["include/coap/address.h",
            "async.c",
            "include/coap/async.h",
            "include/coap/bits.h",
            "block.c",
            "include/coap/block.h",
            #"coap.h",
            "coap_list.c",
            "include/coap/coap_list.h",
            "include/coap/coap_time.h",
            #"config.h",
            #config.h.in
            #configure.in
            "debug.c",
            "include/coap/debug.h",
            "encode.c",
            "include/coap/encode.h",
            #examples
            "hashkey.c",
            "include/coap/hashkey.h",
            #makefile_arduino.mak
            "include/coap/mem.h",
            "net.c",
            "include/coap/net.h",
            "option.c",
            "include/coap/option.h",
            "pdu.c",
            #"pdu.h",
            "include/coap/prng.h",
            "resource.c",
            "include/coap/resource.h",
            "str.c",
            "include/coap/str.h",
            "subscribe.c",
            "include/coap/subscribe.h",
            "include/coap/t_list.h",
            #tests
            "uri.c",
            #"uri.h",
            "include/coap/uthash.h",
            "include/coap/utlist.h"],
    visibility = ["//src/comm:__subpackages__",
                  "//src/provisioning:__pkg__",
                  "//src/sec/acl:__pkg__",
                  "//src/sec:__pkg__"]
)

