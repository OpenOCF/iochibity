package openocf.constants;

public class OCStackResult {
    /** Success status code - START HERE.*/
    public static final char OK                         = 0;
    public static final char RESOURCE_CREATED           = 1;
    public static final char RESOURCE_DELETED           = 2;
    public static final char CONTINUE                   = 3;
    public static final char RESOURCE_CHANGED           = 4;
    /** Success status code - END HERE.*/

    // /** Error status code - START HERE.*/
    public static final char INVALID_URI                = 20;
    public static final char INVALID_QUERY              = 21;
    public static final char INVALID_IP                 = 22;
    public static final char INVALID_PORT               = 23;
    public static final char INVALID_CALLBACK           = 24;
    public static final char INVALID_METHOD             = 25;

    // /** Invalid parameter.*/
    public static final char INVALID_PARAM              = 26;
    public static final char INVALID_OBSERVE_PARAM      = 27;
    public static final char NO_MEMORY                  = 28;
    public static final char COMM_ERROR                 = 29;
    public static final char TIMEOUT                    = 30;
    public static final char ADAPTER_NOT_ENABLED        = 31;
    public static final char NOTIMPL                    = 32;

    // /** Resource not found.*/
    public static final char NO_RESOURCE                = 33;

    /** e.g: not supported method or interface.*/
    public static final char RESOURCE_ERROR             = 34;
    public static final char SLOW_RESOURCE              = 35;
    public static final char DUPLICATE_REQUEST          = 36;

    /** Resource has no registered observers.*/
    public static final char NO_OBSERVERS               = 37;
    public static final char OBSERVER_NOT_FOUND         = 38;
    public static final char VIRTUAL_DO_NOT_HANDLE      = 39;
    public static final char INVALID_OPTION             = 40;

    // /** The remote reply contained malformed data.*/
    public static final char MALFORMED_RESPONSE         = 41;
    public static final char PERSISTENT_BUFFER_REQUIRED = 42;
    public static final char INVALID_REQUEST_HANDLE     = 43;
    public static final char INVALID_DEVICE_INFO        = 44;
    public static final char INVALID_JSON               = 45;

    // /** Request is not authorized by Resource Server. */
    public static final char UNAUTHORIZED_REQ           = 46;
    public static final char TOO_LARGE_REQ              = 47;

    // /** Error code from PDM */
    public static final char PDM_IS_NOT_INITIALIZED     = 48;
    public static final char DUPLICATE_UUID             = 49;
    public static final char INCONSISTENT_DB            = 50;

    // /**
    //  * Error code from OTM
    //  * This error is pushed from DTLS interface when handshake failure happens
    //  */
    public static final char AUTHENTICATION_FAILURE     = 51;

    // /** Insert all new error codes here!.*/
    // #ifdef WITH_PRESENCE
    public static final char PRESENCE_STOPPED           = 128;
    public static final char PRESENCE_TIMEOUT           = 129;
    public static final char PRESENCE_DO_NOT_HANDLE     = 130;
    // #endif
    /** ERROR in stack.*/
    public static final char ERROR                      = 255;
    /** Error status code - END HERE.*/
}
