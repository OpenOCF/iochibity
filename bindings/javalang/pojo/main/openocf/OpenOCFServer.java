package openocf;

import openocf.app.ResourceSP;
import openocf.message.OutboundResponse;

import java.util.List;

public class OpenOCFServer extends OpenOCF
{
    public static native void exhibit(OutboundResponse RSP);

    // below from ServiceManager.java

    // JNI implementations call into //src/ocf/ocf_services_server

    // public static native int OCCreateResource(Object /*OCResourceHandle* */ handle,
    native public synchronized static
	ResourceSP	registerResourceSP(ResourceSP serviceProvider);

    native public static List<ResourceSP> registeredResourceSPs();

    native public static List<ResourceSP> getRelatedResourceSPs();


}
