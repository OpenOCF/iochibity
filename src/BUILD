cc_library(
    name = "logger",
    copts = ["-std=c11"],
    srcs = ["logger/logger.c",
    	    "logger/logger.h",
	    "logger/logger_types.h",
	    "logger/trace.c",
	    "logger/trace.h",
	    ":iotivity_config.h"]
#    visibility = ["//portability:__pkg__"]
)

cc_library(
    name = "util",
    srcs = ["util/iotivity_debug.h"]
#    visibility = ["//portability:__pkg__"]
)

cc_library(
    name = "portability",
    srcs = ["portability/iotivity_commontypes.h",
    	    "portability/oic_malloc.c",
            "portability/oic_malloc.h",
	    "portability/ocatomic.h",
	    "portability/ocevent.h",
            "portability/ocrandom.c",
            "portability/ocrandom.h",
            "portability/ocrandom_seed.c",
	    "portability/octhread.h",
            "portability/oic_string.c",
            "portability/oic_string.h",
            "portability/oic_time.c",
            "portability/oic_time.h",
            "portability/oc_uuid.c",
            "portability/oc_uuid.h",
            "portability/platform_features.h",
    	    "util/iotivity_debug.h"]
	    # "//:iotivity_config.h"]
    + select({
                ":windows": ["getopt.c",
                             "getopt.h",
			     "octhread.c",
                             "pthread_create.c",
                             "pthread_create.h",
                             "snprintf.c",
                             "vs12_snprintf.h",
                             "win_sleep.c",
                             "win_sleep.h",
			     "windows/ocatomic.c",
			     "windows/ocevent.c"],
                ":arduino": ["arduino/ocatomic.c",
			     "noop/octhread.c"],
                "//conditions:default": ["portability/gcc/ocatomic.c",
					 "portability/gcc/ocevent.c",
					 "portability/posix/octhread.c"]}),
    copts = ["-Iutil", "-Ilogger"],
    deps = [":logger",
    	    ":util"]
 )

config_setting(
    name = "arduino",
    values = { "define": "platform=arduino" }
)

config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)
