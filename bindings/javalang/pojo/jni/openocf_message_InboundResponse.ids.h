#ifndef openocf_message_InboundResponse_h
#define openocf_message_InboundResponse_h

extern jclass    K_INBOUND_RESPONSE;
extern jmethodID MID_INRESP_CTOR;
extern jfieldID  FID_INRESP_HANDLE;
extern jfieldID  FID_INRESP_OBSERVATION_HANDLE;
/* extern jfieldID  FID_INRESP_REMOTE_DEVADDR; */
extern jmethodID MID_INRESP_GET_EP;
extern jfieldID  FID_INRESP_IS_RETAIN;

/* extern jfieldID  FID_INRESP_CONN_TYPE; */
/* extern jfieldID  FID_INRESP_REMOTE_SID; */
/* extern jfieldID  FID_INRESP_RESULT; */
/* extern jfieldID  FID_INRESP_SERIAL; */
/* extern jfieldID  FID_INRESP_URI; */
/* extern jfieldID  FID_INRESP_PAYLOAD; */
extern jmethodID MID_INRESP_GET_OPTIONS;
extern jmethodID MID_INRESP_GET_OBSERVATION;

#endif
