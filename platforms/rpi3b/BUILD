
cc_toolchain_suite(
    name = "toolchain",
    toolchains = {
        # --cpu | --compiler : <cc_toolchain name>
        "armv8|gcc": "cc-compiler-armv8-rpi3",
        # "k8|compiler": "cc-compiler-k8",
    },
)


cc_toolchain(
    name = "cc-compiler-armv8-rpi3",
    all_files = "@rpi3b_repo//:everything",
    compiler_files = "@rpi3b_repo//:compile",  # "@rpi3b_repo//:gcc-armv8-toolchain",
    cpu = "armv8",
    dwp_files = "@rpi3b_repo//:empty",
    linker_files = "@rpi3b_repo//:link", # gcc-arm-android-4.9-toolchain",
    objcopy_files = "@rpi3b_repo//:objcopy",
    dynamic_runtime_libs = ["@rpi3b_repo//:link"],
    static_runtime_libs = ["@rpi3b_repo//:link"],
    strip_files = "@rpi3b_repo//:strip",
    supports_param_files = 1,
    visibility = ["//visibility:public"],
)
