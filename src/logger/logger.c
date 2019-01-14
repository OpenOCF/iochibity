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

#include "logger.h"

#if INTERFACE
#include "coap_config.h"
#include "coap/pdu.h"
#endif

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

#include <errno.h>
#include <limits.h>             /* PATH_MAX */
#include <stdlib.h>

#ifdef HAVE_BSD_STRING_H
#include <bsd/string.h>
#else
#include <string.h>
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

#if INTERFACE
#include <stdio.h>
#endif

#include <stdarg.h>

#ifdef HAVE_STRLCPY
#include <string.h>
#endif

#ifdef HAVE_BSD_STRING
#include <bsd/string.h>
#endif

/* FIXME: flockfile on mingw? */
/* #ifdef __MINGW64__ */
/* #define _POSIX_THREAD_SAFE_FUNCTIONS */
/* #include <stdlib.h> */
/* #include <pthread_unistd.h> */
/* #endif */

oc_mutex log_mutex = NULL;
oc_mutex logv_mutex = NULL;

#define TAG  "OCF LOGGER"

#define ANALYZER_TAG "OIC_ANALYZER"

#if EXPORT_INTERFACE
#ifdef TB_LOG
#define OIC_LOG_PAYLOAD(level, payload) OCPayloadLog((level),(payload))
#endif
#endif	/* INTERFACE */

#if EXPORT_INTERFACE
#ifdef TB_LOG
#define CA_TRANSPORT_ADAPTER_STRING(TRANSPORT) ( \
    (TRANSPORT == CA_DEFAULT_ADAPTER) ? "CA_DEFAULT_ADAPTER" : \
    (TRANSPORT == CA_ADAPTER_IP) ? "CA_ADAPTER_IP" : \
    (TRANSPORT == CA_ADAPTER_GATT_BTLE) ? "CA_ADAPTER_GATT_BTLE" : \
    (TRANSPORT == CA_ADAPTER_RFCOMM_BTEDR) ? "CA_ADAPTER_RFCOMM_BTEDR" : \
    (TRANSPORT == CA_ADAPTER_TCP) ? "CA_ADAPTER_TCP" : \
    (TRANSPORT == CA_ALL_ADAPTERS) ? "CA_ALL_ADAPTERS" : "UNKNOWN")
#endif
#endif

#if EXPORT_INTERFACE
typedef enum
{
    CA_LOG_LEVEL_ALL = 1,             // all logs.
    CA_LOG_LEVEL_INFO,                // debug level is disabled.
} CAUtilLogLevel_t;

// Use the PCF macro to wrap strings stored in FLASH on the Arduino
// Example:  OIC_LOG(INFO, TAG, PCF("Entering function"));
#define PCF(str) str

typedef enum
{
    OC_LOG_MIN_VAL__   = -1,
    OC_LOG_ALL         = 0,
    OC_LOG_FATAL,
    OC_LOG_ERROR,
    OC_LOG_WARNING,
    OC_LOG_INFO,
    OC_LOG_DEBUG,
    OC_LOG_DISABLED,
    OC_LOG_MAX_VAL__
} oc_log_level;

typedef struct _oc_log_ctx
{
    void*                  ctx;

    oc_log_level           log_level;

    char*                  module_name;

    /* Required interface: */
    int  (*init)           (struct _oc_log_ctx *, void *);
    void (*destroy)        (struct _oc_log_ctx *);
    void (*flush)          (struct _oc_log_ctx *);
    void (*set_level)      (struct _oc_log_ctx *, const int);
    size_t (*write_level)  (struct _oc_log_ctx *, const int, const char *);
    int  (*set_module)     (struct _oc_log_ctx *, const char *);

    /* Optional interface (if one is implemented, all must be implemented): */
    int (*lock)            (struct _oc_log_ctx *);
    int (*unlock)          (struct _oc_log_ctx *);
    int (*try_lock)        (struct _oc_log_ctx *);
    int (*locked_destroy)  (struct _oc_log_ctx *);
} oc_log_ctx_t;

/* Notice that these are all passed the /top level/ ctx-- it's "public" with respect to
these functions, they have full access to fiddle with the structure all they want (but,
generally should avoid doing that); I could certainly be convinced to go the other direction,
and have most functions only take the inner context: */
typedef int    (*oc_log_init_t)          (oc_log_ctx_t *, void *);
typedef void   (*oc_log_destroy_t)       (oc_log_ctx_t *);
typedef void   (*oc_log_flush_t)         (oc_log_ctx_t *);
typedef void   (*oc_log_set_level_t)     (oc_log_ctx_t *, const int);
typedef size_t (*oc_log_write_level_t)   (oc_log_ctx_t *, const int, const char *);
typedef int    (*oc_log_set_module_t)    (oc_log_ctx_t *, const char *);
typedef int    (*oc_log_lock_t)          (oc_log_ctx_t *);
typedef int    (*oc_log_unlock_t)        (oc_log_ctx_t *);
typedef int    (*oc_log_try_lock_t)      (oc_log_ctx_t *);

/* printf function type */
/* typedef int (*log_writer_t)(const char * format, ...); */
void oocf_log_hook_stdout(FILE *fd);
/* void oocf_log_hook_stdout(log_writer_t hook); */

/* For printing __LINE__ */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Max buffer size used in variable argument log function
#define MAX_LOG_V_BUFFER_SIZE (256)

