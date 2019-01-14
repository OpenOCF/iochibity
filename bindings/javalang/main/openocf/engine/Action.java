package openocf.engine;

import java.util.LinkedList;

public class Action		// OCAction
{
    // typedef struct ocaction {
    //     /** linked list; for multiple actions. */
    //     struct ocaction *next;

    //     /** Target Uri. It will be used to execute the action. */
    //     char *resourceUri;
    public String resourceUri;

    //     /** head pointer of a linked list of capability nodes.*/
    //     OCCapability* head;
    public LinkedList<Capability> capabilities;
    // } OCAction;
}
