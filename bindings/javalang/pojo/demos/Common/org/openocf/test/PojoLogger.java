package org.openocf.test;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import openocf.OpenOCF;
import openocf.Endpoint;
import openocf.engine.OCFCommonSP;
import openocf.app.CoResourceSP;
import openocf.observation.ObservationRecord;
import openocf.message.CoAPOption;
import openocf.message.InboundRequest;
import openocf.message.InboundResponse;
import openocf.message.OutboundRequest;
import openocf.message.OutboundResponse;

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
//import java.util.logging.Logger;
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

public class PojoLogger
{
    static private FileHandler fileTxt;
    static private SimpleFormatter formatterTxt;

    static private FileHandler fileHTML;
    static private Formatter formatterHTML;

    public static final Logger LOGGER = LoggerFactory.getLogger(PojoLogger.class);

    // public static Logger LOGGER = Logger.getLogger(Logger.GLOBAL_LOGGER_NAME);

    static public void setup() throws IOException {
	// String path = PojoLogger.class
	//     .getClassLoader()
	//     .getResource("logger.properties")
	//     .getFile();
	// System.setProperty("java.util.logging.config.file", path);
	// LOGGER = Logger.getLogger(PojoLogger.class.getName());

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
	//     PojoLogger.LOGGER.info("SETUP FAIL");
	// }
	// PojoLogger.LOGGER.info("logger SETUP complete");

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
	PojoLogger.LOGGER.info("LOGGING CO-ADDRESS:");
	// Endpoint ep = cosp.getEndpoint();

	if (ep != null) {
	    try {
		PojoLogger.LOGGER.info("\tIP address:\t\t" + ep.getIPAddress());
		PojoLogger.LOGGER.info("\tport:\t\t\t" + ep.getPort());

		// PojoLogger.LOGGER.info("\tnetwork protocol: "
		// 		   + String.format("0x%04X", ep.getAdapter()
		// 				   & 0xFFFFF)
		// 		   + " " + netProtocols.get(ep.getAdapter()));

		// PojoLogger.LOGGER.info("\tnetwork flags: "
		// 		   + String.format("0x%04X", ep.getNetworkFlags()
		// 				   & 0xFFFFF));

		// PojoLogger.LOGGER.info("\tIPv4?\t\t\t" + ep.isNetworkIPv4());

		// PojoLogger.LOGGER.info("\tIPv6?\t\t\t" + ep.isNetworkIPv6());

		// PojoLogger.LOGGER.info("\tMulticast?\t\t" + ep.isRoutingMulticast());

		// PojoLogger.LOGGER.info(" IPAddress\t nework scope:\t\t"
		// 		   + String.format("0x%02X", ep.getNetworkScope())
		// 		   + " " + netScope.get((int)ep.getNetworkScope()));

		// PojoLogger.LOGGER.info(" IPAddress\t transport security:\t"
		// 		   + ep.isTransportSecure());

		// // int scope = (ep.networkPolicy >> 4) & 0x000F;
		// // PojoLogger.LOGGER.info(" IPAddress\t nework scope:\t\t"
		// // 		       // + String.format("0x%02X", scope)
		// // 		       + netPolicy.get(scope));

		// // String sec = (0 == (ep.networkPolicy & 0x0010))? "OFF" : "ON";
		// // PojoLogger.LOGGER.info(" IPAddress\t transport security:\t" + sec);

		// PojoLogger.LOGGER.info(" IPAddress\t ifindex:\t\t" + ep.getIfIndex());
		// // PojoLogger.LOGGER.info("REQUEST IN: devaddr route data: " + ep.routeData);


		// // PojoLogger.LOGGER.info(" IPAddress route data: " + ep.routeData);
	    } catch (NullPointerException e) {
		PojoLogger.LOGGER.info("Device Address is NULL");
	    }
	} else {
	    PojoLogger.LOGGER.info("Device Address is NULL");
	}
        return;
    }

    // static public void logResourcePolicies(ResourceLocal resource)
    // {
    // 	PojoLogger.LOGGER.info("RESOURCE: policies: "
    // 			   + String.format("0x%X", resource.policies & 0xFFFFF));
    // 	if ( (resource.policies & ResourcePolicy.DISCOVERABLE) > 0 )
    // 	    {PojoLogger.LOGGER.info("\tDISCOVERABLE");}
    // 	if ( (resource.policies & ResourcePolicy.ACTIVE) > 0 ) {PojoLogger.LOGGER.info("\tACTIVE");}
    // 	if ( (resource.policies & ResourcePolicy.OBSERVABLE) > 0) {PojoLogger.LOGGER.info("\tOBSERVABLE");}
    // 	if ( (resource.policies & ResourcePolicy.SECURE) > 0) {PojoLogger.LOGGER.info("\tSECURE");}
    // }

