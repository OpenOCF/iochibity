/* This file was automatically generated.  Do not edit! */
#define HAVE_WINDOWS_H 1
#if defined(HAVE_WINDOWS_H)
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#endif
#if !(defined(HAVE_WINDOWS_H))
#include <sys/time.h>
#endif
#include <stddef.h>
#define HAVE_UNISTD_H 1
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#define HAVE_TIME_H 1
#if defined(HAVE_TIME_H)
#include <time.h>
#endif
#define HAVE_SYS_TIME_H 1
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#include <stdint.h>
typedef enum {
    TIME_IN_MS = 0,     //!< milliseconds
    TIME_IN_US,         //!< microseconds
}OICTimePrecision;
extern "C" uint64_t OICGetCurrentTime(OICTimePrecision precision);
