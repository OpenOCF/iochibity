#!/bin/sh

# local config

set -x

# third_party/coap requires --disable-examples

./configure --disable-examples \
	    $@ \
            CPPFLAGS="-P"

#	    --enable-logging \

./bin/local_mkhdrs.sh
