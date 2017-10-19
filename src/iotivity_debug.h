//******************************************************************
//
// Copyright 2016 Microsoft
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//******************************************************************

/**
 * @file
 *
 * This file contains debug helpers.
 */

#ifndef IOTIVITY_DEBUG_H_
#define IOTIVITY_DEBUG_H_

#include <assert.h>

// Macro used to avoid the need for a local variable just for an assert. Using
// a local variable just for assert, instead of this macro, can cause compiler
// warnings on NDEBUG builds. Example: use OC_VERIFY(foo() == 0) instead of
// {int local = foo(); assert(local == 0);}
#if defined(NDEBUG)
#define OC_VERIFY(condition) ((void)(condition))
#else
#define OC_VERIFY(condition) assert(condition)
#endif

/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)
/* #pragma message VAR_NAME_VALUE(foo) */

#endif // #ifndef IOTIVITY_DEBUG_H_
