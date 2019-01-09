#include "oocf_uri.h"

#include <arpa/inet.h>
#include <netinet/in.h>

/* NOTE: OCF uses "uri" to refer to the path portion of a resource url */
#if EXPORT_INTERFACE
#define MAX_URI_LENGTH (256)    /* src: ocstackconfig.h */
#define MAX_QUERY_LENGTH (256)  /* src: ocstackconfig.h */
#define MAX_URI_QUERY MAX_URI_LENGTH + MAX_QUERY_LENGTH
#endif

static const char COAP_TCP_SCHEME[] = "coap+tcp:";
static const char COAPS_TCP_SCHEME[] = "coaps+tcp:";

/**
 *  A request URI consists of the following components in order:
 *                              example
 *  optionally one of
 *      CoAP over UDP prefix    "coap://"
 *      CoAP over TCP prefix    "coap+tcp://"
 *      CoAP over DTLS prefix   "coaps://"
 *      CoAP over TLS prefix    "coaps+tcp://"
 *  optionally one of
 *      IPv6 address            "[1234::5678]"
 *      IPv4 address            "192.168.1.1"
 *  optional port               ":5683"
 *  resource uri                "/oc/core..."
 *
 *  for PRESENCE requests, extract resource type.
 *
 * this routine sets the SECURE flag if scheme is coaps
 */
