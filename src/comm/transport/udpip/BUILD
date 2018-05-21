load("//tools/config:variables.bzl", "COPTS_ANDROID")

config_setting(
    name = "rpi3b",
     values = { "crosstool_top": "//platforms/rpi3b:toolchain" }
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
# config_setting(
#     name = "ndk",
#     values = { "crosstool_top": "//platforms/ndk:toolchain" }
# )

# cc_library(
#     name = "jnihdrs",
#     copts = ["-std=c11"],
#     hdrs = ["@local_jdk//:jni_header"
#             # FIXME: platform must include host, e.g. android_linux
#     ] + select({"//config:darwin_with_jni": ["@local_jdk//:jni_md_header-darwin"],
#         # ":darwin": ["@local_jdk//:jni_md_header-darwin"],
#                 "//conditions:default": []})
# )

cc_library(
    name = "udpip",
    copts = ["-Isrc",
             "-Isrc/util",
             "-Ithird_party/utlist",
             "-Iexternal/openocf/third_party/utlist",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
	     # FIXME: for jni.h - but this should come from NDK, not local_jdk
	     # and toolchain should have it on --sysroot so we do not need it here
             #"-Iexternal/local_jdk/include",
             #"-Iexternal/local_jdk/include/linux",
             #"-Iexternal/local_jdk/include/darwin",
             "-Wall",
    # ] + select({"@openocf//config:darwin_with_jni": ["-Iexternal/local_jdk/include/darwin"],
    #             "@openocf//config:linuxwith_jni": ["-Iexternal/local_jdk/include/linux"],
    #             "//conditions:default": ["BROKEN"]}),

    ] + select({":windows": [],
                ":msvc": [],
                ":msys": [],
	        # find jni_md.h for exec host
	        # FIXME: select on android_linux, android_darwin, android_win
                # ":android": ["-std=c11", "-Iexternal/local_jdk/include/linux"],
                #"//config:darwin_with_jni": COPTS_ANDROID + ["-Iexternal/local_jdk/include/darwinX"],
                # FIXME: select on java_linux, java_darwin
                "//config:linux": ["-std=c11"] + ["-Iexternal/local_jdk/include/linuxY"],
                #":darwin": ["-std=c11", "-UDEBUG"],
                #":darwin64": ["-std=c11", "-UDEBUG"],
                "//conditions:default": ["-std=c11", "-Wextra"]}),
    linkstatic = 1,
    deps = ["//src:config",
            "//src/logger",
            "//src/util",
    	    # "//src/comm:hdrs",
            # "//src/comm/security",
    	    "//src/comm/transport/util",
            "//src/comm/transport:sockets",
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
                       "//config:android": ["-llog"],
                       "//config:android_arm64": ["-llog"],
                       "//config:android_x86": ["-llog"],
                       "//conditions:default": []}),
    srcs = glob(["*0.c"]) + glob(["*0.h"]) + glob(["udp_*.c"]) + glob(["udp_*.h"])
    + select({":msvc": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              ":msys": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              ":windows": glob(["*_windows.c"]) + glob(["*_windows.h"]),
              "//config:darwin": glob(["*_posix.c"]) + glob(["*_posix.h"])
	                       + glob(["*_darwin.c"]) + glob(["*_darwin.h"]),
              # "//config:darwin_with_jni": glob(["*_posix.c"]) + glob(["*_posix.h"])
	      #                  + glob(["*_darwin.c"]) + glob(["*_darwin.h"])
                               # + ["@local_jdk//:jni_header",
                               #    "@local_jdk//:jni_md_header-darwin"],
              "//config:linux": glob(["*_posix.c"]) + glob(["*_posix.h"])
	      	      + glob(["*_linux.c"]) + glob(["*_linux.h"]),
              ":rpi3b": glob(["*_posix.c"]) + glob(["*_posix.h"])
	      	      + glob(["*_linux.c"]) + glob(["*_linux.h"]),
              # "//config:android": glob(["*_android.c"]) + glob(["*_android.h"])
	      #                   + ["caipserver_posix.c", "caipserver_posix.h"],
              "//config:android": ["caipnwmonitor_posix.c", "caipnwmonitor_posix.h"]
  	                            + ["caipnwmonitor_linux.c", "caipnwmonitor_linux.h"]
  	                            + ["caipserver_posix.c", "caipserver_posix.h"]
  	                            + ["caipserver_linux.c", "caipserver_linux.h"],
              "//config:android_arm64": ["caipnwmonitor_posix.c", "caipnwmonitor_posix.h"]
  	                            + ["caipnwmonitor_linux.c", "caipnwmonitor_linux.h"]
  	                            + ["caipserver_posix.c", "caipserver_posix.h"]
  	                            + ["caipserver_linux.c", "caipserver_linux.h"],
  	      # + ["BORKEN"],
              "//config:android_x86": glob(["*_android.c"]) + glob(["*_android.h"])
  	                            + ["caipserver_posix.c", "caipserver_posix.h"],
                 # "org_iotivity_ca_CaIpInterface.h"
              "//conditions:default": ["BROKEN2"]}),
# glob(["*_posix.c"]) + glob(["*_posix.h"])
# 	      	      + glob(["*_linux.c"]) + glob(["*_linux.h"])}),
    visibility = ["//src/comm:__subpackages__"]
)
