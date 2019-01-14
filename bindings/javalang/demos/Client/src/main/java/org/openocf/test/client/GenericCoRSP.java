package org.openocf.test.client;

import openocf.app.CoResourceSP;
import openocf.constants.Method;
import openocf.message.InboundResponse;
import openocf.observation.ObservationRecord;
// import openocf.ObservationList;
import openocf.constants.OCStackResult;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.openocf.test.OCFLogger;

import java.util.List;

public class GenericCoRSP
    extends  CoResourceSP
    // implements ICoResourceSP
{
    private final static Logger LOGGER = Logger.getLogger(Logger.GLOBAL_LOGGER_NAME);

    private int cbdata = 99;

    public GenericCoRSP() {
	super();
    }
    public GenericCoRSP(String uri) {
	super();
	setUri(uri);
    }

    // we need info from both response msg and observation payload to create a unicast request
    // public GenericCoSP(ObservationIn observationIn, ObservationRecord observationRecord) {
    // 	super();
    // 	System.out.println("GenericCoSP CTOR");
    // 	System.out.println("Uri path: " + observationIn.getUriPath());
    // 	System.out.println("Remote Device Address: "
    // 			   + observationIn.getRemoteDeviceAddress().ipAddress());
    // 	System.out.println("Remote Device Port:    "
    // 			   + observationIn.getRemoteDeviceAddress().port());

    // 	// we don't need the following setDestination anymore since
    // 	// the incoming response rec is already stored in a TLS var,
    // 	// making the remote DevAddr available.
    // 	// setDestination(observationIn.getRemoteDeviceAddress());
    // 	uriPath(observationRecord.getUriPath());

    // 	// method and qos to be set by user before sending

    // 	// FIXME: deal with header options

    // 	// FIXME: explode OCConnectivityType into network protocol,
    // 	// policies, and scope, and transport security flag
    // 	int connectivityType = observationIn.connType;
    // 	// setNetworkPolicies(connectivityType);

    // 	// types, interfaces, properties?
    // }

    public void coReact(InboundResponse resp)
    {
	System.out.println("JAVA: GenericCoSP.coReact ENTRY");
	// System.out.println("JAVA: cbdata: " + cbdata);
	// Logger.logObservationIn(responseIn);
	// Logger.logObservationIn(this);

	// save incoming resource info - ServiceManager.registerRemoteResource(...)?
	// update screen ...

	// return 0;
    }
}
