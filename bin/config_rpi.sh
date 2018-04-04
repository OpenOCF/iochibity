#!/bin/sh

# example; customize to fit your toolchain

set -x

export PATH="${PATH}:/Volumes/CrossToolNG/armv8-rpi3-linux-gnueabihf/bin"

export COSYSROOT=$HOME/cosysroots/rpi3b

echo COSYSROOT: $COSYSROOT

./configure --build=x86_64-apple-darwin \
            --host=armv8-rpi3-linux-gnueabihf \
            --enable-kernel=4.9.35 \
            --prefix=/usr \
	    --mandir="/usr/share/man" \
	    --without-manpages \
	    --with-shared \
	    --libdir="/usr/lib/arm-linux-gnueabihf" \
	    --disable-examples \
            CPPFLAGS="-P"


	    # --with-terminfo-dirs="/etc/terminfo:/lib/terminfo:/usr/share/terminfo" \
	    # --with-default-terminfo-dir="/etc/terminfo" \
