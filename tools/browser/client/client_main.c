//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "openocf.h"
#include "client_main.h"

#include <limits.h>
#include <pthread.h>
#if INTERFACE
#include <cdk_test.h>
#include <sys/stat.h>		/* symbolic permission bits (linux)  */
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#include <errno.h>

#define TAG ("client")

/* #if INTERFACE */
int       g_quit_flag;
sem_t           *quit_semaphore;
pthread_t        quit_thread;

CDKSCREEN  *cdkscreen;

pthread_mutex_t display_mutex;
pthread_mutex_t msgs_mutex;
pthread_mutex_t dirty_mutex;

sem_t *inbound_msg_semaphore;
sem_t *inbound_msg_log_ready_semaphore;

sem_t *outbound_msg_semaphore;
sem_t *outbound_msg_log_ready_semaphore;

/* u_linklist_t    *inbound_msgs; */
u_linklist_t    *outbound_msgs;

/* #endif */

int g_quit_flag = 0;

sem_t *inbound_msg_semaphore = NULL;
sem_t *inbound_msg_log_ready_semaphore = NULL;

sem_t *outbound_msg_semaphore = NULL;
sem_t *outbound_msg_log_ready_semaphore = NULL;

static oc_thread        response_msg_dispatcher_thread;

/* static pthread_t        inbound_msg_display_thread;
 *
 * static pthread_t        outbound_msg_display_thread; */

static pthread_t        discovery_thread; /* just for testing */

FILE                   *logfd;

static void quit_routine(void)
{
    OIC_LOG_V(INFO, TAG, "%s ENTRY", __func__);
    int rc = sem_wait(quit_semaphore);
    if (rc < 0) {
	OIC_LOG_V(ERROR, TAG, "sem_wait(quit_semaphore) rc: %s", strerror(errno));
    }
    OIC_LOG_V(INFO, TAG, "%s acquired quit_semaphore", __func__);
    rc = pthread_mutex_lock(&msgs_mutex);
    if (rc < 0) {
	OIC_LOG_V(ERROR, TAG, "mutex lock failed on msgs_mutex rc: %s", strerror(errno));
    }
    g_quit_flag = 1;
    sem_post(inbound_msg_semaphore);
    sem_post(outbound_msg_semaphore);
    pthread_mutex_unlock(&msgs_mutex);
    OIC_LOG_V(INFO, TAG, "%s EXIT %d", __func__, g_quit_flag);
}

/* SIGINT handler: set g_quit_flag to 1 for graceful termination */
static void handleSigInt(int signum) {
    if (signum == SIGINT) {
	g_quit_flag = 1;
	int rc = sem_post(quit_semaphore);
	if (rc < 0) {
	    OIC_LOG_V(ERROR, TAG, "sem_wait(quit_semaphore) rc: %s", strerror(errno));
	}
    }
}

