# if platform != windows and defined(HAVE_LIBPTHREAD)
# NB: for now we ignore platforms lacking pthreads
cc_library(
    name = "posix",
    copts = ["-Ithird_party/utlist",
             "-DWITH_POSIX",
    #          "-Iexternal/openocf/src",
    #          "-Isrc/logger",
    #          "-Iexternal/openocf/src/logger",
    #          "-Isrc/portability"]
             # Some source files #include "mbedtls/config.h", others just "config.h"
             "-Iconfig",
             "-Iconfig/mbedtls",
             "-Iexternal/mbedtls/patched/include",
    ] + select({"//config:windows": [],
                "//config:msvc": [],
                "//config:msys": [],
                "//conditions:default": ["-std=c11",
                                         "-U DEBUG"]}), # OS X
    deps = [#"//src:config",
            "//src/logger",
            # "//src/portability:hdrs",
            "@mbedtls//:mbedtls",
            "//third_party/utlist"],
    linkopts = select({"//config:windows": [],
                       "//config:msvc": [],
                       "//config:msys": ["-lpthread"],
                       "//config:darwin": [],
                       "//config:linux": ["-lpthread"],
                       "//config:linux-x86_64": ["-lpthread"],
                       "//config:rpi": ["-lpthread"],
                       "//config:android_x86": ["BORKED1"],
                       "//conditions:default": ["BORKED"]}),
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//visibility:public"]
)
