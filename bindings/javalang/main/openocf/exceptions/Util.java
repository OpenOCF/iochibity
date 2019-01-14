package openocf.exceptions;

/**
 * Support routines for the JNI exception classes.
 */
public class Util {

    /**
     * Pushes a stack trace element onto the existing stack trace of the throwable.
     */
    public static void addStackTraceElement (
        Throwable t,
        String functionName,
        String file,
        int line) {

        StackTraceElement[] currentStack =
            t​.getStackTrace ();
        StackTraceElement[] newStack =
            new StackTraceElement[currentStack​.length + 1];

        System​.arraycopy (currentStack, 0,
            newStack, 1, currentStack​.length);

        file = file​.replace ('\\', '/');
        if (file​.lastIndexOf ('/') > -1) {
            file = file​.substring (file​.lastIndexOf ('/') + 1);
        }

        newStack[0] = new StackTraceElement ("<native>", functionName, file, line);

        t​.setStackTrace (newStack);
    }
}