// Setting this flag for a log level means that the corresponding log message
// contains private data. This kind of message is logged only when a call to
// OCSetLogLevel() enabled private data logging.
#define OC_LOG_PRIVATE_DATA (1 << 31)

// Log levels

#define DEBUG_PRIVATE       ((OC_LOG_PRIVATE_DATA) | (DEBUG))
#define INFO_PRIVATE        ((OC_LOG_PRIVATE_DATA) | (INFO))
#define WARNING_PRIVATE     ((OC_LOG_PRIVATE_DATA) | (WARNING))
#define ERROR_PRIVATE       ((OC_LOG_PRIVATE_DATA) | (ERROR))
#define FATAL_PRIVATE       ((OC_LOG_PRIVATE_DATA) | (FATAL))

#ifndef OC_LOG_LEVEL
#define OC_MINIMUM_LOG_LEVEL    (DEBUG)
#else
#define OC_MINIMUM_LOG_LEVEL    (OC_LOG_LEVEL)
#endif


/* Show 16 bytes, 2 chars/byte, spaces between bytes, null termination */
/* (16 * 2) + 16 + 1 = 49 */
#define LINE_BUFFER_SIZE 49

#endif	/* EXPORT_INTERFACE */


// Perform signed comparison here, to avoid compiler warnings caused by
// unsigned comparison with DEBUG (i.e., with value 0 on some platforms).
/* #define IF_OC_PRINT_LOG_LEVEL(level) \
 *     if (((int)OC_MINIMUM_LOG_LEVEL) <= ((int)(level & (~OC_LOG_PRIVATE_DATA))))
 * #endif	/\* INTERFACE *\/ */

/**
 * Set log level and privacy log to print.
 *
 * @param level                   - log level.
 * @param hidePrivateLogEntries   - Hide Private Log.
 */


#if EXPORT_INTERFACE
enum LogLevel{
    LL_DEBUG = 0,
    LL_INFO,
    LL_WARNING,
    LL_ERROR,
    LL_FATAL,
    LL_DEBUG_LITE,       // The DEBUG log for Lite device
    LL_INFO_LITE,        // The INFO log for Lite device
};
typedef LogLevel LogLevel;
#define DEBUG 0
#define INFO 1
#define WARNING 2
#ifdef _WIN32
#undef ERROR
#endif
#define ERROR 3
#define FATAL 4
#define DEBUG_LITE 5
#define INFO_LITE 6
/* static LogLevel mkhdrs_log_level; /\* help makeheaders *\/ */
#endif	/* EXPORT_INTERFACE */

enum LogLevel mkhdr_ll;		/* help makeheaders */

/**
 * Log level to print can be controlled through this enum.
 * And privacy logs contained uid, Token, Device id, etc can also disable.
 * This enum (OCLogLevel) must be kept synchronized with
 * CAUtilLogLevel_t (in CACommon.h).
 */
typedef enum
{
    OC_LOG_LEVEL_ALL = 1,             // all logs.
    OC_LOG_LEVEL_INFO,                // debug level is disabled.
} OCLogLevel;

FILE *logfile = NULL;
static char logfile_name[PATH_MAX] = "";

/* void oocf_log_hook_stdout(log_writer_t hook) */
/* void OCLogHookFd(FILE *fd) */
/* EXPORT */
/* { */
/*     logfile = fd; */
/*     printf("hooking logger\n"); */
/*     /\* hook("Hello %s\n", "world"); *\/ */
/* } */

// log level
static int g_level = DEBUG;
// private log messages are not logged unless they have been explicitly enabled by calling OCSetLogLevel().

static bool g_hidePrivateLogEntries = true;

oc_log_ctx_t *logCtx = 0;


/* #ifdef __ANDROID__ */
/* #elif defined(__linux__) || defined(__APPLE__) || defined(_WIN32) */
oc_log_level LEVEL_XTABLE[] = {OC_LOG_DEBUG, OC_LOG_INFO,
                                      OC_LOG_WARNING, OC_LOG_ERROR, OC_LOG_FATAL};
/* #endif */

/* // Convert LogLevel to platform-specific severity level.  Store in PROGMEM on Arduino */
/* #ifdef __ANDROID__ */
/* #ifdef ADB_SHELL */
/*     static const char *LEVEL[] = */
/*     {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"}; */

/* #else */
/*     static android_LogPriority LEVEL[] = */
/*     {ANDROID_LOG_DEBUG, ANDROID_LOG_INFO, ANDROID_LOG_WARN, ANDROID_LOG_ERROR, ANDROID_LOG_FATAL}; */
/* #endif */
/* #elif defined(__linux__) || defined(__APPLE__) || defined(__msys_nt__) */
/*     static const char * LEVEL[] __attribute__ ((unused)) = {"\e[0;32mDEBUG\033[0m", "\e[0;33mINFO\033[0m", "\e[0;35mWARNING\033[0m", "\e[0;31mERROR\033[0m", "\e[0;31mFATAL\033[0m"}; */
/* #elif defined(_MSC_VER) */
/*     static const char * LEVEL[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"}; */
/* #endif */
/*     /\* static const char *LEVEL[] __attribute__ ((unused)) = *\/ */
/*     /\* {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"}; *\/ */

