/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gtest/gtest.h"
#include "ocstack.h"
#include "utlist.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocprovisioningmanager.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "securevirtualresourcetypes.h"
#include "provisioningdatabasemanager.h"
#include "srmutility.h"
#include "doxmresource.h"
#include "pmtypes.h"
#include "pmutility.h"

using namespace std;

TEST(JustWorksOxMTest, NullParam)
{
    OTMContext_t* otmCtx = NULL;
    OCStackResult res = OC_STACK_ERROR;
    uint8_t *payloadRes = NULL;
    size_t size = 0;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    res = LoadSecretJustWorksCallback(otmCtx);
    EXPECT_TRUE(OC_STACK_OK == res);

    res = CreateSecureSessionJustWorksCallback(otmCtx);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateJustWorksSelectOxmPayload(otmCtx, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateJustWorksOwnerTransferPayload(otmCtx, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    OTMContext_t otmCtx2;
    otmCtx2.selectedDeviceInfo = NULL;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    res = LoadSecretJustWorksCallback(&otmCtx2);
    EXPECT_TRUE(OC_STACK_OK == res);

    res = CreateSecureSessionJustWorksCallback(&otmCtx2);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateJustWorksSelectOxmPayload(&otmCtx2, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateJustWorksOwnerTransferPayload(&otmCtx2, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);
}

TEST(RandomPinOxMTest, NullParam)
{
    OTMContext_t* otmCtx = NULL;
    OCStackResult res = OC_STACK_ERROR;
    uint8_t *payloadRes = NULL;
    size_t size = 0;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    res = InputPinCodeCallback(otmCtx);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateSecureSessionRandomPinCallback(otmCtx);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreatePinBasedSelectOxmPayload(otmCtx, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreatePinBasedOwnerTransferPayload(otmCtx, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    OTMContext_t otmCtx2;
    otmCtx2.selectedDeviceInfo = NULL;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    res = InputPinCodeCallback(&otmCtx2);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateSecureSessionRandomPinCallback(&otmCtx2);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreatePinBasedSelectOxmPayload(&otmCtx2, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreatePinBasedOwnerTransferPayload(&otmCtx2, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);
}


/****************************************
 * Test the OTM modules with sample server
 ****************************************/
#define TAG "JUSTWORKS_UNITTEST"
#define OTM_TIMEOUT 5
#define DISCOVERY_TIMEOUT 3

#define SVR_DB_PATH "oic_svr_db_client.dat"
#define UT_PATH "resource/csdk/security/provisioning/unittest"
char pdb_path[1024];
char svr_path[1024];

static uint8_t DEFAULT_SVR_DB[] = {
    0xBF, 0x63, 0x61, 0x63, 0x6C, 0x59, 0x02, 0x57, 0xA2, 0x66, 0x61, 0x63, 0x6C, 0x69, 0x73, 0x74,
    0xA1, 0x64, 0x61, 0x63, 0x65, 0x73, 0x83, 0xA3, 0x6B, 0x73, 0x75, 0x62, 0x6A, 0x65, 0x63, 0x74,
    0x75, 0x75, 0x69, 0x64, 0x61, 0x2A, 0x69, 0x72, 0x65, 0x73, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x73,
    0x83, 0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x68, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x72, 0x65, 0x73,
    0x62, 0x72, 0x74, 0x81, 0x6A, 0x6F, 0x69, 0x63, 0x2E, 0x77, 0x6B, 0x2E, 0x72, 0x65, 0x73, 0x62,
    0x69, 0x66, 0x81, 0x69, 0x6F, 0x69, 0x63, 0x2E, 0x69, 0x66, 0x2E, 0x6C, 0x6C, 0x63, 0x72, 0x65,
    0x6C, 0x60, 0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x66, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x64, 0x62,
    0x72, 0x74, 0x81, 0x68, 0x6F, 0x69, 0x63, 0x2E, 0x77, 0x6B, 0x2E, 0x64, 0x62, 0x69, 0x66, 0x82,
    0x6F, 0x6F, 0x69, 0x63, 0x2E, 0x69, 0x66, 0x2E, 0x62, 0x61, 0x73, 0x65, 0x6C, 0x69, 0x6E, 0x65,
    0x68, 0x6F, 0x69, 0x63, 0x2E, 0x69, 0x66, 0x2E, 0x72, 0x63, 0x72, 0x65, 0x6C, 0x60, 0xA4, 0x64,
    0x68, 0x72, 0x65, 0x66, 0x66, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x70, 0x62, 0x72, 0x74, 0x81, 0x68,
    0x6F, 0x69, 0x63, 0x2E, 0x77, 0x6B, 0x2E, 0x70, 0x62, 0x69, 0x66, 0x82, 0x6F, 0x6F, 0x69, 0x63,
    0x2E, 0x69, 0x66, 0x2E, 0x62, 0x61, 0x73, 0x65, 0x6C, 0x69, 0x6E, 0x65, 0x68, 0x6F, 0x69, 0x63,
    0x2E, 0x69, 0x66, 0x2E, 0x72, 0x63, 0x72, 0x65, 0x6C, 0x60, 0x6A, 0x70, 0x65, 0x72, 0x6D, 0x69,
    0x73, 0x73, 0x69, 0x6F, 0x6E, 0x02, 0xA3, 0x6B, 0x73, 0x75, 0x62, 0x6A, 0x65, 0x63, 0x74, 0x75,
    0x75, 0x69, 0x64, 0x61, 0x2A, 0x69, 0x72, 0x65, 0x73, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x73, 0x82,
    0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x6D, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x73, 0x65, 0x63, 0x2F,
    0x64, 0x6F, 0x78, 0x6D, 0x62, 0x72, 0x74, 0x81, 0x6A, 0x6F, 0x69, 0x63, 0x2E, 0x72, 0x2E, 0x64,
    0x6F, 0x78, 0x6D, 0x62, 0x69, 0x66, 0x81, 0x6F, 0x6F, 0x69, 0x63, 0x2E, 0x69, 0x66, 0x2E, 0x62,
    0x61, 0x73, 0x65, 0x6C, 0x69, 0x6E, 0x65, 0x63, 0x72, 0x65, 0x6C, 0x60, 0xA4, 0x64, 0x68, 0x72,
    0x65, 0x66, 0x6E, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x73, 0x65, 0x63, 0x2F, 0x70, 0x73, 0x74, 0x61,
    0x74, 0x62, 0x72, 0x74, 0x81, 0x6B, 0x6F, 0x69, 0x63, 0x2E, 0x72, 0x2E, 0x70, 0x73, 0x74, 0x61,
    0x74, 0x62, 0x69, 0x66, 0x81, 0x6F, 0x6F, 0x69, 0x63, 0x2E, 0x69, 0x66, 0x2E, 0x62, 0x61, 0x73,
    0x65, 0x6C, 0x69, 0x6E, 0x65, 0x63, 0x72, 0x65, 0x6C, 0x60, 0x6A, 0x70, 0x65, 0x72, 0x6D, 0x69,
    0x73, 0x73, 0x69, 0x6F, 0x6E, 0x02, 0xA3, 0x6B, 0x73, 0x75, 0x62, 0x6A, 0x65, 0x63, 0x74, 0x75,
    0x75, 0x69, 0x64, 0x61, 0x2A, 0x69, 0x72, 0x65, 0x73, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x73, 0x82,
    0xA4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x6E, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x73, 0x65, 0x63, 0x2F,
    0x70, 0x63, 0x6F, 0x6E, 0x66, 0x62, 0x72, 0x74, 0x81, 0x6B, 0x6F, 0x69, 0x63, 0x2E, 0x72, 0x2E,
    0x70, 0x63, 0x6F, 0x6E, 0x66, 0x62, 0x69, 0x66, 0x81, 0x6F, 0x6F, 0x69, 0x63, 0x2E, 0x69, 0x66,
    0x2E, 0x62, 0x61, 0x73, 0x65, 0x6C, 0x69, 0x6E, 0x65, 0x63, 0x72, 0x65, 0x6C, 0x60, 0xA4, 0x64,
    0x68, 0x72, 0x65, 0x66, 0x71, 0x2F, 0x6F, 0x69, 0x63, 0x2F, 0x73, 0x65, 0x63, 0x2F, 0x64, 0x70,
    0x61, 0x69, 0x72, 0x69, 0x6E, 0x67, 0x62, 0x72, 0x74, 0x81, 0x6E, 0x6F, 0x69, 0x63, 0x2E, 0x72,
    0x2E, 0x64, 0x70, 0x61, 0x69, 0x72, 0x69, 0x6E, 0x67, 0x62, 0x69, 0x66, 0x81, 0x6F, 0x6F, 0x69,
    0x63, 0x2E, 0x69, 0x66, 0x2E, 0x62, 0x61, 0x73, 0x65, 0x6C, 0x69, 0x6E, 0x65, 0x63, 0x72, 0x65,
    0x6C, 0x60, 0x6A, 0x70, 0x65, 0x72, 0x6D, 0x69, 0x73, 0x73, 0x69, 0x6F, 0x6E, 0x02, 0x6A, 0x72,
    0x6F, 0x77, 0x6E, 0x65, 0x72, 0x75, 0x75, 0x69, 0x64, 0x78, 0x24, 0x36, 0x31, 0x36, 0x34, 0x36,
    0x64, 0x36, 0x39, 0x2D, 0x36, 0x65, 0x34, 0x34, 0x2D, 0x36, 0x35, 0x37, 0x36, 0x2D, 0x36, 0x39,
    0x36, 0x33, 0x2D, 0x36, 0x35, 0x35, 0x35, 0x37, 0x35, 0x36, 0x39, 0x36, 0x34, 0x33, 0x30, 0x65,
    0x70, 0x73, 0x74, 0x61, 0x74, 0x58, 0x79, 0xA7, 0x64, 0x69, 0x73, 0x6F, 0x70, 0xF5, 0x6A, 0x64,
    0x65, 0x76, 0x69, 0x63, 0x65, 0x75, 0x75, 0x69, 0x64, 0x78, 0x24, 0x36, 0x31, 0x36, 0x34, 0x36,
    0x64, 0x36, 0x39, 0x2D, 0x36, 0x65, 0x34, 0x34, 0x2D, 0x36, 0x35, 0x37, 0x36, 0x2D, 0x36, 0x39,
    0x36, 0x33, 0x2D, 0x36, 0x35, 0x35, 0x35, 0x37, 0x35, 0x36, 0x39, 0x36, 0x34, 0x33, 0x30, 0x62,
    0x63, 0x6D, 0x00, 0x62, 0x74, 0x6D, 0x00, 0x62, 0x6F, 0x6D, 0x03, 0x62, 0x73, 0x6D, 0x03, 0x6A,
    0x72, 0x6F, 0x77, 0x6E, 0x65, 0x72, 0x75, 0x75, 0x69, 0x64, 0x78, 0x24, 0x36, 0x31, 0x36, 0x34,
    0x36, 0x64, 0x36, 0x39, 0x2D, 0x36, 0x65, 0x34, 0x34, 0x2D, 0x36, 0x35, 0x37, 0x36, 0x2D, 0x36,
    0x39, 0x36, 0x33, 0x2D, 0x36, 0x35, 0x35, 0x35, 0x37, 0x35, 0x36, 0x39, 0x36, 0x34, 0x33, 0x30,
    0x64, 0x64, 0x6F, 0x78, 0x6D, 0x58, 0xC4, 0xA8, 0x64, 0x6F, 0x78, 0x6D, 0x73, 0x81, 0x00, 0x66,
    0x6F, 0x78, 0x6D, 0x73, 0x65, 0x6C, 0x00, 0x63, 0x73, 0x63, 0x74, 0x01, 0x65, 0x6F, 0x77, 0x6E,
    0x65, 0x64, 0xF5, 0x6A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x75, 0x75, 0x69, 0x64, 0x78, 0x24,
    0x36, 0x31, 0x36, 0x34, 0x36, 0x64, 0x36, 0x39, 0x2D, 0x36, 0x65, 0x34, 0x34, 0x2D, 0x36, 0x35,
    0x37, 0x36, 0x2D, 0x36, 0x39, 0x36, 0x33, 0x2D, 0x36, 0x35, 0x35, 0x35, 0x37, 0x35, 0x36, 0x39,
    0x36, 0x34, 0x33, 0x30, 0x6C, 0x64, 0x65, 0x76, 0x6F, 0x77, 0x6E, 0x65, 0x72, 0x75, 0x75, 0x69,
    0x64, 0x78, 0x24, 0x36, 0x31, 0x36, 0x34, 0x36, 0x64, 0x36, 0x39, 0x2D, 0x36, 0x65, 0x34, 0x34,
    0x2D, 0x36, 0x35, 0x37, 0x36, 0x2D, 0x36, 0x39, 0x36, 0x33, 0x2D, 0x36, 0x35, 0x35, 0x35, 0x37,
    0x35, 0x36, 0x39, 0x36, 0x34, 0x33, 0x30, 0x6A, 0x72, 0x6F, 0x77, 0x6E, 0x65, 0x72, 0x75, 0x75,
    0x69, 0x64, 0x78, 0x24, 0x36, 0x31, 0x36, 0x34, 0x36, 0x64, 0x36, 0x39, 0x2D, 0x36, 0x65, 0x34,
    0x34, 0x2D, 0x36, 0x35, 0x37, 0x36, 0x2D, 0x36, 0x39, 0x36, 0x33, 0x2D, 0x36, 0x35, 0x35, 0x35,
    0x37, 0x35, 0x36, 0x39, 0x36, 0x34, 0x33, 0x30, 0x71, 0x78, 0x2E, 0x63, 0x6F, 0x6D, 0x2E, 0x73,
    0x61, 0x6D, 0x73, 0x75, 0x6E, 0x67, 0x2E, 0x64, 0x70, 0x63, 0xF4, 0xFF
};

static bool g_doneCB;
static bool g_callbackResult;
static pid_t g_myPID1;
static pid_t g_myPID2;

static const char* g_otmCtx = "Test User Context";
static OCProvisionDev_t* g_unownedDevices = NULL;
static OCProvisionDev_t* g_ownedDevices = NULL;

static void GetCurrentWorkingDirectory(char* buf, size_t bufsize)
{
    char cwd[1024] = {0};
    const char* unittest_path = "resource/csdk/security/provisioning/unittest";
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        if(strstr(cwd, unittest_path) == NULL)
        {
#if defined __linux__
#if __x86_64__
        snprintf(buf, bufsize, "%s/out/linux/x86_64/release/%s/", cwd, unittest_path);
        snprintf(buf, bufsize, "%s/out/linux/x86_64/release/%s/", cwd, unittest_path);
#else
        snprintf(buf, bufsize, "%s/out/linux/x86/release/%s/", cwd, unittest_path);
        snprintf(buf, bufsize, "%s/out/linux/x86/release/%s/", cwd, unittest_path);
#endif //__x86_64__
#endif //defined __linux__
        }
        else
        {
            snprintf(buf, bufsize, "%s/", cwd);
        }
    }
}

static FILE* fopen_prvnMng(const char* path, const char* mode)
{
    (void)path;  // unused |path| parameter

    // input |g_svr_db_fname| internally by force, not using |path| parameter
    // because |OCPersistentStorage::open| is called |OCPersistentStorage| internally
    // with its own |SVR_DB_FILE_NAME|
    char cwd[1024] = {0};
    char svr_db_path[1024] = {0};
    GetCurrentWorkingDirectory(cwd, sizeof(cwd));
    snprintf(svr_db_path, sizeof(svr_db_path), "%s%s", cwd, SVR_DB_PATH);
    return fopen(svr_db_path, mode);
}

// callback function(s) for provisioning client using C-level provisioning API
static void ownershipTransferCB(void* ctx, int UNUSED1, OCProvisionResult_t* UNUSED2, bool hasError)
{
    (void)UNUSED1;
    (void)UNUSED2;
    (void)ctx;

    if(!hasError)
    {
        OIC_LOG_V(INFO, TAG, "Ownership Transfer SUCCEEDED - ctx: %s", (char*) ctx);
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "Ownership Transfer FAILED - ctx: %s", (char*) ctx);
    }
    g_callbackResult = !hasError;
    g_doneCB = true;
}


// callback function(s) for provisioning client using C-level provisioning API
static void removeDeviceCB(void* ctx, int UNUSED1, OCProvisionResult_t* UNUSED2, bool hasError)
{
    (void)UNUSED1;
    (void)UNUSED2;
    (void)ctx;

    if(!hasError)
    {
        OIC_LOG_V(INFO, TAG, "Remove device request SUCCEEDED - ctx: %s", (char*) ctx);
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "Remove device request FAILED - ctx: %s", (char*) ctx);
    }
    g_callbackResult = !hasError;
    g_doneCB = true;
}


static int waitCallbackRet(void)
{
    struct timespec timeout;
    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000000L;

    for(long long i=0; !g_doneCB && OTM_TIMEOUT * 100000000L * 1000L > i; ++i)
    {
        nanosleep(&timeout, NULL);
        if(OC_STACK_OK != OCProcess())
        {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return -1;
        }
    }

    return 0;
}

TEST(InitForOTM, NullParam)
{
    OCStackResult result = OC_STACK_ERROR;

    OTMCallbackData_t otmcb;
    otmcb.loadSecretCB = LoadSecretJustWorksCallback;
    otmcb.createSecureSessionCB = CreateSecureSessionJustWorksCallback;
    otmcb.createSelectOxmPayloadCB = CreateJustWorksSelectOxmPayload;
    otmcb.createOwnerTransferPayloadCB = CreateJustWorksOwnerTransferPayload;

    static OCPersistentStorage pstStr;
    pstStr.open = fopen_prvnMng;
    pstStr.read = fread;
    pstStr.write = fwrite;
    pstStr.close = fclose;
    pstStr.unlink = unlink;

    //Get current path to execute the sample server.
    char cwd[1024] = {0};
    char server1_path[1024] = {0};
    char server2_path[1024] = {0};
    char pdb_path[1024] = {0};
    char del_cmd[1024] = {0};
    char svrdb_path[1024] = {0};
    FILE* fp = NULL;

    GetCurrentWorkingDirectory(cwd, sizeof(cwd));
    EXPECT_TRUE(0 < strlen(cwd));

    //Delete previous PDB, if exist.
    GetCurrentWorkingDirectory(cwd, sizeof(cwd));
    snprintf(del_cmd, sizeof(del_cmd), "rm -rf %stest.db", cwd);
    system(del_cmd);

    //Delete previous SVR DB, if exist.
    snprintf(del_cmd, sizeof(del_cmd), "rm -rf %s%s", cwd, SVR_DB_PATH);
    system(del_cmd);

    //Generate default SVR DB.
    snprintf(svrdb_path, sizeof(svrdb_path), "%s%s", cwd, SVR_DB_PATH);
    fp = fopen(svrdb_path, "w");
    if(NULL != fp)
    {
        size_t numberItems = fwrite(DEFAULT_SVR_DB, 1, sizeof(DEFAULT_SVR_DB), fp);
        fclose(fp);
        ASSERT_TRUE(sizeof(DEFAULT_SVR_DB) == numberItems);
    }

    //Execute sample server to perform ownership transfer
    int status1 = 0;
    int status2 = 0;
    if(0 == (g_myPID1 = fork()))
    {
        snprintf(server1_path, sizeof(server1_path), "%ssample_server1", cwd);
        status1 = system(server1_path);
        (void)status1;
    }
    if(0 == (g_myPID2 = fork()))
    {
        snprintf(server2_path, sizeof(server2_path), "%ssample_server2", cwd);
        status2= system(server2_path);
        (void)status2;
    }

    // register the persistent storage handler for SVR
    result = OCRegisterPersistentStorageHandler(&pstStr);
    EXPECT_EQ(OC_STACK_OK, result);

    // initialize OC stack and provisioning manager
    result = OCInit(NULL, 0, OC_CLIENT_SERVER);
    EXPECT_EQ(OC_STACK_OK, result);

    //initialize Provisioning DB Manager

    snprintf(pdb_path, sizeof(pdb_path), "%stest.db", cwd);
    result = OCInitPM(pdb_path);
    EXPECT_EQ(OC_STACK_OK, result);

    // register callback function(s) for Justworks OxM
    result = OCSetOwnerTransferCallbackData(OIC_JUST_WORKS, &otmcb);
    EXPECT_EQ(OC_STACK_OK, result);

    g_doneCB = false;
    g_callbackResult = false;
}


TEST(PerformUnownedDeviceDiscovery, NullParam)
{
    OCStackResult result = OC_STACK_ERROR;

    OIC_LOG(INFO, TAG, "Discovering Only Unowned Devices on Network..\n");
    result = OCDiscoverUnownedDevices(DISCOVERY_TIMEOUT, &g_unownedDevices);
    EXPECT_EQ(OC_STACK_OK, result);

    int NumOfUnownDevice = 0;
    OCProvisionDev_t* tempDev = g_unownedDevices;
    while(tempDev)
    {
        NumOfUnownDevice++;
        tempDev = tempDev->next;
    }
    EXPECT_EQ(2, NumOfUnownDevice);
}

TEST(PerformJustWorksOxM, NullParam)
{
    OCStackResult result = OC_STACK_ERROR;

    OIC_LOG(INFO, TAG, "Try Ownership Transfer for Unowned Devices...\n");
    result = OCDoOwnershipTransfer((void*)g_otmCtx, g_unownedDevices, ownershipTransferCB);
    EXPECT_EQ(OC_STACK_OK, result);

    if(waitCallbackRet())  // input |g_doneCB| flag implicitly
    {
        OIC_LOG(ERROR, TAG, "OCProvisionCredentials callback error");
        return;
    }
    OIC_LOG(INFO, TAG, "Registered Discovered Unowned Device...\n");

    EXPECT_EQ(true, g_callbackResult);
    EXPECT_EQ(true, g_doneCB);
}


TEST(PerformOwnedDeviceDiscovery, NullParam)
{
    OCStackResult result = OC_STACK_ERROR;

    OIC_LOG(INFO, TAG, "Discovering Only Owned Devices on Network..\n");
    result = OCDiscoverOwnedDevices(DISCOVERY_TIMEOUT, &g_ownedDevices);
    EXPECT_EQ(OC_STACK_OK, result);

    int NumOfOwnDevice = 0;
    OCProvisionDev_t* tempDev = g_ownedDevices;
    while(tempDev)
    {
        NumOfOwnDevice++;
        tempDev = tempDev->next;
    }

    EXPECT_EQ(2/*Server*/ , NumOfOwnDevice);
}

TEST(PerformLinkDevices, NullParam)
{
    OicUuid_t myUuid;
    OCStackResult result = OC_STACK_ERROR;
    result = GetDoxmDeviceID(&myUuid);
    EXPECT_EQ(OC_STACK_OK, result);

    //Extract target device except PT to perform link devices.
    OCProvisionDev_t* dev1 = NULL;
    OCProvisionDev_t* dev2 = NULL;
    OCProvisionDev_t* tempDev = g_ownedDevices;

    while(tempDev)
    {
        if(memcmp(tempDev->doxm->deviceID.id, myUuid.id, UUID_LENGTH) != 0)
        {
            if(NULL == dev1)
            {
                dev1 = tempDev;
            }
            else if(NULL == dev2)
            {
                dev2 = tempDev;
                break;
            }
            else
            {
                break;
            }
        }
        tempDev = tempDev->next;
    }
    EXPECT_TRUE(NULL != dev1);
    EXPECT_TRUE(NULL != dev2);

    // TODO: Pairwise provisioning (Cred & ACL)
    // TODO: This part will be updated after ACL and credential data-structure is updated.

    EXPECT_EQ(OC_STACK_OK, result);
}

TEST(PerformUnlinkDevices, NullParam)
{
    OCStackResult result = OC_STACK_OK;

    // TODO: Unlink devices
    // TODO: This part will be updated after ACL and credential data-structure is updated.

    EXPECT_EQ(OC_STACK_OK, result);
}

TEST(PerformRemoveDevice, NullParam)
{
    OicUuid_t myUuid;
    OCStackResult result = OC_STACK_ERROR;
    result = GetDoxmDeviceID(&myUuid);
    EXPECT_EQ(OC_STACK_OK, result);

    //Extract target device except PT to perform remove device.
    OCProvisionDev_t* removeDev = g_ownedDevices;
    while(removeDev)
    {
        if(memcmp(removeDev->doxm->deviceID.id, myUuid.id, UUID_LENGTH) != 0)
        {
            break;
        }
        removeDev = removeDev->next;
    }
    EXPECT_TRUE(NULL != removeDev);

    g_doneCB = false;
    g_callbackResult = false;

    result = OCRemoveDevice((void*)g_otmCtx, DISCOVERY_TIMEOUT, removeDev, removeDeviceCB);
    EXPECT_EQ(OC_STACK_OK, result);
    EXPECT_EQ(true, g_callbackResult);
    EXPECT_EQ(true, g_doneCB);
}

TEST(FinalizeOTMTest, NullParam)
{
    OCStackResult result = OCStop();
    EXPECT_EQ(OC_STACK_OK, result);

    PMDeleteDeviceList(g_unownedDevices);
    PMDeleteDeviceList(g_ownedDevices);
    result = PDMClose();
    EXPECT_EQ(OC_STACK_OK, result);

    kill(g_myPID2, SIGKILL);
    kill(g_myPID1, SIGKILL);

    int interpreter_res1 = system("pkill -f \"sample_server1\"");
    EXPECT_TRUE(0 <= interpreter_res1);
    int interpreter_res2 = system("pkill -f \"sample_server2\"");
    EXPECT_TRUE(0 <= interpreter_res2);
}

