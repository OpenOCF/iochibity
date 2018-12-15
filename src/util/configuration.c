#include "configuration.h"
#include <string.h>
#include <stdio.h>

static const char* version = "1.3.0";

const char * configuration(void) EXPORT
{
    return version;
}

const char * oocf_get_version(void) EXPORT
{
    // OIC_LOG_V(DEBUG, TAG, "%s ENTRY %s", __func__, version);
    return version;
}
