cc_library(
    name = "utils",
    copts = ["-std=c11",
             "-Isrc",          # iotivity_config.h
             "-Isrc/comm/api",  # cacommon.h
             "-Isrc/util", # logger.h
             "-Ithird_party/coap"],
    deps  = ["//src:config",
    	     "//src/comm/api",
             "//src/util",
             "//third_party/coap"],
    hdrs = ["caadapterutils.h"],
    srcs  = ["ca_adapter_net_ssl.c",
             "caadapterutils.c",
             "cafragmentation.c"],
    visibility = ["//src/comm/common:__pkg__"]
)

