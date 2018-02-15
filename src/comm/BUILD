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
    name = "comm",
    # alwayslink = True,
    linkstatic = 1,
    copts = ["-Isrc",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/coap",
             "-Iexternal/openocf/third_party/coap",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist",
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = ["//src:config",
            "//third_party/coap",
            "//src/logger",
    	    "//src/portability",
            "//src/comm/transport/util",
            "//src/comm/util",
            # FIXME: select for this
            "//src/comm/transport/udpip",
            "//third_party/utlist",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509"],
    srcs = glob(["*.c"], exclude=["caadapternetdtls.c"])
    + glob(["*.h"], exclude=["caadapternetdtls.h"]),
    visibility = ["//visibility:public"]
)
