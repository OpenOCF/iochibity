/**
 * @file org_iochibity_Exceptions.h
 * @author Gregg Reynolds
 * @date December 2016
 *
 * @brief Functions and macros for throwing exceptions from JNI
 */
#ifndef ocf_exceptions_h
#define ocf_exceptions_h

#include <jni.h>

/* #include "openocf.h" */

#define ERR_CALLINTMETHOD  "CallIntMethod"
#define ERR_CLASS  "FindClass"
#define ERR_FLD    "GetFieldID"
#define ERR_METHOD "GetMethodID"
#define ERR_NEWOBJ "NewObject"
#define ERR_MSG(s1, s2) s1 " failed for " s2 "\n"

jint throw_no_class_def_error( JNIEnv *env, char *message );

jint throw_not_implemented( JNIEnv *env, char *message );

void throw_jni_exception(JNIEnv* env,
			 const char* functionName,
			 const char* file,
			 const int line,
			 const char* message);

void throw_stack_exception(JNIEnv* env,
			 const char* functionName,
			 const char* file,
			 const int line,
			 const int code,
			 const char* message);

void throw_eh_exception(JNIEnv* env,
			 const char* functionName,
			 const char* file,
			 const int line,
			 const int code,
			 const char* message);


/**
 * Throw macros macro assume that the current scope has a symbol named
 * env that is of the type JNIEnv*.
 */
#define THROW_JNI_EXCEPTION(message) throw_jni_exception(env, __func__, __FILE__, __LINE__, message)

#define THROW_STACK_EXCEPTION(code, message) throw_stack_exception(env, __func__, __FILE__, __LINE__, code, message)

#define THROW_EH_EXCEPTION(code, message) throw_eh_exception(env, __func__, __FILE__, __LINE__, code, message)

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define JNI_ASSERT_NULL(arg, msg, ...)		\
    if (NULL == (arg)) \
    { \
	THROW_JNI_EXCEPTION(msg);	\
        return __VA_ARGS__; \
    } \

#endif
