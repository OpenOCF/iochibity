# the only reason for this is so _ipif.h will be picked up by //:mkhdrs

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
    name = "ip",
    srcs = ["_ipif.h"],
    visibility = ["//src/comm:__subpackages__"]
)
