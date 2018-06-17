load("@//config:variables.bzl", "ANDROID_SDK_ROOT", "CROSSTOOL_NG_HOME")

LOCAL_CONFIG  = "".join([
    " (cd external/libcoap;",
    " bash -c ./autogen.sh 2>&1;",
    " ./configure --disable-examples --disable-tests --disable-documentation);",
    " cp external/libcoap/coap_config.h $(@D);",
    " cp external/libcoap/include/coap/coap.h $(@D);"])

ANDROID_ARM7_CONFIG="".join([
    "(cd external/libcoap;",
    " bash -c ./autogen.sh 2>&1;",
    " export PATH=\"",
    ANDROID_SDK_ROOT,
    "/ndk-bundle/toolchains/arm-linux-android-4.9/prebuilt/darwin-x86_64:$$PATH;\"",
    " export CC=clang;",
    " export CPPFLAGS=\"-P -isystem",
    ANDROID_SDK_ROOT,
    "/ndk-bundle/sysroot/usr/include/arm-linux-androideabi\";",
    " ./configure",
    " --build=x86_64-apple-darwin",
    " --host=armv7-none-linux-androideabi",
    " --target=armv7-none-linux-androideabi",
    " --with-sysroot=\"",
    ANDROID_SDK_ROOT,
    "/ndk-bundle/platforms/android-26/arch-arm\"",
    " --prefix=/usr",
    " --mandir=\"/usr/share/man\"",
    " --disable-examples;);",
    " cp external/libcoap/coap_config.h $(@D);",
    " cp external/libcoap/include/coap/coap.h $(@D);",])

ANDROID_ARM8_CONFIG="".join([
    "(cd external/libcoap;",
    " bash -c ./autogen.sh 2>&1;",
    " export PATH=\"",
    ANDROID_SDK_ROOT,
    "/ndk-bundle/toolchains/aarch64-linux-android-4.9/prebuilt/darwin-x86_64:$$PATH;\"",
    " export CC=clang;",
    " export CPPFLAGS=\"-P -isystem",
    ANDROID_SDK_ROOT,
    "/ndk-bundle/sysroot/usr/include/aarch64-linux-android\";",
    " ./configure",
    " --build=x86_64-apple-darwin",
    " --host=aarch64-none-linux-android",
    " --target=aarch64-none-linux-android",
    " --with-sysroot=\"",
    ANDROID_SDK_ROOT,
    "/ndk-bundle/platforms/android-26/arch-arm64\"",
    " --prefix=/usr",
    " --mandir=\"/usr/share/man\"",
    " --disable-examples;);",
    " cp external/libcoap/coap_config.h $(@D);",
    " cp external/libcoap/include/coap/coap.h $(@D);",])

ANDROID_X86_64_CONFIG="".join([
    "(cd external/libcoap;",
    " bash -c ./autogen.sh 2>&1;",
    " export PATH=\"",
    ANDROID_SDK_ROOT,
    "/ndk-bundle/toolchains/x86_64-4.9/prebuilt/darwin-x86_64:$$PATH;\"",
    " export CC=clang;",
    " export CPPFLAGS=\"-P -isystem",
    ANDROID_SDK_ROOT,
    "/ndk-bundle/sysroot/usr/include/x86_64-linux-android\";",
    " ./configure",
    " --build=x86_64-apple-darwin",
    " --host=x86_64-none-linux-android",
    " --target=x86_64-none-linux-android",
    " --with-sysroot=\"",
    ANDROID_SDK_ROOT,
    "/ndk-bundle/platforms/android-26/arch-x86_64\"",
    " --prefix=/usr",
    " --mandir=\"/usr/share/man\"",
    " --disable-examples;);",
    " cp external/libcoap/coap_config.h $(@D);",
    " cp external/libcoap/include/coap/coap.h $(@D);",])

RPI_ARM8_CONFIG  = "".join([
    "(cd external/libcoap; ",
    "bash -c ./autogen.sh 2>&1; ",
    "export PATH=\"",
    CROSSTOOL_NG_HOME,
    "/armv8-rpi3-linux-gnueabihf/bin:$${PATH}\"; ",
    "export CC=armv8-rpi3-linux-gnueabihf-gcc;",
    " ./configure",
    " --build=x86_64-apple-darwin",
    " --host=armv8-rpi3-linux-gnueabihf",
    " --prefix=/usr",
    " --mandir=\"/usr/share/man\"",
    " --libdir=\"/usr/lib/arm-linux-gnueabihf\"",
    " --disable-examples",
    " CPPFLAGS=\"-P\";);",
    " cp external/libcoap/coap_config.h $(@D);",
    " cp external/libcoap/include/coap/coap.h $(@D);",]
)

# "    --enable-logging",
# "    --enable-kernel=4.9.35",
# "    --without-manpages",
# "    --with-shared",

TEST_CONFIG  = "".join([
    "echo 'foo'  > $@\n",
]
)

CONFIG_SRCS = ["autogen.sh",
               "configure.ac",
               "Makefile.am",
               "NEWS",
               "README",
               "AUTHORS",
               "ChangeLog",
               "doc/Doxyfile.in",
               "doc/Makefile.am",
               "examples/Makefile.am",
               "examples/coap-client.txt.in",
               "examples/coap-rd.txt.in",
               "examples/coap-server.txt.in",
               "include/coap/coap.h.in",
               "tests/Makefile.am",
               "m4/ax_check_compile_flag.m4",
               "m4/ax_check_link_flag.m4",
               "libcoap-1.pc.in"]

genrule(
    name = "config",
    srcs = CONFIG_SRCS,
    outs = ["coap_config.h", "coap.h"],
    cmd  = select({"@//config:android_arm7": ANDROID_ARM7_CONFIG,
                   "@//config:android_arm8": ANDROID_ARM8_CONFIG,
                   "@//config:android_x86_64": ANDROID_X86_64_CONFIG,
                   "@//config:rpi_arm8"     : RPI_ARM8_CONFIG,
                   "//conditions:default"   : LOCAL_CONFIG})
)

cc_library(
    name = "libcoap",
    copts = ["-std=gnu99", "-fPIC",
             '-Wall', '-ffunction-sections',
             '-fdata-sections', '-fno-exceptions',
             "-Iexternal/libcoap/include/coap",
             "-DWITH_POSIX",
             "-D_GNU_SOURCE",
             "-DWITH_UPSTREAM_LIBCOAP"]
    + select({"@//config:enable_tcp": ["-DWITH_TCP"],
              "//conditions:default": []}),

    srcs = glob(["src/**/*.c"],
                exclude=["src/**/coap_io_lwip.c"]),
    hdrs = glob(["include/**/*.h"]) + [":config"],
    visibility = ["//visibility:public"]
)
