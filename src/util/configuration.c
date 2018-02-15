#include "configuration.h"
#include <string.h>

static const char* version = "1.3.0";

const char * configuration() EXPORT
{
    return version;
}
