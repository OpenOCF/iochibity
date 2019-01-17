package openocf.constants;

public class ServiceResult	// OCEntityHandlerResult
{
// typedef enum
// {
//     OC_EH_OK = 0,
    public static final int OK = 0;
//     OC_EH_ERROR,
    public static final int ERROR = 1;
//     OC_EH_RESOURCE_CREATED, // 2.01
    public static final int CREATED = 2;
//     OC_EH_RESOURCE_DELETED, // 2.02
    public static final int DELETED = 3;
//     OC_EH_SLOW, // 2.05
    public static final int SLOW = 4;
//     OC_EH_FORBIDDEN, // 4.03
    public static final int FORBIDDEN = 5;
//     OC_EH_RESOURCE_NOT_FOUND, // 4.04
    public static final int NOT_FOUND = 6;
//     OC_EH_VALID,   // 2.03
    public static final int VALID = 7;
//     OC_EH_CHANGED, // 2.04
    public static final int CHANGED = 8;
//     OC_EH_CONTENT, // 2.05
    public static final int CONTENT = 9;
//     OC_EH_BAD_REQ, // 4.00
    public static final int BAD_REQ = 10;
//     OC_EH_UNAUTHORIZED_REQ, // 4.01
    public static final int UNAUTHORIZED = 11;
//     OC_EH_BAD_OPT, // 4.02
    public static final int BAD_OPTION = 12;
//     OC_EH_METHOD_NOT_ALLOWED, // 4.05
    public static final int NOT_ALLOWED = 13;
//     OC_EH_NOT_ACCEPTABLE, // 4.06
    public static final int NOT_ACCEPTABLE = 14;
//     OC_EH_INTERNAL_SERVER_ERROR, // 5.00
    public static final int SERVER_ERROR = 15;
//     OC_EH_RETRANSMIT_TIMEOUT // 5.04
    public static final int RETRANSMIT_TIMEOUT = 16;
 // } OCEntityHandlerResult;
}
