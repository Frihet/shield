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

namespace shield
{

  namespace transform
  {

    function::
    function (printable *name, data_type type, printable *param, bool aggregate)
      : __type (type), __aggregate (aggregate)
      {
	if (!name)
	  {
	    throw exception::invalid_param (get_node_name () +" constructor called with null name");
	  }
	_set_child (CHILD_NAME, name);
	if (param)
	  _set_child (CHILD_PARAM, param);
      }

    bool function::
    get_aggregate ()
    {
      return __aggregate;
    }

    data_type function::
    get_context ()
    {
      if (__type == DATA_TYPE_UNDEFINED)
	{
	  printable *node = _get_child (CHILD_PARAM);
	  if (!node)
	    {
	      throw exception::invalid_state ("Function called with undefined return type and no parameter list");
	    }
	  while(true)
	    {
	      chain *ch = dynamic_cast<chain *> (node);
	      if (!ch)
		break;
	      if (!ch->size ())
		{
		  return __type;
		}
	      node = (*ch)[0];
	    }
	  
	  if (node)
	    return node->get_context ();
	}

      return __type;
    }

    void function::
    _print (ostream &stream)
    {

      if (!get_skip_space ())
	stream << " ";
      
      stream << *get_name ();

      if (get_param ())
	{
	  stream << " (";
	  get_param ()->set_skip_space (true);
	  stream << *get_param () << ")";
	}

    }

  }

}
