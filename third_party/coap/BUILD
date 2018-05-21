# Template file for use with autoconf
load("//tools/config:variables.bzl", "COPTS_ANDROID")

licenses(["notice", "reciprocal"])

# Vars exported by ./configure:
WARNING_CFLAGS="-pedantic -Wall -Wextra -Wformat-security -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wshadow -Wstrict-prototypes -Wswitch-default -Wswitch-enum -Wunused  -Wunused-result"

# LD version script stuff:
# libcoap_SYMBOLS="-export-symbols $(srcdir)/libcoap-$(LIBCOAP_API_VERSION).sym"

# _GNU_SOURCE WITH_POSIX
# PREDEFINED_CFLAGS="_GNU_SOURCE WITH_POSIX"

# CFLAGS includes predefined flags -D_GNU_SOURCE -DWITH_POSIX
CFLAGS="-g -O2 -fdiagnostics-color -D_GNU_SOURCE -DWITH_POSIX"

config_setting(
    name = "android",
    values = {"cpu": "armeabi-v7a",
              # "define": "target=android",
              # "crosstool_top": "@android_ndk//:toolchain-libcpp",
              # "android_cpu": "x86",
              # "android_crosstool_top": "//platforms/ndk:toolchain"
    }
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
config_setting(
    name = "linux",
    values = { "cpu": "k8" }
)

config_setting(
    name = "rpi",
    values = { "cpu": "armv8" }
)

cc_library(
    name = "coap",
    # alwayslink = True,
    copts = ["-Ithird_party/coap",
             "-Iexternal/openocf/third_party/coap",
             "-Ithird_party/coap/include/coap",
             "-Iexternal/openocf/third_party/coap/include/coap",
             "-Isrc",
             "-DHAVE_CONFIG_H"]
    + WARNING_CFLAGS.split(" ")
    + CFLAGS.split(" ")
    # + libcoap_SYMBOLS.split(" ")
    + select({"msys": [],
              "msvc": [],
              "windows": [],
              "linux": ["-std=c11"],
              "rpi": ["-std=c11"],
              "android": COPTS_ANDROID,
              "//conditions:default": ["-std=c11"]}),
    hdrs = ["coap_config.h"] + glob(["include/**/*.h"],
                                    exclude = ["include/coap/lwippools.h"]),
    # windows: ntohs etc. are in ws2_32.lib
    linkopts = select({":msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 ws2_32.lib"],
                       ":msys": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lws2_32"],
                       "//conditions:default": []}),
    srcs = glob(["src/*.c"], exclude = ["src/coap_io_lwip.c"]),
    visibility = ["//visibility:public"]
)
