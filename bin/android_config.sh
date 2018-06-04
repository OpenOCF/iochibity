#!/bin/sh

# $1 = target arch:  arm7 | arm8 | x86

set -x

# third_party/coap requires --disable-examples

# android builds look like this:
# external/androidndk/ndk/toolchains/llvm/prebuilt/darwin-x86_64/bin/clang
# -gcc-toolchain external/androidndk/ndk/toolchains/aarch64-linux-android-4.9/prebuilt/darwin-x86_64
# -target aarch64-none-linux-android
# '-D__ANDROID_API__=26'
# '--sysroot=external/androidndk/ndk/platforms/android-26/arch-arm64'
# -isystemexternal/androidndk/ndk/sysroot/usr/include/aarch64-linux-android
# -isystemexternal/androidndk/ndk/sysroot/usr/include

# NOTES: https://gcc.gnu.org/onlinedocs/gcc/Directory-Options.html
# --sysroot is for both headers and libs; android toolchain
# puts only headers in ndk/sysroot, libs are in
# ndk/platform/android-XX.

# -isystem is for headers only

# FIXME: --build is the build host, sense this?
# OS X:  uname -m = x86_64, uname -s = Darwin
# extract it from $ bash --version
# --build=x86_64-apple-darwin \

export PATH=${ANDROID_SDK_ROOT}/ndk-bundle/toolchains/aarch64-linux-android-4.9/prebuilt/darwin-x86_64:$PATH
export CC=clang
#export AR=llvm-ar

export CPPFLAGS="-P -isystem${HOME}/sdk/android/ndk-bundle/sysroot/usr/include/aarch64-linux-android"


#--with-sysroot=${ANDROID_NDK_HOME}/sysroot \

./configure --build=x86_64-apple-darwin \
	    --host=aarch64-none-linux-android \
	    --target=aarch64-none-linux-android \
	    --with-sysroot=${ANDROID_NDK_HOME}/platforms/android-26/arch-arm64 \
            --prefix=/usr \
	    --mandir="/usr/share/man" \
	    --without-manpages \
	    --with-shared \
	    --enable-logging \
	    --disable-examples

#            --enable-kernel=4.9.35 \
#	    --libdir="/usr/lib/arm-linux-gnueabihf" \

#	    --enable-malloc-debug \

./bin/android_mkhdrs_$1.sh
