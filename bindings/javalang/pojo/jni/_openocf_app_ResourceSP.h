/* This is part of the Service Programming Interface (SPI), which
   provides services to the lower-level OpenOCF stack. The service
   points in the SPI are only accessed from below; they are never
   accessed by application code. */

#ifndef _c_resource_sp_h
#define _c_resource_sp_h

#include "openocf.h"

OCEntityHandlerResult
_openocf_app_ResourceSP_react(OCEntityHandlerFlag watch_flag,
			      OCEntityHandlerRequest* c_OCEntityHandlerRequest,
			      void* j_ResourceSP);

#endif
