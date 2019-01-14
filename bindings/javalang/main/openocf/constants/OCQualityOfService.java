package openocf.constants;

public class OCQualityOfService
{
    /** Packet delivery is best effort.*/
    public static final int LOW = 0;

    /** Packet delivery is best effort.*/
    public static final int MEDIUM = 1;

    /** Acknowledgments are used to confirm delivery.*/
    public static final int HIGH = 2;

    /** No Quality is defined, let the stack decide.*/
    public static final int NA = 3;
}
