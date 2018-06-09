exports_files(["config.h", "ocf.patch"])

genrule(
    name = "gentest",
    srcs = glob(["**/*"]),
    outs = ["genrule.log"],
    cmd  = "find -L external -name *.data > $@"
)

# copy our custom config.h into the patch output dir
genrule(
    name = "config",
    srcs = ["@//config/mbedtls:config.h"],
    outs = ["patched/include/mbedtls/config.h"],
    cmd  = "cp $(location @//config/mbedtls:config.h) $@"
)

genrule(
    name = "patch",
    srcs = glob(["**/*.c"])
         + glob(["**/*.h"])
         + glob(["**/*.data"])
         + glob(["**/*.function"])
         + glob(["**/*.sh"])
         + ["@//config/mbedtls:ocf.patch"],
    outs = ["patched/" + x for x in glob(["**/library/*.c"])]
         + ["patched/" + x for x in glob(["**/include/**/*.h"],
                                         exclude=["**/config.h"])],
    cmd  = "\n".join([
        "cp -R external/mbedtls/ patched",
        "patch -dpatched -p1 -l -f < $(location @//config/mbedtls:ocf.patch)",
        "cp -R patched $(@D)",
        ])
)

mbedtls_hdrs = ["patched/include/mbedtls/aes.h",
                "patched/include/mbedtls/aesni.h",
                "patched/include/mbedtls/arc4.h",
                "patched/include/mbedtls/asn1.h",
                "patched/include/mbedtls/asn1write.h",
                "patched/include/mbedtls/base64.h",
                "patched/include/mbedtls/bignum.h",
                "patched/include/mbedtls/blowfish.h",
                "patched/include/mbedtls/bn_mul.h",
                "patched/include/mbedtls/camellia.h",
                "patched/include/mbedtls/ccm.h",
                "patched/include/mbedtls/certs.h",
                "patched/include/mbedtls/check_config.h",
                "patched/include/mbedtls/cipher.h",
                "patched/include/mbedtls/cipher_internal.h",
                "patched/include/mbedtls/cmac.h",
                "patched/include/mbedtls/compat-1.3.h",
                "patched/include/mbedtls/config.h",
                "patched/include/mbedtls/ctr_drbg.h",
                "patched/include/mbedtls/debug.h",
                "patched/include/mbedtls/des.h",
                "patched/include/mbedtls/dhm.h",
                "patched/include/mbedtls/ecdh.h",
                "patched/include/mbedtls/ecdsa.h",
                "patched/include/mbedtls/ecjpake.h",
                "patched/include/mbedtls/ecp.h",
                "patched/include/mbedtls/entropy.h",
                "patched/include/mbedtls/entropy_poll.h",
	        "patched/include/mbedtls/error.h",
	        "patched/include/mbedtls/gcm.h",
	        "patched/include/mbedtls/havege.h",
	        "patched/include/mbedtls/hmac_drbg.h",
	        "patched/include/mbedtls/md.h",
	        "patched/include/mbedtls/md2.h",
	        "patched/include/mbedtls/md4.h",
	        "patched/include/mbedtls/md5.h",
	        "patched/include/mbedtls/md_internal.h",
	        "patched/include/mbedtls/memory_buffer_alloc.h",
	        "patched/include/mbedtls/net.h",
	        "patched/include/mbedtls/net_sockets.h",
	        "patched/include/mbedtls/oid.h",
	        "patched/include/mbedtls/padlock.h",
	        "patched/include/mbedtls/pem.h",
	        "patched/include/mbedtls/pk.h",
	        "patched/include/mbedtls/pk_internal.h",
	        "patched/include/mbedtls/pkcs11.h",
	        "patched/include/mbedtls/pkcs12.h",
	        "patched/include/mbedtls/pkcs5.h",
	        "patched/include/mbedtls/platform.h",
	        "patched/include/mbedtls/platform_time.h",
	        "patched/include/mbedtls/ripemd160.h",
	        "patched/include/mbedtls/rsa.h",
	        "patched/include/mbedtls/sha1.h",
	        "patched/include/mbedtls/sha256.h",
	        "patched/include/mbedtls/sha512.h",
	        "patched/include/mbedtls/ssl.h",
	        "patched/include/mbedtls/ssl_cache.h",
	        "patched/include/mbedtls/ssl_ciphersuites.h",
	        "patched/include/mbedtls/ssl_cookie.h",
	        "patched/include/mbedtls/ssl_internal.h",
	        "patched/include/mbedtls/ssl_ticket.h",
	        "patched/include/mbedtls/threading.h",
	        "patched/include/mbedtls/timing.h",
	        "patched/include/mbedtls/version.h",
	        "patched/include/mbedtls/x509.h",
	        "patched/include/mbedtls/x509_crl.h",
	        "patched/include/mbedtls/x509_crt.h",
	        "patched/include/mbedtls/x509_csr.h",
	        "patched/include/mbedtls/xtea.h"]

cc_library(
    name = "mbedtls",
    copts = ["-Ipatched/include",
             "-Ipatched/include/mbedtls",
             "-Iconfig",
             "-Iconfig/mbedtls",
             "-Iexternal/config",
             "-Iexternal/config/mbedtls",
             ],
    defines = select({"@//config:rpi_arm8": ["_POSIX_C_SOURCE=200809L"],
                      "//conditions:default": []}),
    data = [":patch"],
    srcs = [":patch", ":config"],
    hdrs = mbedtls_hdrs, # + ["@//config/mbedtls:config.h"],
    includes = ["patched/include", "patched/include/mbedtls", "x"],
    visibility = ["//visibility:public"]
)


