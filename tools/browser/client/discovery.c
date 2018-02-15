#include "openocf.h"
#include "discovery.h"

/* #include <limits.h> */
/* #include <pthread.h> */
/* #if INTERFACE */
/* #include <cdk_test.h> */
/* #include <sys/stat.h>		/\* symbolic permission bits (linux)  *\/ */
/* #include <fcntl.h>           /\* For O_* constants *\/ */
/* #include <sys/stat.h>        /\* For mode constants *\/ */
/* #include <semaphore.h> */
/* #endif */
/* #include <stdarg.h> */
/* #include <stdio.h> */
/* #include <stdlib.h> */
/* #include <string.h> */
/* #include <signal.h> */
/* #include <time.h> */
/* #ifdef HAVE_UNISTD_H */
/* #include <unistd.h> */
/* #endif */
/* #ifdef HAVE_WINDOWS_H */
/* #include <windows.h> */
/* #endif */

/* #include <errno.h> */

#define TAG ("discovery")

// This is a function called back when resources are discovered
static OCStackApplicationResult resource_discovery_cb(void* ctx,
						      OCDoHandle handle,
						      OCClientResponse * clientResponse)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, tid %d", __func__, pthread_self());

    int rc;
    /* MAX_ADDR_STR_SIZE + MAX_URI_LENGTH + separators + \0 */
    char inbound_msg[66 + 256 + 3] = { '\0' };
    int sz = sizeof(inbound_msg) + sizeof(void*);

    char *ptrstr;
    char *endptr;
    unsigned long val;
    OCClientResponse *msg;

    log_discovery_payload(clientResponse);

    /* notify display mgr */
    rc = sem_wait(inbound_msg_log_ready_semaphore);
    if (rc < 0) {
	OIC_LOG_V(ERROR, TAG, "sem_wait(inbound_msg_log_ready_semaphore) rc: %s", strerror(errno));
    /* } else {
     * 	OIC_LOG_V(DEBUG, TAG, "PRODUCER acquired inbound_msg_log_ready_semaphore"); */
    }

    /* pthread_mutex_lock(&dirty_mutex); */
    cosp_mgr_register_coresource(clientResponse);
    inbound_msg_scroller_dirty = true;
    coresource_scroller_dirty = true;
    /* pthread_mutex_unlock(&dirty_mutex); */

    errno = 0;
    rc = sem_post(inbound_msg_semaphore);
    if (rc < 0) {
	OIC_LOG_V(DEBUG, TAG, "sem_post(inbound_msg_semaphore) rc: %s", strerror(errno));
    /* } else {
     * 	OIC_LOG_V(DEBUG, TAG, "PRODUCER sem_post(inbound_msg_semaphore)"); */
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    //return OC_STACK_DELETE_TRANSACTION;
    return OC_STACK_KEEP_TRANSACTION | OC_STACK_KEEP_RESPONSE;
}

void discover_resources ()
{
    OCDoHandle *request_handle;
    /* Start a discovery query*/
    OCCallbackData cbData;
    cbData.cb = resource_discovery_cb;
    cbData.context = NULL;
    cbData.cd = NULL;
    char szQueryUri[MAX_QUERY_LENGTH] = { 0 };
    strcpy(szQueryUri, OC_MULTICAST_DISCOVERY_URI);

    for (;;) {
	if (OCDoResource(&request_handle,
			 OC_REST_DISCOVER,
			 szQueryUri,
			 NULL,	/* endpoint - NULL for multicast */
			 NULL,	/* payload */
			 CT_DEFAULT, /* connectivity type */
			 OC_LOW_QOS,
			 &cbData, NULL, 0) != OC_STACK_OK) {
	    OIC_LOG(ERROR, TAG, "OCStack resource error");
	}
	/* notify display mgr */
	char *outbound_msg = "GET <multicast>/oic/res";
	int sz = sizeof(outbound_msg) + sizeof(void*);
	pthread_mutex_lock(&msgs_mutex);
	sem_wait(outbound_msg_log_ready_semaphore);
	if (outbound_msgs) {
	    char *s = malloc(sz);
	    snprintf(s, sz, "%s %p", outbound_msg, request_handle);
	    u_linklist_add(outbound_msgs, s);
	    OIC_LOG(INFO, TAG, s);
	} else {
	    OIC_LOG_V(ERROR, TAG, "outbound_msgs not initialized");
	    exit(EXIT_FAILURE);
	}
	pthread_mutex_unlock(&msgs_mutex);
	static int rc;
	rc = sem_post(outbound_msg_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(ERROR, TAG, "sem_post(outbound_msg_semaphore) rc: %s", strerror(errno));
	}
	/* if (g_quit_flag)
	 *     break;
	 * else sleep(5); */
	break;
	/* OIC_LOG_V(ERROR, TAG, "DISCOVERY LOOP"); */
    }
}

