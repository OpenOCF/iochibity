/* This file was automatically generated.  Do not edit! */
extern int next_scroller;
extern char *msg_box_msg[1];
void run_coresource_mgr(void);
void reinitialize_coresource_scroller();
void initialize_coresource_scroller(void);
#include <cdk_test.h>
#include <semaphore.h>
extern CDKSCREEN *cdkscreen;
void browse_coresource_json(OCClientResponse *msg);
extern CDKLABEL *msg_box;
int run_coresource_inspector(int index);
extern CDKLABEL *infoBox;
void update_coresource_scroller(void);
extern int coresource_current_item;
extern CDKSCROLL *coresource_scroller;
#define MSG_MAX 40
extern bool coresource_scroller_dirty;
#define CORESOURCES 2
#define INBOUND 0
