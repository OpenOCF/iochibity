#!/usr/bin/env sh

# fixup dynamic links. maybe not needed if DYLD_LIBRARY_PATH correct?

JLP=${IOTIVITY_HOME}/out/darwin/x86_64/release

if [ -d "$IOTIVITY_HOME" ];
then
        if [ ! -d "$JLP" ]
	then
	        echo "java.library.path not found: $JLP";
	        exit
	fi
else
	echo "IOTIVITY_HOME ($IOTIVITY_HOME) not found."
	exit
fi

install_name_tool \
    -change \
    out/darwin/x86_64/release/resource/src/liboc.dylib \
    $JLP/liboc.dylib \
    $JLP/libocstack-jni.jnilib

install_name_tool \
    -change \
    out/darwin/x86_64/release/resource/oc_logger/liboc_logger.dylib \
    $JLP/resource/oc_logger/liboc_logger.dylib \
    $JLP/libocstack-jni.jnilib

install_name_tool \
    -change \
    out/darwin/x86_64/release/resource/csdk/liboctbstack.dylib \
    $JLP/liboctbstack.dylib \
    $JLP/libocstack-jni.jnilib

install_name_tool \
    -change \
    out/darwin/x86_64/release/resource/oc_logger/liboc_logger.dylib \
    $JLP/resource/oc_logger/liboc_logger.dylib \
    $JLP/liboc.dylib
