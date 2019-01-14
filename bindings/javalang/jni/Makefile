default:
	gcc -std=c11 \
	-dynamiclib \
	-I${TARGET_JDK_HOME}/include \
	-I${TARGET_JDK_HOME}/include/darwin \
	-I${INSTALL_SYSROOT}/include \
	-Wl,-install_name,/Users/gar/iotivity/kit/java/jni-c/libocfjni-c.jnilib \
	-L${INSTALL_SYSROOT}/lib \
	-lchibity \
	-o libocfjni-c.jnilib \
	ocfjni.c
