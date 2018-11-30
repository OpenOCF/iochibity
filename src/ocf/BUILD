load("//config:variables.bzl", "CSTD", "DEFINES")

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
    copts = ["-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
    ] + select({"//config:darwin":["-U DEBUG"],
                "//conditions:default": []})
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
             "-Iexternal/libcoap/include",
             # "-Iexternal/libcoap/include/coap",
             "-Iexternal/tinycbor/src",
             "-Iexternal/utlist/src",
    ] + select({"//config:windows": [],
                "//config:msvc": [],
                "//config:msys": [],
                "@//config:android_arm8": ["-std=c11", "-DHAVE_CONFIG_H", "-D_GNU_SOURCE", "-DWITH_POSIX"], # for building coap
                "//config:linux": ["-DHAVE_CONFIG_H", "-D_GNU_SOURCE", "-DWITH_POSIX"], # for building coap
                "//config:darwin": ["-U DEBUG"],
                "//conditions:default": ["BROKEN"]})
    + CSTD,

    defines = DEFINES,

    deps = ["//src:config",
            "//src/comm",
            "//src/logger",
            "//src/portability",
            "//src/provisioning",
            "//src/sec",
            "//src/util",
            "//src/coap",
            "@cjson//:cjson",
            "@libcoap//:libcoap",
            "@tinycbor//:tinycbor",
	    "@utlist//:utlist"]
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
