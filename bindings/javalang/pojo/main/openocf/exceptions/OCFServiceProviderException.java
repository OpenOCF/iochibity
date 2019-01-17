package openocf.exceptions;

public class OCFServiceProviderException extends JNIRuntimeException {

    public OCFServiceProviderException(int code, String codeMessage, String message) {
	super (code + " " + codeMessage + " (" + message + ")");
    }
}
