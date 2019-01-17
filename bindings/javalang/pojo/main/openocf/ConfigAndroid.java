package openocf;

import openocf.OpenOCF;

import android.util.Log;
import android.content.Context;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URL;
import java.util.StringTokenizer;

//import cz.adamh.utils.NativeUtils;

/**
 *
 */
public class ConfigAndroid {
    private static final String TAG = "ConfigAndroid";

    static {
        Log.i(TAG, "static initializer ENTRY");
	try{
	    Log.i(TAG, "loading libopenocf_jni.so");
	    System.loadLibrary("openocf_jni");
	} catch (Exception e) {
	    // This is probably not the best way to handle exception :-)
	    Log.i(TAG, "loadLibrary failed for openocfserver");
	    e.printStackTrace();
	}
	Log.i(TAG, "LOADED libopenocf_jni.so");
        Log.i(TAG, "static initializer EXIT");
    }

    // FIXME: can we do this without android Context?
    public static void config(Context ctx, String apk)
    {
	Log.v(TAG, "CONFIG");
	String property = System.getProperty("java.library.path");
	StringTokenizer parser = new StringTokenizer(property, ";");
	while (parser.hasMoreTokens()) {
	    Log.v(TAG, parser.nextToken());
	}
	// Log.v(TAG, "openocf.ConfigAndroid::config");
	// try{
	//     System.loadLibrary(apk); // "openocf_jni");
	//     //System.load("/libopenocf_jni.so");
	// } catch (Exception e) {
        //     // This is probably not the best way to handle exception :-)
	//     Log.i(TAG, "loadLibrary failed for openocfserver");
        //     e.printStackTrace();
        // }

	try {
	    OpenOCF.config_logging(null);
	} catch(Exception e){
	    Log.e(TAG, e.toString());
	    System.exit(0);
	}
	Log.e(TAG, "logging configured");

	try {
	    Log.i(TAG, "Getting path for resource " + "svrs.cbor");
	    String f = extractAssetToFile(ctx, "svrs.cbor");
	    OpenOCF.config_svrs(f);
	} catch(Exception e){
	    Log.i(TAG, e.toString());
	    System.exit(0);
	}
    }

    public static void extractAsset(Context context, String filename)
	// throws BBBEPubFileNotFoundException
    {
	try {
	    InputStream myInput = context.getAssets().open(filename);
	    OutputStream myOutput = context.openFileOutput(filename,
							   Context.MODE_PRIVATE);

	    // transfer bytes from the input file to the output file
	    byte[] buffer = new byte[1024];
	    int length;
	    while ((length = myInput.read(buffer)) > 0) {
		myOutput.write(buffer, 0, length);
	    }

	    // Close the streams
	    myOutput.flush();
	    myOutput.close();
	    myInput.close();
	} catch (Exception e) {
	    Log.i(TAG, e.toString());
	    System.exit(0);
	}
    }
    private static String extractAssetToFile(Context ctx, String fileName)
    {
	Log.v(TAG, "private files path: " + ctx.getFilesDir());
	File file = new File(ctx.getFilesDir(), fileName);
	String fullPath = file.getAbsolutePath();
	if (!file.exists()) {
		extractAsset(ctx, fileName);
	}
	return fullPath;
    }
}
