#ifndef CLIENT_H
#define CLIENT_H	1

#include <pthread.h>
#include <semaphore.h>
#include <cdk_test.h>

int run_action_dialog (CDKSCREEN *cdkscreen);


pthread_mutex_t  msgs_mutex;

sem_t *incoming_msg_semaphore;
sem_t *incoming_msg_log_ready_semaphore;

sem_t *outgoing_msg_semaphore;
sem_t *outgoing_msg_log_ready_semaphore;

u_linklist_t    *incoming_msgs;
u_linklist_t    *outgoing_msgs;

#endif /* CLIENT_H */
