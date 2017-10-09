config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)


cc_library(
    name = "util",
    copts = ["-Isrc",
             "-Iexternal/openocf/src/",
             "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
             "-Isrc/comm/api",
             "-Iexternal/openocf/src/comm/api",
             "-Isrc/comm/adapter_util",
             "-Iexternal/openocf/src/comm/adapter_util",
             "-Isrc/comm/common",
             "-Iexternal/openocf/src/comm/common",
             "-Isrc/comm/interface",
             "-Iexternal/openocf/src/comm/interface",
             "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/util",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
    	     "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
	     "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/comm/adapter_util",
    	    "//src/comm/common",
    	    "//src/comm/interface",
            "//src/logger",
    	    "//src/portability",
            "//src/util",
            "//third_party/coap"],
    hdrs = ["cautilinterface.h",
            "commdefs.h"],
    srcs = ["cautilinterface.c",
            "camanagerleinterface.h",
            "cabtpairinginterface.h"],
# if with_cloud and with_tcp:
# "caconnectionmanager.c",
# "camanagerutil.c",
# "camessagearbiter.c",
# "capolicymanager.c"
    visibility = ["//src/comm:__subpackages__"]
)
