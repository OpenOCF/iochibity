#ifndef MSG_SCROLLERS_H
#define MSG_SCROLLERS_H 1

CDKSCROLL *incoming_msg_scroller;
extern int incoming_current_item;

CDKSCROLL *outgoing_msg_scroller;
extern int outgoing_current_item;

CDKSCROLL *coresource_scroller;
extern int coresource_current_item;

CDKLABEL *msg_box;
char     *msg_details[1];

void init_msg_log_scrollers();

void* incoming_msg_ui_writer(void *arg);

void* outgoing_msg_ui_writer(void *arg);

void draw_scrollers();

#endif
