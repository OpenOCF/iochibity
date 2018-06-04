#!/bin/sh

# example; customize to fit your toolchain

set -x

export PATH="${PATH}:/Volumes/CrossToolNG/armv8-rpi3-linux-gnueabihf/bin"

export COSYSROOT=$HOME/cosysroots/rpi3b

echo COSYSROOT: $COSYSROOT

# bug in autoconf?
# https://changetheworldwithyourpassion.blogspot.com/2009/09/fix-undefined-rplmalloc-on-autoconf.html
# export ac_cv_func_malloc_0_nonnull=yes
# export ac_cv_func_realloc_0_nonnull=yes

./configure --build=x86_64-apple-darwin \
            --host=armv8-rpi3-linux-gnueabihf \
            --enable-kernel=4.9.35 \
            --prefix=/usr \
	    --mandir="/usr/share/man" \
	    --without-manpages \
	    --with-shared \
	    --libdir="/usr/lib/arm-linux-gnueabihf" \
	    --enable-logging \
	    --disable-examples \
            CPPFLAGS="-P"

	    # --with-terminfo-dirs="/etc/terminfo:/lib/terminfo:/usr/share/terminfo" \
	    # --with-default-terminfo-dir="/etc/terminfo" \

./bin/rpi3b_arm8_mkhdrs.sh
