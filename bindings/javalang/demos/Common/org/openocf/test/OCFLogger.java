package org.openocf.test;

import openocf.OpenOCF;
import openocf.engine.OCFCommonSP;
import openocf.app.CoResourceSP;
import openocf.observation.ObservationRecord;
import openocf.utils.CoAPOption;
import openocf.utils.Endpoint;
import openocf.message.OutboundRequest;
import openocf.message.InboundRequest;
import openocf.message.OutboundResponse;
import openocf.message.InboundResponse;

// import openocf.ObservationList;
import openocf.utils.PropertyMap;
import openocf.app.IResourceSP;
import openocf.constants.OCStackResult;
import openocf.constants.ResourcePolicy;
import openocf.constants.ServiceResult;
import openocf.exceptions.OCFNotImplementedException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.LinkedList;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import java.io.IOException;
import java.util.logging.ConsoleHandler;
import java.util.logging.FileHandler;
import java.util.logging.Formatter;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.logging.LogRecord;
import java.util.logging.SimpleFormatter;

// TODO: use https://github.com/dialex/JCDP

// http://www.vogella.com/tutorials/Logging/article.html
// this custom formatter formats parts of a log record to a single line
class OCFFormatter extends Formatter {
    // this method is called for every log record
    public String format(LogRecord rec) {
        StringBuffer buf = new StringBuffer(1000);
        buf.append("\n");

        // colorize any levels >= WARNING in red
        if (rec.getLevel().intValue() >= Level.WARNING.intValue()) {
            buf.append(rec.getLevel());
        } else {
            buf.append(rec.getLevel());
        }

        // buf.append(calcDate(rec.getMillis()));
        buf.append(formatMessage(rec));
        return buf.toString();
    }

    private String calcDate(long millisecs) {
        SimpleDateFormat date_format = new SimpleDateFormat("MMM dd,yyyy HH:mm");
        Date resultdate = new Date(millisecs);
        return date_format.format(resultdate);
    }

    // this method is called just after the handler using this
    // formatter is created
    // public String getHead(Handler h) {
    //     return "<!DOCTYPE html>\n<head>\n<style>\n"
    //         + "table { width: 100% }\n"
    //         + "th { font:bold 10pt Tahoma; }\n"
    //         + "td { font:normal 10pt Tahoma; }\n"
    //         + "h1 {font:normal 11pt Tahoma;}\n"
    //         + "</style>\n"
    //         + "</head>\n"
    //         + "<body>\n"
    //         + "<h1>" + (new Date()) + "</h1>\n"
    //         + "<table border=\"0\" cellpadding=\"5\" cellspacing=\"3\">\n"
    //         + "<tr align=\"left\">\n"
    //         + "\t<th style=\"width:10%\">Loglevel</th>\n"
    //         + "\t<th style=\"width:15%\">Time</th>\n"
    //         + "\t<th style=\"width:75%\">Log Message</th>\n"
    //         + "</tr>\n";
    //   }

    // // this method is called just after the handler using this
    // // formatter is closed
    // public String getTail(Handler h) {
    //     return "</table>\n</body>\n</html>";
    // }
}

public class OCFLogger
{
    static private FileHandler fileTxt;
    static private SimpleFormatter formatterTxt;

    static private FileHandler fileHTML;
    static private Formatter formatterHTML;

    public static Logger LOGGER = Logger.getLogger(Logger.GLOBAL_LOGGER_NAME);

    static public void setup() throws IOException {
	// String path = OCFLogger.class
	//     .getClassLoader()
	//     .getResource("logger.properties")
	//     .getFile();
	// System.setProperty("java.util.logging.config.file", path);
	// LOGGER = Logger.getLogger(OCFLogger.class.getName());

        // get the global logger to configure it
        // LOGGER = Logger.getLogger(Logger.GLOBAL_LOGGER_NAME);

        // suppress the logging output to the console
        // Logger rootLogger = Logger.getLogger("");
        // Handler[] handlers = rootLogger.getHandlers();
        // if (handlers[0] instanceof ConsoleHandler) {
        //     rootLogger.removeHandler(handlers[0]);
        // }

        // LOGGER.setLevel(Level.INFO);
        // fileTxt = new FileHandler("Logging.txt");
        // fileOCF = new FileHandler("Logging.log");

        // create a TXT formatter
        // formatterTxt = new SimpleFormatter();
        // fileTxt.setFormatter(formatterTxt);
        // LOGGER.addHandler(fileTxt);

        // formatterOCF = new OCFFormatter();
        // fileHTML.setFormatter(formatterOCF);
        // logger.addHandler(fileHTML);
    }

    public static final HashMap errcodeMap;
    public static final HashMap observationTypes;
    public static final HashMap netProtocols;
    public static final HashMap netPolicy;
    public static final HashMap netScope;

