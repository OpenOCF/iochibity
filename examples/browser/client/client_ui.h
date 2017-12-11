/* This file was automatically generated.  Do not edit! */
extern int coresource_current_item;
void reinitialize_coresource_scroller();
extern bool coresource_scroller_dirty;
void run_coresource_mgr(void);
extern int outgoing_current_item;
void reinitialize_inbound_msg_scroller();
extern bool inbound_msg_scroller_dirty;
void run_inbound_msg_db_mgr(void);
#include <cdk_test.h>
#include <semaphore.h>
extern CDKSCREEN *cdkscreen;
int run_action_dialog(CDKSCREEN *cdkscreen,char *item);
extern int g_quit_flag;
extern int g_quit_flag;
void *outgoing_msg_ui_writer(void *arg);
void *inbound_msg_ui_writer(void *arg);
void draw_scrollers();
void init_msg_log_scrollers();
int run_gui(void);
extern int next_scroller;
extern pthread_mutex_t msgs_mutex;
extern CDKSCROLL *coresource_scroller;
extern CDKSCROLL *outgoing_msg_scroller;
extern CDKSCROLL *inbound_msg_scroller;
extern sem_t *outgoing_msg_semaphore;
extern sem_t *outgoing_msg_semaphore;
extern sem_t *inbound_msg_semaphore;
extern sem_t *inbound_msg_semaphore;
extern sem_t *quit_semaphore;
void exit_ui(void);
extern WINDOW *subWindow;
void gui_view_resources();
void discover_resources();
void gui_discover_resources();
extern CDKLABEL *msg_box;
extern SCREEN *screen;
extern SCREEN *screen;
#if defined(HAVE_XCURSES)
extern char *XCursesProgramName;
extern char *XCursesProgramName;
extern char *XCursesProgramName;
#endif
