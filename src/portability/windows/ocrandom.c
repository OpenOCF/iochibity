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

#include "ocrandom.h"

/* #ifdef HAVE_FCNTL_H */
/* #include <fcntl.h> */
/* #endif */
/* #ifdef HAVE_UNISTD_H */
/* #include <unistd.h> */
/* #endif */
/* #ifdef HAVE_STDLIB_H */
/* #include <stdlib.h> */
/* #endif */
/* #ifdef HAVE_STRING_H */
/* #include <string.h> */
/* #elif defined(HAVE_STRINGS_H) */
/* #include <strings.h> */
/* #endif */
/* #if defined(__ANDROID__) */
/* #include <ctype.h> */
/* #endif */

#if EXPORT_INTERFACE
#include <winsock2.h>
#undef ERROR			/* defined by wingdi.h */
#include <windows.h>
#include <Bcrypt.h>
/* #endif */

/* /\* http://kirkshoop.blogspot.com/2011/09/ntstatus.html *\/ */
/* #define WIN32_NO_STATUS */
/* #include <windows.h> */
/* #undef WIN32_NO_STATUS */
/* #include <winternl.h> */
/* #include <ntstatus.h> */
#endif	/* EXPORT_INTERFACE */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

/* #if EXPORT_INTERFACE */
/* #include <stddef.h> */
/* #include <stdint.h> */
/* #include <stdbool.h> */

/*  /\* Number of bytes in a UUID. *\/ */
/* #define UUID_SIZE (16) */

/* /\* */
/*  * Size of a UUID string. */
/*  * IoTivity formats UUIDs as strings following RFC 4122, Section 3. */
/*  * For example, "f81d4fae-7dec-11d0-a765-00a0c91e6bf6". */
/*  * This requires 36 characters, plus one for the null terminator. */
/*  *\/ */
/* #define UUID_STRING_SIZE (37) */
/* #endif */
bool OCGetRandomBytes(uint8_t * output, size_t len)
{
    if ( (output == NULL) || (len == 0) )
    {
        return false;
    }

    /*
     * size_t may be 64 bits, but ULONG is always 32.
     * If len is larger than the maximum for ULONG, just fail.
     * It's unlikely anything ever will want to ask for this much randomness.
     */
    if (len > 0xFFFFFFFFULL)
    {
        OIC_LOG(FATAL, OCRANDOM_TAG, "Requested number of bytes too large for ULONG");
        assert(false);
        return false;
    }

    NTSTATUS status = BCryptGenRandom(NULL, output, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!BCRYPT_SUCCESS(status))
    {
        OIC_LOG_V(FATAL, OCRANDOM_TAG, "BCryptGenRandom failed (%X)!", status);
        assert(false);
        return false;
    }
    return true;
}
