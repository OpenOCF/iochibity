filegroup(
    name = "srcs",
    srcs = glob(["**"]),
    visibility = ["//src/test/shell/bazel/testdata:__pkg__"],
)

cc_binary(
    name = "hello",
    srcs = ["hello.cc"],
)

cc_library(
    name = "cbor",
    deps = ["@tinycbor//:tinycbor-x509"])

cc_library(
    name = "coap",
    deps = ["@libcoap//:libcoap-lib"])

# cc_library(
#     name = "mbed",
#     deps = ["@mbedtls//:mbedtls-lib"])

load("//tools/makeheaders:makeheaders.bzl", "prep_headers", "make_headers")

# genrule(
#     name = "headers",
#     srcs = [],
#     outs = [],
#     cmd = "./$(location makeheaders)",
#     tools = ["//tools/makeheaders"],
# )

# filegroup(
#   name = "mh_srcs",
#   srcs = glob(["src/**/*.c"]),
# )

prep_headers(
    name = "mkhdrs",
    # out = "foo.dat",
    # srcs = [":mh_srcs"],
    hdr_deps = ["//src/ocf"],
            # "//tools/makeheaders"]
)

make_headers(
    name = "headers",
    deps = [":mkhdrs"]
)