    static {
	// try {
	//     setup();
	// } catch (Exception e) {
	//     System.out.println("SETUP FAIL");
	// }
	// System.out.println("logger SETUP complete");

	errcodeMap = new HashMap<Integer, String>();
        errcodeMap.put(0, "OK");
	errcodeMap.put(1, "RESOURCE_CREATED");
	errcodeMap.put(2, "RESOURCE_DELETED");
	errcodeMap.put(3, "CONTINUE");
	errcodeMap.put(4, "RESOURCE_CHANGED");
	errcodeMap.put(26, "INVALID_PARAM");
	errcodeMap.put(33, "RESOURCE NOT FOUND");
	errcodeMap.put(40, "INVALID_OPTION");
	errcodeMap.put(46, "UNAUTHORIZED_REQ");
	errcodeMap.put(52, "OBSERVER REGISTRATION FAILURE");
	errcodeMap.put(53, "RESOURCE UNOBSERVABLE");
	errcodeMap.put(54, "METHOD NOT ALLOWED");
	errcodeMap.put(255, "ERROR");
	errcodeMap.put(402, "CA_BAD_OPT");

	observationTypes = new HashMap<Integer, String>();
	observationTypes.put(ObservationRecord.PLATFORM, "PLATFORM");
	observationTypes.put(ObservationRecord.DEVICE, "DEVICE");
	observationTypes.put(ObservationRecord.REPRESENTATION, "REPRESENTATION");
	observationTypes.put(ObservationRecord.DISCOVERY, "DISCOVERY");
	observationTypes.put(ObservationRecord.SECURITY, "SECURITY");
	observationTypes.put(ObservationRecord.PRESENCE, "PRESENCE");
	observationTypes.put(ObservationRecord.RD, "RD");
	observationTypes.put(ObservationRecord.NOTIFICATION, "NOTIFICATION");

	netProtocols = new HashMap<Integer, String>();
	netProtocols.put(0, "OC_DEFAULT_ADAPTER");
	netProtocols.put(1, "OC_ADAPTER_IP");	     // = (1 << 0),
	netProtocols.put(2, "OC_ADAPTER_GATT_BTLE"); // = (1 << 1),
	netProtocols.put(3, "OC_ADAPTER_RFCOMM_BTEDR"); // = (1 << 2),
	/**Remote Access over XMPP.*/
	netProtocols.put(4, "OC_ADAPTER_REMOTE_ACCESS"); // = (1 << 3),
	/** CoAP over TCP.*/
	netProtocols.put(5, "OC_ADAPTER_TCP"); // = (1 << 4),
	netProtocols.put(6, "OC_ADAPTER_NFC"); // = (1 << 5)

	netPolicy    = new HashMap<Integer, String>();
	netPolicy.put( 0, "OC_DEFAULT_FLAGS"); // = 0,
	netPolicy.put( 0x10, "OC_FLAG_SECURE"); // (1 << 4)
	netPolicy.put( 0x20, "OC_IP_USE_V6");	// (1 << 5)
	netPolicy.put( 0x40, "OC_IP_USE_V4");	// (1 << 6)
	netPolicy.put( 0x80, "OC_MULTICAST");	// (1 << 7)

	netScope    = new HashMap<Integer, String>();
	netScope.put( 0,      "OC_SCOPE_NONE");
	netScope.put( 1,      "OC_SCOPE_INTERFACE");
	netScope.put( 2,      "OC_SCOPE_LINK");
	netScope.put( 3,      "OC_SCOPE_REALM");
	netScope.put( 4,      "OC_SCOPE_ADMIN");
	netScope.put( 5,      "OC_SCOPE_SITE");
	netScope.put( 8,      "OC_SCOPE_ORG");
	netScope.put( 0xE,      "OC_SCOPE_GLOBAL");
    }

    static public void logCoAddress(Endpoint ep)
    {
	System.out.println("LOGGING CO-ADDRESS:");
	// Endpoint ep = cosp.getEndpoint();

	if (ep != null) {
	    try {
		System.out.println("APP_LOGGER IPAddress\t IP address:\t\t" + ep.getIPAddress());
		System.out.println("APP_LOGGER IPAddress\t port:\t\t\t" + ep.getPort());

		// System.out.println("APP_LOGGER IPAddress\t network protocol:\t"
		// 		   + String.format("0x%04X", ep.getAdapter()
		// 				   & 0xFFFFF)
		// 		   + " " + netProtocols.get(ep.getAdapter()));

		// System.out.println("APP_LOGGER IPAddress\t network flags:\t\t"
		// 		   + String.format("0x%04X", ep.getNetworkFlags()
		// 				   & 0xFFFFF));

		// System.out.println("APP_LOGGER IPAddress\t IPv4?\t\t\t" + ep.isNetworkIPv4());

		// System.out.println("APP_LOGGER IPAddress\t IPv6?\t\t\t" + ep.isNetworkIPv6());

		// System.out.println("APP_LOGGER IPAddress\t Multicast?\t\t" + ep.isRoutingMulticast());

		// System.out.println("APP_LOGGER IPAddress\t nework scope:\t\t"
		// 		   + String.format("0x%02X", ep.getNetworkScope())
		// 		   + " " + netScope.get((int)ep.getNetworkScope()));

		// System.out.println("APP_LOGGER IPAddress\t transport security:\t"
		// 		   + ep.isTransportSecure());

		// // int scope = (ep.networkPolicy >> 4) & 0x000F;
		// // System.out.println("APP_LOGGER IPAddress\t nework scope:\t\t"
		// // 		       // + String.format("0x%02X", scope)
		// // 		       + netPolicy.get(scope));

		// // String sec = (0 == (ep.networkPolicy & 0x0010))? "OFF" : "ON";
		// // System.out.println("APP_LOGGER IPAddress\t transport security:\t" + sec);

		// System.out.println("APP_LOGGER IPAddress\t ifindex:\t\t" + ep.getIfIndex());
		// // System.out.println("REQUEST IN: devaddr route data: " + ep.routeData);


		// // System.out.println("APP_LOGGER IPAddress route data: " + ep.routeData);
	    } catch (NullPointerException e) {
		System.out.println("Device Address is NULL");
	    }
	} else {
	    System.out.println("Device Address is NULL");
	}
    }

