/**

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

namespace shield
{
  
  namespace transform
  {
    
    drop_table::drop_table( printable *name, bool if_exists)
      : __if_exists (if_exists)
    {
      _set_child (CHILD_NAME, name);
    }

    void drop_table::
    _print (ostream &stream)
    {
      printable *name = _get_child (CHILD_NAME);

      if (!name)
	{
	  throw shield::exception::syntax ("Can not drop unnamed table");
	}

      if (__if_exists)
	{
	  introspection::table &t = introspection::get_table (name->str ());
	  if (!t.exists ())
	    {
	      return;
	    }
	}
  
      stream << "drop table" << *name << endl << endl << sep;
      stream << "delete from shield_table_column where table_name = " << util::oracle_escape (name->str ()) << endl << endl << sep;
    }

    

  }

}
