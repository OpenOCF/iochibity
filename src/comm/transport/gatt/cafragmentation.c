/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
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
 ******************************************************************/

#include "cafragmentation.h"

#include <string.h>
#include <math.h>

/* #include "cacommon.h"
 * #include "caadapterutils.h"
 * #include "cafragmentation.h"
 * #include "caleinterface.h" */

/**
 * Debugging tag for fragmentation module.
 */
#define TAG "OIC_CA_FRAG"

/**
 * The maximum port value for BLE packet format
 */
#define CA_SUPPORTED_BLE_MAX_PORT  127

/**
 * The minimum port value for BLE packet format
 */
#define CA_SUPPORTED_BLE_MIN_PORT  1

/**
 * The multicaset packet remote port value
 */
#define CA_BLE_MULTICAST_PORT  0

/**
 * The header size for ble fragmentation.
 * Specific header descriptions are below.
 */
#define CA_BLE_HEADER_SIZE 2

/**
 * The length header size for ble fragmentation.
 * Length header is embedded in first packet of entire CoAP PDU.
 */
#define CA_BLE_LENGTH_HEADER_SIZE 4

/**
 * Current Header version.
 */
#define HEADER_VERSION 1

/**
 * This enum value is used to make the CA BLE packet header.
 * 1st bit is used to check whether the packet is start packet or not.
 * Start packet should be marked ad CA_BLE_PACKET_START(1) and any other
 * packet is marked as CA_BLE_PACKET_NOT_START(0).
 */
typedef enum {
    CA_BLE_PACKET_NOT_START     = 0,
    CA_BLE_PACKET_START         = 1
} CABLEPacketStart_t;

/**
 * This enum value is used to make the CA BLE packet header.
 * 9th bit is uesd to check the packet use secure logic(dtls) or not.
 * Secure packet should be marking CA_BLE_PACKET_SECURE(1) and other
 * packet is makred CA_BLE_PACKET_NON_SECURE(0).
 */
typedef enum {
    CA_BLE_PACKET_NON_SECURE    = 0,
    CA_BLE_PACKET_SECURE        = 1
} CABLEPacketSecure_t;

//packet format define value
#define CA_BLE_START_POS 7
#define CA_BLE_START_LEN 1
#define CA_BLE_SOURCE_PORT_POS 6
#define CA_BLE_SOURCE_PORT_LEN 7
#define CA_BLE_SECURE_POS 7
#define CA_BLE_SECURE_LEN 1
#define CA_BLE_DESTINATION_PORT_POS 6
#define CA_BLE_DESTINATION_PORT_LEN 7

/**
 * This function is used to set value in specific bit position.
 *
 * @param[out]  x  Pointer to the octet variable that will contain
 *                 value in specific bit position.
 * @param[in]   p  Position which need to embed specific bits(0~7).
 * @param[in]   n  Length to fill several bits from the position.
 * @param[in]   v  Value to contain in specific bit position.
 */
static void CASetBits(uint8_t *x, unsigned p, unsigned n, unsigned v)
{
    if ((p+1) < n)
    {
        OIC_LOG(ERROR, TAG, "set bits - lower err");
        return;
    }
    else if(~(unsigned)(~0u<<n) < v)
    {
        OIC_LOG(ERROR, TAG, "set bits - upper err");
        return;
    }
    *x = (*x & (~(~0u << (p-n+1)))) | (*x & (~0u << (p+1))) | ((v & ~(~0u << n)) << (p-n+1));
}

/**
 * This function is used to get value in specific bit position.
 *
 * @param[in]   x  Pointer to the octet variable that have infomation
 *                 to be extracted.
 * @param[in]   p  Position which need to get specific bits(0~7).
 * @param[in]   n  Length to get several bits from the position.
 *
 * @return @c Extracted value from specific position.
 */
static uint8_t CAGetBits(uint8_t x, unsigned p, unsigned n)
{
    return (x >> (p + 1 - n)) & ~(~0u << n);
}

CAResult_t CAGenerateVariableForFragmentation(size_t dataLength,
                                              uint32_t *midPacketCount,
                                              size_t *remainingLen,
                                              size_t *totalLength,
                                              uint16_t mtuSize)
{
    OIC_LOG_V(DEBUG, TAG, "IN, dataLength = %" PRIuPTR ", mtu = %u",
              dataLength, (uint32_t)mtuSize);

    size_t remainDataSize = 0;
    size_t dataOnlyLen =
            mtuSize - (CA_BLE_HEADER_SIZE + CA_BLE_LENGTH_HEADER_SIZE);
    //total data size is smaller than 14 byte case.
    if (dataLength < dataOnlyLen)
    {
        remainDataSize = 0;
    }
    else
    {
        remainDataSize = dataLength - dataOnlyLen;
    }

    if (mtuSize - CA_BLE_HEADER_SIZE <= 0)
    {
        OIC_LOG_V(ERROR, TAG, "BLE header size shouldn't be bigger than BLE MTU size.");
        return CA_STATUS_FAILED;
    }

    *midPacketCount = (uint32_t)remainDataSize / (mtuSize - CA_BLE_HEADER_SIZE);
    *remainingLen = (uint32_t)remainDataSize % (mtuSize - CA_BLE_HEADER_SIZE);
    uint32_t remainHeaderSize = CA_BLE_HEADER_SIZE * (*midPacketCount + (*remainingLen == 0 ? 0:1));
    *totalLength = dataLength + (CA_BLE_HEADER_SIZE + CA_BLE_LENGTH_HEADER_SIZE) + remainHeaderSize;

    OIC_LOG(DEBUG, TAG, "OUT");

    return CA_STATUS_OK;
}

