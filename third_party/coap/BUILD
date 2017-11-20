#libcoap_repo_url = 'https://github.com/dthaler/libcoap'
#libcoap_repo_url    = 'https://github.com/obgm/libcoap'
# libcoap_version = 'IoTivity-1.2.1d'

# git clone https://github.com/dthaler/libcoap.git . -b IoTivity-1.2.1d

# 1. autoreconf -i
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

cc_library(
    name = "coap",
    # alwayslink = True,
    copts = ["-Ithird_party/coap",
             "-Ithird_party/coap/include/coap",
             "-Isrc",
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

