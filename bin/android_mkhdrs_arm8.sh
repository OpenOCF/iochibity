#!/bin/sh

set -x
#bazel build :mkhdrs --fat_apk_cpu=x86 --android_cpu=x86
#bazel build :mkhdrs --fat_apk_cpu=armeabi-v7a --android_cpu=armeabi-v7a
bazel build :mkhdrs --config=android-arm8

makeheaders -f bazel-bin/mkhdrs.dat
makeheaders -f bazel-bin/mkhdrs.dat -H > include/openocf_h.h
