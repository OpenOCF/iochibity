/* This file was automatically generated.  Do not edit! */
extern int next_scroller;
void run_coresource_mgr(void);
void browse_coresource_json(OCClientResponse *msg);
#include <cdk_test.h>
#include <semaphore.h>
extern CDKSCREEN *cdkscreen;
void initialize_coresource_scroller(void);
extern CDKLABEL *msg_box;
int run_coresource_inspector(int index);
extern int coresource_current_item;
void update_coresource_scroller(void);
extern int inbound_current_item;
extern CDKSCROLL *coresource_scroller;
void reinitialize_coresource_scroller();
#define MSG_MAX 20
extern bool coresource_scroller_dirty;
