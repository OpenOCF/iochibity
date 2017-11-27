licenses(["notice"])

cc_library(
    name = "mbedtls_hdrs",
    hdrs = glob(["include/mbedtls/*.h"]),
)

cc_library(
    name = "mbedtls",
    # alwayslink=True,
    # linkstatic = 1,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Ithird_party/mbedtls",
             "-Ithird_party/mbedtls/include",
             "-Ithird_party/mbedtls/include/mbedtls",
	     # expose getaddrinfo etc. on linux (ubuntu)
	     "-DWITH_POSIX",
	     "-D_GNU_SOURCE"],
    deps = ["//src:config",
            "mbedtls-crypto",
            "mbedtls-x509"],
    srcs = ["include/mbedtls/config.h",
            "include/mbedtls/config_mbedtls.h",
            "include/mbedtls/check_config.h",

            "library/debug.c",
            "include/mbedtls/debug.h",

            "library/net_sockets.c",
            "include/mbedtls/net_sockets.h",

            "library/ssl_cache.c",
            "include/mbedtls/ssl_cache.h",

            "library/ssl_ciphersuites.c",
            "include/mbedtls/ssl_ciphersuites.h",

            "library/ssl_cli.c",

            "library/ssl_cookie.c",
            "include/mbedtls/ssl_cookie.h",

            "library/ssl_srv.c",
            "include/mbedtls/ssl_internal.h",

            "library/ssl_ticket.c",
            "include/mbedtls/ssl_ticket.h",

            "library/ssl_tls.c"],
    #    hdrs = ["src/openssl.h"],
    visibility = ["//visibility:public"])

