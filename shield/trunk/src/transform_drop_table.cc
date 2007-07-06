#include "transform.hh"
#include "introspection.hh"
#include "exception.hh"

namespace shield
{
  
  namespace transform
  {
    
    drop_table::drop_table( printable *name, bool if_exists)
      : __if_exists (if_exists)
    {
      _set_child (__NAME, name);
    }

    void drop_table::
    _print (ostream &stream)
    {
      printable *name = _get_child (__NAME);

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
  
      stream << "drop table" << *name << endl << endl;
    }

    

  }

}
