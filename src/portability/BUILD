# TODO: tizen support
config_setting(
    name = "linux",
    values = { "cpu": "k8" }
)

config_setting(
    name = "rpi3b",
     values = { "cpu": "k8",
                "crosstool_top": "//platforms/rpi3b:toolchain" }
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
		"linux": ["-std=c11", "-DHAVE_CONFIG_H", "-D_GNU_SOURCE", "-DWITH_POSIX"],
                "//config:darwin": ["-std=c11", "-UDEBUG"],
                "//config:darwin64": ["-std=c11", "-UDEBUG"],
                "//conditions:default": ["-std=c11"]}),
    deps = ["//src/logger",
            "//third_party/mbedtls",
            "//third_party/mbedtls:mbedtls-crypto",
            "//third_party/mbedtls:mbedtls-x509",
    ] + select({":linux": ["//src/portability/posix"],
			   # "//src/portability/linux"],
                ":rpi3b": ["//src/portability/posix",
			   "//src/portability/linux"],
                ":windows": ["//src/portability/windows:win"],
                ":msvc": ["//src/portability/windows:win"],
                ":msys": ["//src/portability/posix"],
                "//config:darwin": ["//src/portability/posix",
                            "//src/portability/darwin"],
                "//config:darwin64": ["//src/portability/posix",
                              "//src/portability/darwin"],
                "//config:android_arm64": ["//src/portability/android"],
                "//config:android": ["//src/portability/android"],
              "//conditions:default": ["BROKEN"]}),
    # hdrs = glob(["*.h"]),
    linkopts = select({":msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 bcrypt.lib"],
                       ":msys": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lbcrypt"],
                       ":windows": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 bcrypt.lib"],
                       "//conditions:default": []}),
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//visibility:public"]
    # visibility = ["//src:__subpackages__",
    #               "//third_party:__subpackages__",
    #               "//tools/cbor:__subpackages__",
    # ]
 )
