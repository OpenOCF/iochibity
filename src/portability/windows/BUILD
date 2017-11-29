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
    name = "win",
    copts = select({"windows": [],
              "msvc": [],
              "msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = [#"//src:config",
            "//src/logger",
            # "//src/portability:hdrs",
            "//third_party/mbedtls"],
    # linkopts = ["-W,crypt32.lib"],
    srcs = glob(["*.c"], exclude=["pthread_create.c"])
    + glob(["*.h"], exclude=["pthread_create.h"])
    + select({"msvc": glob(["pthread_create.*"]),
              "windows": glob(["pthread_create.*"]),
              "msys": [],
              "//conditions:default": []}),
    visibility = ["//src/portability:__pkg__"]
)
