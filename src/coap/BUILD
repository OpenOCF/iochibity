load("//config:variables.bzl", "CSTD", "DEFINES", "OS_COPTS")

cc_library(
    name = "coap",
    visibility = ["//visibility:public"],
    linkstatic = 1,
    copts = ["-Iexternal/libcoap/include"]
    + CSTD + OS_COPTS
    + select({"//config:debug_threads": ["-DDEBUG_THREADS"],
              "//conditions:default": []})
    + select({"//config:debug_msgs": ["-DDEBUG_MSGS"],
              "//conditions:default": []})
    + select({"//config:debug_tls": ["-DDEBUG_TLS"],
              "//conditions:default": []}),

    defines = DEFINES,

    deps = ["@libcoap//:libcoap"],

    hdrs = ["coap_codec.h"],
    srcs = ["coap_codec.c"],
)
