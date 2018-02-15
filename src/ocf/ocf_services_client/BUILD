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

cc_library(
    name = "ocf_services_client",
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
             "-Iexternal/openocf/src/portability",
             "-Isrc/util",
             "-Iexternal/openocf/src/util"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              ":darwin": ["-std=c11", "-U DEBUG"],
              ":linux": ["-std=c11"],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            # "//src/comm/api",
            # "//src/ocf:hdrs",
            "//src/logger",
            "//src/util",
            "//src/portability"],
    # hdrs = ["client/co_service_provider.h",
    #         "client/co_service_provider_mgr.h"],
    # srcs = ["ocf_services_client/co_service_provider.c",
    #         "ocf_services_client/co_service_provider_mgr.c"],
    srcs = ["coresource_db_mgr.c",
            "coresource_db_mgr.h",
            "coresource_mgr.c",
            "coresource_mgr.h"],
    visibility = ["//visibility:public"]
)
