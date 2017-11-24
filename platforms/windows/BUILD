package(default_visibility = ['//visibility:public'])

# This is the entry point for --crosstool_top.  Toolchains are found
# by lopping off the name of --crosstool_top and searching for
# 'cc-compiler-${CPU}' in this BUILD file, where CPU is the target CPU
# specified in --cpu.
#
# This file group should include
#   * all cc_toolchain targets supported
#   * all file groups that said cc_toolchain might refer to,
# including the default_grte_top setting in the CROSSTOOL
# protobuf.
filegroup(
    name = "toolchain_fg",
    srcs = [
        ":cc-compiler-x64_windows",
        ":cc-compiler-armeabi-v7a",
        ":cc-compiler-k8",
        ":linaro_linux_all_files",
        "@org_linaro_components_toolchain_gcc_5_3_1//:compiler_components",
    ],
)

filegroup(
    name = "srcs",
    srcs = glob(["**"]) + [
        "//src/test/shell/bazel/testdata/bazel_toolchain_test_data/tools/mingw/linaro_linux_gcc:srcs",
    ],
    visibility = ["//src/test/shell/bazel/testdata/bazel_toolchain_test_data:__pkg__"],
)

cc_toolchain_suite(
    name = "toolchains",
    # target_cpu | compiler
    toolchains = {
        "x64_windows|mingw64-gcc": "cc-compiler-x64_windows",
        "k8|compiler": "cc-compiler-k8",
    },
)

filegroup(
    name = "linaro_linux_all_files",
    srcs = [
        "//tools/mingw/linaro_linux_gcc:tool-wrappers",
        "@org_linaro_components_toolchain_gcc_5_3_1//:compiler_pieces",
    ],
)

filegroup(
    name = "linaro_linux_linker_files",
    srcs = [
        "//tools/mingw/linaro_linux_gcc:ar",
        "//tools/mingw/linaro_linux_gcc:gcc",
        "//tools/mingw/linaro_linux_gcc:ld",
        "@org_linaro_components_toolchain_gcc_5_3_1//:compiler_pieces",
    ],
)

filegroup(
    name = "linaro_linux_compiler_files",
    srcs = [
        "//tools/mingw/linaro_linux_gcc:as",
        "//tools/mingw/linaro_linux_gcc:gcc",
        "//tools/mingw/linaro_linux_gcc:ld",
    ],
)

filegroup(
    name = "empty",
    srcs = [],
)

cc_toolchain(
    name = "cc-compiler-x64_windows",
    all_files = ":empty",
    compiler_files = ":empty",
    cpu = "x64_windows_msys",
    dwp_files = ":empty",
    dynamic_runtime_libs = [":empty"],
    linker_files = ":empty",
    objcopy_files = ":empty",
    static_runtime_libs = [":empty"],
    strip_files = ":empty",
    supports_param_files = 0,
)

cc_toolchain(
    name = "cc-compiler-armeabi-v7a",
    all_files = ":linaro_linux_all_files",
    compiler_files = ":linaro_linux_compiler_files",
    cpu = "armeabi-v7a",
    dwp_files = ":empty",
    dynamic_runtime_libs = [":empty"],
    linker_files = ":linaro_linux_linker_files",
    objcopy_files = "//tools/mingw/linaro_linux_gcc:objcopy",
    static_runtime_libs = [":empty"],
    strip_files = "//tools/mingw/linaro_linux_gcc:strip",
    supports_param_files = 1,
    visibility = ["//visibility:public"],
)

cc_toolchain(
    name = "cc-compiler-k8",
    all_files = ":empty",
    compiler_files = ":empty",
    cpu = "local",
    dwp_files = ":empty",
    dynamic_runtime_libs = [":empty"],
    linker_files = ":empty",
    objcopy_files = ":empty",
    static_runtime_libs = [":empty"],
    strip_files = ":empty",
    supports_param_files = 1,
)
