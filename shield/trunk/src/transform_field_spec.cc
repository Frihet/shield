/**
   @file transform_field_spec.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/exception.hh"
#include "include/util.hh"

namespace shield
{

  namespace transform
  {

      field_spec::
      field_spec (text *name, type *type, chain *attribute_in)
	: __nullable (true)
      {
	chain *attribute_out = new chain ();
	chain::const_iterator it;

	_set_child (CHILD_NAME, name);
	_set_child (CHILD_TYPE, type);

	if (attribute_in)
	  {
	for (it = attribute_in->begin (); it != attribute_in->end (); ++it)
	  {
	    printable *node = *it;
	    nullable *node_nullable = dynamic_cast<nullable *> (node);
	    default_value *node_default = dynamic_cast<default_value *> (node);

	    if (node_nullable)
	      {
		__nullable = node_nullable->is_nullable ();
	      }
	    else if (node_default)
	      {
		_set_child (CHILD_DEFAULT, node_default);
	      }
	    else
	      {
		attribute_out->push (node);
	      }
	    
	  }
	  }
	_set_child (CHILD_ATTRIBUTE, attribute_out);

	if (!__nullable && !get_default ())
	  {
	    printable *inner;
	    switch (get_type ()->get_type ())
	      {
	      case DATA_TYPE_NUMBER:
	      case DATA_TYPE_DATE:
	      case DATA_TYPE_DATETIME:
		inner = new text ("'0'", LITERAL);
		break;

	      case DATA_TYPE_FLOAT:
		inner = new text ("'0.0'", LITERAL);
		break;

	      case DATA_TYPE_CHAR:
	      case DATA_TYPE_VARCHAR:
	      case DATA_TYPE_CLOB:
		inner = new text ("''", LITERAL);
		break;
	      }

	    set_default (new default_value (inner));
	  }
	

      }

    void field_spec::
    _print (ostream &stream)
    {
      stream << "\t" << *get_name () << *get_type ();
      if (get_default ())
	stream << *get_default ();

      stream << (__nullable ? " null" : " not null");
      if (get_attribute ())
	stream << *get_attribute ();
    }

    
  }

}
