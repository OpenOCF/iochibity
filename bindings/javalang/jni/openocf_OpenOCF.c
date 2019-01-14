#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <jni.h>

#ifdef __ANDROID__
#include <android/api-level.h>
#include <android/log.h>
#endif

#ifdef __ANDROID_API__
/* this will be contained on android */
#endif

#ifndef __ANDROID_API__
/* this will NOT be contained for android builds */
#endif


#include "openocf.h"

//#include "openocf_OpenOCF.h"

/* #include "openocf_android_OCFServices.h" */

#include "org_iochibity_Exceptions.h"
#include "_threads.h"

#include "jni_init.h"
#include "jni_utils.h"

/* PRIVATE */
#define TAG  "openocf_OpenOCF"

bool g_quit_flag = false;

FILE                   *logfd;

const char *SVRS_CONFIG_FNAME = "svrs_config.cbor";
const char* g_config_fname;

/* THREAD_T tid_work; */
THREAD_T tid_work;

/* thread routine - service client requests */
THREAD_EXIT_T troutine_work(void *arg)
{
    OC_UNUSED(arg);
    printf("Entering server work thread...\n");

    while (!g_quit_flag) {
	if (OCProcess() != OC_STACK_OK) {
	    printf("OCStack process error\n");
	}
	sleep(1);
    }
    printf("Exiting server work thread...\n");
    OCStop();
    /* we're the only thread left, pthread_exit(NULL) would kill us,
       but not the process. */
    /* exit(0); */

    /* FIXME: pthreads return void*, c11 threads return int */
    return THREAD_EXIT_OK;
}

FILE* server_fopen(const char *path, const char *mode)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, path: %s", __func__, path);
    (void)path;
    /* printf("%s path: %s\n", __func__, path); */
    if (0 == strcmp(path, SVR_DB_DAT_FILE_NAME)) /* "oic_svr_db.dat" */
    {
    	/* override default file */
	OIC_LOG_V(DEBUG, TAG, "Overriding path with: %s", g_config_fname);
        return fopen(g_config_fname, mode);
    }
    else
    {
        return fopen(path, mode);
    }
    return NULL;
}

static OCPersistentStorage ps = {server_fopen, fread, fwrite, fclose, unlink};

JNIEXPORT void JNICALL
Java_openocf_OpenOCF_config_1svrs(JNIEnv *env, jclass klass, jstring j_svrs_config_fname)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    if (j_svrs_config_fname == NULL) {
    	OIC_LOG_V(DEBUG, TAG, "svrs config fname is null; defaulting to %s", SVRS_CONFIG_FNAME);
	g_config_fname = SVRS_CONFIG_FNAME;
    	/* j_svrs_config_fname = (*env)->NewStringUTF(env, SVRS_CONFIG_FNAME); */
    } else {
	g_config_fname = (*env)->GetStringUTFChars(env, j_svrs_config_fname, NULL);
    	OIC_LOG_V(DEBUG, TAG, "svrs config fname: %s", g_config_fname);
    }
    if (g_config_fname == NULL) {
    	THROW_JNI_EXCEPTION("GetStringUTFChars");
    }
    OIC_LOG_V(DEBUG, TAG, "calling OCRegisterPersistentStorageHandler: %s", g_config_fname);
    OCRegisterPersistentStorageHandler(&ps);
    printf("called OCRegisterPersistentStorageHandler: %s\n", g_config_fname);
}

JNIEXPORT void JNICALL
Java_openocf_OpenOCF_config_1logging(JNIEnv *env, jclass klass, jstring j_logfname)
{
    /* __android_log_print(ANDROID_LOG_INFO, TAG, "config_logging ENTRY"); */
	/* : %s\n", */
	/*    (j_logfname == NULL)? "NULL" */
	/*    : (char*) (*env)->GetStringUTFChars(env, j_logfname, NULL)); */
    OCLogInit();
    return;

    if (j_logfname) {
        char *logfname = (char*) (*env)->GetStringUTFChars(env, j_logfname, NULL);
	logfd = fopen(logfname, "w");
	if (logfd != NULL) {
	    OCLogInit(logfd);
	} else {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                fprintf(stdout, "Current working dir: %s\n", cwd);
            /* __android_log_printf(ANDROID_LOG_INFO, TAG,  "Current working dir: %s\n", cwd); */
            else
                perror("getcwd() error");
	    printf("%s ERROR Logfile fopen %s failed with errno: %s\n",
                   __func__, logfname, strerror(errno));
            /* __android_log_printf(ANDROID_LOG_ERROR, TAG, "%s ERROR Logfile fopen %s failed with errno: %s\n",
               __func__, logfname, strerror(errno)); */
	    exit(EXIT_FAILURE);
	}
    } else {
	OCLogInit(NULL);
#ifdef TB_LOG
	OCSetLogLevel(DEBUG, false);
#endif
	/* OIC_LOG_V(DEBUG, TAG, "%s OCLogInit done", __func__); */
    /* LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG */
    /* coap_set_log_level(LOG_WARNING); */

    /* dtls_set_log_level(DTLS_LOG_WARN); /\* DEBUG, INFO,  NOTICE, WARN, CRIT ... *\/ */


    }
}

