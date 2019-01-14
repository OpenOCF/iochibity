package openocf.exceptions;

public class OCFStackException extends JNIRuntimeException {

    public OCFStackException(int code, String codeMessage, String message) {
	super (code + " " + codeMessage + " (" + message + ")");
    }
}
