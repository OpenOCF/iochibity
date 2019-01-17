package openocf.engine;

import java.util.LinkedList;

public class ActionSet		// OCActionSet
{
    // typedef struct ocactionset
    // {
    // linked list; for list of action set. */
    // struct ocactionset *next;

    // Name of the action set.*/
    // char *actionsetName;
    public String name;

    // Time stamp.*/
    // long int timesteps;
    public long timestamp;

    // Type of action.*/
    // unsigned int type;
    public int type;

    //     /** head pointer of a linked list of Actions.*/
    //     OCAction* head;
    public LinkedList<Action> actions;
    // } OCActionSet;
}
