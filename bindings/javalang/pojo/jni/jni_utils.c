/**
 * @file jni_utils.c
 * @author Gregg Reynolds
 * @date December 2016
 *
 * @brief Utility functions for working in JNI
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "jni_init.h"
#include "jni_utils.h"
#include "org_iochibity_Exceptions.h"

/* print the class name */
/* http://stackoverflow.com/questions/12719766/can-i-know-the-name-of-the-class-that-calls-a-jni-c-method */

void print_class_name(JNIEnv* env, jobject obj)
{
    jclass clazz = (*env)->GetObjectClass(env,obj);
    // First get the class object
    jmethodID mid_getClass = (*env)->GetMethodID(env, clazz, "getClass", "()Ljava/lang/Class;");
    jobject clsObj = (*env)->CallObjectMethod(env, obj, mid_getClass);

    // Now get the class object's class descriptor
    jclass cls = (*env)->GetObjectClass(env, clsObj);

    // Find the getName() method on the class object
    jmethodID mid_getName = (*env)->GetMethodID(env, cls, "getName", "()Ljava/lang/String;");

    // Call the getName() to get a jstring object back
    jstring strObj = (jstring)(*env)->CallObjectMethod(env, clsObj, mid_getName);

    // Now get the c string from the java jstring object
    const char* str = (*env)->GetStringUTFChars(env, strObj, NULL);

    // Print the class name
    printf("\t%s\n", str);
    // Release the memory pinned char array
    (*env)->ReleaseStringUTFChars(env,strObj, str);
}

jobject int2Integer(JNIEnv* env, int i)
{
    printf("int2integer ENTRY\n");
    /* jclass i_klass = (*env)->FindClass(env, "java/lang/Integer"); */
    /* if (i_klass == 0) { */
    /* 	THROW_JNI_EXCEPTION("FindClass failed on java/lang/Integer\n"); */
    /* } */
    /* jmethodID ctor = (*env)->GetMethodID(env, K_INTEGER, "<init>", "(I)V"); */
    /* if (ctor == 0) { */
    /* 	THROW_JNI_EXCEPTION("GetMethodID failed on int ctor for Integer\n"); */
    /* } */
    jobject newint  = (*env)->NewObject(env, K_INTEGER, MID_INT_CTOR, i);
    if (newint == NULL) {
	THROW_JNI_EXCEPTION("NewObject failed for Integer(i)\n");
    }
    printf("int2integer EXIT\n");
    return newint;
}

jobject bool2Boolean(JNIEnv* env, bool b)
{
    printf("bool2boolean ENTRY\n");
    jclass klass = (*env)->FindClass(env, "java/lang/Boolean");
    if (klass == 0) {
	THROW_JNI_EXCEPTION("FindClass failed on java/lang/Boolean\n");
    }
    jmethodID ctor = (*env)->GetMethodID(env, klass, "<init>", "(Z)V");
    if (ctor == 0) {
	THROW_JNI_EXCEPTION("GetMethodID failed on bool ctor for Boolean\n");
    }
    jobject newb  = (*env)->NewObject(env, klass, ctor, b);
    if (newb == NULL) {
	THROW_JNI_EXCEPTION("NewObject failed for Boolean(b)\n");
    }
    printf("bool2boolean EXIT\n");
    return newb;
}

jobject double2Double(JNIEnv* env, double d)
{
    printf("double2Double ENTRY\n");
    jclass klass = (*env)->FindClass(env, "java/lang/Double");
    if (klass == 0) {
	THROW_JNI_EXCEPTION("FindClass failed on java/lang/Double\n");
    }
    jmethodID ctor = (*env)->GetMethodID(env, klass, "<init>", "(D)V");
    if (ctor == 0) {
	THROW_JNI_EXCEPTION("GetMethodID failed on d ctor for Double\n");
    }
    jobject newd  = (*env)->NewObject(env, klass, ctor, d);
    if (newd == NULL) {
	THROW_JNI_EXCEPTION("NewObject failed for Double(d)\n");
    }
    printf("double2Double EXIT\n");
    return newd;
}
