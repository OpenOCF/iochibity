/* This file was automatically generated.  Do not edit! */
#include <cdk_test.h>
#include <semaphore.h>
extern sem_t *quit_semaphore;
extern int coresource_current_item;
extern int coresource_current_item;
extern CDKSCROLL *coresource_scroller;
extern int outgoing_current_item;
extern int outgoing_current_item;
extern CDKSCROLL *outgoing_msg_scroller;
extern int incoming_current_item;
extern int incoming_current_item;
extern CDKSCROLL *incoming_msg_scroller;
extern CDKSCREEN *cdkscreen;
int run_action_dialog(CDKSCREEN *cdkscreen,char *item);
extern int g_quit_flag;
extern int g_quit_flag;
void *outgoing_msg_ui_writer(void *arg);
void *incoming_msg_ui_writer(void *arg);
void draw_scrollers();
void init_msg_log_scrollers();
int run_gui(void);
int run_gui(void);
extern WINDOW *subWindow;
extern WINDOW *subWindow;
extern pthread_mutex_t msgs_mutex;
void run_menu();
void initialize_menu();
void erase_menu();
void list_resource_uris();
void gui_view_resources();
void gui_view_resources();
void discover_resources();
void gui_discover_resources();
void gui_discover_resources();
extern CDKLABEL *msg_box;
extern SCREEN *screen;
extern SCREEN *screen;
extern SCREEN *screen;
#if defined(HAVE_XCURSES)
extern char *XCursesProgramName;
extern char *XCursesProgramName;
extern char *XCursesProgramName;
extern char *XCursesProgramName;
extern char *XCursesProgramName;
extern char *XCursesProgramName;
#endif
