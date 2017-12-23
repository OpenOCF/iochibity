config_setting(
    name = "linux",
    values = { "cpu": "k8" }
)
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
             "-Iexternal/openocf/third_party/coap/include",
             "-Wall"
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              ":darwin": ["-UDEBUG"],
              ":darwin64": ["-UDEBUG"],
              "//conditions:default": ["-std=c11", "-Wextra"]}),
    linkstatic = 1,
    deps = ["//src:config",
            "//src/logger",
            "//src/util",
    	    # "//src/comm:hdrs",
            # "//src/comm/security",
    	    "//src/comm/transport/util",
            "//src/comm/transport/ip",
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
    srcs = glob(["*0.c"])
    + glob(["*0.h"])
    + select({":msvc": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              ":msys": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              ":windows": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              ":darwin": glob(["*_posix.c"]) + glob(["*_posix.h"])
	      		 + glob(["*_darwin.c"]) + glob(["*_darwin.h"]),
              ":linux": glob(["*_posix.c"]) + glob(["*_posix.h"])
	      	      + glob(["*_linux.c"]) + glob(["*_linux.h"]),
                # ":ndk": ["//src/comm/transport/udpip/android"],
                # ":android": ["//src/comm/transport/udpip/android"],
        "//conditions:default": []}),
    visibility = ["//src/comm:__subpackages__"]
)
