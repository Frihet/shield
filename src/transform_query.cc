/**
   @file transform_query.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/introspection.hh"
#include "include/exception.hh"

using namespace shield;

namespace shield
{

  namespace transform
  {

    query::
    query ()
    {
    }

    query *query::
    get_query ()
    {
      return this;
    }

    text *query::
    unalias_table (text *table)
    {
      return table;
    }

    text *query::
    get_table (text *field)
    {
      _make_condensed_table_list ();
      
      vector<printable *> &list = _condensed_table_list;      
      vector<printable *>::const_iterator it;

      for (it=list.begin (); it<list.end (); it++)
	{
	  
	  if (!*it)
	    {
	      throw exception::invalid_state ("Null item in _condensed_table_list");
	    }

	  text *t = dynamic_cast<text *> (*it);
	  identity *id = dynamic_cast<identity *> (*it);
	  
	  if (id)
	    {
	      t = id->get_table ();
	    }
	  
	  if (!t)
	    {
	      throw exception::invalid_state (string ("Item of unknown type in _condensed_table_list: ") + (*it)->get_node_name ());
	    }
	  
	  text *unaliased = unalias_table (t);
	  
	  if (!unaliased)
	    {
	      throw exception::invalid_state ( string ("Could not unalias item ") + t->str ());
	    }

	  introspection::table &table = introspection::get_table (unaliased->unmangled_str ());

	  try
	    {
	      table.get_column (field->unmangled_str ());

	      /*
		If that column didn't exist, an exception would have been thrown. We found it! Yay!
		
		Note that we use \c t here, not \c unaliased. 
	      */
	      
	      return new text(t->unmangled_str (), IDENTIFIER);
	    }
	  catch (exception::not_found &e)
	    {
	    }
	}

      return 0;
    }

    void query::
    _make_condensed_table_list ()
    {
      throw exception::syntax ("Called make_condensed_table_list () on query which does not supply table list");
    }

  }

}
