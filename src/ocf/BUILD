config_setting(
    name = "darwin",
    values = { "cpu": "darwin" }
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
    name = "libopenocf.so",
    copts = ["-Ithird_party/coap",
             "-Ithird_party/coap/include"
    ] + select({":windows": [],
                    ":msvc": [],
                    ":msys": [],
                    ":darwin":["-std=c11", "-U DEBUG"],
                    "//conditions:default": []}),
    deps = ["//src:config",
            # "//src/ocf:interface",
            # "//src/ocf:hdrs",
            "//src/ocf:impl",
            "//src/logger",
            "//src/comm",
            # "//src/comm/api",
            "//src/portability",
            "//src/provisioning",
            "//src/sec",
            "//third_party/coap"
    ] + select({":client_role": ["//src/ocf:client"],
              "//conditions:default": []}),
    linkshared = 1,
    visibility = ["//visibility:public"]
)

cc_library(
    name = "ocf",
    copts = ["-Ithird_party/coap",
             "-Ithird_party/coap/include"
    ] + select({":windows": [],
                ":msvc": [],
                ":msys": [],
                ":darwin": ["-std=c11", "-U DEBUG"],
                "//conditions:default": []}),
    linkstatic = 1,
    # alwayslink = True,
    deps = ["//src:config",
            # "//src/ocf:interface",
            # "//src/ocf:hdrs",
            "//src/ocf:impl",
            "//src/logger",
            "//src/comm",
            # "//src/comm/api",
            "//src/portability",
            "//src/provisioning",
            "//src/sec",
            "//third_party/coap"]
    + select({":client_role": ["//src/ocf:client"],
              "//conditions:default": []}),
    visibility = ["//visibility:public"]
)

cc_library(
    name = "client",
    linkstatic = 1,
    # alwayslink = True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             # "-Isrc/comm/api",
             # "-Iexternal/openocf/src/comm/api",
             "-Isrc/ocf",
             "-Iexternal/openocf/src/ocf",
             "-Isrc/ocf/client",
             "-Iexternal/openocf/src/ocf/client",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
             "-Isrc/portability",
             "-Iexternal/openocf/src/portability"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              ":darwin": ["-std=c11", "-U DEBUG"],
              "//conditions:default": []}),
    deps = ["//src:config",
            # "//src/comm/api",
            # "//src/ocf:hdrs",
            "//src/logger",
            # "//src/portability:hdrs",
            "//src/portability"],
    hdrs = ["client/co_service_provider.h",
            "client/co_service_provider_mgr.h"],
    srcs = ["client/co_service_provider.c",
            "client/co_service_provider_mgr.c"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "impl",
    alwayslink = True,
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
             "-Isrc/ocf/client",
             "-Iexternal/openocf/src/ocf/client",
             "-Isrc/ocf",
             "-Iexternal/openocf/src/ocf",
             "-Isrc/sec",
             "-Iexternal/openocf/src/sec",
             "-Isrc/sec/aclroles",
             "-Iexternal/openocf/src/sec/aclroles",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             # "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/tinycbor/src",
             "-Iexternal/openocf/third_party/tinycbor/src",
             "-Ithird_party/utlist"
    ] + select({":windows": [],
                ":msvc": [],
                ":msys": [],
                ":darwin": ["-std=c11", "-U DEBUG"],
                "//conditions:default": []}),
    deps = ["//src:config",
            "//src/logger",
            "//src/portability",
            "//src/sec",
            "//src/util",       # c_common
            "//third_party/coap",
            "//third_party/tinycbor",
            "//third_party/cjson",
            "//src/routing",
            "//src/comm",       # connectivity
            "//src/provisioning",
	    "//third_party/utlist"]
    + select({":client_role": ["//src/ocf:client"],
              "//conditions:default": []}),
    hdrs = select({":client_role": ["client/co_service_provider_mgr.h"],
              "//conditions:default": []}),
    # FIXME: enable occonnectionmanager if cloud is enabled
    srcs = glob(["*.c"], exclude=["occonnectionmanager.c"])
         + glob(["*.h"], exclude=["occonnectionmanager.h"]),
    # if windows, and UWP_APP = 1:
    # "ocsqlite3helper.c"
    # "ocsqlite3helper.h"
)
