
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

cc_binary(
    name = "cbordump",
    copts = ["-Iexternal/tinycbor/src",
    ],
    srcs = ["tools/cbordump/cbordump.c",
            "src/cborparser.c",
            "src/cborerrorstrings.c",
            "src/cborparser_dup_string.c",
            "src/cborpretty.c",
            "src/cbortojson.c",
            "src/open_memstream.c",
    ] + glob(["src/*.h"]),
    visibility = ["//visibility:public"]
)

# cbordump: cbordump.o cborparser.o cborerrorstrings.o cborpretty.o cbortojson.o open_memstream.o
