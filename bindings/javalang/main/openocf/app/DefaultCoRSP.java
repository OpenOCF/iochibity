// FIXME: do we need a default corsp?

package openocf.app;

import openocf.constants.Method;
import openocf.message.InboundResponse;

public class DefaultCoRSP
    extends  CoResourceSP
{
    public DefaultCoRSP() {
	super();
    }

    @Override
    public void coReact(InboundResponse message)
    {
	System.out.println("DefaultCoRSP.coReact ENTRY");
	// return 0;
    }
}