    static public void logEndpoint(Endpoint ep)
    {
        PojoLogger.LOGGER.info("IP address:\t" + ep.getIPAddress());
        PojoLogger.LOGGER.info("port:\t\t" + ep.getPort());
	PojoLogger.LOGGER.info("isTransportUDP?\t\t" + ep.isTransportUDP());
	PojoLogger.LOGGER.info("isTransportTCP?\t\t" + ep.isTransportTCP());
	PojoLogger.LOGGER.info("isTransportGATT?\t" + ep.isTransportGATT());
	PojoLogger.LOGGER.info("isTransportRFCOMM?\t" + ep.isTransportRFCOMM());
	PojoLogger.LOGGER.info("isTransportNFC?\t\t" + ep.isTransportNFC());
	PojoLogger.LOGGER.info("isNetworkIPv4?\t\t" + ep.isNetworkIPv4());
	PojoLogger.LOGGER.info("isNetworkIPv6?\t\t" + ep.isNetworkIPv6());
	PojoLogger.LOGGER.info("isScopeInterface?\t" + ep.isScopeInterface());
	PojoLogger.LOGGER.info("isScopeLink?\t\t" + ep.isScopeLink());
        PojoLogger.LOGGER.info("isTransportSecure?\t" + ep.isTransportSecure());
	PojoLogger.LOGGER.info("isRoutingMulticast?\t" + ep.isRoutingMulticast());
	PojoLogger.LOGGER.info("remote ID:\t\t" + ep.getRemoteId());
	PojoLogger.LOGGER.info("ifindex:\t\t" + ep.getIfIndex() + " " + ep.getIfName());
   }

