/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "caadapterutils_android.h"

#include <string.h>
#include <ctype.h>
/* #include "oic_string.h" */
/* #include "oic_malloc.h" */
#include <errno.h>
#include <inttypes.h>

/* #ifdef HAVE_WS2TCPIP_H */
/* #include <ws2tcpip.h> */
/* #endif */
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if defined(HAVE_WINSOCK2_H) && defined(HAVE_WS2TCPIP_H)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_IN6ADDR_H
#include <in6addr.h>
#endif

/* #ifdef __JAVA__ */
#if EXPORT_INTERFACE
#include <jni.h>
#endif

/**
 * @var g_jvm
 * @brief pointer to store JavaVM
 */
static JavaVM *g_jvm = NULL;

/* #ifdef __ANDROID__ */
/**
 * @var gContext
 * @brief pointer to store context for android callback interface
 */
static jobject g_Context = NULL;
static jobject g_Activity = NULL;
/* #endif */
/* #endif */

#define CA_ADAPTER_UTILS_TAG "OIC_CA_ADAP_UTILS"

/* #ifdef __JAVA__			/\* GAR: this is only used for Android? *\/ */
void CANativeJNISetJavaVM(JavaVM *jvm)
{
    OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG, "CANativeJNISetJavaVM");
    g_jvm = jvm;
}

JavaVM *CANativeJNIGetJavaVM()
{
    return g_jvm;
}

void CADeleteGlobalReferences(JNIEnv *env)
{
    OC_UNUSED(env);
/* #ifdef __ANDROID__ */
    if (g_Context)
    {
        (*env)->DeleteGlobalRef(env, g_Context);
        g_Context = NULL;
    }

    if (g_Activity)
    {
        (*env)->DeleteGlobalRef(env, g_Activity);
        g_Activity = NULL;
    }
/* #endif //__ANDROID__ */
}

jmethodID CAGetJNIMethodID(JNIEnv *env, const char* className,
                           const char* methodName,
                           const char* methodFormat)
{
    VERIFY_NON_NULL_RET(env, CA_ADAPTER_UTILS_TAG, "env", NULL);
    VERIFY_NON_NULL_RET(className, CA_ADAPTER_UTILS_TAG, "className", NULL);
    VERIFY_NON_NULL_RET(methodName, CA_ADAPTER_UTILS_TAG, "methodName", NULL);
    VERIFY_NON_NULL_RET(methodFormat, CA_ADAPTER_UTILS_TAG, "methodFormat", NULL);

    jclass jni_cid = (*env)->FindClass(env, className);
    if (!jni_cid)
    {
        OIC_LOG_V(ERROR, CA_ADAPTER_UTILS_TAG, "jni_cid [%s] is null", className);
        CACheckJNIException(env);
        return NULL;
    }

    jmethodID jni_midID = (*env)->GetMethodID(env, jni_cid, methodName, methodFormat);
    if (!jni_midID)
    {
        OIC_LOG_V(ERROR, CA_ADAPTER_UTILS_TAG, "jni_midID [%s] is null", methodName);
        CACheckJNIException(env);
        (*env)->DeleteLocalRef(env, jni_cid);
        return NULL;
    }

    (*env)->DeleteLocalRef(env, jni_cid);
    return jni_midID;
}

bool CACheckJNIException(JNIEnv *env)
{
    if ((*env)->ExceptionCheck(env))
    {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return true;
    }
    return false;
}

/* #ifdef __ANDROID__ */
void CANativeJNISetContext(JNIEnv *env, jobject context)
{
    OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG, "CANativeJNISetContext");

    if (!context)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "context is null");
        return;
    }

    if (!g_Context)
    {
        g_Context = (*env)->NewGlobalRef(env, context);
    }
    else
    {
        OIC_LOG(INFO, CA_ADAPTER_UTILS_TAG, "context is already set");
    }
}

jobject CANativeJNIGetContext()
{
    return g_Context;
}

void CANativeSetActivity(JNIEnv *env, jobject activity)
{
    OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG, "CANativeSetActivity");

    if (!activity)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "activity is null");
        return;
    }

    if (!g_Activity)
    {
        g_Activity = (*env)->NewGlobalRef(env, activity);
    }
    else
    {
        OIC_LOG(INFO, CA_ADAPTER_UTILS_TAG, "activity is already set");
    }
}

jobject *CANativeGetActivity()
{
    return g_Activity;
}
/* #endif //__ANDROID__ */
/* #endif //JAVA__ */
