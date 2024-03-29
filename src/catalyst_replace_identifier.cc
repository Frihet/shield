/**
   @file catalyst_replace_identifier.cc

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

    transform::printable *replace_identifier::
    catalyze (transform::printable *node)
    {
      string val;
      transform::printable *parent;

      transform::text *t = dynamic_cast<transform::text *> (node);
      if (!t)
	{
	  return node;
	}

      if (t->get_type () != transform::IDENTIFIER &&
	  t->get_type () != transform::IDENTIFIER_QUOTED)
	{
	  return node;
	}

      parent = t->get_parent ();

      /*
	This is not an alias, it is a part of an identifier
      */
      if (dynamic_cast<transform::identity *> (parent))
	{
	  return t;
	}

      val = t->str ();

      if (__mapping.find (val) == __mapping.end ())
	{
	  return node;
	}
      return __mapping[val];
    }
  }
}