    // static public void logResourcePolicies(ResourceLocal resource)
    // {
    // 	System.out.println("RESOURCE: policies: "
    // 			   + String.format("0x%X", resource.policies & 0xFFFFF));
    // 	if ( (resource.policies & ResourcePolicy.DISCOVERABLE) > 0 )
    // 	    {System.out.println("\tDISCOVERABLE");}
    // 	if ( (resource.policies & ResourcePolicy.ACTIVE) > 0 ) {System.out.println("\tACTIVE");}
    // 	if ( (resource.policies & ResourcePolicy.OBSERVABLE) > 0) {System.out.println("\tOBSERVABLE");}
    // 	if ( (resource.policies & ResourcePolicy.SECURE) > 0) {System.out.println("\tSECURE");}
    // }

    static public void logEndpoint(Endpoint ep)
    {
	System.out.println("APP_LOGGER isTransportUDP?\t\t" + ep.isTransportUDP());
	System.out.println("APP_LOGGER isTransportTCP?\t\t" + ep.isTransportTCP());
	System.out.println("APP_LOGGER isTransportGATT?\t\t" + ep.isTransportGATT());
	System.out.println("APP_LOGGER isTransportRFCOMM?\t" + ep.isTransportRFCOMM());
	System.out.println("APP_LOGGER isTransportNFC?\t\t" + ep.isTransportNFC());
	System.out.println("APP_LOGGER isNetworkIP?\t\t" + ep.isNetworkIP());
	System.out.println("APP_LOGGER isNetworkIPv4?\t\t" + ep.isNetworkIPv4());
	System.out.println("APP_LOGGER isNetworkIPv6?\t\t" + ep.isNetworkIPv6());
	System.out.println("APP_LOGGER isScopeInterface?\t" + ep.isScopeInterface());
	System.out.println("APP_LOGGER isScopeLink?\t\t" + ep.isScopeLink());
	System.out.println("APP_LOGGER isTransportSecure?\t" + ep.isTransportSecure());
	System.out.println("APP_LOGGER isRoutingMulticast?\t" + ep.isRoutingMulticast());
   }

