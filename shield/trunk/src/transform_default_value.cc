/**
   @file transform_default_value.cc

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
      string inner;

      printable *def = _get_child (CHILD_INNER);
      text *txt;

      txt = dynamic_cast<text *> (def);

      if (txt)
	{
	  if (txt->get_type () == LITERAL)
	    {
	      inner = txt->unmangled_str ();
	    }
	  else if (txt->get_type () == EXACT)
	    {
	      inner = txt->str ();
	    }
	  else
	    {
	      return this;
	    }
	}
      else
	{
	  return this;
	}

      field_spec *field=0;
      printable *ancestor = get_parent ();
      while (true)
	{
	  if (!ancestor)
	    break;

	  field = dynamic_cast<field_spec *> (ancestor);
	  if (field)
	    break;

	  ancestor = ancestor->get_parent ();
	}
      
      /*
	We can't use the shield.to_date_ or shield.to_number_
	functions in the default clause because Oracle doesn't let you
	use non-builtin functions in default values. Yet another one
	of those arbitrary limitations that Oracle loves so much.
      */
      if (!field)
	throw exception::invalid_state ("Could not locate field_spec for default value");

      if (field->get_type ()->get_type () == DATA_TYPE_DATETIME)
	{
	  if (contains (inner.c_str (), "0", "0000-00-00 00:00:00", "0000-00-00 00:00:00", ""))
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
	  if (contains (inner.c_str (), "0", "0000-00-00", "0000-00-00"))
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
      else if (field->get_type ()->get_type () & (DATA_TYPE_NUMBER | DATA_TYPE_FLOAT) )
	{
	  if (contains (inner.c_str (), ""))
	    {
	      _set_child (CHILD_INNER, new text ("0"));
	    }
	  else
	    {
	      _set_child (CHILD_INNER, new function ( new text ("to_number"), DATA_TYPE_DATETIME, _get_child (CHILD_INNER), false));
	    }
	  
	}

      else if (field->get_type ()->get_type () & DATA_TYPE_CLOB )
	{
	  if (contains (inner.c_str (), ""))
	    {
	      _set_child (CHILD_INNER, new text ("to_clob (chr (1))"));
	    }
	  else
	    {
	      _set_child (CHILD_INNER, new function ( new text ("to_clob"), DATA_TYPE_CLOB, _get_child (CHILD_INNER), false));
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
