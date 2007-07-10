
#include "transform.hh"
#include "catalyst.hh"
#include "exception.hh"
#include "introspection.hh"

namespace shield
{

  namespace transform
  {

    create_table::create_table()
      : __check (false)
    {
    }

    void 
    create_table::set_check_existance (bool check)
    {
      __check = check; 
    }

    void create_table::
    _print (ostream &stream)
    {
      if (!get_name () || (!get_field_list () && !get_like_clause ()))
	throw exception::invalid_state ("Required child nodes unset in create table node");
  
      if (__check)
	{
	  introspection::table &t = introspection::get_table (get_name ()->str ());
	  if (t.exists ())
	    return;
	}

      stream << "create table" << *get_name () << endl;
      
      if (get_like_clause ())
	{
	  stream << "like" << *get_like_clause ();
	}
      else
	{
	  stream << "(" << *get_field_list () << " )" ;
	}

      stream << endl << endl;

    }

  }

}
