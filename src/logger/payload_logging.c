
//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "payload_logging.h"
#include <inttypes.h>
#include <string.h> 		/* memcpy */

#if EXPORT_INTERFACE
#define UUID_SIZE (16)
#ifdef TB_LOG
    #define OIC_LOG_PAYLOAD(level, payload) OCPayloadLog((level),(payload))
#else
    #define OIC_LOG_PAYLOAD(level, payload)
#endif
#endif

#ifdef TB_LOG
INLINE_API void OCPayloadLogRepValues(LogLevel level, OCRepPayloadValue* val)
{
    while (val)
    {
        switch(val->type)
        {
            case OCREP_PROP_NULL:
                OIC_LOG_V(level, PL_TAG, "\t\t%s: NULL", val->name);
                break;
            case OCREP_PROP_INT:
                OIC_LOG_V(level, PL_TAG, "\t\t%s(int):%" PRId64, val->name, val->i);
                break;
            case OCREP_PROP_DOUBLE:
                OIC_LOG_V(level, PL_TAG, "\t\t%s(double):%f", val->name, val->d);
                break;
            case OCREP_PROP_BOOL:
                OIC_LOG_V(level, PL_TAG, "\t\t%s(bool):%s", val->name, val->b ? "true" : "false");
                break;
            case OCREP_PROP_STRING:
                OIC_LOG_V(level, PL_TAG, "\t\t%s(string):%s", val->name, val->str);
                break;
            case OCREP_PROP_BYTE_STRING:
                OIC_LOG_V(level, PL_TAG, "\t\t%s(binary):", val->name);
                OIC_LOG_BUFFER(level, PL_TAG, val->ocByteStr.bytes, val->ocByteStr.len);
                break;
            case OCREP_PROP_OBJECT:
                // Note: Only prints the URI (if available), to print further, you'll
                // need to dig into the object better!
                OIC_LOG_V(level, PL_TAG, "\t\t%s(object):", val->name);
                OCPayloadLogRep(level, val->obj);
                break;
            case OCREP_PROP_ARRAY:
                switch(val->arr.type)
                {
                    case OCREP_PROP_INT:
                        OIC_LOG_V(level, PL_TAG, "\t\t%s(int array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OIC_LOG(level, PL_TAG, "\t\t Values:");
                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OIC_LOG_V(level, PL_TAG, "\t\t\t %" PRId64, val->arr.iArray[i]);
                        }
                        break;
                    case OCREP_PROP_DOUBLE:
                        OIC_LOG_V(level, PL_TAG, "\t\t%s(double array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OIC_LOG(level, PL_TAG, "\t\t Values:");
                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OIC_LOG_V(level, PL_TAG, "\t\t\t %lf", val->arr.dArray[i]);
                        }
                        break;
                    case OCREP_PROP_BOOL:
                        OIC_LOG_V(level, PL_TAG, "\t\t%s(bool array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OIC_LOG(level, PL_TAG, "\t\t Values:");
                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OIC_LOG_V(level, PL_TAG, "\t\t\t %d", val->arr.bArray[i]);
                        }
                        break;
                    case OCREP_PROP_STRING:
                        OIC_LOG_V(level, PL_TAG, "\t\t%s(string array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OIC_LOG(level, PL_TAG, "\t\t Values:");
                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OIC_LOG_V(level, PL_TAG, "\t\t\t %s", val->arr.strArray[i]);
                        }
                        break;
                    case OCREP_PROP_BYTE_STRING:
                        OIC_LOG_V(level, PL_TAG, "\t\t%s(byte array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OIC_LOG(level, PL_TAG, "\t\t Values:");
                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OIC_LOG_BUFFER(level, PL_TAG, val->arr.ocByteStrArray[i].bytes, val->arr.ocByteStrArray[i].len);
                        }
                        break;
                    case OCREP_PROP_OBJECT:
                        OIC_LOG_V(level, PL_TAG, "\t\t%s(object array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OIC_LOG(level, PL_TAG, "\t\t Values:");

                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OCPayloadLogRep(level, val->arr.objArray[i]);
                        }
                        break;
                    case OCREP_PROP_ARRAY: //Seems as nested arrays doesn't not supported in API
                    default:
                        OIC_LOG_V(ERROR, PL_TAG, "%s <-- Unknown/unsupported array type!",
                                val->name);
                        break;
                }
                break;
            default:
                OIC_LOG_V(ERROR, PL_TAG, "%s <-- Unknown type!", val->name);
                break;
        }
        val = val -> next;
    }
}

INLINE_API void OCPayloadLogRep(LogLevel level, OCRepPayload* payload)
{
    OIC_LOG(level, (PL_TAG), "Payload Type: Representation");
    uint32_t i = 1;
    for (OCRepPayload* rep = payload; rep; rep = rep->next, ++i)
    {
        OIC_LOG_V(level, PL_TAG, "\tResource #%d", i);
        if (rep->uri)
        {
            OIC_LOG_V(level, PL_TAG, "\tURI:%s", rep->uri);
        }
        if (rep->types)
        {
            OIC_LOG(level, PL_TAG, "\tResource Types:");
            for (OCStringLL* strll = rep->types; strll; strll = strll->next)
            {
                OIC_LOG_V(level, PL_TAG, "\t\t%s", strll->value);
            }
        }
        if (rep->interfaces)
        {
            OIC_LOG(level, PL_TAG, "\tInterfaces:");
            for (OCStringLL* strll = rep->interfaces; strll; strll = strll->next)
            {
                OIC_LOG_V(level, PL_TAG, "\t\t%s", strll->value);
            }
        }
        OIC_LOG(level, PL_TAG, "\tValues:");
        OCPayloadLogRepValues(level, rep->values);
    }
}

