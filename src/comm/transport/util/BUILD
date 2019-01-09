cc_library(
    name = "util",
    alwayslink = True,
    copts = ["-D_GNU_SOURCE",
             "-Iexternal/libcoap/include" # FIXME: for logging only
    ] + select({"//config:mingw": [],
                "//config:msvc": [],
                "//config:msys": [],
		# FIXME: don't we want jni.h, jni_md.h from the target instead of the build host???
                "//config:android": ["-std=c11",
                             "-Iexternal/local_jdk/include",
                             "-Iexternal/local_jdk/include/linux"],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps  = ["//src:config",
             "//src/logger",
             "//src/portability",
             "//src/util",
             "@libcoap//:libcoap",
             "@mbedtls//:mbedtls"],
    # + select({":android": [":jnihdrs"],
    #           "//conditions:default": []}),
    hdrs = ["caadapterutils.h",
            #"ca_adapter_net_ssl.h"
    ],
    srcs  = ["caadapterutils.c"],
    # + select({":android": glob(["*_android.c"]) + glob(["*_android.h"]),
    #           "//conditions:default": []}),
    visibility = ["//visibility:public"]
# "//src/comm:__subpackages__",
#                   "//src/sec/svrs:__pkg__"]
)

cc_library(
    name = "jnihdrs",
    copts = ["-std=c11"],
    hdrs = ["@local_jdk//:jni_header",
            # "@local_jdk//:jni_md_header-linux"
    ],
)
