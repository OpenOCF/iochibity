
#if INTERFACE
typedef struct OCStringLL
{
    struct OCStringLL *next;
    char* value;
} OCStringLL;
#endif	/* INTERFACE */
