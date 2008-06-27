/**
   @file transform_like.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/


#include "include/transform.hh"
#include "include/exception.hh"
#include "include/introspection.hh"

namespace shield
{

  namespace transform
  {

    like::
    like (printable *filter, printable *value, bool negate)
      : __negate (negate)
    {
      _set_child(CHILD_FILTER, filter);
      _set_child(CHILD_VALUE, value);
    }

    void like::
    _print (ostream &stream)
    {
      data_type ctx = get_filter ()->get_context ();
      
      if (ctx & DATA_TYPE_CLOB)
	{
	  stream << " shield.like_(";
	  stream << *get_filter();
	  stream << ",";
	  stream << *get_value();
	  stream << ")=" << (__negate?"0":"1");
	}
      else 
	{
	  stream << *get_filter();
	  stream << (__negate?" not":"")<< " like ";
	  stream << *get_value();
	}
    }

  }

}
