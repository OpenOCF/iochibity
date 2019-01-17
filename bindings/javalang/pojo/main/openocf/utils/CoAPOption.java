package openocf.utils;

// NB: OCHeaderOption misnamed, CoAP Options are not headers

public class CoAPOption	// OCHeaderOption
{
// typedef struct OCHeaderOption
// {
//     /** The protocol ID this option applies to.*/
//     OCTransportProtocolID protocolID;

//     /** The header option ID which will be added to communication packets.*/
//     uint16_t optionID;

//     /** its length 191.*/
//     uint16_t optionLength;

//     /** pointer to its data.*/
//     uint8_t optionData[MAX_HEADER_OPTION_DATA_LENGTH];

// #ifdef SUPPORTS_DEFAULT_CTOR
//     OCHeaderOption() = default;
//     OCHeaderOption(OCTransportProtocolID pid,
//                    uint16_t optId,
//                    uint16_t optlen,
//                    const uint8_t* optData)
//         : protocolID(pid),
//           optionID(optId),
//           optionLength(optlen)
//     {

//         // parameter includes the null terminator.
//         optionLength = optionLength < MAX_HEADER_OPTION_DATA_LENGTH ?
//                         optionLength : MAX_HEADER_OPTION_DATA_LENGTH;
//         memcpy(optionData, optData, optionLength);
//         optionData[optionLength - 1] = '\0';
//     }
// #endif
// } OCHeaderOption;
}
