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

#include "logger_posix.h"

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

#ifdef __ANDROID__
#include <android/log.h>
#elif defined(__TIZEN__)
#include <dlog.h>
#endif

#include <stdio.h>
#include <stdarg.h>

    static const char *LEVEL[] __attribute__ ((unused)) =
    {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};

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

   #ifdef __ANDROID__

   #ifdef ADB_SHELL
       printf("%s: %s: %s\n", LEVEL[level], tag, logStr);
   #else
       __android_log_write(LEVEL[level], tag, logStr);
   #endif

   #else  /* not ANDROID */
       if (logCtx && logCtx->write_level)
       {
           logCtx->write_level(logCtx, LEVEL_XTABLE[level], logStr);

       }
       else
	   {
	       int min = 0;
	       int sec = 0;
	       int ms = 0;
#if defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0
	       struct timespec when = { .tv_sec = 0, .tv_nsec = 0 };
	       clockid_t clk = CLOCK_REALTIME;
#ifdef CLOCK_REALTIME_COARSE
	       clk = CLOCK_REALTIME_COARSE;
#endif
	       if (!clock_gettime(clk, &when))
		   {
		       min = (when.tv_sec / 60) % 60;
		       sec = when.tv_sec % 60;
		       ms = when.tv_nsec / 1000000;
		   }
#elif defined(_WIN32)
	       //#include <windows.h>
	       SYSTEMTIME systemTime = {0};
	       GetLocalTime(&systemTime);
	       min = (int)systemTime.wMinute;
	       sec = (int)systemTime.wSecond;
	       ms  = (int)systemTime.wMilliseconds;
#else  /* not posix, not win32 */
	       struct timeval now;
	       if (!gettimeofday(&now, NULL))
		   {
		       min = (now.tv_sec / 60) % 60;
		       sec = now.tv_sec % 60;
		       ms = now.tv_usec * 1000;
		   }
#endif	/*  */
	       /* GAR FIXME: make a separate Log fn for timestamped msgs */
	       /* printf("%02d:%02d.%03d %s: %s: %s\n", min, sec, ms, LEVEL[level], tag, logStr); */
	       /* printf("%s %s %s\n", LEVEL[level], tag, logStr); */
	       /* write_log("%s %s %s\n", LEVEL[level], tag, logStr); */
	       flockfile(logfd);
	       fprintf(logfd, "%s %s %s\n", LEVEL[level], tag, logStr);
	       fflush(logfd);
	       funlockfile(logfd);
	   }
#endif	/* not android */
}

#if EXPORT_INTERFACE
#ifdef TB_LOG
#define OIC_LOG_BUFFER(level, tag, buffer, bufferSize) \
    do { \
    if (((int)OC_MINIMUM_LOG_LEVEL) <= ((int)(level & (~OC_LOG_PRIVATE_DATA)))) \
            OCLogBuffer((level), __FILE__, __LINE__, (buffer), (bufferSize)); \
    } while(0)

#define OIC_LOG_CA_BUFFER(level, tag, buffer, bufferSize, isHeader) \
    do { \
    if (((int)OC_MINIMUM_LOG_LEVEL) <= ((int)(level & (~OC_LOG_PRIVATE_DATA)))) \
            OCPrintCALogBuffer((level), __FILE__, __LINE__, (buffer), (bufferSize), (isHeader)); \
    } while(0)

#define OIC_LOG_CONFIG(ctx)    OCLogConfig((ctx))
#define OIC_LOG_SHUTDOWN()     OCLogShutdown()
#define OIC_LOG(level, tag, logStr) \
    do { \
    if (((int)OC_MINIMUM_LOG_LEVEL) <= ((int)(level & (~OC_LOG_PRIVATE_DATA)))) \
	    OCLog((level), (__FILE__ ":" TOSTRING(__LINE__)), (logStr));	\
    } while(0)

// Define variable argument log function for Linux, Android, and Win32
#define OIC_LOG_V(level, tag, ...) \
    do { \
    if (((int)OC_MINIMUM_LOG_LEVEL) <= ((int)(level & (~OC_LOG_PRIVATE_DATA)))) \
	    OCLogv((level), __FILE__, __LINE__, __VA_ARGS__); \
    } while(0)
#else // TB_LOG
#define OIC_LOG_CONFIG(ctx)
#define OIC_LOG_SHUTDOWN()
#define OIC_LOG(level, tag, logStr)
#define OIC_LOG_V(level, tag, ...)
#define OIC_LOG_BUFFER(level, tag, buffer, bufferSize)
#define OIC_LOG_CA_BUFFER(level, tag, buffer, bufferSize, isHeader)
#define OIC_LOG_INIT()
#endif // TB_LOG
#endif	/* EXPORT_INTERFACE */
