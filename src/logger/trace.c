//******************************************************************
//
// Copyright 2017 Samsung Mobile Communications GmbH All Rights Reserved.
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

#include "trace.h"

/* #if (defined(__ANDROID__)) || (defined(__TIZEN__) && defined(OIC_SUPPORT_TIZEN_TRACE)) */

#if EXPORT_INTERFACE
#include <stddef.h>
#include <stdint.h>
#endif

#include <errno.h>

#include <stdio.h>

#if EXPORT_INTERFACE

#ifdef __ANDROID__
#define OIC_TRACE_BEGIN(MSG, ...)
#define OIC_TRACE_END()
#define OIC_TRACE_MARK(MSG, ...)
#define OIC_TRACE_BUFFER(MSG, BUF, SIZ)
// FIXME:
/* #define OIC_TRACE_BEGIN(MSG, ...) \ */
/*         oic_trace_begin("OIC:"#MSG, ##__VA_ARGS__) */
/* #define OIC_TRACE_END() \ */
/*         oic_trace_end() */
/* #define OIC_TRACE_MARK(MSG, ...) \ */
/*         oic_trace_begin("OIC:"#MSG, ##__VA_ARGS__), \ */
/*         oic_trace_end() */
/* #define OIC_TRACE_BUFFER(MSG, BUF, SIZ) \ */
/*         oic_trace_buffer(MSG, BUF, SIZ) */
#elif defined(__TIZEN__)
/* trace macro for Tizen. this will call ttrace api internally*/
#ifdef OIC_SUPPORT_TIZEN_TRACE
#include <ttrace.h>
/* ttrace api is available on tizen2.4 (or above) only */
# define OIC_TRACE_BEGIN(MSG, ...) \
        traceBegin(TTRACE_TAG_APP, "OIC:"#MSG, ##__VA_ARGS__)
# define OIC_TRACE_END() \
        traceEnd(TTRACE_TAG_APP)
# define OIC_TRACE_MARK(MSG, ...) \
        traceBegin(TTRACE_TAG_APP, "OIC:"#MSG, ##__VA_ARGS__), \
        traceEnd(TTRACE_TAG_APP)
# define OIC_TRACE_BUFFER(MSG, BUF, SIZ) \
        oic_trace_buffer(MSG, BUF, SIZ)
#else  /* tizen, not tizen_trace  */
 #define OIC_TRACE_BEGIN(MSG, ...)
 #define OIC_TRACE_END()
 #define OIC_TRACE_MARK(MSG, ...)
 #define OIC_TRACE_BUFFER(MSG, BUF, SIZ)
#endif
#else  /* not android, not tizen */
#define OIC_TRACE_BEGIN(MSG, ...)
#define OIC_TRACE_END()
#define OIC_TRACE_MARK(MSG, ...)
#define OIC_TRACE_BUFFER(MSG, BUF, SIZ)
#endif

#endif	/* INTERFACE */

#define MAX_BUFFER_SIZE 8
#define MAX_LINE_LEN ((MAX_BUFFER_SIZE) * 2) + 1

void CAUtilSetLogLevel(CAUtilLogLevel_t level, bool hidePrivateLogEntries)
{
    OIC_LOG(DEBUG, TAG, "CAUtilSetLogLevel");
    LogLevel logLevel = DEBUG;
    switch(level)
    {
        case CA_LOG_LEVEL_INFO:
            logLevel = INFO;
            break;
        case CA_LOG_LEVEL_ALL:
        default:
            logLevel = DEBUG;
            break;
    }

    OCSetLogLevel(logLevel, hidePrivateLogEntries);
}

void oic_trace_buffer(const char *name, const uint8_t * buffer, size_t bufferSize)
{
    if (!name || !buffer || (0 == bufferSize))
    {
        return;
    }

    char lineBuffer[MAX_LINE_LEN] = {0};
    size_t count = (MAX_BUFFER_SIZE > bufferSize) ? bufferSize : MAX_BUFFER_SIZE;
    size_t remainSize = MAX_LINE_LEN;
    int writtenSize = 0;
    char* buf = &lineBuffer[0];

    for (size_t i = 0; i < count; i++)
    {
        writtenSize = snprintf(buf, remainSize, "%02x", buffer[i]);
        if (2 != writtenSize)
        {
            break;
        }
        buf += writtenSize;
        remainSize -= 2;
    }

    OIC_TRACE_BEGIN("%s:%s", name, lineBuffer);
    OIC_TRACE_END();
}

#ifndef __TIZEN__

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#define FD_INITIAL_VALUE  -1
#define FD_NOT_EXIST    -2
#define MAX_BUF_SIZE    4096
#define MAX_TRACE_LEN   524
#define MAX_HEAD_LEN    8
#define MAX_TAIL_LEN    13
#define POS_LABEL_ST    ((MAX_TRACE_LEN - MAX_HEAD_LEN))
#define POS_LABEL_ED    ((MAX_TRACE_LEN - MAX_TAIL_LEN))

#define TAG "OIC_TRACER"

#ifdef __ANDROID__
/*
* Currently android api level 21 is used for building iotivity project.
* Since Atrace (aka. android trace) API has been provided by NDK above android api level 23,
* we use ftrace directly as workaround to Atrace API until android build level is upgraded
*/
int g_trace_marker_hd=FD_INITIAL_VALUE;

int oic_trace_init()
{
    OIC_LOG(INFO, TAG, "entering oic_trace_init");
    int mounts = -1;
    char buf[MAX_BUF_SIZE] = {0};
    ssize_t buflen = -1;
    char *line = NULL, *tmp1 = NULL, *path = NULL;

    if(g_trace_marker_hd == FD_INITIAL_VALUE)
    {
        mounts = open("/proc/mounts", O_RDONLY);
        if (mounts < 0)
        {
            OIC_LOG(INFO, TAG, "no /proc/mounts");
            return -1;
        }

        buflen = read(mounts, buf, sizeof(buf) - 1);
        close(mounts);

        if (buflen < 0)
        {
            OIC_LOG(INFO, TAG, "failed to read /proc/mounts");
            return -1;
        }

        line = strtok_r(buf, "\n", &tmp1);
        while (line)
        {
            char *tmp2 = NULL, *tmp_path = NULL, *fstype = NULL;
            /* "<dev> <mountpoint> <fs type> ..." */
            strtok_r(line, " ", &tmp2);
            tmp_path = strtok_r(NULL, " ", &tmp2);
            fstype = strtok_r(NULL, " ", &tmp2);

            if (strcmp(fstype, "debugfs") == 0)
            {
                path = tmp_path;
                break;
            }
            line = strtok_r(NULL, "\n", &tmp1);
        }

        if (NULL == path)
        {
            OIC_LOG(INFO, TAG,  "debugfs mountpoint not found");
            return -1;
        }

        snprintf(buf, sizeof(buf) - 1, "%s/tracing/trace_marker", path);
        g_trace_marker_hd = open(buf, O_WRONLY);
        if (g_trace_marker_hd < 0)
        {
            OIC_LOG_V(INFO, TAG, "failed to open trace_marker file: %s (%d)",
                      strerror(errno), errno);
            return -1;
        }
    }
    OIC_LOG_V(INFO, TAG,  "exit oic_trace_init with: %d", g_trace_marker_hd);
    return g_trace_marker_hd;
}

void oic_trace_begin(const char *name, ...)
{
    if (g_trace_marker_hd == FD_INITIAL_VALUE)
    {
        oic_trace_init();
    }

    if (g_trace_marker_hd > 0)
    {
        char buf[MAX_TRACE_LEN]={0};
        int len = MAX_HEAD_LEN, ret = 0;
        va_list ap;

        va_start(ap, name);
        snprintf(buf, MAX_TRACE_LEN, "B|%5d|", getpid());
        len += vsnprintf(buf + MAX_HEAD_LEN, POS_LABEL_ST, name, ap);
        va_end(ap);

        if (len > MAX_TRACE_LEN)
        {
            len = MAX_TRACE_LEN - 1;
        }

        ret = write(g_trace_marker_hd, buf, len);

        if (ret < 0)
        {
            OIC_LOG_V(INFO, TAG, "error writing, len: %d, ret: %d, errno: %d at oic_trace_begin",
                      len, ret, errno);
        }
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "oic_trace_begin: invalid fd: %d", g_trace_marker_hd);
    }
}

void oic_trace_end()
{
    if (FD_INITIAL_VALUE == g_trace_marker_hd)
    {
        oic_trace_init();
    }

    if (g_trace_marker_hd > 0)
    {
        int ret = 0, len = 1;
        char end = 'E';

        ret = write(g_trace_marker_hd, &end, len);
        if (ret < 0)
        {
            OIC_LOG_V(INFO, TAG, "error writing, len: %d, ret: %d, errno: %d at oic_trace_end",
                      len, ret, errno);
        }
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "oic_trace_end: invalid fd: %d", g_trace_marker_hd);
    }
}

#endif

#endif // #ifndef __TIZEN__
