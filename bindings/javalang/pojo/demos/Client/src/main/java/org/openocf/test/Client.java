package org.openocf.test;

// import org.slf4j.Logger;
// import org.slf4j.LoggerFactory;

// UI stuff
// import com.googlecode.lanterna.TerminalSize;
// import com.googlecode.lanterna.TextColor;
// import com.googlecode.lanterna.gui2.*;
// import com.googlecode.lanterna.screen.Screen;
// import com.googlecode.lanterna.screen.TerminalScreen;
// import com.googlecode.lanterna.terminal.DefaultTerminalFactory;
// import com.googlecode.lanterna.terminal.Terminal;

import openocf.OpenOCFClient;
import openocf.ConfigJava;

//import openocf.engine.OCFClientSP;
// import openocf.CoServiceManager;
import openocf.app.CoResourceSP;
import openocf.Endpoint;
// import openocf.Message;
import openocf.message.OutboundRequest;
import openocf.message.InboundResponse;
// import openocf.ObservationOut;
import openocf.message.CoAPOption;
import openocf.observation.ObservationRecord;
// import openocf.ObservationList;
import openocf.utils.PropertyMap;
import openocf.app.ICoResourceSP;

import org.openocf.test.client.DiscoveryCoRSP;
import org.openocf.test.client.GenericCoRSP;

import java.io.IOException;
import java.util.logging.Level;
//import java.util.logging.Logger;
import org.openocf.test.PojoLogger;

import openocf.constants.Method;
import openocf.constants.OCStackResult;
import openocf.constants.ResourcePolicy;
import openocf.constants.ServiceResult;
import openocf.exceptions.OCFNotImplementedException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.ArrayList;
import java.util.List;
import java.util.LinkedList;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.regex.Pattern;

import java.util.stream.Collectors;


public class Client
{
    //static final Logger clientLogger = LoggerFactory.getLogger(Client.class);
    static final String logfile;

    static{
	ConfigJava.config("/tmp/openocf_client.log", // logfile_fname, must be absolute path
                          "/client_config.cbor"   // FIXME: use constant "svrs.cbor"
                          );
        logfile = OpenOCFClient.getLogfileName();
    }

    public static DiscoveryCoRSP discoveryCoRSP;