/**
 * Checks if a message should be logged, based on its priority level, and removes
 * the OC_LOG_PRIVATE_DATA bit if the message should be logged.
 *
 * @param level[in] - One of DEBUG, INFO, WARNING, ERROR, or FATAL plus possibly the OC_LOG_PRIVATE_DATA bit
 *
 * @return true if the message should be logged, false otherwise
 */
bool AdjustAndVerifyLogLevel(int* level)
{
    int localLevel = *level;

    if (OC_LOG_PRIVATE_DATA & localLevel)
    {
        if (g_hidePrivateLogEntries)
        {
            return false;
        }

        localLevel &= ~OC_LOG_PRIVATE_DATA;
    }

    if (g_level > localLevel)
    {
        return false;
    }

    *level = localLevel;
    return true;
}

static void OCLogHexBuffer(int level, const char * tag, int line_nbr, const char * format, ...)
{
    /* oc_mutex_lock(log_mutex); */
    if (!format || !tag) {
	/* oc_mutex_unlock(log_mutex); */
        return;
    }

    if (!AdjustAndVerifyLogLevel(&level))
    {
	/* oc_mutex_unlock(log_mutex); */
        return;
    }

    char tagbuffer[MAX_LOG_V_BUFFER_SIZE] = {0};
    sprintf(tagbuffer, "0x%04X", line_nbr);

    char buffer[MAX_LOG_V_BUFFER_SIZE] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof buffer - 1, format, args);
    va_end(args);
    OCLog(level, tagbuffer, buffer);
    /* oc_mutex_unlock(log_mutex); */
}

/**
 * Output the contents of the specified buffer (in hex) with the specified priority level.
 *
 * @param level      - One of DEBUG, INFO, WARNING, ERROR, or FATAL plus possibly the OC_LOG_PRIVATE_DATA bit
 * @param tag        - Module name
 * @param buffer     - pointer to buffer of bytes
 * @param bufferSize - max number of byte in buffer
 */
void OCLogBuffer(int level, const char* tag, int line_number, const uint8_t* buffer, size_t bufferSize)
{
    oc_mutex_lock(log_mutex);

    if (!buffer || !tag || (bufferSize == 0))
    {
	oc_mutex_unlock(log_mutex);
        return;
    }

/* #ifdef _MSC_VER			/\* compiler is msvc cl.exe *\/ */
#ifdef _WIN32
    /* oc_mutex_lock(log_mutex); */
#else
    /* flockfile(logfile); */
#endif

    if (!AdjustAndVerifyLogLevel(&level))
    {
	oc_mutex_unlock(log_mutex);
        return;
    }

    // No idea why the static initialization won't work here, it seems the compiler is convinced
    // that this is a variable-sized object.
    static char lineBuffer[LINE_BUFFER_SIZE];
    memset(lineBuffer, 0, sizeof lineBuffer);
    size_t byte_index = 0;	/* 2 hex chars plus 1 space per byte */
    size_t line_index = 0;
    for (size_t i = 0; i < bufferSize; i++)
    {
        // Format the buffer data into a line
        snprintf(&lineBuffer[byte_index * 3], sizeof(lineBuffer) - byte_index * 3, "%02X ", buffer[i]);
        byte_index++;
        // Output 16 values per line
        if (((i + 1) % 16) == 0)
        {
            OCLogHexBuffer(level, "\t", line_index * 16, "%s", lineBuffer);
            memset(lineBuffer, 0, sizeof lineBuffer);
            byte_index = 0;
	    line_index++;
        }
    }
    // Output last values in the line, if any
    if (bufferSize % 16)
    {
        OCLogHexBuffer(level, "\t", line_index * 16, "%s", lineBuffer);
    }
    fflush(logfile);
#ifdef _WIN32
    /* oc_mutex_unlock(log_mutex); */
#else
    /* funlockfile(logfile); */
#endif
    oc_mutex_unlock(log_mutex);
}

void OCSetLogLevel(LogLevel level, bool hidePrivateLogEntries)
EXPORT
{
    g_level = level;
    g_hidePrivateLogEntries = hidePrivateLogEntries;
}

void OCLogConfig(oc_log_ctx_t *ctx)
{
    logCtx = ctx;
}

