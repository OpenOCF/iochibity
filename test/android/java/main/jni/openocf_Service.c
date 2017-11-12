#include "openocf_Service.h"

#include <jni.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"

JNIEXPORT jstring JNICALL
Java_openocf_Service_configuration (JNIEnv * env, jclass klass)
{
    char * str = configuration();
    int len = strlen(str);
    jchar *str1;
    str1 = (jchar *)(malloc(len * sizeof(jchar)));

    for (int i = 0; i < len; i++) {
	str1[i] = (unsigned char)str[i];
    }
    jstring result = (*env)->NewString(env, str1, len);
    free(str1);
    return result;
}
