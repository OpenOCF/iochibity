//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ocstack.h"
#include "logger.h"
#include "octhread.h"
#include "cathreadpool.h"
#include "ocpayload.h"
#include "payload_logging.h"
#include "aclresource.h"
#include "acl_logging.h"
#include "crl_logging.h"
#include "crlresource.h"
#include "ocprovisioningmanager.h"
#include "casecurityinterface.h"
#include "mbedtls/ssl_ciphersuites.h"

#include "utils.h"
#include "cloudAuth.h"
#include "cloudCommon.h"
#include "cloudWrapper.h"
#include "cloudDiscovery.h"

#ifdef __unix__
#include <unistd.h> //for unlink
#endif

#define TAG "cloudCommon"

//[IOT-2147] Requests 50-83 are not confirmed to new OCF spec
//disable them for 1.3 rel, later (1.3.1 or after) they should be rewritten and enabled again
#define DISABLE_50_83_REQUESTS_FOR_1_3_REL 1

#define DEFAULT_HOST            "10.113.68.85"//"127.0.0.1"
#define DEFAULT_PORT            OC_MULTICAST_PORT
#define DEFAULT_AUTH_PROVIDER   "github"
#define DEFAULT_DB_FILE         "./cloud.dat"
#define DEFAULT_RESPONSE_WAIT_TIME (10 * 1000000) //10s

#define GITHUB_AUTH_LINK        "https://github.com/login?return_to=%2Flogin%2Foauth%2Fauthorize%3Fclient_id%3Dea9c18f540323b0213d0%26redirect_uri%3Dhttp%253A%252F%252Fwww.example.com%252Foauth_callback%252F"

#define CLIENT_ONLY(mode)       if (OC_SERVER == (mode)) { wrongRequest(); sendDataToServer = false; break; }

static bool fExit = false;

static oc_thread g_requestsThread = NULL;
static OCDevAddr endPoint;
static char token[1024] = "";
static char authProvider[1024] = DEFAULT_AUTH_PROVIDER;
static char *fname = DEFAULT_DB_FILE;
static uint64_t timeout;
static uint16_t g_credId = 0;

static oc_cond cond;
static oc_mutex mutex;

typedef enum {
    SIGN_UP       = 1,
    SIGN_IN       = 2,
    SIGN_OUT      = 3,

    HOST          = 4,
    PORT          = 5,
    AUTH_PROVIDER = 7,
    USE_RSA = 8,
    SAVE_TRUST_CERT = 9,
    USE_SECURE_CONN = 10,
    CONFIG_SELF_OWNERSHIP = 11,

    DISCOVERY     = 13,
    GET           = 14,
    PUT           = 15,
    POST          = 16,

    CSR_SIGN      = 19,

    CRL_GET       = 20,
    CRL_POST      = 21,

    ACL_ID_CREATE = 30,
    ACL_ID_GET_BY_DEVICE = 31,
    ACL_ID_DELETE = 32,

    ACL_INDIVIDUAL_GET_INFO = 40,
    ACL_INDIVIDUAL_UPDATE_ACE = 41,
    ACL_INDIVIDUAL_UPDATE = 42,
    ACL_INDIVIDUAL_DELETE = 43,
    ACL_INDIVIDUAL_DELETE_ACE = 44,

#ifndef DISABLE_50_83_REQUESTS_FOR_1_3_REL
    ACL_GROUP_CREATE = 50,
    ACL_GROUP_FIND   = 51,
    ACL_GROUP_JOIN   = 52,
    ACL_GROUP_OBSERVE= 53,
    ACL_GROUP_DELETE = 54,

    ACL_GROUP_SHARE_DEVICE  = 60,
    ACL_GROUP_DELETE_DEVICE  = 61,
    ACL_GROUP_GET_INFO  = 62,

    ACL_POLICY_CHECK_REQUEST = 70,

    ACL_GROUP_INVITE_USER = 80,
    ACL_GROUP_GET_INVITE  = 81,
    ACL_GROUP_DELETE_INVITE  = 82,
    ACL_GROUP_CANCEL_INVITE  = 83,
#endif

    EXIT          = 99
}CloudRequest;

