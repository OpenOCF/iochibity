package openocf.constants;

public class ObserveAction	// OCObserveAction
{
    // typedef enum
    // {
    //     /** To Register. */
    //     OC_OBSERVE_REGISTER = 0,
    public static final int REGISTER    = 0;

    //     /** To Deregister. */
    //     OC_OBSERVE_DEREGISTER = 1,
    public static final int DEREGISTER  = 1;
    //     /** Others. */
    //     OC_OBSERVE_NO_OPTION = 2,
    public static final int NO_OPTION   = 2;
    // //#ifdef WITH_MQ
    //     OC_MQ_SUBSCRIBER = 3,
    public static final int MQTT_SUBSCRIBER   = 4;
    //     OC_MQ_UNSUBSCRIBER = 4,
    public static final int MQTT_UNSUBSCRIBER = 5;
    // //#endif
    // } OCObserveAction;
}
