/**
   @file introspection_table.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/introspection.hh"
#include "include/transform.hh"
#include "include/exception.hh"
#include "include/database.hh"
#include "include/util.hh"

namespace shield
{

  namespace introspection
  {

    namespace
    {
      const string INDEX_COLUMN_TYPE = "DATA_TYPE_INDEX";
    }

    using namespace shield::database;
    using namespace util;

    table::
    table (string name)
      : __name(name), __is_col_init (false)
    {
    }
  
    void table::
    load_column ()
    {
      
      if (__is_col_init)
	return;
      
      __is_col_init = true;
      
      string q = "";
      
      q += "select a.column_name, a.column_type \n";
      q += "from shield_table_column a \n";
      q += "left join user_tab_columns b\n";
      q += "on upper (a.mangled_column_name )= upper (b.column_name) and upper(a.mangled_table_name) = upper (b.table_name)\n";
      q += "where lower (a.table_name) = lower (%)\n order by b.column_id";
      
      result_set &rs = query (q) << identifier_unescape (__name);
      
      while (rs.next ())
	{
	  string col_name = rs.get_string ("column_name");
	  string col_type = rs.get_string ("column_type");
	  if (col_type == INDEX_COLUMN_TYPE)
	    {
	      /*
		We don't need to handle introspection of indices
		yet. Once we get moving with funky alter table magic,
		this will change. Shield already stores some index
		data though, in the hope that this will make upgrades
		easier in the future.
	      */
	      //__index.push_back (index (col_name, col_type));
	    }
	  else
	    {
	      __col.push_back (column (col_name, col_type));
	    }
	}
      rs.close ();
    }
  
    const column &table::
    get_column (string name)
    {
      load_column ();
      column_const_iterator it;

      string name2 = name;

      for (it=column_begin (); it != column_end (); ++it)
	{
	  const column &c = *it;
	  if (c.get_name () == name2)
	    {
	      return c;
	    }
	}

      throw exception::not_found (name);
    }

    introspection::table::column_const_iterator table::
    column_begin ()
    {
      load_column ();
      return __col.begin ();
    }
  
    introspection::table::column_const_iterator table::
    column_end ()
    {
      load_column ();
      return __col.end ();
    }
    
    bool table::
    exists ()
    {
      string q = "select table_name from shield_table_column where lower (table_name) = lower (%)";
      
      result_set &rs = query (q) << __name;
      
      return rs.next ();
    }

  }

}
