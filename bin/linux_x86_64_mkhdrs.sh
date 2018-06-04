#!/bin/sh

set -x

bazel build :mkhdrs --config=linux-x86_64

makeheaders -f bazel-bin/mkhdrs.dat
makeheaders -f bazel-bin/mkhdrs.dat -H > include/openocf_h.h
