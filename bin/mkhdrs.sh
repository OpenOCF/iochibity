set -x
bazel build :mkhdrs
makeheaders -f bazel-bin/mkhdrs.dat
makeheaders -f bazel-bin/mkhdrs.dat -H > include/openocf_h.h
