/* This file was automatically generated.  Do not edit! */
int run_gui(void);
#include <cdk_test.h>
#include <semaphore.h>
int main();
void discover_resources();
extern bool coresource_scroller_dirty;
extern bool inbound_msg_scroller_dirty;
void log_discovered_resources(OCClientResponse *clientResponse);
extern FILE *logfd;
extern u_linklist_t *outgoing_msgs;
extern u_linklist_t *inbound_msgs;
extern sem_t *outgoing_msg_log_ready_semaphore;
extern sem_t *outgoing_msg_log_ready_semaphore;
extern sem_t *outgoing_msg_semaphore;
extern sem_t *outgoing_msg_semaphore;
extern sem_t *inbound_msg_log_ready_semaphore;
extern sem_t *inbound_msg_log_ready_semaphore;
extern sem_t *inbound_msg_semaphore;
extern sem_t *inbound_msg_semaphore;
extern pthread_mutex_t dirty_mutex;
extern pthread_mutex_t msgs_mutex;
extern pthread_mutex_t display_mutex;
extern CDKSCREEN *cdkscreen;
extern pthread_t quit_thread;
extern sem_t *quit_semaphore;
extern int g_quit_flag;
extern int g_quit_flag;
#define INTERFACE 0