static void printMenu(OCMode mode)
{
    char *title = "Client";
    if (OC_SERVER == mode)
    {
        title = "Server";
    }
    printf("************************************************************\n");
    printf("****************** Cloud %s Requests *******************\n", title);
    printf("************************************************************\n");
    printf("** AUTHORIZATION\n");
    printf("** %d - Sign Up request\n", SIGN_UP);
    printf("** %d - Sign In request\n", SIGN_IN);
    printf("** %d - Sign Out request\n", SIGN_OUT);

    printf("** SETTINGS \n");
    printf("** %d - Change default host\n", HOST);
    printf("** %d - Change default port\n", PORT);
    printf("** %d - Change default auth provider\n", AUTH_PROVIDER);
    printf("** %d - Change TLS cipher suite (ECDSA/RSA)\n", USE_RSA);
    printf("** %d - Save Trust Cert. Chain into Cred of SVR\n", SAVE_TRUST_CERT);
    printf("** %d - Change Protocol type (CoAP/CoAPs)\n", USE_SECURE_CONN);
    printf("** %d - Configure SVRdb as Self-OwnerShip\n", CONFIG_SELF_OWNERSHIP);

    if (OC_CLIENT == mode)
    {
        printf("** DISCOVERY\n");
        printf("** %d - Start Discovery\n", DISCOVERY);
        printf("** %d - Get Request\n", GET);
        printf("** %d - Put Request\n", PUT);
        printf("** %d - Post Request\n", POST);
    }

    printf("** CERTIFICATE REQUEST\n");
    printf("** %d - Certificate Request\n", CSR_SIGN);

    printf("** CRL\n");
    printf("** %d - CRL GET Request\n", CRL_GET);
    printf("** %d - CRL POST Request\n", CRL_POST);

    printf("** ACL MANAGER\n");
    printf("** %d - ACL id create Request\n", ACL_ID_CREATE);
    printf("** %d - ACL id retrieve by device Request\n", ACL_ID_GET_BY_DEVICE);
    printf("** %d - ACL id delete Request\n", ACL_ID_DELETE);

    printf("** ACL INDIVIDUAL\n");
    printf("** %d - ACL individual get info Request\n", ACL_INDIVIDUAL_GET_INFO);
    printf("** %d - ACL individual update ACE Request\n", ACL_INDIVIDUAL_UPDATE_ACE);
    printf("** %d - ACL individual update Request\n", ACL_INDIVIDUAL_UPDATE);
    printf("** %d - ACL individual delete Request\n", ACL_INDIVIDUAL_DELETE);
    printf("** %d - ACL individual delete ACE Request\n", ACL_INDIVIDUAL_DELETE_ACE);

#ifndef DISABLE_50_83_REQUESTS_FOR_1_3_REL
    printf("** ACL GROUP MANAGER\n");
    printf("** %d - ACL Create Group Request\n", ACL_GROUP_CREATE);
    printf("** %d - ACL Find Group Request\n", ACL_GROUP_FIND);
    printf("** %d - ACL Join to invited Group Request\n", ACL_GROUP_JOIN);
    printf("** %d - ACL Observe Group Request\n", ACL_GROUP_OBSERVE);
    printf("** %d - ACL Delete Group Request\n", ACL_GROUP_DELETE);

    printf("** ACL INDIVIDUAL GROUP\n");
    printf("** %d - ACL Share device into Group Request\n", ACL_GROUP_SHARE_DEVICE);
    printf("** %d - ACL Delete device from Group Request\n", ACL_GROUP_DELETE_DEVICE);
    printf("** %d - ACL Get Group Info Request\n", ACL_GROUP_GET_INFO);

    printf("** ACL POLICY ENFORCEMENT\n");
    printf("** %d - ACL Check Permissions Request\n", ACL_POLICY_CHECK_REQUEST);

    printf("** ACL MEMBER INVITATION\n");
    printf("** %d - ACL Invite user to group Request\n", ACL_GROUP_INVITE_USER);
    printf("** %d - ACL Retrieve invitation Request\n", ACL_GROUP_GET_INVITE);
    printf("** %d - ACL Delete invitation Request\n", ACL_GROUP_DELETE_INVITE);
    printf("** %d - ACL Cancel invitation Request\n", ACL_GROUP_CANCEL_INVITE);
#endif

    printf("** EXIT\n");
    printf("** %d - Exit cloud %s\n", EXIT, title);
    printf("************************************************************\n");
}

