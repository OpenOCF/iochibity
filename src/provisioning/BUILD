config_setting(
    name = "windows",
    values = { "cpu": "x64_windows" }
)
config_setting(
    name = "msvc",
    values = { "cpu": "x64_windows_msvc" }
)
config_setting(
    name = "msys",
    values = { "cpu": "x64_windows_msys" }
)

cc_library(
    name = "provisioning",
    copts = ["-Isrc",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/tinycbor/src",
             "-Iexternal/openocf/third_party/tinycbor/src",
             "-Ithird_party/coap",
             "-Iexternal/openocf/third_party/coap",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist",
             "-Ithird_party/sqlite3",
             "-Iexternal/openocf/third_party/sqlite3"
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    linkstatic = 1,
    deps = ["//src:config",
            "//src/comm",
            "//src/logger",
            "//src/portability",
            "//src/sec/svrs",
            "//src/util",
            "//include",
            "//third_party/coap",
            "//third_party/mbedtls",
            "//third_party/tinycbor",
	    "//third_party/sqlite3",
	    "//third_party/utlist"
    ],
    #         # if MULTIPLE_OWNER
    #         #"//src/provisioning/multiowner"
    #         # if WITH_TCP
    #         # "//src/provitioning/cloud"
    # + select({"windows": ["//third_party/sqlite3"],
    #           "//conditions:default": []}),
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//src/ocf:__pkg__",
                  "//src/sec/acl:__pkg__",
                  "//src/sec:__pkg__",
                  "//tools/cbor:__subpackages__"]
    )
