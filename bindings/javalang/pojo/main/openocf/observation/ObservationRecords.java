package openocf.observation;

import java.util.LinkedList;

import openocf.constants.PropertyType;
import openocf.observation.ObservationRecord;

public class ObservationRecords extends LinkedList<ObservationRecord>
{
    private long head; // handle of containing payload

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

}
