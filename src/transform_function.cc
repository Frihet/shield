/**
   @file transform_function.cc

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
	  {
	    _set_child (CHILD_PARAM, param);
	  }
      }

    bool function::
    get_aggregate ()
    {
      return __aggregate;
    }

    data_type function::
    get_context ()
    {
      return __type;
    }

    namespace 
    {
      data_type merge_types(data_type a, data_type b) {

	data_type order[] = 
	  {
	    DATA_TYPE_CLOB,
	    DATA_TYPE_VARCHAR,
	    DATA_TYPE_CHAR,
	    DATA_TYPE_NUMBER,
	    DATA_TYPE_FLOAT,
	    DATA_TYPE_DATETIME,
	    DATA_TYPE_DATE
	  }
	;

	for (size_t i=0; i<(sizeof(order)/sizeof(data_type)); i++)
	  {
	    if ((a==order[i]) || (b==order[i]))
	      {
		return order[i];
	      }
	  }
	
	return DATA_TYPE_UNDEFINED;
	
      }
    }

    printable *function::
    internal_transform (void)
    {
      return this;
    }

    void function::
    _print (ostream &stream)
    {

      if (__type == DATA_TYPE_UNDEFINED)
	{
	  printable *node = _get_child (CHILD_PARAM);
	  if (!node)
	    {
	      throw exception::invalid_state ("Function called with undefined return type and no parameter list");
	    }
	  chain *ch = dynamic_cast<chain *> (node);

	  if (!ch)
	    {
	      __type = node->get_context ();
	    }
	  else if (!ch->size ())
	    {
	      throw exception::invalid_state ("Function called with undefined return type and empty parameter list");
	    }
	  else 
	    {
	      data_type t = (*ch->begin())->get_context ();
	      
	      chain::iterator it;

	      for (it=ch->begin ()+1; it!=ch->end (); ++it)
		{
		  data_type t2 = (*it)->get_context ();
		  t = merge_types( t, t2);
		}
	      
	      __type = t;

	      for (it=ch->begin (); it!=ch->end (); ++it)
		{
	      
		  *it = new cast(*it, __type);
		  (*it)->set_parent(this);
		}
	      
	    }
	  
	}

      



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
