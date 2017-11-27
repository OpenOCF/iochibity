
#if EXPORT_INTERFACE
typedef struct OCStringLL
{
    struct OCStringLL *next;
    char* value;
} OCStringLL;
#endif	/* INTERFACE */
