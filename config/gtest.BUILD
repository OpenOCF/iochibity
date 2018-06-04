cc_binary(
    name = "sample1",
    srcs = ["samples/sample1.cc", "samples/sample1.h"],
    deps = [":gtest_main"]
)

cc_library(
    name = "gtest",
    copts = ["-Iexternal/gtest/include",
             "-Iexternal/gtest/include/gtest",
             "-Iexternal/gtest"],
    # gtest-all.cc includes all of the other cc files
    hdrs = ["src/gtest.cc",
            "src/gtest-death-test.cc",
            "src/gtest-filepath.cc",
            "src/gtest-port.cc",
            "src/gtest-printers.cc",
            "src/gtest-test-part.cc",
            "src/gtest-typed-test.cc"],
    srcs = ["src/gtest-all.cc"]
         + glob(["src/*.h"])
         + glob(["include/**/*.h"])  # + glob(["include/gtest/internal/*.h"])
)

cc_library(
    name = "gtest_main",
    copts = ["-Iexternal/gtest/include",
             "-Iexternal/gtest/include/gtest",
             "-Iexternal/gtest"],
    hdrs = ["include/gtest/gtest.h"],
    srcs = ["src/gtest_main.cc"]
         + glob(["include/**/*.h"]),  # + glob(["include/gtest/internal/*.h"])
    deps = [":gtest"],
    visibility = ["//visibility:public"]
)