    static public void testNetworking(Endpoint ep)
    {
	boolean torf;
	torf = ep.isTransportUDP();
	PojoLogger.LOGGER.info("PRE  isTransportUDP? " + ep.isTransportUDP());
	ep.setTransportUDP(true);
	PojoLogger.LOGGER.info("POST isTransportUDP? (t) " + ep.isTransportUDP());
	ep.setTransportUDP(false);
	PojoLogger.LOGGER.info("POST isTransportUDP? (f) " + ep.isTransportUDP());
	ep.setTransportUDP(torf);

	torf = ep.isTransportTCP();
	PojoLogger.LOGGER.info("PRE isTransportTCP? " + ep.isTransportTCP());
	ep.setTransportTCP(true);
	PojoLogger.LOGGER.info("POST isTransportTCP? (t) " + ep.isTransportTCP());
	ep.setTransportTCP(false);
	PojoLogger.LOGGER.info("POST isTransportTCP? (f) " + ep.isTransportTCP());
	ep.setTransportTCP(torf);

	torf = ep.isTransportGATT();
	PojoLogger.LOGGER.info("PRE isTransportGATT? " + ep.isTransportGATT());
	ep.setTransportGATT(true);
	PojoLogger.LOGGER.info("POST isTransportGATT? (t) " + ep.isTransportGATT());
	ep.setTransportGATT(false);
	PojoLogger.LOGGER.info("POST isTransportGATT? (f) " + ep.isTransportGATT());
	ep.setTransportGATT(torf);

	torf = ep.isTransportRFCOMM();
	PojoLogger.LOGGER.info("PRE isTransportRFCOMM? " + ep.isTransportRFCOMM());
	ep.setTransportRFCOMM(true);
	PojoLogger.LOGGER.info("POST isTransportRFCOMM? (t) " + ep.isTransportRFCOMM());
	ep.setTransportRFCOMM(false);
	PojoLogger.LOGGER.info("POST isTransportRFCOMM? (f) " + ep.isTransportRFCOMM());
	ep.setTransportRFCOMM(torf);

	torf = ep.isTransportNFC();
	PojoLogger.LOGGER.info("PRE isTransportNFC? " + ep.isTransportNFC());
	ep.setTransportNFC(true);
	PojoLogger.LOGGER.info("POST isTransportNFC? (t) " + ep.isTransportNFC());
	ep.setTransportNFC(false);
	PojoLogger.LOGGER.info("POST isTransportNFC? (f) " + ep.isTransportNFC());
	ep.setTransportNFC(torf);

	torf = ep.isNetworkIPv4();
	PojoLogger.LOGGER.info("PRE isNetworkIPv4? " + ep.isNetworkIPv4());
	ep.setNetworkIPv4(true);
	PojoLogger.LOGGER.info("POST isNetworkIPv4? (t) " + ep.isNetworkIPv4());
	ep.setNetworkIPv4(false);
	PojoLogger.LOGGER.info("POST isNetworkIPv4? (f) " + ep.isNetworkIPv4());
	ep.setNetworkIPv4(torf);

	torf = ep.isNetworkIPv6();
	PojoLogger.LOGGER.info("PRE isNetworkIPv6? " + ep.isNetworkIPv6());
	ep.setNetworkIPv6(true);
	PojoLogger.LOGGER.info("POST isNetworkIPv6? (t) " + ep.isNetworkIPv6());
	ep.setNetworkIPv6(false);
	PojoLogger.LOGGER.info("POST isNetworkIPv6? (f) " + ep.isNetworkIPv6());
	ep.setNetworkIPv6(torf);

	torf = ep.isScopeInterface();
	PojoLogger.LOGGER.info("PRE isScopeInterface? " + ep.isScopeInterface());
	ep.setScopeInterface(true);
	PojoLogger.LOGGER.info("POST isScopeInterface? (t) " + ep.isScopeInterface());
	ep.setScopeInterface(false);
	PojoLogger.LOGGER.info("POST isScopeInterface? (f) " + ep.isScopeInterface());
	ep.setScopeInterface(torf);

	torf = ep.isScopeLink();
	PojoLogger.LOGGER.info("PRE isScopeLink? " + ep.isScopeLink());
	ep.setScopeLink(true);
	PojoLogger.LOGGER.info("POST isScopeLink? (t) " + ep.isScopeLink());
	ep.setScopeLink(false);
	PojoLogger.LOGGER.info("POST isScopeLink? (f) " + ep.isScopeLink());
	ep.setScopeLink(torf);

	torf = ep.isTransportSecure();
	PojoLogger.LOGGER.info("PRE isTransportSecure? " + ep.isTransportSecure());
	ep.setTransportSecure(true);
	PojoLogger.LOGGER.info("POST isTransportSecure? (t) " + ep.isTransportSecure());
	ep.setTransportSecure(false);
	PojoLogger.LOGGER.info("POST isTransportSecure? (f) " + ep.isTransportSecure());
	ep.setTransportSecure(torf);

	torf = ep.isRoutingMulticast();
	PojoLogger.LOGGER.info("PRE isRoutingMulticast? " + ep.isRoutingMulticast());
	ep.setRoutingMulticast(true);
	PojoLogger.LOGGER.info("POST isRoutingMulticast? (t) " + ep.isRoutingMulticast());
	ep.setRoutingMulticast(false);
	PojoLogger.LOGGER.info("POST isRoutingMulticast? (f) " + ep.isRoutingMulticast());
	ep.setRoutingMulticast(torf);
    }

    static public void logCoSP(CoResourceSP cosp)
    {
	// PojoLogger.LOGGER.info("CoSP URI PATH: " + cosp.getUriPath());
	// logDeviceAddress(cosp.coAddress());

	// PojoLogger.LOGGER.info("CoSP network protocol: "
	// 		   + String.format("0x%04X",
	// 				   cosp.networkProtocol() & 0xFFFFF));
	// PojoLogger.LOGGER.info("CoSP network Scope: "    + cosp.networkScope());
	// PojoLogger.LOGGER.info("CoSP network Policies: "
	// 			       + String.format("0x%04X",
	// 					       cosp.networkPolicies() & 0xFFFFF));
	// PojoLogger.LOGGER.info("CoSP transport secure?: " + cosp.isTransportSecure());

	// List<String> ts = cosp.getTypes();
	// PojoLogger.LOGGER.info("SP Types:");
	// ts.forEach(typ -> PojoLogger.LOGGER.info("\t" + typ));

	// List<String> ifs = cosp.getInterfaces();
	// PojoLogger.LOGGER.info("SP Interfaces");
	// ifs.forEach(iface -> PojoLogger.LOGGER.info("\t" + iface));
    }

    static public void logSP(IResourceSP theSP)
    {
	PojoLogger.LOGGER.info("SP URI PATH: " + theSP.getUriPath());
	List<String> ts = theSP.getTypes();
	PojoLogger.LOGGER.info("SP Types:");
	ts.forEach(typ -> PojoLogger.LOGGER.info("\t" + typ));

	List<String> ifs = theSP.getInterfaces();
	PojoLogger.LOGGER.info("SP Interfaces");
	ifs.forEach(iface -> PojoLogger.LOGGER.info("\t" + iface));
    }