    public static synchronized void promptUser()
    {
	Scanner scanner = new Scanner(System.in);
	String action = null;
	Pattern top  = Pattern.compile("[12miecrudnx]");
	Pattern pdrx = Pattern.compile("[dprx]");

    	String uri = null;
	// OutboundRequest msgRequestOut;

	// CoServiceManager.registerCoServiceProvider(discoveryCoRSP);

	boolean again = true;
	while(again) {
            PojoLogger.LOGGER.info("OpenOCF Logfile: " + logfile);
	    System.out.println("Choose an action:");
	    // list messages
	    // inspect message
	    System.out.println("\t1) Discovery");
	    System.out.println("\t2) List discovered resources");
	    System.out.println("\tm) List Messages");
	    System.out.println("\ti) Inspect Message");
	    System.out.println("\te) List local Endpoints");
	    System.out.println("\tc) CREATE");
	    System.out.println("\tr) RETRIEVE");
	    System.out.println("\tu) UPDATE");
	    // System.out.println("\t6) POST");
	    System.out.println("\td) DELETE");
	    System.out.println("\tn) NOTIFY (WATCH)");
	    System.out.println("\tx) Exit\n");
	    while (!scanner.hasNext(top)) {
		System.out.println("Invalid input, try again");
		scanner.next();
	    }
	    action = scanner.next(top);
	    switch(action) {
	    case "0":		// initialize CoRSPs
		// WhatsitCoRSP whatsitCoRSP = new WhatsitCoRSP();
		// whatsitCoRSP.dest = gWhatsitAddress;
		// whatsitCoRSP.uri  = "/a/whatsit";
		// ServiceManager.registerCoServiceProvider(whatsitCoRSP);
		break;
	    case "1":			// Discover
		System.out.println("Discover:");
		System.out.println("\tp) Platforms (RETRIEVE oic/p) ");
		System.out.println("\td) Devices   (RETRIEVE oic/d)");
		System.out.println("\tr) Resources (RETRIEVE oic/res");
		System.out.println("\tx) Cancel");
		while (!scanner.hasNext(pdrx)) {
		    System.out.println("Invalid input, try again");
		    scanner.next();
		}
		action = scanner.next(pdrx);

		switch(action) {
		case "p":
		    System.out.println("requested DISCOVER PLATFORMS");
		    uri = "/oic/p";
		    break;
		case "d":
		    System.out.println("requested DISCOVER DEVICES");
		    uri = "/oic/d";
		    break;
		case "r":
		    System.out.println("requested DISCOVER RESOURCES");
		    uri = "/oic/res";
		    break;
		case "x":
		    System.out.println("CANCELLED");
		    break;
		}

		// NB: in Iotivity all these params are passed as args
		// to OCDoResource, not retained as state; there is no
		// analog to OutboundRequest (or CoRSP) in Iotivity.

		// a resource is identified by UriPath, independent of
		// network address. /foo/bar could be hosted at
		// multiple addresses; same CoRSP could be used for
		// each. Addressing is set by OutboundRequest created
		// for the CoRSP.
		discoveryCoRSP
		    // metadata:
		    .setUri(uri);
		// data: .setPayload(???)
		    // .addType("foo.t.bar")

		OutboundRequest requestOut =
		    new OutboundRequest(discoveryCoRSP)
		    .setMethod(Method.DISCOVER)
		    .setQualityOfService(OpenOCFClient.QOS_HIGH);

		    // Alternative: pass an Endpoint object instead of setting individual props
		requestOut
		    .getEndpoint()
		    // .setIPAddress("foo")
		    // .setPort(9999)
		    // .setTransportSecure(true)
		    // .setTransportUDP(true)
		    // .setNetworkIPv6(true)
		    // .setScopeGlobal(true)
		    //.setRoutingMulticast(true)
		    // .setCoAPOptions(...)
		    ;

		// LOGGER.info("BEFORE EXHIBIT");
		//Logger.logEndpoint(requestOut.getEndpoint());
		// Logger.logCoRSP(discoveryCoRSP);

		try {
		    CountDownLatch finished = discoveryCoRSP.latch();
		    // discoveryCoRSP.coExhibit();
		    // Or:  coServiceManager.coExhibit(discoveryCoRSP);
		    //OCFClientSP.coExhibit(requestOut);
		    OpenOCFClient.coExhibit(requestOut);
		    finished.await(5, TimeUnit.SECONDS);
		} catch (Exception e) {
		    PojoLogger.LOGGER.info("ERROR: discovery");
		    e.printStackTrace();
		    msgError(TAG, e.toString());
		}
		break;
	    case "2":
		System.out.println("Discovered Resources:");
		// OCFClientSP.registeredCoServiceProviders();
		// for (CoResourceSP cosp : OpenOCFClient.registeredCoResourceSPs()) {
		for (InboundResponse r : OpenOCFClient.getInboundResponses()) {
		    System.out.println("\t" + r.getUri());
		}
		break;
	    case "3":
		System.out.println("RETRIEVE: Select a resource.");

		// UI tedium:
		CoResourceSP cosp = null;
		// int sz = ServiceManager.coServiceProviders.size();
		// for(int i = 0; i < sz; i++) {
		//     cosp = ServiceManager.coServiceProviders.get(i);
		//     DeviceAddress da = cosp.coAddress();
		//     String s;
		//     if (da != null) {
		// 	s = i + " : " + da.ipAddress()
		// 	+ " " + da.port()
		// 	    + " " + cosp.uriPath();
		//     } else {
		// 	s = i + " : " + cosp.uriPath();
		//     }
		//     System.out.println("\t" + s);
		// }

		int i = 0;
		// while(true) {
		//     while (!scanner.hasNextInt()) {
		// 	System.out.println("Invalid input: " + scanner.next() + "; try again");
		//     }
		//     i = scanner.nextInt();
		//     if ( (i < sz) && i >= 0) {
		// 	break;
		//     } else {
		// 	System.out.println("Invalid input: " + i + "; try again");
		//     }
		// }
		// Yay! UI done.

		// cosp = ServiceManager.coServiceProviders.get(i);
		// cosp.method(OCF.RETRIEVE);
		// Logger.logCoRSP(cosp);
		// try {
		//     cosp.coExhibit();
		//     // cosp.coExhibit();
		//     Thread.sleep(500); // just to let the user prompt work
		// } catch (Exception e) {
		//     e.printStackTrace();
		//     msgError(TAG, e.toString());
		// }
		break;
	    case "4":
		System.out.println("requested WATCH");
		// try {
		//     OutboundRequest requestOut
		//     	= new OutboundRequest(new WhatsitSR());
		//     requestOut.dest = gWhatsitAddress;
		//     requestOut.uri  = "/a/whatsit";
		//     byte[] bs = Messenger.exhibitRequest(OCF.WATCH, requestOut);
		//     Thread.sleep(1000);
		// } catch (Exception e) {
		//     e.printStackTrace();
		//     msgError(TAG, e.toString());
		// }
		break;
	    case "5":
		System.out.println("requested PUT - NOT YET IMPLEMENTED");
		break;
	    case "6":
		System.out.println("requested POST");
		try {
		    Thread.sleep(1000);
		} catch (Exception e) {
		    e.printStackTrace();
		    msgError(TAG, e.toString());
		}
	    	break;
	    case "m":
		System.out.println("Listing Messages");
		try {
		    Thread.sleep(1000);
		} catch (Exception e) {
		    e.printStackTrace();
		    msgError(TAG, e.toString());
		}
	    	break;
	    case "e":
		System.out.println("Listing local Endpoints");
		try {
		    List<HashMap> eps = Endpoint.getLocalEndpoints();
		    System.out.println("Local EP count: " + eps.size());
		    for (HashMap ep: eps) {
			System.out.println(
					   "["+ ep.get(OpenOCFClient.ADDRESS)
					   + "]:" + ep.get(OpenOCFClient.PORT)
					   + " [" + ep.get(OpenOCFClient.INDEX) + "] "
					   + "; Transport: " + ep.get(OpenOCFClient.TRANSPORT)
					   + "; Secure? " + ep.get(OpenOCFClient.SECURE)
					   + "; Multicast? " + ep.get(OpenOCFClient.MCAST));
		    }
		} catch (Exception e) {
		    e.printStackTrace();
		    msgError(TAG, e.toString());
		}
	    	break;
	    case "x":
		System.out.println("EXITING");
		again = false;
		OpenOCFClient.stop();
		// try {
		//     Thread.sleep(1000);
		// } catch (InterruptedException e) {
		//     e.printStackTrace();
		// }
		System.exit(0);
		break;
	    default:
		break;
	    }
	    // give logger time to flush
	    try {
		Thread.sleep(1000);
	    } catch (InterruptedException e) {
		e.printStackTrace();
	    }
	}
    }

