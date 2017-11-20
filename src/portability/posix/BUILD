config_setting(
    name = "androidx86",
    values = {"fat_apk_cpu": "x86",
              # "android_crosstool_top": "//platforms/ndk:toolchain"
    }
)
config_setting(
    name = "wrlinux",
    values = { "crosstool_top": "//platforms/wrlinux:toolchain" }
)
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

# if platform != windows and defined(HAVE_LIBPTHREAD)
# NB: for now we ignore platforms lacking pthreads
cc_library(
    name = "posix",
    # copts = ["-Isrc",
    #          "-Iexternal/openocf/src",
    #          "-Isrc/logger",
    #          "-Iexternal/openocf/src/logger",
    #          "-Isrc/portability"]
    #          # "-Ithird_party/mbedtls/include"]
    copts =  select({"windows": [],
                     "msvc": [],
                     "msys": [],
                     "//conditions:default": ["-std=c11",
                                              "-U DEBUG"]}), # OS X
    deps = [#"//src:config",
            "//src/logger",
            # "//src/portability:hdrs",
            "//third_party/mbedtls"],
    linkopts = select({"windows": [],
                       "msvc": [],
                       "msys": ["-lpthread"],
                       "wrlinux": ["-lpthread"],
                       # "android": [],
                       "androidx86": [],
                       #"ndk": [],
                       "//conditions:default": []}),
    srcs = [#"oic_platform.c",
            #"oic_platform.h",
            "ocatomic_posix.c",
            "ocatomic_posix.h",
            "ocevent_posix.c",
            "ocevent_posix.h",
            "octhread_posix.c",
            "octhread_posix.h",
            "caipserver_posix.c",
            "caipserver_posix.h",
    ],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "posixFOO",
    copts = [#"-I.",
             "-Isrc",
             # "-Iexternal/openocf/src/",
             "-Isrc/util",
             "-Iexternal/openocf/src/util",
    	     # "-Isrc/comm",
             # "-Iexternal/openocf/src/comm",
    	     # "-Isrc/comm/transport/util",
             # "-Iexternal/openocf/src/comm/transport/util",
    	     # # "-Isrc/comm/api",
             # # "-Iexternal/openocf/src/comm/api",
    	     # "-Isrc/comm/common",
             # "-Iexternal/openocf/src/comm/common",
    	     # # "-Isrc/comm/interface",
             # # "-Iexternal/openocf/src/comm/interface",
	     # "-Isrc/comm/transport/udpip",
             # "-Iexternal/openocf/src/comm/transport/udpip",
    	     # "-Isrc/comm/security",
             # "-Iexternal/openocf/src/comm/security",
	     # "-Isrc/comm/util",
             # "-Iexternal/openocf/src/comm/util",
             # "-Isrc/logger",
             # "-Iexternal/openocf/src/logger",
	     # "-Isrc/portability",
             # "-Iexternal/openocf/src/portability",
	     "-Ithird_party/coap/include",
	     "-Iexternal/openocf/third_party/coap/include",
             "-Ithird_party/mbedtls/include",
             "-Iexternal/openocf/third_party/mbedtls/include",
             # "-Iexternal/local_jdk/include",
             # "-Iexternal/local_jdk/include/darwin"
    ] + select({":windows": [],
              ":msvc": [],
              ":msys": [],
              "//conditions:default": ["-std=c11",
                                       "-U DEBUG"]}), # OS X
    deps = ["//src:config",
            # "//src/logger",
            "//src/util",
    	    # "//src/comm/api",
    	    # "//src/comm/common",
    	    # "//src/comm:hdrs",
    	    # "//src/comm/interface",
    	    # "//src/comm/transport/udpip:hdrs",
    	    # "//src/comm/transport/util",
            # "//src/comm/security",
            # "//src/portability",
	    "//third_party/coap",
	    "//third_party/mbedtls",
            ],
    srcs = ["caipnwmonitor_posix.c",
            "caipnwmonitor_posix.h",
            "caipserver_posix.c",
            "caipserver_posix.h",
            #"monitor_impl_posix.c"
    ],
    visibility = ["//visibility:public"]
)
