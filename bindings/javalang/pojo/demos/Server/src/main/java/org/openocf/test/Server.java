package org.openocf.test;

import org.openocf.test.PojoLogger;

import openocf.OpenOCFServer;
import openocf.ConfigJava;
//import openocf.utils.Endpoint;
import openocf.utils.CoAPOption;
// import openocf.InboundRequest;
import openocf.utils.PropertyMap;
import openocf.app.ResourceSP;
import openocf.app.IResourceSP;
// import openocf.engine.OCFServerSP; // ServiceManager;

// import org.openocf.test.server.LedSP;
// import org.openocf.test.server.LightSP;
// import org.openocf.test.server.TemperatureSP;
import org.openocf.test.server.WhatsitSP;

import openocf.exceptions.OCFNotImplementedException;

import openocf.constants.Method;
import openocf.constants.OCStackResult;
import openocf.constants.ResourcePolicy;
import openocf.constants.ServiceResult;


// import mraa.mraa;
// import mraa.IntelEdison;
// import mraa.Platform;
// import mraa.Result;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.ArrayList;
import java.util.List;
import java.util.LinkedList;
import java.util.Map;

public class Server
{
    static{
	// FIXME - init method handles configuration
	ConfigJava.config("/tmp/openocf_server.log", // logfile_fname, must be absolute path to work with bazel run
                          "/server_config.cbor"	  // FIXME: constant "svrs.cbor"
			     );
        // ConfigJava.getLogfileName();
    }

    public static void main(String[] args)
    {
        PojoLogger.LOGGER.info("OpenOCF Logfile: " + OpenOCFServer.getLogfileName());
	// Platform platform = mraa.getPlatformType();
	// if (platform != Platform.INTEL_EDISON_FAB_C) {
	//     System.err.println("Error: This program can only be run on edison");
	//     System.exit(Result.ERROR_INVALID_PLATFORM.swigValue());
	// }

	Runtime.getRuntime().addShutdownHook(new Thread()
	    {
		@Override
		public void run()
		{
		    System.out.println("Shutdown hook running!");
		    OpenOCFServer.stop();
		}
	    });

	//OCFServices.Init(OCFServices.SERVER);
	OpenOCFServer.Init(OpenOCFServer.SERVER);

	// ServiceManager.
	// OpenOCFServer.configurePlatformSP("76a10fbf-0cbb-4e27-a748-cec0eb9bdc92",
	// 				   "Acme Novelties",
	// 				   "http://acme.example.org",
	// 				   "modelnbr", "mfgdate", "platversion",
	// 				   "osversion", "hwversion", "firmwareversion",
	// 				   "http://acme.example.org/support",
	// 				   new SimpleDateFormat("yyyy.MM.dd.HH.mm.ss").format(new Date()));

	// OpenOCFServer.configureDeviceSP("Fartmaster2020 Server",
	// 				 new String[] {"type1", "type2"},
	// 				 "version-0.1",
	// 				 new String[] {"dmversion-0.1"});

	// TemperatureSP tempSP = new TemperatureSP();
	// tempSP.addType("foo.t.bar");
	// tempSP.addInterface("foo.if.bar");
	// Logger.logSP(tempSP);

	// ResourceSP tSP = null;
	// tSP = ServiceManager.registerResourceSP(tempSP);

	//OCFServerSP
	OpenOCFServer.registerResourceSP(new WhatsitSP());
	OpenOCFServer.registerResourceSP(new WhatsitSP("/foo/bar/baz/long/very/long/url/path/too/long"));

	// ServiceManager.registerResourceSP(new LedSP());

	OpenOCFServer.run();
	//OpenOCF.observe();

        while(true){
	    try {
		Thread.sleep(2000);
		// System.out.println("GUI thread loop");
	    } catch (InterruptedException e) {
		e.printStackTrace();
		msgError(TAG, e.toString());
	    }
        }
    }

    public static void msgError(final String tag ,final String text) {
        System.out.println("[E]" + tag + " | " + text);
    }

    private final static String TAG = Server.class.getSimpleName();
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
