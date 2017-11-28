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
    name = "util",
    alwayslink = True,
    copts = ["-D_GNU_SOURCE",
     "-DWITH_POSIX"
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps  = ["//src:config",
    	     # "//src/comm",
             "//src/logger",
             "//src/portability",
             "//src/util",
             "//third_party/coap",
             "//third_party/mbedtls",
             "//third_party/mbedtls:mbedtls-crypto",
             "//third_party/mbedtls:mbedtls-x509"],
    hdrs = ["caadapterutils.h",
            #"ca_adapter_net_ssl.h"
    ],
    srcs  = [#"ca_adapter_net_ssl.c",
             "caadapterutils.c",
    ],
    visibility = ["//visibility:public"]
# "//src/comm:__subpackages__",
#                   "//src/sec/svrs:__pkg__"]
)
