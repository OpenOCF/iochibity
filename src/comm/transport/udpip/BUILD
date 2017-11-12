config_setting(
    name = "darwin",
    values = { "cpu": "darwin_foo" }
)
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
config_setting(
    name = "ndk",
    values = { "crosstool_top": "//platforms/ndk:toolchain" }
)
config_setting(
    name = "android",
    # values = { "crosstool_top": "//external:android/crosstool."} # "@android_ndk//:toolchain-libcpp"
    values = {"cpu": "armeabi-v7a"}
)

cc_library(
    name = "hdrs",
    copts = ["-Isrc",
             "-Iexternal/openocf/src/",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
    	     "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
    	     "-Isrc/comm/transport/util",
             "-Iexternal/openocf/src/comm/transport/util",
    	     "-Isrc/comm/api",
             "-Iexternal/openocf/src/comm/api",
    	     "-Isrc/comm/common",
             "-Iexternal/openocf/src/comm/common",
    	     "-Isrc/comm/interface",
             "-Iexternal/openocf/src/comm/interface",
    	     "-Isrc/comm/security",
             "-Iexternal/openocf/src/comm/security",
	     "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/api",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
	     "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
	     "-Ithird_party/coap/include",
	     "-Iexternal/openocf/third_party/coap/include"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
            "//src/util",
    	    "//src/comm/api",
    	    "//src/comm/common",
    	    "//src/comm:hdrs",
    	    "//src/comm/interface",
            "//src/comm/security",
    	    "//src/comm/transport/util",
            "//src/portability",
	    "//third_party/coap"],
    hdrs = ["caipadapter.h",
            "caipnwmonitor.h",
            "caipserver.h"],
    visibility = ["//src/comm:__subpackages__"]
)

cc_library(
    name = "udpip",
    copts = ["-Isrc",
             "-Iexternal/openocf/src/",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
    	     "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
    	     "-Isrc/comm/transport/util",
             "-Iexternal/openocf/src/comm/transport/util",
    	     "-Isrc/comm/api",
             "-Iexternal/openocf/src/comm/api",
    	     "-Isrc/comm/common",
             "-Iexternal/openocf/src/comm/common",
    	     "-Isrc/comm/interface",
             "-Iexternal/openocf/src/comm/interface",
    	     "-Isrc/comm/security",
             "-Iexternal/openocf/src/comm/security",
	     "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/api",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
	     "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
	     "-Ithird_party/coap/include",
	     "-Iexternal/openocf/third_party/coap/include"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
            "//src/util",
    	    "//src/comm/api",
    	    "//src/comm/common",
    	    "//src/comm:hdrs",
    	    "//src/comm/interface",
            "//src/comm/security",
    	    "//src/comm/transport/udpip/posix",
    	    "//src/comm/transport/util",
            "//src/portability",
	    "//third_party/coap",
            ]
    + select({#":windows": ["windows/caipnwmonitor.c"],
              #":msvc": ["windows/caipnwmonitor.c"],
              #":msys": ["linux/caipnwmonitor.c"],
              ":darwin": ["//src/comm/transport/udpip/darwin"],
              # ":android": ["//src/comm/transport/udpip/android"],
              # ":ndk": ["//src/comm/transport/udpip/android"],
              # ":linux": ["//src/comm/transport/udpip/linux"],
              "//conditions:default": []}),
    hdrs = ["caipadapter.h",
            "caipnwmonitor.h",
            "caipserver.h"],
    srcs = ["caipadapter.c"],
            # "caipserver.c"],
    # + select({#":windows": ["windows/caipnwmonitor.c"],
    #     #":msvc": ["windows/caipnwmonitor.c"],
    #     #":msys": ["linux/caipnwmonitor.c"],
    #     # ":darwin": ["//src/comm/transport/udpip/darwin"],
    #     # ":ndk": ["//src/comm/transport/udpip/android"],
    #     # ":android": ["//src/comm/transport/udpip/android"],
    #     "//conditions:default": ["caipnwmonitor.c"]}),
    visibility = ["//src/comm:__subpackages__"]
)
