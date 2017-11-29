
/**
 * Enums for oc_cond_wait_for and oc_event_wait_for return values.
 */
#if EXPORT_INTERFACE
typedef enum
{
   OC_WAIT_SUCCESS = 0,    /**< Condition or event is signaled. */
   OC_WAIT_INVAL = -1,     /**< Condition or event is invalid. */
   OC_WAIT_TIMEDOUT = -2   /**< Condition or event is timed out. */
} OCWaitResult_t;
#endif	/* INTERFACE */
