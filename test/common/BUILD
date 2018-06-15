load("//test:variables.bzl", "DEFINES", "INCLUDES")

cc_binary(name = "oic_time_tests", visibility = ["//visibility:public"],
    copts = INCLUDES + ["-Isrc/portability"],
    defines = DEFINES,
    srcs = select({"//config:darwin": ["oic_time_tests.cpp", "oic_time_tests.hpp"],
                   "//config:linux": ["oic_time_tests.cpp"],
                   "//conditions:default": ["NOT IMPLEMENTED"]}),
    deps = ["@gtest//:gtest_main", "//:openocf"]
)

cc_binary(name = "oic_string_tests", visibility = ["//visibility:public"],
    copts = INCLUDES + ["-Isrc/portability"],
    defines = DEFINES,
    srcs = select({"//config:darwin": ["oic_string_tests.cpp", "oic_string_tests.hpp"],
                   "//config:linux": ["oic_string_tests.cpp", "oic_string_tests.hpp"],
                   "//conditions:default": ["NOT IMPLEMENTED"]}),
    deps = ["@gtest//:gtest_main", "//:openocf"]
)

cc_binary(name = "oic_malloc_tests", visibility = ["//visibility:public"],
    copts = INCLUDES + ["-Isrc/portability"],
    defines = DEFINES,
    srcs = select({"//config:darwin": ["oic_malloc_tests.cpp", "oic_malloc_tests.hpp"],
                   "//config:linux": ["oic_malloc_tests.cpp", "oic_malloc_tests.hpp"],
                   "//conditions:default": ["NOT IMPLEMENTED"]}),
    deps = ["@gtest//:gtest_main", "//:openocf"]
)

# cc_binary(name = "oic_random_tests", visibility = ["//visibility:public"],
#     copts = INCLUDES + ["-Isrc/portability"],
#     defines = DEFINES,
#     srcs = select({"//config:darwin": ["oic_random_tests.cpp", "oic_random_tests.hpp"],
#                    "//config:linux": ["oic_random_tests.cpp", "oic_random_tests.hpp"],
#                    "//conditions:default": ["NOT IMPLEMENTED"]}),
#     deps = ["@gtest//:gtest_main", "//:openocf", "//src/portability"]
# )

cc_binary(name = "randomtest", visibility = ["//visibility:public"],
    copts = INCLUDES + ["-Isrc/portability"],
    defines = DEFINES,
    srcs = select({"//config:darwin": ["randomtest.cpp", "randomtest.hpp"],
                   "//config:linux": ["randomtest.cpp", "randomtest.hpp"],
                   "//conditions:default": ["NOT IMPLEMENTED"]}),
    deps = ["@gtest//:gtest_main", "//:openocf", "//src/portability"]
)
