package openocf.utils;

import openocf.constants.PropertyType;
import java.util.HashMap;

// These are properties (name,val pairs) of a resource, but are in a message payload.
// so they are resource properties but indirectly. they are part of a signal.

// TODO: rename betteer to reflect the semantics

// Alternative: just treat this as a utility class - who cares where the props came from?
// so put in openocf.utils.PropertyMap?

public class PropertyMap<K, V> extends HashMap<K, V> // OCRepPayload.values
{
    private long handleOCRepPayload; // handle of containing payload

    // in struct OCRepPayload:
    //     OCRepPayloadValue* values;
    // private long handle;	// handle of "property" linked list in containing payload

    // typedef struct OCRepPayloadValue
    // {
    //     char* name;
    //     OCRepPayloadPropType type;
    //     union
    //     {
    //         int64_t i;
    //         double d;
    //         bool b;
    //         char* str;

    //         /** ByteString object.*/
    //         OCByteString ocByteStr;

    //         struct OCRepPayload* obj;
    //         OCRepPayloadValueArray arr;
    //     };
    //     struct OCRepPayloadValue* next;
    // } OCRepPayloadValue;

    public native V getProp(String name);
    private native V setProp(String name, V value, long type);

    // @Override
    // public V put(K name, V value)
    // {
    // 	System.out.println("PP put: " + name + " = " + value);
    // 	if ( !(name instanceof String) ) {
    // 	    // throw bad param
    // 	    throw new IllegalArgumentException("Property Key must be a String");
    // 	}
    // 	// validate value type
    // 	if ( value instanceof Integer ) {
    // 	    System.out.println("Property value is int");
    // 	    V retval = getProp((String)name);
    // 	    System.out.println("retval: " + retval);
    // 	    setProp( (String)name, value, PropertyType.INT );
    // 	    return retval;
    // 	} else {
    // 	    return null;
    // 	}

	// switch(value) {
	// case (int)value:
	//     type = PropertyType.INT;
	//     break;
	// }
	// update underlying struct
	// SetProp(handle, name, value,
    // }
}
