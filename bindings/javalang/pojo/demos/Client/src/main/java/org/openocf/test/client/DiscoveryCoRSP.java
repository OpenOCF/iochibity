package org.openocf.test.client;

import openocf.OpenOCF;
import openocf.OpenOCFClient;
import openocf.app.CoResourceSP;
import openocf.constants.Method;
import openocf.message.InboundResponse;
// import openocf.IObservationRecord;
import openocf.observation.ObservationRecord;
// import openocf.ObservationList;

import openocf.constants.OCStackResult;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.openocf.test.PojoLogger;

import java.lang.RuntimeException;
import java.util.List;
import java.util.Map;

import java.util.concurrent.CountDownLatch;

// CoResourceSPs contain everything needed to create OutboundRequest
// messages to be exhibited.

public class DiscoveryCoRSP
    extends  CoResourceSP
    // implements ICoResourceSP
{
    private final static Logger LOGGER = Logger.getLogger(DiscoveryCoRSP.class.getName());

    // 1. set properties of OutboundRequest msg so server can select it:
    // UriPath, Action (method), etc.
    // 2. set properties of comm mechanism:
    // qos, UDP/TCP, IPv6/IPv4, multicast/unicast, etc.
    // 3. set resource state properties

    public CountDownLatch finished; // to control UI in OCFTestClient
    public CountDownLatch latch() {
	finished = new CountDownLatch(1);
	return finished;
    }

    public int cbdata = 99;  // callback param data

    public DiscoveryCoRSP() {
	super();
    }
    public DiscoveryCoRSP(String uri) {
	super();
	setUri(uri);
    }

    /**
       By the time `coReact` is called, the CoRSPs matching the incoming
       RSPs will have been registered with the ServiceManager.  All
       this CoRSP needs to do is coReact.
     */
    // The lower layer observes inbound msg (oocf_inbound_response,
    // OCClientResponse), passes to coReact method
    @Override
    public void coReact(InboundResponse resp)
    {
	PojoLogger.LOGGER.info("ENTRY");
	PojoLogger.LOGGER.info("cbdata: " + cbdata);

	try {
	    PojoLogger.logInboundResponse(resp);
	}
	catch (Exception e) {
	    PojoLogger.LOGGER.info("Logger Exception occurred");
	    PojoLogger.LOGGER.info(e.getMessage());
	}

        // Flutter:
	//PojoLogger.logInboundResponseMap(resp.toMap());

	this.isRetain = true;
	resp.isRetain = true;

	finished.countDown();

	// List<CoResourceSP> cosps = ServiceManager.registeredCoResourceSPs();


	// ServiceManager.registerCoResourceSPs(this.observations());
	// ServiceManager.registerCoResourceSPs(this.getSPObservations());

	// first update _this_ with incoming data
	// this.setDestination(observationIn.getRemoteDeviceAddress());
	// etc. connType, etc.

	// then iterate over Observation payloads
	// if (observationIn.result == OCStackResult.OK) {
	//     ObservationList<ObservationRecord> observationRecords = observationIn.getObservationRecords();
	//     for (ObservationRecord observationRecord
	// 	     : (ObservationList<ObservationRecord>) observationRecords) {
	// 	PojoLogger.PojoLogger.LOGGER.info("\tOBSERVED: " + observationRecord.getUriPath());
	// 	List<IObservationRecord> kids = observationRecord.getChildren();
	// 	if (kids != null) {
	// 	    for (IObservationRecord childObservationRecord : kids) {
	// 		PojoLogger.PojoLogger.LOGGER.info("\t->OBSERVED: " + childObservationRecord.getUriPath());

	// 		GenericCoRSP cosp = new GenericCoRSP(observationIn, (ObservationRecord)childObservationRecord);
	// 		ServiceManager.registerCoResourceSP(cosp);

	// 		if (childObservationRecord.getUriPath().equals("/a/temperature")) {
	// 		    PojoLogger.PojoLogger.LOGGER.info("LOG: found temperature resource");
	// 		    // gRemoteResourceAddress = observationIn.getRemoteDeviceAddress();
	// 		    // gRemoteResourceAddress.port
	// 		    // 	= ((Integer)childObservation.getProperties().get("port"))
	// 		    // 	.intValue();
	// 		    // Logger.logDeviceAddress(gRemoteResourceAddress);
	// 		}

	// 		if (childObservationRecord.getUriPath().equals("/a/led")) {
	// 		    PojoLogger.PojoLogger.LOGGER.info("LOG: found LED resource");
	// 		    // gLEDAddress = observationIn.getRemoteDeviceAddress();
	// 		    // gLEDAddress.port
	// 		    // 	= ((Integer)childObservationRecord.getProperties().get("port"))
	// 		    // 	.intValue();
	// 		    // Logger.logDeviceAddress(gLEDAddress);
	// 		}

	// 		if (childObservationRecord.getUriPath().equals("/a/whatsit")) {
	// 		    PojoLogger.PojoLogger.LOGGER.info("LOG: found whatsit resource");
	// 		    // gWhatsitAddress = observationIn.getRemoteDeviceAddress();
	// 		    // gWhatsitAddress.port
	// 		    // 	= ((Integer)childObservation.getProperties().get("port"))
	// 		    // 	.intValue();
	// 		    // Logger.logDeviceAddress(gWhatsitAddress);
	// 		}
	// 	    }
	// 	}
	//     }
	// }

	// throw new RuntimeException("test exception");

	// this.deactivate();

	// return 3; // (OC_STACK_KEEP_TRANSACTION | OC_STACK_KEEP_PAYLOAD);
    }
}
