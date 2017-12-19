/* This file was automatically generated.  Do not edit! */
extern bool outbound_msg_scroller_dirty;
int run_outbound_msg_db_mgr(void);
extern bool inbound_msg_scroller_dirty;
int run_inbound_msg_db_mgr(void);
extern int next_scroller;
#include <cdk_test.h>
#include <semaphore.h>
extern CDKSCREEN *cdkscreen;
extern char *msg_box_msg[1];
void reinitialize_outbound_msg_scroller();
void reinitialize_inbound_msg_scroller();
void run_msg_logs(void);
extern CDKLABEL *msg_box;
void destroy_msg_scrollers();
extern CDKSCROLL *inbound_msg_scroller;
extern CDKSCROLL *outbound_msg_scroller;
void draw_msg_scrollers();
void reset_scroller(CDKSCROLL *scroller);
#define CORESOURCES 2
#define OUTBOUND 1
#define INBOUND 0
#define INTERFACE 0
