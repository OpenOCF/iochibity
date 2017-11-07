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

# filegroup(
#     name = "jniheaders",
#     srcs = ["@local_jdk//:jni_header",
#             "@local_jdk//:jni_md_header-darwin"]
# )

cc_library(
    name = "android",
    copts = ["-Isrc",
             "-Iexternal/openocf/src/",
             "-Isrc/comm",
             "-Iexternal/openocf/src/comm",
             "-Isrc/comm/api",
             "-Iexternal/openocf/src/comm/api",
             "-Isrc/comm/transport/ble",
             "-Iexternal/openocf/src/comm/transport/ble",
             "-Isrc/comm/transport/ble/android",
             "-Iexternal/openocf/src/comm/transport/ble/android",
             "-Isrc/comm/transport/util",
             "-Iexternal/openocf/src/comm/transport/util",
             "-Isrc/comm/common",
             "-Iexternal/openocf/src/comm/common",
             "-Isrc/comm/interface",
             "-Iexternal/openocf/src/comm/interface",
             "-Isrc/comm/util",
             "-Iexternal/openocf/src/comm/util",
             "-Isrc/logger",
             "-Iexternal/openocf/src/logger",
    	     "-Isrc/portability",
             "-Iexternal/openocf/src/portability",
	     "-Isrc/util",
             "-Iexternal/openocf/src/util",
             "-Ithird_party/coap/include",
             "-Iexternal/openocf/third_party/coap/include",
             "-Iexternal/local_jdk/include",
             "-Iexternal/local_jdk/include/darwin"]
    + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11"]}),
    deps = ["//src:config",
            "//src/comm/transport/util",
            "//src/comm/transport/ble",
            "//src/comm/transport/ble/android",
    	    "//src/comm/common",
    	    "//src/comm/interface",
            "//src/logger",
    	    "//src/portability",
            "//src/util",
            "//third_party/coap"],
    srcs = ["caleautoconnector.c",
            "caleautoconnector.h",
            "caleconnectionmanager.c",
            "camanagerdevice.c",
            "camanagerdevice.h",
            "camanagerleutil.c",
            "camanagerleutil.h"]
    + ["@local_jdk//:jni_header",
       "@local_jdk//:jni_md_header-darwin"],
    # includes = ["external/local_jdk/include",
    #             "external/local_jdk/include/darwin"],
    visibility = ["//src/comm:__subpackages__"]
)
