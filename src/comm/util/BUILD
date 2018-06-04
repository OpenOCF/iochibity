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
    copts = ["-Isrc",
             "-Iexternal/openocf/src/",
             "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
             "-Isrc/comm/transport/util",
             "-Iexternal/openocf/src/comm/transport/util",
             "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/util",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
    	     "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
	     "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Iexternal/libcoap/include",
             "-Iexternal/openocf/third_party/coap/include"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = ["//src:config",
            "//src/comm/transport/util",
            "//src/logger",
    	    "//src/portability",
            "//src/util",
            "@libcoap//:libcoap"],
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//src/comm:__subpackages__"]
)