// This is a function called back when resources are discovered
static OCStackApplicationResult resource_discovery_cb(void* ctx,
						  OCDoHandle handle,
						  OCClientResponse * clientResponse) {
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, tid %d", __func__, pthread_self());

    int rc;
    /* MAX_ADDR_STR_SIZE + MAX_URI_LENGTH + separators + \0 */
    char inbound_msg[66 + 256 + 3] = { '\0' };
    int sz = sizeof(inbound_msg) + sizeof(void*);

    char *ptrstr;
    char *endptr;
    unsigned long val;
    OCClientResponse *msg;

    log_discovery_msg(clientResponse);

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

static void* response_msg_dispatcher(void *arg) {
    OIC_LOG_V(INFO, TAG, "%s ENTRY, tid: %x", __func__, pthread_self());

    // Break from loop with Ctrl+C
    signal(SIGINT, handleSigInt);
    int i = 0;
    while (!g_quit_flag) {
	/* OIC_LOG_V(INFO, TAG, "process loop %d, tid %d", i++, pthread_self()); */
        if (OCProcess() != OC_STACK_OK) {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }
	fflush(logfd);		/* FIXME */
        sleep(1);
    }

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    return 0;
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

int main ()
{
    int err;

    /* OCLogInit(NULL); */
    logfd = fopen("./logs/client.log", "w");
    if (logfd)
    	OCLogInit(logfd);
    else {
    	printf("Logfile fopen failed\n");
    	exit(EXIT_FAILURE);
    }

    OIC_LOG_V(DEBUG, TAG, "%s ENTRY, tid %d", __func__, pthread_self());

    pthread_mutex_init(&msgs_mutex, NULL);
    pthread_mutex_init(&display_mutex, NULL);
    pthread_mutex_init(&dirty_mutex, NULL);

    /* msg lists for passing to display mgr */
    /* inbound_msgs = u_linklist_create(); */
    /* if (!inbound_msgs) { */
    /* 	OIC_LOG_V(ERROR, TAG, "u_linklist_create inbound_msgs"); */
    /* 	exit(EXIT_FAILURE); */
    /* } else { */
    /* 	OIC_LOG_V(INFO, TAG, "EXIT u_linklist_create inbound_msgs"); */
    /* } */

    outbound_msgs = u_linklist_create();
    if (!outbound_msgs) {
    	OIC_LOG_V(ERROR, TAG, "u_linklist_create outbound_msgs");
    	exit(EXIT_FAILURE);
    } else {
    	OIC_LOG_V(INFO, TAG, "EXIT u_linklist_create outbound_msgs");
    }

    errno = 0;
    int rc = 0;
    rc = sem_unlink("/oocf_quit_sem");
    if (rc != 0) {
      if (errno != ENOENT) {
    	OIC_LOG_V(ERROR, TAG, "sem_unlink(\"/oocf_quit_sem\") rc: %d %s", errno, strerror(errno));
    	/* exit(EXIT_FAILURE); */
      }
    }
    if ((quit_semaphore = sem_open("/oocf_quit_sem", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED ) {
      OIC_LOG_V(ERROR, TAG, "sem_open(\"/oocf_quit_sem\") rc: %d %s", errno, strerror(errno));
    	exit(EXIT_FAILURE);
    /* } else {
     * 	OIC_LOG_V(DEBUG, TAG, "sem_open(\"/oocf_quit_sem) returned: %p", quit_semaphore);
     * 	/\* sem_wait(quit_semaphore); *\/
     * 	int val;
     * 	sem_getvalue(quit_semaphore, &val);
     * 	OIC_LOG_V(DEBUG, TAG, "quit_semaphore val %d", val); */
    }

    /* enable quit handler */
    err = pthread_create(&(quit_thread), NULL, &quit_routine, NULL);
    if (err != 0) {
    	OIC_LOG_V(DEBUG, TAG, "Can't create quit_thread :[%s]", strerror(err));
    	exit(EXIT_FAILURE);
    }

    /* OS X does not support unnamed semaphores (sem_init) */
    sem_unlink("/inbound_msg");
    if (rc != 0) {
      if (errno != ENOENT) {
    	OIC_LOG_V(ERROR, TAG, "sem_unlink(\"/inbound_msg\") rc: %d %s", errno, strerror(errno));
    	/* exit(EXIT_FAILURE); */
      }
    }
    if ((inbound_msg_semaphore = sem_open("/inbound_msg", O_CREAT, 0644, 0)) == SEM_FAILED ) {
	OIC_LOG_V(ERROR, TAG, "sem_open(\"/inbound_msg\") rc: %s", strerror(errno));
	exit(EXIT_FAILURE);
    /* } else {
     * 	OIC_LOG_V(DEBUG, TAG, "sem_open(\"/inbound_msg) returned: %p", inbound_msg_semaphore); */
    }

    sem_unlink("/inbound_msg_log_ready");
    if ((inbound_msg_log_ready_semaphore = sem_open("/inbound_msg_log_ready", O_CREAT, 0644, 0)) == SEM_FAILED ) {
	OIC_LOG_V(ERROR, TAG, "sem_open(\"/inbound_msg_log_ready\") rc: %s", strerror(errno));
	exit(EXIT_FAILURE);
    /* } else {
     * 	OIC_LOG_V(DEBUG, TAG, "sem_open(\"/inbound_msg_log_ready) returned: %p", inbound_msg_log_ready_semaphore); */
    }

    sem_unlink("/outbound_msg_semaphore");
    if ((outbound_msg_semaphore = sem_open("/outbound_msg_semaphore", O_CREAT, 0644, 0)) == SEM_FAILED ) {
	OIC_LOG_V(ERROR, TAG, "sem_open(\"/outbound_msg\") rc: %s", strerror(errno));
	exit(EXIT_FAILURE);
    /* } else {
     * 	OIC_LOG_V(DEBUG, TAG, "sem_open(\"/outbound_msg) returned: %p", outbound_msg_semaphore); */
    }

    sem_unlink("/outbound_msg_log_ready");
    if ((outbound_msg_log_ready_semaphore = sem_open("/outbound_msg_log_ready", O_CREAT, 0644, 1)) == SEM_FAILED ) {
	OIC_LOG_V(ERROR, TAG, "sem_open(\"/outbound_msg_log_ready\") rc: %s", strerror(errno));
	exit(EXIT_FAILURE);
    /* } else {
     * 	OIC_LOG_V(DEBUG, TAG, "sem_open(\"/outbound_msg_log_ready) returned: %p", outbound_msg_log_ready_semaphore); */
    }

    /* Initialize OCStack. Do this here rather than in the work
       thread, to ensure initialization is complete before sending any
       request. */
    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack init error");
	exit(EXIT_FAILURE);
    }

    /* now dispatch inbound msgs */
    err = pthread_create(&(response_msg_dispatcher_thread), NULL, &response_msg_dispatcher, NULL);
    if (err != 0) {
	OIC_LOG_V(DEBUG, TAG, "Can't create response_msg_dispatcher_thread :[%s]", strerror(err));
	exit(EXIT_FAILURE);
    }

    /* err = pthread_create(&(inbound_msg_display_thread), NULL, &inbound_msg_display_handler, NULL);
     * if (err != 0) {
     * 	OIC_LOG_V(DEBUG, TAG, "Can't create inbound_msg_display_handler thread :[%s]", strerror(err));
     * 	exit(EXIT_FAILURE);
     * } */

    /* err = pthread_create(&(outbound_msg_display_thread), NULL, &outbound_msg_display_handler, NULL);
     * if (err != 0) {
     * 	OIC_LOG_V(DEBUG, TAG, "Can't create msg thread :[%s]", strerror(err));
     * 	exit(EXIT_FAILURE);
     * } */

    /* generate some traffic, for testing */
    /* err = pthread_create(&(discovery_thread), NULL, &discover_resources, NULL); */
    /* if (err != 0) { */
    /* 	OIC_LOG_V(DEBUG, TAG, "Can't create discovery_thread :[%s]", strerror(err)); */
    /* 	exit(EXIT_FAILURE); */
    /* } */

    main_ui();

    pthread_join(response_msg_dispatcher_thread, NULL);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT joined", __func__);

    /* quiting: cleanup msg lists */
    int mutex_rc = pthread_mutex_lock(&msgs_mutex);
    if (mutex_rc) {
	    OIC_LOG_V(INFO, TAG, "FAIL: pthread_mutex_lock(&msgs_mutex), rc: %s",
		    (mutex_rc == EINVAL)? "EINVAL"
		    :(mutex_rc == EDEADLK) ? "EDEADLLK"
		    : "UNKNOWN ERROR");
    } else {
	/* int lllen = u_linklist_length(inbound_msgs); */
	/* OIC_LOG_V(INFO, TAG, "inbound msg count: %d", lllen); */
	/* /\* free the strings, then the list *\/ */
	/* u_linklist_iterator_t *iterTable = NULL; */
	/* u_linklist_init_iterator(inbound_msgs, &iterTable); */
	/* while (NULL != iterTable) */
	/*     { */
	/* 	char *s = u_linklist_get_data(iterTable); */
	/* 	OIC_LOG_V(INFO, TAG, "freeing msg: %s", s); */
	/* 	free(s); */
	/* 	u_linklist_get_next(&iterTable); */
	/*     } */
	/* u_linklist_free(&inbound_msgs); */

	/* lllen = u_linklist_length(outbound_msgs); */
	/* OIC_LOG_V(INFO, TAG, "outbound msg count: %d", lllen); */
	/* /\* free the strings, then the list *\/ */
	/* iterTable = NULL; */
	/* u_linklist_init_iterator(outbound_msgs, &iterTable); */
	/* while (NULL != iterTable) */
	/*     { */
	/* 	char *s = u_linklist_get_data(iterTable); */
	/* 	OIC_LOG_V(INFO, TAG, "freeing msg: %s", s); */
	/* 	free(s); */
	/* 	u_linklist_get_next(&iterTable); */
	/*     } */
	/* u_linklist_free(&outbound_msgs); */
    }
    pthread_mutex_unlock(&msgs_mutex);

    if (OCStop() != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
    }

    oocf_coresource_mgr_reset();

 /* FIXME: semaphores */

    pthread_mutex_destroy(&msgs_mutex);
    pthread_mutex_destroy(&display_mutex);
    pthread_mutex_destroy(&dirty_mutex);

    fclose(logfd);

    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
    /* pthread_exit(NULL); */
}