    // static public void logResource(ResourceLocal resource)
    // {
    // 	PojoLogger.LOGGER.info("RESOURCE: logResource ENTRY");
    // 	PojoLogger.LOGGER.info("RESOURCE: resource uri: " + resource.getUri());

    // 	List<String> tll = resource.getTypes();
    // 	tll.forEach(t -> PojoLogger.LOGGER.info("RESOURCE: type:     " + t)); // Java 8

    // 	List<String> ifll = resource.getInterfaces();
    // 	ifll.forEach(iface -> PojoLogger.LOGGER.info("RESOURCE: interface: " + iface));
    // 	// for (int i = 0; i < ifll.size(); i++) {   // Java 7
    // 	//     PojoLogger.LOGGER.info("REQUEST IN: resource if:    " + ifll.get(i));
    // 	// }

    // 	List<PropertyString> pll = resource.getProperties();
    // 	PojoLogger.LOGGER.info("RESOURCE: properties count: " + pll.size());
    // 	pll.forEach(p -> PojoLogger.LOGGER.info("RESOURCE: property: " + p.name + " = " + p.value));

    // 	List<ResourceLocal> cll = resource.getChildren();
    // 	PojoLogger.LOGGER.info("RESOURCE: child resources count: " + cll.size());
    // 	cll.forEach(ch -> PojoLogger.LOGGER.info("RESOURCE: child resource: " + ch));

    // 	PojoLogger.LOGGER.info("RESOURCE: service provider (callback): "
    // 			   + resource.getServiceProvider().getClass().getName());

    // 	PojoLogger.LOGGER.info("RESOURCE: callback param: "
    // 			   + resource.getCallbackParam().getClass().getName());

    // 	PojoLogger.LOGGER.info("RESOURCE: serial number: " + resource.sn);

    // 	// Instance Id
    // 	// PojoLogger.LOGGER.info("RESOURCE: resource instance id: " + resource.id.getClass().getName());
    // 	if (resource.id.getClass().getName().equals("openocf.Resource$InstanceId")) {
    // 	    PojoLogger.LOGGER.info("RESOURCE: resource InstanceId class: InstanceId");
    // 	} else if (resource.id.getClass().getName().equals("openocf.Resource$InstanceOrdinal")) {
    // 	    PojoLogger.LOGGER.info("RESOURCE: resource InstanceId class: InstanceOrdinal, val="
    // 			       + ((openocf.ResourceLocal.InstanceOrdinal)resource.id).val);
    // 	} else if (resource.id.getClass().getName().equals("openocf.ResourceLocal$InstanceString")) {
    // 	    PojoLogger.LOGGER.info("RESOURCE: resource InstanceId class: InstanceString, val="
    // 			       + ((openocf.ResourceLocal.InstanceString)resource.id).val);
    // 	} else if (resource.id.getClass().getName().equals("openocf.ResourceLocal$InstanceUuid")) {
    // 	    PojoLogger.LOGGER.info("RESOURCE: resource InstanceId class: InstanceUuid, val="
    // 			       + ((openocf.ResourceLocal.InstanceUuid)resource.id).val);
    // 	}

    // 	logResourcePolicies(resource);

    // 	try {
    // 	    PojoLogger.LOGGER.info("RESOURCE: action set: " + resource.getActionSet());
    // 	} catch (OCFNotImplementedException e) {
    // 	    PojoLogger.LOGGER.info("ERROR**** RESOURCE: getActionSet not implemented.");
    // 	}
    // 	PojoLogger.LOGGER.info("RESOURCE: logResource EXIT");
    // }

    // static public void logObservation(ObservationRecord observationRecord)
    // {
    // 	// PojoLogger.LOGGER.info("OBSERVATION: logObservation ENTRY");

    // 	PojoLogger.LOGGER.info("\tOBSERVED uri: " + observationRecord.getUriPath());

    // 	PojoLogger.LOGGER.info("\tOBSERVED type: " + observationRecord.getType());

    // 	// log rtypes
    // 	List<String> rtypes = observationRecord.getResourceTypes();
    // 	PojoLogger.LOGGER.info("\tOBSERVED RESOURCE TYPES count: " + rtypes.size());
    // 	for (String t : (List<String>)rtypes) {
    // 	    PojoLogger.LOGGER.info("\tOBSERVED rtype: " + t);
    // 	}

