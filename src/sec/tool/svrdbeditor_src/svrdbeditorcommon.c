/* *****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/

#include <stdio.h>
#include <string.h>

#include "utlist.h"
#include "oic_malloc.h"
#include "oic_string.h"

#include "srmresourcestrings.h"
#include "experimental/securevirtualresourcetypes.h"
#include "srmutility.h"

#include "svrdbeditorcommon.h"

#define STR_UUID_LENGTH (UUID_LENGTH * 2 + 4 + 1) // length + dash length  + '\0'
#define STR_UUID_ZERO "0"

#define BOLD_BEGIN    "\033[1m"
#define RED_BEGIN      "\033[1;31m"
#define YELLOW_BEGIN  "\033[1;33m"
#define CYAN_BEGIN  "\033[1;36m"
#define GREEN_BEGIN  "\033[1;92m"
#define COLOR_END      "\033[0m"
#define COLOR_END_NL      "\033[0m\n"

#define SVR_MAX_ENTITY (16)

#define SVR_DB_PATH_LENGTH (1024)
#define PRINT_ERR(fmt,...) printf(RED_BEGIN "error: " fmt COLOR_END_NL, ##__VA_ARGS__)
#define PRINT_WARN(fmt,...) printf(YELLOW_BEGIN "warning : " fmt COLOR_END_NL, ##__VA_ARGS__)
#define PRINT_INFO(fmt,...) printf(YELLOW_BEGIN fmt COLOR_END_NL, ##__VA_ARGS__)
#define PRINT_PROG(fmt,...) printf(BOLD_BEGIN fmt COLOR_END, ##__VA_ARGS__)
#define PRINT_DATA(fmt,...) printf(CYAN_BEGIN fmt COLOR_END, ##__VA_ARGS__)
#define PRINT_NORMAL(fmt,...) printf(fmt, ##__VA_ARGS__)
#define PRINT_NL() printf("\n");

typedef enum SubOperationType
{
    SVR_PRINT = 1,
    SVR_ADD = 2,
    SVR_REMOVE = 3,
    SVR_MODIFY = 4,
    SVR_EDIT_IDX_SIZE = 5,
    BACK = 99
} SubOperationType_t;

void PrintUuid(const OicUuid_t *uuid)
{
    char *strUuid = NULL;
    if (OC_STACK_OK == ConvertUuidToStr(uuid, &strUuid))
    {
        PRINT_DATA("%s\n", strUuid);
        OICFree(strUuid);
    }
    else
    {
        PRINT_ERR("Can not convert UUID to String");
    }
}

void PrintIntArray(const int *array, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        PRINT_DATA("%d ", array[i]);
    }
    PRINT_NL();
}

void PrintStringArray(const char **array, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        PRINT_DATA("%s ", array[i]);
    }
    PRINT_NL();
}

void PrintInt(int value)
{
    PRINT_DATA("%d\n", value);
}

void PrintString(const char *text)
{
    PRINT_DATA("%s\n", text);
}

void PrintBuffer(const uint8_t *buf, size_t bufLen)
{
    size_t i = 0;

    for (i = 0; i < bufLen; i++)
    {
        if (0 == (i + 1) % 20 || bufLen - 1 == i)
        {
            PRINT_DATA("%02X \n", buf[i]);
        }
        else
        {
            PRINT_DATA("%02X ", buf[i]);
        }
    }
}

int InputNumber(const char *infoText)
{
    int inputValue = 0;

    PRINT_PROG("%s", infoText);
    for (int ret = 0; 1 != ret; )
    {
        ret = scanf("%d", &inputValue);
        for ( ; 0x20 <= getchar(); ); // for removing overflow garbages
        // '0x20<=code' is character region
    }

    return inputValue;
}

char *InputString(const char *infoText)
{
    char tmpStr[SVR_DB_PATH_LENGTH] = {0};

    PRINT_PROG("%s", infoText);
    for (int ret = 0; 1 != ret; )
    {
        ret = scanf("%1024s", tmpStr);
        for ( ; 0x20 <= getchar(); ); // for removing overflow garbages
        // '0x20<=code' is character region
    }

    return OICStrdup(tmpStr);
}

int InputUuid(OicUuid_t *uuid)
{
    OCStackResult ocResult = OC_STACK_ERROR;
    char strUuid[STR_UUID_LENGTH] = {0};
    size_t strLen = 0;

    if (NULL == uuid)
    {
        PRINT_ERR("Invalid parameter");
        return -1;
    }

    if (NULL == fgets(strUuid, STR_UUID_LENGTH, stdin))
    {
        PRINT_ERR("Failed fgets");
        return -1;
    }
    strLen = strlen(strUuid);
    if ('\n' == strUuid[strLen - 1])
    {
        strUuid[strLen - 1] = '\0';
    }

    if (0 == strncmp(strUuid, STR_UUID_ZERO, sizeof(STR_UUID_ZERO)))
    {
        memset(uuid->id, 0x00, sizeof(uuid->id));
    }
    else if (0 == strncmp(strUuid, (char *)WILDCARD_SUBJECT_ID.id, sizeof(WILDCARD_SUBJECT_ID.id)))
    {
        memset(uuid->id, 0x00, sizeof(uuid->id));
        memcpy(uuid->id, WILDCARD_SUBJECT_ID.id, WILDCARD_SUBJECT_ID_LEN);
    }
    else
    {
        ocResult = ConvertStrToUuid(strUuid, uuid);
        if (OC_STACK_OK != ocResult)
        {
            PRINT_ERR("Failed ConvertStrToUuid");
            return -1;
        }
    }

    return 0;
}

