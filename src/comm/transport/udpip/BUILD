config_setting(
    name = "windows",
    # compiler: cl.exe, from msvc
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
    name = "ndk",
    values = { "crosstool_top": "//platforms/ndk:toolchain" }
)
config_setting(
    name = "darwin",
    values = { "cpu": "darwin" }
)
config_setting(
    name = "darwin64",
    values = { "cpu": "darwin_x86_64" }
)
config_setting(
    name = "android",
    # values = { "crosstool_top": "//external:android/crosstool."} # "@android_ndk//:toolchain-libcpp"
    values = {"cpu": "armeabi-v7a"}
)

cc_library(
    name = "udpip",
    copts = ["-Isrc",
             "-Isrc/util",
             "-Ithird_party/coap/include",
             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/coap/include"
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-UDEBUG"]}), # OS X
    linkstatic = 1,
    deps = ["//src:config",
            "//src/logger",
            "//src/util",
    	    # "//src/comm:hdrs",
            # "//src/comm/security",
    	    "//src/comm/transport/util",
            "//src/portability",
	    "//third_party/coap",
	    "//third_party/utlist"],
    # ] + select({":windows": ["//src/portability/windows"],
    #           #":msvc": ["windows/caipnwmonitor.c"],
    #           #":msys": ["linux/caipnwmonitor.c"],
    #           ":darwin64": ["//src/portability/posix",
    #                         "//src/portability/darwin"],
    #           # ":android": ["//src/comm/transport/udpip/android"],
    #           # ":ndk": ["//src/comm/transport/udpip/android"],
    #           # ":linux": ["//src/comm/transport/udpip/linux"],
    #           "//conditions:default": ["//src/portability/posix",
    #                                    "//src/portability/darwin"]}),
    linkopts = select({":msys": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lwinsock2", "-liphlpapi", "-lws2_32"],
                       ":msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 iphlpapi.lib ws2_32.lib"],
                       ":windows": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 iphlpapi.lib ws2_32.lib"],
                       "//conditions:default": []}),
    srcs = ["caipadapter0.c",
            "caipadapter0.h",
            "caipnwmonitor0.c",
            "caipnwmonitor0.h",
            "caipserver0.c",
            "caipserver0.h"
    ] + select({":msvc": [#"caipadapter_windows.c",
                             #"caipadapter_windows.h",
                             "caipnwmonitor_windows.c",
                             "caipnwmonitor_windows.h",
                             "caipserver_windows.c",
                             "caipserver_windows.h"],
                ":msys": [#"caipadapter_windows.c",
                             #"caipadapter_windows.h",
                             "caipnwmonitor_windows.c",
                             "caipnwmonitor_windows.h",
                             "caipserver_windows.c",
                             "caipserver_windows.h"],
                ":windows": [#"caipadapter_windows.c",
                             #"caipadapter_windows.h",
                             "caipnwmonitor_windows.c",
                             "caipnwmonitor_windows.h",
                             "caipserver_windows.c",
                             "caipserver_windows.h"],
                ":darwin": ["caipnwmonitor_posix.c",
                            "caipnwmonitor_posix.h",
                            "caipnwmonitor_darwin.c",
                            "caipnwmonitor_darwin.h",
                            "caipserver_posix.c",
                            "caipserver_posix.h",
                            "caipserver_darwin.c",
                            "caipserver_darwin.h"],
                # ":ndk": ["//src/comm/transport/udpip/android"],
                # ":android": ["//src/comm/transport/udpip/android"],
        "//conditions:default": []}),
    visibility = ["//src/comm:__subpackages__"]
)
