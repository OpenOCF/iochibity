/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class openocf_utils_PropertyMap */

#ifndef _Included_openocf_utils_PropertyMap
#define _Included_openocf_utils_PropertyMap
#ifdef __cplusplus
extern "C" {
#endif
#undef openocf_utils_PropertyMap_serialVersionUID
#define openocf_utils_PropertyMap_serialVersionUID 362498820763181265LL
#undef openocf_utils_PropertyMap_DEFAULT_INITIAL_CAPACITY
#define openocf_utils_PropertyMap_DEFAULT_INITIAL_CAPACITY 16L
#undef openocf_utils_PropertyMap_MAXIMUM_CAPACITY
#define openocf_utils_PropertyMap_MAXIMUM_CAPACITY 1073741824L
#undef openocf_utils_PropertyMap_DEFAULT_LOAD_FACTOR
#define openocf_utils_PropertyMap_DEFAULT_LOAD_FACTOR 0.75f
#undef openocf_utils_PropertyMap_TREEIFY_THRESHOLD
#define openocf_utils_PropertyMap_TREEIFY_THRESHOLD 8L
#undef openocf_utils_PropertyMap_UNTREEIFY_THRESHOLD
#define openocf_utils_PropertyMap_UNTREEIFY_THRESHOLD 6L
#undef openocf_utils_PropertyMap_MIN_TREEIFY_CAPACITY
#define openocf_utils_PropertyMap_MIN_TREEIFY_CAPACITY 64L
/*
 * Class:     openocf_utils_PropertyMap
 * Method:    getProp
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_openocf_utils_PropertyMap_getProp
  (JNIEnv *, jobject, jstring);

/*
 * Class:     openocf_utils_PropertyMap
 * Method:    setProp
 * Signature: (Ljava/lang/String;Ljava/lang/Object;J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_openocf_utils_PropertyMap_setProp
  (JNIEnv *, jobject, jstring, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif