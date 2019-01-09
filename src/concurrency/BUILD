load("//config:variables.bzl", "CSTD", "DEFINES", "OS_COPTS")

cc_library(
    name = "concurrency",
    alwayslink = True,
    linkstatic = 1,
    copts = ["-Isrc",
             # "-Iexternal/openocf/src/util",
             "-Iexternal/libcoap/include",
             # "-Iexternal/libcoap/include/coap",
    ]+ CSTD + OS_COPTS
    + select({"//config:debug_threads": ["-DDEBUG_THREADS"],
              "//conditions:default": []})
    + select({"//config:debug_tls": ["-DDEBUG_TLS"],
              "//conditions:default": []})
    + select({"//config:debug_msgs": ["-DDEBUG_MSGS"],
              "//conditions:default": []}),
    defines = DEFINES,
    deps = ["//src:config",
            "@libcoap//:libcoap"
    ],
    srcs = ["caqueueingthread.c", "caqueueingthread.h",
            "cathreadpool_pthreads.c", "cathreadpool_pthreads.h"],
    visibility = ["//visibility:public"]
)
