// FIXME: make this abstract? client should only use OCFClientSP, OCFServerSP, or OCFClientServerSP, not this

package openocf.engine;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URL;

import cz.adamh.utils.NativeUtils;

/**
 *
 */
// abstract?
public class OCFCommonSP {
    private static final String TAG = "openocf.OCFCommonSP";

    // OCMode
    public static final int CLIENT = 0;
    public static final int SERVER = 1;
    public static final int CLIENT_SERVER = 2;
    public static final int GATEWAY = 3;

    // OCQualityOfService
    public static final int QOS_LOW    = 0; // best-effort
    public static final int QOS_MEDIUM = 1;
    public static final int QOS_HIGH   = 2; // ACKs confirm delivery
    public static final int QOS_NA     = 3; // stack decides

    // ****************************************************************
    // public static native void config_logging(String logfile_fname);
    // public static native void config_svrs(String svrs_config_fname);

    // returns configuration info from engine, e.g. version string
    public static native String configuration();

    public static native void /*OCStackResult*/
	Init(int /*OCMode*/ mode);
	     // String securityConfigFileName);

    // public static native void /*OCStackResult*/
    // 	Init(String /*const char* */ ip_addr,
    // 	     int /*uint16_t*/ port,
    // 	     int /*OCMode*/ mode,
    // 	     String securityConfigFileName);

    public static native void /*OCStackResult*/
	Init(int /*OCMode*/ mode,
	     int /*OCTransportFlags*/ server_flags,
	     int /*OCTransportFlags*/ client_flags);

    private native int /*OCStackResult*/
    // OCRegisterPersistentStorageHandler(Object /*OCPersistentStorage* */ persistent_storage_handler);
    // OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };
    // OCRegisterPersistentStorageHandler(&ps);
	XOCRegisterPersistentStorageHandler(Object /*OCPersistentStorage* */ persistent_storage_handler);
    //NB: default ps file is "oic_svr_db.dat"

    public native void /*OCStackResult*/ OCStartMulticastServer();

    public native void /*OCStackResult*/ OCStopMulticastServer();

    public static native void /*OCStackResult*/	run();

    public static native void /*OCStackResult*/ stop();

    // setPlatformInfo =>  ServiceManager.registerPlatform

    // setDeviceInfo =>  ServiceManager.registerDevice

    // OCDoResource =>  ServiceManager.sendResource

    public native void OCCancel(Object /*OCDoHandle*/ handle,
				int /*OCQualityOfService*/ qos,
				Object /*OCHeaderOption* */ options,
				byte /*uint8_t*/ options_count);
}