OCStackResult ParseRequestUri(const char *fullUri,
                              OCTransportAdapter adapter,
                              OCTransportFlags flags,
                              OCDevAddr **devAddr,
                              char **resourceUri,
                              char **resourceType)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    VERIFY_NON_NULL(fullUri, FATAL, OC_STACK_INVALID_CALLBACK);

    OCStackResult result = OC_STACK_OK;
    OCDevAddr *da = NULL;
    char *colon = NULL;
    char *end;

    // provide defaults for all returned values
    if (devAddr)
    {
        *devAddr = NULL;
    }
    if (resourceUri)
    {
        *resourceUri = NULL;
    }
    if (resourceType)
    {
        *resourceType = NULL;
    }

    // delimit url prefix, if any
    const char *start = fullUri;
    char *slash2 = strstr(start, "//");
    if (slash2)
    {
        start = slash2 + 2;
    }
    char *slash = strchr(start, '/');
    if (!slash)
    {
        return OC_STACK_INVALID_URI;
    }

    // process url scheme
    size_t prefixLen = slash2 - fullUri;
    bool istcp = false;
    if (prefixLen)
    {
        if (((prefixLen == sizeof(COAP_TCP_SCHEME) - 1) && (!strncmp(fullUri, COAP_TCP_SCHEME, prefixLen)))
        || ((prefixLen == sizeof(COAPS_TCP_SCHEME) - 1) && (!strncmp(fullUri, COAPS_TCP_SCHEME, prefixLen))))
        {
            istcp = true;
        }
    }

    // TODO: this logic should come in with unit tests exercising the various strings
    // processs url prefix, if any
    size_t urlLen = slash - start;
    // port
    uint16_t port = 0;
    size_t len = 0;
    OIC_LOG_V(INFO, TAG, "\turlLen: %d; devAddr: %p", urlLen, devAddr);

    if (urlLen && devAddr)      /* this will never be true? client always passes NULL devAddr */
    {   // construct OCDevAddr
        OIC_LOG_V(INFO, TAG, "\tconstructing OCDevAddr");
        if (start[0] == '[')
        {   // ipv6 address
            char *close = strchr(++start, ']');
            if (!close || close > slash)
            {
                return OC_STACK_INVALID_URI;
            }
            end = close;
            if (close[1] == ':')
            {
                colon = close + 1;
            }

            if (istcp)
            {
                adapter = (OCTransportAdapter)(adapter | OC_ADAPTER_TCP);
            }
            else
            {
                adapter = (OCTransportAdapter)(adapter | OC_ADAPTER_IP);
            }
            flags = (OCTransportFlags)(flags | OC_IP_USE_V6);
        }
        else
        {
            char *dot = strchr(start, '.');
            if (dot && dot < slash)
            {   // ipv4 address
                colon = strchr(start, ':');
                end = (colon && colon < slash) ? colon : slash;

                if (istcp)
                {
                    // coap over tcp
                    adapter = (OCTransportAdapter)(adapter | OC_ADAPTER_TCP);
                }
                else
                {
                    adapter = (OCTransportAdapter)(adapter | OC_ADAPTER_IP);
                }
                flags = (OCTransportFlags)(flags | OC_IP_USE_V4);
            }
            else
            {   // MAC address
                end = slash;
            }
        }
        len = end - start;
        if (len >= sizeof(da->addr))
        {
            return OC_STACK_INVALID_URI;
        }
        // collect port, if any
        if (colon && colon < slash)
        {
            for (colon++; colon < slash; colon++)
            {
                char c = colon[0];
                if (c < '0' || c > '9')
                {
                    return OC_STACK_INVALID_URI;
                }
                port = 10 * port + c - '0';
            }
        }

        len = end - start;
        if (len >= sizeof(da->addr))
        {
            return OC_STACK_INVALID_URI;
        }
        OIC_LOG_V(INFO, TAG, "\taddr len: %d", len);


        da = (OCDevAddr *)OICCalloc(sizeof (OCDevAddr), 1);
        if (!da)
        {
            return OC_STACK_NO_MEMORY;
        }

        // Decode address per RFC 6874.
        result = OCDecodeAddressForRFC6874(da->addr, sizeof(da->addr), start, end);
        if (result != OC_STACK_OK)
        {
             OICFree(*devAddr);
             return result;
        }

        da->port = port;
        da->adapter = adapter;
        da->flags = flags;
        if (!strncmp(fullUri, "coaps", 5))
        {
            da->flags = (CATransportFlags_t)(da->flags|CA_SECURE);
        }
        *devAddr = da;
        LogDevAddr(*devAddr);
    }

    // process resource uri, if any
    if (slash)
    {   // request uri and query
        size_t ulen = strlen(slash); // resource uri length
        size_t tlen = 0;      // resource type length
        char *type = NULL;

        static const char strPresence[] = "/oic/ad?rt=";
        static const size_t lenPresence = sizeof(strPresence) - 1;
        if (!strncmp(slash, strPresence, lenPresence))
        {
            type = slash + lenPresence;
            tlen = ulen - lenPresence;
        }
        // resource uri
        if (resourceUri)
        {
            *resourceUri = (char *)OICMalloc(ulen + 1);
            if (!*resourceUri)
            {
                result = OC_STACK_NO_MEMORY;
                goto error;
            }
            OICStrcpy(*resourceUri, (ulen + 1), slash);
        }
        // resource type
        if (type && resourceType)
        {
            *resourceType = (char *)OICMalloc(tlen + 1);
            if (!*resourceType)
            {
                result = OC_STACK_NO_MEMORY;
                goto error;
            }

            OICStrcpy(*resourceType, (tlen + 1), type);
        }
    }
    OIC_LOG_V(INFO, TAG, "\tresourceUri: %s", *resourceUri);
    OIC_LOG_V(INFO, TAG, "\tresourceType: %s", *resourceType);

    OIC_LOG_V(INFO, TAG, "%s EXIT OK", __func__);
    return OC_STACK_OK;

error:
    // free all returned values
    if (devAddr)
    {
        OICFree(*devAddr);
    }
    if (resourceUri)
    {
        OICFree(*resourceUri);
    }
    if (resourceType)
    {
        OICFree(*resourceType);
    }
    OIC_LOG_V(INFO, TAG, "%s EXIT error: %d", __func__, result);
    return result;
}

