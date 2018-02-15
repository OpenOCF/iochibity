/* utils.c */

#include "openocf.h"
#include "utils.h"

bool starts_with(const char *pfx, const char *str)
{
    size_t lenpfx = strlen(pfx),
           lenstr = strlen(str);
    return lenstr < lenpfx ? false : strncmp(pfx, str, lenpfx) == 0;
}
