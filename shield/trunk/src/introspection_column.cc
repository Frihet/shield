/**
   @file introspection_column.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/introspection.hh"

namespace shield
{
  
  namespace introspection
  {
    
    column::
    column (string name, string type)
      : __name (name), __type (type)
    {
    }
    
    string column::
    get_name () const
    {
      return __name;
    }
    
    column_type column::
    get_type () const
    {
      return __type;
    }
    
  }

}
