config_setting(
    name = "windows",
    values = { "cpu": "x64_windows" }
)
config_setting(
    name = "darwin",
    values = { "cpu": "darwin" })
config_setting(
    name = "darwin64",
    values = { "cpu": "darwin_x86_64" })

cc_library(
    name = "dox",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/logger",
             "-Isrc/comm/api",
             "-Isrc/ocf",
             "-Isrc/portability",
             "-Isrc/sec",
             "-Isrc/util",
             "-Ithird_party/coap",
             "-Iexternal/openocf/third_party/coap",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/tinycbor/src",
             "-Iexternal/openocf/third_party/tinycbor/src"
    ] + select({"windows": [],
                "darwin": ["-std=c11", "-UDEBUG"],
                "darwin64": ["-std=c11", "UDEBUG"],
                "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
            # "//src/ocf:hdrs",
            "//src/portability",
            "//src/sec/crypto",
            "//src/sec/util",
            "//src/util",
            "//third_party/coap",
            "//third_party/tinycbor"],
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//src/provisioning:__pkg__",
                  "//src/sec:__pkg__",
                  "//src/ocf:__pkg__"]
)
