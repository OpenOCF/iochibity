/* JSON Web Key https://tools.ietf.org/html/draft-ietf-jose-json-web-key-41 */

/**
 * /oic/sec/jwk (JSON Web Key) data type.
 * See JSON Web Key (JWK)  draft-ietf-jose-json-web-key-41
 */
#define JWK_LENGTH 256/8 // 256 bit key length

#if EXPORT_INTERFACE
struct OicSecKey
{
    uint8_t                *data;
    size_t                  len;

    // TODO: This field added as workaround. Will be replaced soon.
    OicEncodingType_t encoding;

};
typedef struct OicSecKey OicSecKey_t;

#endif	/* INTERFACE */

