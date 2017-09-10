config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)

cc_library(
    name = "config",
    hdrs = ["iotivity_config.h",
            "iotivity_constants.h",
            "iotivity_debug.h",
            "system.h",
            "transport_types.h"]
    + select({"windows": ["msvc_config.h"],
              "//conditions:default": []}),
    visibility = ["//visibility:public"])
