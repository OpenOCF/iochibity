## usage: load("//config:variables.bzl", "DEFINES", "OS_COPTS")

# set config options in //user.bazelrc or //.bazelrc
# e.g. build --define enable_logging=true

CROSSTOOL_NG_HOME="/Volumes/CrossToolNG"

ANDROID_SDK_ROOT="/Users/gar/sdk/android"

CSTD = select({"//config:cstd_c11" : ["-std=c11", "-x c"],
               "//config:cstd_iso9899_2011" : ["-std=iso9899:2011", "-x c"], # = c11
               "//config:cstd_c99" : ["-std=c99", "-x c"],
               "//config:cstd_iso9899_1999" : ["-std=iso9899:1999", "-x c"], # = c99
               "//config:cstd_c90" : ["-std=c90", "-x c"],
               "//config:cstd_iso9899_1990" : ["-iso9899:1990", "-x c"], # = c90
               "//conditions:default" : ["-std=c11", "-x c"]
    })

COPTS_ANDROID = ["-x c"]

TESTINCLUDES = ["-Iresource/csdk/include",
    	        "-Iresource/c_common",
		"-Iresource/c_common/ocrandom/include",
		"-Iresource/c_common/oic_malloc/include",
                "-Iresource/c_common/oic_string/include",
                "-Iresource/c_common/oic_time/include",
    	        "-Iresource/csdk/logger/include",
    	        "-Iresource/csdk/stack/include",
    	        "-Iresource/csdk/stack/include/internal",
                "-Iexternal/gtest/include",
]

OS_COPTS = select({"//config:darwin": ["-U DEBUG"],
                   "//conditions:default": []})

TESTDEPS = ["@gtest//:gtest_main",
            "//resource/c_common",
            "//resource/csdk/logger"]


# global defines

# WARNING: using "defines" in c rules adds -D flags to target and
# everything that depends on it!

# #ifdef WITH_TCP:: comm, provisioning
# #ifdef TCP_ADAPTER: ocf, provisioning, sec/svrs, comm, comm/util, comm/util/bt, udp, tcp
# FIXME: use ENABLE_TCP, ENABLE_TLS
DEFTCP  = select({"@//config:enable_tcp": ["TCP_ADAPTER", "ENABLE_TCP", "WITH_TCP",
                                           "ENABLE_TLS", "__WITH_TLS__"],
                      "//conditions:default": []})

DEFDTLS = select({"//config:disable_dtls": [],
                  "//conditions:default": ["__WITH_DTLS__"]})

DEFTLS  = select({"//config:disable_tls": [],
                  "//conditions:default": []})

DEFLOG = select({"//config:enable_logging": ["TB_LOG"],
                 "//conditions:default": []})

DBG_THREADS = select({"//config:debug_threads": ["DEBUG_THREADS"],
	              "//conditions:default": []})

DBG_PAYLOAD = select({"//config:debug_payload": ["DEBUG_PAYLOAD"],
	              "//conditions:default": []})

DBG_TLS = select({"//config:debug_tls": ["DEBUG_TLS"],
	          "//conditions:default": []})

DBG_MSGS = select({"//config:debug_msgs": ["DEBUG_MSGS"],
	           "//conditions:default": []})

MULTIOWNER = select({"//config:multi_own": ["MULTIPLE_OWNER"],
	             "//conditions:default": []})

# libcoap hdrs etc. require WITH_POSIX on Linux
POSIX = select({"//config:linux": ["WITH_POSIX"],
                "//config:darwin": ["WITH_POSIX"],
                "//conditions:default": []})

DEFINES = DEFDTLS + DEFTCP + DEFTLS + DEFLOG + DBG_THREADS + DBG_PAYLOAD + DBG_TLS + DBG_MSGS + MULTIOWNER + POSIX

