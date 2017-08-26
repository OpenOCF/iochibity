config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)


cc_library(
    name = "ifcontrol",
    copts = ["-Isrc",
             "-Isrc/comm/adapter_util",
             "-Isrc/comm/api",
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
            "//src/comm/adapter_util:utils",
            "//src/comm/api",
            "//src/comm/common",
            "//src/comm/interface",
            "//src/comm/util",
            "//src/logger",
            "//src/portability",
            "//src/util",
            "//third_party/coap"],
    srcs  = ["cainterfacecontroller.c",
	     "cainterfacecontroller.h",
             "caedradapter.h",
             "caleadapter.h",
             "canfcadapter.h",
             "caipadapter.h",
             "caqueueingthread.h",
             "canetworkconfigurator.h"]
)

cc_library(
    name = "comm",
    alwayslink = True,
    copts = ["-Isrc",
             "-Isrc/comm",
             "-Isrc/comm/api",
             "-Isrc/comm/adapter_util",
	     "-Isrc/comm/common",
	     "-Isrc/comm/include",
	     "-Isrc/comm/interface",
             "-Isrc/comm/util",
             "-Isrc/comm/network",
             "-Isrc/logger",
             "-Isrc/portability",
             #"-Isrc/sec",
             "-Isrc/util",
             "-Ithird_party/coap/include",
             "-Ithird_party/mbedtls/include"]
    + select({"windows": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/logger",
    	    "//src/portability",
            "//src/comm/adapter_util:utils",
	    "//src/comm/common",
	    "//src/comm:ifcontrol",
	    "//src/comm/interface",
	    # "//src/comm:shared",
            #"//src/comm:util",
            #"//src/comm/network:ip",
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