/* generate data for testing only */
/* static void* inbound_msg_display_handler(void *arg) */
/* { */
/*     OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self()); */
/*     srand((unsigned int)time(NULL)); */

/*     inbound_msgs = u_linklist_create(); */
/*     if (!inbound_msgs) { */
/* 	OIC_LOG_V(ERROR, TAG, "u_linklist_create inbound_msgs"); */
/* 	exit(EXIT_FAILURE); */
/*     } */

/*     char *inbound_msg = "inbound_msg"; */

/*     int sz = sizeof(inbound_msg) + 32; */
/*     static int rc; */
/*     while (1) { */
/* 	/\* OIC_LOG_V(DEBUG, TAG, "PRODUCER waiting on inbound_msg_log_ready_semaphore"); *\/ */
/* 	rc = sem_wait(inbound_msg_log_ready_semaphore); */
/* 	if (rc < 0) { */
/* 	    OIC_LOG_V(ERROR, TAG, "sem_wait(inbound_msg_log_ready_semaphore) rc: %s", strerror(errno)); */
/* 	} else { */
/* 	    /\* OIC_LOG_V(DEBUG, TAG, "PRODUCER acquired inbound_msg_log_ready_semaphore"); *\/ */
/* 	} */
/* 	static i = 1; */
/* 	pthread_mutex_lock(&msgs_mutex); */
/* 	if (inbound_msgs) { */
/* 	    char *s = malloc(sz); */
/* 	    snprintf(s, sz, "%s %d", inbound_msg, i++); */
/* 	    u_linklist_add(inbound_msgs, s); */
/* 	    /\* OIC_LOG(INFO, TAG, s); *\/ */
/* 	} */
/* 	pthread_mutex_unlock(&msgs_mutex); */
/* 	errno = 0; */
/* 	rc = sem_post(inbound_msg_semaphore); */
/* 	if (rc < 0) { */
/* 	    OIC_LOG_V(DEBUG, TAG, "sem_post(inbound_msg_semaphore) rc: %s", strerror(errno)); */
/* 	} else { */
/* 	    /\* OIC_LOG_V(DEBUG, TAG, "PRODUCER sem_post(inbound_msg_semaphore)"); *\/ */
/* 	} */
/*         sleep(3); */
/*         /\* sleep((float)rand()/(float)(RAND_MAX/2)); *\/ */
/*     } */

/*     OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__); */
/*     return 0; */
/* } */

/* generate data for testing only */
/* static void* outbound_msg_display_handler(void *arg) */
/* { */
/*     OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self()); */
/*     srand((unsigned int)time(NULL)); */

/*     outbound_msgs = u_linklist_create(); */

/*     char *outbound_msg = "outbound_msg"; */
/*     int sz = sizeof(outbound_msg) + 32; */
/*     while (1) { */
/* 	static i = 1; */
/* 	pthread_mutex_lock(&msgs_mutex); */
/* 	sem_wait(outbound_msg_log_ready_semaphore); */
/* 	if (outbound_msgs) { */
/* 	    char *s = malloc(sz); */
/* 	    snprintf(s, sz, "%s %d", outbound_msg, i++); */
/* 	    u_linklist_add(outbound_msgs, s); */
/* 	    /\* OIC_LOG(INFO, TAG, s); *\/ */
/* 	} */
/* 	pthread_mutex_unlock(&msgs_mutex); */
/* 	static int rc; */
/* 	rc = sem_post(outbound_msg_semaphore); */
/* 	if (rc < 0) { */
/* 	    OIC_LOG_V(ERROR, TAG, "sem_post(outbound_msg_semaphore) rc: %s", strerror(errno)); */
/* 	} */
/*         sleep(2); */
/*         /\* sleep((float)rand()/(float)(RAND_MAX/2)); *\/ */
/*     } */
/*     OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__); */
/*     return 0; */
/* } */
