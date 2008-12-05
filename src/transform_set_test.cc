/**
   @file transform_set_test.cc

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

namespace shield
{

  namespace transform
  {

    data_type set_test::
    get_context ()
    {
      return _get_child(CHILD_IDENTITY) -> get_context ();
    }
    
    void set_test::
    _print (ostream &stream)
    {
      if ( !_get_child (CHILD_IDENTITY) 
	   || !_get_child (CHILD_SET))
	{
	  throw exception::invalid_state ("Required child node unset");
	}

      if (!get_skip_space ())
	stream << " ";

      stream << *_get_child(CHILD_IDENTITY);
      
      if (__not_in)
	stream << " not";

      stream << " in (";

      stream << *_get_child(CHILD_SET);

      stream << ")";
      
    }

    printable *set_test::
    internal_transform ()
    {
      catalyst::create_identity id_catalyst (get_query ());

      printable *res= this->transform (id_catalyst);

      data_type c = get_context ();
      if (c == DATA_TYPE_UNDEFINED)
	{
	  return this;
	}

      chain *in = _get_child<chain> (CHILD_SET);
      chain::const_iterator it;
      chain *out = new chain ();
      out->set_separator (",");
      for (it = in->begin (); it != in->end (); ++it)
	{
	  out->push (new cast (*it, c));
	}

      _set_child (CHILD_SET, out);

      return res;
    }

  }

}
