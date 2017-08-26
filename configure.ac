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
AC_CHECK_HEADERS([arpa/inet.h assert.h fcntl.h float.h inttypes.h limits.h malloc.h memory.h netdb.h netinet/in.h stdbool.h stddef.h stdint.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/mount.h sys/param.h sys/socket.h sys/statvfs.h sys/time.h sys/timeb.h termios.h, time.h unistd.h utime.h windows.h winsock2.h ws2tcpip.h])

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

AC_CHECK_FUNCS([bzero clock_gettime fdatasync floor getcwd getpagesize gettimeofday localtime_r memmove memset pow select socket strcasecmp strchr strerror strncasecmp _strnicmp strndup strptime strrchr strstr strtok_r strtok_s strtol strtoul tzset utime QueryPerformanceCounter QueryPerformanceFrequency])

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

AC_OUTPUT
