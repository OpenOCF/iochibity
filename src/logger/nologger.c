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

#include "nologger.h"

#if EXPORT_INTERFACE

#define OIC_LOG_PAYLOAD(level, payload)

#define OIC_LOG_CONFIG(ctx)
#define OIC_LOG_SHUTDOWN()
#define OIC_LOG(level, tag, logStr)
#define OIC_LOG_V(level, tag, ...)
#define OIC_LOG_STR(level, tag, ...)
#define OIC_LOG_BUFFER(level, tag, buffer, bufferSize)
#define OIC_LOG_CA_BUFFER(level, tag, buffer, bufferSize, isHeader)
#define OIC_LOG_INIT()

#define OIC_LOG_THREADS(level, tag, logStr)
#define OIC_LOG_THREADS_V(level, tag, ...)
#define OIC_LOG_TLS(level, tag, logStr)
#define OIC_LOG_TLS_V(level, tag, ...)
#define OIC_LOG_MSGS(level, tag, logStr)
#define OIC_LOG_MSGS_V(level, tag, ...)

#define OIC_TRACE_BEGIN(MSG, ...)
#define OIC_TRACE_END()
#define OIC_TRACE_MARK(MSG, ...)
#define OIC_TRACE_BUFFER(MSG, BUF, SIZ)

#define OIC_LOG_ACL(level, acl)
#define OIC_LOG_ACE(level, ace)

#define OIC_LOG_CRL(level, crl)

#include <stdio.h>
#endif	/* EXPORT_INTERFACE */

void OCLogInit(FILE *fd) EXPORT {}

// void LogSp(OicSecSp_t* sp, int level, const char* tag, const char* msg) {}

/* log_svrs.c nulls */
void logCredMetadata(void) {}
void LogCert(uint8_t *data, size_t len, OicEncodingType_t encoding, const char* tag) {}
void LogCred(OicSecCred_t *cred, const char* tag) {}
void LogCredResource(OicSecCred_t *cred, const char* tag, const char* label) {}
void LogCurrrentCredResource(void) {}
