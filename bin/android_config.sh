#!/bin/sh

# FIXME: parameterize by arch

set -x

# third_party/coap requires --disable-examples

# FIXME: use ndk toolchain
./configure --disable-examples \
	    --enable-logging \
	    $@ \
            CPPFLAGS="-P"

#	    --enable-malloc-debug \

./bin/android_mkhdrs_v8a.sh