void unlockMenu(void *data)
{
    OICFree(data);

    if (!fExit)
    {
        oc_mutex_lock(mutex);
        oc_cond_signal(cond);
        oc_mutex_unlock(mutex);
    }
}

/**
 * This is default callback to all requests
 * Used to sync with main menu
 *
 * @param[in] ctx          context
 * @param[in] result       result
 * @param[in] data         data
 */
static void handleCB(void* ctx, OCClientResponse *response, void* data)
{
    OC_UNUSED(ctx);
    OC_UNUSED(data);

    if (response)
    {
        OIC_LOG_V(INFO, TAG, "%s: Received result = %d", __func__, response->result);

        if (response->payload)
        {
            OIC_LOG(INFO, TAG, "Payload received:");
            OIC_LOG_PAYLOAD(INFO, response->payload);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Received NULL response", __func__);
    }

    unlockMenu(NULL);
}

/**
 * This function prints Acl id and calls default callback function handleCB()
 *
 * @param[in] ctx          context
 * @param[in] result       result
 * @param[in] aclId        acl id
 */
static void handleAclIdCB(void* ctx, OCClientResponse *response, void* aclId)
{
    OIC_LOG_V(INFO, TAG, "Received Acl id = %s", (char *)aclId);
    handleCB(ctx, response, aclId);
    OICFree(aclId);
}

#ifndef DISABLE_50_83_REQUESTS_FOR_1_3_REL
/**
 * This function prints group id and calls default callback function handleCB()
 *
 * @param[in] ctx          context
 * @param[in] result       result
 * @param[in] groupId      group id
 */
static void handleAclCreateGroupCB(void* ctx, OCClientResponse *response, void* groupId)
{
    OIC_LOG_V(INFO, TAG, "Received gid = %s", (char *)groupId);
    handleCB(ctx, response, groupId);
    OICFree(groupId);
}

/**
 * This function prints group policy and calls default callback function handleCB()
 *
 * @param[in] ctx          context
 * @param[in] result       result
 * @param[in] gp           group policy
 */
static void handleAclPolicyCheckCB(void* ctx, OCClientResponse *response, void* gp)
{
    OIC_LOG_V(INFO, TAG, "Received gp = %s", (char *)gp);
    handleCB(ctx, response, gp);
    OICFree(gp);
}
#endif

/**
 * This function prints received acl and calls default callback function handleCB()
 *
 * @param[in] ctx          context
 * @param[in] result       result
 * @param[in] acl          acl
 */
static void handleAclIndividualGetInfoCB(void* ctx, OCClientResponse *response, void* acl)
{
    OIC_LOG_ACL(INFO, acl);
    handleCB(ctx, response, acl);
    //can't delete acl here because its ACE's were added to gAcl
    //TODO: changes in aclresources.c required to fix that
}

#ifndef DISABLE_50_83_REQUESTS_FOR_1_3_REL
/**
 * This function prints received group id list and calls default callback function handleCB()
 *
 * @param[in] ctx          context
 * @param[in] result       result
 * @param[in] gidList      group id list
 */
static void handleAclFindMyGroupCB(void* ctx, OCClientResponse *response, void* gidList)
{
    printStringArray((stringArray_t *)gidList);
    handleCB(ctx, response, gidList);
    clearStringArray((stringArray_t *)gidList);
}
#endif

/**
 * This function prints received acl and calls default callback function handleCB()
 *
 * @param[in] ctx          context
 * @param[in] result       result
 * @param[in] crl          crl
 */
static void handleGetCrlCB(void* ctx, OCClientResponse *response, void* crl)
{
    OIC_LOG_CRL(INFO, crl);
    handleCB(ctx, response, crl);
    DeleteCrl((OicSecCrl_t *)crl);
}

#ifndef DISABLE_50_83_REQUESTS_FOR_1_3_REL
/**
 * This function prints received invitation response and calls default callback function handleCB()
 *
 * @param[in] ctx          context
 * @param[in] result       result
 * @param[in] invite       invitation response (it has inviteResponse_t* type)
 */
static void handleAclGetInvitationCB(void* ctx, OCClientResponse *response, void* invite)
{
    printInviteResponse((inviteResponse_t *)invite);
    handleCB(ctx, response, invite);
    clearInviteResponse((inviteResponse_t *)invite);
    OICFree(invite);
}
#endif

static OCStackResult saveTrustCert(void)
{
    OCStackResult res = OC_STACK_ERROR;
    OIC_LOG(INFO, TAG, "Save Trust Cert. Chain into Cred of SVR");

    ByteArray_t trustCertChainArray = {0, 0};
    const char *filename = "rootca.crt";

    if (!readFile(filename, (OCByteString *)&trustCertChainArray))
    {
        OIC_LOG_V(ERROR, TAG, "Can't read %s file", filename);
        return OC_STACK_ERROR;
    }
    OIC_LOG_BUFFER(DEBUG, TAG, trustCertChainArray.data, trustCertChainArray.len);

    res = OCSaveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len, OIC_ENCODING_PEM,&g_credId);

    if (OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "OCSaveTrustCertChain API error");
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "CredId of Saved Trust Cert. Chain into Cred of SVR : %d.\n", g_credId);
    }
    OICFree(trustCertChainArray.data);

    return res;
}

