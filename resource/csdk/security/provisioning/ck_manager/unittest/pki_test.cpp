/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *

 ******************************************************************/

#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <oic_malloc.h>
#include <gtest/gtest.h>
#include "ocstack.h"

#include "cert_generator.h"
#include "ck_manager.h"
#include "pki.h"
#include "sn_store.h"
#include "der_dec.h"
#include "crl.h"
#include "crl_generator.h"
#include "crlresource.h"
#include "ckm_info.h"


#define RUNS          1
#define MAX_LEN     1000
#define TEST_SN       50
#define READ_WRITE_BLOCK_N 1ul
#define N_LENGTH_BYTES 3

const char *CKMI_PS_FILE_NAME = "CKMInfo.dat";

#define CRL_DEFAULT_CRL_ID           1
#define CRL_DEFAULT_THIS_UPDATE     "150101000000Z"
#define CRL_DEFAULT_CRL_DATA        "-"

#define NUMBER_OF_REVOKED 2

OCPersistentStorage ps = { NULL, NULL, NULL, NULL, NULL};

//#define NUM_ACE_FOR_WILDCARD_IN_CKM1_JSON (2)

FILE* ckm_fopen(const char * /*path*/, const char *mode)
{
    return fopen(CKMI_PS_FILE_NAME, mode);
}

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)

static char* resolve_file_path(const char* filename )
{
    int len = strlen(STRINGIZE(SECURITY_BUILD_UNITTEST_DIR)) + strlen(filename) + 1;
    char *filepath = (char *)OICCalloc(1, len);

    if (!filepath)
    {
        printf("filepath memory allocation failed. \n");
        return NULL;
    }
    int ret = snprintf(filepath, len, "%s%s", STRINGIZE(SECURITY_BUILD_UNITTEST_DIR), filename);
    return filepath;
}

void SetPersistentHandler(OCPersistentStorage *ps)
{
    if(ps)
    {
        ps->open = ckm_fopen;
        ps->read = fread;
        ps->write = fwrite;
        ps->close = fclose;
        ps->unlink = unlink;
    }
}

// Length of test certificate
#define SIMPLE_CRT_LEN 469

class PKITest : public ::testing::Test
{
public:
    static void SetUpTestCase()
    {
        SetPersistentHandler(&ps);
        OCStackResult res = OCRegisterPersistentStorageHandler(&ps);
        ASSERT_TRUE(res == OC_STACK_OK);
    }

    static void TearDownTestCase()
    {
    }

    static CertificateX509  g_certificate;

    static const ByteArray g_caPublicKey;

    static const ByteArray g_derCode ;

    static ByteArray g_serNum;
};

CertificateX509  PKITest::g_certificate;

const ByteArray PKITest::g_derCode = {(uint8_t[])
    {
        0x30, 0x82, 0x01, 0xd1, 0x30, 0x82, 0x01, 0x77, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x09, 0x00,
        0xd7, 0x56, 0x8c, 0xfc, 0x53, 0x18, 0xb0, 0xab, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce,
        0x3d, 0x04, 0x03, 0x02, 0x30, 0x45, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
        0x02, 0x41, 0x55, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x0a, 0x53, 0x6f,
        0x6d, 0x65, 0x2d, 0x53, 0x74, 0x61, 0x74, 0x65, 0x31, 0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x04,
        0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x20, 0x57, 0x69, 0x64, 0x67,
        0x69, 0x74, 0x73, 0x20, 0x50, 0x74, 0x79, 0x20, 0x4c, 0x74, 0x64, 0x30, 0x1e, 0x17, 0x0d, 0x31,
        0x35, 0x30, 0x33, 0x31, 0x32, 0x31, 0x32, 0x32, 0x35, 0x31, 0x31, 0x5a, 0x17, 0x0d, 0x31, 0x37,
        0x30, 0x33, 0x31, 0x31, 0x31, 0x32, 0x32, 0x35, 0x31, 0x31, 0x5a, 0x30, 0x45, 0x31, 0x0b, 0x30,
        0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x41, 0x55, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03,
        0x55, 0x04, 0x08, 0x0c, 0x0a, 0x53, 0x6f, 0x6d, 0x65, 0x2d, 0x53, 0x74, 0x61, 0x74, 0x65, 0x31,
        0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x74, 0x65, 0x72, 0x6e,
        0x65, 0x74, 0x20, 0x57, 0x69, 0x64, 0x67, 0x69, 0x74, 0x73, 0x20, 0x50, 0x74, 0x79, 0x20, 0x4c,
        0x74, 0x64, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06,
        0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x8c, 0xc8, 0x92,
        0x1d, 0xaa, 0x7f, 0xf0, 0xe4, 0xb2, 0x75, 0xd6, 0x4a, 0xf1, 0xd5, 0x14, 0x3f, 0x1a, 0x09, 0xc5,
        0x3e, 0x52, 0xd6, 0xda, 0xa0, 0xbf, 0x90, 0x43, 0xd1, 0x6b, 0xfe, 0xd1, 0xb3, 0x75, 0x5c, 0xdd,
        0x69, 0xac, 0x42, 0xa1, 0xcb, 0x03, 0x16, 0xee, 0xa4, 0x30, 0xa5, 0x8d, 0x36, 0x8f, 0xc5, 0x7b,
        0xb4, 0xb5, 0x6a, 0x7d, 0x9b, 0x16, 0x04, 0x46, 0xab, 0xae, 0xbb, 0x56, 0xa1, 0xa3, 0x50, 0x30,
        0x4e, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x5c, 0x0e, 0x30, 0xa8,
        0x8e, 0x7f, 0xc9, 0x02, 0xcd, 0xa8, 0xed, 0x0d, 0x1a, 0x1b, 0xd9, 0x7d, 0xe6, 0xce, 0x2a, 0x59,
        0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x5c, 0x0e, 0x30,
        0xa8, 0x8e, 0x7f, 0xc9, 0x02, 0xcd, 0xa8, 0xed, 0x0d, 0x1a, 0x1b, 0xd9, 0x7d, 0xe6, 0xce, 0x2a,
        0x59, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30,
        0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x48, 0x00, 0x30, 0x45,
        0x02, 0x21, 0x00, 0xf6, 0x79, 0xed, 0x69, 0xd5, 0xe5, 0xba, 0x42, 0x14, 0xfc, 0xce, 0x47, 0xf1,
        0x61, 0x1c, 0x51, 0x11, 0x2b, 0xba, 0x04, 0x70, 0x56, 0x78, 0xaf, 0xa9, 0xa6, 0x98, 0x8f, 0x4b,
        0xa8, 0x11, 0x67, 0x02, 0x20, 0x3a, 0xdf, 0xf1, 0x74, 0xc9, 0x2f, 0xfb, 0x84, 0x46, 0xde, 0xbc,
        0x2d, 0xda, 0xe3, 0x05, 0xb4, 0x81, 0x31, 0x45, 0xf7, 0x3d, 0x71, 0x46, 0x07, 0xa7, 0xd8, 0xcb,
        0xae, 0x1e, 0x1b, 0x1c, 0x5a
    }, SIMPLE_CRT_LEN };


