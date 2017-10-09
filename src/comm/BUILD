config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)


cc_library(
    name = "hdrs",
    alwayslink = True,
    linkstatic = True,
    copts = ["-Isrc"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    hdrs = ["caipadapter.h",
    "caipnwmonitor.h",
    "caqueueingthread.h"],
    visibility = ["//src/comm/network:__pkg__"]
)

cc_library(
    name = "ifcontrol",
    alwayslink = True,
    linkstatic = True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
             "-Isrc/comm/adapter_util",
             "-Iexternal/openocf/src/comm/adapter_util",
             "-Isrc/comm/api",
             "-Iexternal/openocf/src/comm/api",
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
            "//src/comm/api",
            "//src/comm/common",
            "//src/comm:hdrs",
            "//src/comm/interface",
            "//src/comm/util",
            "//src/logger",
            "//src/portability",
            "//src/util",
            "//third_party/coap"],
    srcs  = ["cainterfacecontroller.c",
	     "cainterfacecontroller.h",
             # "caipnwmonitor.h",
             "caedradapter.h",
             "caleadapter.h",
             "canfcadapter.h",
             # "caipadapter.h",
             # "caqueueingthread.h",
             "canetworkconfigurator.h"],
    visibility = ["//src/comm/network:__pkg__"]
)

cc_library(
    name = "comm",
    alwayslink = True,
    linkstatic = True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
             "-Isrc/comm/api",
             "-Iexternal/openocf/src/comm/api",
             "-Isrc/comm/adapter_util",
             "-Iexternal/openocf/src/comm/adapter_util",
	     "-Isrc/comm/common",
             "-Iexternal/openocf/src/comm/common",
	     "-Isrc/comm/include",
             "-Iexternal/openocf/src/comm/include",
	     "-Isrc/comm/interface",
             "-Iexternal/openocf/src/comm/interface",
             "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/util",
             "-Isrc/comm/network",
             "-Iexternal/openocf/src/comm/network",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/mbedtls/include"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
    	    "//src/portability",
            "//src/comm/adapter_util",
	    "//src/comm/common",
	    "//src/comm:ifcontrol",
	    "//src/comm/interface",
	    # "//src/comm:shared",
            "//src/comm/network:ip",
            "//third_party/coap",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509"],
    #hdrs = ["include/casecurityinterface.h"],
    srcs = [
        # "adapter_util/caadapterutils.c",
        # "adapter_util/cafragmentation.c", # transport ALL or BLE
        # "adapter_util/ca_adapter_net_ssl.c", # not tizen or no tls path

        "cablockwisetransfer.c", # transport ALL or IP
        "cablockwisetransfer.h",

        "caconnectivitymanager.c",


        "caedradapter.h",
        "caleadapter.h",
        "canfcadapter.h",
        "caraadapter.h",
        "caipadapter.h",

	"camessagehandler.c",
	"camessagehandler.h",


	"canetworkconfigurator.c",
        "canetworkconfigurator.h",

	"caprotocolmessage.c",

        "caqueueingthread.c",
        "caqueueingthread.h",

	"caretransmission.c",
	"caretransmission.h"],
    visibility = ["//src/comm/network:__pkg__",
                  "//src/ocf:__pkg__",
                  "//src/sec:__pkg__",
                  "//src/provisioning:__pkg__"]
)
