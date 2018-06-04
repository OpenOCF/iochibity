load("//config:variables.bzl", "CSTD")

config_setting(
    name = "client_role",
    values = { "define": "role=client" }
)
config_setting(
    name = "server-role",
    values = { "define": "role=server" }
)
config_setting(
    name = "client-server-role",
    values = { "define": "role=clientserver" }
)

cc_binary(
    # name = "libopenocf.dylib",
    name = "libopenocfa.so",
    linkshared = 1,
    copts = ["-Iconfig/darwin",
             "-Iconfig/darwin/coap",
             "-Iexternal/libcoap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
    ] + select({"//config:windows": [],
                "//config:msvc": [],
                "//config:msys": [],
                "//config:darwin":["-std=c11", "-U DEBUG"],
                "//conditions:default": ["-x c", "-std=c11"]})
    + CSTD,
    deps = ["//src:config",
            # "//src/ocf:interface",
            # "//src/ocf:hdrs",
            "//src/ocf",
            "//src/logger",
            "//src/comm",
            # "//src/comm/api",
            "//src/portability",
            "//src/provisioning",
            "//src/sec",
            "@libcoap//:libcoap"
    ] + select({":client_role": ["//src/ocf/ocf_services_client"],
              "//conditions:default": []}),
    visibility = ["//visibility:public"]
)

cc_library(
    name = "ocf",
    alwayslink = True,
    linkstatic=True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
             # "-Isrc/comm/api",
             # "-Iexternal/openocf/src/comm/api",
             # "-Isrc/comm/common",
             # "-Iexternal/openocf/src/comm/common",
             # "-Isrc/comm/interface",
             # "-Iexternal/openocf/src/comm/interface",
             "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/util",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             # "-Isrc/ocf/client",
             # "-Iexternal/openocf/src/ocf/client",
             "-Isrc/ocf",
             "-Iexternal/openocf/src/ocf",
             "-Isrc/sec",
             "-Iexternal/openocf/src/sec",
             "-Isrc/sec/aclroles",
             "-Iexternal/openocf/src/sec/aclroles",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Iconfig/darwin",
             "-Iconfig/darwin/coap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
             "-Iexternal/tinycbor/src",
             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist"
    ] + select({"//config:windows": [],
                "//config:msvc": [],
                "//config:msys": [],
                "//config:android": ["-std=c11", "-DHAVE_CONFIG_H", "-D_GNU_SOURCE", "-DWITH_POSIX"], # for building coap
                "//config:linux": ["-std=c11", "-DHAVE_CONFIG_H", "-D_GNU_SOURCE", "-DWITH_POSIX"], # for building coap
                "//config:darwin": ["-std=c11", "-U DEBUG"],
                "//conditions:default": ["-x c", "-std=c11"]}),
    deps = ["//src:config",
            "//src/comm",       # connectivity
            "//src/logger",
            "//src/portability",
            "//src/provisioning",
            "//src/routing",
            "//src/sec",
            "//src/util",       # c_common
            "@libcoap//:libcoap",
            "@tinycbor//:tinycbor",
            #"@cjson//:cjson",
            #"//third_party/cjson",
	    "//third_party/utlist"]
    + select({":client_role": ["//src/ocf/ocf_services_client"],
              "//conditions:default": []}),
    # hdrs = select({":client_role": ["client/co_service_provider_mgr.h"],
    #           "//conditions:default": []}),
    # FIXME: enable occonnectionmanager if cloud is enabled
    # FIXME: presence support
    srcs = glob(["*.c"], exclude=["occonnectionmanager.c"]) # , "presence.c"])
         + glob(["*.h"], exclude=["occonnectionmanager.h"]), # "presence.h"]),
    # if windows, and UWP_APP = 1:
    # "ocsqlite3helper.c"
    # "ocsqlite3helper.h"
    visibility = ["//visibility:public"]
)