const ByteArray PKITest::g_caPublicKey = {(uint8_t[])
{
    0x8c, 0xc8, 0x92, 0x1d, 0xaa, 0x7f, 0xf0, 0xe4, 0xb2, 0x75, 0xd6, 0x4a, 0xf1, 0xd5, 0x14, 0x3f,
    0x1a, 0x09, 0xc5, 0x3e, 0x52, 0xd6, 0xda, 0xa0, 0xbf, 0x90, 0x43, 0xd1, 0x6b, 0xfe, 0xd1, 0xb3,
    0x75, 0x5c, 0xdd, 0x69, 0xac, 0x42, 0xa1, 0xcb, 0x03, 0x16, 0xee, 0xa4, 0x30, 0xa5, 0x8d, 0x36,
    0x8f, 0xc5, 0x7b, 0xb4, 0xb5, 0x6a, 0x7d, 0x9b, 0x16, 0x04, 0x46, 0xab, 0xae, 0xbb, 0x56, 0xa1
}, PUBLIC_KEY_SIZE };


ByteArray PKITest::g_serNum = {(uint8_t[SERIAL_NUMBER_MAX_LEN]) {0}, SERIAL_NUMBER_MAX_LEN};

//registering persistent storage test
TEST(CKManager, RegisterPersistentStorage)
{
    SetPersistentHandler(&ps);
    ASSERT_EQ(OC_STACK_OK, OCRegisterPersistentStorageHandler(&ps));
}

//check decoding predefined certificate
TEST(X509Certificate, DecodeTest)
{
    ByteArray code = PKITest::g_derCode;

    ASSERT_EQ(DecodeCertificate(code, &PKITest::g_certificate), PKI_SUCCESS);
    code.data = NULL;
    ASSERT_NE(DecodeCertificate(code, &PKITest::g_certificate), PKI_SUCCESS);
}

//check decoding of random symbols sequence
TEST(X509Certificate, RandomDecode)
{
    srand((unsigned int)time(NULL));

    ByteArray code;
    INIT_BYTE_ARRAY(code);

    for (unsigned int i = 0; i < RUNS; i++)
    {
        code.len = (size_t)((size_t)rand() % MAX_LEN);
        code.data = (uint8_t *)malloc(code.len * sizeof(uint8_t));

        EXPECT_NE(code.data, (uint8_t *)NULL);

        for (unsigned int j = 0; j < code.len; j++)
        {
            code.data[j] = (uint8_t)(rand() % 128 + 1);
        }

        EXPECT_NE(PKI_SUCCESS, DecodeCertificate(code, &PKITest::g_certificate));

        free(code.data);
    }
}
//testing validity check of predefined certificate
TEST(X509Certificate, testCheckValidity)
{
    CertificateX509 tempCrt;
    ASSERT_EQ(PKI_SUCCESS, DecodeCertificate(PKITest::g_derCode, &tempCrt));
    ASSERT_EQ(PKI_SUCCESS, CheckValidity(tempCrt.validFrom, tempCrt.validTo));
    ByteArray temp = tempCrt.validTo;

    tempCrt.validTo = tempCrt.validFrom;
    tempCrt.validFrom = temp;
    ASSERT_EQ(PKI_CERT_DATE_INVALID, CheckValidity(tempCrt.validFrom, tempCrt.validTo));
}

//testing signature check of predefined certificate
TEST(X509Certificate, CheckSignature)
{
    ByteArray code = PKITest::g_derCode;

    ASSERT_EQ(PKI_SUCCESS, CheckCertificate(code, PKITest::g_caPublicKey));
    code.data = NULL;
    ASSERT_NE(PKI_SUCCESS, CheckCertificate(code, PKITest::g_caPublicKey));
}

