/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/exception.hh"

namespace shield
{

  namespace transform
  {

    void fake_query::
    _print (ostream &stream)
    {
      printable *inner = _get_child (CHILD_INNER);
      
      if (!inner)
	throw shield::exception::syntax ("Tried to print null fake_query node");
      
      stream << *inner;
    }

  }

}

