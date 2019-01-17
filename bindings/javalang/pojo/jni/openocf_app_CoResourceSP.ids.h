/* This is part of the Service Programming Interface (SPI), which
   provides services to the lower-level OpenOCF stack. The service
   points in the SPI are only accessed from below; they are never
   accessed by application code. */

#ifndef openocf_app_CoResourceSP_ids_h
#define openocf_app_CoResourceSP_ids_h

#include <jni.h>

extern jclass K_CORESOURCE_SP;
extern jmethodID MID_ICORSP_COREACT;
extern jfieldID  FID_CORSP_IS_RETAIN;
extern jfieldID  FID_CORSP_URI;
extern jfieldID  FID_CORSP_METHOD;

#endif