//test saving certificate into file
TEST_F(PKITest, DERCertificateFile)
{
    uint8_t derData[ISSUER_MAX_CERT_SIZE] = {0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] = {0};

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    certDer.data = derData;
    certDer.len = ISSUER_MAX_CERT_SIZE;

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = sizeof(caPubKey);
    privKeyIss.data = caPrivKey;
    privKeyIss.len = sizeof(caPrivKey);
    rootName.data = (uint8_t *)"ROOT1";
    rootName.len = strlen((char *)rootName.data);
    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());

    for (int i = 1; i <= RUNS; i++)
    {
        ASSERT_EQ(PKI_SUCCESS, GenerateCAKeyPair(&privKeyIss, &pubKeyIss));
        ASSERT_EQ(PKI_SUCCESS, SetSerialNumber(i));
        ASSERT_EQ(PKI_SUCCESS, SetRootName(rootName));
        ASSERT_EQ(PKI_SUCCESS, CKMIssueRootCertificate(0, 0, &certDer));
        ASSERT_EQ(PKI_SUCCESS, CheckCertificate(certDer, pubKeyIss));
        ASSERT_EQ(PKI_SUCCESS, GenerateDERCertificateFile (&certDer, "der_cert"));
    }
    ASSERT_EQ(CloseCKMInfo(), PKI_SUCCESS);
}

//test checking time validity of generated certificate
TEST_F(PKITest, TimeValidity)
{
    uint8_t derData[ISSUER_MAX_CERT_SIZE] = {0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] = {0};

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKey = BYTE_ARRAY_INITIALIZER;
    ByteArray privKey = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    privKey.data = caPrivKey;
    privKey.len = sizeof(caPrivKey);

    certDer.data = derData;
    certDer.len = sizeof(derData);

    pubKey.data = caPubKey;
    pubKey.len = sizeof(caPubKey);

    rootName.data = (uint8_t *)"ROOT3";
    rootName.len = strlen((char *)rootName.data);
    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());

    for (int i = 1; i <= RUNS; i++)
    {
        ASSERT_EQ(PKI_SUCCESS, GenerateCAKeyPair(&privKey, &pubKey));
        ASSERT_EQ(PKI_SUCCESS, SetSerialNumber(i));
        ASSERT_EQ(PKI_SUCCESS, SetRootName(rootName));

        ASSERT_EQ(PKI_SUCCESS, CKMIssueRootCertificate(0, 0, &certDer));
        ASSERT_EQ(PKI_SUCCESS, CheckCertificate(certDer, pubKey));

        certDer.len = sizeof(derData);
        ASSERT_EQ(PKI_SUCCESS, CKMIssueRootCertificate(0, (uint8_t *)"130101000000Z", &certDer));
        ASSERT_EQ(PKI_CERT_DATE_INVALID, CheckCertificate(certDer, pubKey));

        certDer.len = sizeof(derData);
        ASSERT_EQ(PKI_SUCCESS, CKMIssueRootCertificate((uint8_t *)"170101000000Z", 0, &certDer));
        ASSERT_EQ(PKI_CERT_DATE_INVALID, CheckCertificate(certDer, pubKey));
    }
    ASSERT_EQ(PKI_SUCCESS, CloseCKMInfo());
}

//testing certificate generation by certificate signing request
TEST_F(PKITest, CertificateSigningRequest)
{
    uint8_t certData[ISSUER_MAX_CERT_SIZE] = {0};
    uint8_t csrData[CSR_MAX_SIZE] = {0};
    uint8_t subjPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t subjPrivKey[PRIVATE_KEY_SIZE] = {0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] = {0};
    uint8_t *subjName = (uint8_t *)"Subject05";

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray csrDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeySubj = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeySubj = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    certDer.data = certData;
    certDer.len = sizeof(certData);
    csrDer.data = csrData;
    csrDer.len = CSR_MAX_SIZE;

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = sizeof(caPubKey);
    privKeyIss.data = caPrivKey;
    privKeyIss.len = sizeof(caPrivKey);
    pubKeySubj.data = subjPubKey;
    pubKeySubj.len = sizeof(subjPubKey);
    privKeySubj.data = subjPrivKey;
    privKeySubj.len = sizeof(subjPrivKey);
    rootName.data = (uint8_t *)"ROOT2";
    rootName.len = strlen((char *)rootName.data);
    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());

    ASSERT_EQ(GenerateCAKeyPair(&privKeyIss, &pubKeyIss), PKI_SUCCESS);
    ASSERT_EQ(SetSerialNumber(1), PKI_SUCCESS);
    ASSERT_EQ(SetRootName(rootName), PKI_SUCCESS);

    for (int i = 1; i <= RUNS; i++)
    {
        ASSERT_EQ(PKI_SUCCESS, GenerateKeyPair(&privKeySubj, &pubKeySubj));
        ASSERT_EQ(PKI_SUCCESS, GenerateCSR(subjName, subjPubKey, subjPrivKey, &csrDer));
        ASSERT_EQ(PKI_SUCCESS, GenerateCertificateByCSR(&csrDer, &certDer));
        ASSERT_EQ(PKI_SUCCESS, CheckCertificate(certDer, pubKeyIss));
        certDer.data[0]++;
        ASSERT_NE(PKI_SUCCESS, CheckCertificate(certDer, pubKeyIss));
        certDer.data[0]--;
        ASSERT_EQ(PKI_SUCCESS, CheckCertificate(certDer, pubKeyIss));
    }
    ASSERT_EQ(PKI_SUCCESS, CloseCKMInfo());
}

//test public key structure parsing
TEST(X509Certificate, testParsePublicKey)
{
    ASSERT_EQ(PKI_SUCCESS, ParsePublicKey((ByteArray*)&PKITest::g_caPublicKey));

    size_t length = 3;
    uint8_t shortAr[length];
    ByteArray shortArray = {shortAr, length};
    ASSERT_EQ(PKI_WRONG_ARRAY_LEN, ParsePublicKey(&shortArray));

    uint8_t uncompressed[PUBLIC_KEY_SIZE + 2];
    uncompressed[0] = 0;
    uncompressed[1] = ASN1_UNCOMPRESSED_KEY;
    memcpy(&uncompressed[2], PKITest::g_caPublicKey.data, PUBLIC_KEY_SIZE);
    ByteArray uncomprArr = {uncompressed, PUBLIC_KEY_SIZE+2};
    ParsePublicKey(&uncomprArr);
    ASSERT_EQ((size_t)PUBLIC_KEY_SIZE, uncomprArr.len);
    ASSERT_EQ(0, memcmp(uncomprArr.data, PKITest::g_caPublicKey.data, PUBLIC_KEY_SIZE));
}

