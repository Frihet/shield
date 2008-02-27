/**
   @file occi_wrapper.hh

   Wrapper around occi.h to drop warnings.

*/
#ifndef OCCI_WRAPPER_HH
#define OCCI_WRAPPER_HH

/*
  Include occi.h with warnings disabled. Uglyyyyyy!
*/

#if defined __GNUC__
#pragma GCC system_header
#elif defined __SUNPRO_CC
#pragma disable_warn
#elif defined _MSC_VER
#pragma warning(push, 1)
#endif 

#include <occi.h>

#if defined __SUNPRO_CC
#pragma enable_warn
#elif defined _MSC_VER
#pragma warning(pop)
#endif 

#endif //#ifndef OCCI_WRAPPER_HH

