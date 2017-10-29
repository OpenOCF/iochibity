/* ****************************************************************
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

/**
 * @file
 *
 * This file contains common utility function for CA transport adaptors.
 */

#ifndef CA_ADAPTER_UTILS_H_
#define CA_ADAPTER_UTILS_H_

#include "iotivity_config.h"

#include <stdbool.h>
#ifdef __JAVA__
#include <jni.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#if defined(HAVE_WINSOCK2_H) && defined(HAVE_WS2TCPIP_H)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#include "cacommon.h"
#include "logger.h"
#include "coap/pdu.h"
#include "uarraylist.h"
#include "cacommonutil.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Length of network interface name.
 */
#define CA_INTERFACE_NAME_SIZE 16

/**
 * Macro to allocate memory for ipv4 address in the form of uint8_t.
 */
#define IPV4_ADDR_ONE_OCTECT_LEN 4

#ifdef __JAVA__
/**
 * To set jvm object.
 * This must be called by the Android API before CA Initialization.
 * @param[in]   jvm         jvm object.
 */
void CANativeJNISetJavaVM(JavaVM *jvm);

/**
 * To get JVM object.
 * Called from adapters to get JavaVM object.
 * @return  JVM object.
 */
JavaVM *CANativeJNIGetJavaVM();

/**
 * get method ID for method Name and class
 * @param[in]   env              JNI interface pointer.
 * @param[in]   className        android class.
 * @param[in]   methodName       android method name.
 * @param[in]   methodFormat     method type of methodName.
 * @return      jmethodID        iD of the method.
 */
jmethodID CAGetJNIMethodID(JNIEnv *env, const char* className,
                           const char* methodName,
                           const char* methodFormat);

/**
 * check JNI exception occurrence
 * @param[in]   env              JNI interface pointer.
 * @return  true(occurrence) or false(no occurrence).
 */
bool CACheckJNIException(JNIEnv *env);

/**
 * To Delete other Global References
 * Called during CATerminate to remove global references
 */
void CADeleteGlobalReferences();

#ifdef __ANDROID__
/**
 * To set context of JNI Application.
 * This must be called by the Android API before CA Initialization.
 * @param[in]   env         JNI interface pointer.
 * @param[in]   context     context object.
 */
void CANativeJNISetContext(JNIEnv *env, jobject context);

/**
 * To set Activity to JNI.
 * This must be called by the Android API before CA Initialization.
 * @param[in]   env         JNI Environment pointer.
 * @param[in]   activity    Activity object.
 */
void CANativeSetActivity(JNIEnv *env, jobject activity);

/**
 * To get context.
 * Called by adapters to get Application context.
 * @return  context object.
 */
jobject CANativeJNIGetContext();

/**
 * To get Activity.
 * Called from adapters to get Activity.
 * @return  Activity object.
 */
jobject *CANativeGetActivity();
#endif
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif  /* CA_ADAPTER_UTILS_H_ */

