cc_library(
    name = "windows",
    copts = select({"//config:mingw": ["-DNOGDI", "-DNOSERVICE"], # avoid warnings about redefined macros
                    "//config:msvc": [],
                    "//config:msys": [],
                    "//config:darwin": ["-U DEBUG"],
                    "//conditions:default": ["-std=c11"]}),
    deps = ["//src/logger",
            "@mbedtls//:mbedtls"],
    # linkopts = ["-W,crypt32.lib"],
    srcs = glob(["*.c"], exclude=["pthread_create.c"])
    + glob(["*.h"], exclude=["pthread_create.h"])
    + select({"//config:msvc": glob(["pthread_create.*"]),
              "//config:mingw": glob(["pthread_create.*"]),
              "//config:msys": [],
              "//conditions:default": []}),
    visibility = ["//src/portability:__pkg__"]
)