static OCStackResult configSelfOwnership(void)
{
    OCStackResult res = OC_STACK_ERROR;
    OIC_LOG(INFO, TAG, "Configures SVR DB as self-ownership.");

    res = OCConfigSelfOwnership();

    if (OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "OCConfigSelfOwnership API error. Please check SVR DB");
    }
    else
    {
        OIC_LOG(INFO, TAG, "Success to configures SVR DB as self-ownership");
    }

    return res;
}

static void wrongRequest()
{
    printf(">> Entered Wrong Menu Number. Please Enter Again\n\n");
}

static void *userRequests(void *data)
{
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "Received NULL data");
        return NULL;
    }

    OCMode mode = *(OCMode*)data;

    memset(&endPoint, 0, sizeof(endPoint));
    strncpy(endPoint.addr, DEFAULT_HOST, sizeof(endPoint.addr));
    endPoint.port = DEFAULT_PORT;

    mutex = oc_mutex_new();
    cond = oc_cond_new();

    while (false == fExit)
    {
        OCStackResult res = OC_STACK_ERROR;
        bool sendDataToServer = true;
        timeout = DEFAULT_RESPONSE_WAIT_TIME;
        //startup report
        printf("-----------------------------------------------------------\n");
        printf("Connecting to: %s:%d\n", endPoint.addr, endPoint.port);
        printf("via auth provider: %s\n", authProvider);
        printf("srv file: %s\n", fname);
        printf("CoAP prefix: %s\n", DEFAULT_PREFIX);
        printf("-----------------------------------------------------------\n");

        printMenu(mode);

        int request = 0;
        readInteger(&request, "Menu number", "see above");

        switch (request)
        {
        case SIGN_UP:
            if (0 == strncmp(authProvider, DEFAULT_AUTH_PROVIDER, sizeof(authProvider)))
            {
                printf("Paste to browser %s and get auth code\n", GITHUB_AUTH_LINK);
            }
            readString(token, sizeof(token), "auth token", "check link above");
            res = CloudSignUp(&endPoint, authProvider, token);
            break;
        case SIGN_IN:
            res = CloudSignIn(&endPoint);
            break;
        case SIGN_OUT:
            res = CloudSignOut(&endPoint);
            break;
        case HOST:
            readString(endPoint.addr, sizeof(endPoint.addr), "host ip address", DEFAULT_HOST);
            sendDataToServer = false;
            break;
        case PORT:
        {
            char example[8];
            snprintf(example, sizeof(example), "%d", DEFAULT_PORT);
            uint16_t tmp = 0;
            readUInt16(&tmp, "port number", example);
            endPoint.port = tmp;
            sendDataToServer = false;
        }
        break;
        case CRL_GET:
            res = OCWrapperGetCRL(&endPoint, handleGetCrlCB);
            break;
        case CRL_POST:
            res = OCWrapperPostCRL(&endPoint, handleCB);
            break;
#ifndef DISABLE_50_83_REQUESTS_FOR_1_3_REL
        case ACL_GROUP_CREATE:
            res = OCWrapperAclCreateGroup(&endPoint, handleAclCreateGroupCB);
            break;
        case ACL_GROUP_FIND:
            res = OCWrapperAclFindMyGroup(&endPoint, handleAclFindMyGroupCB);
            break;
        case ACL_GROUP_DELETE:
            res = OCWrapperAclDeleteGroup(&endPoint, handleCB);
            break;
        case ACL_GROUP_JOIN:
            res = OCWrapperAclJoinToInvitedGroup(&endPoint, handleCB);
            break;
        case ACL_GROUP_OBSERVE:
            res = OCWrapperAclObserveGroup(&endPoint, handleCB);
            break;
        case ACL_GROUP_SHARE_DEVICE:
            res = OCWrapperAclShareDeviceIntoGroup(&endPoint, handleCB);
            break;
        case ACL_GROUP_DELETE_DEVICE:
            res = OCWrapperAclDeleteDeviceFromGroup(&endPoint, handleCB);
            break;
        case ACL_GROUP_GET_INFO:
            res = OCWrapperAclGroupGetInfo(&endPoint, handleCB);
            break;
        case ACL_GROUP_INVITE_USER:
            res = OCWrapperAclInviteUser(&endPoint, handleCB);
            break;
        case ACL_GROUP_GET_INVITE:
            res = OCWrapperAclGetInvitation(&endPoint, handleAclGetInvitationCB);
            break;
        case ACL_GROUP_DELETE_INVITE:
            res = OCWrapperAclDeleteInvitation(&endPoint, handleCB);
            break;
        case ACL_GROUP_CANCEL_INVITE:
            res = OCWrapperAclCancelInvitation(&endPoint, handleCB);
            break;
        case ACL_POLICY_CHECK_REQUEST:
            res = OCWrapperAclPolicyCheck(&endPoint, handleAclPolicyCheckCB);
            break;
#endif
        case ACL_ID_GET_BY_DEVICE:
            res = OCWrapperAclIdGetByDevice(&endPoint, handleAclIdCB);
            break;
        case ACL_ID_CREATE:
            res = OCWrapperAclIdCreate(&endPoint, handleAclIdCB);
            break;
        case ACL_ID_DELETE:
            res = OCWrapperAclIdDelete(&endPoint, handleCB);
            break;
        case ACL_INDIVIDUAL_GET_INFO:
            res = OCWrapperAclIndividualGetInfo(&endPoint, handleAclIndividualGetInfoCB);
            break;
        case ACL_INDIVIDUAL_UPDATE_ACE:
            res = OCWrapperAclIndividualUpdateAce(&endPoint, handleCB);
            break;
        case ACL_INDIVIDUAL_UPDATE:
            res = OCWrapperAclIndividualUpdate(&endPoint, handleCB);
            break;
        case ACL_INDIVIDUAL_DELETE:
            res = OCWrapperAclIndividualDelete(&endPoint, handleCB);
            break;
        case ACL_INDIVIDUAL_DELETE_ACE:
            res = OCWrapperAclIndividualDeleteAce(&endPoint, handleCB);
            break;
        case CSR_SIGN:
            res = OCWrapperCertificateIssueRequest(&endPoint, handleCB);
            break;
        case DISCOVERY:
            CLIENT_ONLY(mode);
            res = InitDiscovery();
            break;
        case GET:
            CLIENT_ONLY(mode);
            res = InitRequest(OC_REST_GET);
            break;
        case PUT:
            CLIENT_ONLY(mode);
            res= InitRequest(OC_REST_PUT);
            break;
        case POST:
            CLIENT_ONLY(mode);
            res= InitRequest(OC_REST_POST);
            break;
        case USE_RSA:
        {
            int tmp = 0;
            readInteger(&tmp, "Select Cipher Suite", "0 - ECDSA, other - RSA");
            uint16_t cipher = tmp? MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256:
                                   MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8;
            if (OC_STACK_OK != OCSelectCipherSuite(cipher, OC_ADAPTER_TCP))
            {
                OIC_LOG(ERROR, TAG, "CASelectCipherSuite returned an error");
            }
            sendDataToServer = false;
        }
            break;
        case SAVE_TRUST_CERT:
            saveTrustCert();
            sendDataToServer = false;
            break;
        case USE_SECURE_CONN:
        {
            int tmp = 0;
            readInteger(&tmp, "CoAP protocol type", "0 - non-secure, 1 - secure");
            setCoapPrefix(0 == tmp ? false : true);
            sendDataToServer = false;
        }
            break;
        case CONFIG_SELF_OWNERSHIP:
            configSelfOwnership();
            sendDataToServer = false;
            break;
        case EXIT:
            oc_mutex_free(mutex);
            oc_cond_free(cond);
            fExit = true;
            sendDataToServer = false;
            break;
        default:
            wrongRequest();
            sendDataToServer = false;
            break;
        }

        //if requests were sent then wait response
        if (sendDataToServer)
        {
            if (OC_STACK_OK == res)
            {
                oc_mutex_lock(mutex);
                oc_cond_wait_for(cond, mutex, timeout);
                oc_mutex_unlock(mutex);
            }
            else
            {
                OIC_LOG_V(ERROR, TAG, "Request returned an error %d", res);
            }
        }
    }

    return NULL;
}

