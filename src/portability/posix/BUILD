# if platform != windows and defined(HAVE_LIBPTHREAD)
# NB: for now we ignore platforms lacking pthreads
cc_library(
    name = "posix",
    copts = ["-DWITH_POSIX",
             "-Iexternal/mbedtls/patched/include",
    ] + select({"//config:darwin": ["-U DEBUG"],
                "//config:mingw": [],
                "//config:msvc": [],
                "//config:msys": [],
                "//conditions:default": ["-std=c11"]}),
    deps = ["//src/logger",
            "@mbedtls//:mbedtls",
            "@uthash//:uthash"],
    linkopts = select({"//config:msvc": [],
                       "//config:msys": ["-lpthread"],
                       "//config:mingw": ["-lpthread"],
                       "//config:darwin": [],
                       "//config:linux": ["-lpthread"],
                       "//config:linux64": ["-lpthread"],
                       "//config:rpi_arm8": ["-lpthread"],
                       "//config:android_x86": ["BORKED1"],
                       "//conditions:default": ["BORKED"]}),
    srcs = glob(["*.c"]) + glob(["*.h"]),
    visibility = ["//visibility:public"]
)
