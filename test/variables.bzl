COPTS_ANDROID = ["-std=c11",
                 "-x c",
]

INCLUDES = ["-Iresource/csdk/include",
            "-Iresource/c_common",
            "-Iexternal/gtest/include",
] + select({"//config:darwin": ["-UDEBUG"],
            "//conditions:default": []})

DEFDTLS = select({"//config:disable_dtls": [],
                  "//conditions:default": ["__WITH_DTLS__"]})

DEFTCP  = select({"@//config:enable_tcp": ["TCP_ADAPTER", "WITH_TCP", "__WITH_TLS__"],
                      "//conditions:default": []})

# FIXME: use nocopts to disable tls
DEFTLS  = select({"//config:disable_tls": [],
	          "//conditions:default": []})

DEFINES = DEFDTLS + DEFTCP + DEFTLS