void OCLogInit(void) // FILE *fd)
{
    int fd = -1;

    strlcpy(logfile_name, "/tmp/openocf.XXXXXX", sizeof logfile_name);
    fd = mkstemp(logfile_name);
    if (fd == -1) {
        unlink(logfile_name);
        close(fd);
        fprintf(stderr, "Unable to create logfile %s: %s\n", logfile_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    logfile = fdopen(fd, "w+");
    if (logfile == NULL) {
        fprintf(stderr, "Unable to open logfile %s: %s\n", logfile_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* if (fd) */
    /*     logfile = fd; */
    /* else */
    /*     logfile = stdout; */
    if (NULL == log_mutex)
    {
	/* FIXME: oc_mutex_new uses oicmalloc, which uses OIC_LOG_V,
	   which uses log_mutex, which crashes if ENABLE_MALLOC_DEBUG */
        log_mutex = oc_mutex_new();
        logv_mutex = oc_mutex_new();
    }
}

char *oocf_get_logfile_name() EXPORT
{
    return logfile_name;
}

void OCLogShutdown(void)
{
    oc_mutex_free(log_mutex);
    oc_mutex_free(logv_mutex);
#if defined(__linux__) || defined(__APPLE__) || defined(_WIN32)
    if (logCtx && logCtx->destroy)
    {
        logCtx->destroy(logCtx);
    }
#endif
}

/**
 * Output a variable argument list log string with the specified priority level.
 * Only defined for Linux and Android
 *
 * @param level  - One of DEBUG, INFO, WARNING, ERROR, or FATAL plus possibly the OC_LOG_PRIVATE_DATA bit
 * @param tag    - Module name
 * @param format - variadic log string
 */
void OCLogv(int level, const char * tag, int line_nbr, const char * format, ...)
{
    oc_mutex_lock(logv_mutex);
    if (!format || !tag) {
	oc_mutex_unlock(logv_mutex);
        return;
    }

    if (!AdjustAndVerifyLogLevel(&level))
    {
	oc_mutex_unlock(logv_mutex);
        return;
    }
    static char tagbuffer[MAX_LOG_V_BUFFER_SIZE] = {0};
    sprintf(tagbuffer, "%s:%d", tag, line_nbr);

    static char buffer[MAX_LOG_V_BUFFER_SIZE] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer) - 1, format, args);
    va_end(args);
    OCLog(level, tagbuffer, buffer);
    oc_mutex_unlock(logv_mutex);
}

/**
 * Output a variable argument list log string with the specified priority level.
 * Log string may be of any length..
 *
 * @param level  - One of DEBUG, INFO, WARNING, ERROR, or FATAL plus possibly the OC_LOG_PRIVATE_DATA bit
 * @param tag    - Module name
 * @param format - variadic log string
 */
void OCLogStr(int level, const char * tag, int line_nbr, const char * header, const char * format, ...)
{
    oc_mutex_lock(log_mutex);
    if (!format || !tag) {
	oc_mutex_unlock(log_mutex);
        return;
    }

    if (!AdjustAndVerifyLogLevel(&level))
    {
	oc_mutex_unlock(log_mutex);
        return;
    }

    static char tagbuffer[MAX_LOG_V_BUFFER_SIZE] = {0};
    sprintf(tagbuffer, "%s:%d", tag, line_nbr);


    OCLog(level, tagbuffer, header);

    /* static char buffer[MAX_LOG_V_BUFFER_SIZE] = {0}; */
    va_list args;
    va_start(args, format);
    vfprintf(logfile, format, args);
    va_end(args);
    oc_mutex_unlock(log_mutex);
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
	    oc_mutex_lock(log_mutex); \
	    OCLog((level), (__FILE__ ":" TOSTRING(__LINE__)), (logStr));	\
	    oc_mutex_unlock(log_mutex); \
    } while(0)

// Define variable argument log function for Linux, Android, and Win32
#define OIC_LOG_V(level, tag, ...) \
    do { \
    if (((int)OC_MINIMUM_LOG_LEVEL) <= ((int)(level & (~OC_LOG_PRIVATE_DATA)))) \
	    OCLogv((level), __FILE__, __LINE__, __VA_ARGS__); \
    } while(0)

// Print a string of any length
#define OIC_LOG_STR(level, tag, ...) \
    do { \
    if (((int)OC_MINIMUM_LOG_LEVEL) <= ((int)(level & (~OC_LOG_PRIVATE_DATA)))) \
	    OCLogStr((level), __FILE__, __LINE__, __VA_ARGS__); \
    } while(0)
#endif // TB_LOG

#ifdef DEBUG_PAYLOAD
#define OIC_LOG_PAYLOAD_BUFFER OIC_LOG_BUFFER
#else
#define OIC_LOG_PAYLOAD_BUFFER(level, tag, buffer, bufferSize)
#endif

#ifdef DEBUG_THREADS
#define OIC_LOG_THREADS OIC_LOG
#define OIC_LOG_THREADS_V OIC_LOG_V
#else
#define OIC_LOG_THREADS(level, tag, logStr)
#define OIC_LOG_THREADS_V(level, tag, ...)
#endif

#ifdef DEBUG_TLS
#define OIC_LOG_TLS OIC_LOG
#define OIC_LOG_TLS_V OIC_LOG_V
#else
#define OIC_LOG_TLS(level, tag, logStr)
#define OIC_LOG_TLS_V(level, tag, ...)
#endif

#ifdef DEBUG_MSGS
#define OIC_LOG_MSGS OIC_LOG
#define OIC_LOG_MSGS_V OIC_LOG_V
#else
#define OIC_LOG_MSGS(level, tag, logStr)
#define OIC_LOG_MSGS_V(level, tag, ...)
#endif

#endif	/* EXPORT_INTERFACE */

// FIXME: this is a hack from spresource.c. get rid of it
void LogSp(OicSecSp_t* sp, int level, const char* tag, const char* msg)
{
    // some compilers not flagging the use of level and tag in the logging
    // macros as being used.  This is to get around these compiler warnings
    (void) level;
    (void) tag;
    (void) msg;

    if (NULL != msg)
    {
        OIC_LOG(level, tag, "-------------------------------------------------");
        OIC_LOG_V(level, tag, "%s", msg);
    }

    OIC_LOG(level, tag, "-------------------------------------------------");
    OIC_LOG_V(level, tag, "# security profiles supported: %lu", (unsigned long)sp->supportedLen);
    for (size_t i = 0; i < sp->supportedLen; i++)
    {
        OIC_LOG_V(level, tag, "  %lu: %s", (unsigned long)i, sp->supportedProfiles[i]);
    }
    OIC_LOG_V(level, tag, "Current security profile: %s", sp->currentProfile);
    OIC_LOG(level, tag, "-------------------------------------------------");
}

void LogDevAddr(OCDevAddr *devaddr)
{
    OIC_LOG_V(DEBUG, "DEVADDR", "devaddr.addr: %s", devaddr->addr);
    OIC_LOG_V(DEBUG, "DEVADDR", "devaddr.port: %d", devaddr->port);
    OIC_LOG_V(DEBUG, "DEVADDR", "devaddr.adapter: %x", devaddr->adapter);
    OIC_LOG_V(DEBUG, "DEVADDR", "devaddr.flags: %x", devaddr->flags);
    OIC_LOG_V(DEBUG, "DEVADDR", "devaddr.remoteId: %s", devaddr->remoteId);
}

void LogEndpoint(const CAEndpoint_t *ep)
{
    OIC_LOG_V(DEBUG, "Endpoint", "endpoint.addr: %s", ep->addr);
    OIC_LOG_V(DEBUG, "Endpoint", "endpoint.port: %d", ep->port);
    OIC_LOG_V(DEBUG, "Endpoint", "endpoint.adapter: %x", ep->adapter);
    OIC_LOG_V(DEBUG, "Endpoint", "endpoint.flags: %x", ep->flags);
    OIC_LOG_V(DEBUG, "Endpoint", "endpoint.ifindex: %d", ep->ifindex);
    OIC_LOG_V(DEBUG, "Endpoint", "endpoint.remoteId: %s", ep->remoteId);
}

void LogSecureEndpoint(const CASecureEndpoint_t *sep)
{
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.identity: %s", sep->identity.id);
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.userId: %s", sep->userId.id);
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.addr: %s", sep->endpoint.addr);
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.port: %d", sep->endpoint.port);
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.ifindex: %d", sep->endpoint.ifindex);
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.adapter: %x", sep->endpoint.adapter);
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.flags: 0x%04x", sep->endpoint.flags);
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.ipv6? %s", (sep->endpoint.flags & CA_IPV6)? "t":"f");
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.ipv4?: %s", (sep->endpoint.flags & CA_IPV4)? "t":"f");
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.mcast?: %s", (sep->endpoint.flags & CA_MULTICAST)? "t":"f");
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.secure?: %s", (sep->endpoint.flags & CA_SECURE)? "t":"f");
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.scope if?: %s", (sep->endpoint.flags & CA_SCOPE_INTERFACE)? "t":"f");
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.scope link?: %s",
              ((sep->endpoint.flags & CA_SCOPE_MASK) == CA_SCOPE_LINK)? "t":"f");
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.scope realm?: %s",
              ((sep->endpoint.flags & CA_SCOPE_MASK) == CA_SCOPE_REALM)? "t":"f");
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.scope site?: %s",
              ((sep->endpoint.flags & CA_SCOPE_MASK) == CA_SCOPE_SITE)? "t":"f");
    OIC_LOG_V(DEBUG, "SEndpoint", "sep.remoteId: %s", sep->endpoint.remoteId);
}

