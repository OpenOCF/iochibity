/* #ifndef SYSTEM_H */
/* #define SYSTEM_H */

#if INTERFACE
/* /\* definition to expand macro then apply to pragma message *\/ */
/* #define VALUE_TO_STRING(x) #x */
/* #define VALUE(x) VALUE_TO_STRING(x) */
/* #define VAR_NAME_VALUE(var) #var "="  VALUE(var) */

/* undo mingw-based autoconf stuff if using ms toolchain for win */
#ifdef _MSC_VER
#undef HAVE_LIBPTHREAD
#undef HAVE_PTHREAD_H
#undef HAVE_STRINGS_H
#undef HAVE_SYS_TIME_H
#undef TIME_WITH_SYS_TIME
#undef HAVE_UNISTD_H
#endif

/* #pragma message(VAR_NAME_VALUE(HAVE_SYS_TIME_H)) */

/* from resource/c_common/platform_features.h */
#if defined(__STDC__)
/* #if defined(__ANDROID__)	   /\* FIXME: android x86 toolchain broken *\/ */
/* #  define OC_STATIC_ASSERT */
/* #else */
#if (__STDC_VERSION__ >= 201112L) /* C11 */
#   include <assert.h>
#   define OC_STATIC_ASSERT(condition, msg) static_assert(condition, msg)
#else  /* non-C11, non-MS c compiler  */
#  error "OpenOCF requires C11; you are using a compiler with __STDC_VERSION_ =" __STDC_VERSION__
#endif
#endif

#if defined(_MSC_VER) 
#   include <assert.h>
#   define OC_STATIC_ASSERT(condition, msg) static_assert(condition, msg)
#endif

/* OpenOCF is C only, no GLIBCXX */

// see https://gustedt.wordpress.com/2010/11/29/myth-and-reality-about-inline-in-c99/
/* #ifndef INLINE_API */
#ifdef _MSC_VER
#define INLINE_API static __inline
#else
#define INLINE_API static inline
#endif
/* #endif */

#ifdef _MSC_VER
#  define OC_ANNOTATE_UNUSED
#else
#  define OC_ANNOTATE_UNUSED  __attribute__((unused))
#endif


#  define CJSON_HIDE_SYMBOLS 1


/* C99 (see https://www.gnu.org/software/autoconf/manual/autoconf-2.64/html_node/Particular-Headers.html#Particular-Headers)
*/
#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
# ifndef HAVE__BOOL
#  ifdef __cplusplus
typedef bool _Bool;
#  else
#   define _Bool signed char
#  endif
# endif
# define bool _Bool
# define false 0
# define true 1
# define __bool_true_false_are_defined 1
#endif

/* for libcoap-4.1.1 (protocol/coap): WITH_POSIX == HAVE_LIBPTHREAD? */

#define WITH_POSIX 1

#ifdef _WIN32
//GAR: mingw has both strtok_r and strtok_s
//GAR: strtok_s is c11, strtok_r is posix
//GAR: todo: conditional compile: c11 uses strtok_s, c99 strtok_r?
#  define strtok_r strtok_s

//GAR: this goes in windows portability layer
/* #  if _MSC_VER && (_MSC_VER < 1900) */
/* #  if (_MSC_VER < 1900) */
/* #    include "windows/include/vs12_snprintf.h" */
/* #  endif */

#  include <BaseTsd.h>
#  define ssize_t SSIZE_T
#  define SHUT_RDWR           SD_BOTH
#  define sleep(SECS)         Sleep(1000*(SECS))
#endif

//GAR: built as windows portability layer
/* #  include "windows/include/memmem.h" */
/* #  include "windows/include/win_sleep.h" */
/* #  include "windows/include/pthread_create.h" */

#ifdef HAVE_STRNCASECMP
# ifdef _MSC_VER
#  undef HAVE_STRNCASECMP
#  ifdef HAVE__STRNICMP
#    include <string.h>
#    define strncasecmp _strnicmp
#  else
#    include <string.h>
#    define strncasecmp _strnicmp
/* #    error "No strncasecmp (posix), no _strnicmp (windows)" */
#  endif
# else
#    include <string.h>
# endif
#else
#  ifdef HAVE__STRNICMP		
#    include <string.h>
#    define strncasecmp _strnicmp
#  else
#    error "No strncasecmp (posix), no _strnicmp (windows)"
#  endif
#endif

//GAR: todo: find an AC macro that does this
// e.g. https://github.com/gagern/gnulib/blob/master/m4/size_max.m4
#include <stdint.h>
#ifndef SIZE_MAX
/* Some systems fail to define SIZE_MAX in <stdint.h>, even though C99 requires it...
 * Conversion from signed to unsigned is defined in 6.3.1.3 (Signed and unsigned integers) p2,
 * which says: "the value is converted by repeatedly adding or subtracting one more than the
 * maximum value that can be represented in the new type until the value is in the range of the
 * new type."
 * So -1 gets converted to size_t by adding SIZE_MAX + 1, which results in SIZE_MAX.
 */
#  define SIZE_MAX ((size_t)-1)
#endif

#ifdef _WIN32
/*
 * Set to __stdcall for Windows, consistent with WIN32 APIs.
 */
#  define OC_CALL   __stdcall
#else
#  define OC_CALL
#endif

/**
 * Mark a parameter as unused. Used to prevent unused variable compiler warnings.
 */
#define OC_UNUSED(x) (void)(x)

#endif	/* INTERFACE */
