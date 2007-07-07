#include "transform.hh"
#include "catalyst.hh"

namespace shield
{

  namespace transform
  {


    void update::
    _print (ostream &stream)
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

    printable *update::
    internal_transform (void)
    {
      catalyst::create_identity ident (this);
      return this->transform (ident);
    }


    chain *update::
    _get_condensed_table_list (void)
    {
      catalyst::find_table cat (this);
      transform (cat);
      return cat.get_table_list ();
    }


  }

}
