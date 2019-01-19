package org.openocf.test.server;

import openocf.OpenOCFServer;
import openocf.message.CoAPOption;
import openocf.message.InboundRequest;
import openocf.message.OutboundResponse;
// import openocf.message.ObservationRecord;
// import openocf.ObservationList;
import openocf.utils.PropertyMap;
// import openocf.app.Resource;
import openocf.app.ResourceSP;
import openocf.app.IResourceSP;
import openocf.constants.Method;
import openocf.constants.OCStackResult;
import openocf.constants.ResourcePolicy;
import openocf.constants.ServiceResult;

import org.openocf.test.PojoLogger;

import openocf.exceptions.OCFNotImplementedException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.ArrayList;
import java.util.List;
import java.util.LinkedList;
import java.util.Map;

public class WhatsitSP
    extends  ResourceSP
    // implements IResourceSP
{
    // 1. set parameters to select InboundRequest messages::
    // UriPath, rts, ifs, action, etc.
    // 2. set properties of low-level comm channel:
    // ??

    int foo = 99;

    public WhatsitSP() {
	setUriPath("/foo/whatsit");
	addType("foo.t.whatsit");
	addInterface("foo.if.whatsit");
	setPolicies(ResourcePolicy.DISCOVERABLE
		    // | ResourcePolicy.WATCHABLE
		    | ResourcePolicy.SECURE);
    }

    public WhatsitSP(String uriPath) {
	setUriPath(uriPath);
	addType("bar.t.whoozit");
	addType("bar.t.whatsit");
	addInterface("foo.if.whatsit");
	addInterface("foo.if.whoozit");
	setPolicies(ResourcePolicy.DISCOVERABLE
		    // | ResourcePolicy.WATCHABLE
		    | ResourcePolicy.SECURE);
    }

    @Override
    public void react(InboundRequest request)
    {
	System.out.println("WhatsitSP.react routine ENTRY");
	PojoLogger.logInboundRequest(request);

	System.out.println("WhatsitSP: requestIn callback param foo = " + foo);

	OutboundResponse response = null;

	switch (this.method()) {
	case Method.GET:
	    System.out.println("WhatsitSP: method: GET");
	    // FIXME: try catch?
	    response = reactToGetObservation(request);
	    break;
	// case Method.PUT:
	//     System.out.println("WhatsitSP: method: PUT");
	//     break;
	// case Method.POST:
	//     System.out.println("WhatsitSP: method: POST");
	//     // observationsOut = servicePostRequest(requestIn);
	//     break;
	// case Method.DELETE:
	//     System.out.println("Whatsit method: DELETE");
	//     break;
	// case Method.WATCH:
	//     System.out.println("Whatsit method: WATCH");
	//     // observationsOut = serviceGetRequest(requestIn);
	//     break;
	// case Method.WATCH_ALL:
	//     System.out.println("Whatsit method: WATCH_ALL");
	//     break;
	// case Method.CANCEL_WATCH:
	//     System.out.println("Whatsit method: CANCEL_WATCH");
	//     break;
	// case Method.PRESENCE:
	//     System.out.println("Whatsit method: PRESENCE");
	//     break;
	// case Method.DISCOVER:
	//     System.out.println("Whatsit method: DISCOVER");
	//     // should not happen - DISCOVER method is only for client-side coreaction
	//     // discover logic is handled by the stack on the server
	//     // Throw an exception if we hit this?
	//     break;
	// case Method.NOMETHOD:
	//     System.out.println("Whatsit method: NOMETHOD");
	//     break;
	// default:
	//     System.out.println("Whatsit method: UNKNOWN");
	//     break;
	}

	try {
	    OpenOCFServer.exhibit(response);
	    // this.exhibit();
	} catch (Exception e) {
	    System.out.println("[E] WhatisSP" + " | " + "exhibit exception");
	    e.printStackTrace();
	}

	System.out.println("WhatsitSP.react EXIT");
	return; // ServiceResult.OK;
    }

    // private ObservationList<Observation> serviceGetRequest(InboundRequest request)
    // private ObservationList<Observation> ObserveGetRequest(InboundRequest requestIn)
    private OutboundResponse reactToGetObservation(InboundRequest request)
    {
	System.out.println("WhatsitSP.serviceGetRequest ENTRY");

	// to react to a request, the ResourceSP sets its
	// internal state (types, interfaces, properties, etc.).  Then
	// the exhibit will use the state to generate and send
	// an Observation.

	// NOTE that messages and messaging are hidden.

	// Mandatory: first react to the InboundRequest. This sets some of
	// the internal state of the ResourceSP
	// this.react(requestIn);

	// Now modify internal state.  The `exhibit` routine
	// will use it to generate and send an observation message qua
	// response.

	// FIXME: these should modify the underlying OCResource, using e.g.
	// OCBindResourceTypeToResource and OCBindResourceInterfaceToResource
	this.addType("foo.t.a");
	this.addInterface("foo.if.a");

	// create the response
	OutboundResponse resp = new OutboundResponse(request);

	// if we were providing services for a real sensor instrument,
	// we would read the instrument here and set properties of the
	// response appropriately.
	this.putProperty("whatsit int", 1);
	this.putProperty("whatsit d", 1.1);
	this.putProperty("whatsit str", "Hello world");
	this.putProperty("whatsit bool", true);

	// what if we have child resources?

	return resp;
    }

    // private ObservationList<Observation> servicePostRequest(InboundRequest request)
    // {
    // 	System.out.println("WhatsitSP.servicePostRequest ENTRY");

    // 	// ResourceLocal r = request.getResource();
    // 	System.out.println("WhatsitSP: resource uri: " + this.getUriPath());

    // 	PayloadForResourceState pfrs = new PayloadForResourceState(request);

    // 	System.out.println("WhatsitSP: observation uri: " + pfrs.getUri());
    // 	// pfrs.setUri("/a/foo");
    // 	// System.out.println("WhatsitSP: observation new uri: " + pfrs.getUri());

    // 	pfrs.addResourceType("foo.t.a");
    // 	// pfrs.addResourceType("foo.t.b");
    // 	// List<String> llts = pfrs.getResourceTypes();
    // 	// for (String s : (List<String>)llts) {
    // 	//     System.out.println("WhatsitSP: observation r type: " + s);
    // 	// }
    // 	List<String> llifs = pfrs.getInterfaces();
    // 	llifs.add("foo.if.a");
    // 	// for (String s : (List<String>)llifs) {
    // 	//     System.out.println("WhatsitSP: observation r interface: " + s);
    // 	// }

    // 	PropertyMap<String, Object> pmps = pfrs.getProperties();
    // 	pmps.put("whatsit int", 1);
    // 	pmps.put("whatsit d", 1.1);
    // 	pmps.put("whatsit str", "Hello world");
    // 	pmps.put("whatsit bool", true);
    // 	// for(Map.Entry<String, Object> entry : pmps.entrySet()) {
    // 	//     System.out.println("WhatsitSP: observation r prop: " + entry.getKey() + " = " + entry.getValue());
    // 	// }

    // 	ObservationList<Observation> pll = new ObservationList<Observation>();

    // 	pll.add(pfrs);

    // 	return pll; //observation;
    // }

}
