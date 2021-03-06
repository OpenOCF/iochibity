#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([OpenOCF], [1.3.0], [dev@mobileink.com])
#AC_CONFIG_SRCDIR([extlibs/rapidxml/rapidxml.hpp])
AC_CONFIG_HEADERS([src/_openocf_config.h])
dnl AC_CONFIG_MACRO_DIRS([m4]
AC_USE_SYSTEM_EXTENSIONS

dnl AC_EGREP_CPP(posix_200809L_supported,
dnl              [#define _POSIX_C_SOURCE 200809L
dnl               #include <unistd.h>
dnl               #ifdef _POSIX_VERSION
dnl               #if _POSIX_VERSION == 200809L
dnl               posix_200809L_supported
dnl               #endif
dnl               #endif
dnl              ],
dnl              [],
dnl              [AC_MSG_FAILURE([*** Implementation must conform to the POSIX.1-2008 standard.])]
dnl )

dnl AH_TOP([#if INTERFACE])

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

dnl mingw has pthread.h, but windows does not have libpthread
dnl on windows, we use native threads via windows.h
dnl we check for libpthread, not pthread.h
dnl if successful, this will set HAVE_LIBPTHREAD but not HAVE_PTHREAD_H
AC_CHECK_LIB(pthread, pthread_create)

dnl if platform != windows and HAVE_LIBPTHREAD:
AC_CHECK_HEADERS([pthread.h])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h assert.h fcntl.h errno.h float.h inttypes.h limits.h malloc.h memory.h net/if.h netdb.h netinet/in.h stdbool.h stddef.h stdint.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/mount.h sys/param.h sys/select.h sys/socket.h sys/statvfs.h sys/time.h sys/timeb.h syslog.h termios.h, time.h unistd.h utime.h windows.h winsock2.h ws2tcpip.h])

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
AC_FUNC_MKTIME
# see https://lists.gnu.org/archive/html/automake/2003-05/msg00018.html
# buggy, use AC_CHECK_FUNCS instead: AC_FUNC_MALLOC
# buggy, use AC_CHECK_FUNCS instead: AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_FUNC_STRNLEN
AC_FUNC_STRTOD

AC_CHECK_FUNCS([bzero clock_gettime fdatasync floor flockfile getcwd getpagesize gettimeofday localtime_r malloc memmove memset pipe2 pow realloc select socket strcasecmp strchr strerror strncasecmp strnlen _strnicmp strndup strptime strrchr strstr strtok_r strtok_s strtol strtoul time tzset utime])

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

dnl TODO: for consistency with autotools, change __WITH_DTLS__ to ENABLE_DTLS, etc.

dnl IPv4 enabled by default
AC_ARG_ENABLE([ipv4],
    AS_HELP_STRING([--disable-ipv4], [Disable IPv4 support (enabled by default)]))

AS_IF([test "x$enable_ipv4" != "xno"], [
  AC_DEFINE([ENABLE_IPV4], [], [Enable IPv4])
])

dnl IPv6 enabled by default
AC_ARG_ENABLE([ipv6],
    AS_HELP_STRING([--disable-ipv6], [Disable IPv6 support (enabled by default)]))

AS_IF([test "x$enable_ipv6" != "xno"], [
  AC_DEFINE([ENABLE_IPV6], [], [Enable IPv6])
])

dnl DTSL and TLS are build configurations, set by bazel options and defined on cmd line
dnl dnl DTLS is on by default
dnl AC_ARG_ENABLE([dtls],
dnl     AS_HELP_STRING([--disable-dtls], [Disable DTLS (enabled by default)]))

dnl AS_IF([test "x$enable_dtls" != "xno"], [
dnl   AC_DEFINE([__WITH_DTLS__], [], [Enable DTLS])
dnl ])

dnl dnl TLS is OFF by default
dnl AC_ARG_ENABLE([tls],
dnl     AS_HELP_STRING([--enable-tls], [Enable TLS]))

dnl AS_IF([test "x$enable_tls" = "xyes"], [
dnl   AC_DEFINE([__WITH_TLS__], [], [Enable TLS])
dnl ])

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

dnl debugging is OFF by default
AC_ARG_ENABLE([debugging],
    AS_HELP_STRING([--enable-debugging], [Enable debugging]))

AS_IF([test "x$enable_debugging" = "xyes"], [
  dnl AC_DEFINE([DEBUG], [], [Debugging])
  AC_DEFINE([OC_DEBUG], [], [OC Debugging])
])

dnl thread debugging is OFF by default
AC_ARG_ENABLE([thread-debugging],
    AS_HELP_STRING([--enable-thread-debugging], [Enable thread debugging]))

AS_IF([test "x$enable_thread_debugging" = "xyes"], [
  AC_DEFINE([DEBUG_THREADING], [], [Thread Debugging])
])

dnl mbedtls debugging is OFF by default
AC_ARG_ENABLE([malloc-debug],
    AS_HELP_STRING([--enable-malloc-debug], [Enable malloc debug]))

AS_IF([test "x$enable_malloc_debug" = "xyes"], [
  AC_DEFINE([ENABLE_MALLOC_DEBUG], [], [Malloc Debug])
])

dnl mbedtls debugging is OFF by default
AC_ARG_ENABLE([mbedtls-debugging],
    AS_HELP_STRING([--enable-mbedtls-debugging], [Enable mbedtls debugging]))

AS_IF([test "x$enable_mbedtls_debugging" = "xyes"], [
  AC_DEFINE([MBEDTLS_DEBUG_C], [], [MBEDTLS Debugging])
])

dnl libcoap debugging is OFF by default
AC_ARG_ENABLE([coap-debugging],
    AS_HELP_STRING([--enable-coap-debugging], [Enable coap debugging]))

AS_IF([test "x$enable_coap_debugging" = "xyes"], [
  AC_DEFINE([COAP_DEBUG_C], [], [COAP Debugging])
])

dnl logging is OFF by default
AC_ARG_ENABLE([logging],
    AS_HELP_STRING([--enable-logging], [Enable logging]))

AS_IF([test "x$enable_logging" = "xyes"], [
  AC_DEFINE([TB_LOG], [], [Logging])
])

dnl nw change event logging is on by default
AC_ARG_ENABLE([nwchg-logging],
    AS_HELP_STRING([--disable-nwchg-logging], [Disable Network Change Logging]))

AS_IF([test "x$enable_nwchg" != "xno"], [
  AC_DEFINE([NETWORK_INTERFACE_CHANGED_LOGGING], [], [Network Change Logging])
])

dnl enable adapters. default to IP_ADAPTER
AC_ARG_ENABLE([udpip],
    AS_HELP_STRING([--disable-udpip], [Disable UDP/IP adapter (default: enabled)]))

AS_IF([test "x$enable_udpip" != "xno"], [
  AC_DEFINE([IP_ADAPTER], [], [Enable IP_ADAPTER]),
  AC_DEFINE([ENABLE_UDP], [], [Enable UDP])
])

AC_ARG_ENABLE([ble],
    AS_HELP_STRING([--enable-ble], [Enable BLE transport]))

AS_IF([test "x$enable_ipadapter" = "xyes"], [
  AC_DEFINE([LE_ADAPTER], [], [Enable BLE transport])
])

AC_ARG_ENABLE([bredr],
    AS_HELP_STRING([--enable-bredr], [Enable BR/EDR transpor]))

AS_IF([test "x$enable_bredr" = "xyes"], [
  AC_DEFINE([EDR_ADAPTER], [], [Enable BR/EDR transport])
])

AC_ARG_ENABLE([nfc],
    AS_HELP_STRING([--enable-nfc], [Enable NFC transport]))

AS_IF([test "x$enable_nfc" = "xyes"], [
  AC_DEFINE([NFC_ADAPTER], [], [Enable NFC transport])
])

AC_ARG_ENABLE([xmpp],
    AS_HELP_STRING([--enable-xmpp], [Enable XMPP (Remote Access) transport]))

AS_IF([test "x$enable_xmpp" = "xyes"], [
  AC_DEFINE([RA_ADAPTER], [], [Enable XMPP (Remote Access) transport])
])

AC_ARG_ENABLE([tcpip],
    AS_HELP_STRING([--enable-tcpip], [Enable TCP/IP transport]))

AS_IF([test "x$enable_tcpip" = "xyes"], [
  AC_DEFINE([TCP_ADAPTER], [], [Enable TCP/IP transport])
])

dnl Presence is DISABLED by default
AC_ARG_ENABLE([presence],
    AS_HELP_STRING([--enable-presence], [Enable PRESENCE]))

AS_IF([test "x$enable_presence" = "xyes"], [
  AC_DEFINE([WITH_PRESENCE], [], [Enable PRESENCE])
])

dnl FIXME: --with-presence

dnl AC_ARG_WITH([msvc],
dnl             [AS_HELP_STRING([--with-msvc],
dnl               [enable support for msvc toolchain])],
dnl             [with_msvc=yes],
dnl             [with_msvc=no])

dnl AS_IF([test "x$with_msvc" != xyes], [
dnl   AC_CHECK_HEADER(sys/time.h)
dnl   AC_CHECK_HEADER(foo.h)
dnl ])

dnl AH_BOTTOM([
dnl #ifdef _MSC_VER
dnl #include "msvc_config.h"
dnl #endif
dnl ])

AH_BOTTOM([
/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
#  undef WORDS_BIGENDIAN
# endif
#endif])


AH_BOTTOM([#include "system.h"])
dnl AH_BOTTOM([#endif /* INTERFACE */])

AC_OUTPUT
