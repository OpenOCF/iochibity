/* coresource.c */

/* OBSOLETE? */

#include "coresource_mgr.h"

/* A coresource is a co-service-provider.
 *
 * A co_service_provider is a client-side proxy for a service_provider
 * ("resource") on the server side. We use the OCClientReponse to
 * discovery requests as a database of such co_service_providers. The
 * co_service_provider itself is expressed as an OCResourcePayload
 * struct, which is pointed to by an OCDiscoveryPayload (which is
 * pointed to by the OCClientResponse struct).
 *
 * Note that an OCResourcePayload is actually a representation of a
 * link in oic/res; it includes ep info like rel and anchor, secure
 * flag, policy bitmask.
 *
 * To uniquely identify a "resource", we need the Service ID of the
 * hosting device plus the URI of the hosted resource.  SIDs are
 * relative to the local network (i.e. they uniquely identify the
 * device on the subnet); resource URIs are relative to the device.
 *
 * (Note that a device may have multiple network addresses, so they
 * cannot be used as unique device identifiers.)
 *
 * The OCDiscoveryPayload represents the oic/res resource of the
 * device, which in turn contains all the resources hosted on the
 * device. OCDiscoveryPayload encodes the device SID, a name, list of
 * rtypes and rinterfaces, plus a list of OCResourcePayload structs,
 * one per hosted resource; the latter are the co_service_providers.
 *
 * The OCResourcePayload encodes the resource URI but not the device
 * SID. So to use these internal data structs as a database we need to
 * track the OCDiscoveryPayload (for the device ID) plus a pointer to
 * the OCResourcePayload.
 *
 * Note that the OCClientResponse (which is the root node of this tree
 * structure) also contains an identity field (of type OCIdentity) and
 * a resourceUri. For discovery responses, the former is (evidently)
 * unused (it's encoded in the OCDiscoveryResponse), and the latter is
 * "oic/res".  (The identity field would be needed for e.g. an
 * OCRepPayload, which does not record an SID.)
 *
 */

/* NOTE: the "anchor" field of the OCResourcePayload _may_ contain the
 * device SID (of form ocf://3de345b3-465e-2ab4-20dd-214883e06fdc);
 * but that field can be used to indicate that a resource is hosted on
 * some other device, so it cannot be used as the ID of the oic/res
 * host device! */

#if EXPORT_INTERFACE
struct CoServiceProvider {
    OCClientResponse* discovery_response;
    OCDiscoveryPayload* device;	/* this struct is a linked list, so we need a ptr to it */
    OCResourcePayload* co_service_provider;
};
#endif	/* INTERFACE */