cc_library(
    name = "mbedtls-crypto",
    alwayslink=True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Ithird_party/mbedtls",
             "-Ithird_party/mbedtls/include",
             "-Ithird_party/mbedtls/include/mbedtls"],
    deps = ["//src:config",
            ":mbedtls_hdrs"],
    strip_include_prefix = "include",
    hdrs = glob(["include/mbedtls/*.h"]),
    srcs = [#"include/mbedtls/config.h",
            "include/mbedtls/config_mbedtls.h",
            "include/mbedtls/check_config.h",

            "library/aes.c",
            "include/mbedtls/aes.h",

            "library/aesni.c",
            "include/mbedtls/aesni.h",

            # "library/arc4.c",
            # "include/mbedtls/arc4.h",

            "library/asn1parse.c",
            "include/mbedtls/asn1.h",

            "library/asn1write.c",
            "include/mbedtls/asn1write.h",

            "library/base64.c",
            "include/mbedtls/base64.h",

            "library/bignum.c",
            "include/mbedtls/bignum.h",
            "include/mbedtls/bn_mul.h",

            # "library/blowfish.c",
            # "include/mbedtls/blowfish.h",

            # "library/camellia.c",
            # "include/mbedtls/camellia.h",

            "library/ccm.c",
            "include/mbedtls/ccm.h",

            "library/cipher.c",
            "library/cipher_wrap.c",
            "include/mbedtls/cipher.h",
            "include/mbedtls/cipher_internal.h",

            # "library/des.c",
            # "include/mbedtls/des.h",

            # "library/dhm.c",
            # "include/mbedtls/dhm.h",

            "library/ecdh.c",
            "include/mbedtls/ecdh.h",

            "library/ecdsa.c",
            "include/mbedtls/ecdsa.h",

            # "library/ecjpake.c",
            # "include/mbedtls/ecjpake.h",

            "library/ecp.c",
            "include/mbedtls/ecp.h",

            "library/ecp_curves.c",

            "library/entropy.c",
            "include/mbedtls/entropy.h",

            "library/entropy_poll.c",
            "include/mbedtls/entropy_poll.h",

            "library/error.c",
            "include/mbedtls/error.h",
            "include/mbedtls/ctr_drbg.h",
            "library/ctr_drbg.c",
            "include/mbedtls/net_sockets.h",
            "include/mbedtls/ssl.h",
            "include/mbedtls/ssl_ciphersuites.h",
            "include/mbedtls/x509_crt.h",
            "include/mbedtls/x509_crl.h",

            "library/gcm.c",
            "include/mbedtls/gcm.h",

            # "library/havege.c",
            # "include/mbedtls/havege.h",

            "library/hmac_drbg.c",
            "include/mbedtls/hmac_drbg.h",

            "library/md.c",
            "include/mbedtls/md.h",

            # "library/md2.c",
            # "include/mbedtls/md2.h",

            # "library/md4.c",
            # "include/mbedtls/md4.h",

            # "library/md5.c",
            # "include/mbedtls/md5.h",
            # "include/mbedtls/md_internal.h",

            "library/md_wrap.c",

            # "library/memory_buffer_alloc.c",
            # "include/mbedtls/memory_buffer_alloc.h",

            "library/oid.c",
            "include/mbedtls/oid.h",
            "include/mbedtls/x509.h",

            # "library/padlock.c",
            # "include/mbedtls/padlock.h",

            "library/pem.c",
            "include/mbedtls/pem.h",

            "library/pk.c",
            "include/mbedtls/pk.h",
            "include/mbedtls/pk_internal.h",

            "library/pk_wrap.c",

            # "library/pkcs12.c",
            # "include/mbedtls/pkcs12.h",

            "library/pkcs5.c",
            "include/mbedtls/pkcs5.h",

            "library/pkparse.c",

            "library/pkwrite.c",

            "library/platform.c",
            "include/mbedtls/platform.h",
            "include/mbedtls/platform_time.h",

            # "library/ripemd160.c",
            # "include/mbedtls/ripemd160.h",

            "library/rsa.c",
            "include/mbedtls/rsa.h",

            "library/sha1.c",
            "include/mbedtls/sha1.h",

            "library/sha256.c",
            "include/mbedtls/sha256.h",

            "library/sha512.c",
            "include/mbedtls/sha512.h",

            # "library/threading.c",
            # "include/mbedtls/threading.h",

            "library/timing.c",
            "include/mbedtls/timing.h",

            "library/version.c",
            "include/mbedtls/version.h",

            "library/version_features.c",

            # "library/xtea.c",
            # "include/mbedtls/xtea.h"
    ],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "mbedtls-x509",
    alwayslink=True,
    copts = ["-Isrc",
             "-Iexternal/openocf/src",
             "-Ithird_party/mbedtls/include",
             "-Ithird_party/mbedtls/include/mbedtls"],
    deps = ["//src:config",
            ":mbedtls_hdrs"],
    strip_include_prefix = "include",
    hdrs = glob(["include/mbedtls/*.h"]),
    srcs = [# "include/mbedtls/config.h",

            # "library/certs.c",
            # "include/mbedtls/certs.h",

            # "library/pkcs11.c",
            # "include/mbedtls/pkcs11.h",

            "library/x509.c",
            "include/mbedtls/x509.h",
            "include/mbedtls/pk.h",
            "include/mbedtls/md.h",
            "include/mbedtls/rsa.h",
            "include/mbedtls/ecp.h",
            "include/mbedtls/ecdsa.h",
            "include/mbedtls/asn1.h",
            "include/mbedtls/bignum.h",

            "library/x509_create.c",

            "library/x509_crl.c",
            "include/mbedtls/x509_crl.h",
            "include/mbedtls/oid.h",
            "include/mbedtls/cipher.h",
            "include/mbedtls/platform.h",
            "include/mbedtls/platform_time.h",

            "library/x509_crt.c",
            "include/mbedtls/x509_crt.h",
            "include/mbedtls/asn1write.h",
            "include/mbedtls/sha1.h",

            "library/x509_csr.c",
            "include/mbedtls/x509_csr.h",
            "include/mbedtls/pem.h",

            "library/x509write_crt.c",
            "library/x509write_csr.c"],
    visibility = ["//visibility:public"]
)
