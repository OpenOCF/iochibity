cc_library(
    name = "cbor",
    copts = ["-Iexternal/libcoap/include",
             "-Iexternal/tinycbor/src"],
    deps = ["//src/logger",
            "@libcoap//:libcoap",
            "@tinycbor//:tinycbor"],
    hdrs = ["oocf_cbor_decode.h",
            "oocf_cbor_encode.h"],
    srcs = ["oocf_cbor_decode.c",
            "oocf_cbor_encode.c"],
    visibility = ["//visibility:public"]
)