    static public void testNetworking(Endpoint ep)
    {
	boolean torf;
	torf = ep.isTransportUDP();
	System.out.println("PRE  isTransportUDP? " + ep.isTransportUDP());
	ep.setTransportUDP(true);
	System.out.println("POST isTransportUDP? (t) " + ep.isTransportUDP());
	ep.setTransportUDP(false);
	System.out.println("POST isTransportUDP? (f) " + ep.isTransportUDP());
	ep.setTransportUDP(torf);

	torf = ep.isTransportTCP();
	System.out.println("PRE isTransportTCP? " + ep.isTransportTCP());
	ep.setTransportTCP(true);
	System.out.println("POST isTransportTCP? (t) " + ep.isTransportTCP());
	ep.setTransportTCP(false);
	System.out.println("POST isTransportTCP? (f) " + ep.isTransportTCP());
	ep.setTransportTCP(torf);

	torf = ep.isTransportGATT();
	System.out.println("PRE isTransportGATT? " + ep.isTransportGATT());
	ep.setTransportGATT(true);
	System.out.println("POST isTransportGATT? (t) " + ep.isTransportGATT());
	ep.setTransportGATT(false);
	System.out.println("POST isTransportGATT? (f) " + ep.isTransportGATT());
	ep.setTransportGATT(torf);

	torf = ep.isTransportRFCOMM();
	System.out.println("PRE isTransportRFCOMM? " + ep.isTransportRFCOMM());
	ep.setTransportRFCOMM(true);
	System.out.println("POST isTransportRFCOMM? (t) " + ep.isTransportRFCOMM());
	ep.setTransportRFCOMM(false);
	System.out.println("POST isTransportRFCOMM? (f) " + ep.isTransportRFCOMM());
	ep.setTransportRFCOMM(torf);

	torf = ep.isTransportNFC();
	System.out.println("PRE isTransportNFC? " + ep.isTransportNFC());
	ep.setTransportNFC(true);
	System.out.println("POST isTransportNFC? (t) " + ep.isTransportNFC());
	ep.setTransportNFC(false);
	System.out.println("POST isTransportNFC? (f) " + ep.isTransportNFC());
	ep.setTransportNFC(torf);

	System.out.println("PRE isNetworkIP? " + ep.isNetworkIP());
	torf = ep.isTransportUDP();
	ep.setTransportUDP(true); // implies IP
	System.out.println("POST isNetworkIP? (t) " + ep.isNetworkIP());
	ep.setTransportUDP(false);
	System.out.println("POST isNetworkIP? (f) " + ep.isNetworkIP());
	ep.setTransportUDP(torf);

	torf = ep.isTransportTCP();
	ep.setTransportTCP(true); // implies IP
	System.out.println("POST isNetworkIP? (t) " + ep.isNetworkIP());
	ep.setTransportTCP(false);
	System.out.println("POST isNetworkIP? (f) " + ep.isNetworkIP());
	ep.setTransportTCP(torf);

	torf = ep.isNetworkIPv4();
	System.out.println("PRE isNetworkIPv4? " + ep.isNetworkIPv4());
	ep.setNetworkIPv4(true);
	System.out.println("POST isNetworkIPv4? (t) " + ep.isNetworkIPv4());
	ep.setNetworkIPv4(false);
	System.out.println("POST isNetworkIPv4? (f) " + ep.isNetworkIPv4());
	ep.setNetworkIPv4(torf);

	torf = ep.isNetworkIPv6();
	System.out.println("PRE isNetworkIPv6? " + ep.isNetworkIPv6());
	ep.setNetworkIPv6(true);
	System.out.println("POST isNetworkIPv6? (t) " + ep.isNetworkIPv6());
	ep.setNetworkIPv6(false);
	System.out.println("POST isNetworkIPv6? (f) " + ep.isNetworkIPv6());
	ep.setNetworkIPv6(torf);

	torf = ep.isScopeInterface();
	System.out.println("PRE isScopeInterface? " + ep.isScopeInterface());
	ep.setScopeInterface(true);
	System.out.println("POST isScopeInterface? (t) " + ep.isScopeInterface());
	ep.setScopeInterface(false);
	System.out.println("POST isScopeInterface? (f) " + ep.isScopeInterface());
	ep.setScopeInterface(torf);

	torf = ep.isScopeLink();
	System.out.println("PRE isScopeLink? " + ep.isScopeLink());
	ep.setScopeLink(true);
	System.out.println("POST isScopeLink? (t) " + ep.isScopeLink());
	ep.setScopeLink(false);
	System.out.println("POST isScopeLink? (f) " + ep.isScopeLink());
	ep.setScopeLink(torf);

	torf = ep.isTransportSecure();
	System.out.println("PRE isTransportSecure? " + ep.isTransportSecure());
	ep.setTransportSecure(true);
	System.out.println("POST isTransportSecure? (t) " + ep.isTransportSecure());
	ep.setTransportSecure(false);
	System.out.println("POST isTransportSecure? (f) " + ep.isTransportSecure());
	ep.setTransportSecure(torf);

	torf = ep.isRoutingMulticast();
	System.out.println("PRE isRoutingMulticast? " + ep.isRoutingMulticast());
	ep.setRoutingMulticast(true);
	System.out.println("POST isRoutingMulticast? (t) " + ep.isRoutingMulticast());
	ep.setRoutingMulticast(false);
	System.out.println("POST isRoutingMulticast? (f) " + ep.isRoutingMulticast());
	ep.setRoutingMulticast(torf);
    }

    static public void logCoSP(CoResourceSP cosp)
    {
	// System.out.println("CoSP URI PATH: " + cosp.getUriPath());
	// logDeviceAddress(cosp.coAddress());

	// System.out.println("CoSP network protocol: "
	// 		   + String.format("0x%04X",
	// 				   cosp.networkProtocol() & 0xFFFFF));
	// System.out.println("CoSP network Scope: "    + cosp.networkScope());
	// System.out.println("CoSP network Policies: "
	// 			       + String.format("0x%04X",
	// 					       cosp.networkPolicies() & 0xFFFFF));
	// System.out.println("CoSP transport secure?: " + cosp.isTransportSecure());

	// List<String> ts = cosp.getTypes();
	// System.out.println("SP Types:");
	// ts.forEach(typ -> System.out.println("\t" + typ));

	// List<String> ifs = cosp.getInterfaces();
	// System.out.println("SP Interfaces");
	// ifs.forEach(iface -> System.out.println("\t" + iface));
    }

    static public void logSP(IResourceSP theSP)
    {
	System.out.println("SP URI PATH: " + theSP.getUriPath());
	List<String> ts = theSP.getTypes();
	System.out.println("SP Types:");
	ts.forEach(typ -> System.out.println("\t" + typ));

	List<String> ifs = theSP.getInterfaces();
	System.out.println("SP Interfaces");
	ifs.forEach(iface -> System.out.println("\t" + iface));
    }

