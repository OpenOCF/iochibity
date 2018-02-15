config_setting(
    name = "linux",
    values = { "cpu": "k8" }
)
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
             "-Iexternal/openocf/third_party/coap",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include"
    ] + select({":windows": [],
                    ":msvc": [],
                    ":msys": [],
                    ":darwin":["-std=c11", "-U DEBUG"],
                    "//conditions:default": ["-x c", "-std=c11"]}),
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
            "//third_party/coap"
    ] + select({":client_role": ["//src/ocf/ocf_services_client"],
              "//conditions:default": []}),
    linkshared = 1,
    visibility = ["//visibility:public"]
)

cc_library(
    name = "Xopenocf",
    copts = ["-Ithird_party/coap",
             "-Iexternal/openocf/third_party/coap",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include"
    ] + select({":windows": [],
                ":msvc": [],
                ":msys": [],
                ":darwin": ["-std=c11", "-U DEBUG"],
                "//conditions:default": []}),
    linkstatic = 1,
    # alwayslink = True,
    deps = ["//src:config",
            "//src/ocf",
            "//src/logger",
            "//src/comm",
            "//src/portability",
            "//src/provisioning",
            "//src/sec",
            "//third_party/coap"]
    + select({":client_role": ["//src/ocf/ocf_services_client"],
              "//conditions:default": []}),
    visibility = ["//visibility:public"]
)

cc_library(
    name = "ocf",
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
             "-Ithird_party/coap",
             "-Iexternal/openocf/third_party/coap",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/tinycbor/src",
             "-Iexternal/openocf/third_party/tinycbor/src",
             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist"
    ] + select({":windows": [],
                ":msvc": [],
                ":msys": [],
                "linux": ["-std=c11", "-DHAVE_CONFIG_H", "-D_GNU_SOURCE", "-DWITH_POSIX"], # for building coap
                ":darwin": ["-std=c11", "-U DEBUG"],
                "//conditions:default": ["-x c", "-std=c11"]}),
    deps = ["//src:config",
            "//src/comm",       # connectivity
            "//src/logger",
            "//src/portability",
            "//src/provisioning",
            "//src/routing",
            "//src/sec",
            "//src/util",       # c_common
            "//third_party/coap",
            "//third_party/tinycbor",
            "//third_party/cjson",
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
