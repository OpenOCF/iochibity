#!/bin/sh

ARCH=arm
# ARCH=mips64			# mips64 compiler
# ARCH=mips			# mips GCC 4.8 compiler
# ARCH=x86			# x86 GCC 4.8 compiler
# ARCH=x86_64			# x86_64 GCC 4.8 compiler
# ARCH=mips			# mips GCC 4.8 compiler

OUTDIR=/tmp/crystax-toolchain

PLATFORM=21

SYSTEM=darwin-x86_64
# SYSTEM=linux-x86_64
# SYSTEM=windows-x86_64

# Run "make-standalone-toolchain.sh --toolchain=foo" to see a list of legal toolchains
TOOLCHAIN=arm-linux-androideabi-clang3.6
# aarch64-linux-android-4.9
# aarch64-linux-android-5
# aarch64-linux-android-clang3.6
# aarch64-linux-android-clang3.7
# arm-linux-androideabi-4.9
# arm-linux-androideabi-5
# arm-linux-androideabi-clang3.6
# arm-linux-androideabi-clang3.7
# llvm-3.6
# llvm-3.7
# mips64el-linux-android-4.9
# mips64el-linux-android-5
# mips64el-linux-android-clang3.6
# mips64el-linux-android-clang3.7
# mipsel-linux-android-4.9
# mipsel-linux-android-5
# mipsel-linux-android-clang3.6
# mipsel-linux-android-clang3.7
# renderscript
# x86-4.9
# x86-5
# x86-clang3.6
# x86-clang3.7
# x86_64-4.9
# x86_64-5
# x86_64-clang3.6
# x86_64-clang3.7

## specify EITHER --arch OR --toolchain


# IOTIVITY_NDK_HOME=${HOME}/android/android-ndk-r12b
IOTIVITY_NDK_HOME=${HOME}/android/crystax-ndk-10.3.2

# make-standalone-toolchain.sh will be replace by make_standalone_toolchain.py in NDK
#  NOTE SPELLING:  _ rather than -

## Using crystax:
${IOTIVITY_NDK_HOME}/build/tools/make-standalone-toolchain.sh \
	   --install-dir=${OUTDIR} \
	   --platform=android-${PLATFORM} \
	   --system=${SYSTEM} \
	   --toolchain=${TOOLCHAIN}

STL=gnustl
# STL=libc++
# STL=stlport

# # Using Android NDK:
# ${IOTIVITY_NDK_HOME}/build/tools/make_standalone_toolchain.py \
#	   --install-dir=${OUTDIR} \
# 	   --arch=${ARCH} \
# 	   --api=21 \
# 	   --stl=${STL} \
# 	   --v \
# 	   --force


