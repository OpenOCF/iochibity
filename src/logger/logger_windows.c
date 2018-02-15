//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/*
 * Expose POSIX.1-2008 base specification,
 * Refer http://pubs.opengroup.org/onlinepubs/9699919799/
 * For this specific file, see use of clock_gettime,
 * Refer to http://pubs.opengroup.org/stage7tc1/functions/clock_gettime.html
 * and to http://man7.org/linux/man-pages/man2/clock_gettime.2.html
 */
#define _POSIX_C_SOURCE 200809L

#include "logger_windows.h"

/* MINGW64: */
/* uname -s MINGW64_NT-6.1 */
/* Predefined macros: */
/* #define __MINGW32__ 1 */
/* #define __MINGW64__ 1 */
/* #define __WIN32 1 */
/* #define __WIN32__ 1 */
/* #define __WIN64 1 */
/* #define __WIN64__ 1 */
/* #define __WINNT 1 */
/* #define __WINNT__ 1 */
/* #define __x86_64 1 */
/* #define __x86_64__ 1 */
/* #define _WIN32 1 */
/* #define _WIN64 1 */
/* #define WIN32 1 */
/* #define WIN64 1 */
/* #define WINNT 1 */

// Pull in _POSIX_TIMERS feature test macro to check for
// clock_gettime() support.
#if EXPORT_INTERFACE
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#include <inttypes.h>
#include <stddef.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <time.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

/* #if EXPORT_INTERFACE		/\* FIXME: why doesn't CNOH_INTERFACE work? *\/ */
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
/* #endif */

/* #include "logger.h" */
#include <string.h>
/* #include "logger_types.h" */

#include <stdio.h>
#include <stdarg.h>

/* #if defined(_MSC_VER) */
/* #define LINE_BUFFER_SIZE (16 * 2) + 16 + 1  // Show 16 bytes, 2 chars/byte, spaces between bytes, null termination */
/* #endif //defined(_MSC_VER) */


#if defined(__MINGW64__) || defined(__MINGW32__)
const char * LEVEL[] __attribute__ ((unused)) = {"\e[0;32mDEBUG\033[0m", "\e[0;33mINFO\033[0m", "\e[0;35mWARNING\033[0m", "\e[0;31mERROR\033[0m", "\e[0;31mFATAL\033[0m"};
#elif defined(_MSC_VER)
const char * LEVEL[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
#endif
    /* static const char *LEVEL[] __attribute__ ((unused)) = */
    /* {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"}; */

/**
 * Output a log string with the specified priority level.
 * Only defined for Linux and Android
 *
 * @param level  - One of DEBUG, INFO, WARNING, ERROR, or FATAL plus possibly the OC_LOG_PRIVATE_DATA bit
 * @param tag    - Module name
 * @param logStr - log string
 */
void OCLog(int level, const char * tag, const char * logStr)
{
    if (!logStr || !tag)
    {
       return;
    }

    if (!AdjustAndVerifyLogLevel(&level))
    {
        return;
    }

    switch(level)
    {
        case DEBUG_LITE:
            level = DEBUG;
            break;
        case INFO_LITE:
            level = INFO;
            break;
        default:
            break;
    }

    if (logCtx && logCtx->write_level)
	{
	    logCtx->write_level(logCtx, LEVEL_XTABLE[level], logStr);

	}
    else
	{
	    int min = 0;
	    int sec = 0;
	    int ms = 0;

	    SYSTEMTIME systemTime = {0};
	    GetLocalTime(&systemTime);
	    min = (int)systemTime.wMinute;
	    sec = (int)systemTime.wSecond;
	    ms  = (int)systemTime.wMilliseconds;

	    /* GAR FIXME: make a separate Log fn for timestamped msgs */
	    /* printf("%02d:%02d.%03d %s: %s: %s\n", min, sec, ms, LEVEL[level], tag, logStr); */
	    /* printf("%s %s %s\n", LEVEL[level], tag, logStr); */
	    write_log("%s %s %s\n", LEVEL[level], tag, logStr);
	    fflush(stdout);
	}
}
