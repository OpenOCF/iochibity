load("//config:variables.bzl", "CSTD", "DEFINES", "OS_COPTS")

cc_library(
    name = "comm",
    # alwayslink = True,
    linkstatic = 1,
    copts = ["-Isrc",
             "-Iexternal/openocf/src/util",

             #"-Ithird_party/coap",
             #"-Ithird_party/coap/include",
             "-Iconfig/darwin",
             "-Iconfig/darwin/coap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",

             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist",
    ]+ CSTD + OS_COPTS
    + select({"//config:debug_threads": ["-DDEBUG_THREADS"],
              "//conditions:default": []}),

 # + select({"//config:enable_tcp": "-DSTATEFUL_PROTOCOL_SUPPORTED",
 #                                 "//conditions:default": []}),

    defines = DEFINES,

    deps = ["//src:config",
            "//src/logger",
    	    "//src/portability",
            "//src/comm/transport/util",
            "//src/comm/util",
            # FIXME: select for this
            "//third_party/utlist",
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
