package openocf;

//import openocf.engine.OCFCommonSP;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URL;
import java.util.List;

// import cz.adamh.utils.NativeUtils;

/**
 *
 */
public class OpenOCF {
    private static final String TAG         = "openocf.std.OpenOCF";

    // private static final String JNILIB   = "/libopenocf_jni.so";
    private static final String JNILIB      = "/libopenocf_jni.dylib";

    // keys for object maps (for Flutter interop)
    // must be kept in sync with jni/map_keys.[ch]
    public static final int ADDRESS         = 1;
    public static final int PORT            = 2;
    public static final int TRANSPORT       = 3;
    public static final int INDEX           = 4;
    public static final int SECURE          = 5;
    public static final int MCAST           = 6;
    public static final int DEVADDR         = 7;
    public static final int IPV4            = 8;
    public static final int IPV6            = 9;

    public static final int SCOPE           = 10;
    public static final int SCOPE_IF        = 11;
    public static final int SCOPE_LINK      = 12;
    public static final int SCOPE_REALM     = 13;
    public static final int SCOPE_ADMIN     = 14;
    public static final int SCOPE_SITE      = 15;
    public static final int SCOPE_ORG       = 16;
    public static final int SCOPE_GLOBAL    = 17;
    public static final int UDP             = 18;
    public static final int TCP             = 19;

    public static final int REMOTE_IDENTITY = 20;
    public static final int REMOTE_RESULT   = 21;
    public static final int PAYLOAD_TYPE    = 22;
    public static final int PAYLOAD         = 23;
    public static final int COAP_OPTIONS    = 24;
    public static final int SERIAL          = 25;
    public static final int URI             = 26;
    public static final int OCF_ADDR        = 27;
    public static final int OCF_PORT        = 28;
    public static final int OCF_DEVICE      = 29; // d

    public static final int OCF_POLICY      = 30; // p
    public static final int OCF_DI          = 31; // device id?
    public static final int OCF_INSTANCE    = 32; // ins
    public static final int OCF_RT          = 33;
    public static final int OCF_IF          = 34; // interfaces
    public static final int OCF_NAME        = 35; // n
    public static final int OCF_ID          = 36;
    public static final int OCF_TITLE       = 37;
    public static final int OCF_MEDIA_TYPE  = 38;
    public static final int OCF_ANCHOR      = 39;

    public static final int OCF_POLICY_BITMASK= 40; // bm
    public static final int OCF_SEC         = 41;
    public static final int OCF_EP          = 42;
    public static final int OCF_EPS         = 43;
    public static final int OCF_PRIORITY    = 44; // pri
    public static final int OCF_LINK        = 45;
    public static final int OCF_LINKS       = 46;
    public static final int OCF_HREF        = 47;
    public static final int OCF_LINK_RELATION= 48;
    public static final int OCF_TPS         = 49;

    public static final int TRANSPORT_FLAGS = 50;
    public static final int OCF_BURI        = 51; // base uri
    public static final int OCF_PI          = 52; // platform id
    public static final int TCP_PORT        = 53;
    public static final int DISCOVERABLE    = 54;
    public static final int OBSERVABLE      = 55;
    public static final int CONN_TYPE       = 56;
    public static final int TRANSPORT_ADAPTER= 57;

    // OCMode
    public static final int CLIENT        = 0;
    public static final int SERVER        = 1;
    public static final int CLIENT_SERVER = 2;
    public static final int GATEWAY       = 3;

    // OCQualityOfService
    public static final int QOS_LOW       = 0; // best-effort
    public static final int QOS_MEDIUM    = 1;
    public static final int QOS_HIGH      = 2; // ACKs confirm delivery
    public static final int QOS_NA        = 3; // stack decides

    native public static void config_logging(String logfile_fname);
    native public static void config_svrs(String svrs_config_fname);

    // public static void config(String logfile_fname, String svrs_config_fname)
    // {
    // 	System.out.println("openocf.standard.OCFServices::config");
    // 	try{
    // 	    // Plain Java on macOS:
    // 	    NativeUtils.loadLibraryFromJar(JNILIB);
    // 	    // Linux:
    // 	    // NativeUtils.loadLibraryFromJar(JNILIB);
    // 	} catch (Exception e) {
    //         // This is probably not the best way to handle exception :-)
    // 	    System.out.println("NativeUtils.loadLibraryFromJar failed");
    //         e.printStackTrace();
    //     }
    // 	System.out.println("LOADED " + JNILIB);

    // 	try {
    // 	    config_logging(logfile_fname);
    // 	} catch(Exception e){
    // 	    System.out.println(e.toString());
    // 	    System.exit(0);
    // 	}

    // 	try {
    // 	    System.out.println("Getting path for resource " + "/svrs.cbor"); // svrs_config_fname);
    // 	    String f = NativeUtils.extractSVRConfigFile("/svrs.cbor");
    // 	    config_svrs(f);
    // 	} catch(Exception e){
    // 	    System.out.println(e.toString());
    // 	    System.exit(0);
    // 	}
    // }

    // below: from CoServiceManager.
    // FIXME: this does not belong here? move to openocf/ocfresources?
     native public static void configurePlatformSP(String platform_id, // setPlatformInfo
						  String manufacturer_name,
						  String manufacturer_url,
						  String model_number,
						  String date_of_manufacture,
						  String platform_version,
						  String operating_system_version,
						  String hardware_version,
						  String firmware_version,
						  String support_url,
						  String system_time);

    // FIXME: this does not belong here?
     native public static void configureDeviceSP(String deviceName, // setDeviceInfo
						String[] types,
						String specVersion,
						String[] dataModelVersions);

    // returns configuration info from engine, e.g. version string
    native public static String configuration();
    native public static String getVersion();

     native public static void /*OCStackResult*/
	 Init(int /*OCMode*/ mode);
	     // String securityConfigFileName);

    // public static native void /*OCStackResult*/
    // 	Init(String /*const char* */ ip_addr,
    // 	     int /*uint16_t*/ port,
    // 	     int /*OCMode*/ mode,
    // 	     String securityConfigFileName);

     native public static void /*OCStackResult*/
	Init(int /*OCMode*/ mode,
	     int /*OCTransportFlags*/ server_flags,
	     int /*OCTransportFlags*/ client_flags);

    native private int /*OCStackResult*/
    // OCRegisterPersistentStorageHandler(Object /*OCPersistentStorage* */ persistent_storage_handler);
    // OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };
    // OCRegisterPersistentStorageHandler(&ps);
	XOCRegisterPersistentStorageHandler(Object /*OCPersistentStorage* */ persistent_storage_handler);
    //NB: default ps file is "oic_svr_db.dat"

    native public void /*OCStackResult*/ OCStartMulticastServer();

    native public void /*OCStackResult*/ OCStopMulticastServer();

    native public static void /*OCStackResult*/	run();

    native public static void /*OCStackResult*/ stop();

    // setPlatformInfo =>  ServiceManager.registerPlatform

    // setDeviceInfo =>  ServiceManager.registerDevice

    // OCDoResource =>  ServiceManager.sendResource

    native public void OCCancel(Object /*OCDoHandle*/ handle,
				int /*OCQualityOfService*/ qos,
				Object /*OCHeaderOption* */ options,
				byte /*uint8_t*/ options_count);
}
