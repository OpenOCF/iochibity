package(default_visibility = ["//visibility:public"])

filegroup(
    name = "toolchain_fg",
    srcs = [
        # ":cc-compiler-armeabi-v7a",
        ":empty",
        ":everything",
        # "//android-ndk/platforms/android-23/arch-arm:everything",
    ],
)

cc_library(
    name = "malloc",
    srcs = [],
)


filegroup(
    name = "compile",
    srcs = glob(
        [
            "x86_64-w64-mingw32/include/**/*.h"
        ],
    ),
)


# filegroup(
#   name = 'compiler_pieces',
#   srcs = glob([
#     'armv7l-tizen-linux-gnueabi/**',
#     'libexec/**',
#     'lib/gcc/armv7l-tizen-linux-gnueabi/**',
#     'include/**',
#   ]),
# )

filegroup(
    name = "link",
    srcs = glob(
        [
            "lib/gcc/x86_64-w64-mingw32/7.2.0/*.a",
            "lib/*.o",
        ],
    )
)

filegroup(
    name = "objcopy",
    srcs = [
        "bin/objcopy"
    ],
)

filegroup(
    name = "strip",
    srcs = [
        "bin/strip"
    ],
)

filegroup(
    name = "gcc-armv8-toolchain",
    srcs = glob([
        "armv8-rpi3-linux-gnueabihf/**",
    ]),
    output_licenses = ["unencumbered"],
)

filegroup(
    name = "everything",
    srcs = [
        ":compile",
        ":link"
    ],
)

filegroup(
    name = "empty",
    srcs = [],
)
