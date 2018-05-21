#!/bin/sh

# $1 = target arch:  v7a | v8a | x86

set -x

# third_party/coap requires --disable-examples

# FIXME: use ndk toolchain
./configure --disable-examples \
	    --enable-logging \
	    $@ \
            CPPFLAGS="-P"

#	    --enable-malloc-debug \

./bin/android_mkhdrs_$1.sh
