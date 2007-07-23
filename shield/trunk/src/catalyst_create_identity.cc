/**

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

  namespace catalyst
  {

    transform::printable *create_identity::
    catalyze (transform::printable *p)
    {
      transform::text * t = dynamic_cast<transform::text *> (p);

      if (!t)
	{
	  return p;
	}

      if (t->get_type () != transform::IDENTIFIER &&
	  t->get_type () != transform::IDENTIFIER_QUOTED)
	{
	  return p;
	}
      transform::printable *parent = t->get_parent ();
      bool parent_is_id = dynamic_cast<transform::identity *> (parent) != 0;
      bool parent_is_item_wild = dynamic_cast<transform::select_item_wild *> (parent) != 0;
      
      if (parent_is_id || parent_is_item_wild)
	{
	  return p;
	}
      
      transform::printable_alias *parent_as_alias = dynamic_cast<transform::printable_alias *> (parent);
      
      if (parent_as_alias && p == parent_as_alias->get_alias ())
	return p;

      if (p->get_query () != __query)
	return p;

      transform::text *table = p->get_query ()->get_table (t);
      
      if (!table)
	{
	  //	  throw exception::invalid_state ("Could not find corresponding table for identity '" + p->get_path () + "'");
	  /*
	    This is probably an alias
	  */
	  return p;
	}

      transform::identity *res = new transform::identity (0, 0, t);
      
      return res;
    }

  }

}