    // static public void logResource(ResourceLocal resource)
    // {
    // 	System.out.println("RESOURCE: logResource ENTRY");
    // 	System.out.println("RESOURCE: resource uri: " + resource.getUri());

    // 	List<String> tll = resource.getTypes();
    // 	tll.forEach(t -> System.out.println("RESOURCE: type:     " + t)); // Java 8

    // 	List<String> ifll = resource.getInterfaces();
    // 	ifll.forEach(iface -> System.out.println("RESOURCE: interface: " + iface));
    // 	// for (int i = 0; i < ifll.size(); i++) {   // Java 7
    // 	//     System.out.println("REQUEST IN: resource if:    " + ifll.get(i));
    // 	// }

    // 	List<PropertyString> pll = resource.getProperties();
    // 	System.out.println("RESOURCE: properties count: " + pll.size());
    // 	pll.forEach(p -> System.out.println("RESOURCE: property: " + p.name + " = " + p.value));

    // 	List<ResourceLocal> cll = resource.getChildren();
    // 	System.out.println("RESOURCE: child resources count: " + cll.size());
    // 	cll.forEach(ch -> System.out.println("RESOURCE: child resource: " + ch));

    // 	System.out.println("RESOURCE: service provider (callback): "
    // 			   + resource.getServiceProvider().getClass().getName());

    // 	System.out.println("RESOURCE: callback param: "
    // 			   + resource.getCallbackParam().getClass().getName());

    // 	System.out.println("RESOURCE: serial number: " + resource.sn);

    // 	// Instance Id
    // 	// System.out.println("RESOURCE: resource instance id: " + resource.id.getClass().getName());
    // 	if (resource.id.getClass().getName().equals("openocf.Resource$InstanceId")) {
    // 	    System.out.println("RESOURCE: resource InstanceId class: InstanceId");
    // 	} else if (resource.id.getClass().getName().equals("openocf.Resource$InstanceOrdinal")) {
    // 	    System.out.println("RESOURCE: resource InstanceId class: InstanceOrdinal, val="
    // 			       + ((openocf.ResourceLocal.InstanceOrdinal)resource.id).val);
    // 	} else if (resource.id.getClass().getName().equals("openocf.ResourceLocal$InstanceString")) {
    // 	    System.out.println("RESOURCE: resource InstanceId class: InstanceString, val="
    // 			       + ((openocf.ResourceLocal.InstanceString)resource.id).val);
    // 	} else if (resource.id.getClass().getName().equals("openocf.ResourceLocal$InstanceUuid")) {
    // 	    System.out.println("RESOURCE: resource InstanceId class: InstanceUuid, val="
    // 			       + ((openocf.ResourceLocal.InstanceUuid)resource.id).val);
    // 	}

    // 	logResourcePolicies(resource);

    // 	try {
    // 	    System.out.println("RESOURCE: action set: " + resource.getActionSet());
    // 	} catch (OCFNotImplementedException e) {
    // 	    System.out.println("ERROR**** RESOURCE: getActionSet not implemented.");
    // 	}
    // 	System.out.println("RESOURCE: logResource EXIT");
    // }

    // static public void logObservation(ObservationRecord observationRecord)
    // {
    // 	// System.out.println("OBSERVATION: logObservation ENTRY");

    // 	System.out.println("\tOBSERVED uri: " + observationRecord.getUriPath());

    // 	System.out.println("\tOBSERVED type: " + observationRecord.getType());

    // 	// log rtypes
    // 	List<String> rtypes = observationRecord.getResourceTypes();
    // 	System.out.println("\tOBSERVED RESOURCE TYPES count: " + rtypes.size());
    // 	for (String t : (List<String>)rtypes) {
    // 	    System.out.println("\tOBSERVED rtype: " + t);
    // 	}

    // 	// log interfaces
    // 	List<String> ifaces = observationRecord.getInterfaces();
    // 	System.out.println("\tOBSERVED INTERFACES count: " + ifaces.size());
    // 	for (String iface : ifaces) {
    // 	    System.out.println("\tOBSERVED interface: " + iface);
    // 	}

    // 	// log properties (PlatformInfo, DeviceInfo, or "values" for resources)
    // 	PropertyMap<String, Object> pmap = observationRecord.getProperties();
    // 	System.out.println("\tOBSERVED PROPERTIES count: " + pmap.size());
    // 	for (Map.Entry<String, Object> entry : pmap.entrySet())
    // 	    {
    // 		System.out.println("\tOBSERVED property: "
    // 				   + entry.getKey()
    // 				   + " = "
    // 				   + entry.getValue());
    // 	    }
    // 	List<IObservationRecord> kids = observationRecord.getChildren();
    // 	if (kids != null) {
    // 	    System.out.println("\tOBSERVED CHILDREN count: " + kids.size());
    // 	    for (IObservationRecord p : kids) {
    // 		System.out.println("================ CHILD");
    // 		logObservation((ObservationRecord)p);
    // 	    }
    // 	}
    // }

