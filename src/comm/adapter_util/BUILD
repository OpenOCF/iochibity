cc_library(
    name = "adapter_util",
    alwayslink = True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/comm/api",  # cacommon.h
             "-Iexternal/openocf/src/comm/api",  # cacommon.h
             "-Isrc/comm/common",  # cacommonutil.h
             "-Iexternal/openocf/src/comm/common",  # cacommonutil.h
             "-Isrc/comm/interface",
             "-Iexternal/openocf/src/comm/interface",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             #"-Ithird_party/coap/include/coap",
             # "-Ithird_party/mbedtls",
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
