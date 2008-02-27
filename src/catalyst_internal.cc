/**
   @file catalyst_internal.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/


#include "include/transform.hh"
#include "include/exception.hh"
#include "include/catalyst.hh"

namespace shield
{

  namespace catalyst
  {
    
    transform::printable *internal::
    catalyze (transform::printable *node)
    {
      return node->internal_transform ();
    }

  }

}
