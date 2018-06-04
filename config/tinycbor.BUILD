
# TODO: platform-dependent configs
# TODO: with_tcp, etc

cc_library(
    name = "tinycbor",
    copts = ["-Iexternal/tinycbor/src",
    ],
    srcs = ["src/cborparser.c",
            "src/cborparser_dup_string.c",
            "src/cborencoder.c",
            "src/cborerrorstrings.c"],
    hdrs = glob(["src/*.h"]),
    visibility = ["//visibility:public"]
)
