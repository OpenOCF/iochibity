# needed by third_party libs
cc_library(
    name = "config",
    srcs = ["_openocf_config.h",
            # "iotivity_constants.h",
            # "iotivity_debug.h",
            "_system.h",
            # "transport_types.h"
    ],
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
