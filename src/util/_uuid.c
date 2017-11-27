/**
 * Node to construct UUID linked list.
 */
/* typedef struct OCUuidList  OCUuidList_t; */
#if EXPORT_INTERFACE
typedef struct OCUuidList_t
{
    OicUuid_t dev;
    OCUuidList_t *next;
} OCUuidList_t;
#endif	/* INTERFACE */
