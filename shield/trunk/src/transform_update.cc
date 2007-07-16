/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/catalyst.hh"
#include "include/exception.hh"
#include "include/enum_char.hh"

namespace shield
{

  namespace transform
  {

    void update::
    _print (ostream &stream)
    {
      
      if (!get_table_list () || !get_update_list ())
	{
	  throw exception::invalid_state (get_node_name () + ": name or value list not set when calling _print ()");
	}
      
      stream << "update" << *get_table_list () << " set " << *get_update_list ();
      if (get_where_clause ())
	{
	  stream << "\nwhere" << *get_where_clause ();
	}
      if (get_order_clause ())
	stream << *get_order_clause ();
      
      if (get_delete_limit_clause ())
	stream << *get_delete_limit_clause ();
      
      stream << endl << endl << sep;
    }

    printable *update::
    internal_transform (void)
    {
      catalyst::create_identity ident (this);
      return this->transform (ident);
    }

    
    void update::
    _make_condensed_table_list (void)
    {
      _condensed_table_list.clear ();
      catalyst::find_table cat (this);      
      transform (cat);
      _condensed_table_list = cat.get_table_list ();
    }


  }

}
