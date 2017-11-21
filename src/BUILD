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

# needed by third_party libs
cc_library(
    name = "config",
    hdrs = [# "openocf_config.h",
            # "iotivity_constants.h",
            # "iotivity_debug.h",
            # "system.h",
            # "transport_types.h"
    ]
    + select({":windows": ["msvc_config.h"],
              ":msvc": ["msvc_config.h"],
              ":msys": [],
              "//conditions:default": []}),
    visibility = ["//visibility:public"]
)

android_library(
    name = "openocf",
    deps = ["//src/ocf",
            "//src/comm/transport/udpip/android"],
    srcs = ["Lib.java"],
)

android_binary(
    name = "hello_world",
    srcs = glob([
        "MainActivity.java",
        "Jni.java",
    ]),
    manifest = "AndroidManifest.xml",
    resource_files = glob(["res/**"]),
    deps = [
        "//src/ocf",
        ":lib",
        "@androidsdk//com.android.support:appcompat-v7-25.0.0",
    ],
)
