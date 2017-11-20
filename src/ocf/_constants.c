#if EXPORT_INTERFACE
/**
 * @def UUID_PREFIX
 * @brief uuid prefix in certificate subject field
 */
#define UUID_PREFIX "uuid:"

/**
 * @def SUBJECT_PREFIX
 * @brief prefix for specifying part of a cert's subject for a particular uuid
 */
#define SUBJECT_PREFIX "CN=" UUID_PREFIX

/**
* TODO: Move these COAP defines to CoAP lib once approved.
*/
#define COAP_MEDIATYPE_APPLICATION_VND_OCF_CBOR 10000 // application/vnd.ocf+cbor
#define OCF_ACCEPT_CONTENT_FORMAT_VERSION 2049
#define OCF_CONTENT_FORMAT_VERSION 2053

// The Accept Version and Content-Format Version for OCF 1.0.0 (0b0000 1000 0000 0000).
#define DEFAULT_VERSION_VALUE 2048

#endif	/* INTERFACE */