/* /\* */
/*  * Class:     openocf_OpenOCF */
/*  * Method:    Init */
/*  * Signature: (ILjava/lang/String;)V */
/*  *\/ */
/* JNIEXPORT void JNICALL */
/* Java_openocf_OpenOCF_Init__I(JNIEnv * env, jclass klass, jint mode) */

/*
 * Class:     openocf_OpenOCF
 * Method:    Init
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_openocf_OpenOCF_Init__I (JNIEnv *env, jclass klass, jint mode)
{
    OC_UNUSED(klass);
    printf("%s ENTRY, mode: %d\n", __func__, mode);

    /* store Handler for UI updates */
    /* if (FID_OPENOCF_UI_HANDLER == NULL) { */
    /* 	FID_OPENOCF_UI_HANDLER = (*env)->GetStaticFieldID(env, klass, */
    /* 							  "uiHandler", */
    /* 							  "Landroid/os/Handler;"); */
    /* 	if (FID_OPENOCF_UI_HANDLER == NULL) { */
    /* 	    /\* printf("ERROR: GetStaticFieldID failed for ClientConfig.uiHandler"); *\/ */
    /* 	    THROW_JNI_EXCEPTION("ERROR: GetStaticFieldID failed for ClientConfig.uiHandler"); */
    /* 	} */
    /* } */

    /* jobject j_Handler = NULL; */

    /* j_Handler = (*env)->GetStaticObjectField(env, klass, FID_OPENOCF_UI_HANDLER); */

    /* moved to openocf.Service::config */
    /* /\* First configure security *\/ */
    /* if (j_config_fname == NULL) { */
    /* 	printf("GetStconfig fname is null\n"); */
    /* 	j_config_fname = (*env)->NewStringUTF(env, g_config_fname); */
    /* } */
    /* g_config_fname = (*env)->GetStringUTFChars(env, j_config_fname, NULL); */
    /* if (g_config_fname == NULL) { */
    /* 	THROW_JNI_EXCEPTION("GetStringUTFChars"); */
    /* } */
    /* OCPersistentStorage ps = {server_fopen, fread, fwrite, fclose, unlink}; */
    /* printf("calling OCRegisterPersistentStorageHandler: %s\n", g_config_fname); */
    /* OCRegisterPersistentStorageHandler(&ps); */

    /* (*env)->ReleaseStringUTFChars(env, j_config_fname, g_config_fname); */

    /* const char *cip_addr = ""; */
    /* if (j_ip_addr == NULL) { */
    /* 	printf("ip addr null\n"); */
    /* 	/\* j_ip_addr = (*env)->NewStringUTF(env, cip_addr); *\/ */
    /* 	cip_addr = NULL;  /\* (*env)->GetStringUTFChars(env, j_ip_addr, NULL); *\/ */
    /* } else { */
    /* 	cip_addr = (*env)->GetStringUTFChars(env, j_ip_addr, NULL); */
    /* 	if (cip_addr == NULL) { */
    /* 	    THROW_JNI_EXCEPTION("GetStringUTFChars"); */
    /* 	} */
    /* } */
    /* printf("ip addr: [%s]\n", cip_addr); */


    /* Then initialize supervisor */
    OCStackResult op_result;
    /* op_result = OCInit(cip_addr, (uint16_t)port, mode); */
    op_result = OCInit1(mode, OC_FLAG_SECURE, OC_FLAG_SECURE);
    if (op_result != OC_STACK_OK) {
	printf("OCStack init error\n");
	THROW_STACK_EXCEPTION(op_result, "Initialization failure");
    }
    /* (*env)->ReleaseStringUTFChars(env, j_ip_addr, cip_addr); */

    /* return op_result; */
}

/*
 * Class:     openocf_OpenOCF
 * Method:    OCInit
 * Signature: (III)I
 */
JNIEXPORT void JNICALL Java_openocf_OpenOCF_Init__III
  (JNIEnv * env, jclass clazz, jint mode, jint server_flags, jint client_flags)
{
    OC_UNUSED(env);
    OC_UNUSED(clazz);
    OC_UNUSED(mode);
    OC_UNUSED(server_flags);
    OC_UNUSED(client_flags);
}

