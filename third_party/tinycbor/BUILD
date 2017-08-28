package(default_visibility = ["//visibility:public"])

licenses(["notice"])

config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "tinycbor",
    copts = select({"windows": [],
                    "//conditions:default": ["-std=c11"]}),
    hdrs = ["src/cbor.h"],
    srcs = [
        "src/cbor.h",
        "src/assert_p.h",
        "src/cborconstants_p.h",
        "src/cborencoder.c",
        #"src/cborencoder_close_container_checked.c",
        "src/cborerrorstrings.c",
        #"src/cborjson.h",
        "src/cborparser.c",
        "src/cborparser_dup_string.c",
        #"src/cborpretty.c",
        #"src/cbortojson.c",
        "src/compilersupport_p.h",
        "src/extract_number_p.h",
        #"src/math_support_p.h",
        #"src/open_memstream.c"
    ]
    # visibility = ["//src/ocf:__pkg__",
    #               "//src/ocf/stack:__pkg__",
    #               "//src/sec:__pkg__",
    #               "//src/sec/acl:__pkg__",
    #               "//src/provisioning:__pkg__"]
)
