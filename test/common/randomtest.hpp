/* This file was automatically generated.  Do not edit! */
#if defined(HAVE_WINDOWS_H)
#include <winsock2.h>
#include <windows.h>
#include <Bcrypt.h>
#endif
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#if (__STDC_VERSION__ >= 201112L) /* C11 */ && defined(__STDC__)
#include <assert.h>
#endif
#if defined(_MSC_VER) 
#include <assert.h>
#endif
#define HAVE_STDBOOL_H 1
#if defined(HAVE_STDBOOL_H)
#include <stdbool.h>
#endif
#define HAVE__BOOL 1
#if !defined(HAVE__BOOL) && !(defined(HAVE_STDBOOL_H))
#   define _Bool signed char
#endif
#if !(defined(HAVE_STDBOOL_H))
# define bool _Bool
#endif
#define TB_LOG /**/
#if defined(TB_LOG)
#define UUID_SIZE (16)
#endif
#define UUID_SIZE (16)
#define UUID_STRING_SIZE (37)
extern "C" bool OCConvertUuidToString(const uint8_t uuid[UUID_SIZE],char uuidString[UUID_STRING_SIZE]);
extern "C" bool OCGenerateUuid(uint8_t uuid[UUID_SIZE]);
#if !(defined(HAVE_STDBOOL_H))
# define true 1
# define false 0
#endif
extern "C" bool OCGetRandomBytes(uint8_t *output,size_t len);
extern "C" uint32_t OCGetRandom();
