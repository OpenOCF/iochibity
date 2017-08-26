cc_library(
    name = "utils",
    alwayslink = True,
    copts = ["-Isrc",
             "-Isrc/comm/api",  # cacommon.h
             "-Isrc/comm/common",  # cacommonutil.h
             "-Isrc/comm/interface",
             "-Isrc/logger",
             "-Isrc/portability",
             "-Isrc/util",
             # "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             #"-Ithird_party/coap/include/coap",
             "-Ithird_party/mbedtls",
             "-Ithird_party/mbedtls/include"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps  = ["//src:config",
    	     "//src/comm/api",
    	     "//src/comm/common",
    	     "//src/comm/interface",
             "//src/logger",
             "//src/portability",
             "//src/util",
             "//third_party/coap",
             "//third_party/mbedtls",
             "//third_party/mbedtls:mbedtls-crypto",
             "//third_party/mbedtls:mbedtls-x509"],
    hdrs = ["caadapterutils.h",
            "ca_adapter_net_ssl.h"],
    srcs  = ["ca_adapter_net_ssl.c",
             # "cafragmentation.c", # ble only?
             # "cafragmentation.h", # ble only?
             "caadapterutils.c"],
    visibility = ["//src/comm:__subpackages__",
                  "//src/sec:__pkg__"]
)

config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)
