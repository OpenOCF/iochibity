/* #if EXPORT_INTERFACE */
#define HISTORYSIZE (4)
/* #endif	/\* INTERFACE *\/
 * 
 * #if EXPORT_INTERFACE */
typedef struct
{
    CATransportFlags_t flags;
    uint16_t messageId;
    char token[CA_MAX_TOKEN_LEN];
    uint8_t tokenLength;
    uint32_t ifindex;
} CAHistoryItem_t;
/* #endif	/\* INTERFACE *\/
 * 
 * #if EXPORT_INTERFACE */
typedef struct
{
    int nextIndex;
    CAHistoryItem_t items[HISTORYSIZE];
} CAHistory_t;
/* #endif	/\* INTERFACE *\/ */

