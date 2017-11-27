# TODO: tizen support
config_setting(
    name = "linux",
    values = { "cpu": "k8" }
)
config_setting(
    name = "darwin",
    values = { "cpu": "darwinX" }
)
config_setting(
    name = "darwin64",
    values = { "cpu": "darwin_x86_64X" }
)
config_setting(
    name = "android",
    # values = { "crosstool_top": "//external:android/crosstool."} # "@android_ndk//:toolchain-libcpp"
    values = {"cpu": "armeabi-v7a"}
)
config_setting(
    name = "rpi3b",
     values = { "crosstool_top": "//platforms/rpi3b:toolchain" }
)
config_setting(
    name = "ndk",
    values = { "crosstool_top": "//platforms/ndk:toolchain" }
)
config_setting(
    name = "wrlinux",
    values = { "crosstool_top": "//platforms/wrlinux:toolchain" }
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
# FIXME: detect android builds
# config_setting(
#     name = "android",
#     values = {"define": "target=android",
#               "fat_apk_cpu": "x86",
#               #"android_crosstool_top": "//platforms/ndk:toolchain",
#               # "cpu": "armeabi-v7a"
#     }
# )
config_setting(
    name = "androidx86",
    values = {"fat_apk_cpu": "x86",
              # "android_crosstool_top": "//platforms/ndk:toolchain"
    }
)

# cc_library(
#     name = "hdrs",
#     hdrs = ["ocatomic.h",
#             "ocevent.h",
#             "ocrandom.h",
#             "octhread.h",
#             "oic_malloc.h",
#             "oic_platform.h",
#             "oic_string.h",
#             "oic_time.h",
#             "iotivity_commontypes.h"],
#     visibility = ["//visibility:public"],
# )

cc_library(
    name = "portability",
    # alwayslink = True,
    linkstatic = 1,
    copts = ["-Ithird_party/mbedtls/include",
             "-Iexternal/openocf/third_party/mbedtls/include",
	     "-D_GNU_SOURCE",
	     "-DWITH_POSIX",
    ] + select({":windows": [],
                ":msvc": [],
                ":msys": [],
                ":linux": ["-std=c11"],
                # ":darwin": ["-std=c11", "-UDEBUG"],
                # ":darwin64": ["-std=c11", "-UDEBUG"],
                "//conditions:default": ["-std=c11"]}),
    deps = ["//src/logger",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509",
    ] + select({":linux": ["//src/portability/posix",
			   "//src/portability/linux"],
                ":rpi3b": ["//src/portability/posix",
			   "//src/portability/linux"],
                ":windows": ["//src/portability/windows:win"],
                ":msvc": ["//src/portability/windows:win"],
                ":msys": ["//src/portability/posix"],
                #":darwin": ["//src/portability/darwin"],
                #":darwin64": ["//src/portability/darwin"],
              "//conditions:default": ["//src/portability/posix"]}),
    hdrs = glob(["*.h"]),
    linkopts = select({":msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 bcrypt.lib"],
                       ":msys": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lbcrypt"],
                       ":windows": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 bcrypt.lib"],
                       "//conditions:default": []}),
    srcs = glob(["*.c"],
                exclude = ["_events.c"]),
    visibility = ["//src:__subpackages__",
                  "//third_party:__subpackages__"]
 )
