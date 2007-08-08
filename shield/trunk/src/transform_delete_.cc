/**
   @file transform_delete_.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/exception.hh"
#include "include/catalyst.hh"

namespace shield
{

  namespace transform
  {

    delete_::
    delete_ (identity *from, printable *where)
    {
      _set_child (CHILD_TABLE_LIST, from);
      _set_child (CHILD_WHERE_CLAUSE, where);
    }

    /*
      Reparent this node as a new query, but first find out a bit about the table, like what field we operate on
    */
    printable *delete_::
    internal_transform (void)
    {
      catalyst::create_identity id_catalyst (get_query ());
      printable *where = _get_child (CHILD_WHERE_CLAUSE);
      
      if (where)
	{
	  _set_child (CHILD_WHERE_CLAUSE, where->transform (id_catalyst));
	}
      
      return this;
    }
    
    /**
       Print 
    */
    void delete_::
    _print (ostream &stream)
    {
      identity *from = _get_child<identity> (CHILD_TABLE_LIST);
      printable *where = _get_child (CHILD_WHERE_CLAUSE);
      
      if (!from)
	{
	  throw exception::invalid_state ("No from cluase in delete query");
	}

      stream <<"delete from" << *from;
      
      if (where)
	{
	  stream << " where" << *where;
	}

      stream << "\n\n" << sep;

    }
    
    void delete_::
    _make_condensed_table_list (void)
    {
      if (_condensed_table_list.size ())
	return;

      identity *from = _get_child<identity> (CHILD_TABLE_LIST);
      
      if (!from)
	{
	  throw exception::invalid_state ("No from cluase in delete query");
	}

      _condensed_table_list.push_back (from);

      
    }

  }

}

