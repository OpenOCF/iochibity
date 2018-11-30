load("//config:variables.bzl", "COPTS_ANDROID", "DEFINES", "DBG_COPTS")

ANDROID_SRCS = ["udp_status_manager_posix.c",
                "udp_status_manager_posix.h",
                "udp_status_manager_linux.c",
                "udp_status_manager_linux.h",
                "udp_data_receiver_posix.c",
                "udp_data_receiver_posix.h",
                "udp_data_receiver_linux.c",
                "udp_data_receiver_linux.h"]

cc_library(
    name = "udpip",
    copts = ["-Isrc",
             "-Isrc/util",
             "-Iexternal/utlist/src",
             "-Iexternal/libcoap/include",
             "-Wall",
    ] + select({"//config:windows": [],
                "//config:msvc": [],
                "//config:msys": [],
                "//config:linux": ["-D_DEFAULT_SOURCE","-std=c11"],
                "//config:linux64": ["-D_DEFAULT_SOURCE", "-std=c11"],
                "//conditions:default": ["-std=c11", "-Wextra"]})
    + DBG_COPTS,

    # + select({#"//config::rpi_arm8": [],
    #          "//conditions:default": ["-DPORT_U6=$(FOO)", "-DPORT_U6S=0", "-DPORT_U4=0", "-DPORT_U4S=0"]}),

    defines = select({"//config:rpi_arm8": ["_DEFAULT_SOURCE"], # enables IFF_UP etc. defines in net/if.h
                      "//config:linux64": ["_DEFAULT_SOURCE"],
                      "//conditions:default": []})
    + DEFINES,

    linkstatic = 1,
    deps = ["//src:config",
            "//src/logger",
            "//src/util",
    	    # "//src/comm:hdrs",
            # "//src/comm/security",
    	    "//src/comm/transport/util",
            "//src/comm/transport:sockets",
            "//src/portability",
	    "@libcoap//:libcoap",
	    "@utlist//:utlist"],
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
    linkopts = select({"//config:msys": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lwinsock2", "-liphlpapi", "-lws2_32"],
                       "//config:msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 iphlpapi.lib ws2_32.lib"],
                       "//config:windows": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 iphlpapi.lib ws2_32.lib"],
                       "//config:darwin": ["-framework SystemConfiguration"], # for network monitory
                       "//config:android": ["-llog"],
                       "//conditions:default": []}),
    srcs = ["udp_controller.c", "udp_controller.h",
            "udp_data_receiver.c", "udp_data_receiver.h",
            "udp_data_sender.c", "udp_data_sender.h",
            "udp_data_sockets.c", "udp_data_sockets.h",
            "udp_status_manager.c", "udp_status_manager.h",
            "udp_thread_manager.c", "udp_thread_manager.h"]
    + select({"//config:msvc": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              "//config:msys": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              "//config:windows": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              "//config:darwin": glob(["*_posix.c"]) + glob(["*_posix.h"])
	                       + glob(["*_darwin.c"]) + glob(["*_darwin.h"]),
              # "//config:darwin_with_jni": glob(["*_posix.c"]) + glob(["*_posix.h"])
	      #                  + glob(["*_darwin.c"]) + glob(["*_darwin.h"])
                               # + ["@local_jdk//:jni_header",
                               #    "@local_jdk//:jni_md_header-darwin"],
              "//config:linux": glob(["*_posix.c"]) + glob(["*_posix.h"])
	                      + glob(["*_linux.c"]) + glob(["*_linux.h"]),
              "//config:linux64": glob(["*_posix.c"]) + glob(["*_posix.h"])
	                             + glob(["*_linux.c"]) + glob(["*_linux.h"]),
              "//config:rpi_arm8": ["udp_status_manager_posix.c",
                               "udp_status_manager_posix.h",
                               "udp_status_manager_linux.c",
                               "udp_status_manager_linux.h",
                               "udp_data_receiver_posix.c",
                               "udp_data_receiver_posix.h",
                               "udp_data_receiver_linux.c",
                               "udp_data_receiver_linux.h"],
              "//config:android": ANDROID_SRCS,
              "//conditions:default": ["BROKEN2"]}),

    visibility = ["//src/comm:__subpackages__"]
)
