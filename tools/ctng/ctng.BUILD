# This file exposes the resources of the crosstool-NG toolchains, so
# that CROSSTOOL may refer to them.

# These filegroups will be passed to CROSSTOOL via the cc_toolchain
# rules in ctng/BUILD. Any files needed by CROSSTOOL must be exposed
# here. We need one set of filegroups per toolchain.

# So we need:
#    one filegroup per CROSSTOOL toolpath
#    filegroups for the headers and binaries

package(default_visibility = ["//visibility:public"])


filegroup(name = "empty", srcs = [], visibility = ["//visibility:public"])

################################################################
#### Toolchain:  x86_64-unknown-linux-gnu

# Tools: one per CROSSTOOL toolpath

# NB: two paths:
#        x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-ar
#        x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/bin/ar
filegroup(name = "x86_64-unknown-linux-gnu-ar",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-ar"])
filegroup(name = "x86_64-unknown-linux-gnu-as",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-as"])
filegroup(name = "x86_64-unknown-linux-gnu-cpp",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-cpp"])
filegroup(name = "x86_64-unknown-linux-gnu-dwp",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-dwp"])
filegroup(name = "x86_64-unknown-linux-gnu-gcc",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-gcc"])
filegroup(name = "x86_64-unknown-linux-gnu-gcov",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-gcov"])
filegroup(name = "x86_64-unknown-linux-gnu-ld",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-ld"])
filegroup(name = "x86_64-unknown-linux-gnu-nm",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-nm"])
filegroup(name = "x86_64-unknown-linux-gnu-objcopy",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-objcopy"])
filegroup(name = "x86_64-unknown-linux-gnu-objdump",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-objdump"])
filegroup(name = "x86_64-unknown-linux-gnu-strip",
          srcs = ["x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-strip"])

# Include dirs:
filegroup(name = "x86_64-unknown-linux-gnu-includedirs"
          srcs = glob([
              "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/usr/include/**",
              "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/include/c++/6.3.0/**"]))

            # "x86_64-unknown-linux-gnu/include/**/*h",
            # "x86_64-unknown-linux-gnu/lib/gcc/x86_64-unknown-linux-gnu/6.3.0/include/**/*.h",
            # "x86_64-unknown-linux-gnu/lib/gcc/x86_64-unknown-linux-gnu/6.3.0/include-fixed/**/*.h",
            # "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/include/**/*.h",
            # "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/usr/include/*.h",
            # "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/usr/include/*.h",
            # "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/include/c++/**",


filegroup(name = "x86_64-unknown-linux-gnu-binaries"
          srcs = glob([
              "x86_64-unknown-linux-gnu/lib/gcc/x86_64-unknown-linux-gnu/6.3.0/include",
          ]))

# x86_64-unknown-linux-gnu/lib/gcc/x86_64-unknown-linux-gnu/6.3.0/include-fixed"
# x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/usr/include"

filegroup(
    name = "x86_64-unknown-linux-gnu-static-runtime-libs",
    srcs = glob([
        "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/lib/gcc/x86_64-unknown-linux-gnu/6.3.0/crt*.o",
        "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/usr/lib/crt*.o",
        "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/lib/*.a",
        "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/usr/lib/*.a"
    ]),
    output_licenses = ["unencumbered"],
    visibility = ["//visibility:public"]
)

filegroup(
    name = "x86_64-unknown-linux-gnu-dynamic-runtime-libs",
    srcs = glob([
        # "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/usr/lib/crt*.o"
        "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/lib/**/*.so",
        "x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/x86_64-unknown-linux-gnu/sysroot/usr/lib/**/*.so"
    ]),
    output_licenses = ["unencumbered"],
    visibility = ["//visibility:public"]
)

filegroup(
    name = "x86_64-all-files",
    srcs = [
        ":x86_64-unknown-linux-gnu-ar",
        ":x86_64-unknown-linux-gnu-as",
        ":x86_64-unknown-linux-gnu-cpp",
        ":x86_64-unknown-linux-gnu-dwp",
        ":x86_64-unknown-linux-gnu-gcc",
        ":x86_64-unknown-linux-gnu-ld",
        ":x86_64-unknown-linux-gnu-nm",
        ":x86_64-unknown-linux-gnu-objcopy",
        ":x86_64-unknown-linux-gnu-objdump",
        ":x86_64-unknown-linux-gnu-strip",
        ":x86_64-unknown-linux-gnu-includedirs",
        ":x86_64-unknown-linux-gnu-binaries",
        ":x86_64-unknown-linux-gnu-static-runtime-libs",
        ":x86_64-unknown-linux-gnu-dynamic-runtime-libs",
    ],
    visibility = ["//visibility:public"]
)

################################################################
####  toolchain: armv8-rpi3-linux-gnueabihf

## NB: The ctng tools already know where to find things, so we don't
## really need this stuff. To see the search paths try e.g.
# echo | /Volumes/CrossToolNG/armv8-rpi3-linux-gnueabihf/bin/armv8-rpi3-linux-gnueabihf-gcc -E -Wp,-v -

filegroup(
    name = "rpi-arm8-all-files",
    srcs = [
        ":rpi-arm8-compiler",
        ":rpi-arm8-linker"
    ],
    visibility = ["//visibility:public"]
)

filegroup(
    name = "rpi-arm8-gcc",
    srcs = 
        [
            "armv8-rpi3-linux-gnueabihf/bin/armv8-rpi3-linux-gnueabihf-gcc",
        ],
)

filegroup(
    name = "rpi-arm8-gcc",
    srcs = 
        [
            "armv8-rpi3-linux-gnueabihf/bin/armv8-rpi3-linux-gnueabihf-gcc",
            #"armv8-rpi3-linux-gnueabihf/sysroot/**/*.h",
            # "toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/**/*.h",
            # "platforms/android-23/arch-arm/**/*.h",
        ],
)

filegroup(
  name = 'gcc',
  srcs = [
      ':rpi-arm8-gcc',
      "armv8-rpi3-linux-gnueabihf-gcc"
  ],
)

filegroup(
    name = "rpi-arm8-linker",
    srcs = glob(
        [
            # "armv8-rpi3-linux-gnueabihf/lib/*.a",
            # "armv8-rpi3-linux-gnueabihf/sysroot/lib/*.a",
            # "armv8-rpi3-linux-gnueabihf/sysroot/usr/lib/**/*.a",
            # "lib/gcc/armv8-rpi3-linux-gnueabihf/6.3.0/*.a",

            # "armv8-rpi3-linux-gnueabihf/sysroot/usr/lib/*.o",
            # "lib/gcc/armv8-rpi3-linux-gnueabihf/6.3.0/*.o",

            # "armv8-rpi3-linux-gnueabihf/lib/*.so",
            # "armv8-rpi3-linux-gnueabihf/sysroot/lib/*.so",
            # "armv8-rpi3-linux-gnueabihf/sysroot/usr/lib/*.so",
            # "armv8-rpi3-linux-gnueabihf/sysroot/usr/**/*.so",
            # "armv8-rpi3-linux-gnueabihf/sysroot/lib/plugin/*.so",
            # "lib/gcc/armv8-rpi3-linux-gnueabihf/6.3.0/**/*.so",
            # "lib/*.so",
            # "libexec/**/*.so",
        ],
    )
)

filegroup(
    name = "rpi-arm8-objcopy",
    srcs = [
        # "armv8-rpi3-linux-gnueabihf/sysroot/bin/armv8-rpi3-linux-gnueabihf-objcopy"
    ],
)

filegroup(
    name = "rpi-arm8-strip",
    srcs = [
        # "armv8-rpi3-linux-gnueabihf/sysroot/bin/armv8-rpi3-linux-gnueabihf-strip"
    ],
)

filegroup(
    name = "rpi-arm8-static-runtime-libs",
    srcs = glob([
    ]),
    output_licenses = ["unencumbered"],
    visibility = ["//visibility:public"]
)

filegroup(
    name = "rpi-arm8-dynamic-runtime-libs",
    srcs = glob([
    ]),
    output_licenses = ["unencumbered"],
    visibility = ["//visibility:public"]
)
