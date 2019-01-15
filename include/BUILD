package(default_visibility = ["//visibility:public"])

exports_files(["openocf.h", "openocf_h.h"])

# FIXME: deal with externals, coap

cc_library(
    name = "include",
    copts = ["-std=c11"],
    hdrs = ["openocf.h", "openocf_h.h"]
)
