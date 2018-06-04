#!/bin/sh

# $1 = target arch:  arm7 | arm8 | x86

set -x

# third_party/coap requires --disable-examples

# FIXME: use ndk toolchain
# ./configure --disable-examples \
# 	    --enable-logging \
# 	    $@ \
#             CPPFLAGS="-P"

# FIXME: --build is the build host, sense this?
# OS X:  uname -m = x86_64, uname -s = Darwin

# triple =
# --build=x86_64-apple-darwin \


./configure --host=x86_64-unknown-linux-gnu \
            --target=x86_64-unknown-linux-gnu \
            --enable-kernel=4.9.35 \
            --prefix=/usr \
	    --mandir="/usr/share/man" \
	    --without-manpages \
	    --with-shared \
	    --libdir="/usr/lib/arm-linux-gnueabihf" \
	    --enable-logging \
	    --disable-examples \
            CPPFLAGS="-P"


#	    --enable-malloc-debug \

./bin/android_mkhdrs_$1.sh
