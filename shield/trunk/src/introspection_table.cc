#include "introspection.hh"
#include "transform.hh"
#include "exception.hh"
#include "database.hh"
#include "util.hh"

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
      
      string q = "select column_name, data_type from user_tab_cols where table_name = % order by column_id";
      
      result_set &rs = query (q) << to_upper (__name);

      while (rs.next ())
	{
	  __col.push_back (column (to_lower (rs.get_string ("column_name")), to_lower (rs.get_string ("data_type"))));
	}
      rs.close ();
    }
  
    const column &table::
    get_column (string name)
    {
      load_column ();
      column_const_iterator i;

      string name2 = to_lower (name);

      for (i=column_begin (); i<column_end (); i++)
	{
	  const column &c = *i;
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