//test checking of certificate generated by OpenSSL
TEST(OpenSSLCompatibility, verifyOpenSslCertSign)
{
    struct stat st;
    char *fpath = NULL;
    uint8_t crtData[ISSUER_MAX_CERT_SIZE] = {0};
    uint8_t pubKeyData[PUBLIC_KEY_SIZE] = {0};
    ByteArray crtDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKey = BYTE_ARRAY_INITIALIZER;
    CertificateX509 certificate;

    crtDer.data = crtData;
    crtDer.len = sizeof(crtData);
    pubKey.data = pubKeyData;
    pubKey.len = sizeof(pubKeyData);

    //open file
    fpath = resolve_file_path("/01.der");
    int fileCert = open(fpath, O_RDONLY);
    OICFree(fpath);

    ASSERT_TRUE(fileCert != -1);
    //get status
    ASSERT_TRUE(fstat(fileCert, &st) == 0);
    //read the content
    int res = read(fileCert, crtDer.data, crtDer.len);
    ASSERT_NE(-1, res);
    EXPECT_EQ(st.st_size, res);
    close(fileCert);

     //open file
    fpath = resolve_file_path("/capub.der");
    int fileKey = open(fpath, O_RDONLY);
    OICFree(fpath);
    ASSERT_TRUE(fileKey != -1);
    //get status
    ASSERT_TRUE(fstat(fileKey, &st) == 0);

    if(st.st_size > PUBLIC_KEY_SIZE)
    {
        ASSERT_NE(-1, lseek(fileKey, (st.st_size - PUBLIC_KEY_SIZE), SEEK_SET));
    }

    //read the content
    res = read(fileKey, pubKey.data, pubKey.len);
    ASSERT_NE(-1, res);
    EXPECT_EQ(PUBLIC_KEY_SIZE, res);
    close(fileKey);

    EXPECT_EQ(PKI_SUCCESS, DecodeCertificate(crtDer, &certificate));
    EXPECT_EQ(PKI_SUCCESS, CheckCertificate(crtDer, pubKey));
}

//test parsing of certificate chain generated by OpenSSL
TEST(CertificateChain, LoadCertificateChain)
{
    char* fpath = NULL;
    ByteArray crtChainDer[MAX_CHAIN_LEN] = {{0,0},};
    CertificateX509 crtChain[MAX_CHAIN_LEN] = {{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}},};
    ByteArray msg = BYTE_ARRAY_INITIALIZER;
    uint8_t chainLength;

    fpath = resolve_file_path("/cert_chain.dat");
    FILE *file = fopen(fpath, "rb");
    OICFree(fpath);
    ASSERT_TRUE(file  != NULL);

    while (!feof (file))
    {
        msg.data = (uint8_t *) realloc (msg.data, msg.len + 1);
        msg.data[msg.len] = fgetc (file);
        msg.len++;
    }
    msg.len--;
    fclose (file);
    INC_BYTE_ARRAY(msg, 3);
    EXPECT_EQ(PKI_SUCCESS, LoadCertificateChain (msg, crtChainDer, &chainLength));
#ifdef X509_DEBUG
    printf("chain len: %d\n", chainLength);
#endif
    EXPECT_EQ(PKI_UNKNOWN_OID, ParseCertificateChain (crtChainDer, crtChain, chainLength));

    free(msg.data - 3);
}

//test checking CA certificate generated by OpenSSL
TEST(OpenSSLCompatibility, testOpenSSLCertificate)
{
    char* fpath = NULL;
    struct stat st;
    ByteArray crtDer = BYTE_ARRAY_INITIALIZER;
    uint8_t crtData[ISSUER_MAX_CERT_SIZE] = {0};

    crtDer.data = crtData;
    crtDer.len = sizeof(crtData);

    //open file
    fpath = resolve_file_path("/cacert.der");
    int fd = open(fpath, O_RDONLY);
    OICFree(fpath);
    ASSERT_TRUE(fd != -1);
    //get status
    ASSERT_TRUE(fstat(fd, &st) == 0);

    //read the content
    int res = read(fd, crtDer.data, crtDer.len);
    ASSERT_NE(-1, res);
    ASSERT_EQ(st.st_size, res);
    close(fd);

    crtDer.len = st.st_size;

    #ifdef X509_DEBUG
    printf("Length of cert: %lu\n", crtDer.len);
    #endif
    EXPECT_EQ(PKI_SUCCESS, DecodeCertificate(crtDer, &PKITest::g_certificate));
}

