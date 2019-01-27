#include "jni_init.h"

#include "org_iochibity_Exceptions.h"

#include "openocf_message_CoAPOption.ids.h"

#define FQCN_COAP_OPTION "openocf/message/CoAPOption"

jclass    K_COAP_OPTION                 = NULL;
jmethodID MID_COAP_OPTION_CTOR          = NULL;

int init_CoAPOption(JNIEnv* env)
{
    jclass klass;
    klass = (*env)->FindClass(env, FQCN_COAP_OPTION);
    JNI_ASSERT_NULL(klass, ERR_MSG(ERR_CLASS, FQCN_COAP_OPTION), -1);
    K_COAP_OPTION = (jclass)(*env)->NewGlobalRef(env, klass);
    (*env)->DeleteLocalRef(env, klass);

    if (MID_COAP_OPTION_CTOR == NULL) {
	MID_COAP_OPTION_CTOR = (*env)->GetMethodID(env, K_COAP_OPTION, "<init>", "()V");
	JNI_ASSERT_NULL(MID_COAP_OPTION_CTOR,
			ERR_MSG(ERR_METHOD, "CoAPOption() (ctor)"), -1);
    }
    return 0;
}
