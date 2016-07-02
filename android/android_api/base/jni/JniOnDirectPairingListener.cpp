/*
* //******************************************************************
* //
* // Copyright 2016 Samsung Electronics All Rights Reserved.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
* //
* // Licensed under the Apache License, Version 2.0 (the "License");
* // you may not use this file except in compliance with the License.
* // You may obtain a copy of the License at
* //
* //      http://www.apache.org/licenses/LICENSE-2.0
* //
* // Unless required by applicable law or agreed to in writing, software
* // distributed under the License is distributed on an "AS IS" BASIS,
* // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* // See the License for the specific language governing permissions and
* // limitations under the License.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
#include "JniOnDirectPairingListener.h"
#include "JniOcDirectPairDevice.h"
#include "OCDirectPairing.h"
#include "OCApi.h"
using namespace OC;

JniOnDirectPairingListener::JniOnDirectPairingListener(JNIEnv *env, jobject jListener,
    RemoveListenerCallback removeListenerCallback)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
    m_removeListenerCallback = removeListenerCallback;
}


JniOnDirectPairingListener::~JniOnDirectPairingListener()
{
    LOGI("~JniOnDirectPairingListener()");
    if (m_jwListener)
    {
        jint ret = JNI_ERR;
        JNIEnv *env = GetJNIEnv(ret);
        if (nullptr == env)
        {
            return;
        }
        env->DeleteWeakGlobalRef(m_jwListener);
        m_jwListener = nullptr;
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
    }
}

void JniOnDirectPairingListener::doDirectPairingCB(std::shared_ptr<OC::OCDirectPairing> dpDev,
                                                                    OCStackResult result)
{
    jint ret = JNI_ERR;
    JNIEnv *env = GetJNIEnv(ret);
    if (nullptr == env)
    {
        return;
    }

    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    jclass clsL = env->GetObjectClass(jListener);
    if (!clsL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    JniOcDirectPairDevice *device = new JniOcDirectPairDevice(dpDev);
    if (!device)
    {
        return;
    }

    jstring jStr = env->NewStringUTF((dpDev->getDeviceID()).c_str());
    if (!jStr)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    jobject jresult = env->NewObject(g_cls_OcDirectPairDevice, g_mid_OcDirectPairDevice_ctor);
    if (!jresult)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    SetHandle<JniOcDirectPairDevice>(env, jresult, device);

    jint jres = static_cast<jint>(result);

    jmethodID midL = env->GetMethodID(clsL, "onDirectPairingListener", "(Ljava/lang/String;I)V");

    if (!midL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    env->CallVoidMethod(jListener, midL, jStr, jres);
    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
    }

    checkExAndRemoveListener(env);

    if (JNI_EDETACHED == ret)
    {
        g_jvm->DetachCurrentThread();
    }
}

void JniOnDirectPairingListener::checkExAndRemoveListener(JNIEnv* env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_removeListenerCallback(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_removeListenerCallback(env, m_jwListener);
    }
}
