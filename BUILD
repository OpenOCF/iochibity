load("//config:variables.bzl", "CSTD", "DEFINES")

# filegroup(
#     name = "srcs",
#     srcs = glob(["**"]),
#     visibility = ["//src/test/shell/bazel/testdata:__pkg__"],
# )

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
                "//test/unit/common:oic_malloc_tests",
                "//test/unit/common:randomtest",
                "//test/unit/common:oic_string_tests",
                "//test/unit/common:oic_time_tests"],
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

load("@build_bazel_rules_apple//apple:ios.bzl", "ios_application")

apple_static_library(
    name = "ios_openocf",
    minimum_os_version = "8.0",
    platform_type = "ios",
    # linkopts=["--no_warnings_for_no_symbols"],
    deps = ["//src/ocf"],
    visibility = ["//visibility:public"]
)

