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
    name = "hdrs",
    alwayslink = True,
    linkstatic = True,
    copts = ["-Isrc"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    hdrs = ["caqueueingthread.h"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "ifcontrol",
    alwayslink = True,
    linkstatic = True,
    copts = [#"-Isrc",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/utlist",
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = ["//src:config",
            "//src/comm/transport/util",
            # "//src/comm/api",
            # "//src/comm:hdrs",
            # "//src/comm/interface",
            # "//src/comm/transport/bredr", # FIXME: select for this
            # "//src/comm/transport/ble", # FIXME: select for this
            # "//src/comm/transport/nfc", # FIXME: select for this
            # "//src/comm/transport/tcpip", # FIXME: select for this
            "//src/comm/transport/udpip", # FIXME: select for this
            "//src/comm/util",
            "//src/logger",
            "//src/portability",
            "//src/util",
            "//third_party/coap",
            "//third_party/utlist",
    ],
    srcs  = ["cainterfacecontroller.c",
	     "cainterfacecontroller.h",
             "canetworkconfigurator.h"
    ],
    # visibility = ["//src/comm/transport:__subpackages__"]
)

cc_library(
    name = "comm",
    # alwayslink = True,
    linkstatic = 1,
    copts = ["-Isrc",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
    #          "-Iexternal/openocf/third_party/coap/include",
    #          "-Ithird_party/mbedtls/include"
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
	    "//src/comm:ifcontrol",
	    # "//src/comm/interface",
	    # "//src/comm/security",
            # FIXME: select for this
            "//src/comm/transport/udpip",
            # "//third_party/utlist",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509"],
    # hdrs = ["casecurityinterface.h"],
    # linkopts = ["-Lthird_party/coap",
    #             "-lcoap"],
    srcs = ["caadapterinterface.c",
            "caadapterinterface.h",

            # not used:
            # "caadapternetdtls.c",
            # "caadapternetdtls.h",

            "ca_adapter_net_ssl.c",
            "ca_adapter_net_ssl.h",
            "cablockwisetransfer.c", # transport ALL or IP
            "cablockwisetransfer.h",
            "caconnectivitymanager.c",
            "caconnectivitymanager.h",
            # "caedradapter.h",
            # "caleadapter.h",
            # "canfcadapter.h",
            # "caraadapter.h",
	    "camessagehandler.c",
	    "camessagehandler.h",
	    "canetworkconfigurator.c",
            "canetworkconfigurator.h",
	    "caprotocolmessage.c",
	    "caprotocolmessage.h",
            "caqueueingthread.c",
            "caqueueingthread.h",
            "caremotehandler.c",
            "caremotehandler.h",
	    "caretransmission.c",
	    "caretransmission.h",
            "cathreadpool_pthreads.c",
            "cathreadpool_pthreads.h"],
    visibility = ["//visibility:public"]
)
