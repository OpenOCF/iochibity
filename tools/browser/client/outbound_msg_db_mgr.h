/* This file was automatically generated.  Do not edit! */
void draw_msg_scrollers();
#define INBOUND 0
extern int next_scroller;
void destroy_msg_scrollers();
int run_outbound_msg_db_mgr(void);
void reinitialize_outbound_msg_scroller();
#include <cdk_test.h>
#include <semaphore.h>
extern CDKSCREEN *cdkscreen;
void initialize_outbound_msg_scroller(void);
extern CDKLABEL *msg_box;
extern CDKLABEL *infoBox;
extern sem_t *outbound_msg_log_ready_semaphore;
extern sem_t *outbound_msg_log_ready_semaphore;
extern u_linklist_t *outbound_msgs;
extern pthread_mutex_t msgs_mutex;
extern int g_quit_flag;
extern int g_quit_flag;
extern sem_t *outbound_msg_semaphore;
extern sem_t *outbound_msg_semaphore;
void *outbound_msg_ui_writer(void *arg);
extern int outbound_current_item;
extern bool outbound_msg_scroller_dirty;
extern CDKSCROLL *outbound_msg_scroller;