static void OCStringLLPrint(LogLevel level, OCStringLL *type)
{
    for (OCStringLL *strll = type; strll; strll = strll->next)
    {
        OIC_LOG_V(level, PL_TAG, "\t\t %s", strll->value);
    }
}

INLINE_API void OCPayloadLogDiscovery(LogLevel level, OCDiscoveryPayload* payload)
{
    OIC_LOG(level, PL_TAG, "Payload Type: Discovery");

    while(payload && payload->resources)
    {
        OIC_LOG_V(level, PL_TAG, "\tDI: %s", payload->sid);
        if (payload->name)
        {
            OIC_LOG_V(level, PL_TAG, "\tNAME: %s", payload->name);
        }

        if (payload->type)
        {
            OIC_LOG(level, PL_TAG, "\tResource Type:");
            OCStringLLPrint(level, payload->type);
        }

        if (payload->iface)
        {
            OIC_LOG(level, PL_TAG, "\tInterface:");
            OCStringLLPrint(level, payload->iface);
        }

        OCResourcePayload* res = payload->resources;

        uint32_t i = 1;
        while(res)
        {
            OIC_LOG_V(level, PL_TAG, "\tLink#%d", i);
            OIC_LOG_V(level, PL_TAG, "\tURI:%s", res->uri);
            if (res->rel)
            {
                OIC_LOG_V(level, PL_TAG, "\tRelation:%s", res->rel);
            }
            if (res->anchor)
            {
                OIC_LOG_V(level, PL_TAG, "\tAnchor:%s", res->anchor);
            }
            OIC_LOG(level, PL_TAG, "\tResource Types:");
            OCStringLL* strll =  res->types;
            while(strll)
            {
                OIC_LOG_V(level, PL_TAG, "\t\t%s", strll->value);
                strll = strll->next;
            }
            OIC_LOG(level, PL_TAG, "\tInterfaces:");
            strll =  res->interfaces;
            while(strll)
            {
                OIC_LOG_V(level, PL_TAG, "\t\t%s", strll->value);
                strll = strll->next;
            }

            OIC_LOG_V(level, PL_TAG, "\tBitmap: %u", res->bitmap);
            OIC_LOG_V(level, PL_TAG, "\tSecure?: %s", res->secure ? "true" : "false");
            OIC_LOG_V(level, PL_TAG, "\tPort: %u", res->port);

            uint32_t j = 1;
            OCEndpointPayload* eps = res->eps;
            while (eps)
            {
                OIC_LOG_V(level, PL_TAG, "\tEndpoint #%d", j);
                OIC_LOG_V(level, PL_TAG, "\t\ttps: %s", eps->tps);
                OIC_LOG_V(level, PL_TAG, "\t\taddr: %s", eps->addr);
                OIC_LOG_V(level, PL_TAG, "\t\tport: %d", eps->port);
                OIC_LOG_V(level, PL_TAG, "\t\tpri: %d", eps->pri);
                eps = eps->next;
                ++j;
            }

            OIC_LOG(level, PL_TAG, "");
            res = res->next;
            ++i;
        }
        payload = payload->next;
    }
}

#ifdef WITH_PRESENCE
INLINE_API void OCPayloadLogPresence(LogLevel level, OCPresencePayload* payload)
{
    OIC_LOG(level, PL_TAG, "Payload Type: Presence");
    OIC_LOG_V(level, PL_TAG, "\tSequence Number:%u", payload->sequenceNumber);
    OIC_LOG_V(level, PL_TAG, "\tMax Age:%d", payload->maxAge);
    OIC_LOG_V(level, PL_TAG, "\tTrigger:%s", convertTriggerEnumToString(payload->trigger));
    OIC_LOG_V(level, PL_TAG, "\tResource Type:%s", payload->resourceType);
}
#endif

INLINE_API void OCPayloadLogSecurity(LogLevel level, OCSecurityPayload* payload)
{
    size_t payloadSize = payload->payloadSize;
    OIC_LOG(level, PL_TAG, "Payload Type: Security");

    if (payloadSize > 0)
    {
        // Add a zero-character string terminator.
        char *securityData = (char *)OICMalloc(payloadSize + 1);

        if (securityData)
        {
            memcpy(securityData, payload->securityData, payloadSize);
            // assert(securityData[payloadSize - 1] != '\0');
            securityData[payloadSize] = '\0';
            OIC_LOG_V(level, PL_TAG, "\tSecurity Data: %s", securityData);
            OICFree(securityData);
        }
    }
}

void OCPayloadLog(LogLevel level, OCPayload* payload) EXPORT
{
    if(!payload)
    {
        OIC_LOG(level, PL_TAG, "NULL Payload");
        return;
    }
    switch(payload->type)
    {
        case PAYLOAD_TYPE_REPRESENTATION:
            OCPayloadLogRep(level, (OCRepPayload*)payload);
            break;
        case PAYLOAD_TYPE_DISCOVERY:
            OCPayloadLogDiscovery(level, (OCDiscoveryPayload*)payload);
            break;
#ifdef WITH_PRESENCE
        case PAYLOAD_TYPE_PRESENCE:
            OCPayloadLogPresence(level, (OCPresencePayload*)payload);
            break;
#endif
        case PAYLOAD_TYPE_SECURITY:
            OCPayloadLogSecurity(level, (OCSecurityPayload*)payload);
            break;
        default:
            OIC_LOG_V(level, PL_TAG, "Unknown Payload Type: %d", payload->type);
            break;
    }
}
#endif
