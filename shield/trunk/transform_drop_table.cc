#include "transform.hh"
#include "introspection.hh"

namespace shield
{
  
  namespace transform
  {
    
    drop_table::drop_table( printable *name, bool if_exists)
      : __name (name), __if_exists (if_exists)
    {
      __name->set_parent (this);

      assert (__name->get_parent ());
    }

    void 
    drop_table::print (ostream &stream)
    {

      if (__if_exists)
	{
	  introspection::table &t = introspection::get_table (__name->str ());
	  if (!t.exists ())
	    {
	      return;
	    }
	}
  
      stream << "drop table" << *__name << endl << endl;
    }

    

  }

}
