load("//config:variables.bzl", "DEFINES", "OS_COPTS")

cc_library(
    name = "sec",
    alwayslink = True,
    linkstatic = True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
             "-Isrc/comm/include",
             "-Iexternal/openocf/src/comm/include",
             "-Isrc/ocf",
             "-Iexternal/openocf/src/ocf",
             "-Isrc/ocf/internal",
             "-Iexternal/openocf/src/ocf/internal",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
             "-Isrc/provisioning/include",
             "-Iexternal/openocf/src/provisioning/include",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Isrc/sec",
             "-Iexternal/openocf/src/sec",
             "-Iconfig/darwin",
             "-Iconfig/darwin/coap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
             "-Iexternal/mbedtls/patched/include",
             "-Iexternal/tinycbor/src",
    ] + OS_COPTS,

    defines = DEFINES,

    deps = ["//src:config",
            "//src/comm",
            "//src/logger",
            "//src/portability",
            "//src/provisioning",
            "//src/sec/certificates",
            "//src/sec/crypto",
            "//src/sec/dox",
            "//src/sec/svrs",
            "//src/util",
            "@tinycbor//:tinycbor",
            "@libcoap//:libcoap",
            "@mbedtls//:mbedtls",
            "@uthash//:uthash"],
    srcs = glob(["*.c"]) + glob(["*.h"]),
    # + select({"windows": ["strptime.c"],
    #           "//conditions:default": []}),
    visibility = [#"//src/ocf:__pkg__",
                  #"//src/provisioning:__pkg__",
                  "//visibility:public"]
    )
