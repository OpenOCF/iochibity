package openocf.exceptions;

public class JNIRuntimeException extends RuntimeException {
    public JNIRuntimeException(String message) {
        super(message);
    }

    /**
     * Called by native code during construction to set the location.
     */
    public void _​_jni​_setLocation (String functionName, String file, int line) {
        Util​.addStackTraceElement (this, functionName, file, line);
    }
}
