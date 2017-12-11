/* This file was automatically generated.  Do not edit! */
void initialize_coresource_scroller(void);
#include <cdk_test.h>
#include <semaphore.h>
extern CDKSCREEN *cdkscreen;
void initialize_inbound_msg_scroller(void);
void init_msg_log_scrollers();
extern CDKSCROLL *inbound_msg_scroller;
void draw_scrollers();
extern CDKLABEL *msg_box;
extern sem_t *outgoing_msg_log_ready_semaphore;
extern sem_t *outgoing_msg_log_ready_semaphore;
extern u_linklist_t *outgoing_msgs;
extern pthread_mutex_t msgs_mutex;
extern int g_quit_flag;
extern int g_quit_flag;
extern sem_t *outgoing_msg_semaphore;
extern sem_t *outgoing_msg_semaphore;
void *outgoing_msg_ui_writer(void *arg);
void reset_scroller(CDKSCROLL *scroller);
extern SCREEN *screen;
extern SCREEN *screen;
extern int coresource_current_item;
extern int outgoing_current_item;
extern CDKSCROLL *coresource_scroller;
extern CDKSCROLL *outgoing_msg_scroller;
#if defined(HAVE_XCURSES)
extern char *XCursesProgramName;
extern char *XCursesProgramName;
extern char *XCursesProgramName;
#endif
