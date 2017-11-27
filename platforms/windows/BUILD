package(default_visibility = ['//visibility:public'])

# This is the entry point for --crosstool_top.  Toolchains are found
# by lopping off the name of --crosstool_top and searching for
# 'cc-compiler-${CPU}' in this BUILD file, where CPU is the target CPU
# specified in --cpu.
#
cc_toolchain_suite(
    name = "toolchains",
    # target_cpu | compiler
    toolchains = {
        "x64_windows|mingw64-gcc": "cc-compiler-x64_windows",
        "x64_windows_msys|mingw64-gcc": "cc-compiler-x64_windows_msys",
        "x64_windows_msvc|cl": "cc-compiler-x64_windows_msvc",
    },
)

cc_toolchain(
    name = "cc-compiler-x64_windows",
    cpu = "x64_windows",
    all_files = "@toolchain_mingw64//:everything",
    compiler_files = "@toolchain_mingw64//:compile",
    linker_files = "@toolchain_mingw64//:link",
    dwp_files = "@toolchain_mingw64//:empty",
    dynamic_runtime_libs = ["@toolchain_mingw64//:link"],
    static_runtime_libs = ["@toolchain_mingw64//:link"],
    objcopy_files = "@toolchain_mingw64//:objcopy",
    strip_files = "@toolchain_mingw64//:strip",
    supports_param_files = 0,
)

# cc_toolchain(
#     name = "cc-compiler-x64_windows_msys",
#     all_files = ":empty",
#     compiler_files = ":empty",
#     cpu = "x64_windows_msys",
#     dwp_files = ":empty",
#     dynamic_runtime_libs = [":empty"],
#     linker_files = ":empty",
#     objcopy_files = ":empty",
#     static_runtime_libs = [":empty"],
#     strip_files = ":empty",
#     supports_param_files = 0,
# )

# cc_toolchain(
#     name = "cc-compiler-armeabi-v7a",
#     all_files = ":linaro_linux_all_files",
#     compiler_files = ":linaro_linux_compiler_files",
#     cpu = "armeabi-v7a",
#     dwp_files = ":empty",
#     dynamic_runtime_libs = [":empty"],
#     linker_files = ":linaro_linux_linker_files",
#     objcopy_files = "//tools/mingw/linaro_linux_gcc:objcopy",
#     static_runtime_libs = [":empty"],
#     strip_files = "//tools/mingw/linaro_linux_gcc:strip",
#     supports_param_files = 1,
#     visibility = ["//visibility:public"],
# )

# cc_toolchain(
#     name = "cc-compiler-k8",
#     all_files = ":empty",
#     compiler_files = ":empty",
#     cpu = "local",
#     dwp_files = ":empty",
#     dynamic_runtime_libs = [":empty"],
#     linker_files = ":empty",
#     objcopy_files = ":empty",
#     static_runtime_libs = [":empty"],
#     strip_files = ":empty",
#     supports_param_files = 1,
# )
