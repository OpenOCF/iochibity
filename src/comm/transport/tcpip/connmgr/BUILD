load("//config:variables.bzl", "CSTD", "COPTS_ANDROID", "DEFINES")

cc_library(
    name = "connmgr",
    visibility = ["//src/comm:__subpackages__"],

    copts = ["-Isrc",
             "-Isrc/util",
             "-Iexternal/uthash/src",
             "-Iexternal/libcoap/include",
             "-Wall",
             "-DTCP_ADAPTER",
    ] + select({"//config:windows": [],
                "//config:msvc": [],
                "//config:msys": [],
                "//config:linux": ["-std=c11"],
                "//config:linux64": ["-std=c11"],
                "//conditions:default": ["-std=c11", "-Wextra"]})
    + CSTD,

    defines = DEFINES,

    linkstatic = 1,
    deps = ["//src:config",
            "//src/logger",
            "//src/util",
            "//src/comm/transport:sockets",
    	    "//src/comm/transport/util",
            "//src/portability",
            "@libcoap//:libcoap",
	    "@uthash//:uthash"],
    linkopts = select({"//config:msys": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lwinsock2", "-liphlpapi", "-lws2_32"],
                       "//config:msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 iphlpapi.lib ws2_32.lib"],
                       "//config:windows": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 iphlpapi.lib ws2_32.lib"],
                       "//config:darwin": ["-framework SystemConfiguration"], # for network monitory
                       "//config:android": ["-llog"],
                       "//conditions:default": []}),

    srcs = ["caconnectionmanager.c", "caconnectionmanager.h",
            "camanagerutil.c", "camanagerutil.h",
            "camessagearbiter.c", "camessagearbiter.h",
            "capolicymanager.c", "capolicymanager.h",
            "occonnectionmanager.c", "occonnectionmanager.h",
            "tcp_cm.c", "tcp_cm.h"]
    # + select({"//config:windows": ["tcp_controller_windows.c", "tcp_controller_windows.h",
    #                                "tcp_data_receiver_windows.c", "tcp_data_receiver_windows.h"],
    #           # "//config:posix": ["tcp_data_receiver_posix.c", "tcp_data_receiver_posix.h"],
    #           "//conditions:default": ["tcp_data_receiver_posix.c", "tcp_data_receiver_posix.h"]}),

# with_cloud:
# tcp_cm.c
# tcp_cm.h
# userprefs.c
# userprefs.h
)
