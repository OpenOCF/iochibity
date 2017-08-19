/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef PKIX_INTERFACE_H
#define PKIX_INTERFACE_H

#include "cainterface.h"
#include "experimental/byte_array.h" /* FIXME: mv to //src/util? */

/*GAR TMP - from casecurityinterface.h */
/**
 * @enum CADtlsPskCredType_t
 * Type of PSK credential required during DTLS handshake
 * It does not make much sense in bringing in all definitions from dtls.h into here.
 * Therefore, redefining them here.
 */
typedef enum
{
    CA_DTLS_PSK_HINT,
    CA_DTLS_PSK_IDENTITY,
    CA_DTLS_PSK_KEY
} CADtlsPskCredType_t;

/**
 * Binary structure containing PKIX related info
 * own certificate chain, public key, CA's and CRL's
 * The data member of each ByteArray_t must be allocated with OICMalloc or OICCalloc.
 * The SSL adapter takes ownership of this memory and will free it internally after use.
 * Callers should not reference this memory after it has been provided to the SSL adapter via the
 * callback.
 */
typedef struct
{
    ByteArray_t crt;    /**< own certificate chain as a null-terminated PEM string of certificates */
    ByteArray_t key;    /**< own private key as binary-encoded DER */
    ByteArray_t ca;     /**< trusted CAs as a null-terminated PEM string of certificates */
    ByteArray_t crl;    /**< trusted CRLs as binary-encoded DER */
} PkiInfo_t;



#ifdef __cplusplus
extern "C" {
#endif
/**
 * This method is used by mbedTLS/SRM to retrieve PKIX related info
 *
 * @param[out] inf structure with certificate, private key and crl to be filled.
 *
 */
void GetPkixInfo(PkiInfo_t * inf);
/**
 * This method is used by mbedTLS/SRM to retrieve manufacturer PKIX related info
 *
 * @param[out] inf structure with certificate, private key and crl to be filled.
 *
 */
void GetManufacturerPkixInfo(PkiInfo_t * inf);

/**
 * Used by CA to retrieve credential types
 *
 * @param[out] list TLS suites boolean map.
 * @param[in]  device uuid.
 */
void InitCipherSuiteList(bool * list, const char* deviceId);

/**
 * Used by CA to retrieve manufacturer credential types
 *
 * @param[out] list TLS suites boolean map.
 * @param[in]  device uuid.
 */
void InitManufacturerCipherSuiteList(bool * list, const char* deviceId);
#ifdef __cplusplus
}
#endif

#endif //PKIX_INTERFACE_H
