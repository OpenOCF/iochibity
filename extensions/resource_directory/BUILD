load("//config:variables.bzl", "CSTD", "DEFINES")

cc_library(
    name = "resource_directory",
    alwayslink = True,
    linkstatic=True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
             "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/util",
             "-Isrc/ocf",
             "-Iexternal/openocf/src/ocf",
             "-Isrc/sec",
             "-Iexternal/openocf/src/sec",
             "-Isrc/sec/aclroles",
             "-Iexternal/openocf/src/sec/aclroles",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Iexternal/libcoap/include",
             # "-Iexternal/libcoap/include/coap",
             "-Iexternal/tinycbor/src",
             "-Iexternal/uthash/src",
    ] + select({"//config:msvc": [],
                "//config:darwin": ["-std=c11", "-U DEBUG"],
                "//config:darwin64": ["-std=c11", "-U DEBUG"],
                "//config:ios_arm64": ["-U DEBUG"],
                "//config:ios_x86": ["-U DEBUG"],
                "//conditions:default": ["-std=c11"]})
    + CSTD,

    defines = DEFINES,

    deps = ["@sqlite3//:sqlite3"],
    srcs = ["oicresourcedirectory.c",
            "oicresourcedirectory.h"],
    visibility = ["//visibility:public"]
)
