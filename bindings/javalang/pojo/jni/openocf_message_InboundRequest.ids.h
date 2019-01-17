#ifndef openocf_message_InboundRequest_ids_h
#define openocf_message_InboundRequest_ids_h

#include <jni.h>

extern jclass    K_INBOUND_REQUEST;
extern jmethodID MID_INBOUND_REQUEST_CTOR;
extern jfieldID  FID_INBOUND_REQUEST_HANDLE;
extern jmethodID  MID_INBOUND_REQUEST_IS_WATCH;
extern jfieldID  FID_INBOUND_REQUEST_METHOD;
extern jmethodID MID_INBOUND_REQUEST_GET_METHOD;
extern jfieldID  FID_INBOUND_REQUEST_QUERY;
extern jmethodID MID_INBOUND_REQUEST_GET_QUERY;
extern jfieldID  FID_INBOUND_REQUEST_MSG_ID;
extern jmethodID MID_INBOUND_REQUEST_GET_MSG_ID;

#endif