    // static public void logInboundRequest(InboundRequest requestIn)
    // {
    // 	System.out.println("LOG InboundRequest logInboundRequest ENTRY");
    // 	// System.out.println("LOG InboundRequest this handle: " + requestIn.localHandle);
    // 	// System.out.println("LOG InboundRequest remote handle: " + requestIn.getRemoteHandle);
    // 	// System.out.println("LOG InboundRequest resource handle: " + requestIn.getResourceHandle());
    // 	System.out.println("LOG InboundRequest request method: " + requestIn.getMethod());
    // 	System.out.println("LOG InboundRequest query : \"" + requestIn.getQueryString() + "\"");
    // 	System.out.println("LOG InboundRequest msg id : " + requestIn.getMessageId());

    // 	System.out.println("LOG InboundRequest method : " + requestIn.getMethod());
    // 	System.out.println("LOG InboundRequest watch action : " + requestIn.watchAction);
    // 	System.out.println("LOG InboundRequest watch id :     " + requestIn.watchId);


    // 	// ResourceLocal resource = requestIn.getResource();
    // 	// logResource(resource);

    // 	// logDeviceAddress(requestIn.getRemoteDeviceAddress());

    // 	// System.out.println("LOG InboundRequest watch action: " + requestIn.watchAction);
    // 	// System.out.println("LOG InboundRequest watch id    : " + requestIn.watchId);

    // 	List<HeaderOption> headerOptions = requestIn.getOptions();
    // 	if (headerOptions != null)
    // 	    System.out.println("LOG InboundRequest header opts ct: " + headerOptions.size());

    // 	// ObservationList<PayloadForResourceState> observation = requestIn.getPDUPayload();
    // 	// if (observation == null) {
    // 	//     System.out.println("LOG InboundRequest observation is null");
    // 	// }
    // }

    static public void logInboundRequest(InboundRequest req)
    {
	System.out.println("APP_LOGGER logInboundRequest ENTRY, thread "
			   + Thread.currentThread().getId());

	//FIXME	System.out.println("APP_LOGGER: stack result: " + cosp.getCoResult());

	System.out.println("APP_LOGGER CoSP uri path:\t" + req.getUri());
	// System.out.println("APP_LOGGER CoSP method:\t" + cosp.getMethod());
	// System.out.println("APP_LOGGER CoSP conn type:\t" + cosp.connType());
	//FIXME System.out.println("APP_LOGGER CoSP sec ID:\t"
	// 		   + Arrays.toString(cosp.getCoSecurityId()));
	//FIXME System.out.println("APP_LOGGER CoSP serial:\t" + cosp.getNotificationSerial());

	//	logCoAddress(cosp);

	// List<HeaderOption> headerOptions = cosp.getOptions();
	// if (headerOptions != null)
	//     System.out.println("LOG InboundRequest header options ct: " + headerOptions.size());

	// FIXME:
	// if (cosp.result == OCStackResult.OK) {

	//     System.out.println("APP_LOGGER CoSP OBSERVATIONS:");
	//     // System.out.println("APP_LOGGER CoSP OBSERVATION type: "
	//     // 		       + cosp.getObservationType()
	//     // 		       + ": "
	//     // 		       + observationTypes.get(cosp.getObservationType()));

	//     ObservationList<ObservationRecord> observationRecords = cosp.getObservationRecords();
	//     if (observationRecords != null) {
	// 	System.out.println("LOG OBSERVATIONRECORD count: " + observationRecords.size());
	// 	for (ObservationRecord observationRecord : (ObservationList<ObservationRecord>) observationRecords) {
	// 	    List<IObservationRecord> kids = observationRecord.getChildren();
	// 	    if (kids != null) {
	// 		System.out.println("LOG CHILD OBSERVATIONS count: "
	// 				   + observationRecord.getChildren().size());
	// 	    }
	// 	    // logObservation(observation);
	// 	}
	//     }
	// }
    }

    static public void logOutboundRequest(OutboundRequest req)
    {
	System.out.println("APP_LOGGER logOutboundRequest ENTRY, thread "
			   + Thread.currentThread().getId());

	System.out.println("APP_LOGGER Req uri path:\t" + req.getUri());
	System.out.println("APP_LOGGER Req method:\t" + req.getMethod());
	// System.out.println("APP_LOGGER Req conn type:\t" + req.connType());

	// List<HeaderOption> headerOptions = req.getOptions();
	// if (headerOptions != null)
	//     System.out.println("LOG InboundRequest header options ct: " + headerOptions.size());

	// FIXME:
	// if (req.result == OCStackResult.OK) {

	//     System.out.println("APP_LOGGER Req OBSERVATIONS:");
	//     // System.out.println("APP_LOGGER Req OBSERVATION type: "
	//     // 		       + req.getObservationType()
	//     // 		       + ": "
	//     // 		       + observationTypes.get(req.getObservationType()));

	//     ObservationList<ObservationRecord> observationRecords = req.getObservationRecords();
	//     if (observationRecords != null) {
	// 	System.out.println("LOG OBSERVATIONRECORD count: " + observationRecords.size());
	// 	for (ObservationRecord observationRecord : (ObservationList<ObservationRecord>) observationRecords) {
	// 	    List<IObservationRecord> kids = observationRecord.getChildren();
	// 	    if (kids != null) {
	// 		System.out.println("LOG CHILD OBSERVATIONS count: "
	// 				   + observationRecord.getChildren().size());
	// 	    }
	// 	    // logObservation(observation);
	// 	}
	//     }
	// }
    }