    // 	// log interfaces
    // 	List<String> ifaces = observationRecord.getInterfaces();
    // 	PojoLogger.LOGGER.info("\tOBSERVED INTERFACES count: " + ifaces.size());
    // 	for (String iface : ifaces) {
    // 	    PojoLogger.LOGGER.info("\tOBSERVED interface: " + iface);
    // 	}

    // 	// log properties (PlatformInfo, DeviceInfo, or "values" for resources)
    // 	PropertyMap<String, Object> pmap = observationRecord.getProperties();
    // 	PojoLogger.LOGGER.info("\tOBSERVED PROPERTIES count: " + pmap.size());
    // 	for (Map.Entry<String, Object> entry : pmap.entrySet())
    // 	    {
    // 		PojoLogger.LOGGER.info("\tOBSERVED property: "
    // 				   + entry.getKey()
    // 				   + " = "
    // 				   + entry.getValue());
    // 	    }
    // 	List<IObservationRecord> kids = observationRecord.getChildren();
    // 	if (kids != null) {
    // 	    PojoLogger.LOGGER.info("\tOBSERVED CHILDREN count: " + kids.size());
    // 	    for (IObservationRecord p : kids) {
    // 		PojoLogger.LOGGER.info("================ CHILD");
    // 		logObservation((ObservationRecord)p);
    // 	    }
    // 	}
    // }

    // static public void logInboundRequest(InboundRequest requestIn)
    // {
    // 	PojoLogger.LOGGER.info("LOG InboundRequest logInboundRequest ENTRY");
    // 	// PojoLogger.LOGGER.info("LOG InboundRequest this handle: " + requestIn.localHandle);
    // 	// PojoLogger.LOGGER.info("LOG InboundRequest remote handle: " + requestIn.getRemoteHandle);
    // 	// PojoLogger.LOGGER.info("LOG InboundRequest resource handle: " + requestIn.getResourceHandle());
    // 	PojoLogger.LOGGER.info("LOG InboundRequest request method: " + requestIn.getMethod());
    // 	PojoLogger.LOGGER.info("LOG InboundRequest query : \"" + requestIn.getQueryString() + "\"");
    // 	PojoLogger.LOGGER.info("LOG InboundRequest msg id : " + requestIn.getMessageId());

    // 	PojoLogger.LOGGER.info("LOG InboundRequest method : " + requestIn.getMethod());
    // 	PojoLogger.LOGGER.info("LOG InboundRequest watch action : " + requestIn.watchAction);
    // 	PojoLogger.LOGGER.info("LOG InboundRequest watch id :     " + requestIn.watchId);


    // 	// ResourceLocal resource = requestIn.getResource();
    // 	// logResource(resource);

    // 	// logDeviceAddress(requestIn.getRemoteDeviceAddress());

    // 	// PojoLogger.LOGGER.info("LOG InboundRequest watch action: " + requestIn.watchAction);
    // 	// PojoLogger.LOGGER.info("LOG InboundRequest watch id    : " + requestIn.watchId);

    // 	List<HeaderOption> headerOptions = requestIn.getOptions();
    // 	if (headerOptions != null)
    // 	    PojoLogger.LOGGER.info("LOG InboundRequest header opts ct: " + headerOptions.size());

    // 	// ObservationList<PayloadForResourceState> observation = requestIn.getPDUPayload();
    // 	// if (observation == null) {
    // 	//     PojoLogger.LOGGER.info("LOG InboundRequest observation is null");
    // 	// }
    // }

