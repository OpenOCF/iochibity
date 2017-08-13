cc_library(
    name = "ppl",
    srcs = ["oic_malloc.c",
            "oic_malloc.h",
            "ocrandom.c",
            "ocrandom.h",
            "ocrandom_seed.c",
            "oic_string.c",
            "oic_string.h",
            "oic_time.c",
            "oic_time.h",
            "oc_uuid.c",
            "oc_uuid.h",
            "platform_features.h"]
    + select({
                ":windows": ["getopt.c",
                             "getopt.h",
                             "pthread_create.c",
                             "pthread_create.h",
                             "snprintf.c",
                             "vs12_snprintf.h",
                             "win_sleep.c",
                             "win_sleep.h"],
                "//conditions:default": []})
 )

config_setting(
    name = "windows",
    values = { "define": "platform=windows" }
)
