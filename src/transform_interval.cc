/**
   @file transform_interval.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/exception.hh"

namespace shield
{

  namespace transform
  {
    
    string type_string (interval_type t)
    {
      switch (t)
	{
	case INTERVAL_DAY:
	  return "day";
      
	case INTERVAL_HOUR:
	  return "hour";
      
	case INTERVAL_SECOND:
	  return "second";
      
	case INTERVAL_YEAR:
	  return "year";
      
	case INTERVAL_MONTH:
	  return "month";
      
	case INTERVAL_MINUTE:
	  return "minute";

	default:
	  throw exception::syntax ("Interval of unknown type " + t);
	}
    }

    void interval::
    _print (ostream &stream)
    {
      printable *child = _get_child (CHILD_EXPR);
      text *child_text = dynamic_cast<text *> (child);
      if (child_text)
	{
	  if (child_text->get_type () == EXACT)
	    {
	      child = child_text = new text (string ("'")+child->str ()+"'", LITERAL);
	    }
	}
      stream << " interval" << *child << " " << type_string (__type);
    }

  }

}