void log_coap_msg_code(const coap_pdu_t *pdu, CATransportAdapter_t transport)
// const CASecureEndpoint_t *origin_sep)
{
    if (transport & CA_ADAPTER_IP) {
        /* NOTE: libcoap incorrectly uses RESPONSE class, should be MSG CLASS (covers both requests and responses */
        uint8_t code = pdu->transport_hdr->udp.code & 0x2F;
        if ((0 == COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code))
                   &&
            (0 == (pdu->transport_hdr->udp.code & 0x2F))) {
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE CLASS = %d %s",
                      COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code), "EMPTY MSG");
        }
        if ((0 == COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code))
            &&
            (0 < (pdu->transport_hdr->udp.code & 0x2F))) {
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE CLASS = 0 %s", "REQUEST");
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE DETAIL = %d %s",
                      code,
                      (code == 1)? "GET"
                      :(code == 2)? "POST"
                      :(code == 3)? "PUT"
                      :(code == 2)? "DELETE"
                      :"UNKNOWN");
        }
        if (2 == COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code)) {
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE CLASS = 2 %s", "SUCCESS");
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE DETAIL = %d %s",
                      code,
                      (code == 1)? "CREATED"
                      :(code == 2)? "DELETED"
                      :(code == 3)? "VALID"
                      :(code == 4)? "CHANGED"
                      :(code == 5)? "CONTENT"
                      :"UNKNOWN");
        }
        if (4 == COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code)) {
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE CLASS = 4 %s", "CLIENT ERROR");
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE DETAIL = %d %s",
                      code,
                      (code == 0)? "BAD REQUEST"
                      :(code == 1)? "AUTHORIZED"
                      :(code == 2)? "BAD OPTIONS"
                      :(code == 3)? "FORBIDDEN"
                      :(code == 4)? "NOT FOUND"
                      :(code == 5)? "METHOD NOT ALLOWED"
                      :(code == 6)? "NOT ACCEPTABLE"
                      :(code == 12)? "PRECONDITION FAILED"
                      :(code == 13)? "REQUEST ENTITY TOO LARGE"
                      :(code == 16)? "UNSUPPORTED CONTENT_FORMAT"
                      :"UNKNOWN");
        }
        if (5 == COAP_RESPONSE_CLASS(pdu->transport_hdr->udp.code)) {
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE CLASS = 5 %s", "SERVER ERROR");
            OIC_LOG_V(DEBUG, TAG, "COAP MSG CODE DETAIL = %d %s",
                      code,
                      (code == 0)? "INTERNAL SERVER ERROR"
                      :(code == 1)? "NOT IMPLEMENTED"
                      :(code == 2)? "BAD GATEWAY"
                      :(code == 3)? "SERVICE UNAVAILABLE"
                      :(code == 4)? "GATEWAY TIMEOUT"
                      :(code == 5)? "PROXYING NOT SUPPORTED"
                      :"UNKNOWN");
        }
    }
}

