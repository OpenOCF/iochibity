# origin: https://github.com/bazelbuild/bazel/blob/master/src/test/shell/bazel/testdata/bazel_toolchain_test_data/tools/arm_compiler/BUILD.linaro

# This is the entry point for --crosstool_top.  Toolchains are found
# by lopping off the name of --crosstool_top and searching for
# 'cc-compiler-${CPU}' in this BUILD file, where CPU is the target CPU
# specified in --cpu.
#
# selection driven by --crosstool_top, --cpu (= target cpu), --compiler
#
# This file group should include
#   * all cc_toolchain targets supported
#   * all file groups that said cc_toolchain might refer to,
# including the default_grte_top setting in the CROSSTOOL
# protobuf.

cc_toolchain_suite(
    name = "toolchain",
    toolchains = {
        # --cpu | --compiler : <cc_toolchain name>
        "androideabi-4.9|gcc": "cc-compiler-androideabi-4.9",
        "k8|compiler": "cc-compiler-k8",
    },
)

filegroup(
    name = "empty",
    srcs = [],
)

# evidently we do not need this for much of anything so long as
# CROSSTOOL is correct and has a toolchain corresponding to --cpu and --compiler
cc_toolchain(
    name = "cc-compiler-androideabi-4.9",
    all_files = ":empty", # "@toolchain_ndk//:everything", # :android-armeabi-v7a-files",
    compiler_files = ":empty", # "@toolchain_ndk//:everything", #gcc-arm-android-4.9-toolchain",
    cpu = "", # "androideabi-4.9",
    dwp_files = ":empty",
    linker_files = ":empty", # "@toolchain_ndk//:link", # gcc-arm-android-4.9-toolchain",
    objcopy_files = ":empty", # "@toolchain_ndk//:objcopy",
    dynamic_runtime_libs = [":empty"], # "@toolchain_ndk//:link"],
    static_runtime_libs = [":empty"], # "@toolchain_ndk//:link"],
    strip_files = ":empty", # "@toolchain_ndk//:strip",
    supports_param_files = 1,
    visibility = ["//visibility:public"],
)
