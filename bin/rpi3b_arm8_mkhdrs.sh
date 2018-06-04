#!/bin/sh

set -x

bazel build :mkhdrs --config=rpi-arm8

makeheaders -f bazel-bin/mkhdrs.dat
makeheaders -f bazel-bin/mkhdrs.dat -H > include/openocf_h.h
