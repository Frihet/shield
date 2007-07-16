/**

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
      
      string q = "select a.column_name, a.column_type from shield_table_column a left join user_tab_columns b on upper(a.column_name)=b.column_name and upper(a.table_name)=b.table_name where a.table_name = % order by b.column_id";
      
      result_set &rs = query (q) << __name;

      while (rs.next ())
	{
	  //debug << ( rs.get_string ("column_name") + " "+ rs.get_string ("column_type"));
	  __col.push_back (column (rs.get_string ("column_name"), rs.get_string ("column_type")));
	}
      rs.close ();
    }
  
    const column &table::
    get_column (string name)
    {
      load_column ();
      column_const_iterator it;

      string name2 = to_lower (name);

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
      string q = "select table_name from user_tables where table_name = %";
      
      result_set &rs = query (q) << to_upper (__name);

      return rs.next ();
    }


  }

}
