#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Iochibity], [1.3.0], [dev@mobileink.com])
#AC_CONFIG_SRCDIR([extlibs/rapidxml/rapidxml.hpp])
AC_CONFIG_HEADERS([src/iotivity_config.h])

# Checks for programs.
#AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
#AC_PROG_INSTALL
AC_PROG_LN_S
# AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lchibity':
#AC_CHECK_LIB([chibity], [main])
# FIXME: Replace `main' with a function in `-lconnectivity_abstraction':
#AC_CHECK_LIB([connectivity_abstraction], [main])
# FIXME: Replace `main' with a function in `-lm':
#AC_CHECK_LIB([m], [main])

AC_CHECK_LIB(pthread, pthread_create)
		      dnl [PTHREAD_LIBS+=-lpthread],
		      dnl [)

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h assert.h fcntl.h float.h inttypes.h limits.h malloc.h memory.h netdb.h netinet/in.h stdbool.h stddef.h stdint.h stdlib.h string.h sys/file.h sys/ioctl.h sys/mount.h sys/param.h sys/socket.h sys/statvfs.h sys/timeb.h termios.h, time.h utime.h windows.h winsock2.h ws2tcpip.h])

dnl AS_IF([test "x$_MSC_VER == x"],
dnl #ifndef _MSC_VER /* mingw has it, msvc does not */
dnl #undef HAVE_UNISTD_H
AC_CHECK_HEADER(unistd.h)
AC_CHECK_HEADER(sys/time.h)
AC_CHECK_HEADER(strings.h)
AC_CHECK_FUNCS([strptime])
dnl #endif

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_FUNC_STRNLEN
AC_FUNC_STRTOD

AC_CHECK_FUNCS([bzero clock_gettime fdatasync floor getcwd getpagesize gettimeofday localtime_r memmove memset pow select socket strcasecmp strchr strerror strncasecmp strnlen _strnicmp strndup strrchr strstr strtok_r strtok_s strtol strtoul time tzset utime QueryPerformanceCounter QueryPerformanceFrequency])

dnl ****************************************************************
dnl //third_party/mbedtls config.
dnl taken from extlibs/mbedtls/config-iotivity.h, config-iotivity_windows.h

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

dnl /**
dnl  * \def MBEDTLS_HAVE_ASM
dnl  *
dnl  * The compiler has support for asm().
dnl  *
dnl  * Requires support for asm() in compiler.
dnl  *
dnl  * Used in:
dnl  *      library/timing.c
dnl  *      library/padlock.c
dnl  *      include/mbedtls/bn_mul.h
dnl  *
dnl  * Comment to disable the use of assembly code.
dnl  */
dnl #define MBEDTLS_HAVE_ASM
dnl AC_DEFINE([MBEDTLS_HAVE_ASM], [], [Enable support for asm()])

dnl /**
dnl  * \def MBEDTLS_HAVE_SSE2
dnl  *
dnl  * CPU supports SSE2 instruction set.
dnl  *
dnl  * Uncomment if the CPU supports SSE2 (IA-32 specific).
dnl  */
dnl //#define MBEDTLS_HAVE_SSE2
dnl Find supported SIMD extensions by requesting cpuid. Defines HAVE_SSE2
dnl AX_EXT

dnl /**
dnl  * \def MBEDTLS_HAVE_TIME
dnl  *
dnl  * System has time.h and time().
dnl  * The time does not need to be correct, only time differences are used,
dnl  * by contrast with MBEDTLS_HAVE_TIME_DATE
dnl  *
dnl  * Defining MBEDTLS_HAVE_TIME allows you to specify MBEDTLS_PLATFORM_TIME_ALT,
dnl  * MBEDTLS_PLATFORM_TIME_MACRO, MBEDTLS_PLATFORM_TIME_TYPE_MACRO and
dnl  * MBEDTLS_PLATFORM_STD_TIME.
dnl  *
dnl  * Comment if your system does not support time functions
dnl  */
dnl #define MBEDTLS_HAVE_TIME
AC_CHECK_HEADER(time.h, [have_time_h=yes])
AS_IF([test "x$ac_cv_func_time" == xyes -a "x$have_time_h" == xyes],
	    [AC_DEFINE([MBEDTLS_HAVE_TIME], [1], [mbedtls: system has time.h and time()])])

dnl /**
dnl  * \def MBEDTLS_HAVE_TIME_DATE
dnl  *
dnl  * System has time.h and time(), gmtime() and the clock is correct.
dnl  * The time needs to be correct (not necesarily very accurate, but at least
dnl  * the date should be correct). This is used to verify the validity period of
dnl  * X.509 certificates.
dnl  *
dnl  * Comment if your system does not have a correct clock.
dnl  */
dnl #define MBEDTLS_HAVE_TIME_DATE
AC_CHECK_FUNC(gmtime)
AS_IF([test \( "x$ac_cv_func_time" == xyes \) -a \( "x$have_time_h" == xyes \) -a \( "x$ac_cv_func_gmtime" == xyes \)],
	    [AC_DEFINE([MBEDTLS_HAVE_TIME_DATE], [1], [mbedtls: system has time.h, time(), gmtime()])])


dnl not applicable (yet):
dnl MBEDTLS_PLATFORM_MEMORY
dnl MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
dnl MBEDTLS_PLATFORM_EXIT_ALT
dnl MBEDTLS_PLATFORM_TIME_ALT
dnl MBEDTLS_PLATFORM_FPRINTF_ALT
dnl MBEDTLS_PLATFORM_PRINTF_ALT
dnl MBEDTLS_PLATFORM_SNPRINTF_ALT
dnl MBEDTLS_PLATFORM_NV_SEED_ALT
dnl MBEDTLS_DEPRECATED_WARNING

dnl /**
dnl  * \def MBEDTLS_DEPRECATED_REMOVED
dnl  *
dnl  * Remove deprecated functions so that they generate an error if used.
dnl  * Functions deprecated in one version will usually be removed in the next
dnl  * version. You can enable this to help you prepare the transition to a new
dnl  * major version by making sure your code is not using these functions.
dnl  *
dnl  * Uncomment to get errors on using deprecated functions.
dnl  */
dnl #define MBEDTLS_DEPRECATED_REMOVED
AC_DEFINE([MBEDTLS_DEPRECATED_REMOVED], [1], [mbedtls: remove deprecated functions])

dnl /**
dnl  * \name SECTION: mbed TLS feature support
dnl  *
dnl  * This section sets support for features that are or are not needed
dnl  * within the modules that are enabled.
dnl  * \{
dnl  */

dnl alternate implementations; not used in iotivity currently:
dnl MBEDTLS_TIMING_ALT
dnl MBEDTLS_AES_ALT
dnl MBEDTLS_ARC4_ALT
dnl MBEDTLS_BLOWFISH_ALT
dnl MBEDTLS_CAMELLIA_ALT
dnl MBEDTLS_DES_ALT
dnl MBEDTLS_XTEA_ALT
dnl MBEDTLS_MD2_ALT
dnl MBEDTLS_MD4_ALT
dnl MBEDTLS_MD5_ALT
dnl MBEDTLS_RIPEMD160_ALT
dnl MBEDTLS_SHA1_ALT
dnl MBEDTLS_SHA256_ALT
dnl MBEDTLS_SHA512_ALT
dnl MBEDTLS_MD2_PROCESS_ALT
dnl MBEDTLS_MD4_PROCESS_ALT
dnl MBEDTLS_MD5_PROCESS_ALT
dnl MBEDTLS_RIPEMD160_PROCESS_ALT
dnl MBEDTLS_SHA1_PROCESS_ALT
dnl MBEDTLS_SHA256_PROCESS_ALT
dnl MBEDTLS_SHA512_PROCESS_ALT
dnl MBEDTLS_DES_SETKEY_ALT
dnl MBEDTLS_DES_CRYPT_ECB_ALT
dnl MBEDTLS_DES3_CRYPT_ECB_ALT
dnl MBEDTLS_AES_SETKEY_ENC_ALT
dnl MBEDTLS_AES_SETKEY_DEC_ALT
dnl MBEDTLS_AES_ENCRYPT_ALT
dnl MBEDTLS_AES_DECRYPT_ALT
dnl MBEDTLS_TEST_NULL_ENTROPY
dnl MBEDTLS_ENTROPY_HARDWARE_ALT

dnl MBEDTLS_AES_ROM_TABLES
dnl MBEDTLS_CAMELLIA_SMALL_MEMORY

dnl /**
dnl  * \def MBEDTLS_CIPHER_MODE_CBC
dnl  *
dnl  * Enable Cipher Block Chaining mode (CBC) for symmetric ciphers.
dnl  */
dnl #define MBEDTLS_CIPHER_MODE_CBC
AC_DEFINE([MBEDTLS_CIPHER_MODE_CBC],
	 [1],
	 [mbedtls: enable Cipher Block Chaining mode (CBC) for symmetric ciphers.])

dnl /**
dnl  * \def MBEDTLS_CIPHER_MODE_CFB
dnl  *
dnl  * Enable Cipher Feedback mode (CFB) for symmetric ciphers.
dnl  */
dnl //#define MBEDTLS_CIPHER_MODE_CFB

dnl /**
dnl  * \def MBEDTLS_CIPHER_MODE_CTR
dnl  *
dnl  * Enable Counter Block Cipher mode (CTR) for symmetric ciphers.
dnl  */
dnl #define MBEDTLS_CIPHER_MODE_CTR
AC_DEFINE([MBEDTLS_CIPHER_MODE_CTR],
	 [1],
	 [mbedtls: enable Counter Block Cipher mode (CTR) for symmetric ciphers.])

dnl MBEDTLS_CIPHER_NULL_CIPHER

dnl /**
dnl  * \def MBEDTLS_CIPHER_PADDING_PKCS7
dnl  *
dnl  * MBEDTLS_CIPHER_PADDING_XXX: Uncomment or comment macros to add support for
dnl  * specific padding modes in the cipher layer with cipher modes that support
dnl  * padding (e.g. CBC)
dnl  *
dnl  * If you disable all padding modes, only full blocks can be used with CBC.
dnl  *
dnl  * Enable padding modes in the cipher layer.
dnl  */
dnl #define MBEDTLS_CIPHER_PADDING_PKCS7
dnl //#define MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
dnl //#define MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
dnl //#define MBEDTLS_CIPHER_PADDING_ZEROS
AC_DEFINE([MBEDTLS_CIPHER_PADDING_PKCS7],
	 [1],
	 [mbedtls: enable PKCS7 padding mode])

dnl MBEDTLS_ENABLE_WEAK_CIPHERSUITES

dnl /**
dnl  * \def MBEDTLS_REMOVE_ARC4_CIPHERSUITES
dnl  *
dnl  * Remove RC4 ciphersuites by default in SSL / TLS.
dnl  * This flag removes the ciphersuites based on RC4 from the default list as
dnl  * returned by mbedtls_ssl_list_ciphersuites(). However, it is still possible to
dnl  * enable (some of) them with mbedtls_ssl_conf_ciphersuites() by including them
dnl  * explicitly.
dnl  *
dnl  * Uncomment this macro to remove RC4 ciphersuites by default.
dnl  */
dnl #define MBEDTLS_REMOVE_ARC4_CIPHERSUITES
AC_DEFINE([MBEDTLS_REMOVE_ARC4_CIPHERSUITES],
	 [1],
	 [mbedtls: remove the ciphersuites based on RC4 from the default list])

dnl /**
dnl  * \def MBEDTLS_ECP_DP_SECP192R1_ENABLED
dnl  *
dnl  * MBEDTLS_ECP_XXXX_ENABLED: Enables specific curves within the Elliptic Curve
dnl  * module.  By default all supported curves are enabled.
dnl  *
dnl  * Comment macros to disable the curve and functions for it
dnl  */
dnl //#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
dnl //#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
dnl #define MBEDTLS_ECP_DP_SECP256R1_ENABLED
AC_DEFINE([MBEDTLS_ECP_DP_SECP256R1_ENABLED],
	 [1],
	 [mbedtls: enable specific curve in Elliptic Curve module])
dnl //#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
dnl #define MBEDTLS_ECP_DP_SECP521R1_ENABLED
AC_DEFINE([MBEDTLS_ECP_DP_SECP521R1_ENABLED],
	 [1],
	 [mbedtls: enable specific curve in Elliptic Curve module])
dnl //#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
dnl //#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
dnl //#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
dnl //#define MBEDTLS_ECP_DP_BP256R1_ENABLED
dnl //#define MBEDTLS_ECP_DP_BP384R1_ENABLED
dnl //#define MBEDTLS_ECP_DP_BP512R1_ENABLED
dnl //#define MBEDTLS_ECP_DP_CURVE25519_ENABLED

dnl from now on, omitting the doc - see the files in extlibs/mbedtls
AC_DEFINE([MBEDTLS_ECP_NIST_OPTIM],
	 [1],
	 [mbedtls: enable specific 'modulo p' routines for each NIST prime.])

AC_DEFINE([MBEDTLS_ECDSA_DETERMINISTIC],
	 [1],
	 [mbedtls: enable deterministic ECDSA (RFC 6979)])

AC_DEFINE([MBEDTLS_KEY_EXCHANGE_PSK_ENABLED],
	 [1],
	 [mbedtls: enable the PSK based ciphersuite modes in SSL / TLS.])

dnl MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED

AC_DEFINE([MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED],
	 [1],
	 [mbedtls: enable the ECDHE-PSK based ciphersuite modes in SSL / TLS.])

dnl MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED

AC_DEFINE([MBEDTLS_KEY_EXCHANGE_RSA_ENABLED],
	 [1],
	 [mbedtls: enable the RSA-only based ciphersuite modes in SSL / TLS.])

dnl MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED


AC_DEFINE([MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED],
	 [1],
	 [mbedtls: enable the ECDHE-RSA based ciphersuite modes in SSL / TLS.])

AC_DEFINE([MBEDTLS_KEY_EXCHANGE_ECDH_ANON_ENABLED],
	 [1],
	 [mbedtls: enable the ECDHE-ANON based ciphersuite modes in SSL / TLS.])

AC_DEFINE([MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED],
	 [1],
	 [mbedtls: enable the ECDHE-ECDSA based ciphersuite modes in SSL / TLS.])

AC_DEFINE([MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED],
	 [1],
	 [mbedtls: enable the ECDH-ECDSA based ciphersuite modes in SSL / TLS.])

dnl MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
dnl MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED

AC_DEFINE([MBEDTLS_PK_PARSE_EC_EXTENDED],
	 [1],
	 [mbedtls: enhanced support for reading EC keys using variants of SEC1 not allowed by
 * RFC 5915 and RFC 5480.])

AC_DEFINE([MBEDTLS_ERROR_STRERROR_DUMMY],
	 [1],
	 [mbedtls: enable a dummy error function to make use of mbedtls_strerror()])



dnl ****************************************************************

dnl ****************************************************************
dnl see https://anoncvs.postgresql.org/cvsweb.cgi/pgsql/config/acx_pthread.m4?rev=1.12;content-type=text%2Fplain
dnl ACX_PTHREAD([
dnl 	LIBS="$LIBS $PTHREAD_LIBS"
dnl 	CFLAGS="$CFLAGS $PTHREAD_CFLAGS -g -Wall"
dnl 	CC="$PTHREAD_CC"
dnl 	AC_SUBST([LIBS])
dnl 	AC_SUBST([CFLAGS])
dnl 	AC_SUBST([CC])
dnl 	],
dnl 	[AC_MSG_ERROR([POSIX threads support is required])])

dnl AC_CONFIG_FILES([resource/csdk/connectivity/build/arduino/Makefile
dnl                  resource/csdk/connectivity/build/tizen/Makefile
dnl                  src/comm/bld/android/Makefile
dnl                  src/comm/bld/arduino/Makefile
dnl                  src/comm/bld/tizen/Makefile
dnl                  tools/json2cbor/Makefile])
dnl AC_CONFIG_SUBDIRS([extlibs/tinydtls
dnl                    resource/csdk/connectivity/lib/libcoap-4.1.1
dnl                    third_party/coap
dnl                    third_party/tinydtls])


dnl TODO: for consistency with autotools, change __WITH_DTLS__ to ENABLE_DTLS, etc.
dnl DTLS is on by default
AC_ARG_ENABLE([dtls],
    AS_HELP_STRING([--disable-dtls], [Disable DTLS]))

AS_IF([test "x$enable_dtls" != "xno"], [
  AC_DEFINE([__WITH_DTLS__], [], [Enable DTLS])
])

dnl TLS is OFF by default
AC_ARG_ENABLE([tls],
    AS_HELP_STRING([--enable-tls], [Enable TLS]))

AS_IF([test "x$enable_tls" = "xyes"], [
  AC_DEFINE([__WITH_TLS__], [], [Enable TLS])
])

dnl BlockWise Transfer is on by default
AC_ARG_ENABLE([bwt],
    AS_HELP_STRING([--disable-bwt], [Disable Block-Wise Transfer]))

AS_IF([test "x$enable_bwt" != "xno"], [
  AC_DEFINE([WITH_BWT], [], [Block-wise Transfer])
])

dnl multiple owners is OFF by default
AC_ARG_ENABLE([multiple-owners],
    AS_HELP_STRING([--enable-multiple-owners], [Enable Multiple Ownership]))

AS_IF([test "x$enable_multiple_owners" = "xyes"], [
  AC_DEFINE([MULTIPLE_OWNER], [], [Multiple ownership])
])

dnl logging is OFF by default
AC_ARG_ENABLE([logging],
    AS_HELP_STRING([--enable-logging], [Enable logging]))

AS_IF([test "x$enable_logging" = "xyes"], [
  AC_DEFINE([TB_LOG], [], [Logging])
])

dnl /* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
dnl    significant byte first (like Motorola and SPARC, unlike Intel). */
dnl #if defined AC_APPLE_UNIVERSAL_BUILD
dnl # if defined __BIG_ENDIAN__
dnl #  define WORDS_BIGENDIAN 1
dnl # endif
dnl #else
dnl # ifndef WORDS_BIGENDIAN
dnl #  undef WORDS_BIGENDIAN
dnl # endif
dnl #endif

dnl #include "system.h"


AC_OUTPUT
