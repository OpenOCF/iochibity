config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "acl",
    alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/comm/api",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Isrc/portability",
             "-Isrc/provisioning/include",
             "-Isrc/sec",
             "-Isrc/util",
             "-Ithird_party/coap/include",
             "-Ithird_party/tinycbor/src"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/comm/api",
            "//src/logger",
            "//src/ocf:interface",
            "//src/portability",
            "//src/provisioning",
            # "//src/sec:acl",
            "//src/util",
            "//third_party/coap",
            "//third_party/tinycbor"],
    hdrs = ["ocsecurity.h"],
    srcs = ["ocsecurity.c",
            "aclresource.c",
            "aclresource.h",
            "acl_logging.h"
    ],
    visibility = ["//src/sec:__pkg__",

                  "//src/provisioning:__pkg__"]
)
