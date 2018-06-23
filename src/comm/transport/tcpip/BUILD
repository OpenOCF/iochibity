load("//config:variables.bzl", "COPTS_ANDROID", "DEFINES", "DBG_COPTS")

cc_library(
    name = "tcpip",
    visibility = ["//src/comm:__subpackages__"],

    copts = ["-Isrc",
             "-Isrc/util",
             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist",
             "-Iexternal/libcoap/include",
             "-Wall",
             "-DTCP_ADAPTER",
    ] + select({"//config:windows": [],
                "//config:msvc": [],
                "//config:msys": [],
                "//config:linux": ["-std=c11"] + ["-Iexternal/local_jdk/include/linuxY"],
                "//config:linux64": ["-std=c11"] + ["-Iexternal/local_jdk/include/linuxY"],
                "//conditions:default": ["-std=c11", "-Wextra"]})
    + DBG_COPTS,

    defines = DEFINES,

    linkstatic = 1,
    deps = ["//src:config",
            "//src/logger",
            "//src/util",
    	    "//src/comm/transport/util",
            "//src/comm/transport:sockets",
            "//src/portability",
	    "@libcoap//:libcoap",
	    "//third_party/utlist"],
    linkopts = select({"//config:msys": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lwinsock2", "-liphlpapi", "-lws2_32"],
                       "//config:msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 iphlpapi.lib ws2_32.lib"],
                       "//config:windows": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 iphlpapi.lib ws2_32.lib"],
                       "//config:darwin": ["-framework SystemConfiguration"], # for network monitory
                       "//config:android": ["-llog"],
                       "//conditions:default": []}),

    srcs = ["tcp_controller.c", "tcp_controller.h",
            "tcp_data_receiver.c", "tcp_data_receiver.h",
            "tcp_data_sender.c", "tcp_data_sender.h",
            "tcp_session.c", "tcp_session.h",
            "tcp_sockets.c", "tcp_sockets.h",
            "tcp_status_manager.c", "tcp_status_manager.h"
    ]
    + select({"//config:windows": ["tcp_controller_windows.c", "tcp_controller_windows.h",
                                   "tcp_data_receiver_windows.c", "tcp_data_receiver_windows.h"],
              # "//config:posix": ["tcp_data_receiver_posix.c", "tcp_data_receiver_posix.h"],
              "//conditions:default": ["tcp_data_receiver_posix.c", "tcp_data_receiver_posix.h"]}),

# with_cloud:
# tcp_cm.c
# tcp_cm.h
# userprefs.c
# userprefs.h
)
