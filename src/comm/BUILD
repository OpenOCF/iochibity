
# cc_library(
#     name = "cacommon",
#     copts = ["-std=c11", "-Isrc/comm/util"],
#     srcs  = ["common/cacommon.h"])

cc_library(
    name = "comm",
    copts = ["-std=c11",
    	     "-Isrc",
             "-Isrc/comm",
             "-Isrc/comm/api",
	     "-Isrc/comm/common",
	     "-Isrc/comm/include", "-Isrc/comm/util",
             "-Isrc/comm/network",
             "-Isrc/portability",
             #"-Isrc/sec",
             "-Isrc/util",
             "-Ithird_party/coap",
             "-Iexternal/mbedtls/mbedtls-2.4.2/include"],
    deps = ["//src:config",
    	    "//src/portability",
	    "//src/comm:shared",
            #"//src/comm:util",
            #"//src/comm/network:ip",
            "//third_party/coap",
            "@mbedtls//:mbedtls-crypto",
            "@mbedtls//:mbedtls-lib",
            "@mbedtls//:mbedtls-x509"],
    #hdrs = ["include/casecurityinterface.h"],
    srcs = [
        "adapter_util/caadapterutils.c",
        "adapter_util/cafragmentation.c", # transport ALL or BLE
        "adapter_util/ca_adapter_net_ssl.c", # not tizen or no tls path

        "cablockwisetransfer.c", # transport ALL or IP
        "cablockwisetransfer.h",

        "caconnectivitymanager.c",
        "api/cainterface.h",

        "cainterfacecontroller.c",
	"cainterfacecontroller.h",

	"camessagehandler.c",
	"camessagehandler.h",


	"canetworkconfigurator.c",
        "canetworkconfigurator.h",

	"caprotocolmessage.c",
	"caprotocolmessage.h",

        "caqueueingthread.c",
        "caqueueingthread.h",

	"caretransmission.c",
	"caretransmission.h"],
    visibility = ["//src/comm/network:__pkg__",
                  "//src/ocf:__pkg__",
                  "//src/sec:__pkg__"]
)

