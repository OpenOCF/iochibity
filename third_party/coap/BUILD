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

# then: $ ./configure --disable-examples
# this creates coap_config.h. now you can build with bazel

licenses(["notice", "reciprocal"])

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

# compile cmd from make:
# /bin/bash ./libtool  --tag=CC   --mode=compile gcc -DHAVE_CONFIG_H -I.    -fPIC -fPIE -I./include/coap -I./include/coap -pedantic -Wall -Wextra -Wformat-security -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wshadow -Wstrict-prototypes -Wswitch-default -Wswitch-enum -Wunused  -Wlogical-op -Wunused-result -std=c99 -g -O2 -fdiagnostics-color -D_GNU_SOURCE -DWITH_POSIX -MT src/libcoap_1_la-coap_io.lo -MD -MP -MF src/.deps/libcoap_1_la-coap_io.Tpo -c -o src/libcoap_1_la-coap_io.lo `test -f 'src/coap_io.c' || echo './'`src/coap_io.c
# libtool: compile:  gcc -DHAVE_CONFIG_H -I. -fPIC -I./include/coap -I./include/coap -pedantic -Wall -Wextra -Wformat-security -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wshadow -Wstrict-prototypes -Wswitch-default -Wswitch-enum -Wunused -Wlogical-op -Wunused-result -std=c99 -g -O2 -fdiagnostics-color -D_GNU_SOURCE -DWITH_POSIX -MT src/libcoap_1_la-coap_io.lo -MD -MP -MF src/.deps/libcoap_1_la-coap_io.Tpo -c src/coap_io.c  -fPIC -DPIC -o src/.libs/libcoap_1_la-coap_io.o

cc_library(
    name = "coap",
    # alwayslink = True,
    copts = ["-Ithird_party/coap",
             "-Ithird_party/coap/include/coap",
             "-Isrc",
    ] + select({"msys": [],
              "msvc": [],
              "windows": [
                  "-DHAVE_WINSOCK2_H",
                  "-DHAVE_WS2TCPIP_H"],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config"],
    hdrs = ["coap_config.h"] + glob(["include/**/*.h"],
                                    exclude = ["include/coap/lwippools.h"]),
    # windows: ntohs etc. are in ws2_32.lib
    linkopts = select({":msvc": ["-LIBPATH:/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64 ws2_32.lib"],
                       ":msys": ["-L/c/Program\ Files\ \(x86\)/Windows\ Kits/10/lib/10.0.15063.0/um/x64", "-lws2_32"],
                       "//conditions:default": []}),
    srcs = glob(["src/*.c"], exclude = ["src/coap_io_lwip.c"]),
    visibility = ["//visibility:public"]
)
