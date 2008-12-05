/**
   @file catalyst_create_identity.cc

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

      /*
	Various variables used when checking the type of parent
     
	If the parent is e.g. an identifier, we don't want to perform
	the conversion.
      */
      transform::printable *parent;
      transform::printable_alias *parent_as_alias;

      bool parent_is_id;
      bool parent_is_item_wild;

      if (!t)
	{
	  return p;
	}

      if (t->get_type () != transform::IDENTIFIER &&
	  t->get_type () != transform::IDENTIFIER_QUOTED)
	{
	  return p;
	}

      parent = t->get_parent ();
      parent_is_id = dynamic_cast<transform::identity *> (parent) != 0;
      parent_is_item_wild = dynamic_cast<transform::select_item_wild *> (parent) != 0;

      if (!parent)
	{
	  throw exception::invalid_state ("Parent-less text entry");
	}

      if (parent_is_id || parent_is_item_wild)
	{
	  return p;
	}
      
      parent_as_alias = dynamic_cast<transform::printable_alias *> (parent);
      
      if (parent_as_alias && p == parent_as_alias->get_alias ())
	return p;

      if (p->get_query () != __query)
	return p;

      transform::text *table = p->get_query ()->get_table (t);
      
      if (!table)
	{
	  /*
	    This is probably an alias. Do nothing.

	    FIXME: How hard would it be to locate all aliases and
	    check if this is one of them? That would be a very nice
	    validation - as it stands, this is one of the bigger holes
	    in the validation.
	  */
	  return p;
	}

      return new transform::identity (0, table, t);
    }

  }

}