    // ****************************************************************
    public static void main(String[] args)
    {
	Runtime.getRuntime().addShutdownHook(new Thread()
	    {
		@Override
		public void run()
		{
		    System.out.println("Shutdown hook running!");
		    try {
			Thread.sleep(1000);
		    } catch (InterruptedException e) {
			e.printStackTrace();
		    }
		    OpenOCFClient.stop();
		}
	    });

	//OCFClientSP
	OpenOCFClient.Init(OpenOCFClient.CLIENT); // _SERVER);

	// 0. run OpenOCF.observe()
	// 1. create and register corsp
	// 2. create corresponding OutboundRequest message
	// 3. coexhibit msg

	Thread uithread = new Thread() {
		@Override
		public void run()
		{
		    // cosp must be alloced on same thread that calls its methods
		    discoveryCoRSP = new DiscoveryCoRSP();
		    promptUser();
		}
	    };
	uithread.start();

	OpenOCFClient.run();
	// OpenOCF.monitor();

	// promptUser();

	// try {
	//     Thread.sleep(500);
	// } catch (Exception e) {
	//     e.printStackTrace();
	//     msgError(TAG, e.toString());
	// }



	// // Setup terminal and screen layers
	// Screen screen = null;
	// try {
	//     Terminal terminal = new DefaultTerminalFactory().createTerminal();
	//     screen = new TerminalScreen(terminal);
	//     screen.startScreen();
	// } catch (Exception e) {
	//     System.out.println("CAUGHT TERMINAL EXCEPTION");
	// }
        // // Create panel to hold components
        // Panel panel = new Panel();
        // panel.setLayoutManager(new GridLayout(2));

        // panel.addComponent(new Label("Forename"));
        // panel.addComponent(new TextBox());

        // panel.addComponent(new Label("Surname"));
        // panel.addComponent(new TextBox());

        // panel.addComponent(new EmptySpace(new TerminalSize(0,0))); // Empty space underneath labels
        // panel.addComponent(new Button("Submit"));

        // // Create window to hold the panel
        // BasicWindow window = new BasicWindow();
        // window.setComponent(panel);

        // // Create gui and start gui
        // MultiWindowTextGUI gui = new MultiWindowTextGUI(screen,
	// 						new DefaultWindowManager(),
	// 						new EmptySpace(TextColor.ANSI.BLUE));
        // gui.addWindowAndWait(window);

        // while(true){
	//     try {
	// 	Thread.sleep(2000);
	// 	LOGGER.info("GUI thread loop");
	//     } catch (InterruptedException e) {
	// 	e.printStackTrace();
	// 	msgError(TAG, e.toString());
	//     }
        // }
    }

    public static void msgError(final String tag ,final String text) {
        PojoLogger.LOGGER.info("[E]" + tag + " | " + text);
    }

    private final static String TAG = Client.class.getSimpleName();
    // static BufferedReader in=new BufferedReader(new InputStreamReader(System.in));
    // static boolean quit=false;
    // static class Quitter implements Runnable {
    // 	public void run(){
    // 	    String msg = null;
    // 	    // threading is waiting for the key Q to be pressed
    // 	    while(true){
    // 		try{
    // 		    msg=in.readLine();
    // 		}catch(IOException e){
    // 		    e.printStackTrace();
    // 		}

    // 		if(msg.equals("Q")) {quit=true;break;}
    // 	    }
    // 	}
    // }
}
