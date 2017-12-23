# FIXME: remove deps on third_party, should depend only on openocf

config_setting(
    name = "windows",
    values = { "cpu": "x64_windows" }
)
config_setting(
    name = "android",
    values = {"define": "target=android",
              "android_cpu": "x86",
              "android_crosstool_top": "//platforms/ndk:toolchain" }
)
config_setting(
    name = "linux",
    values = { "cpu": "k8" }
)
config_setting(
    name = "wrlinux",
    values = { "crosstool_top": "//platforms/wrlinux:toolchain" }
)
config_setting(
    name = "rpi3b",
     values = { "cpu": "k8",
     "crosstool_top": "//platforms/rpi3b:toolchain" }
)

cc_binary(
    name = "client",
    copts = ["-Isrc",
             "-Iinclude",
             "-Isrc/comm/api",
             "-Isrc/logger",
             "-Isrc/ocf",
             "-Isrc/ocf/client",
             "-Isrc/portability",
             "-Ithird_party/cjson",
             "-Ithird_party/coap",
             "-Ithird_party/coap/include",
             "-Ithird_party/tinycbor/src",
    ]
    + select({":rpi3b": ["-Iexternal/cosysroot_rpi3b/usr/include",
                         "-Iexternal/cosysroot_rpi3b/usr/include/cdk",
                         "-std=c11"],
              ":android": ["-Iexternal/cosysroot_ndk/usr/include",
                         "-Iexternal/cosysroot_ndk/usr/include/cdk",
                         "-std=c11"],
              ":wrlinux": ["-Iexternal/cosysroot_wrlinux/usr/local/include",
                         "-Iexternal/cosysroot_wrlinux/usr/local/include/cdk",
                         "-Iexternal/cosysroot_wrlinux/usr/include",
                         "-std=c11"],
              ":linux": ["-Iexternal/usr_local_linux/include",
                         "-Iexternal/usr_local_linux/include/cdk"],
              "//conditions:default": ["-Iexternal/usr_local/include",
                                       "-Iexternal/usr_local/include/cdk",
                                       "-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = ["//src:config",
            "//include",
            "//src/ocf",
            # "//src/ocf:client",
            "//third_party/tinycbor",
            "//third_party/cjson",
            "//third_party/coap"
    ] + select({":rpi3b": ["@cosysroot_rpi3b//:cdk"],
              ":linux": ["@usr_local_linux//:cdk"],
                         # "@usr_local_linux//:ncurses"],
              ":android": ["@cosysroot_ndk//:cdk"],
              ":wrlinux": ["@cosysroot_wrlinux//:cdk"],
              "//conditions:default": ["@usr_local//:cdk"]}),
    # linkstatic = 1,
    # linkopts = ["-lcdk"],
    srcs = ["action_dialog.c",
            "action_dialog.h",
            "client_main.c",
            "client_main.h",
            "client_ui.c",
            "client_ui.h",
            "coresource_inspector.c",
            "coresource_inspector.h",
            "coresource_mgr.c",
            "coresource_mgr.h",
            "inbound_msg_db_mgr.c",
            "inbound_msg_db_mgr.h",
            "inbound_msg_inspector.c",
            "inbound_msg_inspector.h",
            "logging.c",
            "logging.h",
            "menu.c",
            "menu.h",
            "msg_scrollers.c",
            "msg_scrollers.h",
            "outbound_msg_db_mgr.c",
            "outbound_msg_db_mgr.h",
            # "subwindow.c",
            # "subwindow.h"
    ]
)
