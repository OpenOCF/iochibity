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
    name = "interface",
    copts = ["-Isrc/comm/util",
             "-Ithird_party/coap/include"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//third_party/coap"],
    hdrs  = ["caadapterinterface.h",
             "caipinterface.h",
             "caprotocolmessage.h"],
    visibility = ["//src/comm:__subpackages__",
                  "//src/ocf:__pkg__",
                  "//src/sec:__subpackages__",
                  "//src/provisioning:__subpackages__"]
)
