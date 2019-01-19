// FIXME: move this to openocf.OpenOCF
package openocf;

import java.net.URL;
import java.util.List;

import openocf.app.CoResourceSP;
import openocf.message.OutboundRequest;
import openocf.message.InboundResponse;
import openocf.observation.CoResourceLink;
import openocf.message.CoAPOption;
import openocf.Endpoint;

public class OpenOCFClient
    extends OpenOCF
    // implements ICoResourceDBM
{
    // wrap OCDoResource (i.e. send request)
    private static native void _coExhibit(int method,
					  String uri,
					  Endpoint ep,
					  // Payload payload,
					  int qos,
					  CoResourceSP corsp, // obj with callback method
					  List<CoAPOption> options);

    public static void coExhibit(OutboundRequest message) {
        // FIXME: validate inputs here?
	CoResourceSP corsp = message.getCoResourceSP();
	_coExhibit(message.getMethod(),
		   corsp.getUri(),
		   null,
		   message.getQualityOfService(),
		   message.getCoResourceSP(),
		   null);
    };


    // wrap OCSetDefaultDeviceEntityHandler:
    native public static CoResourceSP registerDefaultCoResourceSP(CoResourceSP cosp);


    // FIXME: define an interface for the CoResourceDBM API

    // CoResourceDBM Services: virtual service managing database of CoRSPs
    // jni maps to ocf_services_client/coresource_dbm.c

    native public synchronized static void retain(InboundResponse response);

    // public static native int OCCreateResource(Object /*OCResourceHandle* */ handle,
    // native public synchronized static CoResourceSP registerCoResourceSP(CoResourceSP coRSP);

    // native public static List<CoResourceSP> registeredCoResourceSPs();
    native public static List<InboundResponse> getInboundResponses();

    native public static InboundResponse getInboundResponse(long handle);

    // resource link = OCResourcePayload within OCDiscoveryPayload
    native public static List<CoResourceLink> getCoResourceLinks();

    native public static CoResourceLink getCoResourceLink(URL url);

    // native public static List<CoResourceSP> getRelatedCoResourceSPs();
}
