config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)


cc_library(
    name = "util",
    copts = ["-Isrc",
             "-Isrc/comm",
             "-Isrc/comm/api",
             "-Isrc/comm/adapter_util",
             "-Isrc/comm/common",
             "-Isrc/comm/interface",
             "-Isrc/comm/util",
             "-Isrc/logger",
    	     "-Isrc/portability",
	     "-Isrc/util",
             "-Ithird_party/coap/include"]
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
