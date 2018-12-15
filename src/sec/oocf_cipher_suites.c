#include "oocf_cipher_suites.h"

//#if INTERFACE
#include "mbedtls/platform.h"
//#include "mbedtls/ssl.h"
//#include "mbedtls/entropy.h"
//#include "mbedtls/ctr_drbg.h"
#include "mbedtls/pkcs12.h"
#include "mbedtls/ssl_internal.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/oid.h"
#include "mbedtls/x509.h"
//#include "mbedtls/x509_crt.h"
#include "mbedtls/error.h"
#ifdef __WITH_DTLS__
#include "mbedtls/timing.h"
//#include "mbedtls/ssl_cookie.h"
#endif
//#endif

//#include "mbedtls/ssl.h"


#if INTERFACE
typedef enum SSL_CIPHER
{
    SSL_RSA_WITH_AES_256_CBC_SHA256,
    SSL_RSA_WITH_AES_128_GCM_SHA256,
    SSL_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
    SSL_ECDHE_ECDSA_WITH_AES_128_CCM_8,
    SSL_ECDHE_ECDSA_WITH_AES_128_CCM,
    SSL_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
    SSL_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
    SSL_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
    SSL_ECDHE_PSK_WITH_AES_128_CBC_SHA256,
    SSL_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
    SSL_ECDH_ANON_WITH_AES_128_CBC_SHA256,
    SSL_CIPHER_MAX
} SslCipher_t;
#endif

const int tlsCipher[(SslCipher_t)SSL_CIPHER_MAX][2] =
{
    {MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256, 0},
    {MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256, 0},
    {MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, 0},
    {MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8, 0},
    {MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM, 0},
    {MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, 0},
    {MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384, 0},
    {MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, 0},
    {MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256, 0},
    {MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256, 0},
    {MBEDTLS_TLS_ECDH_ANON_WITH_AES_128_CBC_SHA256, 0}
};



#if INTERFACE
#include <stdint.h>
#endif
OCStackResult OC_CALL OCSelectCipherSuite(uint16_t cipher, OCTransportAdapter adapterType)
{
    // OCTransportAdapter and CATransportAdapter_t are using the same bits for each transport.
    OC_STATIC_ASSERT((unsigned int)OC_ADAPTER_IP == (unsigned int)CA_ADAPTER_IP,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_ADAPTER_GATT_BTLE == (unsigned int)CA_ADAPTER_GATT_BTLE,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_ADAPTER_RFCOMM_BTEDR == (unsigned int)CA_ADAPTER_RFCOMM_BTEDR,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_ADAPTER_TCP == (unsigned int)CA_ADAPTER_TCP,
        "OC/CA bit mismatch");
    OC_STATIC_ASSERT((unsigned int)OC_ADAPTER_NFC == (unsigned int)CA_ADAPTER_NFC,
        "OC/CA bit mismatch");

#ifdef RA_ADAPTER
    OC_STATIC_ASSERT(
        (unsigned int)OC_ADAPTER_REMOTE_ACCESS
            == (unsigned int)CA_ADAPTER_REMOTE_ACCESS, "OC/CA bit mismatch");

    #define ALL_OC_ADAPTER_TYPES (OC_ADAPTER_IP | OC_ADAPTER_GATT_BTLE | OC_ADAPTER_RFCOMM_BTEDR |\
                                  OC_ADAPTER_TCP | OC_ADAPTER_NFC | OC_ADAPTER_REMOTE_ACCESS)
#else
    #define ALL_OC_ADAPTER_TYPES (OC_ADAPTER_IP | OC_ADAPTER_GATT_BTLE | OC_ADAPTER_RFCOMM_BTEDR |\
                                  OC_ADAPTER_TCP | OC_ADAPTER_NFC)
#endif

    assert((adapterType & ~ALL_OC_ADAPTER_TYPES) == 0);

    return CAResultToOCResult(CASelectCipherSuite(cipher, (CATransportAdapter_t)adapterType));
}

CAResult_t CAsetTlsCipherSuite(const uint32_t cipher)
{
    OIC_LOG_V(DEBUG, NET_SSL_TAG, "In %s", __func__);
    oc_mutex_lock(g_sslContextMutex);

    if (NULL == g_caSslContext)
    {
        OIC_LOG(ERROR, NET_SSL_TAG, "SSL context is not initialized.");
        oc_mutex_unlock(g_sslContextMutex);
        return CA_STATUS_NOT_INITIALIZED;
    }

    SslCipher_t index = GetCipherIndex(cipher);
    if (SSL_CIPHER_MAX == index)
    {
        OIC_LOG(WARNING, NET_SSL_TAG, "Unknown cipher");
    }
    else
    {
#ifdef __WITH_TLS__
        /* CONF_SSL(&g_caSslContext->clientTlsConf, &g_caSslContext->serverTlsConf, */
        /* mbedtls_ssl_conf_ciphersuites, tlsCipher[index]); */
        mbedtls_ssl_conf_ciphersuites(&g_caSslContext->clientTlsConf, tlsCipher[index]);
        mbedtls_ssl_conf_ciphersuites(&g_caSslContext->serverTlsConf, tlsCipher[index]);
#endif
#ifdef __WITH_DTLS__
        /* CONF_SSL(&g_caSslContext->clientDtlsConf, &g_caSslContext->serverDtlsConf, */
        /* mbedtls_ssl_conf_ciphersuites, tlsCipher[index]); */
        mbedtls_ssl_conf_ciphersuites(&g_caSslContext->clientDtlsConf, tlsCipher[index]);
        mbedtls_ssl_conf_ciphersuites(&g_caSslContext->serverDtlsConf, tlsCipher[index]);
#endif
        OIC_LOG_V(DEBUG, NET_SSL_TAG, "Selected cipher: 0x%x", cipher);
    }
    g_caSslContext->cipher = index;

    oc_mutex_unlock(g_sslContextMutex);
    OIC_LOG_V(DEBUG, NET_SSL_TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}

CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
{
    (void)(adapter); // prevent unused-parameter warning when building release variant
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);
    OIC_LOG_V(DEBUG, TAG, "cipher : %d , CATransportAdapter : %d", cipher, adapter);
    CAResult_t res = CA_STATUS_FAILED;
#if defined (__WITH_DTLS__) || defined(__WITH_TLS__)
    res = CAsetTlsCipherSuite(cipher);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to CAsetTlsCipherSuite : %d", res);
    }
#else
    (void)(cipher); // prevent unused-parameter warning
    OIC_LOG(ERROR, TAG, "Method not supported");
#endif
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return res;
}


CAResult_t CAEnableAnonECDHCipherSuite(const bool enable)
{
    OIC_LOG(DEBUG, TAG, "CAEnableAnonECDHCipherSuite");
    CAResult_t res = CA_STATUS_FAILED;
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
    // TLS_ECDH_ANON_WITH_AES_128_CBC_SHA256    0xFF00 replaces 0xC018
    res = CAsetTlsCipherSuite(enable ? 0xFF00 : 0x00);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to CAsetTlsCipherSuite : %d", res);
    }
#else
    (void)(enable); // prevent unused-parameter compiler warning
    OIC_LOG(ERROR, TAG, "Method not supported");
#endif
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return res;
}


