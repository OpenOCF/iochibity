#ifndef jni_utils_h
#define jni_utils_h

#include <stdbool.h>

#include <jni.h>

void print_class_name(JNIEnv* env, jobject obj);

jobject int2Integer(JNIEnv* env, int i);

jobject bool2Boolean(JNIEnv* env, bool b);

jobject double2Double(JNIEnv* env, double d);

#endif
