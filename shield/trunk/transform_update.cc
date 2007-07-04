#include "transform.hh"

namespace shield
{

  namespace transform
  {


    void update::
    print (ostream &stream)
    {
      assert (get_table_list ());
      assert (get_update_list ());
      stream << "update" << *get_table_list () << " set " << *get_update_list ();
      if (get_where_clause ())
	{
	  stream << "\nwhere" << *get_where_clause ();
	}
      if (get_order_clause ())
	stream << *get_order_clause ();
      
      if (get_delete_limit_clause ())
	stream << *get_delete_limit_clause ();
      
      stream << endl << endl;
    }

  }

}