    static public void logInboundRequest(InboundRequest req)
    {
	PojoLogger.LOGGER.info("APP_LOGGER logInboundRequest ENTRY, thread "
			   + Thread.currentThread().getId());

	//FIXME	PojoLogger.LOGGER.info("APP_LOGGER: stack result: " + cosp.getCoResult());

	PojoLogger.LOGGER.info("APP_LOGGER CoSP uri path:\t" + req.getUri());
	// PojoLogger.LOGGER.info("APP_LOGGER CoSP method:\t" + cosp.getMethod());
	// PojoLogger.LOGGER.info("APP_LOGGER CoSP conn type:\t" + cosp.connType());
	//FIXME PojoLogger.LOGGER.info("APP_LOGGER CoSP sec ID:\t"
	// 		   + Arrays.toString(cosp.getCoSecurityId()));
	//FIXME PojoLogger.LOGGER.info("APP_LOGGER CoSP serial:\t" + cosp.getNotificationSerial());

	//	logCoAddress(cosp);

	// List<HeaderOption> headerOptions = cosp.getOptions();
	// if (headerOptions != null)
	//     PojoLogger.LOGGER.info("LOG InboundRequest header options ct: " + headerOptions.size());

	// FIXME:
	// if (cosp.result == OCStackResult.OK) {

	//     PojoLogger.LOGGER.info("APP_LOGGER CoSP OBSERVATIONS:");
	//     // PojoLogger.LOGGER.info("APP_LOGGER CoSP OBSERVATION type: "
	//     // 		       + cosp.getObservationType()
	//     // 		       + ": "
	//     // 		       + observationTypes.get(cosp.getObservationType()));

	//     ObservationList<ObservationRecord> observationRecords = cosp.getObservationRecords();
	//     if (observationRecords != null) {
	// 	PojoLogger.LOGGER.info("LOG OBSERVATIONRECORD count: " + observationRecords.size());
	// 	for (ObservationRecord observationRecord : (ObservationList<ObservationRecord>) observationRecords) {
	// 	    List<IObservationRecord> kids = observationRecord.getChildren();
	// 	    if (kids != null) {
	// 		PojoLogger.LOGGER.info("LOG CHILD OBSERVATIONS count: "
	// 				   + observationRecord.getChildren().size());
	// 	    }
	// 	    // logObservation(observation);
	// 	}
	//     }
	// }
    }

    static public void logOutboundRequest(OutboundRequest req)
    {
	PojoLogger.LOGGER.info("APP_LOGGER logOutboundRequest ENTRY, thread "
			   + Thread.currentThread().getId());

	PojoLogger.LOGGER.info("APP_LOGGER Req uri path:\t" + req.getUri());
	PojoLogger.LOGGER.info("APP_LOGGER Req method:\t" + req.getMethod());
	// PojoLogger.LOGGER.info("APP_LOGGER Req conn type:\t" + req.connType());

	// List<HeaderOption> headerOptions = req.getOptions();
	// if (headerOptions != null)
	//     PojoLogger.LOGGER.info("LOG InboundRequest header options ct: " + headerOptions.size());

	// FIXME:
	// if (req.result == OCStackResult.OK) {

	//     PojoLogger.LOGGER.info("APP_LOGGER Req OBSERVATIONS:");
	//     // PojoLogger.LOGGER.info("APP_LOGGER Req OBSERVATION type: "
	//     // 		       + req.getObservationType()
	//     // 		       + ": "
	//     // 		       + observationTypes.get(req.getObservationType()));

	//     ObservationList<ObservationRecord> observationRecords = req.getObservationRecords();
	//     if (observationRecords != null) {
	// 	PojoLogger.LOGGER.info("LOG OBSERVATIONRECORD count: " + observationRecords.size());
	// 	for (ObservationRecord observationRecord : (ObservationList<ObservationRecord>) observationRecords) {
	// 	    List<IObservationRecord> kids = observationRecord.getChildren();
	// 	    if (kids != null) {
	// 		PojoLogger.LOGGER.info("LOG CHILD OBSERVATIONS count: "
	// 				   + observationRecord.getChildren().size());
	// 	    }
	// 	    // logObservation(observation);
	// 	}
	//     }
	// }
    }