#if INTERFACE
#include <coap/pdu.h>
#endif
void log_coap_pdu_hdr(const coap_pdu_t *pdu, CATransportAdapter_t transport)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);

    //coap_transport_t transport = COAP_UDP;
#ifdef WITH_TCP
    if (CAIsSupportedCoAPOverTCP(transport))
    {
        transport = coap_get_tcp_header_type_from_initbyte(((unsigned char *)pdu->transport_hdr)[0] >> 4);
    }
#endif

    OIC_LOG_V(DEBUG, TAG, "transport: %s",
              (transport == CA_ADAPTER_IP)? "UDP"
              :(transport == CA_ADAPTER_IP)? "TCP"
              :"OTHER");
    if (transport == CA_ADAPTER_IP) { // COAP_UDP) {
        OIC_LOG_V(DEBUG, TAG, "coap msg type: %u %s",
                  pdu->transport_hdr->udp.type,
                  (pdu->transport_hdr->udp.type == COAP_MESSAGE_CON)? "CONfirmable"
                  :(pdu->transport_hdr->udp.type == COAP_MESSAGE_NON)? "NON-confirmable"
                  :(pdu->transport_hdr->udp.type == COAP_MESSAGE_ACK)? "ACK"
                  :(pdu->transport_hdr->udp.type == COAP_MESSAGE_RST)? "RESET"
                  :"UNKNOWN");

        OIC_LOG_V(DEBUG, TAG, "coap msg id: %u", pdu->transport_hdr->udp.id);
        OIC_LOG_V(DEBUG, TAG, "coap msg token:");
        OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *)pdu->transport_hdr->udp.token,
                       pdu->transport_hdr->udp.token_length);
    }

    OIC_LOG_V(INFO, TAG, "%s EXIT", __func__);
}

/* CoAP Options */
char *get_coap_option_key_string(int i) EXPORT
//uint16_t id) EXPORT
{
    /* https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#option-numbers */
    switch (i) {
    case COAP_OPTION_IF_MATCH: return "If-Match"; /* 1 */
        break;
    case COAP_OPTION_URI_HOST: return "Uri-Host"; /* 3 */
        break;
    case COAP_OPTION_ETAG: return "ETag";        /* 4 */
        break;
    case COAP_OPTION_IF_NONE_MATCH: return "If-None-Match"; /* 5 */
        break;
    case COAP_OPTION_OBSERVE: return "Observe";  /* 6 */
        break;
    case COAP_OPTION_URI_PORT: return "Uri-Port"; /* 7 */
        break;
    case COAP_OPTION_LOCATION_PATH: return "Location-Path"; /* 8 */
        break;
    case COAP_OPTION_URI_PATH: return "Uri-Path"; /* 11 */
        break;
    case COAP_OPTION_CONTENT_FORMAT: return "Content-Format"; /* 12 */
        break;
    case COAP_OPTION_MAXAGE: return "Max-Age";   /* 14 */
        break;
    case COAP_OPTION_URI_QUERY: return "Uri-Query"; /* 15 */
        break;
    case COAP_OPTION_ACCEPT: return "Accept";    /* 17 */
        break;
    case COAP_OPTION_LOCATION_QUERY: return "Location-Query"; /* 20 */
        break;
    case COAP_OPTION_BLOCK2: return "Block2";    /* 23, RFC 7959, 8323 */
        break;
    case COAP_OPTION_BLOCK1: return "Block1";    /* 27, RFC 7959, 8323 */
        break;
    case COAP_OPTION_SIZE2: return "Size2";      /* 28, RFC 7959 */
        break;
    case COAP_OPTION_PROXY_URI: return "Proxy-Uri"; /* 35 */
        break;
    case COAP_OPTION_PROXY_SCHEME: return "Proxy-Scheme"; /* 39 */
        break;
    case COAP_OPTION_SIZE1: return "Size1";      /* 60 */
        break;
    case COAP_OPTION_NORESPONSE: return "No-Response"; /* 258, RFC 7967 */
        break;
    case OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION: return "OCF-Accept-Content-Format-Version"; /* 2049 */
        break;
    case OCF_OPTION_CONTENT_FORMAT_VERSION: return "OCF-Content-Format-Version"; /* 2053 */
        break;
    default: return "DEFAULT";
        break;
    }
}

