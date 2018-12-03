load("//config:variables.bzl", "CSTD", "DEFINES", "OS_COPTS")

cc_library(
    name = "coap",
    visibility = ["//visibility:public"],
    linkstatic = 1,
    copts = ["-Iexternal/libcoap/include",
             "-Iexternal/uthash/src"]
    + CSTD + OS_COPTS
    + select({"//config:debug_threads": ["-DDEBUG_THREADS"],
              "//conditions:default": []})
    + select({"//config:debug_msgs": ["-DDEBUG_MSGS"],
              "//conditions:default": []})
    + select({"//config:debug_tls": ["-DDEBUG_TLS"],
              "//conditions:default": []}),

    defines = DEFINES,

    deps = ["@libcoap//:libcoap",
            "@uthash//:uthash"],

    hdrs = glob(["*.h"]),
    srcs = ["cablockwisetransfer.c",
            "cablockwisetransfer.h",
            "coap_codec.c",
            "coap_codec.h"
    ] + select({"//config:enable_tcp": ["caping.c",
                                        "caping.h",
                                        "coap_tcp_signaling.c",
                                        "coap_tcp_signaling.h"],
                "//conditions:default": []})
)
