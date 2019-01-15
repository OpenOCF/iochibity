package openocf;

//import openocf.engine.OCFCommonSP;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URL;
import java.util.StringTokenizer;

import cz.adamh.utils.NativeUtils;

/**
 *
 */
public class ConfigJava {
    private static final String TAG = "openocf.ConfigJava";

    private static final String JNILIB = "/bindings/javalang/libopenocf_jni.so";
    //private static final String JNILIB = "/libopenocf_jni.dylib";

    // public static native void config_logging(String logfile_fname);
    // public static native void config_svrs(String svrs_config_fname);
    // public static native String configuration();

    public static void config(String logfile_fname, String svrs_config_fname)
    {
	System.out.println("openocf/ConfigJava.config");
	String property = System.getProperty("java.library.path");
	StringTokenizer parser = new StringTokenizer(property, ";");
        System.out.println("java.library.path:");
	while (parser.hasMoreTokens()) {
	    System.out.println(parser.nextToken());
	}

	try{
	    //System.loadLibrary("openocf_jni");
	    // Plain Java on macOS:
            NativeUtils.loadLibraryFromJar(JNILIB);
	    // Linux:
	    // NativeUtils.loadLibraryFromJar(JNILIB);
	} catch (Exception e) {
            // This is probably not the best way to handle exception :-)
	    System.out.println("NativeUtils.loadLibraryFromJar failed");
            e.printStackTrace();
        }
	System.out.println("LOADED (java)" + JNILIB);

	try {
	    OpenOCF.config_logging(logfile_fname);
	} catch(Exception e){
	    System.out.println(e.toString());
	    System.exit(0);
	}

	try {
	    System.out.println("Getting path for resource " + "/svrs.cbor"); // svrs_config_fname);
	    String f = NativeUtils.extractSVRConfigFile("/svrs.cbor");
	    OpenOCF.config_svrs(f);
	} catch(Exception e){
	    System.out.println(e.toString());
	    System.exit(0);
	}
    }

    // below: from CoServiceManager.
    // FIXME: this does not belong here? move to openocf/ocfresources?
    // public static native void configurePlatformSP(String platform_id, // setPlatformInfo
    //     					  String manufacturer_name,
    //     					  String manufacturer_url,
    //     					  String model_number,
    //     					  String date_of_manufacture,
    //     					  String platform_version,
    //     					  String operating_system_version,
    //     					  String hardware_version,
    //     					  String firmware_version,
    //     					  String support_url,
    //     					  String system_time);


    // // FIXME: this does not belong here?
    // public static native void configureDeviceSP(String deviceName, // setDeviceInfo
    //     					String[] types,
    //     					String specVersion,
    //     					String[] dataModelVersions);

    // // returns configuration info from engine, e.g. version string
    // public static native String configuration();

    // public static native void /*OCStackResult*/
    //     Init(int /*OCMode*/ mode);
    //          // String securityConfigFileName);

    // // public static native void /*OCStackResult*/
    // // 	Init(String /*const char* */ ip_addr,
    // // 	     int /*uint16_t*/ port,
    // // 	     int /*OCMode*/ mode,
    // // 	     String securityConfigFileName);

    // public static native void /*OCStackResult*/
    //     Init(int /*OCMode*/ mode,
    //          int /*OCTransportFlags*/ server_flags,
    //          int /*OCTransportFlags*/ client_flags);

    // private native int /*OCStackResult*/
    // OCRegisterPersistentStorageHandler(Object /*OCPersistentStorage* */ persistent_storage_handler);
    // OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };
    // OCRegisterPersistentStorageHandler(&ps);
	// XOCRegisterPersistentStorageHandler(Object /*OCPersistentStorage* */ persistent_storage_handler);
    //NB: default ps file is "oic_svr_db.dat"

    // public native void /*OCStackResult*/ OCStartMulticastServer();

    // public native void /*OCStackResult*/ OCStopMulticastServer();

    // public static native void /*OCStackResult*/	run();

    // public static native void /*OCStackResult*/ stop();

    // // setPlatformInfo =>  ServiceManager.registerPlatform

    // // setDeviceInfo =>  ServiceManager.registerDevice

    // // OCDoResource =>  ServiceManager.sendResource

    // public native void OCCancel(Object /*OCDoHandle*/ handle,
    //     			int /*OCQualityOfService*/ qos,
    //     			Object /*OCHeaderOption* */ options,
    //     			byte /*uint8_t*/ options_count);
}