//test signatures checking of certificate chain generated by OpenSSL
TEST(OpenSSLCompatibility, ParseAndCheckCertificateChain)
{
    struct stat st;
    ByteArray crtChainDer[MAX_CHAIN_LEN] = {{0,0},};
    CertificateX509 crtChain[MAX_CHAIN_LEN] = {{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}},};
    ByteArray msg = BYTE_ARRAY_INITIALIZER;
    uint8_t chainLength;

    char* chainPath = resolve_file_path("/chain.der");
    FILE *fileChain = fopen(chainPath, "rb");
    OICFree(chainPath);
    ASSERT_TRUE(fileChain != NULL);

    //get the length
    ASSERT_NE(-1, fseek(fileChain, 0, SEEK_END));
    int len = ftell(fileChain);
    ASSERT_NE(-1, len);
    msg.len = (size_t) len;
    ASSERT_NE(-1, fseek(fileChain, 0, SEEK_SET));
    //allocate memory
    msg.data = (uint8_t*)malloc(msg.len+1);
    //read the content
    ASSERT_EQ(READ_WRITE_BLOCK_N, fread(msg.data, msg.len, READ_WRITE_BLOCK_N, fileChain));

    fclose (fileChain);

    INC_BYTE_ARRAY(msg, 3);
    EXPECT_EQ(PKI_SUCCESS, LoadCertificateChain(msg, crtChainDer, &chainLength));
    EXPECT_EQ(3, chainLength);
    #ifdef X509_DEBUG
    printf("Length of the chain: %d\n", chainLength);
    #endif

    EXPECT_EQ(PKI_SUCCESS, ParseCertificateChain(crtChainDer, crtChain, chainLength));

    ByteArray caPubKey = BYTE_ARRAY_INITIALIZER;
    uint8_t pubKeyData[PUBLIC_KEY_SIZE] = {0};

    caPubKey.data = pubKeyData;
    caPubKey.len = sizeof(pubKeyData);

     //open file
    char* fpath = resolve_file_path("/capub.der");
    int fileKey = open(fpath, O_RDONLY);
    OICFree(fpath);
    ASSERT_TRUE(fileKey != -1);
    //get status
    ASSERT_TRUE(fstat(fileKey, &st) == 0);

    if(st.st_size > PUBLIC_KEY_SIZE)
    {
        ASSERT_NE(-1, lseek(fileKey, (st.st_size - PUBLIC_KEY_SIZE), SEEK_SET));
    }

    //read the content
    int res = read(fileKey, caPubKey.data, caPubKey.len);
    ASSERT_NE(-1, res);
    EXPECT_EQ(PUBLIC_KEY_SIZE, res);
    close(fileKey);

    EXPECT_EQ(PKI_SUCCESS, CheckCertificateChain(crtChain, chainLength, caPubKey));

    free(msg.data - 3);
}

//testing correctness of decoding certificate length from ASN.1 structure
TEST(CRL, testDecodeLength)
{
    ByteArray cert = BYTE_ARRAY_INITIALIZER;
    size_t length(0);
    EXPECT_EQ(PKI_NULL_PASSED, DecodeLength(&cert, &length));

    //a simple DER
    size_t derLength = (size_t)rand() % LEN_LONG;
    cert.len = derLength + 2;
    uint8_t *certData = (uint8_t*)malloc(cert.len);
    cert.data = certData;
    cert.data[0] = (uint8_t)0x30; //mixed types
    cert.data[1] = (uint8_t)(derLength & 0xff);
    EXPECT_EQ(PKI_SUCCESS, DecodeLength(&cert, &length));
    EXPECT_EQ(derLength, length);
    free(certData);
}

//testing serial number storage
TEST(CRL, StoreSerialNumber)
{
    uint8_t data[10] = {0x01, 0x82, 0x01, 0xd1, 0x30, 0x82, 0x01, 0x77, 0xa0, 0x03};
    const ByteArray sn = { data, sizeof(data) / sizeof(uint8_t)};
    int i;
    for (i = 0; i < 400; i++)
    {
        sn.data[0] = i % 20;
        ASSERT_EQ(PKI_SUCCESS, StoreSerialNumber(sn));
    }
    ASSERT_EQ(PKI_CERT_REVOKED, CheckSerialNumber(sn));

    sn.data[1] = 0x01;
    ASSERT_EQ(PKI_SUCCESS, CheckSerialNumber(sn));

    FreeSNStore();
}
#ifdef ARDUINO_MEMORY_DEBUG
//testing memory allocation fault handling at Arduino
TEST(SNStore, MemoryOverflow)
{
    uint8_t data[10] = {0x01, 0x82, 0x01, 0xd1, 0x30, 0x82, 0x01, 0x77, 0xa0, 0x03};
    const ByteArray sn = { data, sizeof(data) / sizeof(uint8_t)};
    int i;
    PKIError res;
    do
    {
        res  = StoreSerialNumber(sn);
    }
    while (res == PKI_SUCCESS);
    ASSERT_EQ(PKI_MEMORY_ALLOC_FAILED, res);

    FreeSNStore();
}
#endif /* ARDUINO_MEMORY_DEBUG */

//testing next certificate serial number handling by "CKM info" unit
TEST_F(PKITest, CAInitAndSerialNum)
{
    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());
    long serialNum = (long)((long)rand() % (MAX_LEN - 1) + 1);
    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());
    //all the serials should start from
    ASSERT_EQ(PKI_SUCCESS, SetSerialNumber(serialNum));
    long nextSerial;
    ASSERT_EQ(PKI_SUCCESS, GetNextSerialNumber(&nextSerial));
    ASSERT_EQ(nextSerial, serialNum);
    ASSERT_EQ(PKI_SUCCESS, CloseCKMInfo());
}

