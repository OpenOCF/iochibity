load("//config:variables.bzl", "CSTD", "COPTS_ANDROID", "DEFINES")

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
             "-Iexternal/uthash/src",
             "-Iexternal/libcoap/include",
             "-Wall",
    ] + select({"//config:mingw": [],
                "//config:msvc": [],
                "//config:msys": [],
                "//config:linux": ["-D_DEFAULT_SOURCE","-std=c11"],
                "//config:linux64": ["-D_DEFAULT_SOURCE", "-std=c11"],
                "//conditions:default": ["-std=c11", "-Wextra"]})
    + CSTD,

    # + select({#"//config::rpi_arm8": [],
    #          "//conditions:default": ["-DPORT_U6=$(FOO)", "-DPORT_U6S=0", "-DPORT_U4=0", "-DPORT_U4S=0"]}),

    defines = select({"//config:rpi_arm8": ["_DEFAULT_SOURCE"], # enables IFF_UP etc. defines in net/if.h
                      "//config:linux64": ["_DEFAULT_SOURCE"],
                      "//conditions:default": []})
    + DEFINES,

    alwayslink = 1,
    linkstatic = 1,
    deps = ["//src:config",
    	    "//src/comm/transport/util",
            "//src/comm/transport:sockets",
            "//src/logger",
            "//src/portability",
            "//src/util",
	    "@libcoap//:libcoap",
	    "@uthash//:uthash"],
    linkopts = select({"//config:msys": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-liphlpapi", "-lws2_32"],
                       "//config:msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 iphlpapi.lib ws2_32.lib"],
                       "//config:mingw": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-liphlpapi", "-lws2_32"],
                       "//config:darwin": ["-framework SystemConfiguration"], # for network monitory
                       "//config:android": ["-llog"],
                       "//conditions:default": []}),
    srcs = ["udp_controller.c", "udp_controller.h",
            "udp_data_receiver.c", "udp_data_receiver.h",
            "udp_data_sender.c", "udp_data_sender.h",
            "udp_data_sockets.c", "udp_data_sockets.h",
            "udp_status_manager.c", "udp_status_manager.h",
            "udp_thread_manager.c", "udp_thread_manager.h"]
    + select({"//config:android": ANDROID_SRCS,
              "//config:msvc": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              "//config:msys": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              "//config:mingw": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              "//config:darwin": glob(["*_posix.c"]) + glob(["*_posix.h"])
	                       + glob(["*_darwin.c"]) + glob(["*_darwin.h"]),

              "//config:ios_arm7": glob(["*_posix.c"]) + glob(["*_posix.h"])
	      + glob(["*_darwin.c"], exclude=["udp_status_manager_darwin.c"])
              + glob(["*_darwin.h"], exclude=["udp_status_manager_darwin.h"])
              + ["udp_status_manager_ios.c", "udp_status_manager_ios.h"],

              "//config:ios_arm64": glob(["*_posix.c"]) + glob(["*_posix.h"])
	      + glob(["*_darwin.c"], exclude=["udp_status_manager_darwin.c"])
              + glob(["*_darwin.h"], exclude=["udp_status_manager_darwin.h"])
              + ["udp_status_manager_ios.c", "udp_status_manager_ios.h"],

              "//config:ios_x86": glob(["*_posix.c"]) + glob(["*_posix.h"])
	      + glob(["*_darwin.c"], exclude=["udp_status_manager_darwin.c"])
              + glob(["*_darwin.h"], exclude=["udp_status_manager_darwin.h"])
              + ["udp_status_manager_ios.c", "udp_status_manager_ios.h"],

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
              "//conditions:default": ["BROKEN2"]}),

    visibility = ["//src/comm:__subpackages__"]
)
