
#include "introspection.hh"
#include "transform.hh"
#include "exception.hh"
#include "database.hh"

using namespace shield::database;

namespace shield
{

  namespace introspection
  {

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
      
      string q = "select column_name, data_type from user_tab_cols where table_name = %";
      
      result_set &rs = query (q) << to_upper (__name);
      
      while (rs.next ())
	{
	  __col.push_back (column (rs.get_string ("column_name"), rs.get_string ("data_type")));
	}
      rs.close ();
    }
  
    const column &table::
    get_column (string name)
    {
      load_column ();
      column_const_iterator i;

      for (i=column_begin (); i<column_end (); i++)
	{
	  const column &c = *i;
	  if (c.get_name () == name)
	    {
	      return c;
	    }
	}

      throw exception::not_found_exception (name);
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


  }

}