//testing CA name handling by "CKM info" unit
TEST_F(PKITest, testCAName)
{
    ByteArray caName = BYTE_ARRAY_INITIALIZER;
    caName.len = (size_t)((size_t)rand() % (ISSUER_MAX_NAME_SIZE - 1) + 1);
    caName.data = (uint8_t*)malloc(caName.len);
    size_t i;
    for(i = 0; i < caName.len; i++){
            caName.data[i] = (uint8_t)(rand() % 128);
    }
    EXPECT_EQ(PKI_SUCCESS, InitCKMInfo());
    EXPECT_EQ(PKI_SUCCESS, SetRootName(caName));
    ByteArray getName = BYTE_ARRAY_INITIALIZER;
    uint8_t uint8CAName[ISSUER_MAX_NAME_SIZE] = {0};
    getName.data     = uint8CAName;
    getName.len      = ISSUER_MAX_NAME_SIZE;
    EXPECT_EQ(PKI_SUCCESS, GetCAName(&getName));
    EXPECT_EQ(0, memcmp(caName.data, getName.data, caName.len));
    free(caName.data);
    ASSERT_EQ(PKI_SUCCESS, CloseCKMInfo());
}

//testing key pair generation and storing by "CKM info" unit
TEST_F(PKITest, testKeyPair)
{
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;
    rootName.data = (uint8_t *)"ROOT";
    rootName.len = strlen((char *)rootName.data);
    SetRootName(rootName);

    //first test the GenerateCAKeyPair - this writes to the CA storage
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    privKeyIss.len = PRIVATE_KEY_SIZE;
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] = {0};
    privKeyIss.data = caPrivKey;

    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    pubKeyIss.len = PUBLIC_KEY_SIZE;
    uint8_t caPubKey[PUBLIC_KEY_SIZE] = {0};
    pubKeyIss.data = caPubKey;

    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());
    ASSERT_EQ(PKI_SUCCESS, GenerateCAKeyPair(&privKeyIss, &pubKeyIss));

    ByteArray keyCheck = BYTE_ARRAY_INITIALIZER;
    keyCheck.len = PUBLIC_KEY_SIZE;
    uint8_t keyCheckData[PUBLIC_KEY_SIZE] = {0};
    keyCheck.data = keyCheckData;
    ASSERT_EQ(PKI_SUCCESS, GetCAPrivateKey(&keyCheck));
    ASSERT_EQ(0, memcmp(keyCheck.data, privKeyIss.data, PRIVATE_KEY_SIZE));

    ASSERT_EQ(PKI_SUCCESS, GetCAPublicKey(&keyCheck));
    ASSERT_EQ(0, memcmp(keyCheck.data, pubKeyIss.data, PUBLIC_KEY_SIZE));

    //now test the GenerateKeyPair - does not write to the CA storage
    ASSERT_EQ(PKI_SUCCESS, GenerateKeyPair(&privKeyIss, &pubKeyIss));

    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());
    ASSERT_EQ(PKI_SUCCESS, GetCAPrivateKey(&keyCheck));
    ASSERT_NE(0, memcmp(keyCheck.data, privKeyIss.data, PRIVATE_KEY_SIZE));

    ASSERT_EQ(PKI_SUCCESS, GetCAPublicKey(&keyCheck));
    ASSERT_NE(0, memcmp(keyCheck.data, pubKeyIss.data, PUBLIC_KEY_SIZE));
    ASSERT_EQ(PKI_SUCCESS, CloseCKMInfo());
}

//testing CRL encoding
TEST_F(PKITest, testEncodeCRL)
{
    CertificateList crl;

    uint8_t *uint8ThisUpdateTime = (uint8_t *)"130101000000Z";
    uint32_t numberOfRevoked = 0;
    uint32_t revokedNumbers[2];
    const uint8_t *revocationDates[2];

    ByteArray code = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss =  BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    uint8_t caPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] = {0};

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = PUBLIC_KEY_SIZE;
    privKeyIss.data = caPrivKey;
    privKeyIss.len = PRIVATE_KEY_SIZE;

    numberOfRevoked = 2;

    revokedNumbers[0] = 100; // serial number of first revoked certificate
    revokedNumbers[1] = 200; // serial number of second revoked certificate
    revocationDates[0] = (const uint8_t *)"130101000001Z";
    revocationDates[1] = (const uint8_t *)"130101000002Z";

    rootName.data = (uint8_t *)"ROOT2";
    rootName.len = strlen((char *)rootName.data);
    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());
    ASSERT_EQ(PKI_SUCCESS, SetRootName(rootName));
    ASSERT_EQ(PKI_SUCCESS, GenerateCAKeyPair(&privKeyIss, &pubKeyIss));

    code.data = (uint8_t *)calloc(1,
                (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4)));
    code.len = (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4));

    EXPECT_EQ(PKI_SUCCESS,CKMIssueCRL(uint8ThisUpdateTime, numberOfRevoked, revokedNumbers,
                                      revocationDates,&code));
    EXPECT_EQ(PKI_SUCCESS, DecodeCertificateList (code, &crl, pubKeyIss));
#ifdef X509_DEBUG
    PrintSNStore();
    PrintCRL(&crl);
#endif

    FreeSNStore();
    free(code.data);
    ASSERT_EQ(PKI_SUCCESS, CloseCKMInfo());
}

