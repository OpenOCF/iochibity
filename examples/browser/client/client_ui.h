/* This file was automatically generated.  Do not edit! */
void run_menu();
void *outbound_msg_ui_writer(void *arg);
void *inbound_msg_ui_writer(void *arg);
int main_ui(void);
#include <cdk_test.h>
#include <semaphore.h>
extern pthread_mutex_t msgs_mutex;
extern CDKSCREEN *cdkscreen;
extern CDKSCROLL *coresource_scroller;
extern CDKSCROLL *outbound_msg_scroller;
extern CDKSCROLL *inbound_msg_scroller;
extern sem_t *outbound_msg_semaphore;
extern sem_t *outbound_msg_semaphore;
extern sem_t *inbound_msg_semaphore;
extern sem_t *inbound_msg_semaphore;
extern sem_t *quit_semaphore;
void exit_ui(void);
extern WINDOW *subWindow;
void gui_view_resources();
void discover_resources();
void gui_discover_resources();
extern char *msg_box_msg[1];
extern CDKLABEL *msg_box;
extern SCREEN *screen;
extern int next_scroller;
#define CORESOURCES 2
#define OUTBOUND 1
#define INBOUND 0
#define MSG_MAX 40
#define INTERFACE 0
#if defined(HAVE_XCURSES)
extern char *XCursesProgramName;
extern char *XCursesProgramName;
#endif
