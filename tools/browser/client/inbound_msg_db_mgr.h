/* This file was automatically generated.  Do not edit! */
int run_inbound_msg_db_mgr_dlg(void);
#define OUTBOUND 1
extern int next_scroller;
void destroy_msg_scrollers();
void draw_msg_scrollers();
void reinitialize_outbound_msg_scroller();
int run_inbound_msg_db_mgr(void);
void reinitialize_inbound_msg_scroller();
void initialize_inbound_msg_scroller(void);
extern CDKLABEL *msg_box;
#include <cdk_test.h>
#include <semaphore.h>
extern CDKSCREEN *cdkscreen;
int run_inbound_msg_inspector(CDKSCREEN *cdkscreen,int index);
extern CDKLABEL *infoBox;
extern bool coresource_scroller_dirty;
extern pthread_mutex_t dirty_mutex;
extern pthread_mutex_t msgs_mutex;
extern int g_quit_flag;
extern int g_quit_flag;
extern sem_t *inbound_msg_semaphore;
extern sem_t *inbound_msg_semaphore;
extern sem_t *inbound_msg_log_ready_semaphore;
extern sem_t *inbound_msg_log_ready_semaphore;
void *inbound_msg_ui_writer(void *arg);
extern int inbound_current_item;
extern bool inbound_msg_scroller_dirty;
extern CDKSCROLL *inbound_msg_scroller;
