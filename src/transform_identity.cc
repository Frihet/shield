/**
   @file transform_identity.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/introspection.hh"
#include "include/exception.hh"
#include "include/util.hh"

using namespace shield;

namespace shield
{

  namespace transform
  {

    string identity::
    unmangled_str ()
    {
      string res;
      bool printed = false;

      if (get_namespace () && get_namespace ()->length ())
	{
	  res += get_namespace ()->unmangled_str ();
	  printed = true;
	}

      if (get_table () && get_table ()->length ())
	{
	  if (printed)
	    {	
	      res += ".";
	    }
	  printed = true;
	  res += get_table ()->unmangled_str ();
	}
    
      if (get_field () && get_field ()->length ())
	{
	  if (printed)
	    {
	      res += ".";
	    }
	  res += get_field ()->unmangled_str ();
	}

      return res;
    }
	

    void identity::
    _print (ostream &stream)
    {
      bool printed = false;

      if (get_skip_space ())
	{
	  if (get_namespace () && get_namespace ()->length ())
	    get_namespace ()->set_skip_space (true);
	  else if (get_table () && get_table ()->length ())
	    get_table ()->set_skip_space (true);
	  if (get_field () && get_field ()->length ())
	    get_field ()->set_skip_space (true);
	}

      if (get_namespace () && get_namespace ()->length ())
	{
	  stream << *get_namespace ();
	  printed = true;
	}

      if (get_table () && get_table ()->length ())
	{
	  if (printed)
	    {
	      stream << ".";
	      get_table ()->set_skip_space (true);
	    }
	  printed = true;
	  stream << *get_table ();	
	}
    
      if (get_field () && get_field ()->length ())
	{
	  if (printed)
	    {
	      stream << ".";
	      get_field ()->set_skip_space (true);
	    }

	  stream << *get_field ();	
	}

    }

    text *identity::get_namespace ()
    {
      return _get_child<text> (CHILD_NAMESPACE);
    }

    text *identity::get_table ()
    {
      return _get_child<text> (CHILD_TABLE);
    }

    text *identity::get_field ()
    {
      return _get_child<text> (CHILD_FIELD);
    }

    data_type identity::
    get_context ()
    {
      /*
	We cache the actual lookup, since the get_context function is
	called roughly 1.5 times on average, and the operation is
	reasonably expensive.
      */

      if (printable::get_context () == DATA_TYPE_UNDEFINED)
	{	  
	  __calculate_context ();
	}

      //cerr << "Ok, returning: " << printable::get_context () << endl;      
      return printable::get_context ();
    }

    void identity::
    __calculate_context ()
    {
      query *q = get_query ();
      text *table;
      text *unaliased_table;

      //cerr << "Calculate context type for identity item " << get_field() << endl;
      
      //      debug << (string ("Query is of type ") + q->get_node_name ());

      if (!q)
	{
	  throw exception::invalid_state ("No parent query in identity node " + get_path ());
	}
      table = get_table ();

      if (!table)
	{
	  table = q->get_table (get_field ());
	}

      if (!table)
	throw exception::invalid_state ("Could not introspect table in identity::get_context ()");

      unaliased_table = q->unalias_table (table);

      introspection::table &itbl = introspection::get_table (unaliased_table->unmangled_str ());

      const introspection::column &ic = itbl.get_column (get_field ()->unmangled_str ());
      const introspection::column_type &tp = ic.get_type ();

      //cerr << "Result: " << tp.get_type() << endl;      

      set_context (tp.get_type ());
      
    }

    

  }

}