/*
 * Class:     openocf_engine_OCFClientSP
 * Method:    configurePlatformSP
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
/*
 * Class:     openocf_OpenOCF
 * Method:    configurePlatformSP
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
/* JNIEXPORT void JNICALL Java_openocf_engine_OCFClientSP_configurePlatformSP */
JNIEXPORT void JNICALL Java_openocf_OpenOCF_configurePlatformSP
  (JNIEnv * env, jclass klass,
   jstring j_platform_id,
   jstring j_mfg_name,
   jstring j_mfg_url,
   jstring j_model_number,
   jstring j_mfg_date,
   jstring j_platform_version,
   jstring j_os_version,
   jstring j_hw_version,
   jstring j_fw_version,
   jstring j_support_url,
   jstring j_sys_time)
{
    OC_UNUSED(klass);
    static OCPlatformInfo platform_info =
	{
	    .platformID			= "",
	    .manufacturerName		= "",
	    .manufacturerUrl		= "",
	    .modelNumber		= "",
	    .dateOfManufacture		= "",
	    .platformVersion		= "",
	    .operatingSystemVersion	= "",
	    .hardwareVersion		= "",
	    .firmwareVersion		= "",
	    .supportUrl			= "",
	    .systemTime			= ""   /* "2015-05-15T11.04" */
	};

    if (j_platform_id == NULL) {
	printf("platform id is null\n");
	j_platform_id = (*env)->NewStringUTF(env, platform_info.platformID);
	platform_info.platformID = (char*) (*env)->GetStringUTFChars(env, j_platform_id, NULL);
    } else {
	platform_info.platformID = (char*) (*env)->GetStringUTFChars(env, j_platform_id, NULL);
	if (platform_info.platformID == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c platform id: [%s]\n", platform_info.platformID);

    if (j_mfg_name == NULL) {
	printf("platform id is null\n");
	j_mfg_name = (*env)->NewStringUTF(env, platform_info.manufacturerName);
	platform_info.manufacturerName = (char*) (*env)->GetStringUTFChars(env, j_mfg_name, NULL);
    } else {
	platform_info.manufacturerName = (char*) (*env)->GetStringUTFChars(env, j_mfg_name, NULL);
	if (platform_info.manufacturerName == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c mfg name: [%s]\n", platform_info.manufacturerName);

    if (j_mfg_url == NULL) {
	printf("platform id is null\n");
	j_mfg_url = (*env)->NewStringUTF(env, platform_info.manufacturerUrl);
	platform_info.manufacturerUrl = (char*) (*env)->GetStringUTFChars(env, j_mfg_url, NULL);
    } else {
	platform_info.manufacturerUrl = (char*) (*env)->GetStringUTFChars(env, j_mfg_url, NULL);
	if (platform_info.manufacturerUrl == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c mfg url: [%s]\n", platform_info.manufacturerUrl);

    if (j_model_number == NULL) {
	printf("platform id is null\n");
	j_model_number = (*env)->NewStringUTF(env, platform_info.modelNumber);
	platform_info.modelNumber = (char*) (*env)->GetStringUTFChars(env, j_model_number, NULL);
    } else {
	platform_info.modelNumber = (char*) (*env)->GetStringUTFChars(env, j_model_number, NULL);
	if (platform_info.modelNumber == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c model nbr: [%s]\n", platform_info.modelNumber);

    if (j_mfg_date == NULL) {
	printf("platform id is null\n");
	j_mfg_date = (*env)->NewStringUTF(env, platform_info.dateOfManufacture);
	platform_info.dateOfManufacture = (char*) (*env)->GetStringUTFChars(env, j_mfg_date, NULL);
    } else {
	platform_info.dateOfManufacture = (char*) (*env)->GetStringUTFChars(env, j_mfg_date, NULL);
	if (platform_info.dateOfManufacture == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c mfg date: [%s]\n", platform_info.dateOfManufacture);

    if (j_platform_version == NULL) {
	printf("platform id is null\n");
	j_platform_version = (*env)->NewStringUTF(env, platform_info.platformVersion);
	platform_info.platformVersion = (char*) (*env)->GetStringUTFChars(env, j_platform_version, NULL);
    } else {
	platform_info.platformVersion = (char*) (*env)->GetStringUTFChars(env, j_platform_version, NULL);
	if (platform_info.platformVersion == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c platform version: [%s]\n", platform_info.platformVersion);

    if (j_os_version == NULL) {
	printf("platform id is null\n");
	j_os_version = (*env)->NewStringUTF(env, platform_info.operatingSystemVersion);
	platform_info.operatingSystemVersion = (char*) (*env)->GetStringUTFChars(env, j_os_version, NULL);
    } else {
	platform_info.operatingSystemVersion = (char*) (*env)->GetStringUTFChars(env, j_os_version, NULL);
	if (platform_info.operatingSystemVersion == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c os version: [%s]\n", platform_info.operatingSystemVersion);

    if (j_hw_version == NULL) {
	printf("platform id is null\n");
	j_hw_version = (*env)->NewStringUTF(env, platform_info.hardwareVersion);
	platform_info.hardwareVersion = (char*) (*env)->GetStringUTFChars(env, j_hw_version, NULL);
    } else {
	platform_info.hardwareVersion = (char*) (*env)->GetStringUTFChars(env, j_hw_version, NULL);
	if (platform_info.hardwareVersion == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c hw version: [%s]\n", platform_info.hardwareVersion);

    if (j_fw_version == NULL) {
	printf("platform id is null\n");
	j_fw_version = (*env)->NewStringUTF(env, platform_info.firmwareVersion);
	platform_info.firmwareVersion = (char*) (*env)->GetStringUTFChars(env, j_fw_version, NULL);
    } else {
	platform_info.firmwareVersion = (char*) (*env)->GetStringUTFChars(env, j_fw_version, NULL);
	if (platform_info.firmwareVersion == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c firmware version: [%s]\n", platform_info.firmwareVersion);

    if (j_support_url == NULL) {
	printf("platform id is null\n");
	j_support_url = (*env)->NewStringUTF(env, platform_info.supportUrl);
	platform_info.supportUrl = (char*) (*env)->GetStringUTFChars(env, j_support_url, NULL);
    } else {
	platform_info.supportUrl = (char*) (*env)->GetStringUTFChars(env, j_support_url, NULL);
	if (platform_info.supportUrl == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c support url: [%s]\n", platform_info.supportUrl);

    if (j_sys_time == NULL) {
	printf("platform id is null\n");
	j_sys_time = (*env)->NewStringUTF(env, platform_info.systemTime);
	platform_info.systemTime = (char*) (*env)->GetStringUTFChars(env, j_sys_time, NULL);
    } else {
	platform_info.systemTime = (char*) (*env)->GetStringUTFChars(env, j_sys_time, NULL);
	if (platform_info.systemTime == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c system time: [%s]\n", platform_info.systemTime);

    printf("Setting platform info...\n");
    OCStackResult op_result;
    op_result = OCSetPlatformInfo(platform_info);
    if (op_result != OC_STACK_OK) {
	// FIXME do sth!
    }

    (*env)->ReleaseStringUTFChars(env, j_platform_id, platform_info.platformID);
    (*env)->ReleaseStringUTFChars(env, j_mfg_name, platform_info.manufacturerName);
    (*env)->ReleaseStringUTFChars(env, j_mfg_url, platform_info.manufacturerUrl);
    (*env)->ReleaseStringUTFChars(env, j_model_number, platform_info.modelNumber);
    (*env)->ReleaseStringUTFChars(env, j_mfg_date, platform_info.dateOfManufacture);
    (*env)->ReleaseStringUTFChars(env, j_platform_version, platform_info.platformVersion);
    (*env)->ReleaseStringUTFChars(env, j_os_version, platform_info.operatingSystemVersion);
    (*env)->ReleaseStringUTFChars(env, j_hw_version, platform_info.hardwareVersion);
    (*env)->ReleaseStringUTFChars(env, j_fw_version, platform_info.firmwareVersion);
    (*env)->ReleaseStringUTFChars(env, j_support_url, platform_info.supportUrl);
    (*env)->ReleaseStringUTFChars(env, j_sys_time, platform_info.systemTime);
}

/*
 * Class:     openocf_OpenOCF
 * Method:    configureDeviceSP
 * Signature: (Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_openocf_OpenOCF_configureDeviceSP
  (JNIEnv * env, jclass klass,
   jstring j_device_name,
   jobjectArray j_types,
   jstring j_spec_version,
   jobjectArray j_data_model_versions)
{
    OC_UNUSED(env);
    OC_UNUSED(klass);
    OC_UNUSED(j_types);
    OC_UNUSED(j_data_model_versions);
    static OCDeviceInfo device_info =
	{
	    .deviceName = "", /* Default Device Name", */
	    /* OCStringLL *types; */
	    .types = NULL,
	    .specVersion = "0.0.0", /* device specification version */
	    // .dataModelVersions = "minDeviceModelVersion"
	    .dataModelVersions = NULL
	};

    if (j_device_name == NULL) {
	printf("device name is null, using default\n");
	j_device_name = (*env)->NewStringUTF(env, device_info.deviceName);
	device_info.deviceName = (char*) (*env)->GetStringUTFChars(env, j_device_name, NULL);
    } else {
	device_info.deviceName = (char*) (*env)->GetStringUTFChars(env, j_device_name, NULL);
	if (device_info.deviceName == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c device name: [%s]\n", device_info.deviceName);

    if (j_spec_version == NULL) {
	printf("spec version is null, using default\n");
	j_spec_version = (*env)->NewStringUTF(env, device_info.specVersion);
	device_info.specVersion = (char*) (*env)->GetStringUTFChars(env, j_spec_version, NULL);
    } else {
	device_info.specVersion = (char*) (*env)->GetStringUTFChars(env, j_spec_version, NULL);
	if (device_info.specVersion == NULL) {
	    THROW_JNI_EXCEPTION("GetStringUTFChars");
	}
    }
    printf("c platform id: [%s]\n", device_info.specVersion);



    printf("Setting device info...\n");
    OCStackResult op_result;
    op_result = OCSetDeviceInfo(device_info);
    switch (op_result) {
    case OC_STACK_OK:
	break;
    case OC_STACK_INVALID_PARAM:
	THROW_STACK_EXCEPTION(op_result, "Java_org_iochibity_OCF_setDeviceInfo");
	/* throw_invalid_param(env, "Java_org_iochibity_OCF_setDeviceInfo"); */
	break;
    default:
        printf("Device Registration failed with result %d!\n", op_result);
	THROW_STACK_EXCEPTION(op_result, "UNKNOWN");
    }

    (*env)->ReleaseStringUTFChars(env, j_device_name, device_info.deviceName);
    (*env)->ReleaseStringUTFChars(env, j_spec_version, device_info.specVersion);
}

/*
 * Class:     openocf_OpenOCF
 * Method:    run
 * Signature: ()I
 */
/*
 * Class:     openocf_OpenOCF
 * Method:    run
 * Signature: ()V
 */

 /* FIXME: call this "observe"? */
/* JNIEXPORT void JNICALL Java_openocf_OpenOCF_run (JNIEnv * env, jclass clazz) */
JNIEXPORT void JNICALL Java_openocf_OpenOCF_run (JNIEnv *env, jclass clazz)
{
    OC_UNUSED(env);
    OC_UNUSED(clazz);
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    printf("%s ENTRY\n", __func__);
    g_quit_flag = false;

    /* FIXME: broken! OCProcess is not thread safe */
    /* printf("Launching worker thread...\n"); */
    /* THREAD_CREATE_DEFAULT((THREAD_T*)&tid_work, */
    /* 			  (THREAD_START_T)troutine_work, */
    /* 			  (void *)NULL); */
    while (!g_quit_flag) {
	/* OIC_LOG_V(DEBUG, TAG, "%s LOOP", __func__); */
	if (OCProcess() != OC_STACK_OK) {
	    printf("OCStack process error\n");
	}
	sleep(0.3);
    }
    OCStop();
    /* main thread has nothing to do. by calling pthread_exit it exits
       but the process continues, so any spawned threads do too. */
    /* THREAD_EXIT(THREAD_EXIT_OK); */
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    printf("%s EXIT\n", __func__);
}

/*
 * Class:     openocf_OpenOCF
 * Method:    stop
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_openocf_OpenOCF_stop(JNIEnv * env, jclass clazz)
{
    OC_UNUSED(env);
    OC_UNUSED(clazz);
    g_quit_flag = true;
}

/*
 * Class:     openocf_OpenOCF
 * Method:    OCCancel
 * Signature: (Ljava/lang/Object;ILjava/lang/Object;B)I
 */
/* JNIEXPORT jint JNICALL Java_openocf_OpenOCF_OCCancel */
/*   (JNIEnv *, jobject, jobject, jint, jobject, jbyte); */

/*
 * Class:     openocf_OpenOCF
 * Method:    configuration
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_openocf_OpenOCF_configuration(JNIEnv *env, jclass klass)
{
    /* OIC_LOG_V(DEBUG, TAG, "%s ENTRY %s", __func__); */
    const char * str = configuration();
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

/*
 * Class:     openocf_OpenOCF
 * Method:    getVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_openocf_OpenOCF_getVersion(JNIEnv *env, jclass klass)
{
    /* OIC_LOG_V(DEBUG, TAG, "%s ENTRY %s", __func__); */
    const char * str = oocf_get_version();
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
