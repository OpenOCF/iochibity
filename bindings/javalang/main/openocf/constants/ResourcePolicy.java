package openocf.constants;

public class ResourcePolicy
{
    public static final int NOMETHOD       = 0;
    // typedef enum
    // {
    /** When none of the bits are set, the resource is non-discoverable &amp;
     *  non-observable by the client.*/
    public static final int NONE = 0;

    /** When this bit is set, the resource is allowed to be discovered by clients.*/
    public static final int DISCOVERABLE  = (1 << 0);

    /** When this bit is set, the resource is allowed to be observed by clients.*/
    public static final int OBSERVABLE    = (1 << 1);

    /** When this bit is set, the resource is initialized, otherwise the resource
     *  is 'inactive'. 'inactive' signifies that the resource has been marked for
     *  deletion or is already deleted.*/
    public static final int ACTIVE        = (1 << 2);

    /** When this bit is set, the resource has been marked as 'slow'.
     * 'slow' signifies that responses from this resource can expect delays in
     *  processing its requests from clients.*/
    public static final int SLOW          = (1 << 3);

    /*GAR TODO: always default to secure at compile time; reset at runtime for insecure resources */
    /*GAR #ifdef __WITH_DTLS__ */
    /*     /\** When this bit is set, the resource is a secure resource.*\/ */
    public static final int SECURE        = (1 << 4);
    /* #else */
    /*     OC_SECURE        = (0), */
    /* #endif */

    /** When this bit is set, the resource is allowed to be discovered only
     *  if discovery request contains an explicit querystring.
     *  Ex: GET /oic/res?rt=oic.sec.acl */
    public static final int EXPLICIT_DISCOVERABLE   = (1 << 5);

    // #ifdef WITH_MQ
    // /** When this bit is set, the resource is allowed to be published */
    // ,OC_MQ_PUBLISHER     = (1 << 6)
    // #endif

    // #ifdef MQ_BROKER
    // /** When this bit is set, the resource is allowed to be notified as MQ broker.*/
    // ,OC_MQ_BROKER        = (1 << 7)
    // #endif
    // } OCResourceProperty;
}
