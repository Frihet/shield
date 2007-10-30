/**
   @file transform_drop_table.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/introspection.hh"
#include "include/exception.hh"
#include "include/util.hh"
#include "include/cache.hh"

namespace shield
{
  
  namespace transform
  {
    
    drop_table::drop_table( identity *name, bool if_exists)
      : __if_exists (if_exists)
    {
      _set_child (CHILD_NAME, name);
    }

    void drop_table::
    _print (ostream &stream)
    {
      identity *name = _get_child<identity> (CHILD_NAME);

      if (!name)
	{
	  throw shield::exception::syntax ("Can not drop unnamed table");
	}

      if (__if_exists)
	{
	  introspection::table &t = introspection::get_table (name->unmangled_str ());
	  if (!t.exists ())
	    {
	      return;
	    }
	}

      stream << "drop table" << *name << endl << endl << sep;
      stream << "delete from shield_table_column where lower (table_name) = lower (" << util::oracle_escape (name->str ()).first << ")" << endl << endl << sep;

      introspection::clear_table (name->unmangled_str ());
      cache::clear ();
    }
    
  }

}
