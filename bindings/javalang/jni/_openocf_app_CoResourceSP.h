/* This is part of the Service Programming Interface (SPI), which
   provides services to the lower-level OpenOCF stack. The service
   points in the SPI are only accessed from below; they are never
   accessed by application code. */

#ifndef _c_resource_sp_h
#define _c_resource_sp_h

#include "openocf.h"

#include "jni_init.h"

OCStackApplicationResult
_openocf_app_CoResourceSP_coReact(void* c_CoRSP, /* context */
				  OCDoHandle c_TxnId,
				  OCClientResponse* c_OCClientResponse);

#endif