    static public void logInboundResponseMap(Map rMap)
    {
	System.out.println("APP_LOGGER logInboundResponseMap ENTRY, thread "
			   + Thread.currentThread().getId());
	System.out.println("Response Map size: " + rMap.size());
	Map devAddr = (Map)rMap.get(OpenOCF.DEVADDR);
	System.out.println("rmap uri: " + rMap.get(OpenOCF.URI));
	System.out.println("rmap devaddr:");
	System.out.println("\taddress: " + devAddr.get(OpenOCF.ADDRESS));
	System.out.println("\tport: " + devAddr.get(OpenOCF.PORT));
	System.out.println("\ttransport adapter: "
			   + String.format("0x%08X", (Integer)devAddr.get(OpenOCF.TRANSPORT_ADAPTER)));
	System.out.println("\ttransport flags: "
			   + String.format("0x%08X", (Integer)devAddr.get(OpenOCF.TRANSPORT_FLAGS)));
	System.out.println("\tindex: " + devAddr.get(OpenOCF.INDEX));
	System.out.println("\tsecure? " + devAddr.get(OpenOCF.SECURE));
	System.out.println("\tmcast?: " + devAddr.get(OpenOCF.MCAST));

	System.out.println("rmap connectivity type: "
			   + String.format("0x%08X", (int)rMap.get(OpenOCF.CONN_TYPE)));

	System.out.println("rmap transport: " + rMap.get(OpenOCF.TRANSPORT));
	System.out.println("rmap udp? " + rMap.get(OpenOCF.UDP));
	System.out.println("rmap tcp? " + rMap.get(OpenOCF.TCP));
	System.out.println("rmap ipv4? " + rMap.get(OpenOCF.IPV4));
	System.out.println("rmap ipv6? " + rMap.get(OpenOCF.IPV6));
	System.out.println("rmap scope: " + rMap.get(OpenOCF.SCOPE));
	System.out.println("rmap secure? " + rMap.get(OpenOCF.SECURE));

	System.out.println("rmap remote result: " + rMap.get(OpenOCF.REMOTE_RESULT));
	System.out.println("rmap remote identity: " + rMap.get(OpenOCF.REMOTE_IDENTITY));
	System.out.println("rmap serial: " + rMap.get(OpenOCF.SERIAL));

	ArrayList options = (ArrayList)rMap.get(OpenOCF.COAP_OPTIONS);
	System.out.println("CoAP options (count: " + options.size() + ")");
	for (HashMap option: (List<HashMap>)options) {
	    for (Entry<Object,Object> opt : (Set<Entry<Object,Object>>)option.entrySet()){
	    	//iterate over the pairs
	    	System.out.println("\t" + opt.getKey()+": " + opt.getValue());
	    }
	}

	// PAYLOAD
	System.out.println("Payload type: " + rMap.get(OpenOCF.PAYLOAD_TYPE));
	Map payloadMap = (Map)rMap.get(OpenOCF.PAYLOAD);
	System.out.println("Payload size: " + payloadMap.size());
	System.out.println("Payload di: " + payloadMap.get(OpenOCF.OCF_DI));
	System.out.println("Payload name: " + payloadMap.get(OpenOCF.OCF_NAME));

	List ifaces = (List)payloadMap.get(OpenOCF.OCF_IF);
	System.out.println("Payload interface count: " + ifaces.size());
 	for (String iface: (List<String>)ifaces) {
	    System.out.println("Payload if: " + iface);
	}

	List rts = (List)payloadMap.get(OpenOCF.OCF_RT);
	System.out.println("Payload rt count: " + rts.size());
 	for (String rt: (List<String>)rts) {
	    System.out.println("Payload rt: " + rt);
	}

	List links = (List)payloadMap.get(OpenOCF.OCF_LINKS);
	System.out.println("Links count: " + links.size());
	Map link;
	for (int i = 0; i < links.size(); i++) {
	    link = (Map)links.get(i);
	    System.out.println("Link " + i);
	    System.out.println("\thref: " + link.get(OpenOCF.OCF_HREF));
	    // port/sec: only if OIC 1.1, not OCF 1.0
	    // System.out.println("\tport: " + link.get(OpenOCF.OCF_PORT));
	    // System.out.println("\ttcp port: " + link.get(OpenOCF.TCP_PORT));

	    // buri not in Iotivity?
	    // System.out.println("\tbase uri: " + link.get(OpenOCF.OCF_BURI));

	    System.out.println("\trel: " + link.get(OpenOCF.OCF_LINK_RELATION));
	    System.out.println("\tanchor: " + link.get(OpenOCF.OCF_ANCHOR));

	    // rt (resource types List<String>)
	    rts = (List)link.get(OpenOCF.OCF_RT);
	    System.out.println("\ttypes (count " + rts.size() + ")");
	    for (String rt: (List<String>)rts) {
		System.out.println("\t\t" + rt);
	    }

	    // if (interfaces List<String>)
	    ifaces = (List)link.get(OpenOCF.OCF_IF);
	    System.out.println("\tinterfaces (count " + ifaces.size() + ")");
	    for (String iface: (List<String>)ifaces) {
		System.out.println("\t\t" + iface);
	    }

	    System.out.println("\tpolicy bitmask "
			       + String.format("0x%08X", (int)link.get(OpenOCF.OCF_POLICY_BITMASK)));
	    System.out.println("\t\tdiscoverable? " + link.get(OpenOCF.DISCOVERABLE));
	    System.out.println("\t\tobservable? " + link.get(OpenOCF.OBSERVABLE));

	    // eps (List<Map>)
	    List eps = (List)link.get(OpenOCF.OCF_EPS);
	    System.out.println("\tEndpoints (count " + eps.size() + ")");
	    String epstr;
	    String delimOpen, delimClose;
	    for (Map ep: (List<Map>)eps) {
		delimOpen = (Boolean)ep.get(OpenOCF.IPV6)? "[" : "";
		delimClose = (Boolean)ep.get(OpenOCF.IPV6)? "]" : "";
		epstr = ep.get(OpenOCF.OCF_TPS) + "://"
		    + delimOpen
		    + ep.get(OpenOCF.OCF_ADDR)
		    + delimClose
		    + ":" + ep.get(OpenOCF.OCF_PORT);
		System.out.println("\t\tep: " + epstr);
		System.out.println("\t\ttps: " + ep.get(OpenOCF.OCF_TPS));
		System.out.println("\t\taddr: " + ep.get(OpenOCF.OCF_ADDR));
		System.out.println("\t\tport: " + ep.get(OpenOCF.OCF_PORT));
		System.out.println("\t\ttransport flags: "
				   + String.format("0x%08X", (int)ep.get(OpenOCF.TRANSPORT_FLAGS)));
		System.out.println("\t\tIPv4? " + ep.get(OpenOCF.IPV4));
		System.out.println("\t\tIPv6? " + ep.get(OpenOCF.IPV6));
		System.out.println("\t\tSecure? " + ep.get(OpenOCF.SECURE));
		System.out.println("\t\tMcast? " + ep.get(OpenOCF.MCAST));
		System.out.println("\t\tpriority: " + ep.get(OpenOCF.OCF_PRIORITY));
	    }
	}
    }

