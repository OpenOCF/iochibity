cc_library(
    name = "ppl",
    deps = ["//:logger",
    	    "//:util"],
#    copts = ["-I../util"],
    srcs = ["iotivity_commontypes.h",
    	    "oic_malloc.c",
            "oic_malloc.h",
	    "ocatomic.h",
	    "ocevent.h",
            "ocrandom.c",
            "ocrandom.h",
            "ocrandom_seed.c",
	    "octhread.h",
            "oic_string.c",
            "oic_string.h",
            "oic_time.c",
            "oic_time.h",
            "oc_uuid.c",
            "oc_uuid.h",
            "platform_features.h",
    	    "//:util/iotivity_debug.h"]
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
                "//conditions:default": ["gcc/ocatomic.c",
					 "gcc/ocevent.c",
					 "posix/octhread.c"]})
 )

config_setting(
    name = "arduino",
    values = { "define": "platform=arduino" }
)

config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)