//check correctness of certificate revocation by CKMIssueCRL() and CKMRevocateCertificate()
TEST_F(PKITest, testRevocateCertificate)
{
    CertificateList crl;

    uint8_t *uint8ThisUpdateTime = (uint8_t *)"130101000000Z";
    uint32_t numberOfRevoked = 0;
    uint32_t revokedNumbers[2];
    const uint8_t *revocationDates[2];

    ByteArray code = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss =  BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    uint8_t caPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] = {0};

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = sizeof(caPubKey);
    privKeyIss.data = caPrivKey;
    privKeyIss.len = sizeof(caPrivKey);

    numberOfRevoked = 2;

    revokedNumbers[0] = 100; // serial number of first revoked certificate
    revokedNumbers[1] = 200; // serial number of second revoked certificate
    revocationDates[0] = (const uint8_t *)"130101000001Z";
    revocationDates[1] = (const uint8_t *)"130101000002Z";

    rootName.data = (uint8_t *)"ROOT2";
    rootName.len = strlen((char *)rootName.data);
    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());
    ASSERT_EQ(PKI_SUCCESS, SetRootName(rootName));
    ASSERT_EQ(PKI_SUCCESS, GenerateCAKeyPair(&privKeyIss, &pubKeyIss));

    code.len = CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4);
    code.data = (uint8_t *)calloc(1, code.len);

    EXPECT_EQ(PKI_SUCCESS, CKMIssueCRL (uint8ThisUpdateTime, numberOfRevoked, revokedNumbers,
                                        revocationDates, &code));
    EXPECT_EQ(PKI_SUCCESS, DecodeCertificateList (code, &crl, pubKeyIss));
    free(code.data);
    numberOfRevoked++;
    code.len = CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4);
    code.data = (uint8_t *)calloc(1, code.len);
    EXPECT_EQ(PKI_SUCCESS, CKMRevocateCertificate (uint8ThisUpdateTime, 50, &code));
    EXPECT_EQ(PKI_SUCCESS, DecodeCertificateList (code, &crl, pubKeyIss));
#ifdef X509_DEBUG
    PrintSNStore();
    PrintCRL(&crl);
#endif

    FreeSNStore();
    free(code.data);
    ASSERT_EQ(PKI_SUCCESS, CloseCKMInfo());
}

//checck correctness of saving root certificate to binary file
TEST_F(PKITest, StoreCKMInfo)
{
    struct stat st;
    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());
    uint8_t derData[ISSUER_MAX_CERT_SIZE] = {0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] = {0};
    const long serNum  = 48598490;
    CertificateList crl;
    uint8_t *uint8ThisUpdateTime = (uint8_t *)"130101000000Z";
    uint32_t numberOfRevoked = 0;
    uint32_t revokedNumbers[2];
    const uint8_t *revocationDates[2];

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;
    ByteArray code = BYTE_ARRAY_INITIALIZER;

    certDer.data = derData;
    certDer.len = ISSUER_MAX_CERT_SIZE;
    pubKeyIss.data = caPubKey;
    pubKeyIss.len = PUBLIC_KEY_SIZE;
    privKeyIss.data = caPrivKey;
    privKeyIss.len = PRIVATE_KEY_SIZE;
    rootName.data = (uint8_t *)"ROOT";
    rootName.len = strlen((char *)rootName.data);

    //generate CA Certificate
    ASSERT_EQ(PKI_SUCCESS, GenerateCAKeyPair(&privKeyIss, &pubKeyIss));
    ASSERT_EQ(PKI_SUCCESS, SetSerialNumber(serNum));
    ASSERT_EQ(PKI_SUCCESS, SetRootName(rootName));
    ASSERT_EQ(PKI_SUCCESS, CKMIssueRootCertificate(0, 0, &certDer));

    //generate CRL
    numberOfRevoked = NUMBER_OF_REVOKED;

    revokedNumbers[0] = 100; // serial number of first revoked certificate
    revokedNumbers[1] = 200; // serial number of second revoked certificate
    revocationDates[0] = (const uint8_t *)"130101000001Z";
    revocationDates[1] = (const uint8_t *)"130101000002Z";

    code.data = (uint8_t *)calloc(1,
                (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4)));
    code.len = (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4));

    ASSERT_EQ(PKI_SUCCESS, CKMIssueCRL (uint8ThisUpdateTime, numberOfRevoked, revokedNumbers,
                                        revocationDates, &code));

    // Check Certificate file
    CertificateX509 certificate;
    ByteArray crtDer = BYTE_ARRAY_INITIALIZER;
    uint8_t crtDerData[ISSUER_MAX_CERT_SIZE] = {0};

    crtDer.data = crtDerData;
    crtDer.len = sizeof(crtDerData);

    //open file
    int fd = open(CA_STORAGE_CRT_FILE, O_RDONLY);
    ASSERT_TRUE(fd != -1);
    //get status
    ASSERT_TRUE(fstat(fd, &st) == 0);
    //read the content
    int res = read(fd, crtDer.data, crtDer.len);
    ASSERT_NE(-1, res);
    EXPECT_EQ(st.st_size > ISSUER_MAX_CERT_SIZE ? ISSUER_MAX_CERT_SIZE : st.st_size, res);
    close(fd);

    ByteArray crtCheck;
    crtCheck.data = crtDer.data + 3;    //now file contains length of certificate
    crtCheck.len = crtDer.len - 3;
    EXPECT_EQ(PKI_SUCCESS, DecodeCertificate(crtCheck, &certificate));
#ifdef X509_DEBUG
    PrintCertificate(&certificate);
#endif

    //check CRL
    ByteArray crlDer = BYTE_ARRAY_INITIALIZER;
    crlDer.len = (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4));
    crlDer.data = (uint8_t *)malloc(crlDer.len);

    EXPECT_EQ(PKI_SUCCESS, GetCertificateRevocationList(&crlDer));

    EXPECT_EQ(PKI_SUCCESS, DecodeCertificateList (crlDer, &crl, pubKeyIss));
#ifdef X509_DEBUG
       PrintCRL(&crl);
#endif
    EXPECT_EQ(PKI_SUCCESS, CloseCKMInfo());
    free(crlDer.data);
    free(code.data);
}

