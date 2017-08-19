licenses(["notice", "reciprocal"])

cc_library(
    name = "coap",
    copts = ["-std=c11", "-Icoap"],
    defines = ["WITH_POSIX"],
    hdrs = ["coap/uri.h",
            "coap/pdu.h"],
    srcs = ["address.h",
            "async.c",
            "async.h",
            "bits.h",
            "block.c",
            "block.h",
            "coap.h",
            "coap_list.c",
            "coap_list.h",
            "coap_time.h",
            "config.h",
            #config.h.in
            #configure.in
            "debug.c",
            "debug.h",
            "encode.c",
            "encode.h",
            #examples
            "hashkey.c",
            "hashkey.h",
            #makefile_arduino.mak
            "mem.h",
            "net.c",
            "net.h",
            "option.c",
            "option.h",
            "pdu.c",
            "pdu.h",
            "prng.h",
            "resource.c",
            "resource.h",
            "str.c",
            "str.h",
            "subscribe.c",
            "subscribe.h",
            "t_list.h",
            #tests
            "uri.c",
            "uri.h",
            "uthash.h",
            "utlist.h"],
    visibility = ["//src/comm:__subpackages__",
                  "//src/provisioning:__pkg__",
                  "//src/sec:__pkg__"]
)
