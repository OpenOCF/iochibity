licenses(["notice"])

cc_library(
    name = "tinydtls",
    copts = ["-std=c11",
             "-Ithird_party/tinydtls"], # needed for subdir includes like sha2/sha2.c
    hdrs = ["tinydtls.h"],
    srcs = ["dtls_config.h",
            #"tinydtls.h",

            "debug.h",
            "global.h",

            "ccm.c",
            "ccm.h",

            "crypto.c",
            "crypto.h",
            "numeric.h",
            "prng.h",

            "dtls.c",
            "uthash.h",
            "dtls.h",
            "alert.h",
            "t_list.h",
            "utlist.h",

            "dtls_time.c",
            "dtls_time.h",
            "hmac.c",
            "hmac.h",
            "netq.c",
            "netq.h",

            "peer.c",
            "peer.h",
            "state.h",

            "session.c",
            "session.h",
            "aes/rijndael.c",
            "aes/rijndael.h",
            "ecc/ecc.c",
            "ecc/ecc.h",
            "sha2/sha2.h",
            "sha2/sha2.c"],
    visibility = ["//src/sec:__pkg__"]
)