CAResult_t CAGenerateHeader(uint8_t *header,
                            CABLEPacketStart_t type,
                            const uint8_t sourcePort,
                            CABLEPacketSecure_t secure,
                            const uint8_t destPort)
{
    VERIFY_NON_NULL_MSG(header, TAG, "header is NULL");

    if (sourcePort > CA_SUPPORTED_BLE_MAX_PORT ||
        sourcePort < CA_SUPPORTED_BLE_MIN_PORT ||
        destPort > CA_SUPPORTED_BLE_MAX_PORT)
    {
        OIC_LOG_V(ERROR, TAG, "source port(%d) or destination port(%d) is invalid number!!",
                sourcePort, destPort);
        return CA_STATUS_FAILED;
    }

    CASetBits(&header[0], CA_BLE_START_POS, CA_BLE_START_LEN, type);
    CASetBits(&header[0], CA_BLE_SOURCE_PORT_POS, CA_BLE_SOURCE_PORT_LEN, sourcePort);
    CASetBits(&header[1], CA_BLE_SECURE_POS, CA_BLE_SECURE_LEN, secure);
    CASetBits(&header[1], CA_BLE_DESTINATION_PORT_POS, CA_BLE_DESTINATION_PORT_LEN, destPort);

    return CA_STATUS_OK;
}

CAResult_t CAGenerateHeaderPayloadLength(uint8_t *header,
                                         size_t headerLength,
                                         size_t dataLength)
{
    VERIFY_NON_NULL_MSG(header, TAG, "header is NULL");

    if (headerLength != CA_BLE_LENGTH_HEADER_SIZE)
    {
        return CA_STATUS_FAILED;
    }

    for(size_t idx = 1; idx < CA_BLE_LENGTH_HEADER_SIZE; idx++)
    {
        header[CA_BLE_LENGTH_HEADER_SIZE - idx] = dataLength & 0xFF;
        dataLength >>= 8;
    }
    header[0] = dataLength & 0xFF;

    return CA_STATUS_OK;
}

CAResult_t CAMakeFirstDataSegment(uint8_t *dataSegment,
                                  const uint8_t *data,
                                  const uint32_t dataLength,
                                  const uint8_t *dataHeader,
                                  const uint8_t *lengthHeader)
{
    VERIFY_NON_NULL_MSG(dataSegment, TAG, "dataSegment is NULL");
    VERIFY_NON_NULL_MSG(dataHeader, TAG, "dataHeader is NULL");
    VERIFY_NON_NULL_MSG(lengthHeader, TAG, "lengthHeader is NULL");

    memcpy(dataSegment, dataHeader, CA_BLE_HEADER_SIZE);
    memcpy(dataSegment + CA_BLE_HEADER_SIZE, lengthHeader, CA_BLE_LENGTH_HEADER_SIZE);
    memcpy(dataSegment + CA_BLE_HEADER_SIZE + CA_BLE_LENGTH_HEADER_SIZE, data, dataLength);
    return CA_STATUS_OK;
}

CAResult_t CAMakeRemainDataSegment(uint8_t *dataSegment,
                                   const uint32_t segmentPayloadLength,
                                   const uint8_t *sourceData,
                                   const uint32_t sourceDataLength,
                                   const uint32_t segmentNum,
                                   const uint8_t *dataHeader,
                                   uint16_t mtuSize)
{
    VERIFY_NON_NULL_MSG(dataSegment, TAG, "dataSegment is NULL");
    VERIFY_NON_NULL_MSG(dataHeader, TAG, "dataHeader is NULL");

    uint32_t index = (mtuSize - CA_BLE_HEADER_SIZE - CA_BLE_LENGTH_HEADER_SIZE) +
            (segmentNum * (mtuSize - CA_BLE_HEADER_SIZE));
    if (sourceDataLength < index + segmentPayloadLength)
    {
        OIC_LOG(DEBUG, TAG, "dataSegment will exceed");
        return CA_STATUS_FAILED;
    }

    memcpy(dataSegment, dataHeader, CA_BLE_HEADER_SIZE);
    memcpy(dataSegment + CA_BLE_HEADER_SIZE, sourceData + index, segmentPayloadLength);
    return CA_STATUS_OK;
}

CAResult_t CAParseHeader(const uint8_t *header,
                         CABLEPacketStart_t *startFlag,
                         uint16_t *sourcePort,
                         CABLEPacketSecure_t *secureFlag,
                         uint16_t *destPort)
{
    VERIFY_NON_NULL_MSG(header, TAG, "header is NULL");

    *startFlag = CAGetBits(header[0], CA_BLE_START_POS, CA_BLE_START_LEN);
    *sourcePort = CAGetBits(header[0], CA_BLE_SOURCE_PORT_POS, CA_BLE_SOURCE_PORT_LEN);
    *secureFlag = CAGetBits(header[1], CA_BLE_SECURE_POS, CA_BLE_SECURE_LEN);
    *destPort = CAGetBits(header[1], CA_BLE_DESTINATION_PORT_POS, CA_BLE_DESTINATION_PORT_LEN);

    return CA_STATUS_OK;
}

CAResult_t CAParseHeaderPayloadLength(uint8_t *header,
                                      size_t headerLength,
                                      uint32_t *dataLength)
{
    VERIFY_NON_NULL_MSG(header, TAG, "header is NULL");

    if (headerLength != CA_BLE_LENGTH_HEADER_SIZE)
    {
        return CA_STATUS_FAILED;
    }

    for(size_t idx = 0; idx < headerLength; idx++)
    {
        *dataLength <<= 8;
        *dataLength |= header[CA_BLE_HEADER_SIZE+idx];
    }

    return CA_STATUS_OK;
}
