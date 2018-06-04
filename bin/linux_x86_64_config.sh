#!/bin/sh

set -x

export PATH="${PATH}:${CROSSTOOL_NG_HOME}/x86_64-unknown-linux-gnu/bin"

# export COSYSROOT=$HOME/cosysroots/linux

# echo COSYSROOT: $COSYSROOT

# bug in autoconf?
# https://changetheworldwithyourpassion.blogspot.com/2009/09/fix-undefined-rplmalloc-on-autoconf.html
# export ac_cv_func_malloc_0_nonnull=yes
# export ac_cv_func_realloc_0_nonnull=yes

HOST=x86_64-unknown-linux-gnu

./configure --build=x86_64-apple-darwin \
            --host=${HOST} \
	    --with-sysroot=${CROSSTOOL_NG_HOME}/${HOST}/${HOST}/sysroot
            --prefix=/usr \
	    --mandir="/usr/share/man" \
	    --without-manpages \
	    --with-shared \
	    --enable-logging \
	    --disable-examples \
            CPPFLAGS="-P"

            # --enable-kernel=4.9.35 \
	    # --libdir="/usr/lib/arm-linux-gnueabihf" \
	    # --with-terminfo-dirs="/etc/terminfo:/lib/terminfo:/usr/share/terminfo" \
	    # --with-default-terminfo-dir="/etc/terminfo" \

./bin/linux_x86_64_mkhdrs.sh
