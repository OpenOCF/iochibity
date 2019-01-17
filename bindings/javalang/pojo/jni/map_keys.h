#ifndef MAP_KEYS_H
#define MAP_KEYS_H

#include <jni.h>

extern jobject J_TRUE;
extern jobject J_FALSE;

extern jobject KEY_ADDR;
extern jobject KEY_PORT;
extern jobject KEY_TRANSPORT;
extern jobject KEY_UDP;
extern jobject KEY_TCP;
extern jobject KEY_INDEX;
extern jobject KEY_SECURE;
extern jobject KEY_MCAST;
extern jobject KEY_DEVADDR;
extern jobject KEY_IPV4;
extern jobject KEY_IPV6;
extern jobject KEY_SCOPE;
extern jobject KEY_SCOPE_IF;
extern jobject KEY_SCOPE_LINK;
extern jobject KEY_SCOPE_REALM;
extern jobject KEY_SCOPE_ADMIN;
extern jobject KEY_SCOPE_SITE;
extern jobject KEY_SCOPE_ORG;
extern jobject KEY_SCOPE_GLOBAL;

extern jobject KEY_REMOTE_IDENTITY;
extern jobject KEY_REMOTE_RESULT;
extern jobject KEY_PAYLOAD_TYPE;
extern jobject KEY_PAYLOAD;
extern jobject KEY_COAP_OPTIONS;

extern jobject KEY_SERIAL;
extern jobject KEY_URI;

/* OCF standard property names */
extern jobject KEY_OCF_ADDR;
extern jobject KEY_OCF_PORT;
extern jobject KEY_OCF_DEVICE;		/* d */
extern jobject KEY_OCF_POLICY;		/* p */
extern jobject KEY_OCF_DI;		/* device id? */
extern jobject KEY_OCF_INSTANCE;	/* ins */
extern jobject KEY_OCF_RT;		/* resource types */
extern jobject KEY_OCF_IF;		/* interfaces */
extern jobject KEY_OCF_NAME;		/* n */
extern jobject KEY_OCF_ID;
extern jobject KEY_OCF_TITLE;
extern jobject KEY_OCF_MEDIA_TYPE;
extern jobject KEY_OCF_ANCHOR;
extern jobject KEY_OCF_POLICY_BITMASK;
extern jobject KEY_OCF_SEC;
extern jobject KEY_OCF_EP;
extern jobject KEY_OCF_EPS;
extern jobject KEY_OCF_PRIORITY; /* pri */
extern jobject KEY_OCF_LINK;
extern jobject KEY_OCF_LINKS;
extern jobject KEY_OCF_HREF;
extern jobject KEY_OCF_LINK_RELATION;
extern jobject KEY_OCF_TPS;
extern jobject KEY_TRANSPORT_FLAGS;
extern jobject KEY_OCF_BURI;
extern jobject KEY_OCF_PI;
extern jobject KEY_TCP_PORT;
extern jobject KEY_DISCOVERABLE;
extern jobject KEY_OBSERVABLE;
extern jobject KEY_CONN_TYPE;
extern jobject KEY_TRANSPORT_ADAPTER;


#endif
