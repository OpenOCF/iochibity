#!/usr/bin/env sh

# fixup dynamic links. maybe not needed if DYLD_LIBRARY_PATH correct?

install_name_tool \
    -change \
    out/darwin/x86_64/release/resource/oc_logger/liboc_logger.dylib \
    /Users/gar/iotivity/iotivation/libiotivity/out/darwin/x86_64/release/resource/oc_logger/liboc_logger.dylib \
    /Users/gar/iotivity/iotivation/libiotivity/out/darwin/x86_64/release/libocstack-jni.jnilib


install_name_tool \
    -change \
    out/darwin/x86_64/release/resource/src/liboc.dylib \
    /Users/gar/iotivity/iotivation/libiotivity/out/darwin/x86_64/release/liboc.dylib \
    /Users/gar/iotivity/iotivation/libiotivity/out/darwin/x86_64/release/libocstack-jni.jnilib

install_name_tool \
    -change \
    out/darwin/x86_64/release/resource/csdk/liboctbstack.dylib \
    /Users/gar/iotivity/iotivation/libiotivity/out/darwin/x86_64/release/liboctbstack.dylib \
    /Users/gar/iotivity/iotivation/libiotivity/out/darwin/x86_64/release/liboc.dylib

install_name_tool \
    -change \
    out/darwin/x86_64/release/resource/oc_logger/liboc_logger.dylib \
    /Users/gar/iotivity/iotivation/libiotivity/out/darwin/x86_64/release/resource/oc_logger/liboc_logger.dylib \
    /Users/gar/iotivity/iotivation/libiotivity/out/darwin/x86_64/release/liboc.dylib
