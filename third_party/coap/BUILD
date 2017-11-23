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
	     "-D_GNU_SOURCE",
	     "-DWITH_POSIX",
	     # "-D_POSIX_C_SOURCE=200809L",
	     # "-D_DEFAULT_SOURCE",
             # "-Iexternal/openocf/src",
             # "-Ithird_party/coap/include",
             # "-Iexternal/openocf/third_party/coap/include",
             # "-Ithird_party/coap/include/coap",
             # "-Iexternal/openocf/third_party/coap/include/coap"
    ]
    + select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11"]}),
    defines = ["WITH_POSIX"],
    deps = ["//src:config"],
    hdrs = ["coap_config.h",
            "include/coap/coap.h",
            "include/coap/pdu.h",
            "include/coap/uri.h",
            # if DEBUG_COAP
            "include/coap/debug.h"
    ],
    srcs = ["src/address.c",
            "include/coap/address.h",

            "src/async.c",
            "include/coap/async.h",

            "include/coap/bits.h",

            "src/block.c",
            "include/coap/block.h",

            "src/coap_io.c",
            "include/coap/coap_io.h",

            "src/coap_list.c",
            "include/coap/coap_list.h",

            "src/coap_time.c",
            "include/coap/coap_time.h",

            #FIXME: only if --enable-debug flag:
            "src/debug.c",
            "include/coap/debug.h",

            "src/encode.c",
            "include/coap/encode.h",

            "src/hashkey.c",
            "include/coap/hashkey.h",

            "include/coap/libcoap.h",

            "src/mem.c",
            "include/coap/mem.h",

            "src/net.c",
            "include/coap/net.h",

            "src/option.c",
            "include/coap/option.h",
            "src/pdu.c",
            # "include/coap/pdu.h",

            "include/coap/prng.h",

            "src/resource.c",
            "include/coap/resource.h",

            "src/str.c",
            "include/coap/str.h",

            "src/subscribe.c",
            "include/coap/subscribe.h",

            "src/uri.c",
            # "include/coap/uri.h",

            "include/coap/uthash.h",
            "include/coap/utlist.h"
    ],
    visibility = ["//visibility:public"]
)
