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