    static public void logInboundResponse(InboundResponse response)
    {
	System.out.println("APP_LOGGER logInboundResponse ENTRY, thread "
			   + Thread.currentThread().getId());

	//FIXME System.out.println("APP_LOGGER stack result: " + response.getCoResult());

	System.out.println("APP_LOGGER Response uri path:\t" + response.getUri());

	// inbound messages (OCClientResponse) do not contain method
	// System.out.println("APP_LOGGER Response method:\t" + response.getMethod());

	// System.out.println("APP_LOGGER Response conn type:\t" + response.connType());
	// System.out.println("APP_LOGGER Response sec ID:\t"
	//FIXME 		   + Arrays.toString(response.getCoSecurityId()));
	//FIXME System.out.println("APP_LOGGER Response serial:\t" + response.getNotificationSerial());

	logCoAddress(response.getEndpoint());

	// List<HeaderOption> headerOptions = req.getOptions();
	// if (headerOptions != null)
	//     System.out.println("LOG InboundRequest header options ct: " + headerOptions.size());

	// FIXME:
	// if (result.cosp == OCStackResult.OK) {

	//     System.out.println("APP_LOGGER CoSP OBSERVATIONS:");
	//     // System.out.println("APP_LOGGER CoSP OBSERVATION type: "
	//     // 		       + cosp.getObservationType()
	//     // 		       + ": "
	//     // 		       + observationTypes.get(cosp.getObservationType()));

	//     ObservationList<ObservationRecord> observationRecords = cosp.getObservationRecords();
	//     if (observationRecords != null) {
	// 	System.out.println("LOG OBSERVATIONRECORD count: " + observationRecords.size());
	// 	for (ObservationRecord observationRecord : (ObservationList<ObservationRecord>) observationRecords) {
	// 	    List<IObservationRecord> kids = observationRecord.getChildren();
	// 	    if (kids != null) {
	// 		System.out.println("LOG CHILD OBSERVATIONS count: "
	// 				   + observationRecord.getChildren().size());
	// 	    }
	// 	    // logObservation(observation);
	// 	}
	//     }
	// }
    }
}
