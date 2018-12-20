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

#include "ocrandom_posix.h"

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

#if EXPORT_INTERFACE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#endif

/**
* @def OCRANDOM_TAG
* @brief Logging tag for module name
*/
#define OCRANDOM_TAG "OIC_OCRANDOM"


bool OCGetRandomBytes(uint8_t * output, size_t len)
{
    if ( (output == NULL) || (len == 0) )
    {
        return false;
    }

    FILE* urandom = fopen("/dev/urandom", "r");
    if (urandom == NULL)
    {
        OIC_LOG(FATAL, OCRANDOM_TAG, "Failed open /dev/urandom!");
        assert(false);
        return false;
    }

    if (fread(output, sizeof(uint8_t), len, urandom) != len)
    {
        OIC_LOG(FATAL, OCRANDOM_TAG, "Failed while reading /dev/urandom!");
        assert(false);
        fclose(urandom);
        return false;
    }
    fclose(urandom);

    return true;
}