    static public void logInboundResponseMap(Map rMap)
    {
	PojoLogger.LOGGER.info("APP_LOGGER logInboundResponseMap ENTRY, thread "
			   + Thread.currentThread().getId());
	PojoLogger.LOGGER.info("Response Map size: " + rMap.size());
	Map devAddr = (Map)rMap.get(OpenOCF.DEVADDR);
	PojoLogger.LOGGER.info("rmap uri: " + rMap.get(OpenOCF.URI));
	PojoLogger.LOGGER.info("rmap devaddr:");
	PojoLogger.LOGGER.info("\taddress: " + devAddr.get(OpenOCF.ADDRESS));
	PojoLogger.LOGGER.info("\tport: " + devAddr.get(OpenOCF.PORT));
	PojoLogger.LOGGER.info("\ttransport adapter: "
			   + String.format("0x%08X", (Integer)devAddr.get(OpenOCF.TRANSPORT_ADAPTER)));
	PojoLogger.LOGGER.info("\ttransport flags: "
			   + String.format("0x%08X", (Integer)devAddr.get(OpenOCF.TRANSPORT_FLAGS)));
	PojoLogger.LOGGER.info("\tindex: " + devAddr.get(OpenOCF.INDEX));
	PojoLogger.LOGGER.info("\tsecure? " + devAddr.get(OpenOCF.SECURE));
	PojoLogger.LOGGER.info("\tmcast?: " + devAddr.get(OpenOCF.MCAST));

	PojoLogger.LOGGER.info("rmap connectivity type: "
			   + String.format("0x%08X", (int)rMap.get(OpenOCF.CONN_TYPE)));

	PojoLogger.LOGGER.info("rmap transport: " + rMap.get(OpenOCF.TRANSPORT));
	PojoLogger.LOGGER.info("rmap udp? " + rMap.get(OpenOCF.UDP));
	PojoLogger.LOGGER.info("rmap tcp? " + rMap.get(OpenOCF.TCP));
	PojoLogger.LOGGER.info("rmap ipv4? " + rMap.get(OpenOCF.IPV4));
	PojoLogger.LOGGER.info("rmap ipv6? " + rMap.get(OpenOCF.IPV6));
	PojoLogger.LOGGER.info("rmap scope: " + rMap.get(OpenOCF.SCOPE));
	PojoLogger.LOGGER.info("rmap secure? " + rMap.get(OpenOCF.SECURE));

	PojoLogger.LOGGER.info("rmap remote result: " + rMap.get(OpenOCF.REMOTE_RESULT));
	PojoLogger.LOGGER.info("rmap remote identity: " + rMap.get(OpenOCF.REMOTE_IDENTITY));
	PojoLogger.LOGGER.info("rmap serial: " + rMap.get(OpenOCF.SERIAL));

	ArrayList options = (ArrayList)rMap.get(OpenOCF.COAP_OPTIONS);
	PojoLogger.LOGGER.info("CoAP options (count: " + options.size() + ")");
	for (HashMap option: (List<HashMap>)options) {
	    for (Entry<Object,Object> opt : (Set<Entry<Object,Object>>)option.entrySet()){
	    	//iterate over the pairs
	    	PojoLogger.LOGGER.info("\t" + opt.getKey()+": " + opt.getValue());
	    }
	}

	// PAYLOAD
	PojoLogger.LOGGER.info("Payload type: " + rMap.get(OpenOCF.PAYLOAD_TYPE));
	Map payloadMap = (Map)rMap.get(OpenOCF.PAYLOAD);
	PojoLogger.LOGGER.info("Payload size: " + payloadMap.size());
	PojoLogger.LOGGER.info("Payload di: " + payloadMap.get(OpenOCF.OCF_DI));
	PojoLogger.LOGGER.info("Payload name: " + payloadMap.get(OpenOCF.OCF_NAME));

	List ifaces = (List)payloadMap.get(OpenOCF.OCF_IF);
	PojoLogger.LOGGER.info("Payload interface count: " + ifaces.size());
 	for (String iface: (List<String>)ifaces) {
	    PojoLogger.LOGGER.info("Payload if: " + iface);
	}

	List rts = (List)payloadMap.get(OpenOCF.OCF_RT);
	PojoLogger.LOGGER.info("Payload rt count: " + rts.size());
 	for (String rt: (List<String>)rts) {
	    PojoLogger.LOGGER.info("Payload rt: " + rt);
	}

	List links = (List)payloadMap.get(OpenOCF.OCF_LINKS);
	PojoLogger.LOGGER.info("Links count: " + links.size());
	Map link;
	for (int i = 0; i < links.size(); i++) {
	    link = (Map)links.get(i);
	    PojoLogger.LOGGER.info("Link " + i);
	    PojoLogger.LOGGER.info("\thref: " + link.get(OpenOCF.OCF_HREF));
	    // port/sec: only if OIC 1.1, not OCF 1.0
	    // PojoLogger.LOGGER.info("\tport: " + link.get(OpenOCF.OCF_PORT));
	    // PojoLogger.LOGGER.info("\ttcp port: " + link.get(OpenOCF.TCP_PORT));

	    // buri not in Iotivity?
	    // PojoLogger.LOGGER.info("\tbase uri: " + link.get(OpenOCF.OCF_BURI));

	    PojoLogger.LOGGER.info("\trel: " + link.get(OpenOCF.OCF_LINK_RELATION));
	    PojoLogger.LOGGER.info("\tanchor: " + link.get(OpenOCF.OCF_ANCHOR));

	    // rt (resource types List<String>)
	    rts = (List)link.get(OpenOCF.OCF_RT);
	    PojoLogger.LOGGER.info("\ttypes (count " + rts.size() + ")");
	    for (String rt: (List<String>)rts) {
		PojoLogger.LOGGER.info("\t\t" + rt);
	    }

	    // if (interfaces List<String>)
	    ifaces = (List)link.get(OpenOCF.OCF_IF);
	    PojoLogger.LOGGER.info("\tinterfaces (count " + ifaces.size() + ")");
	    for (String iface: (List<String>)ifaces) {
		PojoLogger.LOGGER.info("\t\t" + iface);
	    }

	    PojoLogger.LOGGER.info("\tpolicy bitmask "
			       + String.format("0x%08X", (int)link.get(OpenOCF.OCF_POLICY_BITMASK)));
	    PojoLogger.LOGGER.info("\t\tdiscoverable? " + link.get(OpenOCF.DISCOVERABLE));
	    PojoLogger.LOGGER.info("\t\tobservable? " + link.get(OpenOCF.OBSERVABLE));

	    // eps (List<Map>)
	    List eps = (List)link.get(OpenOCF.OCF_EPS);
	    PojoLogger.LOGGER.info("\tEndpoints (count " + eps.size() + ")");
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
		PojoLogger.LOGGER.info("\t\tep: " + epstr);
		PojoLogger.LOGGER.info("\t\ttps: " + ep.get(OpenOCF.OCF_TPS));
		PojoLogger.LOGGER.info("\t\taddr: " + ep.get(OpenOCF.OCF_ADDR));
		PojoLogger.LOGGER.info("\t\tport: " + ep.get(OpenOCF.OCF_PORT));
		PojoLogger.LOGGER.info("\t\ttransport flags: "
				   + String.format("0x%08X", (int)ep.get(OpenOCF.TRANSPORT_FLAGS)));
		PojoLogger.LOGGER.info("\t\tIPv4? " + ep.get(OpenOCF.IPV4));
		PojoLogger.LOGGER.info("\t\tIPv6? " + ep.get(OpenOCF.IPV6));
		PojoLogger.LOGGER.info("\t\tSecure? " + ep.get(OpenOCF.SECURE));
		PojoLogger.LOGGER.info("\t\tMcast? " + ep.get(OpenOCF.MCAST));
		PojoLogger.LOGGER.info("\t\tpriority: " + ep.get(OpenOCF.OCF_PRIORITY));
	    }
	}
    }

    static public void logInboundResponse(InboundResponse response)
    {
	PojoLogger.LOGGER.info("uri_path:\t" + response.getUri());

	// inbound messages (OCClientResponse) do not contain method
	PojoLogger.LOGGER.info("method:\t" + response.getMethod());
	//PojoLogger.LOGGER.info("APP_LOGGER Response conn type:\t" + response.connType());
	PojoLogger.LOGGER.info("identity:\t" + response.getIdentity());
	PojoLogger.LOGGER.info("result:\t" + response.getResult());
        PojoLogger.LOGGER.info("serial:\t" + response.getNotificationSerial());

	// logCoAddress(response.getEndpoint());

        PojoLogger.LOGGER.info("Sender endpoint:");
	logEndpoint(response.getEndpoint());

	// List<HeaderOption> headerOptions = req.getOptions();
	// if (headerOptions != null)
	//     PojoLogger.LOGGER.info("LOG InboundRequest header options ct: " + headerOptions.size());

	// FIXME:
	// if (result.cosp == OCStackResult.OK) {

	//     PojoLogger.LOGGER.info("APP_LOGGER CoSP OBSERVATIONS:");
	//     // PojoLogger.LOGGER.info("APP_LOGGER CoSP OBSERVATION type: "
	//     // 		       + cosp.getObservationType()
	//     // 		       + ": "
	//     // 		       + observationTypes.get(cosp.getObservationType()));

	//     ObservationList<ObservationRecord> observationRecords = cosp.getObservationRecords();
	//     if (observationRecords != null) {
	// 	PojoLogger.LOGGER.info("LOG OBSERVATIONRECORD count: " + observationRecords.size());
	// 	for (ObservationRecord observationRecord : (ObservationList<ObservationRecord>) observationRecords) {
	// 	    List<IObservationRecord> kids = observationRecord.getChildren();
	// 	    if (kids != null) {
	// 		PojoLogger.LOGGER.info("LOG CHILD OBSERVATIONS count: "
	// 				   + observationRecord.getChildren().size());
	// 	    }
	// 	    // logObservation(observation);
	// 	}
	//     }
	// }
    }
}
