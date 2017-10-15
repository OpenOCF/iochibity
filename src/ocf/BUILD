cc_library(
    name = "ocf",
    #linkstatic = 1,
    deps = ["//src/ocf:interface",
            "//src/ocf:hdrs",
            "//src/ocf:impl",
            "//src/comm",
            "//src/provisioning",
            "//src/sec"
    ],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "hdrs",
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/ocf",
             "-Iexternal/openocf/src/ocf",],
    deps = ["//src:config"
    ],
    hdrs = ["octypes.h",
            "ocstack.h",
            "ocpayload.h",
            "ocpayloadcbor.h",
            "ocpresence.h",
            "presence_methods.h",
            "payload_logging.h"],
    srcs = ["ocserverrequest.h",
            "ocstackinternal.h", # included by sec/doxmresource.c
            "ocstackconfig.h",   # included by ocstackinternal.h
            "occlientcb.h",      # included by ocstackinternal.h
            "occlientcb_api.h",      # included by ocstackinternal.h
            "ocresource.h",      # included by occlientcb.h
            "ocobserve.h",       # included by ocobserve.h
            "ocresourcehandler.h", # included by sec/secureresourcemanager.c
            "tree.h"],
    visibility = ["//visibility:public"]
    # visibility = ["//src/sec:__pkg__",
    #               "//src/ocf/stack:__pkg__",
    #               "//src/provisioning:__pkg__",
    #               "//src/sec:__subpackages__"]
)

cc_library(
    name = "interface",
    copts = ["-Isrc/comm/api",
    #          "-Isrc/comm/common",
    #          "-Isrc/comm/include",
    #          "-Isrc/comm/util",
    #          "-Isrc/ocf",
    #          "-Isrc/ocf/internal",
    #          "-Isrc/util",
             "-Isrc/sec/ocsecurity",
    #          "-Ithird_party/coap",
    #          "-Ithird_party/cbor"],
    ],
    deps = ["//src/comm/api",
    #         "//src/util",       # c_common
    #         "//third_party/cbor",
    #         "//third_party/cjson",
    #         "//src/routing",
    #         "//src/comm"],
            "//src/sec:interface",
    #         #"//src/provisioning"],
    ],
    hdrs = ["ocpayload.h",      # sec/psinterface.c
            "payload_logging.h",
            # "ocstack.h",
            # "ocstackconfig.h",
            "ocpresence.h",
            "presence_methods.h",

    ],
    srcs = ["ocpayloadcbor.h",
            "occlientcb.h",
            "occlientcb_api.h",
            "ocobserve.h",
            "ocresource.h",
            "ocresourcehandler.h",
            "ocserverrequest.h", # sec/aclresource.c
            "ocstack.h",
            "ocstackinternal.h",
            "octypes.h",
            "tree.h"],
    visibility = ["//src/sec:__pkg__",
                  "//src/ocf:__pkg__",
                  "//src/provisioning:__pkg__",
                  "//src/sec:__subpackages__"]
)

cc_library(
    name = "impl",
    alwayslink = True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
             "-Isrc/comm/api",
             "-Iexternal/openocf/src/comm/api",
             "-Isrc/comm/common",
             "-Iexternal/openocf/src/comm/common",
             "-Isrc/comm/include",
             "-Iexternal/openocf/src/comm/include",
             "-Isrc/comm/interface",
             "-Iexternal/openocf/src/comm/interface",
             "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/util",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/ocf",
             "-Iexternal/openocf/src/ocf",
             "-Isrc/sec",
             "-Iexternal/openocf/src/sec",
             "-Isrc/sec/aclroles",
             "-Iexternal/openocf/src/sec/aclroles",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/tinycbor/src",
             "-Iexternal/openocf/third_party/tinycbor/src"],
    deps = ["//src:config",
            "//src/ocf:interface",
            "//src/comm/api",
            "//src/comm/common",
            "//src/comm/interface",
            "//src/logger",
            "//src/portability",
            # "//src/sec/aclroles",
            # "//src/sec:doxm",
            # "//src/sec:interface",
            "//src/sec",
            "//src/util",       # c_common
            "//third_party/tinycbor",
            "//third_party/cjson",
            "//src/routing",
            "//src/comm",       # connectivity
            "//src/provisioning"],
    srcs = ["occlientcb.c",
            "occollection.c",
            "occollection.h",

            # if --with-cloud:
            # "occonnectionmanager.c",
            # "occonnectionmanager.h",

            "ocendpoint.c",
            "ocendpoint.h",
            "ocobserve.c",
            "ocpayload.c",
            #"ocpayload.h",
            "ocpayloadconvert.c",
            "ocpayloadparse.c",
            "ocresource.c",
            "ocserverrequest.c",
            #"ocserverrequest.h",
            "ocstack.c",
            "ocstack.h",
            "oicgroup.c",
            "oicgroup.h",
            "oickeepalive.c",
            "oickeepalive.h",
            "presence.c",
            "ocpresence.h",
            "presence_methods.h",

            # if SERVER in rd_mode:
            # "oicresourcedirectory.c",
            # "oicresourcedirectory.h",

            "payload_logging.h"
    ]

    # if windows, and UWP_APP = 1:
    # "ocsqlite3helper.c"
    # "ocsqlite3helper.h"
)
