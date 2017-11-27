#ifdef HAVE_WINDOWS_H
#include <windows.h>
typedef struct oc_event_t
{
    HANDLE event;
} oc_event_t;
typedef struct oc_event_t* oc_event;
#endif

