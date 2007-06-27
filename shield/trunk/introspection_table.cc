
#include "introspection.hh"
#include "transform.hh"
#include "exception.hh"

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

      string query = "select column_name, type from user_col_data where table_name = " + oracle_escape (to_upper (__name));

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