FILE* server_fopen(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(fname, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}

/**
 * Check file accessibility
 *
 * @param[in] name        file path
 * @return true           if check was successful
 */
static bool checkConfig(const char *name)
{
    FILE* file = fopen(name, "rb");

    if (file)
    {
        fclose(file);
        return true;
    }
    return false;
}

static void printUsage(char *name)
{
    printf("Wrong arguments count!\n");
    printf("Usage : %s <database_filename>\n", name);
    printf("Examples : 1) %s 2) %s cloud.dat\n", name, name);
}

bool parseCommandLineArguments(int argc, char *argv[])
{
    bool result = true;
    if (2 == argc)
    {
        fname = argv[1];

        if (!checkConfig(fname))
        {
            OIC_LOG_V(ERROR, TAG, "Can't open database file %s, exit!", fname);
            result = false;
        }
    }
    else if (argc > 2)
    {
        printUsage(argv[0]);
        result = false;
    }
    return result;
}

OCStackResult initPersistentStorage()
{
    //Initialize Persistent Storage for SVR database
    static OCPersistentStorage ps = {server_fopen, fread, fwrite, fclose, unlink};

    return OCRegisterPersistentStorageHandler(&ps);
}

OCStackResult startRequestsThread(OCMode *mode)
{
    OCThreadResult_t res = oc_thread_new(&g_requestsThread, userRequests, mode);

    if (OC_THREAD_SUCCESS != res)
    {
        OIC_LOG_V(ERROR, TAG, "oc_thread_new failed - error %u", res);
        return OC_STACK_NO_MEMORY;
    }

    return OC_STACK_OK;
}

OCStackResult initProcess(OCMode mode)
{
    return OCInit(NULL, 0, mode);
}

void startProcess()
{
    while(false == fExit)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG,"OCProcess process error, exit\n");
            break;
        }
    }

    if (OCStop() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStop process error\n");
    }
}

void freeThreadResources()
{
    if (g_requestsThread)
    {
        OCThreadResult_t res = oc_thread_wait(g_requestsThread);

        if (OC_THREAD_SUCCESS != res)
        {
            OIC_LOG_V(ERROR, TAG, "oc_thread_wait failed - error %u", res);
        }

        res = oc_thread_free(g_requestsThread);

        if (OC_THREAD_SUCCESS != res)
        {
            OIC_LOG_V(ERROR, TAG, "oc_thread_free failed - error %u", res);
        }

        g_requestsThread = NULL;
    }
}
