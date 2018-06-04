## load("//config:variables.bzl", "DEFINES", "OS_COPTS")

cc_library(
    name = "libcoap",
    copts = ["-std=gnu99", "-fPIC",
             '-Wall', '-ffunction-sections',
             '-fdata-sections', '-fno-exceptions',

	     # to find headers in the libcoap source tree:
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",

	     # to find coap.h and coap_config.h:
             "-Iconfig/darwin",     # coap_config.h
             "-Iconfig/darwin/coap",  # coap.h
             "-DWITH_POSIX",
             "-D_GNU_SOURCE",
             "-DWITH_UPSTREAM_LIBCOAP"]
    + select({"@//config:enable_tcp": ["-DWITH_TCP"],
              "//conditions:default": []}),

    srcs = glob(["src/**/*.c"],
                exclude=["src/**/coap_io_lwip.c"]),
    hdrs = glob(["include/**/*.h"])
    + ["@//config/darwin/coap:coap.h",
       "@//config/darwin:coap_config.h",
    ],
    visibility = ["//visibility:public"]
)
