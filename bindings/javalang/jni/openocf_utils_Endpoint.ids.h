#ifndef openocf_utils_Endpoint_ids_h
#define openocf_utils_Endpoint_ids_h

#include <jni.h>

#define FQCN_ENDPOINT "openocf/utils/Endpoint"
#define FQCS_ENDPOINT "Lopenocf/utils/Endpoint;"

extern jclass    K_ENDPOINT;
extern jmethodID MID_EP_CTOR;
extern jfieldID  FID_EP_HANDLE;

extern jfieldID  FID_EP_NETWORK_FLAGS;
extern jfieldID  FID_EP_NETWORK_POLICIES;
extern jfieldID  FID_EP_NETWORK_SCOPE;
extern jfieldID  FID_EP_TRANSPORT_SECURITY;
extern jfieldID  FID_EP_PORT;
extern jfieldID  FID_EP_ADDRESS;
extern jfieldID  FID_EP_IFINDEX;
extern jfieldID  FID_EP_ROUTE_DATA;

#endif
