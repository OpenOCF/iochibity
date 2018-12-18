/******************************************************************
*
* Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
*
*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include "ocrandom_windows.h"

#include <winsock2.h>
#include <windows.h>
#include <Bcrypt.h>

/* /\* http://kirkshoop.blogspot.com/2011/09/ntstatus.html *\/ */
/* #define WIN32_NO_STATUS */
/* #include <windows.h> */
/* #undef WIN32_NO_STATUS */
/* #include <winternl.h> */
/* #include <ntstatus.h> */


#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

#if INTERFACE
#include <stdint.h>
#endif
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
