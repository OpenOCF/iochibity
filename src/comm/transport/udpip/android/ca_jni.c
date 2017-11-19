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

#include "caipserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>
#ifdef HAVE_LINUX_IF_H
#include <linux/if.h>
#endif
/* #include <coap/utlist.h> */
#ifdef HAVE_LINUX_NETLINK_H
#include <linux/netlink.h>
#endif
#ifdef HAVE_LINUX_RNETLINK_H
#include <linux/rtnetlink.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif

#include "caadapterutils.h"
#include "caipnwmonitor.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "org_iotivity_ca_CaIpInterface.h"
#include "caifaddrs.h"

#define TAG "OIC_CA_IP_MONITOR"
#define NETLINK_MESSAGE_LENGTH  (4096)
#define IFC_LABEL_LOOP          "lo"
#define IFC_ADDR_LOOP_IPV4      "127.0.0.1"
#define IFC_ADDR_LOOP_IPV6      "::1"
/**
 * Used to storing adapter changes callback interface.
 */
static struct CAIPCBData_t *g_adapterCallbackList = NULL;

/**
 * Initialize JNI interface.
 * @return  ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
CAResult_t CAIPJniInit();

/**
 * Destroy JNI interface.
 * @return  ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
static CAResult_t CAIPDestroyJniInterface();

//#define MAX_INTERFACE_INFO_LENGTH 1024 // allows 32 interfaces from SIOCGIFCONF

CAResult_t CAIPJniInit()
{
    OIC_LOG(DEBUG, TAG, "CAIPJniInit_IN");

    JavaVM *jvm = CANativeJNIGetJavaVM();
    if (!jvm)
    {
        OIC_LOG(ERROR, TAG, "Could not get JavaVM pointer");
        return CA_STATUS_FAILED;
    }

    jobject context = CANativeJNIGetContext();
    if (!context)
    {
        OIC_LOG(ERROR, TAG, "unable to get application context");
        return CA_STATUS_FAILED;
    }

    JNIEnv* env = NULL;
    if ((*jvm)->GetEnv(jvm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        OIC_LOG(ERROR, TAG, "Could not get JNIEnv pointer");
        return CA_STATUS_FAILED;
    }

    jmethodID mid_getApplicationContext = CAGetJNIMethodID(env, "android/content/Context",
                                                           "getApplicationContext",
                                                           "()Landroid/content/Context;");

    if (!mid_getApplicationContext)
    {
        OIC_LOG(ERROR, TAG, "Could not get getApplicationContext method");
        return CA_STATUS_FAILED;
    }

    jobject jApplicationContext = (*env)->CallObjectMethod(env, context,
                                                           mid_getApplicationContext);
    if (!jApplicationContext)
    {
        OIC_LOG(ERROR, TAG, "Could not get application context");
        return CA_STATUS_FAILED;
    }

    jclass cls_CaIpInterface = (*env)->FindClass(env, "org/iotivity/ca/CaIpInterface");
    if (!cls_CaIpInterface)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaIpInterface class");
        return CA_STATUS_FAILED;
    }

    jmethodID mid_CaIpInterface_ctor = (*env)->GetMethodID(env, cls_CaIpInterface, "<init>",
                                                                   "(Landroid/content/Context;)V");
    if (!mid_CaIpInterface_ctor)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaIpInterface constructor method");
        return CA_STATUS_FAILED;
    }

    (*env)->NewObject(env, cls_CaIpInterface, mid_CaIpInterface_ctor, jApplicationContext);
    OIC_LOG(DEBUG, TAG, "Create CaIpInterface instance, success");

    OIC_LOG(DEBUG, TAG, "CAIPJniInit_OUT");
    return CA_STATUS_OK;
}

static CAResult_t CAIPDestroyJniInterface()
{
    OIC_LOG(DEBUG, TAG, "CAIPDestroyJniInterface");

    JavaVM *jvm = CANativeJNIGetJavaVM();
    if (!jvm)
    {
        OIC_LOG(ERROR, TAG, "Could not get JavaVM pointer");
        return CA_STATUS_FAILED;
    }

    bool isAttached = false;
    JNIEnv* env = NULL;
    jint res = (*jvm)->GetEnv(jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*jvm)->AttachCurrentThread(jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    jclass jni_IpInterface = (*env)->FindClass(env, "org/iotivity/ca/CaIpInterface");
    if (!jni_IpInterface)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaIpInterface class");
        goto error_exit;
    }

    jmethodID jni_InterfaceDestroyMethod = (*env)->GetStaticMethodID(env, jni_IpInterface,
                                                                     "destroyIpInterface",
                                                                     "()V");
    if (!jni_InterfaceDestroyMethod)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaIpInterface destroy method");
        goto error_exit;
    }

    (*env)->CallStaticVoidMethod(env, jni_IpInterface, jni_InterfaceDestroyMethod);

    if ((*env)->ExceptionCheck(env))
    {
        OIC_LOG(ERROR, TAG, "destroyIpInterface has failed");
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        goto error_exit;
    }

    OIC_LOG(DEBUG, TAG, "Destroy instance for CaIpInterface");

    if (isAttached)
    {
        (*jvm)->DetachCurrentThread(jvm);
    }

    return CA_STATUS_OK;

error_exit:

    if (isAttached)
    {
        (*jvm)->DetachCurrentThread(jvm);
    }

    return CA_STATUS_FAILED;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaIpInterface_caIpStateEnabled(JNIEnv *env, jclass class)
{
    (void)env;
    (void)class;

    OIC_LOG(DEBUG, TAG, "Wifi is in Activated State");
    CAIPPassNetworkChangesToAdapter(CA_INTERFACE_UP);

    // Apply network interface changes.
    u_arraylist_t *iflist = CAIPGetInterfaceInformation(0);
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
        return;
    }

    size_t listLength = u_arraylist_length(iflist);
    for (size_t i = 0; i < listLength; i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
        if (!ifitem)
        {
            continue;
        }

        CAProcessNewInterface(ifitem); /* GAR: in caipserver.c; adds if to multicast grp */

    }
    u_arraylist_destroy(iflist);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaIpInterface_caIpStateDisabled(JNIEnv *env, jclass class)
{
    (void)env;
    (void)class;

    OIC_LOG(DEBUG, TAG, "Wifi is in Deactivated State");
    CAIPPassNetworkChangesToAdapter(CA_INTERFACE_DOWN);
}

