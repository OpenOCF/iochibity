load("//config:variables.bzl", "CSTD", "DEFINES", "OS_COPTS")

cc_library(
    name = "comm",
    alwayslink = True,
    linkstatic = 1,
    copts = ["-Isrc",
             "-Iexternal/openocf/src/util",

             #"-Ithird_party/coap",
             #"-Ithird_party/coap/include",
             "-Iconfig/darwin",
             "-Iconfig/darwin/coap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
    ]+ CSTD + OS_COPTS
    + select({"//config:debug_threads": ["-DDEBUG_THREADS"],
              "//conditions:default": []})
    + select({"//config:debug_tls": ["-DDEBUG_TLS"],
              "//conditions:default": []})
    + select({"//config:debug_msgs": ["-DDEBUG_MSGS"],
              "//conditions:default": []}),


 # + select({"//config:enable_tcp": "-DSTATEFUL_PROTOCOL_SUPPORTED",
 #                                 "//conditions:default": []}),

    defines = DEFINES,

    deps = ["//src:config",
    	    "//src/portability",
            "//src/comm/transport/util",
            "//src/comm/util",
            "//src/coap",
            "//src/logger",
            # FIXME: select for this
            "@uthash//:uthash",
            # "//third_party/mbedtls",
            # "//third_party/mbedtls:mbedtls-crypto",
            # "//third_party/mbedtls:mbedtls-x509",
            "@mbedtls//:mbedtls",
            "@libcoap//:libcoap"]
    + select({"//config:disable_dtls": [],
              "//conditions:default": ["//src/comm/transport/udpip"]})
    + select({"//config:enable_tcp": ["//src/comm/transport/tcpip"],
              "//conditions:default": []}),
    srcs = glob(["*.c"], exclude=["caadapternetdtls.c"])
    + glob(["*.h"], exclude=["caadapternetdtls.h"]),
    visibility = ["//visibility:public"]
)
