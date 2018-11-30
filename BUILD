load("//config:variables.bzl", "CSTD", "DEFINES")

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

# genrule(
#     name = "headers",
#     srcs = ["mkhdrs.dat"],
#     outs = ["foo.h"],
#     cmd = "makeheaders -f mkhdrs.dat",
# )

prep_headers(
    name = "mkhdrs",
    hdr_deps = [":openocf",
                "//test/common:oic_malloc_tests",
                "//test/common:randomtest",
                "//test/common:oic_string_tests",
                "//test/common:oic_time_tests"],
)

# prep_headers(
#     name = "testhdrs",
#     hdr_deps = ["//test/common:oic_time_tests"],
# )

# make_headers(
#     name = "headers",
#     deps = [":mkhdrs"]
# )

cc_binary(
    name = "libopenocf.so",
    # copts = ["-Ithird_party/coap",
    #          "-Ithird_party/coap/include"
    # ] + select({":windows": [],
    #             ":msvc": [],
    #             ":msys": [],
    #             ":darwin": ["-std=c11", "-U DEBUG"],
    #             "//conditions:default": []}),
    #linkstatic = 1,
    linkshared = 1,
    # alwayslink = True,
    # data = ["//:mkhdrs"],
    deps = ["//src/ocf"],
    # srcs = ["libopenocf.so"],
    visibility = ["//visibility:public"]
)

# cc_binary(
#     name = "libopenocf.so",
#     # copts = ["-Ithird_party/coap",
#     #          "-Ithird_party/coap/include"
#     # ] + select({":windows": [],
#     #             ":msvc": [],
#     #             ":msys": [],
#     #             ":darwin": ["-std=c11", "-U DEBUG"],
#     #             "//conditions:default": []}),
#     #linkstatic = 1,
#     linkshared = 1,
#     # alwayslink = True,
#     # data = ["//:mkhdrs"],
#     deps = ["//src/ocf"],
#     # srcs = ["libopenocf.so"],
#     visibility = ["//visibility:public"]
# )

# cc_library(
#     name = "openocf.a",
#     linkstatic = 1,
#     alwayslink = True,
#     srcs = ["openocf"],
#     visibility = ["//visibility:public"]
# )

# cc_binary(
#     name = "libopenocf.a",
#     linkstatic = 1,
#     # linkopts = ["-static"],
#     # alwayslink = True,
#     deps = ["//src/ocf"],
#     srcs = ["//src/ocf"],
#     # hdrs = glob(["include/*h"]),
#     visibility = ["//visibility:public"]
# )

# used by applications
cc_library(
    name = "openocf",
    linkstatic = True,
    # linkopts = ["-static"],
    alwayslink = True,
    deps = ["//src/ocf"],
    visibility = ["//visibility:public"]
)

