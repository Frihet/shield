/**
   @file transform_type.cc

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

    void type::_print (ostream &stream)
    {
      if (!get_skip_space ())
	stream << " ";
      switch (__type)
	{
	
	case DATA_TYPE_CLOB:
	  stream << "clob";
	  break;

	case DATA_TYPE_NUMBER:
	  stream << "number";
	  break;

	case DATA_TYPE_FLOAT:
	  stream << "float";
	  break;

	case DATA_TYPE_CHAR:
	  stream << "char";
	  break;

	case DATA_TYPE_VARCHAR:
	  stream << "varchar2";
	  break;

	case DATA_TYPE_DATE:
	  stream << "date";
	  break;

	case DATA_TYPE_DATETIME:
	  stream << "date";
	  break;

	default:
	  throw exception::invalid_state ("Tried to create unknown data type");
	}
      chain::_print (stream);
    }

  }

}