/*
 * This function splits the URI using the '?' delimiter.
 * "uriWithoutQuery" is the block of characters between the beginning
 * till the delimiter or '\0' which ever comes first.
 * "query" is whatever is to the right of the delimiter if present.
 * No delimiter sets the query to NULL.
 * If either are present, they will be malloc'ed into the params 2, 3.
 * The first param, *uri is left untouched.

 * NOTE: This function does not account for whitespace at the end of the URI NOR
 *       malformed URIs with '??'. Whitespace at the end will be assumed to be
 *       part of the query.
 */
/**
 * Extract query from a URI.
 *
 * @param uri Full URI with query.
 * @param query Pointer to string that will contain query.
 * @param newURI Pointer to string that will contain URI.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
/* static OCStackResult getQueryFromUri(const char * uri, char** resourceType, char ** newURI); */
OCStackResult getQueryFromUri(const char * uri, char** query, char ** uriWithoutQuery)
{
    if(!uri)
    {
        return OC_STACK_INVALID_URI;
    }
    if(!query || !uriWithoutQuery)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *query           = NULL;
    *uriWithoutQuery = NULL;

    size_t uriWithoutQueryLen = 0;
    size_t queryLen = 0;
    size_t uriLen = strlen(uri);

    char *pointerToDelimiter = strstr(uri, "?");

    uriWithoutQueryLen = pointerToDelimiter == NULL ? uriLen : (size_t)(pointerToDelimiter - uri);
    queryLen = pointerToDelimiter == NULL ? 0 : uriLen - uriWithoutQueryLen - 1;

    if (uriWithoutQueryLen)
    {
        *uriWithoutQuery =  (char *) OICCalloc(uriWithoutQueryLen + 1, 1);
        if (!*uriWithoutQuery)
        {
            goto exit;
        }
        OICStrcpy(*uriWithoutQuery, uriWithoutQueryLen +1, uri);
    }
    if (queryLen)
    {
        *query = (char *) OICCalloc(queryLen + 1, 1);
        if (!*query)
        {
            OICFree(*uriWithoutQuery);
            *uriWithoutQuery = NULL;
            goto exit;
        }
        OICStrcpy(*query, queryLen + 1, pointerToDelimiter + 1);
    }

    return OC_STACK_OK;

    exit:
        return OC_STACK_NO_MEMORY;
}

char  *getPathFromRequestURL(const char * url)
{
    if(!url)
    {
        return NULL;
    }

    char *uriWithoutQuery = NULL;
    char *query = NULL;
    OCStackResult requestResult = OC_STACK_ERROR;

    requestResult = getQueryFromUri(url, &query, &uriWithoutQuery);

    if (requestResult != OC_STACK_OK || !uriWithoutQuery)
    {
        OIC_LOG_V(ERROR, TAG, "getQueryFromUri() failed with OC error code %d\n", requestResult);
        return NULL;
    }
    return uriWithoutQuery;
}

char  *getQueryFromRequestURL(const char * url) EXPORT
{
    if(!url)
    {
        return NULL;
    }

    char *uriWithoutQuery = NULL;
    char *query = NULL;
    OCStackResult requestResult = OC_STACK_ERROR;

    requestResult = getQueryFromUri(url, &query, &uriWithoutQuery);

    if (requestResult != OC_STACK_OK || !uriWithoutQuery)
    {
        OIC_LOG_V(ERROR, TAG, "getQueryFromUri() failed with OC error code %d\n", requestResult);
        return NULL;
    }
    return query;
}

bool oocf_addr_is_scoped(char *addr, OCTransportFlags scope) EXPORT
{
    char buf[MAX_URI_QUERY + 1];
    strncpy(buf, addr, strlen(addr));
    char *addr_literal = strtok(buf, "%"); /* drop the zone suffix */

    struct sockaddr_in6 saddr;
    inet_pton(AF_INET6, addr_literal, &saddr.sin6_addr);
    if (scope == CA_SCOPE_LINK) {
        if (IN6_IS_ADDR_LINKLOCAL(&(saddr.sin6_addr.s6_addr))) {
            return true;
        }
    }
    return false;
}

