# WARNING!  mingw requires some patching. see coap_io.c

#libcoap_repo_url = 'https://github.com/dthaler/libcoap'
#libcoap_repo_url    = 'https://github.com/obgm/libcoap'
# libcoap_version = 'IoTivity-1.2.1d'

# git clone https://github.com/dthaler/libcoap.git . -b IoTivity-1.2.1d

# 1. ./autogen.sh
# 2. autoreconf -i
# if that doesn't work:

# 1. $ libtoolize (install ltmain.sh)
# 2. $ aclocal
# 3. $ autoheader
# 4. $ autoconf
# 5. $ automake

# then: $ ./configure --disable-examples this creates
# coap_config.h.

# DARWIN: define WITH_POSIX in coap_config.h

# WINDOWS:
# unfortunately, its feature tests are based on the
# mingw environment; you must adjust it for the msvc environment. Edit
# coap_config.h to undefine the following:
#undef HAVE_LIBPTHREAD
#undef HAVE_PTHREAD_H
#undef HAVE_STRINGS_H
#undef HAVE_SYS_SOCKET_H
#undef HAVE_SYS_TIME_H
#undef TIME_WITH_SYS_TIME
#undef HAVE_UNISTD_H
#undef HAVE_SYS_UNISTD_H

# if that's not enough of a pain, ssize_t is used by libcoap
# (coap.io.[ch]). GCC puts that in unistd.h, which windows does not
# have. Posix puts it in sys/types.h, which windows does have - but it
# does not contain a definition of ssize_t. Thanks, MS. Windows does
# have SSIZE_T, in BaseTsd.h.

# the current workaround is to insert the following in
# include/coap/coap_io.h:
# /* FIXME GAR: we need this for ssize_t */
# #ifdef _WIN32
# #include <BaseTsd.h>
# #define ssize_t SSIZE_T
# #endif

# we also have to define HAVE_WS2TCPIP_H for windows. we could do that
# in this BUILD file, but then if an OpenOCF target (e.g. ocf/BUILD)
# depends on a libcoap header, it too would have to define
# HAVE_WS2TCPIP_H. So need to add it instead to coap_config.h

load("//tools/config:variables.bzl", "COPTS_ANDROID")

licenses(["notice", "reciprocal"])

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

cc_library(
    name = "coap",
    # alwayslink = True,
    copts = ["-Ithird_party/coap",
             "-Iexternal/openocf/third_party/coap",
             "-Ithird_party/coap/include/coap",
             "-Iexternal/openocf/third_party/coap/include/coap",
             "-Isrc",
    ] + select({"msys": [],
                "msvc": [],
                "windows": [],
                "linux": ["-std=c11", "-DHAVE_CONFIG_H", "-D_GNU_SOURCE", "-DWITH_POSIX"],
                "android": COPTS_ANDROID,
              "//conditions:default": ["-std=c11"]}),
    # deps = ["//src:config", "//third_party/ndk"],
    hdrs = ["coap_config.h"] + glob(["include/**/*.h"],
                                    exclude = ["include/coap/lwippools.h"]),
    # windows: ntohs etc. are in ws2_32.lib
    linkopts = select({":msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 ws2_32.lib"],
                       ":msys": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lws2_32"],
                       "//conditions:default": []}),
    srcs = glob(["src/*.c"], exclude = ["src/coap_io_lwip.c"]),
    visibility = ["//visibility:public"]
)
