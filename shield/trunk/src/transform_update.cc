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
      printable *res;
      catalyst::create_identity ident (this);
      res = this->transform (ident);
      
      __cast_update_list ();

      return res;
    }

    
    void update::
    _make_condensed_table_list (void)
    {
      _condensed_table_list.clear ();
      catalyst::find_table cat (this);      
      transform (cat);
      _condensed_table_list = cat.get_table_list ();
    }

    void update::
    __cast_update_list ()
    {
      chain::const_iterator it;
      chain *update_list = get_update_list ();

      cerr << "LALALA" << endl;

      if (!update_list)
	{
	  throw exception::invalid_state ("Update list missing");
	}

      for (it=update_list->begin (); it != update_list->end (); ++it)
	{
	  chain *elem = dynamic_cast<chain *> (*it);

	  if (!elem || elem->size () != 3)
	    {
	      throw exception::invalid_state ("Update element was not of type chain or of wrong size");
	    }

	  identity *id = dynamic_cast<identity *> ((*elem)[0]);
	  if (!id)
	    {
	      throw exception::invalid_state ("Update element id was not of type identity");	      
	    }

	  cerr << "LALALA2" << endl;

	  elem->set(2, new cast ((*elem)[2], id->get_context ()));
	  
	}
    }

  }

}
