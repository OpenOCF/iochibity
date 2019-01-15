cc_library(
    name = "darwin",
    copts = ["-Isrc",
             "-Isrc/util",
	     "-Ithird_party/coap/include",
	     "-Iexternal/openocf/third_party/coap/include",
             "-Iexternal/local_jdk/include",
             "-Iexternal/local_jdk/include/darwin",
             "-std=c11",
             "-UDEBUG"],
    deps = ["//src:config",
            "//src/logger",
            "//src/util"],
    hdrs = [#"ocatomic_darwin.h",
            "ocevent_darwin.h"],
    srcs = [#"ocatomic_darwin.c",
            "ocevent_darwin.c"],
    visibility = ["//visibility:public"]
)
