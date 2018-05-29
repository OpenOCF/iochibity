# This BUILD file defines the toolchains, using the targets in
# toolchain.BUILD. The latter defines the external repo containing the
# actual toolchains; this file provides the Bazel "view" of the toolchains.

cc_toolchain_suite(
    name = "toolchain",
    toolchains = {
        # --cpu | --compiler : <cc_toolchain name>

        # --config=rpi-arm8
        "arm64-v8a|gcc": "cc-compiler-armv8-rpi3",

        # --config=linux-x86_64
        "x86_64|gcc": "cc-compiler-linux-gnu",
    },
)


#### ctng toolchain: armv8-rpi3-linux-gnueabihf
cc_toolchain(
    name = "cc-compiler-linux-gnu",
    cpu = "x86_64",

    # all system binaries, libraries and includes needed
    #all_files = "@toolchain_wrlinux//:all-files",
    all_files = "@ctng//:x86_64-all-files",

    # # gcc, as, and ld?
    compiler_files = "@ctng//:x86_64-compiler",

    # # ar, gcc, ld?
    linker_files = "@ctng//:x86_64-linker",

    # # debug stuff - see https://gcc.gnu.org/wiki/DebugFission
    dwp_files = "@ctng//:empty",

    # # runtime files - see libc/gcc crt mini-faq at https://dev.gentoo.org/~vapier/crt.txt
    # # also https://gcc.gnu.org/onlinedocs/gccint/Initialization.html

    # # a label that references the library files needed to dynamically link
    # # the C/C++ runtime (i.e. libgcc_s.so, libstdc++.so) for the target architecture.
    dynamic_runtime_libs = ["@ctng//:x86_64-unknown-linux-gnu-dynamic-runtime-libs"],

    # # a label that references the library files needed to statically link
    # # the C/C++ runtime (i.e. crt*.o, libgcc.a, libgcc_eh.a, libstdc++.a, etc.)
    # # for the target architecture.
    static_runtime_libs = ["@ctng//:x86_64-unknown-linux-gnu-static-runtime-libs"],

    objcopy_files = "@ctng//:x86_64-objcopy",
    strip_files = "@ctng//:x86_64-strip",
    supports_param_files = 1,
    visibility = ["//visibility:public"],
)

####  ctng toolchain: armv8-rpi3-linux-gnueabihf
cc_toolchain(
    name = "cc-compiler-armv8-rpi3",
    cpu = "arm64-v8a",
    all_files = "@ctng//:rpi-arm8-all-files",
    compiler_files = "@ctng//:rpi-arm8-compiler",
    linker_files = "@ctng//:rpi-arm8-linker",
    objcopy_files = "@ctng//:rpi-arm8-objcopy",
    strip_files = "@ctng//:rpi-arm8-strip",
    dynamic_runtime_libs = ["@ctng//:rpi-arm8-dynamic-runtime-libs"],
    static_runtime_libs = ["@ctng//:rpi-arm8-static-runtime-libs"],
    supports_param_files = 1,
    # dwp: debugging stuff, https://gcc.gnu.org/wiki/DebugFission
    dwp_files = "@ctng//:empty",
    visibility = ["//visibility:public"],
)


