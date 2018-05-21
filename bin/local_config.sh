#!/bin/sh

# local config

set -x

# third_party/coap requires --disable-examples

./configure --disable-examples \
	    --enable-logging \
	    $@ \
            CPPFLAGS="-P"

./bin/local_mkhdrs.sh
