#ifndef CLIENT_H
#define CLIENT_H	1

#include <pthread.h>
#include <semaphore.h>
#include <cdk_test.h>

#include "openocf.h"

extern int       g_quit_flag;
sem_t           *quit_semaphore;
pthread_t        quit_thread;

CDKSCREEN  *cdkscreen;

pthread_mutex_t display_mutex;
pthread_mutex_t  msgs_mutex;

extern sem_t *incoming_msg_semaphore;
extern sem_t *incoming_msg_log_ready_semaphore;

extern sem_t *outgoing_msg_semaphore;
extern sem_t *outgoing_msg_log_ready_semaphore;

u_linklist_t    *incoming_msgs;
u_linklist_t    *outgoing_msgs;

CDKLABEL *infoBox;

int run_action_dialog (CDKSCREEN *cdkscreen, char * item);
int run_msg_dialog (CDKSCREEN *cdkscreen, char * item);

#endif /* CLIENT_H */
