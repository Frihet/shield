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
#include "include/util.hh"

namespace shield
{
  
  namespace transform
  {

    printable *default_value::
    internal_transform (void)
    {
      bool handled = false;
      string inner = _get_child (CHILD_INNER)->str ();

      printable *grand_parent = get_parent ()->get_parent ();
      field_spec *field = dynamic_cast<field_spec *> (grand_parent);
      
      /*
	We can't use the shield.to_date_ function in the default
	clause because Oracle doesn't let you use non-builtin
	functions in default values. Yet another one of those
	arbitrary limitations that Oracle loves so much.
      */
      if (!grand_parent || !field)
	throw exception::invalid_state ("invalid grandparent for default value");

      if (field->get_type ()->get_type () == DATA_TYPE_DATETIME)
	{
	  if (contains (inner.c_str (), "0", "'0'", "\"0\"", "'0000-00-00 00:00:00'", "\"0000-00-00 00:00:00\""))
	    {
	      _set_child (CHILD_INNER, new text ("to_date ('0001-01-01 00:00:00','yyyy-mm-dd hh24:mi:ss')"));
	    }
	  else
	    {
	      chain *param = new chain (_get_child (CHILD_INNER), new text ("'yyyy-mm-dd hh24:mi:ss'", LITERAL));
	      param->set_separator (",");
	      _set_child (CHILD_INNER, new function ( new text ("to_date"), DATA_TYPE_DATETIME, param, false));
	    }
	}
      else if (field->get_type ()->get_type () == DATA_TYPE_DATE)
	{
	  if (contains (inner.c_str (), "0", "'0'", "\"0\"", "'0000-00-00'", "\"0000-00-00\""))
	    {
	      _set_child (CHILD_INNER, new text ("to_date ('0001-01-01','yyyy-mm-dd')"));
	    }
	  else
	    {
	      chain *param = new chain (_get_child (CHILD_INNER), new text ("'yyyy-mm-dd'", LITERAL));
	      param->set_separator (",");
	      _set_child (CHILD_INNER, new function ( new text ("to_date"), DATA_TYPE_DATETIME, param, false));
	    }
	}
      
      /*
	This default cast should not be needed...
      */
      /*
      else
	{
	  _set_child (CHILD_INNER, new cast (_get_child (CHILD_INNER), field->get_type ()->get_type ()));
	}
      */
      return this;
    }

  }

}