char *log_coap_option(coap_option *option) EXPORT
//uint16_t id) EXPORT
{
    /* https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#option-numbers */
    switch (COAP_OPTION_KEY(*option)) {
    case COAP_OPTION_IF_MATCH:  /* 1 */
        OIC_LOG_V(DEBUG, "OPT", "option 1 If-Match, len %d: 0x%02x",
                  COAP_OPTION_LENGTH(*option), *COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_URI_HOST: /* 3 */
        OIC_LOG_V(DEBUG, "OPT", "option 3 Uri-Host, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_ETAG: /* 4 */
        OIC_LOG_V(DEBUG, "OPT", "option 4 ETag, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_IF_NONE_MATCH: /* 5 */
        OIC_LOG_V(DEBUG, "OPT", "option 5 If-None-Match, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_OBSERVE:   /* 6, uint */
        OIC_LOG_V(DEBUG, "OPT", "option 6 Observe, len %d: 0x%02x",
                  COAP_OPTION_LENGTH(*option), *COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_URI_PORT:  /* 7 */
        OIC_LOG_V(DEBUG, "OPT", "option 7 Uri-Port, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_LOCATION_PATH:  /* 8 */
        OIC_LOG_V(DEBUG, "OPT", "option 8 Location-Path, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_URI_PATH: /* 11 */
        OIC_LOG_V(DEBUG, "OPT", "option 11 Uri-Path, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_CONTENT_FORMAT: /* 12 */
        OIC_LOG_V(DEBUG, "OPT", "option 12 Content-Format, len %d: %u",
                  COAP_OPTION_LENGTH(*option),
                  ntohs( *((uint16_t*)COAP_OPTION_DATA(*option)))
                  );
        break;
    case COAP_OPTION_MAXAGE:   /* 14 */
        OIC_LOG_V(DEBUG, "OPT", "option 14 Max-Age, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_URI_QUERY: /* 15 */
        OIC_LOG_V(DEBUG, "OPT", "option 15 Uri-Query, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_ACCEPT:    /* 17 */
        OIC_LOG_V(DEBUG, "OPT", "option 17 Accept, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_LOCATION_QUERY: return "Location-Query"; /* 20 */
        break;
    case COAP_OPTION_BLOCK2:     /* 23, RFC 7959, 8323 */
        OIC_LOG_V(DEBUG, "OPT", "option 23 Block2, len %d: NUM %u, M: %1x, SZX %d",
                  COAP_OPTION_LENGTH(*option),
                  (*((uint8_t*)COAP_OPTION_DATA(*option)) >> 4),
                  (*((uint8_t*)COAP_OPTION_DATA(*option)) >> 3),
                  (0x7 & *((uint8_t*)COAP_OPTION_DATA(*option))));
        break;
    case COAP_OPTION_BLOCK1:    /* 27, RFC 7959, 8323 */
        OIC_LOG_V(DEBUG, "OPT", "option 27 Block1, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_SIZE2: /* 28, RFC 7959 */
        OIC_LOG_V(DEBUG, "OPT", "option 28 Size2, len %d: %u",
                  COAP_OPTION_LENGTH(*option),
                  ntohs( *((uint16_t*)COAP_OPTION_DATA(*option))));
        break;
    case COAP_OPTION_PROXY_URI:  /* 35 */
        OIC_LOG_V(DEBUG, "OPT", "option 35 Proxy-Uri, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_PROXY_SCHEME:  /* 39 */
        OIC_LOG_V(DEBUG, "OPT", "option 39 Proxy-Scheme, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_SIZE1: return "Size1";      /* 60 */
        OIC_LOG_V(DEBUG, "OPT", "option 60 Size1, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case COAP_OPTION_NORESPONSE: /* 258, RFC 7967 */
        OIC_LOG_V(DEBUG, "OPT", "option 258 No-Response, len %d: %s",
                  COAP_OPTION_LENGTH(*option), COAP_OPTION_DATA(*option));
        break;
    case OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION: /* 2049 */
        OIC_LOG_V(DEBUG, "OPT", "option 2049 OCF-Accept-Content-Format-Version, len %d: %u",
                  COAP_OPTION_LENGTH(*option),
                  ntohs( *((uint16_t*)COAP_OPTION_DATA(*option))));
        break;
    case OCF_OPTION_CONTENT_FORMAT_VERSION: /* 2053 */
        OIC_LOG_V(DEBUG, "OPT", "option 2053 OCF-Accept-Content-Format-Version, len %d: %u",
                  COAP_OPTION_LENGTH(*option),
                  ntohs( *((uint16_t*)COAP_OPTION_DATA(*option))));
        break;
    default:
        OIC_LOG_V(DEBUG, "OPT", "default");
        break;
    }
}

char *log_coap_option_string(struct oocf_coap_options options[], int i) EXPORT
//uint16_t id) EXPORT
{
    /* https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#option-numbers */
    switch (options[i].optionID) {
    case COAP_OPTION_IF_MATCH: return "If-Match"; /* 1 */
        break;
    case COAP_OPTION_URI_HOST: return "Uri-Host"; /* 3 */
        break;
    case COAP_OPTION_ETAG: return "ETag";        /* 4 */
        break;
    case COAP_OPTION_IF_NONE_MATCH: return "If-None-Match"; /* 5 */
        break;
    case COAP_OPTION_OBSERVE: return "Observe";  /* 6 */
        break;
    case COAP_OPTION_URI_PORT: return "Uri-Port"; /* 7 */
        break;
    case COAP_OPTION_LOCATION_PATH: return "Location-Path"; /* 8 */
        break;
    case COAP_OPTION_URI_PATH: return "Uri-Path"; /* 11 */
        break;
    case COAP_OPTION_CONTENT_FORMAT: return "Content-Format"; /* 12 */
        break;
    case COAP_OPTION_MAXAGE: return "Max-Age";   /* 14 */
        break;
    case COAP_OPTION_URI_QUERY: return "Uri-Query"; /* 15 */
        break;
    case COAP_OPTION_ACCEPT: return "Accept";    /* 17 */
        break;
    case COAP_OPTION_LOCATION_QUERY: return "Location-Query"; /* 20 */
        break;
    case COAP_OPTION_BLOCK2: return "Block2";    /* 23, RFC 7959, 8323 */
        break;
    case COAP_OPTION_BLOCK1: return "Block1";    /* 27, RFC 7959, 8323 */
        break;
    case COAP_OPTION_SIZE2: return "Size2";      /* 28, RFC 7959 */
        break;
    case COAP_OPTION_PROXY_URI: return "Proxy-Uri"; /* 35 */
        break;
    case COAP_OPTION_PROXY_SCHEME: return "Proxy-Scheme"; /* 39 */
        break;
    case COAP_OPTION_SIZE1: return "Size1";      /* 60 */
        break;
    case COAP_OPTION_NORESPONSE: return "No-Response"; /* 258, RFC 7967 */
        break;
    case OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION: return "OCF-Accept-Content-Format-Version"; /* 2049 */
        break;
    case OCF_OPTION_CONTENT_FORMAT_VERSION: return "OCF-Content-Format-Version"; /* 2053 */
        break;
    default: return "DEFAULT";
        break;
    }
}

char *log_coap_option_value_string(struct oocf_coap_options options[], int i) EXPORT
{
    /* https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#option-numbers */
    switch (options[i].optionID) {
    case COAP_OPTION_IF_MATCH: return "If-Match"; /* 1 */
        break;
    case COAP_OPTION_URI_HOST: return "Uri-Host"; /* 3 */
        break;
    case COAP_OPTION_ETAG: return "ETag";        /* 4 */
        break;
    case COAP_OPTION_IF_NONE_MATCH: return "If-None-Match"; /* 5 */
        break;
    case COAP_OPTION_OBSERVE: return "Observe";  /* 6 */
        break;
    case COAP_OPTION_URI_PORT: return "Uri-Port"; /* 7 */
        break;
    case COAP_OPTION_LOCATION_PATH: return "Location-Path"; /* 8 */
        break;
    case COAP_OPTION_URI_PATH: return "Uri-Path"; /* 11 */
        break;
    case COAP_OPTION_CONTENT_FORMAT: return "Content-Format"; /* 12 */
        break;
    case COAP_OPTION_MAXAGE: return "Max-Age";   /* 14 */
        break;
    case COAP_OPTION_URI_QUERY: return "Uri-Query"; /* 15 */
        break;
    case COAP_OPTION_ACCEPT: return "Accept";    /* 17 */
        break;
    case COAP_OPTION_LOCATION_QUERY: return "Location-Query"; /* 20 */
        break;
    case COAP_OPTION_BLOCK2: return "Block2";    /* 23, RFC 7959, 8323 */
        break;
    case COAP_OPTION_BLOCK1: return "Block1";    /* 27, RFC 7959, 8323 */
        break;
    case COAP_OPTION_SIZE2: return "Size2";      /* 28, RFC 7959 */
        break;
    case COAP_OPTION_PROXY_URI: return "Proxy-Uri"; /* 35 */
        break;
    case COAP_OPTION_PROXY_SCHEME: return "Proxy-Scheme"; /* 39 */
        break;
    case COAP_OPTION_SIZE1: return "Size1";      /* 60 */
        break;
    case COAP_OPTION_NORESPONSE: return "No-Response"; /* 258, RFC 7967 */
        break;
    case OCF_OPTION_ACCEPT_CONTENT_FORMAT_VERSION: return "OCF-Accept-Content-Format-Version"; /* 2049 */
        break;
    case OCF_OPTION_CONTENT_FORMAT_VERSION: return "OCF-Content-Format-Version"; /* 2053 */
        break;
    default: return "DEFAULT";
        break;
    }
}

void log_bwt_block_data(CABlockData_t *block_data)
{
    OIC_LOG_V(DEBUG, "BWT", "%s ENTRY", __func__);
    OIC_LOG_V(DEBUG, "BWT", "block1.num: %u", block_data->block1.num);
    OIC_LOG_V(DEBUG, "BWT", "block1.m: %u", block_data->block1.m);
    OIC_LOG_V(DEBUG, "BWT", "block1.szx: %u", block_data->block1.szx);
    OIC_LOG_V(DEBUG, "BWT", "block2.num: %u", block_data->block2.num);
    OIC_LOG_V(DEBUG, "BWT", "block2.m: %u", block_data->block2.m);
    OIC_LOG_V(DEBUG, "BWT", "block2.szx: %u", block_data->block2.szx);
    OIC_LOG_V(DEBUG, "BWT", "type: %u", block_data->type);
    OIC_LOG_V(DEBUG, "BWT", "body len: %u", block_data->bwt_body_length);
    OIC_LOG_V(DEBUG, "BWT", "body len so far: %u", block_data->bwt_body_partial_length);
}
