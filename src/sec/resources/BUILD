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
    name = "pbkdf2",
    srcs = ["pbkdf2.c",
            "pbkdf2.h"]
)

cc_library(
    name = "resources",
    # alwayslink = 1,
    linkstatic = True,
    copts = ["-Isrc",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/mbedtls/include",
             "-Iexternal/openocf/third_party/mbedtls/include",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             "-Ithird_party/tinycbor/src",
             "-Iexternal/openocf/third_party/tinycbor/src",
             "-Ithird_party/utlist",
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = ["//src:config",
            "//src/comm/transport/util",
            # "//src/comm/api",
            # "//src/comm/common",
            # "//src/comm/interface",
            # "//src/comm/security",
            "//src/logger",
            "//src/portability",
            "//src/sec/authorization",
            "//src/sec:helpers",
            "//src/sec:strings",
            "//src/util",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509",
            "//third_party/tinycbor",
            "//third_party/utlist",
    ],
    linkopts = select({":msys": ["-L:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lcrypt32"],
                       ":msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 crypt32.lib"],
                       ":windows": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 crypt32.lib"],
              "//conditions:default": []}),
    srcs = glob(["*.c"], exclude = ["_events.c"])
     + glob(["*.h"]),
    # srcs = ["aclresource.c",
    #         "aclresource.h",

    #         "amaclresource.c",
    #         "amaclresource.h",

    #         "credresource.c",
    #         "credresource.h",

    #         "doxmresource.c",
    #         "doxmresource.h",

    #         # if env.get('DTLS_WITH_X509') == '1':  # and env.get('SECURED'):
    #         "crlresource.c",    # SECURED=1
    #         "crlresource.h",    # SECURED=1

    #         "csrresource.c",
    #         "csrresource.h",

    #         "pstatresource.c",
    #         "pstatresource.h",

    #         "rolesresource.c",
    #         "rolesresource.h",

    #         "resourcemanager.c",
    #         "resourcemanager.h",

    #         "secureresourcemanager.c",
    #         "secureresourcemanager.h",
    # ],
    visibility = ["//visibility:public"]
)

