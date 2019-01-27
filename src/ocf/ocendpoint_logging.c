
#include "ocendpoint_logging.h"

void log_endpoint_info(OCClientResponse *clientResponse)
{
    OIC_LOG_V(INFO, TAG, "ep addr: %s:%d", clientResponse->devAddr.addr, clientResponse->devAddr.port);
    OIC_LOG_V(INFO, TAG, "ep ifindex: %d", clientResponse->devAddr.ifindex);
    OIC_LOG_V(INFO, TAG, "ep route data: %s", clientResponse->devAddr.routeData);
    OIC_LOG_V(INFO, TAG, "ep device ID: %s", clientResponse->devAddr.remoteId);

    switch ( clientResponse->devAddr.adapter) {
    case OC_DEFAULT_ADAPTER: /** value zero indicates discovery.  FIXME:  huh? */
	OIC_LOG_V(INFO, TAG, "Transport adapter: DEFAULT (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_IP:	/* (1 << 0) IPv4 and IPv6, including 6LoWPAN.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: UDP/IP (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_GATT_BTLE: /* (1 << 1) GATT over Bluetooth LE.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: GATT/BTLE (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_RFCOMM_BTEDR: /* (1 << 2) RFCOMM over Bluetooth EDR.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: RFCOMM/BLEDR (%d)", clientResponse->devAddr.adapter);
	break;
#ifdef RA_ADAPTER
    case OC_ADAPTER_REMOTE_ACCESS: /* (1 << 3) Remote Access over XMPP.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: XMPP (%d)", clientResponse->devAddr.adapter);
	break;
#endif
    case OC_ADAPTER_TCP:    /* (1 << 4) CoAP over TCP.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: TCP (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ADAPTER_NFC:    /* (1 << 5) NFC Transport for Messaging.*/
	OIC_LOG_V(INFO, TAG, "Transport adapter: NFC (%d)", clientResponse->devAddr.adapter);
	break;
    case OC_ALL_ADAPTERS: /* 0xffffffff CA_ALL_ADAPTERS */
	OIC_LOG_V(INFO, TAG, "Transport adapter: ALL (%d)", clientResponse->devAddr.adapter);
	break;
    default:
	break;
    }

    if ( OC_DEFAULT_FLAGS == clientResponse->devAddr.flags)
	OIC_LOG_V(INFO, TAG, "DEFAULT FLAGS (%d)", clientResponse->devAddr.flags);

    /** Insecure transport is the default (subject to change).*/
    /** secure the transport path*/
    if (OC_FLAG_SECURE & clientResponse->devAddr.flags) /* (1 << 4) */
	OIC_LOG_V(INFO, TAG, "Transport security: TRUE");
    else OIC_LOG_V(INFO, TAG, "Transport security: FALSE");

    /** IPv4 & IPv6 auto-selection is the default.*/
    /** if adapter = IP (UDP) or TCP*/
    OIC_LOG_V(INFO, TAG, "Network protocols: %s %s",
	      ( (OC_IP_USE_V4 & clientResponse->devAddr.flags) > 0)? /* (1 << 6) */
	      "IPv4" : "",
	      ( (OC_IP_USE_V6 & clientResponse->devAddr.flags) > 0)? /* (1 << 5) */
	      "IPv6" : "");

    OIC_LOG_V(INFO, TAG, "Transport flags: 0x%08X", clientResponse->devAddr.flags);

    /* OIC_LOG_V(INFO, TAG, "IPv6 Scopes: %s%s%s%s%s%s%s",
     * 	      ((OC_SCOPE_INTERFACE & clientResponse->devAddr.flags) > 0)? /\* 0x1 *\/
     * 	      "Interface-Local" : "",
     * 	      ((OC_SCOPE_LINK & clientResponse->devAddr.flags) > 0)? /\* 0x2 *\/
     * 	      ", Link-Local" : "",
     * 	      ((OC_SCOPE_REALM & clientResponse->devAddr.flags) > 0)? /\* 0x3 *\/
     * 	      ", Realm-Local" : "",
     * 	      ((OC_SCOPE_ADMIN & clientResponse->devAddr.flags) > 0)? /\* 0x4 *\/
     * 	      ", Admin-Local" : "",
     * 	      ((OC_SCOPE_SITE & clientResponse->devAddr.flags) > 0)? /\* 0x5 *\/
     * 	      ", Site-Local" : "",
     * 	      ((OC_SCOPE_ORG & clientResponse->devAddr.flags) > 0)? /\* 0x8 *\/
     * 	      ", Organization-Local" : "",
     * 	      ((OC_SCOPE_GLOBAL & clientResponse->devAddr.flags) > 0)? /\* 0xE *\/
     * 	      ", Global" : ""); */

    /* /\** if adapter = IP (UDP) or TCP*\/
     * if (OC_IP_USE_V4 & clientResponse->devAddr.flags) /\* (1 << 6) *\/
     * 	OIC_LOG_V(INFO, TAG, "Network protocol: IPv4"); */

    /** Multicast only.*/
    if (OC_MULTICAST & clientResponse->devAddr.flags) /* (1 << 7) */
	OIC_LOG_V(INFO, TAG, "Multicast? TRUE");
    else OIC_LOG_V(INFO, TAG, "Multicast? FALSE");

    OIC_LOG_V(INFO, TAG, "IPv6 Scopes:");
    /** Link-Local multicast is the default multicast scope for IPv6.
     *  These are placed here to correspond to the IPv6 multicast address bits.*/

    /** IPv6 Interface-Local scope (loopback).*/
    if (OC_SCOPE_INTERFACE & clientResponse->devAddr.flags) /* 0x1 */
    	OIC_LOG_V(INFO, TAG, "\tInterface-Local");

    /** IPv6 Link-Local scope (default).*/
    if (OC_SCOPE_LINK & clientResponse->devAddr.flags) /* 0x2 */
    	OIC_LOG_V(INFO, TAG, "\tLink-Local");

    /** IPv6 Realm-Local scope. */
    if (OC_SCOPE_REALM & clientResponse->devAddr.flags) /* 0x3 */
    	OIC_LOG_V(INFO, TAG, "\tRealm-Local");

    /** IPv6 Admin-Local scope. */
    if (OC_SCOPE_ADMIN & clientResponse->devAddr.flags) /* 0x4 */
    	OIC_LOG_V(INFO, TAG, "\tAdmin-Local");

    /** IPv6 Site-Local scope. */
    if (OC_SCOPE_SITE & clientResponse->devAddr.flags) /* 0x5 */
    	OIC_LOG_V(INFO, TAG, "\tSite-Local");

    /** IPv6 Organization-Local scope. */
    if (OC_SCOPE_ORG & clientResponse->devAddr.flags) /* 0x8 */
    	OIC_LOG_V(INFO, TAG, "\tOrganization-Local");

    /**IPv6 Global scope. */
    if (OC_SCOPE_GLOBAL & clientResponse->devAddr.flags) /* 0x# */
    	OIC_LOG_V(INFO, TAG, "\tGlobal");
}

