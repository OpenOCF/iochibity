config_setting(
    name = "windows",
    values = { "cpu": "x64_windows" }
)

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
             "-Ithird_party/coap/include",
             "-Ithird_party/tinycbor/src"
    ] + select({"windows": [],
                "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
            # "//src/ocf:hdrs",
            "//src/portability",
            "//src/sec/crypto",
            "//src/util",
            "//third_party/coap",
            "//third_party/tinycbor"],
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//src/provisioning:__pkg__",
                  "//src/sec:__pkg__",
                  "//src/ocf:__pkg__"]
)