//check correctness of root certificate generation
TEST_F(PKITest, GenerateRootCertificate)
{
    uint8_t derData[ISSUER_MAX_CERT_SIZE] = {0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] = {0};

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    certDer.data = derData;
    certDer.len = sizeof(derData);

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = sizeof(caPubKey);
    privKeyIss.data = caPrivKey;
    privKeyIss.len = sizeof(caPrivKey);
    rootName.data = (uint8_t *)"ROOT";
    rootName.len = strlen((char *)rootName.data);
    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());

    for (int i = 1; i <= RUNS; i++)
    {
        ASSERT_EQ(PKI_SUCCESS, GenerateCAKeyPair(&privKeyIss, &pubKeyIss));
        ASSERT_EQ(PKI_SUCCESS, SetSerialNumber(i));
        ASSERT_EQ(PKI_SUCCESS, SetRootName(rootName));
        ASSERT_EQ(PKI_SUCCESS, CKMIssueRootCertificate(0, 0, &certDer));

        ASSERT_EQ(PKI_SUCCESS, CheckCertificate(certDer, pubKeyIss));
        certDer.data[0]++;
        ASSERT_NE(PKI_SUCCESS, CheckCertificate(certDer, pubKeyIss));
        certDer.data[0]--;
        ASSERT_EQ(PKI_SUCCESS, CheckCertificate(certDer, pubKeyIss));
    }
    ASSERT_EQ(PKI_SUCCESS, CloseCKMInfo());
}

//check correctness of ordinal device certificate generation
TEST_F(PKITest, GenerateDeviceCertificate)
{
    uint8_t derData[ISSUER_MAX_CERT_SIZE] = {0};
    uint8_t subjPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t subjPrivKey[PRIVATE_KEY_SIZE] = {0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] = {0};
    uint8_t *subjName = (uint8_t *)"Subject Name";

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeySubj = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeySubj = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    certDer.data = derData;
    certDer.len = ISSUER_MAX_CERT_SIZE;

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = sizeof(caPubKey);
    privKeyIss.data = caPrivKey;
    privKeyIss.len = sizeof(caPrivKey);
    pubKeySubj.data = subjPubKey;
    pubKeySubj.len = sizeof(subjPubKey);
    privKeySubj.data = subjPrivKey;
    privKeySubj.len = sizeof(subjPrivKey);
    rootName.data = (uint8_t *)"ROOT2";
    rootName.len = strlen((char *)rootName.data);
    ASSERT_EQ(PKI_SUCCESS, InitCKMInfo());

    ASSERT_EQ(GenerateCAKeyPair(&privKeyIss, &pubKeyIss), PKI_SUCCESS);
    for (int i = 1; i <= RUNS; i++)
    {
        ASSERT_EQ(PKI_SUCCESS, GenerateKeyPair(&privKeySubj, &pubKeySubj));
        ASSERT_EQ(PKI_SUCCESS, SetSerialNumber(i));
        ASSERT_EQ(PKI_SUCCESS, SetRootName(rootName));
        ASSERT_EQ(PKI_SUCCESS, CKMIssueDeviceCertificate(subjName, 0, 0, subjPubKey, &certDer));

        ASSERT_EQ(PKI_SUCCESS, CheckCertificate(certDer, pubKeyIss));
        certDer.data[0]++;
        ASSERT_NE(PKI_SUCCESS, CheckCertificate(certDer, pubKeyIss));
        certDer.data[0]--;
        ASSERT_EQ(PKI_SUCCESS, CheckCertificate(certDer, pubKeyIss));
    }
    ASSERT_EQ(CloseCKMInfo(), PKI_SUCCESS);
}

//check correctness of saving CRL to storage and loading CRL from storage
TEST_F(PKITest, CRLSetGet)
{
    OicSecCrl_t *defaultCrl = NULL;
    defaultCrl = (OicSecCrl_t *)OICCalloc(1, sizeof(OicSecCrl_t));
    ASSERT_NE(defaultCrl, (OicSecCrl_t *) NULL);
    defaultCrl->CrlId = CRL_DEFAULT_CRL_ID;
    defaultCrl->CrlData.data = (uint8_t *)CRL_DEFAULT_CRL_DATA;
    defaultCrl->CrlData.len = strlen(CRL_DEFAULT_CRL_DATA);
    defaultCrl->ThisUpdate.data = (uint8_t *)CRL_DEFAULT_THIS_UPDATE;
    defaultCrl->ThisUpdate.len = strlen(CRL_DEFAULT_THIS_UPDATE);
    EXPECT_EQ(OC_STACK_OK, UpdateCRLResource(defaultCrl));

    EXPECT_NE((void *)NULL, GetCrl());
    OICFree(defaultCrl);
}

//CKMGetCRL test
TEST_F(PKITest, CKMGetCRL)
{
    const size_t CRL_MAX_SIZE = 1024;
    ByteArray certificateRevocationList;
    certificateRevocationList.data = (uint8_t*)OICCalloc(1,CRL_MAX_SIZE);
    certificateRevocationList.len = CRL_MAX_SIZE;
    EXPECT_EQ(PKI_SUCCESS, CKMGetCRL(&certificateRevocationList));
    OICFree(certificateRevocationList.data);
}

//CKMSetCAInfo test
TEST_F(PKITest, CKMSetCAInfo)
{
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;
    rootName.data = (uint8_t *)"ROOT2";
    rootName.len = strlen((char *)rootName.data);
    const long serNum = 100;
    EXPECT_EQ(PKI_SUCCESS, CKMSetCAInfo(serNum, rootName));
}

int main(int argc, char **argv)
{
    int urandom;
    unsigned int seed = 0;

    urandom = open ("/dev/urandom", O_RDONLY);
    if(urandom)
    {
        fprintf(stderr, "Cannot open /dev/urandom\n");
    }
    else
    {
        if(read(urandom, &seed, sizeof(seed)) != sizeof(seed))
        {
            fprintf(stderr, "Error read from /dev/urandom\n");
        }
        close(urandom);
    }

    srand(seed);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
