/* platform-independent thread api, for composition with
   platform-dependent code */

#include <stdint.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#if EXPORT_INTERFACE
#include <stdint.h>		/* api args uint32_t etc. */

#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

/**
 * Value used for the owner field of an oc_mutex that doesn't have an owner.
 */
#define OC_INVALID_THREAD_ID    0

typedef enum
{
    OC_THREAD_SUCCESS = 0,
    OC_THREAD_ALLOCATION_FAILURE = 1,
    OC_THREAD_CREATE_FAILURE=2,
    OC_THREAD_INVALID=3,
    OC_THREAD_WAIT_FAILURE=4,
    OC_THREAD_INVALID_PARAMETER=5
} OCThreadResult_t;

typedef struct oc_event_t *oc_event;
typedef struct oc_mutex_internal *oc_mutex;
typedef struct oc_cond_internal *oc_cond;
typedef struct oc_thread_internal *oc_thread;

#endif
